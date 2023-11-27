#include "exception.hpp"
#include "jit_common.hpp"
#include "memory.hpp"
#include <memory>
#include <unordered_map>
#include <utility>
using namespace asmjit;
static void parseExt_jit(int reg) {
    uint32_t pc = cpu.PC;
    uint16_t next = FETCH();
    int ri = next >> 12 & 15;
    bool ri_l = next & 1 << 11;
    int ri_c = next >> 9 & 3;
    // base
    if(reg >= 0) {
        as->mov(x86::r10d, AR_L(reg));
    } else {
        as->mov(x86::r10d, pc);
    }
    // index
    if(ri_l) {
        as->mov(x86::eax, DR_L(ri));
    } else {
        as->movsx(x86::eax, DR_W(ri));
    }
    if(!(next & 1 << 8)) {
        int8_t d = next & 0xff;
        as->lea(x86::rsi,
              x86::ptr(x86::r10, x86::rax, ri_c, static_cast<int32_t>(d)));
        return;
    }
    if(next & 1 << 7) {
        as->xor_(x86::r10, x86::r10);
    }
    if(next & 1 << 6) {
        as->xor_(x86::rax, x86::rax);
    }
    int32_t bd = 0;
    switch(next >> 4 & 3) {
    case 1:
        break;
    case 2:
        bd = static_cast<int16_t>(FETCH());
        break;
    case 3:
        bd = FETCH32();
        break;
    }
    int32_t od_v = 0;
    switch(next & 3) {
    case 0:
        as->lea(x86::rsi,
              x86::ptr(x86::r10, x86::rax, ri_c, static_cast<int32_t>(bd)));
        return;
    case 1:
        break;
    case 2:
        od_v = static_cast<int16_t>(FETCH());
        break;
    case 3:
        od_v = FETCH32();
        break;
    }
    if(!(next & 1 << 2)) {
        as->lea(x86::r11,
              x86::ptr(x86::r10, x86::rax, ri_c, static_cast<int32_t>(bd)));
        jit_readL( x86::r11d);
        as->lea(x86::rsi, x86::ptr(x86::rax, od_v));
    } else {
        as->mov(x86::r12, x86::rax);
        as->lea(x86::r11, x86::ptr(x86::r10, static_cast<int32_t>(bd)));
        jit_readL( x86::r11d);
        as->lea(x86::rsi, x86::ptr(x86::r12, x86::rax, ri_c, od_v));
    }
}
void ea_getaddr_jit(int type, int reg, int sz) {
    switch(type) {
    case 2:
        as->mov(x86::esi, AR_L(reg));
        break;
    case 3:
        if(reg == 7 && sz == 1) {
            sz = 2;
        }
        as->mov(x86::esi, AR_L(reg));
        as->lea(x86::rax, x86::ptr(x86::rsi, sz));
        as->mov(AR_L(reg), x86::eax);
        break;
    case 4:
        if(reg == 7 && sz == 1) {
            sz = 2;
        }
        as->sub(AR_L(reg), sz);
        as->mov(x86::esi, AR_L(reg));
        break;
    case 5: {
        int16_t d = FETCH();
        as->mov(x86::esi, AR_L(reg));
        as->lea(x86::rsi, x86::ptr(x86::rsi, d));
        break;
    }
    case 6:
        parseExt_jit( reg);
        break;
    case 7:
        switch(reg) {
        case 0:
            as->mov(x86::esi, static_cast<int16_t>(FETCH()));
            break;
        case 1:
            as->mov(x86::esi, FETCH32());
            break;
        case 2: {
            uint32_t now = cpu.PC;
            int16_t d = FETCH();
            as->mov(x86::esi, now + d);
            break;
        }
        case 3:
            parseExt_jit( -1);
            break;
        default:
            throw JitError{};
        }
        break;
    default:
        throw JitError{};
    }
    as->mov(ARG1.r32(), x86::esi);
    as->mov(EA, x86::esi);
}

void ea_readB_jit(int type, int reg, bool lk) {
    if(type == 0) {
        as->movzx(x86::eax, DR_B(reg));
    } else if(type == 7 && reg == 4) {
        as->mov(x86::eax, FETCH() & 0xff);
    } else {
        ea_getaddr_jit( type, reg, 1);
        if( lk) {
            as->inc(CPU_BYTE(bus_lock));
        }
        jit_readB( ARG1.r32());
    }
}

void ea_readW_jit(int type, int reg, bool lk) {
    if(type == 0) {
        as->movzx(x86::eax, DR_W(reg));
    } else if(type == 1) {
        as->movzx(x86::eax, AR_W(reg));
    } else if(type == 7 && reg == 4) {
        as->mov(x86::eax, FETCH());
    } else {
        ea_getaddr_jit( type, reg, 2);
        if( lk) {
            as->inc(CPU_BYTE(bus_lock));
        }
        jit_readW( ARG1.r32());
    }
}

void ea_readL_jit(int type, int reg, bool lk) {
    if(type == 0) {
        as->mov(x86::eax, DR_L(reg));
    } else if(type == 1) {
        as->mov(x86::eax, AR_L(reg));
    } else if(type == 7 && reg == 4) {
        as->mov(x86::eax, FETCH32());
    } else {
        ea_getaddr_jit( type, reg, 4);
        if( lk) {
            as->inc(CPU_BYTE(bus_lock));
        }
        jit_readL( ARG1.r32());
    }
}

void ea_writeB_jit(int type, int reg, bool update) {
    if(type == 0) {
        as->mov(DR_B(reg), x86::al);
    } else {
        if(!update) {
            ea_getaddr_jit( type, reg, 1);
        }
        jit_writeB( EA, x86::al);
        as->mov(CPU_BYTE(bus_lock), 0);
    }
}

void ea_writeW_jit(int type, int reg, bool update) {
    if(type == 0) {
        as->mov(DR_W(reg), x86::ax);
    } else if(type == 1) {
        as->cwde();
        as->mov(AR_L(reg), x86::eax);
    } else {
        if(!update) {
            ea_getaddr_jit( type, reg, 2);
        }
        jit_writeW( EA, x86::ax);
        as->mov(CPU_BYTE(bus_lock), 0);
    }
}

void ea_writeL_jit(int type, int reg, bool update) {
    if(type == 0) {
        as->mov(DR_L(reg), x86::eax);
    } else if(type == 1) {
        as->mov(AR_L(reg), x86::eax);
    } else {
        if(!update) {
            ea_getaddr_jit( type, reg, 4);
        }
        jit_writeL( EA, x86::eax);
        as->mov(CPU_BYTE(bus_lock), 0);
    }
}