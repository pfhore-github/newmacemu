#ifndef IO_HPP_
#define IO_HPP_
#include "bus.hpp"
#include <memory>
#include <stdint.h>
#include <unordered_map>
struct IO_BUS {
    virtual uint8_t readB(uint32_t /* addr */) { throw BusError{}; }
    virtual void writeB(uint32_t /* addr */, uint8_t /* value */) { throw BusError{}; }
    virtual uint16_t readW(uint32_t addr) {
        uint16_t v = readB(addr) << 8;
        return v | readB(addr + 1);
    }
    virtual void writeW(uint32_t addr, uint16_t value) {
        writeB(addr, value >> 8);
        writeB(addr + 1, value);
    }
    virtual uint32_t readL(uint32_t addr) {
        uint32_t v = readB(addr) << 24;
        v |= readB(addr+1) << 16;
        v |= readB(addr+2) << 8;
        return v | readB(addr + 3);
    }
    virtual void writeL(uint32_t addr, uint32_t value) {
        writeB(addr, value >> 24);
        writeB(addr + 1, value >> 16);
        writeB(addr + 2, value >> 16);
        writeB(addr + 3, value >> 16);
    }
};

extern std::unique_ptr<IO_BUS> io;

// convinence wrapper
inline uint16_t SET_B1_W(uint16_t v, uint8_t b) {
    return (v & 0x00ff) | (b << 8);
}
inline uint16_t SET_B2_W(uint16_t v, uint8_t b) { return (v & 0xff00) | (b); }

inline uint8_t GET_B1_W(uint16_t v) { return v >> 8; }
inline uint8_t GET_B2_W(uint16_t v) { return v; }

inline uint32_t SET_B1(uint32_t v, uint8_t b) {
    return (v & 0x00ffffff) | (b << 24);
}
inline uint32_t SET_B2(uint32_t v, uint8_t b) {
    return (v & 0xff00ffff) | (b << 16);
}
inline uint32_t SET_B3(uint32_t v, uint8_t b) {
    return (v & 0xffff00ff) | (b << 8);
}
inline uint32_t SET_B4(uint32_t v, uint8_t b) { return (v & 0xffffff00) | (b); }
inline uint32_t SET_Bn(uint32_t v, uint8_t b, uint8_t n) {
    n = (3 - n) * 8;
    return (v & ~(0xff << n)) | (b << n);
}

inline uint8_t GET_B1(uint32_t v) { return v >> 24; }
inline uint8_t GET_B2(uint32_t v) { return v >> 16; }
inline uint8_t GET_B3(uint32_t v) { return v >> 8; }
inline uint8_t GET_B4(uint32_t v) { return v; }
inline uint8_t GET_Bn(uint32_t v, uint8_t n) {
    n = (3 - n) * 8;
    return v >> n;
}

#endif