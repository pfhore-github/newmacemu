#include <memory>

#include "68040.hpp"
#include "SDL_endian.h"
#include "bus.hpp"
#include "exception.hpp"
#include "io.hpp"
#include "memory.hpp"

extern std::vector<uint8_t> RAM;
extern const uint8_t *ROM;
// bool rom_is_overlay = true;
extern size_t ROMSize;

inline static uint16_t readBE16(const uint8_t *base, uint32_t offset) {
    return SDL_SwapBE16(*reinterpret_cast<const uint16_t *>(base + offset));
}

inline static uint32_t readBE32(const uint8_t *base, uint32_t offset) {
    return SDL_SwapBE32(*reinterpret_cast<const uint32_t *>(base + offset));
}

inline static void writeBE16(uint8_t *base, uint32_t offset, uint16_t v) {
    *reinterpret_cast<uint16_t *>(base + offset) = SDL_SwapBE16(v);
}

inline static void writeBE32(uint8_t *base, uint32_t offset, uint32_t v) {
    *reinterpret_cast<uint32_t *>(base + offset) = SDL_SwapBE32(v);
}

uint8_t BusReadB(uint32_t addr) {
    uint32_t offset = addr & 0x0FFFFFFF;
    switch(addr >> 28) {
    case 0:
    case 1:
    case 2:
    case 3:
        // RAM
        if(addr >= RAM.size()) {
            throw AccessFault{};
        }
        return RAM[addr];
    case 4:
        return ROM[offset & (ROMSize - 1)];
    case 5:
        return io->Read8(offset);
    default:
        throw AccessFault{};
    }
}
void BusWriteB(uint32_t addr, uint8_t v) {
    uint32_t offset = addr & 0x0FFFFFFF;
    switch(addr >> 28) {
    case 0:
    case 1:
    case 2:
    case 3:
        // RAM
        if(addr >= RAM.size()) {
            throw AccessFault{};
        }
        RAM[addr] = v;
        break;
    case 5:      
        io->Write8(offset, v);
        break;
    default:
        throw AccessFault{};
    }
}

uint16_t BusReadW(uint32_t addr) {
    uint32_t offset = addr & 0x0FFFFFFF;
    switch(addr >> 28) {
    case 0:
    case 1:
    case 2:
    case 3:
        // RAM
        if(addr >= RAM.size()) {
            throw AccessFault{};
        }
        return readBE16(RAM.data(), addr);
    case 4:
        return readBE16(ROM, offset & (ROMSize - 1));
    case 5:
        return io->Read16(offset);
    default:
        throw AccessFault{};
    }
}

void BusWriteW(uint32_t addr, uint16_t v) {
    uint32_t offset = addr & 0x0FFFFFFF;
    switch(addr >> 28) {
    case 0:
    case 1:
    case 2:
    case 3:
        // RAM
        if(addr >= RAM.size()) {
            throw AccessFault{};
        }
        writeBE16(RAM.data(), addr, v);
        break;
    case 5:
        io->Write16(offset, v);
        break;
    default:
        throw AccessFault{};
    }
}
uint32_t BusReadL(uint32_t addr) {
    uint32_t offset = addr & 0x0FFFFFFF;
    switch(addr >> 28) {
    case 0:
    case 1:
    case 2:
    case 3:
        // RAM
        if(addr >= RAM.size()) {
            throw AccessFault{};
        }
        return readBE32(RAM.data(), addr);
    case 4:
        return readBE32(ROM, offset & (ROMSize - 1));
    case 5:
        return io->Read32(offset);
    default:
        throw AccessFault{};
    }
}
void BusWriteL(uint32_t addr, uint32_t v) {
    uint32_t offset = addr & 0x0FFFFFFF;
    switch(addr >> 28) {
    case 0:
    case 1:
    case 2:
    case 3:
        // RAM
        if(addr >= RAM.size()) {
            throw AccessFault{};
        }
        writeBE32(RAM.data(), addr, v);
        break;
    case 5:
        io->Write32(offset, v);
        break;
    default:
        throw AccessFault{};
    }
}

uint32_t ptest_and_raise(uint32_t addr, bool sys, bool code, bool W);
void Read16(uint32_t addr, uint8_t *to) {
    try {
        auto paddr = ptest_and_raise(addr, cpu.S, false, false);
        uint32_t offset = paddr & 0x0FFFFFFF;
        switch(paddr >> 28) {
        case 0:
        case 1:
        case 2:
        case 3:
            // RAM
            if(offset >= RAM.size()) {
                throw AccessFault{};
            }
            memcpy(to, RAM.data() + offset, 16);
            break;
        case 4:
            memcpy(to, ROM + offset, 16);
            break;
        default:
            throw AccessFault{};
        }
    } catch(AccessFault &) {
        cpu.af_value.addr = addr;
        cpu.af_value.RW = true;
        cpu.af_value.size = SIZ::LN;
        cpu.af_value.tt = TT::MOVE16;
        cpu.af_value.tm = TM::USER_DATA;
        ACCESS_FAULT();
    }
}

void Write16(uint32_t addr, const uint8_t *to) {
    try {
        auto paddr = ptest_and_raise(addr, cpu.S, false, true);
        uint32_t offset = paddr & 0x0FFFFFFF;
        switch(addr >> 28) {
        case 0:
        case 1:
        case 2:
        case 3:
            // RAM
            memcpy(RAM.data() + offset, to, 16);
            break;
        default:
            throw AccessFault{};
        }
    } catch(AccessFault &) {
        cpu.af_value.addr = addr;
        cpu.af_value.RW = false;
        cpu.af_value.size = SIZ::LN;
        cpu.af_value.tt = TT::MOVE16;
        cpu.af_value.tm = TM::USER_DATA;
        ACCESS_FAULT();
    }
}
void MMU_Transfer16(uint32_t from, uint32_t to) {
    uint8_t buf[16];
    Read16(from & ~0xf, buf);
    Write16(to & ~0xf, buf);
}
