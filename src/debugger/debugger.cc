#include "68040.hpp"
#include "bus.hpp"
#include "inline.hpp"
#include "memory.hpp"
#include <asio.hpp>
#include <deque>
#include <expected>
#include <format>
#include <mutex>
#include <signal.h>
#include <thread>
#include <unordered_set>
std::expected<void, uint16_t> WriteBImpl(uint32_t addr, uint8_t b);
void run_cpu();
using asio::ip::tcp;
static std::string nextPacket(tcp::socket &sock) {
    std::array<char, 2> c = {'\0', '\0'};
    int i = sock.read_some(asio::buffer(c));
    if(i == 0) {
        return "";
    }
    if(c[0] != '$') {
        // special letter
        return &c[0];
    }
    std::string p;
    for(;;) {
        std::array<char, 512> cc;
        int i = sock.read_some(asio::buffer(cc));
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
static void sendPacket(tcp::socket &sock, const std::string &str) {
    std::lock_guard lk{packetLock};
    uint8_t cc = 0;
    for(char c : str) {
        cc += (uint8_t)c;
    }
    auto re = std::format("${}#{:02x}", str, cc);
    lastTransmission = str;
    //    std::print("DEBUG:RES={}\n", re);
    sock.write_some(asio::buffer(re));
}
std::tuple<uint64_t, uint16_t> store_fpX(int i);
uint32_t Get_FPSR();
uint16_t Get_FPCR();
std::string dumpReg() {
    std::string s;
    s.reserve(1024);
    for(int i = 0; i < 8; ++i) {
        s += std::format("{:08x}", cpu.D[i]);
    }
    for(int i = 0; i < 8; ++i) {
        s += std::format("{:08x}", cpu.A[i]);
    }
    s += std::format("{:08x}{:08x}", GetSR(), cpu.PC);
    for(int i = 0; i < 8; ++i) {
        auto [f, e] = store_fpX(i);
        s += std::format("{:04x}0000{:016x}", e, f);
    }
    s += std::format("{:08x}", Get_FPCR());
    s += std::format("{:08x}", Get_FPSR());
    s += std::format("{:08x}", cpu.FPIAR);
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
static std::string lastStopped;
extern std::unordered_set<uint32_t> bks;
static std::unique_ptr<tcp::socket> sock;
void run_op();
void stop_cpu(const std::string &stop) {
    sendPacket(*sock, lastStopped = stop);
    cpu.sleeping.store(true);
}
void gdb_q(tcp::socket &sock, const std::string &s) {
    if(s.starts_with("Supported")) {
        sendPacket(sock, "swbreak+;hwbreak+");
    } else {
        sendPacket(sock, "");
    }
}
bool gdb_cmd(tcp::socket &sock, const char *c) {
    switch(*c) {
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
        gdb_q(sock, c + 1);
        break;
    case 'c':
        if(c[1] != '\0') {
            cpu.PC = strtoul(c + 1, nullptr, 16);
        }
        cpu.sleeping.store(false);
        cpu.sleeping.notify_one();
        break;
    case 'C': {
        char *nx;
        int sig = strtoul(c + 1, &nx, 16);
        if(*nx == ';') {
            cpu.PC = strtoul(nx + 1, nullptr, 16);
        }
        if(sig == get_signum(ex_n)) {
            handle_exception(ex_n);
        } else {
            // TODO: convert to another exception?
        }
        cpu.sleeping.store(false);
        cpu.sleeping.notify_one();
        break;
    }

    case 'D':
        sendPacket(sock, "OK");
        return true;
    case 'm': {
        char *next;
        int addr = strtoul(c + 1, &next, 16);
        int length = strtoul(next + 1, nullptr, 16);
        std::string ret;
        ret.reserve(length * 2);
        for(int i = addr; i < addr + length; ++i) {
            if(auto p = ReadBImpl(i, false)) {
                ret += std::format("{:02x}", p);
            } else {
                ret += "00";
            }
        }
        sendPacket(sock, ret);
        break;
    }
    case 'M': {
        char *next;
        int addr = strtoul(c + 1, &next, 16);
        int length = strtoul(next + 1, &next, 16);
        next++;
        for(int i = addr; i < addr + length; ++i) {
            char bb[3] = {next[0], next[1], 0};
            next += 2;
            uint8_t v = strtoul(bb, nullptr, 16);
            WriteBImpl(i, v);
        }
        sendPacket(sock, "OK");
        break;
    }
    case 'X': {
        char *next;
        int addr = strtoul(c + 1, &next, 16);
        int length = strtoul(next + 1, &next, 16);
        next++;
        for(int i = addr; i < addr + length; ++i) {
            WriteBImpl(i, *next++);
        }
        sendPacket(sock, "OK");
        break;
    }
    case 's':
        if(setjmp(ex_buf) == 0) {
            run_op();
            sendPacket(sock, "S00");
        } else {
            cpu.bus_lock = false;
            cpu.PC = cpu.oldpc;
            lastStopped = std::format("S{:02x}", get_signum(ex_n));
            sendPacket(sock, lastStopped);
        }
        break;
    case 'g':
        sendPacket(sock, dumpReg());
        break;
    case '?':
        sendPacket(sock, lastStopped);
        break;
    case 'Z':
        switch(c[1]) {
        case '0':
        case '1': {
            char *nx;
            uint32_t addr = strtoul(c + 3, &nx, 16);
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
            uint32_t addr = strtoul(c + 3, &nx, 16);
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
extern bool DEBUG;
void debug_activate() {
    lastStopped = "S00";
    asio::io_context io_context;
    cpu.sleeping.store(true);
    DEBUG = true;
    std::thread th{run_cpu};
    th.detach();
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 5555));
    for(;;) {
        sock = std::make_unique<tcp::socket>(io_context);
        acceptor.accept(*sock);
        for(std::string s = nextPacket(*sock); !s.empty();
            s = nextPacket(*sock)) {
            if(s == "-") {
                // retry
                sendPacket(*sock, lastTransmission);
                continue;
            }
            if(s == "+") {
                // ack
                continue;
            }
            sock->write_some(asio::buffer("+"));
            //                    std::print("DEBUG:REQ={}\n", s);
            if(s[0] == 0x03) {
                // Inturrput
                stop_cpu(std::format("S{:02x}", SIGINT));
                continue;
            }
            if(gdb_cmd(*sock, s.c_str())) {
                sock.reset();
                return;
            }
        }
    }
}
