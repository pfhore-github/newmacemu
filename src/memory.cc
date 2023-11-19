#include <memory>

#include "68040.hpp"
#include "SDL_endian.h"
#include "bus.hpp"
#include "exception.hpp"
#include "io.hpp"
#include "memory.hpp"
#include <expected>
#include <optional>
inline TM GetTMData() { return cpu.S ? TM::SYS_DATA : TM::USER_DATA; }
inline TM GetTMCode() { return cpu.S ? TM::SYS_CODE : TM::USER_CODE; }
std::optional<uint32_t> ptest_and_check(uint32_t addr, bool code, bool W);
std::optional<uint8_t> ReadBImpl(uint32_t addr) {
    auto paddr = ptest_and_check(addr, false, false);
    return paddr.transform(BusReadB);
}

uint8_t ReadB(uint32_t addr) {
    try {
        auto v = ReadBImpl(addr);
        if(!v) {
            goto FAULT;
        }
        return *v;
    } catch(BusError &) {
        goto FAULT;
    }
FAULT:
    ACCESS_FAULT(addr, SIZ::B, true, GetTMData());
}

std::optional<uint16_t> ReadWImpl(uint32_t addr) {
    if(addr & 1) [[unlikely]] {
        auto vx1 = ReadBImpl(addr);
        if(!vx1) {
            return {};
        }
        cpu.faultParam->MA = true;
        auto vx2 = ReadBImpl(addr + 1);
        if(!vx2) {
            return {};
        }
        return *vx1 << 8 | *vx2;
    }
    auto paddr = ptest_and_check(addr, false, false);
    return paddr.transform(BusReadW);
}

uint16_t ReadW(uint32_t addr) {
    try {
        auto v = ReadWImpl(addr);
        if(!v) {
            goto FAULT;
        }
        return *v;
    } catch(BusError &) {
        goto FAULT;
    }
FAULT:
    ACCESS_FAULT(addr, SIZ::W, true, GetTMData());
}

uint16_t FetchW(uint32_t addr) {
    try {
        auto paddr = ptest_and_check(addr, true, false);
        if(!paddr) {
            goto FAULT;
        }
        return BusReadW(*paddr);
    } catch(BusError &) {
        goto FAULT;
    }
FAULT:
    ACCESS_FAULT(addr, SIZ::W, true, GetTMCode());
}

uint32_t ReadL(uint32_t addr) {
    try {
        if(addr & 2) [[unlikely]] {
            auto vx1 = ReadWImpl(addr);
            if(!vx1) {
                goto FAULT;
            }
            cpu.faultParam->MA = true;
            auto vx2 = ReadWImpl(addr + 2);
            if(!vx2) {
                goto FAULT;
            }
            return *vx1 << 16 | *vx2;
        }
        auto paddr = ptest_and_check(addr, false, false);
        if(!paddr) {
            goto FAULT;
        }
        return BusReadL(*paddr);
    } catch(BusError &) {
        goto FAULT;
    }
FAULT:
    ACCESS_FAULT(addr, SIZ::L, true, GetTMData());
}

static bool WriteBImpl(uint32_t addr, uint8_t b) {
    auto paddr = ptest_and_check(addr, false, true);
    if(paddr) {
        BusWriteB(*paddr, b);
        return true;
    }
    return false;
}

void WriteB(uint32_t addr, uint8_t b) {
    try {
        if(!WriteBImpl(addr, b)) {
            goto FAULT;
        }
        return;
    } catch(BusError &) {
        goto FAULT;
    }
FAULT:
    ACCESS_FAULT(addr, SIZ::B, false, GetTMData());
}

bool WriteWImpl(uint32_t addr, uint16_t w) {
    if(addr & 1)  [[unlikely]] {
        if(!WriteBImpl(addr, w >> 8)) {
            return false;
        }
        cpu.faultParam->MA = true;
        return WriteBImpl(addr + 1, w);
    }
    auto paddr = ptest_and_check(addr, false, true);
    if(paddr) {
        BusWriteW(*paddr, w);
        return true;
    }
    return false;
}
void WriteW(uint32_t addr, uint16_t w) {
    try {
        if(!WriteWImpl(addr, w)) {
            goto FAULT;
        }
        return;
    } catch(BusError &) {
        goto FAULT;
    }
FAULT:
    ACCESS_FAULT(addr, SIZ::W, false, GetTMData());
}
void WriteL(uint32_t addr, uint32_t l) {
    try {
        if(addr & 2)  [[unlikely]] {
            if(!WriteWImpl(addr, l >> 16)) {
                goto FAULT;
            }
            cpu.faultParam->MA = true;
            if(!WriteWImpl(addr + 2, l)) {
                goto FAULT;
            }
            return;
        }
        auto paddr = ptest_and_check(addr, false, true);
        if(!paddr) {
            goto FAULT;
        }
        BusWriteL(*paddr, l);
        return;
    } catch(BusError &) {
        goto FAULT;
    }
FAULT:
    ACCESS_FAULT(addr, SIZ::L, false, GetTMData());
}
uint8_t *doBus16(uint32_t addr) ;
void Read16(uint32_t addr, uint8_t *to) {
    try {
        auto paddr = ptest_and_check(addr, false, false);
        if(!paddr) {
            goto FAULT;
        }
        memcpy(to, doBus16(*paddr), 16);
        return;
    } catch(BusError &) {
        goto FAULT;
    }
FAULT:
    cpu.faultParam->tt = TT::MOVE16;
    ACCESS_FAULT(addr, SIZ::LN, true, TM::USER_DATA);
}

void Write16(uint32_t addr, const uint8_t *from) {
    try {
        auto paddr = ptest_and_check(addr, false, true);
        if(!paddr) {
            goto FAULT;
        }
        memcpy(doBus16(*paddr), from, 16);
        return;
    } catch(BusError &) {
        goto FAULT;
    }
FAULT:
    cpu.faultParam->tt = TT::MOVE16;
    ACCESS_FAULT(addr, SIZ::LN, false, TM::USER_DATA);
}