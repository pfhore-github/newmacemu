#ifndef INCLUDE_TEST_
#define INCLUDE_TEST_ 1
#include <vector>
#include <stdint.h>
#include <functional>
#include <utility>
extern std::vector<uint8_t> RAM;
std::pair<std::function<void()>, int> decode();
namespace TEST {
inline uint16_t GET_W(uint32_t addr) {
    return RAM[addr] << 8 | RAM[addr+1];
}
inline uint32_t GET_L(uint32_t addr) {
    return RAM[addr] << 24 | RAM[addr+1] << 16 | RAM[addr+2] << 8 | RAM[addr+3];
}
inline void SET_W(uint32_t addr, uint16_t v) {
    RAM[addr] = v >> 8;
    RAM[addr+1] = v;
}
inline void SET_L(uint32_t addr, uint32_t v) {
    RAM[addr] = v >> 24;
    RAM[addr+1] = v >> 16;
    RAM[addr+2] = v>> 8;
    RAM[addr+3] = v;
}
}
#endif