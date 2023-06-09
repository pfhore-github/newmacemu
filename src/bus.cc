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
    uint16_t v;
    memcpy(&v, base + offset, 2);
    return SDL_SwapBE16(v);
}

inline static uint32_t readBE32(const uint8_t *base, uint32_t offset) {
    uint32_t v;
    memcpy(&v, base + offset, 4);
    return SDL_SwapBE32(v);
}
inline static void writeBE8(uint8_t *base, uint32_t offset, uint8_t v) {
    *(base + offset) = v;
}

inline static void writeBE16(uint8_t *base, uint32_t offset, uint16_t v) {
    v = SDL_SwapBE16(v);
    memcpy(base + offset, &v, 2);
}

inline static void writeBE32(uint8_t *base, uint32_t offset, uint32_t v) {
    v = SDL_SwapBE32(v);
    memcpy(base + offset, &v, 4);
}
struct AccessFault {};
class BusBase {
  public:
    virtual uint8_t ReadB(uint32_t /* address */) { throw AccessFault{}; }
    virtual uint16_t ReadW(uint32_t /* address */) { throw AccessFault{}; }
    virtual uint32_t ReadL(uint32_t /* address */) { throw AccessFault{}; }
    virtual void Read16B(uint32_t /* address */, uint8_t * /* dst */) {
        throw AccessFault{};
    }
    virtual void WriteB(uint32_t /* address */, uint8_t /* from */) {
        throw AccessFault{};
    }
    virtual void WriteW(uint32_t /* address */, uint16_t /* from */) {
        throw AccessFault{};
    }
    virtual void WriteL(uint32_t /* address */, uint32_t /* from */) {
        throw AccessFault{};
    }
    virtual void Write16B(uint32_t /* address */, const uint8_t * /* dst */) {
        throw AccessFault{};
    }
    virtual ~BusBase() {}
};
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
    void Read16B(uint32_t address, uint8_t *dst) override {
        if(address >= ROMSize) {
            throw AccessFault{};
        }
        memcpy(dst, ROM + address, 16);
    }

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
    void WriteB(uint32_t address, uint8_t from) override {
        StoreIO_B(address, from);
    }
    void WriteW(uint32_t address, uint16_t from) override {
        StoreIO_W(address, from);
    }
    void WriteL(uint32_t address, uint32_t from) override {
        StoreIO_L(address, from);
    }
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
    for(int i = 6; i < 0x10; ++i) {
        busMap[i] = std::make_unique<BusBase>();
    }
}
struct __attribute__((packed)) BusAddr {
    uint32_t offset : 28;
    uint8_t top : 4;
};
static_assert(sizeof(BusAddr) == sizeof(uint32_t));

uint8_t BusReadB(uint32_t addr) {
    auto ba = std::bit_cast<BusAddr>(addr);
    return busMap[ba.top]->ReadB(ba.offset);
}
void BusWriteB(uint32_t addr, uint8_t v) {
    auto ba = std::bit_cast<BusAddr>(addr);
    busMap[ba.top]->WriteB(ba.offset, v);
}
uint16_t BusReadW(uint32_t addr) {
    auto ba = std::bit_cast<BusAddr>(addr);
    return busMap[ba.top]->ReadW(ba.offset);
}

void BusWriteW(uint32_t addr, uint16_t v) {
    auto ba = std::bit_cast<BusAddr>(addr);
    busMap[ba.top]->WriteW(ba.offset, v);
}
uint32_t BusReadL(uint32_t addr) {
    auto ba = std::bit_cast<BusAddr>(addr);
    return busMap[ba.top]->ReadL(ba.offset);
}
void BusWriteL(uint32_t addr, uint32_t v) {
    auto ba = std::bit_cast<BusAddr>(addr);
    busMap[ba.top]->WriteL(ba.offset, v);
}

uint32_t ptest_and_raise(uint32_t addr, bool sys, bool code, bool W);
void MMU_Read16(uint32_t from, uint8_t *to) {
    try {
        auto paddr = ptest_and_raise(from & 0xFFFFF000, cpu.S, false, false) |
                     (from & 0xfff);
        auto ba = std::bit_cast<BusAddr>(paddr);
        busMap[ba.top]->Read16B(ba.offset, to);
    } catch(AccessFault &) {
        cpu.af_value.addr = from;
        cpu.af_value.ea = 0;
        cpu.af_value.RW = true;
        cpu.af_value.size = SIZ::LN;
        cpu.af_value.tt = TT::MOVE16;
        cpu.af_value.tm = TM::USER_DATA;
        ACCESS_FAULT();
    }
}

void MMU_Write16(uint32_t from, const uint8_t *to) {
    try {
        auto paddr = ptest_and_raise(from & 0xFFFFF000, cpu.S, false, true) |
                     (from & 0xfff);
        auto ba = std::bit_cast<BusAddr>(paddr);
        busMap[ba.top]->Write16B(ba.offset, to);
    } catch(AccessFault &) {
        cpu.af_value.addr = from;
        cpu.af_value.ea = 0;
        cpu.af_value.RW = false;
        cpu.af_value.size = SIZ::LN;
        cpu.af_value.tt = TT::MOVE16;
        cpu.af_value.tm = TM::USER_DATA;
        ACCESS_FAULT();
    }
}
void MMU_Transfer16(uint32_t from, uint32_t to) {
    uint8_t buf[16];
    MMU_Read16(from &~ 0xf, buf);
    MMU_Write16(to &~ 0xf, buf);
}
