#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <math.h>
namespace bdata = boost::unit_test::data;
std::ostream &operator<<(std::ostream &os, FPU_PREC f);
static double LOAD_MEMX(uint32_t addr) {
    int16_t exp = TEST::GET_W(addr) - 16383 - 63;
    uint64_t frac = static_cast<uint64_t>(TEST::GET_L(addr + 4)) << 32 |
                    TEST::GET_L(addr + 8);
    MPFR_DECL_INIT(tmp, 64);
    mpfr_set_uj_2exp(tmp, frac, exp, cpu.FPCR.RND);
    return mpfr_get_d(tmp, MPFR_RNDN);
}

static void STORE_MEMX(uint32_t addr, double value) {
    MPFR_DECL_INIT(tmp, 64);
    mpfr_set_d(tmp, value, MPFR_RNDN);
    mpfr_exp_t exp;
    mpfr_frexp(&exp, tmp, tmp, MPFR_RNDN);
    exp--;
    mpfr_mul_2si(tmp, tmp, 64, cpu.FPCR.RND);
    uint64_t frac = mpfr_get_uj(tmp, cpu.FPCR.RND);
    TEST::SET_W(addr, exp + 16383);
    TEST::SET_L(addr + 4, frac >> 32);
    TEST::SET_L(addr + 8, frac);
}
BOOST_FIXTURE_TEST_SUITE(FMOVEM, Prepare)
BOOST_AUTO_TEST_SUITE(ToReg)
BOOST_AUTO_TEST_CASE(trace_postincr) {
    TEST::SET_W(0, 0171030 | 3);
    TEST::SET_W(2, 0140000 | 2 << 11 | 1);
    cpu.A[3] = 0x1000;
    cpu.T = 1;
    STORE_MEMX(0x1000, 1.1);
    BOOST_TEST(run_test() == 9);
}
BOOST_AUTO_TEST_CASE(static_postincr) {
    TEST::SET_W(0, 0171030 | 3);
    TEST::SET_W(2, 0140000 | 2 << 11 | 0B01010101);
    cpu.A[3] = 0x1000;
    STORE_MEMX(0x1000, 1.1);
    STORE_MEMX(0x100C, 1.2);
    STORE_MEMX(0x1018, 1.3);
    STORE_MEMX(0x1024, 1.4);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(TEST::GET_FP(1) == 1.1);
    BOOST_TEST(TEST::GET_FP(3) == 1.2);
    BOOST_TEST(TEST::GET_FP(5) == 1.3);
    BOOST_TEST(TEST::GET_FP(7) == 1.4);
    BOOST_TEST(cpu.A[3] == 0x1030);
}

BOOST_AUTO_TEST_CASE(trace_addr) {
    TEST::SET_W(0, 0171020 | 3);
    TEST::SET_W(2, 0140000 | 2 << 11 | 1);
    cpu.A[3] = 0x1000;
    cpu.T = 1;
    STORE_MEMX(0x1000, 1.1);
    BOOST_TEST(run_test() == 9);
}
BOOST_AUTO_TEST_CASE(static_addr) {
    TEST::SET_W(0, 0171020 | 3);
    TEST::SET_W(2, 0140000 | 2 << 11 | 0B01010101);
    cpu.A[3] = 0x1000;
    STORE_MEMX(0x1000, 1.1);
    STORE_MEMX(0x100C, 1.2);
    STORE_MEMX(0x1018, 1.3);
    STORE_MEMX(0x1024, 1.4);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(TEST::GET_FP(1) == 1.1);
    BOOST_TEST(TEST::GET_FP(3) == 1.2);
    BOOST_TEST(TEST::GET_FP(5) == 1.3);
    BOOST_TEST(TEST::GET_FP(7) == 1.4);
    BOOST_TEST(cpu.A[3] == 0x1000);
}

BOOST_AUTO_TEST_CASE(dynamic_postincr) {
    TEST::SET_W(0, 0171030 | 3);
    TEST::SET_W(2, 0140000 | 3 << 11 | 4 << 4);
    cpu.A[3] = 0x1000;
    cpu.D[4] = 0B01010101;
    STORE_MEMX(0x1000, 1.1);
    STORE_MEMX(0x100C, 1.2);
    STORE_MEMX(0x1018, 1.3);
    STORE_MEMX(0x1024, 1.4);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(TEST::GET_FP(1) == 1.1);
    BOOST_TEST(TEST::GET_FP(3) == 1.2);
    BOOST_TEST(TEST::GET_FP(5) == 1.3);
    BOOST_TEST(TEST::GET_FP(7) == 1.4);
    BOOST_TEST(cpu.A[3] == 0x1030);
}

