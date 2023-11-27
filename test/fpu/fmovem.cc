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
struct F_FMOVEM {
    F_FMOVEM() {
        // FMOVEM.X (%A3)+, %FP4-%FP7
        TEST::SET_W(0, 0171030 | 3);
        TEST::SET_W(2, 0140000 | 2 << 11 | 0B00001111);
        TEST::SET_W(4, TEST_BREAK);

        // FMOVEM.X (%A3), %FP4-%FP7
        TEST::SET_W(6, 0171020 | 3);
        TEST::SET_W(8, 0140000 | 2 << 11 | 0B00001111);
        TEST::SET_W(10, TEST_BREAK);

        // FMOVEM.X (%A3)+, %D4
        TEST::SET_W(12, 0171030 | 3);
        TEST::SET_W(14, 0140000 | 3 << 11 | 4 << 4);
        TEST::SET_W(16, TEST_BREAK);

        // FMOVEM.X (%A3), %D4
        TEST::SET_W(18, 0171020 | 3);
        TEST::SET_W(20, 0140000 | 3 << 11 | 4 << 4);
        TEST::SET_W(22, TEST_BREAK);

        // FMOVEM.X %FP0-%FP3, -(%A3)
        TEST::SET_W(24, 0171040 | 3);
        TEST::SET_W(26, 0160000 | 0 << 11 | 0B00001111);
        TEST::SET_W(28, TEST_BREAK);

        // FMOVEM.X %FP4-%FP7, (%A3)
        TEST::SET_W(30, 0171020 | 3);
        TEST::SET_W(32, 0160000 | 0 << 11 | 0B00001111);
        TEST::SET_W(34, TEST_BREAK);

        // FMOVEM.X %D2, -(%A3)
        TEST::SET_W(36, 0171040 | 3);
        TEST::SET_W(38, 0160000 | 1 << 11 | 2 << 4);
        TEST::SET_W(40, TEST_BREAK);

        // FMOVEM.X %D2, (%A3)
        TEST::SET_W(42, 0171020 | 3);
        TEST::SET_W(44, 0160000 | 3 << 11 | 4 << 4);
        TEST::SET_W(46, TEST_BREAK);

        // FMOVEM.L %FPCR/%FPSR/%FPIAR, -(%A3)
        TEST::SET_W(48, 0171040 | 3);
        TEST::SET_W(50, 0120000 | 7 << 10);
        TEST::SET_W(52, TEST_BREAK);

        // FMOVEM.L %FPCR/%FPSR, -(%A3)
        TEST::SET_W(54, 0171040 | 3);
        TEST::SET_W(56, 0120000 | 6 << 10);
        TEST::SET_W(58, TEST_BREAK);

        // FMOVEM.L %FPCR/%FPIAR, -(%A3)
        TEST::SET_W(60, 0171040 | 3);
        TEST::SET_W(62, 0120000 | 5 << 10);
        TEST::SET_W(64, TEST_BREAK);

        // FMOVEM.L %FPSR/%FPIAR, -(%A3)
        TEST::SET_W(66, 0171040 | 3);
        TEST::SET_W(68, 0120000 | 3 << 10);
        TEST::SET_W(70, TEST_BREAK);

        // FMOVEM.L (%A3)+, %FPCR/%FPSR/%FPIAR
        TEST::SET_W(72, 0171030 | 3);
        TEST::SET_W(74, 0100000 | 7 << 10);
        TEST::SET_W(76, TEST_BREAK);

        // FMOVEM.L (%A3)+, %FPCR/%FPSR
        TEST::SET_W(78, 0171030 | 3);
        TEST::SET_W(80, 0100000 | 6 << 10);
        TEST::SET_W(82, TEST_BREAK);

        // FMOVEM.L (%A3)+, %FPCR/%FPIAR
        TEST::SET_W(84, 0171030 | 3);
        TEST::SET_W(86, 0100000 | 5 << 10);
        TEST::SET_W(88, TEST_BREAK);

        // FMOVEM.L (%A3)+, %FPSR/%FPIAR
        TEST::SET_W(90, 0171030 | 3);
        TEST::SET_W(92, 0100000 | 3 << 10);
        TEST::SET_W(94, TEST_BREAK);

        jit_compile(0, 96);
    }
};
BOOST_FIXTURE_TEST_SUITE(FMOVEM, Prepare,
                         *boost::unit_test::fixture<F_FMOVEM>())
