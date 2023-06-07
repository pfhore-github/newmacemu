#ifndef INCLUDE_TEST_
#define INCLUDE_TEST_ 1
#include <functional>
#include <stdint.h>
#include <string.h>
#include <utility>
#include <vector>
extern std::vector<uint8_t> RAM;
void reset_fpu();
std::pair<std::function<void()>, int> decode();
struct Prepare {
    Prepare() {
        reset_fpu();
        for(int i = 0; i < 8; ++i) {
            cpu.D[i] = cpu.A[i] = 0;
        }
        cpu.Z = cpu.X = cpu.V = cpu.C = cpu.N = false;
        memset(RAM.data(), 0, 4096);
        cpu.PC = 0;
    }
};
namespace TEST {
inline uint16_t GET_W(uint32_t addr) { return RAM[addr] << 8 | RAM[addr + 1]; }
inline uint32_t GET_L(uint32_t addr) {
    return RAM[addr] << 24 | RAM[addr + 1] << 16 | RAM[addr + 2] << 8 |
           RAM[addr + 3];
}
inline void SET_W(uint32_t addr, uint16_t v) {
    RAM[addr] = v >> 8;
    RAM[addr + 1] = v;
}
inline void SET_L(uint32_t addr, uint32_t v) {
    RAM[addr] = v >> 24;
    RAM[addr + 1] = v >> 16;
    RAM[addr + 2] = v >> 8;
    RAM[addr + 3] = v;
}
} // namespace TEST
inline int decode_and_run() {
    auto [f, i] = decode();
    f();
    return i;
}
#endif