BOOST_AUTO_TEST_CASE(dynamic_addr) {
    TEST::SET_W(0, 0171020 | 3);
    TEST::SET_W(2, 0140000 | 3 << 11 | 4 << 4);
    cpu.A[3] = 0x1000;
    cpu.D[4] = 0B01010101;
    STORE_MEMX(0x1000, 1.1);
    STORE_MEMX(0x100C, 1.2);
    STORE_MEMX(0x1018, 1.3);
    STORE_MEMX(0x1024, 1.4);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(TEST::GET_FP(1) == 1.1);
    BOOST_TEST(TEST::GET_FP(3) == 1.2);
    BOOST_TEST(TEST::GET_FP(5) == 1.3);
    BOOST_TEST(TEST::GET_FP(7) == 1.4);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(FromReg)
BOOST_AUTO_TEST_CASE(trace_predecr) {
    TEST::SET_W(0, 0171040 | 3);
    TEST::SET_W(2, 0160000 | 0 << 11 | 1);
    cpu.A[3] = 0x1000;
    cpu.T = 1;
    TEST::SET_FP(0, 1.1);
    BOOST_TEST(run_test() == 9);
}
BOOST_AUTO_TEST_CASE(static_predecr) {
    TEST::SET_W(0, 0171040 | 3);
    TEST::SET_W(2, 0160000 | 0 << 11 | 0B01010101);
    cpu.A[3] = 0x1030;
    TEST::SET_FP(0, 1.1);
    TEST::SET_FP(2, 1.2);
    TEST::SET_FP(4, 1.3);
    TEST::SET_FP(6, 1.4);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(LOAD_MEMX(0x1024) == 1.4);
    BOOST_TEST(LOAD_MEMX(0x1018) == 1.3);
    BOOST_TEST(LOAD_MEMX(0x100C) == 1.2);
    BOOST_TEST(LOAD_MEMX(0x1000) == 1.1);
    BOOST_TEST(cpu.A[3] == 0x1000);
}
BOOST_AUTO_TEST_CASE(trace_addr) {
    TEST::SET_W(0, 0171020 | 3);
    TEST::SET_W(2, 0160000 | 0 << 11 | 1);
    cpu.A[3] = 0x1000;
    cpu.T = 1;
    TEST::SET_FP(0, 1.1);
    BOOST_TEST(run_test() == 9);
}
BOOST_AUTO_TEST_CASE(static_addr) {
    TEST::SET_W(0, 0171020 | 3);
    TEST::SET_W(2, 0160000 | 2 << 11 | 0B01010101);
    cpu.A[3] = 0x1000;
    TEST::SET_FP(1, 1.1);
    TEST::SET_FP(3, 1.2);
    TEST::SET_FP(5, 1.3);
    TEST::SET_FP(7, 1.4);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(LOAD_MEMX(0x1024) == 1.4);
    BOOST_TEST(LOAD_MEMX(0x1018) == 1.3);
    BOOST_TEST(LOAD_MEMX(0x100C) == 1.2);
    BOOST_TEST(LOAD_MEMX(0x1000) == 1.1);
    BOOST_TEST(cpu.A[3] == 0x1000);
}

BOOST_AUTO_TEST_CASE(dynamic_predecr) {
    TEST::SET_W(0, 0171040 | 3);
    TEST::SET_W(2, 0160000 | 1 << 11 | 2 << 4);
    cpu.A[3] = 0x1030;
    cpu.D[2] = 0B01010101;
    TEST::SET_FP(0, 1.1);
    TEST::SET_FP(2, 1.2);
    TEST::SET_FP(4, 1.3);
    TEST::SET_FP(6, 1.4);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(LOAD_MEMX(0x1024) == 1.4);
    BOOST_TEST(LOAD_MEMX(0x1018) == 1.3);
    BOOST_TEST(LOAD_MEMX(0x100C) == 1.2);
    BOOST_TEST(LOAD_MEMX(0x1000) == 1.1);
    BOOST_TEST(cpu.A[3] == 0x1000);
}

BOOST_AUTO_TEST_CASE(dynamic_addr) {
    TEST::SET_W(0, 0171020 | 3);
    TEST::SET_W(2, 0160000 | 3 << 11 | 4 << 4);
    cpu.A[3] = 0x1000;
    cpu.D[4] = 0B01010101;
    TEST::SET_FP(1, 1.1);
    TEST::SET_FP(3, 1.2);
    TEST::SET_FP(5, 1.3);
    TEST::SET_FP(7, 1.4);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(LOAD_MEMX(0x1024) == 1.4);
    BOOST_TEST(LOAD_MEMX(0x1018) == 1.3);
    BOOST_TEST(LOAD_MEMX(0x100C) == 1.2);
    BOOST_TEST(LOAD_MEMX(0x1000) == 1.1);
    BOOST_TEST(cpu.A[3] == 0x1000);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(FromCR)

BOOST_AUTO_TEST_CASE(All) {
    TEST::SET_W(0x2000, 0171040 | 3);
    TEST::SET_W(0x2002, 0120000 | 7 << 10);
    cpu.A[3] = 0x100C;
    cpu.FPCR.OPERR = true;
    cpu.FPCR.DZ = true;
    cpu.FPCR.PREC = FPU_PREC::S;
    cpu.FPCR.RND = MPFR_RNDZ;

    cpu.FPSR.CC_Z = true;
    cpu.FPSR.QuatSign = true;
    cpu.FPSR.Quat = 22;
    cpu.FPSR.OVFL = true;
    cpu.FPSR.EXC_IOP = true;

    cpu.PC = 0x2000;

    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 0x2004);
    BOOST_TEST(TEST::GET_L(0x1000) == 0x2450);
    BOOST_TEST(TEST::GET_L(0x1004) == (0x4801080 | 22 << 16));
    BOOST_TEST(TEST::GET_L(0x1008) == 0x2000);
    BOOST_TEST(cpu.A[3] == 0x1000);
}

BOOST_AUTO_TEST_CASE(CR_SR) {
    TEST::SET_W(0, 0171040 | 3);
    TEST::SET_W(2, 0120000 | 6 << 10);
    cpu.A[3] = 0x1008;
    cpu.FPCR.OPERR = true;
    cpu.FPCR.DZ = true;
    cpu.FPCR.PREC = FPU_PREC::S;
    cpu.FPCR.RND = MPFR_RNDZ;

    cpu.FPSR.CC_Z = true;
    cpu.FPSR.QuatSign = true;
    cpu.FPSR.Quat = 22;
    cpu.FPSR.OVFL = true;
    cpu.FPSR.EXC_IOP = true;

    BOOST_TEST(run_test() == 0);
    BOOST_TEST(TEST::GET_L(0x1000) == 0x2450);
    BOOST_TEST(TEST::GET_L(0x1004) == (0x4801080 | 22 << 16));
    BOOST_TEST(cpu.A[3] == 0x1000);
}

BOOST_AUTO_TEST_CASE(CR_IAR) {
    TEST::SET_W(0x2000, 0171040 | 3);
    TEST::SET_W(0x2002, 0120000 | 5 << 10);
    cpu.A[3] = 0x1008;
    cpu.FPCR.OPERR = true;
    cpu.FPCR.DZ = true;
    cpu.FPCR.PREC = FPU_PREC::S;
    cpu.FPCR.RND = MPFR_RNDZ;

    cpu.FPSR.CC_Z = true;
    cpu.FPSR.QuatSign = true;
    cpu.FPSR.Quat = 22;
    cpu.FPSR.OVFL = true;
    cpu.FPSR.EXC_IOP = true;

    cpu.PC = 0x2000;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(TEST::GET_L(0x1000) == 0x2450);
    BOOST_TEST(TEST::GET_L(0x1004) == 0x2000);
    BOOST_TEST(cpu.A[3] == 0x1000);
}

BOOST_AUTO_TEST_CASE(SR_IAR) {
    TEST::SET_W(0x2000, 0171040 | 3);
    TEST::SET_W(0x2002, 0120000 | 3 << 10);
    cpu.A[3] = 0x1008;
    cpu.FPCR.OPERR = true;
    cpu.FPCR.DZ = true;
    cpu.FPCR.PREC = FPU_PREC::S;
    cpu.FPCR.RND = MPFR_RNDZ;

    cpu.FPSR.CC_Z = true;
    cpu.FPSR.QuatSign = true;
    cpu.FPSR.Quat = 22;
    cpu.FPSR.OVFL = true;
    cpu.FPSR.EXC_IOP = true;

    cpu.PC = 0x2000;

    BOOST_TEST(run_test() == 0);
    BOOST_TEST(TEST::GET_L(0x1000) == (0x4801080 | 22 << 16));
    BOOST_TEST(TEST::GET_L(0x1004) == 0x2000);
    BOOST_TEST(cpu.A[3] == 0x1000);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ToCR)

BOOST_AUTO_TEST_CASE(All) {
    TEST::SET_W(0, 0171030 | 3);
    TEST::SET_W(2, 0100000 | 7 << 10);

    TEST::SET_L(0x1000, 0x2450);
    TEST::SET_L(0x1004, 0x4801080 | 22 << 16);
    TEST::SET_L(0x1008, 0x1000);

    cpu.A[3] = 0x1000;

    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(cpu.FPCR.OPERR);
    BOOST_TEST(cpu.FPCR.DZ);
    BOOST_TEST(cpu.FPCR.PREC == FPU_PREC::S);
    BOOST_TEST(cpu.FPCR.RND == MPFR_RNDZ);

    BOOST_TEST(cpu.FPSR.CC_Z);
    BOOST_TEST(cpu.FPSR.QuatSign);
    BOOST_TEST(cpu.FPSR.Quat == 22);
    BOOST_TEST(cpu.FPSR.OVFL);
    BOOST_TEST(cpu.FPSR.EXC_IOP);

    BOOST_TEST(cpu.FPIAR == 0x1000);

    BOOST_TEST(cpu.A[3] == 0x100C);
}

BOOST_AUTO_TEST_CASE(CR_SR) {
    TEST::SET_W(0, 0171030 | 3);
    TEST::SET_W(2, 0100000 | 6 << 10);

    TEST::SET_L(0x1000, 0x2450);
    TEST::SET_L(0x1004, 0x4801080 | 22 << 16);

    cpu.A[3] = 0x1000;

    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.FPCR.OPERR);
    BOOST_TEST(cpu.FPCR.DZ);
    BOOST_TEST(cpu.FPCR.PREC == FPU_PREC::S);
    BOOST_TEST(cpu.FPCR.RND == MPFR_RNDZ);

    BOOST_TEST(cpu.FPSR.CC_Z);
    BOOST_TEST(cpu.FPSR.QuatSign);
    BOOST_TEST(cpu.FPSR.Quat == 22);
    BOOST_TEST(cpu.FPSR.OVFL);
    BOOST_TEST(cpu.FPSR.EXC_IOP);

    BOOST_TEST(cpu.A[3] == 0x1008);
}

