#ifndef IO_HPP_
#define IO_HPP_
#include <stdint.h>
#include <unordered_map>
#include <memory>

struct IO_BUS {
    virtual uint8_t Read8(uint32_t addr) = 0;
    virtual uint16_t Read16(uint32_t addr) {
        return Read8(addr) << 8 | Read8(addr+1);
    }
    virtual uint32_t Read32(uint32_t addr) {
        return Read16(addr) << 16 | Read16(addr + 2);
    }
    virtual void Write8(uint32_t addr, uint8_t value) = 0;
    virtual void Write16(uint32_t addr, uint16_t value) {
        Write8(addr, value >> 8);
        Write8(addr + 1, value);
    }
    virtual void Write32(uint32_t addr, uint32_t value) {
        Write16(addr, value >> 16);
        Write16(addr + 2, value);
    }
};

extern std::unique_ptr<IO_BUS> io;

// convinence wrapper
inline uint16_t SET_B1_W(uint16_t v, uint8_t b) {
    return (v & 0x00ff) | (b << 8);
}
inline uint16_t SET_B2_W(uint16_t v, uint8_t b) {
    return (v & 0xff00) | (b);
}

inline uint8_t GET_B1_W(uint16_t v) {
    return v >> 8;
}
inline uint8_t GET_B2_W(uint16_t v) {
    return v;
}

inline uint32_t SET_B1(uint32_t v, uint8_t b) {
    return (v & 0x00ffffff) | (b << 24);
}
inline uint32_t SET_B2(uint32_t v, uint8_t b) {
    return (v & 0xff00ffff) | (b << 16);
}
inline uint32_t SET_B3(uint32_t v, uint8_t b) {
    return (v & 0xffff00ff) | (b << 8);
}
inline uint32_t SET_B4(uint32_t v, uint8_t b) {
    return (v & 0xffffff00) | (b);
}
inline uint32_t SET_Bn(uint32_t v, uint8_t b, uint8_t n) {
    n = (3 - n)* 8;
    return (v &~ (0xff << n)) | (b << n);
}

inline uint8_t GET_B1(uint32_t v) {
    return v >> 24;
}
inline uint8_t GET_B2(uint32_t v) {
    return v >> 16;
}
inline uint8_t GET_B3(uint32_t v) {
    return v >> 8;
}
inline uint8_t GET_B4(uint32_t v) {
    return v;
}
inline uint8_t GET_Bn(uint32_t v, uint8_t n) {
    n = (3 - n)* 8;
    return v >> n;
}

#endif