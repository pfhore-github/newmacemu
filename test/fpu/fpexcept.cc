#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(FP_exception, Prepare)

BOOST_AUTO_TEST_CASE(BSUN) {
    cpu.FPCR.BSUN = true;
    TEST::SET_W(0, 0171200 | 0x12);
    TEST::SET_W(2, 0x100);
    cpu.FPSR.CC_NAN = true;
    BOOST_TEST(run_test() == 48);
}

BOOST_AUTO_TEST_CASE(INEX1) {
    cpu.FPCR.INEX1 = true;
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 0B0000000 | 1 << 14 | 3 << 10 | 2 << 7);
    TEST::SET_L(0x1000, 0xC0120001);
    TEST::SET_L(0x1004, 0x33333333);
    TEST::SET_L(0x1008, 0x33333333);
    cpu.A[4] = 0x1000;
    BOOST_TEST(run_test() == 49);
}

BOOST_AUTO_TEST_CASE(INEX2) {
    cpu.FPCR.INEX2 = true;
    TEST::SET_FP(3, 1.1);
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 0 << 10 | 3 << 7);
    cpu.A[4] = 0x1000;
    BOOST_TEST(run_test() == 49);
}

BOOST_AUTO_TEST_CASE(DV0) {
    cpu.FPCR.DZ = true;
    TEST::SET_FP(3, 2.0);
    TEST::SET_FP(2, copysign(0.0, 1));
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0100000 | 3 << 10 | 2 << 7);

    BOOST_TEST(run_test() == 50);
}

BOOST_AUTO_TEST_CASE(UNFL) {
    cpu.FPCR.UNFL = true;
    mpfr_set_si_2exp(cpu.FP[3], 1, -16385, MPFR_RNDN);
    mpfr_set_si_2exp(cpu.FP[2], 1, -16385, MPFR_RNDN);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0100011 | 3 << 10 | 2 << 7);
    BOOST_TEST(run_test() == 51);
}

BOOST_AUTO_TEST_CASE(OPERR) {
    cpu.FPCR.OPERR = true;
    TEST::SET_FP(3, copysign(INFINITY, 1));
    TEST::SET_FP(2, copysign(INFINITY, -1));
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0100010 | 3 << 10 | 2 << 7);
    BOOST_TEST(run_test() == 52);
}

BOOST_AUTO_TEST_CASE(OVFL) {
    cpu.FPCR.OVFL = true;
    mpfr_set_ui_2exp(cpu.FP[3], 1, 16383, MPFR_RNDN);
    mpfr_set_ui_2exp(cpu.FP[2], 1, 16383, MPFR_RNDN);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0100011 | 3 << 10 | 2 << 7);
    BOOST_TEST(run_test() == 53);
}

BOOST_AUTO_TEST_CASE(SNAN_) {
    cpu.FPCR.S_NAN = true;
    mpfr_set_nan(cpu.FP[3]);
    cpu.FP_nan[3] = 0x0fff000000000000LLU;
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B1011000 | 3 << 10 | 2 << 7);
    BOOST_TEST(run_test() == 54);
}
BOOST_AUTO_TEST_SUITE_END()