#ifndef INCLUDE_TEST_
#define INCLUDE_TEST_ 1
#include <functional>
#include <stdint.h>
#include <string.h>
#include <utility>
#include <vector>
#include "mpfr.h"
extern std::vector<uint8_t> RAM;
void reset_fpu();
std::pair<std::function<void()>, int> decode();
struct Prepare {
    Prepare() ;
};
namespace TEST {
inline void SET_FP(int n, double v) {
    mpfr_set_d(cpu.FP[n], v, MPFR_RNDN);
}
inline double GET_FP(int n) {
    return mpfr_get_d(cpu.FP[n], MPFR_RNDN);
}
inline bool FP_TEST(int n, double vv, double eps = 1e-10) {
    MPFR_DECL_INIT(tmp, 64);
    MPFR_DECL_INIT(tmp2, 64);
    mpfr_set(tmp, cpu.FP[n], MPFR_RNDN);
    mpfr_set_d(tmp2, eps, MPFR_RNDN);
    mpfr_sub_d(tmp, tmp, vv, MPFR_RNDN);
    mpfr_abs(tmp, tmp, MPFR_RNDN);
    return mpfr_less_p(tmp, tmp2);
}   
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

 int decode_and_run() ;

void qnan_test(uint16_t op) ;
void snan_test(uint16_t op) ;
int GET_EXCEPTION() ;
#endif