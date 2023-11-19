#include <memory>

#include "68040.hpp"
#include "bus.hpp"
#include "exception.hpp"
#include "io.hpp"
#include "memory.hpp"
struct IOMap {};
template <class T> struct bus_trait {};
template <> struct bus_trait<const uint8_t> {
    static uint8_t memDo(const uint8_t *p, uint8_t) { return *p; }
    static uint8_t ioDo(const uint32_t addr, uint8_t) {
        return io->Read8(addr & 0x0FFFFFFF);
    }
};
template <> struct bus_trait<uint8_t> {
    static void memDo(uint8_t *p, uint8_t v) { *p = v; }
    static void ioDo(const uint32_t addr, uint8_t v) {
        io->Write8(addr & 0x0FFFFFFF, v);
    }
};

template <> struct bus_trait<const uint16_t> {
    static uint16_t memDo(const uint8_t *p, uint16_t) { return readBE16(p); }
    static uint16_t ioDo(const uint32_t addr, uint16_t) {
        return io->Read16(addr & 0x0FFFFFFF);
    }
};
template <> struct bus_trait<uint16_t> {
    static void memDo(uint8_t *p, uint16_t v) { writeBE16(p, v); }
    static void ioDo(const uint32_t addr, uint16_t v) {
        io->Write16(addr & 0x0FFFFFFF, v);
    }
};

template <> struct bus_trait<const uint32_t> {
    static uint32_t memDo(const uint8_t *p, uint32_t) { return readBE32(p); }
    static uint32_t ioDo(const uint32_t addr, uint32_t) {
        return io->Read32(addr & 0x0FFFFFFF);
    }
};
template <> struct bus_trait<uint32_t> {
    static void memDo(uint8_t *p, uint32_t v) { writeBE32(p, v); }
    static void ioDo(const uint32_t addr, uint32_t v) {
        io->Write32(addr & 0x0FFFFFFF, v);
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
void BusWriteB(uint32_t addr, uint8_t v) { doBus<uint8_t>(addr, v); }

uint16_t BusReadW(uint32_t addr) { return doBus<const uint16_t>(addr, 0); }
void BusWriteW(uint32_t addr, uint16_t v) { doBus<uint16_t>(addr, v); }

uint32_t BusReadL(uint32_t addr) { return doBus<const uint32_t>(addr, 0); }
void BusWriteL(uint32_t addr, uint32_t v) { doBus<uint32_t>(addr, v); }

void Read16(uint32_t addr, uint8_t *to);
void Write16(uint32_t addr, const uint8_t *from) ;

void MMU_Transfer16(uint32_t from, uint32_t to) {
    uint8_t buf[16];
    Read16(from & ~0xf, buf);
    Write16(to & ~0xf, buf);
}
