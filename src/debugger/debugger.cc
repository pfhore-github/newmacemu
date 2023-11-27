#ifndef CI
#include "68040.hpp"
#include "SDL_net.h"
#include "bus.hpp"
#include "memory.hpp"
#include "inline.hpp"
#include <deque>
#include <fmt/format.h>
#include <signal.h>
#include <thread>
#include <unordered_set>
static TCPsocket recv;
static std::string nextPacket() {
    char c[2] = {};
    int i = SDLNet_TCP_Recv(recv, c, 1);
    if(i == 0) {
        return "";
    }
    if(c[0] != '$') {
        // special letter
        return c;
    }
    std::string p;
    for(;;) {
        char cc[512];
        int i = SDLNet_TCP_Recv(recv, cc, 512);
        if(i <= 0) {
            return "";
        }
        p += std::string(cc, cc + i);
        if(i < 512) {
            break;
        }
    }
    return p.substr(0, p.size() - 3);
}
static std::string lastTransmission = "";
static void sendPacket(const std::string &str) {
    uint8_t cc = 0;
    for(char c : str) {
        cc += (uint8_t)c;
    }
    auto re = fmt::format("${}#{:02x}", str, cc);
    fmt::print("DEBUG:response={}\n", re);
    lastTransmission = str;
    SDLNet_TCP_Send(recv, re.data(), re.size());
}
std::tuple<uint64_t, uint16_t> store_fpX(int i);
uint32_t Get_FPSR();
uint16_t Get_FPCR();
std::string dumpReg() {
    std::string s;
    s.reserve(1024);
    for(int i = 0; i < 8; ++i) {
        s += fmt::format("{:08x}", cpu.D[i]);
    }
    for(int i = 0; i < 8; ++i) {
        s += fmt::format("{:08x}", cpu.A[i]);
    }
    s += fmt::format("{:08x}{:08x}", GetSR(), cpu.PC);
    for(int i = 0; i < 8; ++i) {
        auto [f, e] = store_fpX(i);
        s += fmt::format("{:04x}0000{:016x}", e, f);
    }
    s += fmt::format("{:08x}", Get_FPCR());
    s += fmt::format("{:08x}", Get_FPSR());
    s += fmt::format("{:08x}", cpu.FPIAR);
    return s;
}
int get_signum(EXCAPTION_NUMBER con) {
    switch(con) {
    case EXCAPTION_NUMBER::AFAULT:
    case EXCAPTION_NUMBER::ADDR_ERR:
        return SIGBUS;
    case EXCAPTION_NUMBER::ILLEGAL_OP:
    case EXCAPTION_NUMBER::PRIV_ERR:
    case EXCAPTION_NUMBER::ALINE:
    case EXCAPTION_NUMBER::FLINE:
        return SIGILL;
    case EXCAPTION_NUMBER::DIV0:
    case EXCAPTION_NUMBER::FP_UNORDER:
    case EXCAPTION_NUMBER::FP_INEX:
    case EXCAPTION_NUMBER::FP_DIV0:
    case EXCAPTION_NUMBER::FP_UNFL:
    case EXCAPTION_NUMBER::FP_OPERR:
    case EXCAPTION_NUMBER::FP_OVFL:
    case EXCAPTION_NUMBER::FP_SNAN:
    case EXCAPTION_NUMBER::FP_UNIMPL_TYPE:
        return SIGFPE;
    case EXCAPTION_NUMBER::CHK_FAIL:
    case EXCAPTION_NUMBER::TRAPx:
    case EXCAPTION_NUMBER::TRACE:
        return SIGTRAP;
    default:
        return 0;
    }
}
static std::string lastStopped;
static std::unordered_set<uint32_t> bks;
static std::atomic<int> cnt{-1};
void run_op();
void stop_cpu(const std::string &stop) {
    sendPacket(lastStopped = stop);
    cpu.sleeping.store(true);
}
void debug_cpu(std::stop_token stoken) {
    while(!stoken.stop_requested()) {
        if(cnt != -1 && --cnt == 0) {
            stop_cpu("S00");
            continue;
        }
        if(bks.contains(cpu.PC)) {
            stop_cpu("T05hwbreak:;");
            continue;
        }
        if(cpu.sleeping.load()) {
            while(cpu.sleeping.load()) {
                cpu.sleeping.wait(true);
            }
            continue;
        }
        if( setjmp(cpu.ex) == 0) {
            run_op();
        } else {
            // EXCEPTION
            cpu.bus_lock = false;
            cpu.PC = cpu.oldpc;
            stop_cpu(fmt::format("S{:02x}", get_signum(cpu.ex_n)));
        }
    }
}
void run_op();
void gdb_q(const std::string &s) {
    if(s.starts_with("Supported")) {
        sendPacket("swbreak+;hwbreak+");
    } else {
        sendPacket("");
    }
}
bool gdb_cmd(const char *c) {
    switch(*c) {
    case 'H':
        sendPacket("OK");
        break;
    case 'k':
        return true;
    case 'R':
        cpu.A[7] = ReadL(cpu.VBR);
        cpu.PC = ReadL(cpu.VBR + 4);
        break;
    case 'q':
        gdb_q(c + 1);
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
        if( sig == get_signum(cpu.ex_n)) {
            handle_exception(cpu.ex_n);
        } else {
            // TODO: convert to another exception?
        }
        cpu.sleeping.store(false);
        cpu.sleeping.notify_one();
        break;
    }

    case 'D':
        sendPacket("OK");
        return true;
    case 'm': {
        char *next;
        int addr = strtoul(c + 1, &next, 16);
        int length = strtoul(next + 1, nullptr, 16);
        std::string ret;
        ret.reserve(length * 2);
        for(int i = addr; i < addr + length; ++i) {
            try {
                ret += fmt::format("{:02x}", ReadB(i));
            } catch(BusError &) {
                ret += "00";
            }
        }
        sendPacket(ret);
        break;
    }
    case 'M': {
        char *next;
        int addr = strtoul(c + 1, &next, 16);
        int length = strtoul(next + 1, &next, 16);
        next++;
        for(int i = addr; i < addr + length; ++i) {
            try {
                char bb[3] = {next[0], next[1], 0};
                next += 2;
                uint8_t v = strtoul(bb, nullptr, 16);
                WriteB(i, v);
            } catch(BusError &) {
            }
        }
        sendPacket("OK");
        break;
    }
    case 'X': {
        char *next;
        int addr = strtoul(c + 1, &next, 16);
        int length = strtoul(next + 1, &next, 16);
        next++;
        for(int i = addr; i < addr + length; ++i) {
            try {
                WriteB(i, *next++);
            } catch(BusError &) {
            }
        }
        sendPacket("OK");
        break;
    }
    case 's':
        if(setjmp(cpu.ex) == 0) {
            run_op();
            sendPacket("S00");
        } else {
            cpu.bus_lock = false;
            cpu.PC = cpu.oldpc;
            lastStopped = fmt::format("S{:02x}", get_signum(cpu.ex_n));
            sendPacket(lastStopped);
        }
        break;
    case 'g':
        sendPacket(dumpReg());
        break;
    case '?':
        sendPacket(lastStopped);
        break;
    case 'Z':
        switch(c[1]) {
        case '0':
        case '1': {
            char *nx;
            uint32_t addr = strtoul(c + 3, &nx, 16);
            bks.insert(addr);
            sendPacket("OK");
            break;
        }
        default:
            sendPacket("");
        }
        break;
    case 'z':
        switch(c[1]) {
        case '0':
        case '1': {
            char *nx;
            uint32_t addr = strtoul(c + 3, &nx, 16);
            bks.erase(addr);
            sendPacket("OK");
            break;
        }
        default:
            sendPacket("");
        }
        break;
    default:
        sendPacket("");
        break;
    }
    return false;
}
void debug_activate() {
    SDLNet_Init();
    lastStopped = "S00";
    IPaddress addr;
    if(SDLNet_ResolveHost(&addr, NULL, 5555) == -1) {
        printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        exit(1);
    }
    TCPsocket sock = SDLNet_TCP_Open(&addr);
    SDLNet_SocketSet socketset = SDLNet_AllocSocketSet(2);
    SDLNet_TCP_AddSocket(socketset, sock);
    cpu.sleeping.store(true);
    std::jthread th{debug_cpu};
    for(;;) {
        SDLNet_CheckSockets(socketset, ~0);
        if(SDLNet_SocketReady(sock)) {
            if((recv = SDLNet_TCP_Accept(sock))) {
                for(std::string s = nextPacket(); !s.empty();
                    s = nextPacket()) {
                    if(s == "-") {
                        // retry
                        sendPacket(lastTransmission);
                        continue;
                    }
                    if(s == "+") {
                        // ack
                        continue;
                    }
                    SDLNet_TCP_Send(recv, "+", 1); // ACK
                    fmt::print("DEBUG:REQ={}\n", s);
                    if(s[0] == 0x03) {
                        // Inturrput
                        stop_cpu(fmt::format("S{:02x}", SIGINT));
                        continue;
                    }
                    if(gdb_cmd(s.c_str())) {
                        goto END;
                    }
                }
            }
        }
    }
END:
    th.request_stop();
    th.join();
    SDLNet_TCP_Close(sock);
    SDLNet_Quit();
}
#else
void debug_activate() {}
#endif