#include <memory>

#include "68040.hpp"
#include "SDL_endian.h"
#include "bus.hpp"
#include "exception.hpp"
#include "io.hpp"
#include "memory.hpp"

extern std::vector<uint8_t> RAM;
extern const uint8_t *ROM;
bool rom_is_overlay = true;
extern size_t ROMSize;
inline static uint8_t readBE8(const uint8_t *base, uint32_t offset) {
    return *(base + offset);
}
inline static uint16_t readBE16(const uint8_t *base, uint32_t offset) {
    return SDL_SwapBE16(*reinterpret_cast<const uint16_t *>(base + offset));
}

inline static uint32_t readBE32(const uint8_t *base, uint32_t offset) {
    return SDL_SwapBE32(*reinterpret_cast<const uint32_t *>(base + offset));
}
inline static void writeBE8(uint8_t *base, uint32_t offset, uint8_t v) {
    *(base + offset) = v;
}

inline static void writeBE16(uint8_t *base, uint32_t offset, uint16_t v) {
    *reinterpret_cast<uint16_t *>(base + offset) = SDL_SwapBE16(v);
}

inline static void writeBE32(uint8_t *base, uint32_t offset, uint32_t v) {
    *reinterpret_cast<uint32_t *>(base + offset) = SDL_SwapBE32(v);
}
class BusBase {
  public:
    virtual uint8_t ReadB(uint32_t address) = 0;
    virtual uint16_t ReadW(uint32_t address) = 0;
    virtual uint32_t ReadL(uint32_t address) = 0;
    virtual void Read16B(uint32_t address, uint8_t *dst) = 0;
    virtual void WriteB(uint32_t address, uint8_t from) = 0;
    virtual void WriteW(uint32_t address, uint16_t from) = 0;
    virtual void WriteL(uint32_t address, uint32_t from) = 0;
    virtual void Write16B(uint32_t address, const uint8_t *dst) = 0;
};
struct AccessFault {};
class RomBus : public BusBase {
  public:
    uint8_t ReadB(uint32_t address) override {
        if(address >= ROMSize) {
            throw AccessFault{};
        }
        return readBE8(ROM, address);
    }
    uint16_t ReadW(uint32_t address) override {
        if(address >= ROMSize) {
            throw AccessFault{};
        }
        return readBE16(ROM, address);
    }
    uint32_t ReadL(uint32_t address) override {
        if(address >= ROMSize) {
            throw AccessFault{};
        }
        return readBE32(ROM, address);
    }
    virtual void Read16B(uint32_t address, uint8_t *dst) {
        if(address >= ROMSize) {
            throw AccessFault{};
        }
        memcpy(dst, ROM + address, 16);
    }
    void WriteB(uint32_t, uint8_t) { throw AccessFault{}; }
    void WriteW(uint32_t, uint16_t) { throw AccessFault{}; }
    void WriteL(uint32_t, uint32_t) { throw AccessFault{}; }
    void Write16B(uint32_t, const uint8_t *) { throw AccessFault{}; }

    RomBus() {}
};
class RamBus : public BusBase {
  public:
    uint8_t ReadB(uint32_t address) override {
        if(rom_is_overlay) {
            return RomBus().ReadB(address);
        } else {
            if(address >= RAM.size()) {
                throw AccessFault{};
            }
            return readBE8(RAM.data(), address);
        }
    }
    uint16_t ReadW(uint32_t address) override {
        if(rom_is_overlay) {
            return RomBus().ReadW(address);
        } else {
            if(address >= RAM.size()) {
                throw AccessFault{};
            }
            return readBE16(RAM.data(), address);
        }
    }
    uint32_t ReadL(uint32_t address) override {
        if(rom_is_overlay) {
            return RomBus().ReadL(address);
        } else {
            if(address >= RAM.size()) {
                throw AccessFault{};
            }
            return readBE32(RAM.data(), address);
        }
    }
    void Read16B(uint32_t address, uint8_t *dst) override {
        if(rom_is_overlay) {
            return RomBus().Read16B(address, dst);
        } else {
            if(address >= RAM.size()) {
                throw AccessFault{};
            }
            memcpy(dst, RAM.data() + address, 16);
        }
    }
    void WriteB(uint32_t address, uint8_t v) override {
        if(rom_is_overlay) {
            // ignore
            return;
        } else if(address < RAM.size()) {
            writeBE8(RAM.data(), address, v);
        } else {
            throw AccessFault{};
        }
    }
    void WriteW(uint32_t address, uint16_t v) override {
        if(rom_is_overlay) {
            // ignore
            return;
        } else if(address < RAM.size()) {
            writeBE16(RAM.data(), address, v);
        } else {
            throw AccessFault{};
        }
    }
    void WriteL(uint32_t address, uint32_t v) override {
        if(rom_is_overlay) {
            // ignore
            return;
        } else if(address < RAM.size()) {
            writeBE32(RAM.data(), address, v);
        } else {
            throw AccessFault{};
        }
    }

    RamBus() {}
    void Write16B(uint32_t address, const uint8_t *src) override {
        if(rom_is_overlay) {
            // ignore
            return;
        } else {
            if(address >= RAM.size()) {
                throw AccessFault{};
            }
            memcpy(RAM.data() + address, src, 16);
        }
    }
};

