#include "68040.hpp"
#include "SDL3/SDL_timer.h"
#include "bus.hpp"
#include "inline.hpp"
#include "memory.hpp"
#include <asio.hpp>
#include <deque>
#include <expected>
#include <format>
#include <mutex>
#include <print>
#include <signal.h>
#include <thread>
#include <unordered_set>
void WriteBImpl(uint32_t addr, uint8_t b);
void run_cpu();
using asio::ip::tcp;
static std::string nextPacket(tcp::socket *sock) {
    std::array<char, 1> c = {'\0'};
    int i = sock->read_some(asio::buffer(c));
    if(i == 0) {
        return "";
    }
    if(c[0] != '$') {
        // special letter
        return std::string{c[0]};
    }
    std::string p;
    for(;;) {
        std::array<char, 512> cc;
        int i = sock->read_some(asio::buffer(cc));
        if(i <= 0) {
            return "";
        }
        p += std::string(&cc[0], &cc[i]);
        if(i < 512) {
            break;
        }
    }
    return p.substr(0, p.size() - 3);
}
static std::string lastTransmission = "";
static std::mutex packetLock;
static void sendPacket(tcp::socket *sock, const std::string &str) {
    std::lock_guard lk{packetLock};
    uint8_t cc = 0;
    for(char c : str) {
        cc += (uint8_t)c;
    }
    auto re = std::format("${}#{:02x}", str, cc);
    lastTransmission = str;
    std::print("DEBUG:RES={}\n", re);
    sock->write_some(asio::buffer(re));
}

std::string dumpReg() {
    std::string s;
    s.reserve(1024);
    for(int i = 0; i < 8; ++i) {
        s += std::format("{:08x}", cpu.D[i]);
    }
    for(int i = 0; i < 8; ++i) {
        s += std::format("{:08x}", cpu.A[i]);
    }
    s += std::format("{:08x}{:08x}", GetSR(cpu), cpu.PC);
    if( cpu.fpu) {
        s += cpu.fpu->dumpReg();
    }
    return s;
}
int get_signum(EXCEPTION_NUMBER con) {
    switch(con) {
    case EXCEPTION_NUMBER::AFAULT:
    case EXCEPTION_NUMBER::ADDR_ERR:
        return 10;
    case EXCEPTION_NUMBER::ILLEGAL_OP:
    case EXCEPTION_NUMBER::ALINE:
    case EXCEPTION_NUMBER::FLINE:
        return 4;
    case EXCEPTION_NUMBER::PRIV_ERR:
        return 11;
    case EXCEPTION_NUMBER::DIV0:
    case EXCEPTION_NUMBER::FP_UNORDER:
    case EXCEPTION_NUMBER::FP_INEX:
    case EXCEPTION_NUMBER::FP_DIV0:
    case EXCEPTION_NUMBER::FP_UNFL:
    case EXCEPTION_NUMBER::FP_OPERR:
    case EXCEPTION_NUMBER::FP_OVFL:
    case EXCEPTION_NUMBER::FP_SNAN:
    case EXCEPTION_NUMBER::FP_UNIMPL_TYPE:
    case EXCEPTION_NUMBER::CHK_FAIL:
    case EXCEPTION_NUMBER::TRAPx:
    case EXCEPTION_NUMBER::TRACE:
        return 8;
    case EXCEPTION_NUMBER::IRQ_LV7:
        return 2;
    case EXCEPTION_NUMBER::TRAP1:
        return 5;
    case EXCEPTION_NUMBER::TRAP8:
        return 8;
    default:
        return 7;
    }
}
static std::atomic<bool> cpu_interrupted{false};
void run_op();
void stop_cpu(tcp::socket *sock, const std::string &stop) {
    sendPacket(sock, stop);
    cpu.run.stop();
}
void gdb_q(tcp::socket *sock, std::string_view s) {
    if(s.starts_with("Supported")) {
        sendPacket(sock, "swbreak+;hwbreak+");
    } else if(s == "Attached") {
        sendPacket(sock, "1");
    } else if(s == "C") {
        sendPacket(sock, "0");
    } else if(s == "fThreadInfo") {
        sendPacket(sock, "m0");
    } else if(s == "sThreadInfo") {
        sendPacket(sock, "l");
    } else {
        sendPacket(sock, "");
    }
}
static std::unordered_set<uint32_t> bks;
extern std::unordered_set<uint32_t> rom_stops;
static std::atomic<bool> isNextStop;
static std::atomic<bool> dbRun;
void run_cpu_db(tcp::socket *sock) {
    while(dbRun.load()) {
        cpu.run.wait();
        if(rom_stops.contains(cpu.PC)) {
            cpu.run.stop();
            continue;
        }
        if(bks.contains(cpu.PC)) {
            stop_cpu(sock, "T05hwbreak:;");
            continue;
        }
        try {
            //            std::print("{:06x}\n", cpu.PC);
            run_op();
            if(isNextStop) {
                isNextStop = false;
                stop_cpu(sock, "S00");
            }
        } catch(M68kException &e) {
            handle_exception(e.ex_n);
            cpu.bus_lock = false;
            cpu.PC = cpu.oldpc;
            stop_cpu(sock, std::format("S{:02x}", get_signum(e.ex_n)));
        }
    }
}

