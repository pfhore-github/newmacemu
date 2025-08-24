#include <memory>

#include "68040.hpp"
#include "bus.hpp"
#include "exception.hpp"
#include "io.hpp"
#include "memory.hpp"
extern std::vector<uint8_t> vram;
template <class T> struct bus_trait {};
template <> struct bus_trait<const uint8_t> {
    static uint8_t memDo(const uint8_t *p, uint8_t) { return *p; }
    static uint8_t ioDo(uint32_t addr, uint8_t) {
        return io->readB(addr & 0x0FFFFFFF);
    }
    static uint8_t nuBusDo(int base, uint32_t addr, uint8_t) {
        if(base == 9) {
            if(addr >= 0x1000) {
                return vram[addr - 0x1000];
            }
        }
        return 0;
    }
};
template <> struct bus_trait<uint8_t> {
    static void memDo(uint8_t *p, uint8_t v) { *p = v; }
    static void ioDo(uint32_t addr, uint8_t v) {
        io->writeB(addr & 0x0FFFFFFF, v);
    }
    static void nuBusDo(int base, uint32_t addr, uint8_t v) {
        if(base == 9) {
            if(addr >= 0x1000) {
                vram[addr - 0x1000] = v;
            }
        }
    }
};

template <> struct bus_trait<const uint16_t> {
    static uint16_t memDo(const uint8_t *p, uint16_t) { return readBE16(p); }
    static uint16_t ioDo(uint32_t addr, uint16_t) {
        return io->readW(addr & 0x0FFFFFFF);
    }
    static uint16_t nuBusDo(int base, uint32_t addr, uint16_t) {
        if(base == 9) {
            if(addr >= 0x1000) {
                return readBE16(&vram[addr - 0x1000]);
            }
        }
        return 0;
    }
};
template <> struct bus_trait<uint16_t> {
    static void memDo(uint8_t *p, uint16_t v) { writeBE16(p, v); }
    static void ioDo(const uint32_t addr, uint16_t v) {
        io->writeW(addr & 0x0FFFFFFF, v);
    }
    static void nuBusDo(int base, const uint32_t addr, uint16_t v) {
        if(base == 9) {
            if(addr >= 0x1000) {
                writeBE16(&vram[addr - 0x1000], v);
            }
        }
    }
};

template <> struct bus_trait<const uint32_t> {
    static uint32_t memDo(const uint8_t *p, uint32_t) { return readBE32(p); }
    static uint32_t ioDo(const uint32_t addr, uint32_t) {
        return io->readL(addr & 0x0FFFFFFF);
    }
    static uint32_t nuBusDo(int base, const uint32_t addr, uint32_t) {
        if(base == 9) {
            if(addr >= 0x1000) {
                return readBE32(&vram[addr - 0x1000]);
            }
        }
        return 0;
    }
};
template <> struct bus_trait<uint32_t> {
    static void memDo(uint8_t *p, uint32_t v) { writeBE32(p, v); }
    static void ioDo(const uint32_t addr, uint32_t v) {
        io->writeL(addr & 0x0FFFFFFF, v);
    }
    static void nuBusDo(int base, const uint32_t addr, uint32_t v) {
        if(base == 9) {
            if(addr >= 0x1000) {
                writeBE32(&vram[addr - 0x1000], v);
            }
        }
    }
};

template <class T> auto doBus(uint32_t addr, T v) {
    switch(addr >> 28) {
    case 0:
    case 1:
    case 2:
    case 3:
        // RAM
        if(addr >= RAMSize) {
            throw BusError{};
        }
        return bus_trait<T>::memDo(RAM + addr, v);
    case 4:
        // ROM
        return bus_trait<T>::memDo(ROM + (addr & ROMMask), v);
    case 5:
        return bus_trait<T>::ioDo(addr, v);
    case 0xF: {
        int nuBusN = (addr >> 24) & 7;
        return bus_trait<T>::nuBusDo(nuBusN, addr & 0xFFFFFF, v);
    }

    default:
        throw BusError{};
    }
}

uint8_t *doBus16(uint32_t addr) {
    switch(addr >> 28) {
    case 0:
    case 1:
    case 2:
    case 3:
        // RAM
        if(addr >= RAMSize) {
            throw BusError{};
        }
        return RAM + addr;
    case 4:
        // ROM
        return ROM + (addr & ROMMask);
    default:
        throw BusError{};
    }
}
uint8_t BusReadB(uint32_t addr) { return doBus<const uint8_t>(addr, 0); }
uint16_t BusReadW(uint32_t addr) { return doBus<const uint16_t>(addr, 0); }
uint32_t BusReadL(uint32_t addr) { return doBus<const uint32_t>(addr, 0); }
void BusWriteB(uint32_t addr, uint8_t v) { doBus<uint8_t>(addr, v); }
void BusWriteW(uint32_t addr, uint16_t v) { doBus<uint16_t>(addr, v); }
void BusWriteL(uint32_t addr, uint32_t v) { doBus<uint32_t>(addr, v); }

void Read16(uint32_t addr, uint8_t *to);
void Write16(uint32_t addr, const uint8_t *from);
