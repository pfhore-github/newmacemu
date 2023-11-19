#include "68040.hpp"
#include "exception.hpp"
#include "jit.hpp"
#include "memory.hpp"
#include "inline.hpp"
#include <memory>
#include <unordered_map>
#include <utility>
using namespace asmjit;

static void parseExt_jit(x86::Assembler &a, int reg) {
    uint32_t pc = cpu.PC;
    uint16_t next = FETCH();
    int ri = next >> 12 & 15;
    bool ri_l = next & 1 << 11;
    int ri_c = next >> 9 & 3;
    // base
    if(reg >= 0) {
        a.mov(x86::r10d, x86::dword_ptr(x86::rbx, (8 + reg) * sizeof(uint32_t)));
    } else {
        a.mov(x86::r10d, pc);
    }
    // index
    if(ri_l) {
        a.mov(x86::eax, x86::dword_ptr(x86::rbx, ri * sizeof(uint32_t)));
    } else {
        a.movsx(x86::eax, x86::word_ptr(x86::rbx, ri * sizeof(uint32_t)));
    }
    if(!(next & 1 << 8)) {
        int8_t d = next & 0xff;
        a.lea(x86::r9,
              x86::ptr(x86::r10, x86::rax, ri_c, static_cast<int32_t>(d)));
        return;
    }
    if(next & 1 << 7) {
        a.xor_(x86::r10, x86::r10);
    }
    if(next & 1 << 6) {
        a.xor_(x86::rax, x86::rax);
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
        a.lea(x86::r9,
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
        a.lea(x86::r11,
              x86::ptr(x86::r10, x86::rax, ri_c, static_cast<int32_t>(bd)));
        a.mov(ARG1, x86::r11d);
        a.call(ReadL);
        a.lea(x86::r9, x86::ptr(x86::rax, od_v));
    } else {
        a.mov(x86::r12, x86::rax);
        a.lea(x86::r11, x86::ptr(x86::r10, static_cast<int32_t>(bd)));
        a.mov(ARG1, x86::r11d);
        a.call(ReadL);
        a.lea(x86::r9, x86::ptr(x86::r12, x86::rax, ri_c, od_v));
    }
}
void ea_getaddr_jit(x86::Assembler &a, int type, int reg, int sz) {
    switch(type) {
    case 2:
        a.mov(x86::r9d, x86::dword_ptr(x86::rbx, (8 + reg) * sizeof(uint32_t)));
        break;
    case 3:
        if( reg == 7 && sz == 1) {
            sz = 2;
        }
        a.mov(x86::r9d, x86::dword_ptr(x86::rbx, (8 + reg) * sizeof(uint32_t)));
        a.lea(x86::rax, x86::ptr(x86::r9, sz));
        a.mov(x86::dword_ptr(x86::rbx, (8 + reg) * sizeof(uint32_t)), x86::eax);
        break;
    case 4:
        if( reg == 7 && sz == 1) {
            sz = 2;
        }
        a.mov(x86::r9d, x86::dword_ptr(x86::rbx, (8 + reg) * sizeof(uint32_t)));
        a.lea(x86::r9, x86::ptr(x86::r9, -sz));
        a.mov(x86::dword_ptr(x86::rbx, (8 + reg) * sizeof(uint32_t)), x86::r9d);
        break;
    case 5: {
        int16_t d = FETCH();
        a.mov(x86::r9d, x86::dword_ptr(x86::rbx, (8 + reg) * sizeof(uint32_t)));
        a.lea(x86::r9d, x86::ptr(x86::r9d, d));
        break;
    }
    case 6:
        parseExt_jit(a, reg);
        break;
    case 7:
        switch(reg) {
        case 0:
            a.mov(x86::r9d, static_cast<int16_t>(FETCH()));
            break;
        case 1:
            a.mov(x86::r9d, FETCH32());
            break;
        case 2: {
            uint32_t now = cpu.PC;
            int16_t d = FETCH();
            a.mov(x86::r9d, now + d);
            break;
        }
        case 3:
            parseExt_jit(a, -1);
            break;
        default:
            throw JitError{};
        }
        break;
    default:
        throw JitError{};
    }
    a.mov(x86::dword_ptr(x86::rbx, offsetof(Cpu, EA)), x86::r9d);
}

void ea_readB_jit(x86::Assembler &a, int type, int reg) {
    if(type == 0) {
        a.movzx(x86::eax, x86::byte_ptr(x86::rbx, reg * sizeof(uint32_t)));
    } else if(type == 7 && reg == 4) {
        a.mov(x86::eax, FETCH() & 0xff);
    } else {
        ea_getaddr_jit(a, type, reg, 1);
        a.mov(ARG1, x86::r9d);
        a.call(ReadB);
    }
}

void ea_readW_jit(x86::Assembler &a, int type, int reg) {
    if(type == 0) {
        a.movzx(x86::eax, x86::word_ptr(x86::rbx, reg * sizeof(uint32_t)));
    } else if(type == 1) {
        a.movzx(x86::eax, x86::word_ptr(x86::rbx, (8 + reg) * sizeof(uint32_t)));
    } else if(type == 7 && reg == 4) {
        a.mov(x86::eax, FETCH());
    } else {
        ea_getaddr_jit(a, type, reg, 2);
        a.mov(ARG1, x86::r9d);
        a.call(ReadW);
    }
}

void ea_readL_jit(x86::Assembler &a, int type, int reg) {
    if(type == 0) {
        a.mov(x86::eax, x86::dword_ptr(x86::rbx, reg * sizeof(uint32_t)));
    } else if(type == 1) {
        a.mov(x86::eax, x86::dword_ptr(x86::rbx, (8 + reg) * sizeof(uint32_t)));
    } else if(type == 7 && reg == 4) {
        a.mov(x86::eax, FETCH32());
    } else {
        ea_getaddr_jit(a, type, reg, 4);
        a.mov(ARG1, x86::r9d);
        a.call(ReadL);
    }
}

void ea_writeB_jit(x86::Assembler &a, int type, int reg, bool update) {
    if(type == 0) {
        a.mov(x86::byte_ptr(x86::rbx, reg * sizeof(uint32_t)), x86::al);
    } else {
        if(!update) {
            ea_getaddr_jit(a, type, reg, 1);
        }
        a.mov(ARG1, x86::dword_ptr(x86::rbx, offsetof(Cpu, EA)));
        a.movzx(ARG2, x86::al);
        a.call(WriteB);
    }
}

void ea_writeW_jit(x86::Assembler &a, int type, int reg, bool update) {
    if(type == 0) {
        a.mov(x86::word_ptr(x86::rbx, reg * sizeof(uint32_t)), x86::ax);
    } else if(type == 1) {
        a.cwde();
        a.mov(x86::dword_ptr(x86::rbx, (reg + 8) * sizeof(uint32_t)), x86::eax);
    } else {
        if(!update) {
            ea_getaddr_jit(a, type, reg, 2);
        }
        a.mov(ARG1, x86::dword_ptr(x86::rbx, offsetof(Cpu, EA)));
        a.movzx(ARG2, x86::ax);
        a.call(WriteW);
    }
}

void ea_writeL_jit(x86::Assembler &a, int type, int reg, bool update) {
    if(type == 0) {
        a.mov(x86::dword_ptr(x86::rbx, reg * sizeof(uint32_t)), x86::eax);
    } else if(type == 1) {
        a.mov(x86::dword_ptr(x86::rbx, (reg + 8) * sizeof(uint32_t)), x86::eax);
    } else {
        if(!update) {
            ea_getaddr_jit(a, type, reg, 4);
        }
        a.mov(ARG1, x86::dword_ptr(x86::rbx, offsetof(Cpu, EA)));
        a.mov(ARG2, x86::eax);
        a.call(WriteL);
    }
}