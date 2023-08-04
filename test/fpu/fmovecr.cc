#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <math.h>
namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(FMOVECR, Prepare)

BOOST_AUTO_TEST_CASE(PI) {
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0x5C00 | 2 << 7);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(TEST::GET_FP(2) == M_PI);
}

BOOST_AUTO_TEST_CASE(Log10_2) {
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0x5C00 | 2 << 7 | 0x0B);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(TEST::GET_FP(2) == log10(2));
}

BOOST_AUTO_TEST_CASE(E) {
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0x5C00 | 2 << 7 | 0x0C);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(TEST::GET_FP(2) == M_E);
}

BOOST_AUTO_TEST_CASE(LOG2_E) {
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0x5C00 | 2 << 7 | 0x0D);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(TEST::GET_FP(2) == log2(M_E));
}

BOOST_AUTO_TEST_CASE(LOG10_E) {
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0x5C00 | 2 << 7 | 0x0E);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(TEST::GET_FP(2) == log10(M_E));
}

BOOST_AUTO_TEST_CASE(ZERO) {
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0x5C00 | 2 << 7 | 0x0F);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(TEST::GET_FP(2) == 0.0);
}

BOOST_AUTO_TEST_CASE(LN_2) {
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0x5C00 | 2 << 7 | 0x30);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(TEST::GET_FP(2) == log(2));
}

BOOST_AUTO_TEST_CASE(LN_10) {
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0x5C00 | 2 << 7 | 0x31);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(TEST::GET_FP(2) == log(10));
}
static int pw[] = {
    0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096
};
BOOST_DATA_TEST_CASE(POW10_n, pw ^ bdata::xrange(14), n, i) {
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0x5C00 | 2 << 7 | (0x32+i));
    BOOST_TEST(run_test() == 0);
    mpfr_log10(cpu.FP[2], cpu.FP[2], MPFR_RNDN);
    BOOST_TEST(TEST::GET_FP(2) == n);
}

BOOST_AUTO_TEST_SUITE_END()
