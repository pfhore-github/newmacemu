#include "68040.hpp"
#include "bus.hpp"
#include "exception.hpp"
#include "inline.hpp"
#include "memory.hpp"
#include <memory>
void MOVEM_W_TO_MEM_DECR(uint16_t regs, uint8_t reg) {
    for(int i = cpu.movem_cnt; i < 16; ++i) {
        if(regs & 1 << i) {
            MMU_WriteW(cpu.A[reg] -= 2, cpu.R(15 - i));
        }
        cpu.movem_cnt = i;
    }
}

void MOVEM_W_TO_MEM_ADDR(uint16_t regs) {

    for(int i = cpu.movem_cnt; i < 16; ++i) {
        if(regs & 1 << i) {
            MMU_WriteW(cpu.movem_addr, cpu.R(i));
        }
        cpu.movem_addr += 2;
        cpu.movem_cnt = i;
    }
}

void MOVEM_W_FROM_MEM_INCR(uint16_t regs, uint8_t reg) {
    for(int i = cpu.movem_cnt; i < 16; ++i) {
        if(regs & 1 << i) {
            if(i > 7) {
                cpu.A[i & 7] = static_cast<int16_t>(MMU_ReadW(cpu.A[reg]));
            } else {
                STORE_W(cpu.D[i], MMU_ReadW(cpu.A[reg]));
            }
            cpu.A[reg] += 2;
        }
        cpu.movem_cnt = i;
    }
}

void MOVEM_W_FROM_MEM_ADDR(uint16_t regs) {

    for(int i = cpu.movem_cnt; i < 16; ++i) {
        if(regs & 1 << i) {
            if(i > 7) {
                cpu.A[i & 7] = static_cast<int16_t>(MMU_ReadW(cpu.movem_addr));
            } else {
                STORE_W(cpu.D[i], MMU_ReadW(cpu.movem_addr));
            }
            cpu.movem_addr += 2;
        }
        cpu.movem_cnt = i;
    }
}

void MOVEM_L_TO_MEM_DECR(uint16_t regs, uint8_t reg) {
    for(int i = cpu.movem_cnt; i < 16; ++i) {
        if(regs & 1 << i) {
            MMU_WriteL(cpu.A[reg] -= 4, cpu.R(15 - i));
        }
        cpu.movem_cnt = i;
    }
}

void MOVEM_L_TO_MEM_ADDR(uint16_t regs) {
    for(int i = cpu.movem_cnt; i < 16; ++i) {
        if(regs & 1 << i) {
            MMU_WriteL(cpu.movem_addr, cpu.R(i));
        }
        cpu.movem_addr += 4;
        cpu.movem_cnt = i;
    }
}

void MOVEM_L_FROM_MEM_INCR(uint16_t regs, uint8_t reg) {
    for(int i = cpu.movem_cnt; i < 16; ++i) {
        if(regs & 1 << i) {
            cpu.R(i) = MMU_ReadL(cpu.A[reg]);
            cpu.A[reg] += 4;
        }
        cpu.movem_cnt = i;
    }
}

void MOVEM_L_FROM_MEM_ADDR(uint16_t regs) {

    for(int i = cpu.movem_cnt; i < 16; ++i) {
        if(regs & 1 << i) {
            cpu.R(i) = MMU_ReadL(cpu.movem_addr);
            cpu.movem_addr += 4;
        }
        cpu.movem_cnt = i;
    }
}