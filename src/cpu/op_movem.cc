#include "68040.hpp"
#include "bus.hpp"
#include "exception.hpp"
#include "memory.hpp"
#include "proto.hpp"
#include <memory>
void MOVEM_W_TO_MEM_DECR(uint16_t regs, uint8_t reg) {
    cpu.af_value.CM = true;
    uint32_t a = cpu.EA;
    for(int i = 0; i < 16; ++i) {
        if( regs & 1 << i) {
            MMU_WriteW(a, cpu.R(15-i));
            a -= 2;
        }
    }
    cpu.A[reg] = a+2;
    cpu.af_value.CM = false;
    cpu.movem_run = false;
}

void MOVEM_L_TO_MEM_DECR(uint16_t regs, uint8_t reg) {
    cpu.af_value.CM = true;
    uint32_t a = cpu.EA;
    for(int i = 0; i < 16; ++i) {
        if(regs & 1 << i) {
            MMU_WriteL(a, cpu.R(15 - i));
            a -= 4;
        }
    }
    cpu.A[reg] = a+4;
    cpu.af_value.CM = false;
    cpu.movem_run = false;
}

void MOVEM_W_TO_MEM_ADDR(uint16_t regs) {
    cpu.af_value.CM = true;
    uint32_t a = cpu.EA;
    for(int i = 0; i < 16; ++i) {
        if( regs & 1 << i) {
            MMU_WriteW(a, cpu.R(i));
            a += 2;
        }
    }
    cpu.af_value.CM = false;
    cpu.movem_run = false;
}

void MOVEM_L_TO_MEM_ADDR(uint16_t regs) {
    cpu.af_value.CM = true;
    uint32_t a = cpu.EA;
    for(int i = 0; i < 16; ++i) {
        if(regs & 1 << i) {
            MMU_WriteL(a, cpu.R(i));
            a += 4;
        }
    }
    cpu.af_value.CM = false;
    cpu.movem_run = false;
}


void MOVEM_W_FROM_MEM_INCR(uint16_t regs, uint8_t reg) {
    cpu.af_value.CM = true;
    cpu.A[reg] = cpu.EA;
    for(int i = 0; i < 16; ++i) {
        if(regs & 1 << i) {
            uint16_t v = MMU_ReadW(cpu.A[reg]);
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

void MOVEM_L_FROM_MEM_INCR(uint16_t regs, uint8_t reg) {
    cpu.af_value.CM = true;
    cpu.A[reg] = cpu.EA;
    for(int i = 0; i < 16; ++i) {
        if(regs & 1 << i) {
            cpu.R(i) = MMU_ReadL(cpu.A[reg]);
            cpu.A[reg] += 4;
        }
    }
    cpu.af_value.CM = false;
    cpu.movem_run = false;
}

void MOVEM_W_FROM_MEM_ADDR(uint16_t regs) {
    cpu.af_value.CM = true;
    uint32_t a = cpu.EA;
    for(int i = 0; i < 16; ++i) {
        if(regs & 1 << i) {
            if(i > 7) {
                cpu.A[i & 7] = static_cast<int16_t>(MMU_ReadW(a));
            } else {
                STORE_W(cpu.D[i], MMU_ReadW(a));
            }
            a += 2;
        }
    }
    cpu.af_value.CM = false;
    cpu.movem_run = false;
}

void MOVEM_L_FROM_MEM_ADDR(uint16_t regs) {
    cpu.af_value.CM = true;
    uint32_t a = cpu.EA;

    for(int i = 0; i < 16; ++i) {
        if(regs & 1 << i) {
            cpu.R(i) = MMU_ReadL(a);
            a += 4;
        }
    }
    cpu.af_value.CM = false;
    cpu.movem_run = false;
}