BOOST_AUTO_TEST_SUITE(ToReg)
BOOST_AUTO_TEST_CASE(trace_postincr) {
    cpu.A[3] = 0x100;
    cpu.T = 1;
    run_test(0);
    BOOST_TEST(cpu.ex_n == EXCAPTION_NUMBER::TRACE);
}
BOOST_AUTO_TEST_CASE(static_postincr) {
    cpu.A[3] = 0x100;
    STORE_MEMX(0x100, 1.1);
    STORE_MEMX(0x10C, 1.2);
    STORE_MEMX(0x118, 1.3);
    STORE_MEMX(0x124, 1.4);
    run_test(0);
    BOOST_TEST(TEST::GET_FP(4) == 1.1);
    BOOST_TEST(TEST::GET_FP(5) == 1.2);
    BOOST_TEST(TEST::GET_FP(6) == 1.3);
    BOOST_TEST(TEST::GET_FP(7) == 1.4);
    BOOST_TEST(cpu.A[3] == 0x130);
}

BOOST_AUTO_TEST_CASE(trace_addr) {
    cpu.A[3] = 0x100;
    cpu.T = 1;
    STORE_MEMX(0x100, 1.1);
    run_test(6);
    BOOST_TEST(cpu.ex_n == EXCAPTION_NUMBER::TRACE);
}
BOOST_AUTO_TEST_CASE(static_addr) {
    cpu.A[3] = 0x100;
    STORE_MEMX(0x100, 1.1);
    STORE_MEMX(0x10C, 1.2);
    STORE_MEMX(0x118, 1.3);
    STORE_MEMX(0x124, 1.4);
    run_test(6);
    BOOST_TEST(TEST::GET_FP(4) == 1.1);
    BOOST_TEST(TEST::GET_FP(5) == 1.2);
    BOOST_TEST(TEST::GET_FP(6) == 1.3);
    BOOST_TEST(TEST::GET_FP(7) == 1.4);
    BOOST_TEST(cpu.A[3] == 0x100);
}
BOOST_AUTO_TEST_CASE(dynamic_postincr) {
    cpu.A[3] = 0x100;
    cpu.D[4] = 0B01010101;
    STORE_MEMX(0x100, 1.1);
    STORE_MEMX(0x10C, 1.2);
    STORE_MEMX(0x118, 1.3);
    STORE_MEMX(0x124, 1.4);
    run_test(12);
    BOOST_TEST(TEST::GET_FP(1) == 1.1);
    BOOST_TEST(TEST::GET_FP(3) == 1.2);
    BOOST_TEST(TEST::GET_FP(5) == 1.3);
    BOOST_TEST(TEST::GET_FP(7) == 1.4);
    BOOST_TEST(cpu.A[3] == 0x130);
}

