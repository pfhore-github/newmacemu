#ifndef INCLUDE_TEST_
#define INCLUDE_TEST_ 1
#include "mpfr.h"
#include "io.hpp"
#include <functional>
#include <stdint.h>
#include <string.h>
#include <utility>
#include <vector>
#include <deque>
extern std::vector<uint8_t> RAM;
void reset_fpu();
std::string disasm();

struct Prepare {
    Prepare();
};
namespace TEST {
inline void SET_FP(int n, double v) { mpfr_set_d(cpu.FP[n], v, MPFR_RNDN); }
inline double GET_FP(int n) { return mpfr_get_d(cpu.FP[n], MPFR_RNDN); }

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
extern const double sg_v[2];
extern const mpfr_rnd_t RND_MODES[4];

int run_test();

void qnan_test(uint16_t op);
void snan_test(uint16_t op);
int GET_EXCEPTION();

void test_rom(
    uint32_t from, uint32_t to,
    const std::unordered_map<uint32_t, std::function<void()>> &testStubs,
    bool trace = false);

extern std::vector<uint32_t> called;
void called_impl();
extern bool failure_is_exception;
struct DummyIO_B  {
    std::deque<std::pair<uint32_t, uint8_t>> read_expected;
    std::deque<std::pair<uint32_t, uint8_t>> write_expected;
    uint8_t read(uint32_t addr);
    void write(uint32_t addr, uint8_t v);
};
#endif