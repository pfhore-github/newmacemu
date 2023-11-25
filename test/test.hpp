#ifndef INCLUDE_TEST_
#define INCLUDE_TEST_ 1
#include "io.hpp"
#include "memory.hpp"
#include "mpfr.h"
#include "jit.hpp"
#include <deque>
#include <functional>
#include <stdint.h>
#include <string.h>
#include <utility>
#include <vector>
void reset_fpu();
constexpr uint32_t TEST_BREAK = 0044117;
struct Prepare {
    Prepare();
};
namespace TEST {
inline void SET_FP(int n, double v) { mpfr_set_d(cpu.FP[n], v, MPFR_RNDN); }
inline double GET_FP(int n) { return mpfr_get_d(cpu.FP[n], MPFR_RNDN); }

inline uint16_t GET_W(uint32_t addr) { return readBE16(RAM + addr); }
inline uint32_t GET_L(uint32_t addr) {return readBE32(RAM + addr); }
inline void SET_W(uint32_t addr, uint16_t v) { writeBE16(RAM+ addr, v); }
inline void SET_L(uint32_t addr, uint32_t v) { writeBE32(RAM+ addr, v); }
} // namespace TEST
extern const double sg_v[2];
extern const mpfr_rnd_t RND_MODES[4];

void run_test(uint32_t pc);

void qnan_test(uint32_t pc);
void snan_test(uint32_t pc);

void test_rom(
    uint32_t from, uint32_t to,
    const std::unordered_map<uint32_t, std::function<void()>> &testStubs,
    bool trace = false);

extern std::vector<uint32_t> called;
void called_impl();
extern bool failure_is_exception;
void jit_run_test(uint32_t pc);
struct DummyIO_B {
    std::deque<std::pair<uint32_t, uint8_t>> read_expected;
    std::deque<std::pair<uint32_t, uint8_t>> write_expected;
    uint8_t read(uint32_t addr);
    void write(uint32_t addr, uint8_t v);
};
#endif