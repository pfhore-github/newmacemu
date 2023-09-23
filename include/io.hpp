#ifndef IO_HPP_
#define IO_HPP_
#include <stdint.h>
#include <unordered_map>
#include <memory>
struct CHIP_B {
    virtual uint8_t read(uint32_t addr) = 0;
    virtual void write(uint32_t addr, uint8_t v) = 0;
};
struct CHIP_L {
    virtual uint32_t read(uint32_t addr) = 0;
    virtual void write(uint32_t addr, uint32_t v) = 0;
};

extern std::unordered_map<int, std::shared_ptr<CHIP_B>> chipsB;
extern std::unordered_map<int, std::shared_ptr<CHIP_L>> chipsL;
uint8_t LoadIO_B(uint32_t addr);
uint32_t LoadIO_L(uint32_t addr);
void StoreIO_B(uint32_t addr, uint8_t b);
void StoreIO_L(uint32_t addr, uint32_t l);
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
#endif