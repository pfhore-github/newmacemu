#include "68040.hpp"
#include "bus.hpp"
#include "exception.hpp"
#include "memory.hpp"
#include "proto.hpp"
#include <fmt/format.h>
#include <memory>
uint32_t ea_getaddr(int type, int reg, int sz, bool rw);
namespace OP {

void movem_w_store_decr(uint16_t, int, int, int reg) {
    uint16_t regs = FETCH();
    if(!cpu.movem_run) {
        cpu.EA = cpu.A[reg] -= 2;
        cpu.movem_run = true;
    }
    cpu.af_value.CM = true;
    uint32_t a = cpu.EA;
    for(int i = 0; i < 16; ++i) {
        if(regs & 1 << i) {
            WriteW(a, cpu.R(15 - i));
            a -= 2;
        }
    }
    cpu.A[reg] = a + 2;
    cpu.af_value.CM = false;
    cpu.movem_run = false;
}
void movem_w_store_base(uint16_t, int, int type, int reg) {
    uint16_t regs = FETCH();
    if(!cpu.movem_run) {
        cpu.EA = ea_getaddr(type, reg, 2);
        cpu.movem_run = true;
    }
    cpu.af_value.CM = true;
    uint32_t a = cpu.EA;
    for(int i = 0; i < 16; ++i) {
        if(regs & 1 << i) {
            WriteW(a, cpu.R(i));
            a += 2;
        }
    }
    cpu.af_value.CM = false;
    cpu.movem_run = false;
}
void movem_l_store_decr(uint16_t, int, int, int reg) {
    uint16_t regs = FETCH();
    if(!cpu.movem_run) {
        cpu.EA = cpu.A[reg] -= 4;
        cpu.movem_run = true;
    }
    cpu.af_value.CM = true;
    uint32_t a = cpu.EA;
    for(int i = 0; i < 16; ++i) {
        if(regs & 1 << i) {
            WriteL(a, cpu.R(15 - i));
            a -= 4;
        }
    }
    cpu.A[reg] = a + 4;
    cpu.af_value.CM = false;
    cpu.movem_run = false;
}
void movem_l_store_base(uint16_t, int, int type, int reg) {
    uint16_t regs = FETCH();
    if(!cpu.movem_run) {
        cpu.EA = ea_getaddr(type, reg, 4);
        cpu.movem_run = true;
    }
    cpu.af_value.CM = true;
    uint32_t a = cpu.EA;
    for(int i = 0; i < 16; ++i) {
        if(regs & 1 << i) {
            WriteL(a, cpu.R(i));
            a += 4;
        }
    }
    cpu.af_value.CM = false;
    cpu.movem_run = false;
}


void movem_w_load_incr(uint16_t, int, int, int reg) {
    uint16_t regs = FETCH();
    if(!cpu.movem_run) {
        cpu.EA = cpu.A[reg];
        cpu.movem_run = true;
    }
    cpu.af_value.CM = true;
    cpu.A[reg] = cpu.EA;
    for(int i = 0; i < 16; ++i) {
        if(regs & 1 << i) {
            uint16_t v = ReadW(cpu.A[reg]);
            if(i > 7) {
                cpu.A[i & 7] = static_cast<int16_t>(v);
            } else {
                STORE_W(cpu.D[i], v);
            }
            cpu.A[reg] += 2;
        }
    }
    cpu.af_value.CM = false;
    cpu.movem_run = false;
}

void movem_w_load_base(uint16_t, int, int type, int reg) {
    uint16_t regs = FETCH();
    if(!cpu.movem_run) {
        cpu.EA = ea_getaddr(type, reg, 2);
        cpu.movem_run = true;
    }
    cpu.af_value.CM = true;
    uint32_t a = cpu.EA;
    for(int i = 0; i < 16; ++i) {
        if(regs & 1 << i) {
            if(i > 7) {
                cpu.A[i & 7] = static_cast<int16_t>(ReadW(a));
            } else {
                STORE_W(cpu.D[i], ReadW(a));
            }
            a += 2;
        }
    }
    cpu.af_value.CM = false;
    cpu.movem_run = false;
}


void movem_l_load_incr(uint16_t, int, int, int reg) {
    uint16_t regs = FETCH();
    if(!cpu.movem_run) {
        cpu.EA = cpu.A[reg];
        cpu.movem_run = true;
    }
    cpu.af_value.CM = true;
    cpu.A[reg] = cpu.EA;
    for(int i = 0; i < 16; ++i) {
        if(regs & 1 << i) {
            cpu.R(i) = ReadL(cpu.A[reg]);
            cpu.A[reg] += 4;
        }
    }
    cpu.af_value.CM = false;
    cpu.movem_run = false;
}

void movem_l_load_base(uint16_t, int, int type, int reg) {
    uint16_t regs = FETCH();
    if(!cpu.movem_run) {
        cpu.EA = ea_getaddr(type, reg, 4);
        cpu.movem_run = true;
    }
    cpu.af_value.CM = true;
    uint32_t a = cpu.EA;

    for(int i = 0; i < 16; ++i) {
        if(regs & 1 << i) {
            cpu.R(i) = ReadL(a);
            a += 4;
        }
    }
    cpu.af_value.CM = false;
    cpu.movem_run = false;
}


} // namespace OP
