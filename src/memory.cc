#include <memory>

#include "68040.hpp"
#include "SDL3/SDL_endian.h"
#include "bus.hpp"
#include "exception.hpp"
#include "io.hpp"
#include "memory.hpp"
#include <expected>
#include <optional>
inline TM GetTMData() { return cpu.S ? TM::SYS_DATA : TM::USER_DATA; }
inline TM GetTMCode() { return cpu.S ? TM::SYS_CODE : TM::USER_CODE; }
uint32_t ptest_and_check(uint32_t addr, bool code, bool W);

uint8_t ReadBImpl(uint32_t addr, bool lock) {
	auto paddr = ptest_and_check(addr, false, lock);
	return BusReadB(paddr);
}

uint16_t ReadWImpl(uint32_t addr, bool code, bool lock) {
    if(!code && addr & 1) [[unlikely]] {
		auto vx1 = ReadBImpl(addr, lock);
		try {
			auto vx2 = ReadBImpl(addr + 1, lock);
			return vx1 << 8 | vx2;
		} catch(PTestError& e) {
			e.code |= SSW_MA;
			throw e;
		}
    }
	auto paddr = ptest_and_check(addr, code, lock);
	return BusReadW(paddr);
}

uint32_t ReadLImpl(uint32_t addr, bool lock) {
    if(addr & 2) [[unlikely]] {
        auto vx1 = ReadWImpl(addr, false, lock);
		try {
            auto vx2 = ReadWImpl(addr + 2, false, lock);
			return vx1 << 16 | vx2;
		} catch( PTestError& e) {
			e.code |= SSW_MA;
			throw e;
		}
    }
	auto paddr = ptest_and_check(addr, false, lock);
	return BusReadL(paddr);
}

void WriteBImpl(uint32_t addr, uint8_t b) {
	auto paddr = ptest_and_check(addr, false, true);
	BusWriteB(paddr, b);
}

void WriteWImpl(uint32_t addr, uint16_t w) {
    if(addr & 1) [[unlikely]] {
        WriteBImpl(addr, w >> 8);
		try {
            WriteBImpl(addr + 1, w);
			return;
		} catch( PTestError& e) {
			e.code |= SSW_MA;
			throw e;
		}
    } 
    auto paddr = ptest_and_check(addr, false, true);
	BusWriteW(paddr, w);
}

static void WriteLImpl(uint32_t addr, uint32_t l) {
    if(addr & 2) [[unlikely]] {
        WriteWImpl(addr, l >> 16);
		try {
			WriteWImpl(addr + 2, l);
			return;
		} catch(PTestError& e) {
			e.code |= SSW_MA;
			throw e;
		}
    }
    auto paddr = ptest_and_check(addr, false, true);
	BusWriteL(paddr, l);
}

uint8_t ReadB(uint32_t addr) {
	try {
		return ReadBImpl(addr, cpu.bus_lock);
	} catch(PTestError& e) {
        cpu.fault_SSW = e.code;
        ACCESS_FAULT(addr, SIZ::B, true, GetTMData());
    }
}

uint16_t ReadW(uint32_t addr) {
	try {
		return ReadWImpl(addr, false, cpu.bus_lock);
	} catch(PTestError& e) {
        cpu.fault_SSW = e.code;
        ACCESS_FAULT(addr, SIZ::W, true, GetTMData());
    }
}

uint16_t FetchW(uint32_t addr) {
	try {
		return ReadWImpl(addr, true, false);
    } catch(PTestError& e) {
        cpu.fault_SSW = e.code;
        ACCESS_FAULT(addr, SIZ::W, true, GetTMCode());
    }
}

uint32_t ReadL(uint32_t addr) {
	try {
		return ReadLImpl(addr, cpu.bus_lock);
    } catch(PTestError& e) {
        cpu.fault_SSW = e.code;
        ACCESS_FAULT(addr, SIZ::L, true, GetTMData());
    }
}

void WriteB(uint32_t addr, uint8_t b) {
	try {
		WriteBImpl(addr, b);
	} catch(PTestError& e) {
        cpu.fault_SSW = e.code;
        ACCESS_FAULT(addr, SIZ::B, false, GetTMData());
    }
}

void WriteW(uint32_t addr, uint16_t w) {
	try {
		WriteWImpl(addr, w);
	} catch(PTestError& e) {
        cpu.fault_SSW = e.code;
        ACCESS_FAULT(addr, SIZ::W, false, GetTMData());
    }
}
void WriteL(uint32_t addr, uint32_t l) {
	try {
		WriteLImpl(addr, l);
	} catch(PTestError& e) {
        cpu.fault_SSW = e.code;
        ACCESS_FAULT(addr, SIZ::L, false, GetTMData());
    }
}

uint8_t *doBus16(uint32_t addr);
void Read16(uint32_t addr, uint8_t *to) {
	try {
		auto paddr = ptest_and_check(addr, false, false);
		auto maddr = doBus16(paddr);
		memcpy(to, maddr, 16);
		return;
	} catch(PTestError& e) {
		cpu.fault_SSW = e.code;
		ACCESS_FAULT(addr, SIZ::LN, true, TM::USER_DATA, TT_MOVE16);
	}
}

void Write16(uint32_t addr, const uint8_t *from) {
	try {
		auto paddr = ptest_and_check(addr, false, true);
        auto maddr = doBus16(paddr);
		memcpy(maddr, from, 16);
		return;
	} catch(PTestError& e) {
		cpu.fault_SSW |= e.code;
		ACCESS_FAULT(addr, SIZ::LN, false, TM::USER_DATA, TT_MOVE16);
	}
}

void Transfer16(uint32_t from, uint32_t to) {
    uint8_t buf[16];
    Read16(from & ~0xf, buf);
    Write16(to & ~0xf, buf);
}

uint8_t ReadSB(uint32_t addr) {
    try {
		return ReadBImpl(addr, false);
	} catch(PTestError& e) {
        cpu.fault_SSW = e.code;
        ACCESS_FAULT(addr, SIZ::B, true, TM(cpu.SFC), TT_ALT);
    }
}

uint16_t ReadSW(uint32_t addr) {
	try {
		return ReadWImpl(addr, false, false);
	} catch(PTestError& e) {
        cpu.fault_SSW = e.code;
        ACCESS_FAULT(addr, SIZ::W, true, TM(cpu.SFC), TT_ALT);
    }
}

uint32_t ReadSL(uint32_t addr) {
	try {
		return ReadLImpl(addr, false);
	} catch(PTestError& e) {
        cpu.fault_SSW = e.code;
        ACCESS_FAULT(addr, SIZ::L, true, TM(cpu.SFC), TT_ALT);
    }
}

void WriteSB(uint32_t addr, uint8_t b) {
	try {
		WriteBImpl(addr, b);
	} catch(PTestError& e) {
        cpu.fault_SSW = e.code;
        ACCESS_FAULT(addr, SIZ::B, false, TM(cpu.DFC), TT_ALT);
    }
}

void WriteSW(uint32_t addr, uint16_t w) {
	try {
		WriteWImpl(addr, w);
	} catch(PTestError& e) {
        cpu.fault_SSW = e.code;
        ACCESS_FAULT(addr, SIZ::W, false, TM(cpu.DFC), TT_ALT);
    }
}
void WriteSL(uint32_t addr, uint32_t l) {
	try {
		WriteLImpl(addr, l);
	} catch(PTestError& e) {
        cpu.fault_SSW = e.code;
        ACCESS_FAULT(addr, SIZ::L, false, TM(cpu.DFC), TT_ALT);
    }
}
