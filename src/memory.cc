#include <memory>

#include "68040.hpp"
#include "SDL_endian.h"
#include "bus.hpp"
#include "exception.hpp"
#include "io.hpp"
#include "memory.hpp"
inline TM GetTM(bool code) {
    return cpu.S  ? code ? TM::SYS_CODE : TM::SYS_DATA
           : code ? TM::USER_CODE
                  : TM::USER_DATA;
}
struct AccessFault {};
uint32_t ptest_and_raise(uint32_t addr, bool sys, bool code, bool W);
uint8_t MMU_ReadB(uint32_t addr, bool code) {
    try {
        auto ret = ptest_and_raise(addr  >>12, cpu.S, code, false);
        auto paddr = ret | (addr & 0xfff);
        return BusReadB(paddr);
    } catch(AccessFault &) {
        cpu.af_value.addr = addr;
        cpu.af_value.RW = true;
        cpu.af_value.size = SIZ::B;
        cpu.af_value.tt = TT::NORMAL;
        cpu.af_value.tm = GetTM(code);
        ACCESS_FAULT();
        __builtin_unreachable();
    }
}
uint16_t MMU_ReadW(uint32_t addr, bool code) {
    try {
        if(addr & 1) {
            uint16_t v1 = MMU_ReadB(addr, code) << 8;
            try {
                v1 |= MMU_ReadB(addr + 1, code);
            } catch(AccessFault &) {
                cpu.af_value.MA = true;
                throw;
            }
            return v1;
        }
        auto ret = ptest_and_raise(addr  >>12, cpu.S, code, false);
        auto paddr = ret | (addr & 0xfff);
        return BusReadW(paddr);
    } catch(AccessFault &) {
        cpu.af_value.addr = addr;
        cpu.af_value.RW = true;
        cpu.af_value.size = SIZ::W;
        cpu.af_value.tt = TT::NORMAL;
        cpu.af_value.tm = GetTM(code);
        ACCESS_FAULT();
        __builtin_unreachable();
    }
}
uint32_t MMU_ReadL(uint32_t addr, bool code) {
    try {
        if(addr & 2) {
            uint32_t v1 = MMU_ReadW(addr, code) << 16;
            try {
                v1 |= MMU_ReadW(addr + 2, code);
            } catch(AccessFault &) {
                cpu.af_value.MA = true;
                throw;
            }
            return v1;
        }
        auto ret = ptest_and_raise(addr  >>12, cpu.S, code, false);
        auto paddr = ret | (addr & 0xfff);
        return BusReadL(paddr);
    } catch(AccessFault &) {
        cpu.af_value.addr = addr;
        cpu.af_value.RW = true;
        cpu.af_value.size = SIZ::L;
        cpu.af_value.tt = TT::NORMAL;
        cpu.af_value.tm = GetTM(code);
        ACCESS_FAULT();
        __builtin_unreachable();
    }
}
void MMU_WriteB(uint32_t addr, uint8_t b) {
    try {
        auto ret = ptest_and_raise(addr >>12, cpu.S, false, true);
        auto paddr = ret | (addr & 0xfff);
        return BusWriteB(paddr, b);
    } catch(AccessFault &) {
        cpu.af_value.addr = addr;
        cpu.af_value.RW = false;
        cpu.af_value.size = SIZ::B;
        cpu.af_value.tt = TT::NORMAL;
        cpu.af_value.tm = GetTM(false);
        ACCESS_FAULT();
        __builtin_unreachable();
    }
}

void MMU_WriteW(uint32_t addr, uint16_t w) {
    try {
        if(addr & 1) {
            MMU_WriteB(addr, w >> 8);
            try {
                MMU_WriteB(addr + 1, w);
            } catch(AccessFault &) {
                cpu.af_value.MA = true;
                throw;
            }
            return;
        }
        auto ret = ptest_and_raise(addr  >>12, cpu.S, false, true);
        auto paddr = ret | (addr & 0xfff);
        BusWriteW(paddr, w);
    } catch(AccessFault &) {
        cpu.af_value.addr = addr;
        cpu.af_value.RW = false;
        cpu.af_value.size = SIZ::W;
        cpu.af_value.tt = TT::NORMAL;
        cpu.af_value.tm = GetTM(false);
        ACCESS_FAULT();
        __builtin_unreachable();
    }
}
void MMU_WriteL(uint32_t addr, uint32_t l) {
    try {
        if(addr & 1) {
            MMU_WriteW(addr, l >> 16);
            try {
                MMU_WriteW(addr + 2, l);
            } catch(AccessFault &) {
                cpu.af_value.MA = true;
                throw;
            }
            return;
        }
        auto ret = ptest_and_raise(addr  >>12, cpu.S, false, true);
        auto paddr = ret | (addr & 0xfff);
        BusWriteL(paddr, l);
    } catch(AccessFault &) {
        cpu.af_value.addr = addr;
        cpu.af_value.RW = false;
        cpu.af_value.size = SIZ::L;
        cpu.af_value.tt = TT::NORMAL;
        cpu.af_value.tm = GetTM(false);
        ACCESS_FAULT();
        __builtin_unreachable();
    }
}