BOOST_AUTO_TEST_CASE(CR_IAR) {
    TEST::SET_W(0, 0171030 | 3);
    TEST::SET_W(2, 0100000 | 5 << 10);

    TEST::SET_L(0x1000, 0x2450);
    TEST::SET_L(0x1004, 0x1000);

    cpu.A[3] = 0x1000;

    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.FPCR.OPERR);
    BOOST_TEST(cpu.FPCR.DZ);
    BOOST_TEST(cpu.FPCR.PREC == FPU_PREC::S);
    BOOST_TEST(cpu.FPCR.RND == MPFR_RNDZ);

    BOOST_TEST(cpu.FPIAR == 0x1000);

    BOOST_TEST(cpu.A[3] == 0x1008);
}

BOOST_AUTO_TEST_CASE(SR_IAR) {
    TEST::SET_W(0, 0171030 | 3);
    TEST::SET_W(2, 0100000 | 3 << 10);

    TEST::SET_L(0x1000, 0x4801080 | 22 << 16);
    TEST::SET_L(0x1004, 0x1000);

    cpu.A[3] = 0x1000;

    BOOST_TEST(run_test() == 0);

    BOOST_TEST(cpu.FPSR.CC_Z);
    BOOST_TEST(cpu.FPSR.QuatSign);
    BOOST_TEST(cpu.FPSR.Quat == 22);
    BOOST_TEST(cpu.FPSR.OVFL);
    BOOST_TEST(cpu.FPSR.EXC_IOP);

    BOOST_TEST(cpu.FPIAR == 0x1000);

    BOOST_TEST(cpu.A[3] == 0x1008);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