BOOST_AUTO_TEST_CASE(dynamic_addr) {
    cpu.A[3] = 0x100;
    cpu.D[4] = 0B01010101;
    STORE_MEMX(0x100, 1.1);
    STORE_MEMX(0x10C, 1.2);
    STORE_MEMX(0x118, 1.3);
    STORE_MEMX(0x124, 1.4);
    run_test(18);
    BOOST_TEST(TEST::GET_FP(1) == 1.1);
    BOOST_TEST(TEST::GET_FP(3) == 1.2);
    BOOST_TEST(TEST::GET_FP(5) == 1.3);
    BOOST_TEST(TEST::GET_FP(7) == 1.4);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(FromReg)
BOOST_AUTO_TEST_CASE(trace_predecr) {
    cpu.A[3] = 0x100;
    cpu.T = 1;
    run_test(24);
    BOOST_TEST(cpu.ex_n == EXCAPTION_NUMBER::TRACE);
}
BOOST_AUTO_TEST_CASE(static_predecr) {
    cpu.A[3] = 0x130;
    TEST::SET_FP(0, 1.1);
    TEST::SET_FP(1, 1.2);
    TEST::SET_FP(2, 1.3);
    TEST::SET_FP(3, 1.4);
    run_test(24);
    BOOST_TEST(LOAD_MEMX(0x124) == 1.4);
    BOOST_TEST(LOAD_MEMX(0x118) == 1.3);
    BOOST_TEST(LOAD_MEMX(0x10C) == 1.2);
    BOOST_TEST(LOAD_MEMX(0x100) == 1.1);
    BOOST_TEST(cpu.A[3] == 0x100);
}
BOOST_AUTO_TEST_CASE(trace_addr) {
    cpu.A[3] = 0x100;
    cpu.T = 1;
    run_test(30);
    BOOST_TEST(cpu.ex_n == EXCAPTION_NUMBER::TRACE);
}
BOOST_AUTO_TEST_CASE(static_addr) {
    cpu.A[3] = 0x100;
    TEST::SET_FP(4, 1.1);
    TEST::SET_FP(5, 1.2);
    TEST::SET_FP(6, 1.3);
    TEST::SET_FP(7, 1.4);
    run_test(30);
    BOOST_TEST(LOAD_MEMX(0x124) == 1.4);
    BOOST_TEST(LOAD_MEMX(0x118) == 1.3);
    BOOST_TEST(LOAD_MEMX(0x10C) == 1.2);
    BOOST_TEST(LOAD_MEMX(0x100) == 1.1);
    BOOST_TEST(cpu.A[3] == 0x100);
}
BOOST_AUTO_TEST_CASE(dynamic_predecr) {
    cpu.A[3] = 0x130;
    cpu.D[2] = 0B01010101;
    TEST::SET_FP(0, 1.1);
    TEST::SET_FP(2, 1.2);
    TEST::SET_FP(4, 1.3);
    TEST::SET_FP(6, 1.4);
    run_test(36);
    BOOST_TEST(LOAD_MEMX(0x124) == 1.4);
    BOOST_TEST(LOAD_MEMX(0x118) == 1.3);
    BOOST_TEST(LOAD_MEMX(0x10C) == 1.2);
    BOOST_TEST(LOAD_MEMX(0x100) == 1.1);
    BOOST_TEST(cpu.A[3] == 0x100);
}

BOOST_AUTO_TEST_CASE(dynamic_addr) {
    cpu.A[3] = 0x100;
    cpu.D[4] = 0B01010101;
    TEST::SET_FP(1, 1.1);
    TEST::SET_FP(3, 1.2);
    TEST::SET_FP(5, 1.3);
    TEST::SET_FP(7, 1.4);
    run_test(42);
    BOOST_TEST(LOAD_MEMX(0x124) == 1.4);
    BOOST_TEST(LOAD_MEMX(0x118) == 1.3);
    BOOST_TEST(LOAD_MEMX(0x10C) == 1.2);
    BOOST_TEST(LOAD_MEMX(0x100) == 1.1);
    BOOST_TEST(cpu.A[3] == 0x100);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(FromCR)
BOOST_AUTO_TEST_CASE(All) {
    cpu.A[3] = 0x10C;
    cpu.FPCR.OPERR = true;
    cpu.FPCR.DZ = true;
    cpu.FPCR.PREC = FPU_PREC::S;
    cpu.FPCR.RND = MPFR_RNDZ;

    cpu.FPSR.CC_Z = true;
    cpu.FPSR.QuatSign = true;
    cpu.FPSR.Quat = 22;
    cpu.FPSR.OVFL = true;
    cpu.FPSR.EXC_IOP = true;

    run_test(48);
    BOOST_TEST(TEST::GET_L(0x100) == 0x2450);
    BOOST_TEST(TEST::GET_L(0x104) == (0x4801080 | 22 << 16));
    BOOST_TEST(TEST::GET_L(0x108) == 48);
    BOOST_TEST(cpu.A[3] == 0x100);
}

BOOST_AUTO_TEST_CASE(CR_SR) {
    cpu.A[3] = 0x108;
    cpu.FPCR.OPERR = true;
    cpu.FPCR.DZ = true;
    cpu.FPCR.PREC = FPU_PREC::S;
    cpu.FPCR.RND = MPFR_RNDZ;

    cpu.FPSR.CC_Z = true;
    cpu.FPSR.QuatSign = true;
    cpu.FPSR.Quat = 22;
    cpu.FPSR.OVFL = true;
    cpu.FPSR.EXC_IOP = true;

    run_test(54);
    BOOST_TEST(TEST::GET_L(0x100) == 0x2450);
    BOOST_TEST(TEST::GET_L(0x104) == (0x4801080 | 22 << 16));
    BOOST_TEST(cpu.A[3] == 0x100);
}

BOOST_AUTO_TEST_CASE(CR_IAR) {
    cpu.A[3] = 0x108;
    cpu.FPCR.OPERR = true;
    cpu.FPCR.DZ = true;
    cpu.FPCR.PREC = FPU_PREC::S;
    cpu.FPCR.RND = MPFR_RNDZ;

    run_test(60);
    BOOST_TEST(TEST::GET_L(0x100) == 0x2450);
    BOOST_TEST(TEST::GET_L(0x104) == 60);
    BOOST_TEST(cpu.A[3] == 0x100);
}

BOOST_AUTO_TEST_CASE(SR_IAR) {
    cpu.A[3] = 0x108;
    cpu.FPSR.CC_Z = true;
    cpu.FPSR.QuatSign = true;
    cpu.FPSR.Quat = 22;
    cpu.FPSR.OVFL = true;
    cpu.FPSR.EXC_IOP = true;

    run_test(66);
    BOOST_TEST(TEST::GET_L(0x100) == (0x4801080 | 22 << 16));
    BOOST_TEST(TEST::GET_L(0x104) == 66);
    BOOST_TEST(cpu.A[3] == 0x100);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ToCR)

BOOST_AUTO_TEST_CASE(All) {

    TEST::SET_L(0x100, 0x2450);
    TEST::SET_L(0x104, 0x4801080 | 22 << 16);
    TEST::SET_L(0x108, 0x1000);

    cpu.A[3] = 0x100;

    run_test(72);
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

    BOOST_TEST(cpu.A[3] == 0x10C);
}
BOOST_AUTO_TEST_CASE(CR_SR) {

    TEST::SET_L(0x100, 0x2450);
    TEST::SET_L(0x104, 0x4801080 | 22 << 16);

    cpu.A[3] = 0x100;

    run_test(78);
    BOOST_TEST(cpu.FPCR.OPERR);
    BOOST_TEST(cpu.FPCR.DZ);
    BOOST_TEST(cpu.FPCR.PREC == FPU_PREC::S);
    BOOST_TEST(cpu.FPCR.RND == MPFR_RNDZ);

    BOOST_TEST(cpu.FPSR.CC_Z);
    BOOST_TEST(cpu.FPSR.QuatSign);
    BOOST_TEST(cpu.FPSR.Quat == 22);
    BOOST_TEST(cpu.FPSR.OVFL);
    BOOST_TEST(cpu.FPSR.EXC_IOP);

    BOOST_TEST(cpu.A[3] == 0x108);
}

BOOST_AUTO_TEST_CASE(CR_IAR) {

    TEST::SET_L(0x100, 0x2450);
    TEST::SET_L(0x104, 0x1000);

    cpu.A[3] = 0x100;

    run_test(84);
    BOOST_TEST(cpu.FPCR.OPERR);
    BOOST_TEST(cpu.FPCR.DZ);
    BOOST_TEST(cpu.FPCR.PREC == FPU_PREC::S);
    BOOST_TEST(cpu.FPCR.RND == MPFR_RNDZ);

    BOOST_TEST(cpu.FPIAR == 0x1000);

    BOOST_TEST(cpu.A[3] == 0x108);
}

BOOST_AUTO_TEST_CASE(SR_IAR) {

    TEST::SET_L(0x100, 0x4801080 | 22 << 16);
    TEST::SET_L(0x104, 0x1000);

    cpu.A[3] = 0x100;

    run_test(90);

    BOOST_TEST(cpu.FPSR.CC_Z);
    BOOST_TEST(cpu.FPSR.QuatSign);
    BOOST_TEST(cpu.FPSR.Quat == 22);
    BOOST_TEST(cpu.FPSR.OVFL);
    BOOST_TEST(cpu.FPSR.EXC_IOP);

    BOOST_TEST(cpu.FPIAR == 0x1000);

    BOOST_TEST(cpu.A[3] == 0x108);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