class IOBus : public BusBase {
  public:
    uint8_t ReadB(uint32_t address) override { return LoadIO_B(address); }
    uint16_t ReadW(uint32_t address) override { return LoadIO_W(address); }
    uint32_t ReadL(uint32_t address) override { return LoadIO_L(address); }
    void Read16B(uint32_t, uint8_t *) override { throw AccessFault{}; }
    void WriteB(uint32_t address, uint8_t from) override {
        StoreIO_B(address, from);
    }
    void WriteW(uint32_t address, uint16_t from) override {
        StoreIO_W(address, from);
    }
    void WriteL(uint32_t address, uint32_t from) override {
        StoreIO_L(address, from);
    }
    void Write16B(uint32_t, const uint8_t *) override { throw AccessFault{}; }
    IOBus() {}
};

static std::unique_ptr<BusBase> busMap[16];
void initBus() {
    busMap[0] = std::make_unique<RamBus>();
    busMap[1] = std::make_unique<RamBus>();
    busMap[2] = std::make_unique<RamBus>();
    busMap[3] = std::make_unique<RamBus>();
    busMap[4] = std::make_unique<RomBus>();
    busMap[5] = std::make_unique<IOBus>();
}

uint8_t BusReadB(uint32_t addr) {
    uint8_t top = addr >> 28;
    uint32_t offset = addr & 0x3fffffff;
    return busMap[top]->ReadB(offset);
}
void BusWriteB(uint32_t addr, uint8_t v) {
    uint8_t top = addr >> 28;
    uint32_t offset = addr & 0x3fffffff;
    busMap[top]->WriteB(offset, v);
}
uint16_t BusReadW(uint32_t addr) {
    if(addr & 1) {
        uint16_t v = BusReadB(addr) << 8;
        try {
            v |= BusReadB(addr + 1);
        } catch(AccessFault &e) {
            cpu.af_value.MA = true;
            cpu.af_value.addr = addr;
            throw e;
        }
        return v;
    } else {
        uint8_t top = addr >> 28;
        uint32_t offset = addr & 0x3fffffff;
        return busMap[top]->ReadW(offset);
    }
}
void BusWriteW(uint32_t addr, uint16_t v) {
    if(addr & 1) {
        BusWriteB(addr, v >> 8);
        try {
            BusWriteB(addr + 1, v);
        } catch(AccessFault &e) {
            cpu.af_value.addr = addr;
            cpu.af_value.MA = true;
            throw e;
        }
    } else {
        uint8_t top = addr >> 28;
        uint32_t offset = addr & 0x3fffffff;
        busMap[top]->WriteW(offset, v);
    }
}
uint32_t BusReadL(uint32_t addr) {
    if(addr & 2) {
        uint32_t v = BusReadW(addr) << 16;
        try {
            v |= BusReadW(addr + 2);
        } catch(AccessFault &e) {
            cpu.af_value.addr = addr;
            cpu.af_value.MA = true;
            throw e;
        }
        return v;
    } else {
        uint8_t top = addr >> 28;
        uint32_t offset = addr & 0x3fffffff;
        return busMap[top]->ReadL(offset);
    }
}
void BusWriteL(uint32_t addr, uint32_t v) {
    if(addr & 2) {
        BusWriteW(addr, v >> 16);
        try {
            BusWriteW(addr + 2, v);
        } catch(AccessFault &e) {
            cpu.af_value.addr = addr;
            cpu.af_value.MA = true;
            throw e;
        }
    } else {
        uint8_t top = addr >> 28;
        uint32_t offset = addr & 0x3fffffff;
        busMap[top]->WriteL(offset, v);
    }
}

void BusAccess(MemBus &bus) {
    try {
        switch(bus.sz) {
        case SIZ::B:
            if(bus.RW) {
                bus.D = BusReadB(bus.A);
            } else {
                BusWriteB(bus.A, bus.D);
            }
            break;
        case SIZ::W:
            if(bus.RW) {
                bus.D = BusReadW(bus.A);
            } else {
                BusWriteW(bus.A, bus.D);
            }
            break;
        case SIZ::L:
            if(bus.RW) {
                bus.D = BusReadL(bus.A);
            } else {
                BusWriteL(bus.A, bus.D);
            }
            break;
        default:
            ILLEGAL_OP();
        }
    } catch(AccessFault &e) {
        cpu.af_value.addr = bus.A;
        cpu.af_value.ea = 0;
        cpu.af_value.RW = bus.RW;
        cpu.af_value.size = bus.sz;
        cpu.af_value.tt = TT::NORMAL;
        cpu.af_value.tm = bus.tm;
        ACCESS_FAULT();
    }
}

void BusRead16(uint32_t from, uint8_t *to) {
    try {
        uint8_t top = from >> 28;
        uint32_t offset = from & 0x3fffffff;
        busMap[top]->Read16B(offset, to);
    } catch(AccessFault &e) {
        cpu.af_value.addr = from;
        cpu.af_value.ea = 0;
        cpu.af_value.RW = true;
        cpu.af_value.size = SIZ::LN;
        cpu.af_value.tt = TT::MOVE16;
        cpu.af_value.tm = TM::USER_DATA;
        ACCESS_FAULT();
    }
}

void BusWrite16(uint32_t from, const uint8_t *to) {
    try {
        uint8_t top = from >> 28;
        uint32_t offset = from & 0x3fffffff;
        busMap[top]->Write16B(offset, to);
    } catch(AccessFault &e) {
        cpu.af_value.addr = from;
        cpu.af_value.ea = 0;
        cpu.af_value.RW = false;
        cpu.af_value.size = SIZ::LN;
        cpu.af_value.tt = TT::MOVE16;
        cpu.af_value.tm = TM::USER_DATA;
        ACCESS_FAULT();
    }
}