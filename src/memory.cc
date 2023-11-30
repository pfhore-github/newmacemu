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
std::expected<uint32_t, uint16_t> ptest_and_check(uint32_t addr, bool code,
                                                  bool W);

static std::expected<uint8_t, uint16_t> ReadBImpl(uint32_t addr) {
    try {
        if(auto paddr = ptest_and_check(addr, false, cpu.bus_lock)) {
            return BusReadB(*paddr);
        } else {
            return std::unexpected{paddr.error()};
        }
    } catch(BusError &) {
        return std::unexpected{0};
    }
}

static std::expected<uint16_t, uint16_t> ReadWImpl(uint32_t addr,
                                                   bool code = false) {
    if(!code && addr & 1) [[unlikely]] {
        if(auto vx1 = ReadBImpl(addr)) {
            if(auto vx2 = ReadBImpl(addr + 1)) {
                return *vx1 << 8 | *vx2;
            } else {
                return std::unexpected{vx2.error() | SSW_MA};
            }
        } else {
            return std::unexpected{vx1.error()};
        }
    }
    if(auto paddr = ptest_and_check(addr, code, cpu.bus_lock)) {
        try {
            return BusReadW(*paddr);
        } catch(BusError &) {
            return std::unexpected{0};
        }
    } else {
        return std::unexpected{paddr.error()};
    }
}

std::expected<uint32_t, uint16_t> ReadLImpl(uint32_t addr) {
    if(addr & 2) [[unlikely]] {
        if(auto vx1 = ReadWImpl(addr)) {
            if(auto vx2 = ReadWImpl(addr + 2)) {
                return *vx1 << 16 | *vx2;
            } else {
                return std::unexpected{vx2.error() | SSW_MA};
            }
        } else {
            return std::unexpected{vx1.error() | SSW_MA};
        }
    }
    if(auto paddr = ptest_and_check(addr, false, cpu.bus_lock)) {
        try {
            return BusReadL(*paddr);
        } catch(BusError &) {
            return std::unexpected{0};
        }
    } else {
        return std::unexpected{paddr.error()};
    }
}

static std::expected<void, uint16_t> WriteBImpl(uint32_t addr, uint8_t b) {
    try {
        if(auto paddr = ptest_and_check(addr, false, true)) {
            BusWriteB(*paddr, b);
            return {};
        } else {
            return std::unexpected{paddr.error()};
        }
    } catch(BusError &) {
        return std::unexpected{0};
    }
}

std::expected<void, uint16_t> WriteWImpl(uint32_t addr, uint16_t w) {
    if(addr & 1) [[unlikely]] {
        if(auto v1 = WriteBImpl(addr, w >> 8)) {
            if(auto v2 = WriteBImpl(addr + 1, w)) {
                return {};
            } else {
                return std::unexpected{v2.error() | SSW_MA};
            }
        } else {
            return std::unexpected{v1.error() | SSW_MA};
        }
    }
    if(auto paddr = ptest_and_check(addr, false, true)) {
        try {
            BusWriteW(*paddr, w);
            return {};
        } catch(BusError &) {
            return std::unexpected{0};
        }
    } else {
        return std::unexpected{paddr.error()};
    }
}

static std::expected<void, uint16_t> WriteLImpl(uint32_t addr, uint32_t l) {

    if(addr & 2) [[unlikely]] {
        if(auto v1 = WriteWImpl(addr, l >> 16)) {
            if(auto v2 = WriteWImpl(addr + 2, l)) {
                return {};
            } else {
                return std::unexpected{v2.error() | SSW_MA};
            }
        } else {
            return std::unexpected{v1.error() | SSW_MA};
        }
    }
    if(auto paddr = ptest_and_check(addr, false, true)) {
        try {
            BusWriteL(*paddr, l);
            return {};
        } catch(BusError &) {
            return std::unexpected{0};
        }
    } else {
        return std::unexpected{paddr.error()};
    }
}

uint8_t ReadB(uint32_t addr) {
    if(auto v = ReadBImpl(addr)) {
        return *v;
    } else {
        cpu.fault_SSW = v.error();
        ACCESS_FAULT(addr, SIZ::B, true, GetTMData());
    }
}

