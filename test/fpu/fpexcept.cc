#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_FP_exception {
    F_FP_exception() {
        // FBGT  #4
        TEST::SET_W(0, 0171200 | 0x12);
        TEST::SET_W(2, 4);
        TEST::SET_W(4, TEST_BREAK);
        TEST::SET_W(6, TEST_BREAK);

        // FMOVE.P (%A4), %FP2
        TEST::SET_W(8, 0171020 | 4);
        TEST::SET_W(10, 0B0000000 | 1 << 14 | 3 << 10 | 2 << 7);
        TEST::SET_W(12, TEST_BREAK);

        // FMOVE.L %FP3, (%A4)
        TEST::SET_W(14, 0171020 | 4);
        TEST::SET_W(16, 3 << 13 | 0 << 10 | 3 << 7);
        TEST::SET_W(18, TEST_BREAK);

        // FDIV %FP3, %FP2
        TEST::SET_W(20, 0171000);
        TEST::SET_W(22, 0B0100000 | 3 << 10 | 2 << 7);
        TEST::SET_W(24, TEST_BREAK);

        // FMUL %FP3, %FP2
        TEST::SET_W(26, 0171000);
        TEST::SET_W(28, 0B0100011 | 3 << 10 | 2 << 7);
        TEST::SET_W(30, TEST_BREAK);

        // FADD %FP3, %FP2
        TEST::SET_W(32, 0171000);
        TEST::SET_W(34, 0B0100010 | 3 << 10 | 2 << 7);
        TEST::SET_W(36, TEST_BREAK);

        // FABS %FP3, %FP2
        TEST::SET_W(38, 0171000);
        TEST::SET_W(40, 0B0100010 | 3 << 10 | 2 << 7);
        TEST::SET_W(42, TEST_BREAK);
        jit_compile(0, 44);
    }
};
BOOST_FIXTURE_TEST_SUITE(FP_exception, Prepare,
                         *boost::unit_test::fixture<F_FP_exception>())

BOOST_AUTO_TEST_CASE(BSUN) {
    cpu.FPCR.BSUN = true;
    cpu.FPSR.CC_NAN = true;
    run_test(0);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::FP_UNORDER);
}
BOOST_AUTO_TEST_CASE(INEX1) {
    cpu.FPCR.INEX1 = true;
    TEST::SET_L(0x100, 0xC0120001);
    TEST::SET_L(0x104, 0x33333333);
    TEST::SET_L(0x108, 0x33333333);
    cpu.A[4] = 0x100;
    run_test(8);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::FP_INEX);
}

BOOST_AUTO_TEST_CASE(INEX2) {
    cpu.FPCR.INEX2 = true;
    TEST::SET_FP(3, 1.1);
    cpu.A[4] = 0x100;
    run_test(14);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::FP_INEX);
}

BOOST_AUTO_TEST_CASE(DV0) {
    cpu.FPCR.DZ = true;
    TEST::SET_FP(3, 2.0);
    TEST::SET_FP(2, 0.0);
    run_test(20);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::FP_DIV0);
}

BOOST_AUTO_TEST_CASE(UNFL) {
    cpu.FPCR.UNFL = true;
    mpfr_set_si_2exp(cpu.FP[3], 1, -16385, MPFR_RNDN);
    mpfr_set_si_2exp(cpu.FP[2], 1, -16385, MPFR_RNDN);
    run_test(26);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::FP_UNFL);
}

BOOST_AUTO_TEST_CASE(OPERR) {
    cpu.FPCR.OPERR = true;
    TEST::SET_FP(3, copysign(INFINITY, 1));
    TEST::SET_FP(2, copysign(INFINITY, -1));
    run_test(32);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::FP_OPERR);
}

BOOST_AUTO_TEST_CASE(OVFL) {
    cpu.FPCR.OVFL = true;
    mpfr_set_ui_2exp(cpu.FP[3], 1, 16383, MPFR_RNDN);
    mpfr_set_ui_2exp(cpu.FP[2], 1, 16383, MPFR_RNDN);
    run_test(26);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::FP_OVFL);
}

BOOST_AUTO_TEST_CASE(SNAN_) {
    cpu.FPCR.S_NAN = true;
    mpfr_set_nan(cpu.FP[3]);
    cpu.FP_nan[3] = 0x0fff000000000000LLU;
    run_test(38);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::FP_SNAN);
}
BOOST_AUTO_TEST_SUITE_END()