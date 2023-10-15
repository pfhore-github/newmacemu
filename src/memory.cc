#include <memory>

#include "68040.hpp"
#include "SDL_endian.h"
#include "bus.hpp"
#include "exception.hpp"
#include "io.hpp"
#include "memory.hpp"
uint8_t ReadBImpl(uint32_t addr, bool code = false);
uint16_t ReadWImpl(uint32_t addr, bool code = false);
uint32_t ReadLImpl(uint32_t addr, bool code = false);
inline TM GetTM(bool code) {
    return cpu.S  ? code ? TM::SYS_CODE : TM::SYS_DATA
           : code ? TM::USER_CODE
                  : TM::USER_DATA;
}
uint32_t ptest_and_raise(uint32_t addr, bool sys, bool code, bool W);
uint8_t ReadBImpl(uint32_t addr, bool code) {
    auto paddr = ptest_and_raise(addr, cpu.S, code, false);
    return BusReadB(paddr);
}
uint8_t ReadB(uint32_t addr, bool code) {
    try {
        return ReadBImpl(addr, code);
    } catch(AccessFault &) {
        cpu.af_value.addr = addr;
        cpu.af_value.RW = true;
        cpu.af_value.size = SIZ::B;
        cpu.af_value.tm = GetTM(code);
        ACCESS_FAULT();
        __builtin_unreachable();
    }
}

uint16_t ReadWImpl(uint32_t addr, bool code) {
    if(addr & 1) {
        uint16_t v1 = ReadBImpl(addr, code) << 8;
        cpu.af_value.MA = true;
        v1 |= ReadBImpl(addr + 1, code);
        return v1;
    }
    auto paddr = ptest_and_raise(addr, cpu.S, code, false);
    return BusReadW(paddr);
}
uint16_t ReadW(uint32_t addr, bool code) {
    try {
        return ReadWImpl(addr, code);
    } catch(AccessFault &) {
        cpu.af_value.addr = addr;
        cpu.af_value.RW = true;
        cpu.af_value.size = SIZ::W;
        cpu.af_value.tm = GetTM(code);
        ACCESS_FAULT();
        __builtin_unreachable();
    }
}

uint32_t ReadLImpl(uint32_t addr, bool code) {
    if(addr & 2) {
        uint32_t v1 = ReadWImpl(addr, code) << 16;
        cpu.af_value.MA = true;
        v1 |= ReadWImpl(addr + 2, code);
        return v1;
    }
    auto paddr = ptest_and_raise(addr, cpu.S, code, false);
    return BusReadL(paddr);
}
uint32_t ReadL(uint32_t addr, bool code) {
    try {
        return ReadLImpl(addr, code);
    } catch(AccessFault &) {
        cpu.af_value.addr = addr;
        cpu.af_value.RW = true;
        cpu.af_value.size = SIZ::L;
        cpu.af_value.tm = GetTM(code);
        ACCESS_FAULT();
        __builtin_unreachable();
    }
}

void WriteBImpl(uint32_t addr, uint8_t b) {
    auto paddr = ptest_and_raise(addr, cpu.S, false, true);
    BusWriteB(paddr, b);
}
void WriteB(uint32_t addr, uint8_t b) {
    try {
        WriteBImpl(addr, b);
    } catch(AccessFault &) {
        cpu.af_value.addr = addr;
        cpu.af_value.RW = false;
        cpu.af_value.size = SIZ::B;
        cpu.af_value.tm = GetTM(false);
        ACCESS_FAULT();
        __builtin_unreachable();
    }
}

void WriteWImpl(uint32_t addr, uint16_t w) {
    if(addr & 1) {
        WriteBImpl(addr, w >> 8);
        cpu.af_value.MA = true;
        WriteBImpl(addr + 1, w);
        return;
    }
    auto paddr = ptest_and_raise(addr, cpu.S, false, true);
    BusWriteW(paddr, w);
}
void WriteW(uint32_t addr, uint16_t w) {
    try {
        WriteWImpl(addr, w);
    } catch(AccessFault &) {
        cpu.af_value.addr = addr;
        cpu.af_value.RW = false;
        cpu.af_value.size = SIZ::W;
        cpu.af_value.tm = GetTM(false);
        ACCESS_FAULT();
        __builtin_unreachable();
    }
}
void WriteLImpl(uint32_t addr, uint32_t l) {
    if(addr & 2) {
        WriteWImpl(addr, l >> 16);
        cpu.af_value.MA = true;
        WriteWImpl(addr + 2, l);
        return;
    }
    auto paddr = ptest_and_raise(addr, cpu.S, false, true);
    BusWriteL(paddr, l);
}
void WriteL(uint32_t addr, uint32_t l) {
    try {
        WriteLImpl(addr, l);
    } catch(AccessFault &) {
        cpu.af_value.addr = addr;
        cpu.af_value.RW = false;
        cpu.af_value.size = SIZ::L;
        cpu.af_value.tm = GetTM(false);
        ACCESS_FAULT();
        __builtin_unreachable();
    }
}