bool gdb_cmd(tcp::socket *sock, std::string_view c) {
    if(c == "vCont?") {
        sendPacket(sock, "vCont;vs");
        return false;
    } else if(c.starts_with("vKill")) {
        sendPacket(sock, "OK");
        cpu.PC = 0x2A;
        return false;
    }
    char cmd = c[0];
    std::string_view arg = c.substr(1);
    switch(cmd) {
    case '!':
        sendPacket(sock, "OK");
        break;
    case 'H':
        sendPacket(sock, "OK");
        break;
    case 'k':
        return true;
    case 'R':
        cpu.A[7] = ReadL(cpu.VBR);
        cpu.PC = ReadL(cpu.VBR + 4);
        break;
    case 'q':
        gdb_q(sock, arg);
        break;
    case 'c':
        if(!arg.empty()) {
            std::from_chars(arg.begin(), arg.end(), cpu.PC, 16);
        }
        cpu.run.resume();
        break;
    case 'C': {
        int sig;
        auto [nx, _] = std::from_chars(arg.begin(), arg.end(), sig, 16);
        if(*nx == ';') {
            std::from_chars(nx + 1, arg.end(), cpu.PC, 16);
        }
        handle_exception(EXCEPTION_NUMBER(sig));
        cpu.run.resume();
        break;
    }

    case 'D':
        sendPacket(sock, "OK");
        return true;
    case 'm': {
        int addr, length;
        auto [next, _] = std::from_chars(arg.begin(), arg.end(), addr, 16);
        std::from_chars(next + 1, arg.end(), length, 16);
        std::string ret;
        ret.reserve(length * 2);
        for(int i = addr; i < addr + length; ++i) {
            try {
                uint8_t v = ReadBImpl(i, false);
                ret += std::format("{:02x}", v);
            } catch(BusError &) {
                ret += "00";
            }
        }
        sendPacket(sock, ret);
        break;
    }
    case 'M': {
        int addr, length;
        auto [next, _] = std::from_chars(arg.begin(), arg.end(), addr, 16);
        auto [next2, _2] = std::from_chars(next, arg.end(), length, 16);
        next2++;
        for(int i = addr; i < addr + length; ++i) {
            char bb[3] = {next2[0], next2[1], 0};
            next2 += 2;
            uint8_t v = strtoul(bb, nullptr, 16);
            try {
                WriteBImpl(i, v);
            } catch(BusError &) {
            }
        }
        sendPacket(sock, "OK");
        break;
    }
    case 'X': {
        char *next;
        int addr = strtoul(&c[1], &next, 16);
        int length = strtoul(next + 1, &next, 16);
        next++;
        for(int i = addr; i < addr + length; ++i) {
            WriteBImpl(i, *next++);
        }
        sendPacket(sock, "OK");
        break;
    }
    case 's':
        isNextStop = true;
        cpu.run.resume();
        break;
    case 'g':
        sendPacket(sock, dumpReg());
        break;
    case '?':
        sendPacket(sock, "S00");
        break;
    case 'Z':
        switch(c[1]) {
        case '0':
        case '1': {
            char *nx;
            uint32_t addr = strtoul(&c[3], &nx, 16);
            bks.insert(addr);
            sendPacket(sock, "OK");
            break;
        }
        default:
            sendPacket(sock, "");
        }
        break;
    case 'z':
        switch(c[1]) {
        case '0':
        case '1': {
            char *nx;
            uint32_t addr = strtoul(&c[3], &nx, 16);
            bks.erase(addr);
            sendPacket(sock, "OK");
            break;
        }
        default:
            sendPacket(sock, "");
        }
        break;
    default:
        sendPacket(sock, "");
        break;
    }
    return false;
}
void debug_activate() {
    asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 5555));

    tcp::socket sock(io_context);
    acceptor.accept(sock);
    dbRun.store(true);
    cpu.run.stop();
    std::thread th([&sock]() { run_cpu_db(&sock); });
    for(std::string s = nextPacket(&sock); !s.empty(); s = nextPacket(&sock)) {
        if(s == "-") {
            // retry
            sendPacket(&sock, lastTransmission);
            continue;
        }
        if(s == "+") {
            // ack
            continue;
        }
        sock.write_some(asio::buffer("+"));
        std::print("DEBUG:REQ={}\t", s);
        if(s[0] == 0x03) {
            // Inturrput
            sendPacket(&sock, std::format("S{:02x}", SIGINT));
            cpu.run.stop();
            continue;
        }
        if(gdb_cmd(&sock, s.c_str())) {
            dbRun.store(false);
            th.join();
            return;
        }
    }
}

void trace() {
    while(cpu.PC != 0x40846FEE) {
        try {
            std::print(stderr, "{:08x}\n", cpu.PC);
            run_op();
        } catch(M68kException &e) {
            handle_exception(e.ex_n);
            cpu.bus_lock = false;
        }
    }
}