uint16_t ReadW(uint32_t addr) {
    if(auto v = ReadWImpl(addr)) {
        return *v;
    } else {
        cpu.fault_SSW = v.error();
        ACCESS_FAULT(addr, SIZ::W, true, GetTMData());
    }
}

uint16_t FetchW(uint32_t addr) {
    if(auto v = ReadWImpl(addr, true)) {
        return *v;
    } else {
        cpu.fault_SSW = v.error();
        ACCESS_FAULT(addr, SIZ::W, true, GetTMCode());
    }
}

uint32_t ReadL(uint32_t addr) {
    if(auto v = ReadLImpl(addr)) {
        return *v;
    } else {
        cpu.fault_SSW = v.error();
        ACCESS_FAULT(addr, SIZ::L, true, GetTMData());
    }
}

void WriteB(uint32_t addr, uint8_t b) {
    if(auto x = WriteBImpl(addr, b); !x) {
        cpu.fault_SSW = x.error();
        ACCESS_FAULT(addr, SIZ::B, false, GetTMData());
    }
}

void WriteW(uint32_t addr, uint16_t w) {
    if(auto x = WriteWImpl(addr, w); !x) {
        cpu.fault_SSW = x.error();
        ACCESS_FAULT(addr, SIZ::W, false, GetTMData());
    }
}
void WriteL(uint32_t addr, uint32_t l) {
    if(auto x = WriteLImpl(addr, l); !x) {
        cpu.fault_SSW = x.error();
        ACCESS_FAULT(addr, SIZ::L, false, GetTMData());
    }
}

uint8_t *doBus16(uint32_t addr);
void Read16(uint32_t addr, uint8_t *to) {
    auto paddr = ptest_and_check(addr, false, false);
    if(paddr) {
        if(auto maddr = doBus16(*paddr)) {
            memcpy(to, maddr, 16);
            return;
        }
    }
    cpu.fault_SSW = paddr.error();
    ACCESS_FAULT(addr, SIZ::LN, true, TM::USER_DATA, TT_MOVE16);
}

void Write16(uint32_t addr, const uint8_t *from) {
    auto paddr = ptest_and_check(addr, false, true);
    if(paddr) {
        if(auto maddr = doBus16(*paddr)) {
            memcpy(maddr, from, 16);
            return;
        }
    }
    cpu.fault_SSW |= paddr.error();
    ACCESS_FAULT(addr, SIZ::LN, false, TM::USER_DATA, TT_MOVE16);
}

void Transfer16(uint32_t from, uint32_t to) {
    uint8_t buf[16];
    Read16(from & ~0xf, buf);
    Write16(to & ~0xf, buf);
}

uint8_t ReadSB(uint32_t addr) {
    if(auto v = ReadBImpl(addr)) {
        return *v;
    } else {
        cpu.fault_SSW = v.error();
        ACCESS_FAULT(addr, SIZ::B, true, TM(cpu.SFC), TT_ALT);
    }
}

uint16_t ReadSW(uint32_t addr) {
    auto v = ReadWImpl(addr);
    if(v) {
        return *v;
    } else {
        cpu.fault_SSW = v.error();
        ACCESS_FAULT(addr, SIZ::W, true, TM(cpu.SFC), TT_ALT);
    }
}

uint32_t ReadSL(uint32_t addr) {
    auto p = ReadLImpl(addr);
    if(p) {
        return *p;
    } else {
        cpu.fault_SSW = p.error();
        ACCESS_FAULT(addr, SIZ::L, true, TM(cpu.SFC), TT_ALT);
    }
}

void WriteSB(uint32_t addr, uint8_t b) {
    if(auto x = WriteBImpl(addr, b); !x) {
        cpu.fault_SSW = x.error();
        ACCESS_FAULT(addr, SIZ::B, false, TM(cpu.DFC), TT_ALT);
    }
}

void WriteSW(uint32_t addr, uint16_t w) {
    if(auto x = WriteWImpl(addr, w); !x) {
        cpu.fault_SSW = x.error();
        ACCESS_FAULT(addr, SIZ::W, false, TM(cpu.DFC), TT_ALT);
    }
}
void WriteSL(uint32_t addr, uint32_t l) {
    if(auto x = WriteLImpl(addr, l); !x) {
        cpu.fault_SSW = x.error();
        ACCESS_FAULT(addr, SIZ::L, false, TM(cpu.DFC), TT_ALT);
    }
}
