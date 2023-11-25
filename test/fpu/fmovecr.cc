#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <math.h>
namespace bdata = boost::unit_test::data;

struct F_FMOVECR {
    F_FMOVECR() {
        // FMOVECR #0, %FP2
        TEST::SET_W(0, 0171000);
        TEST::SET_W(2, 0x5C00 | 2 << 7);
        TEST::SET_W(4, TEST_BREAK);

        // FMOVECR #0x0B, %FP2
        TEST::SET_W(6, 0171000);
        TEST::SET_W(8, 0x5C00 | 2 << 7 | 0x0B);
        TEST::SET_W(10, TEST_BREAK);

        // FMOVECR #0x0C, %FP2
        TEST::SET_W(12, 0171000);
        TEST::SET_W(14, 0x5C00 | 2 << 7 | 0x0C);
        TEST::SET_W(16, TEST_BREAK);

        // FMOVECR #0x0D, %FP2
        TEST::SET_W(18, 0171000);
        TEST::SET_W(20, 0x5C00 | 2 << 7 | 0x0D);
        TEST::SET_W(22, TEST_BREAK);

        // FMOVECR #0x0E, %FP2
        TEST::SET_W(24, 0171000);
        TEST::SET_W(26, 0x5C00 | 2 << 7 | 0x0E);
        TEST::SET_W(28, TEST_BREAK);

        // FMOVECR #0x0F, %FP2
        TEST::SET_W(30, 0171000);
        TEST::SET_W(32, 0x5C00 | 2 << 7 | 0x0F);
        TEST::SET_W(34, TEST_BREAK);

        // FMOVECR #0x30, %FP2
        TEST::SET_W(36, 0171000);
        TEST::SET_W(38, 0x5C00 | 2 << 7 | 0x30);
        TEST::SET_W(40, TEST_BREAK);

        // FMOVECR #0x31, %FP2
        TEST::SET_W(42, 0171000);
        TEST::SET_W(44, 0x5C00 | 2 << 7 | 0x31);
        TEST::SET_W(46, TEST_BREAK);

        for(int i = 0; i < 14; ++i) {
            TEST::SET_W(48 + 6 * i, 0171000);
            TEST::SET_W(48 + 6 * i + 2, 0x5C00 | 2 << 7 | (0x32 + i));
            TEST::SET_W(48 + 6 * i + 4, TEST_BREAK);
        }
        jit_compile(0, 132);
    }
};
BOOST_FIXTURE_TEST_SUITE(FMOVECR, Prepare,
                         *boost::unit_test::fixture<F_FMOVECR>())

BOOST_AUTO_TEST_CASE(PI) {
    run_test(0);
    BOOST_TEST(TEST::GET_FP(2) == M_PI);
}
BOOST_AUTO_TEST_CASE(Log10_2) {
    run_test(6);
    BOOST_TEST(TEST::GET_FP(2) == log10(2));
}

BOOST_AUTO_TEST_CASE(E) {
    run_test(12);
    BOOST_TEST(TEST::GET_FP(2) == M_E);
}

BOOST_AUTO_TEST_CASE(LOG2_E) {
    run_test(18);
    BOOST_TEST(TEST::GET_FP(2) == log2(M_E));
}

BOOST_AUTO_TEST_CASE(LOG10_E) {
    run_test(24);
    BOOST_TEST(TEST::GET_FP(2) == log10(M_E));
}

BOOST_AUTO_TEST_CASE(ZERO) {
    run_test(30);
    BOOST_TEST(TEST::GET_FP(2) == 0.0);
}

BOOST_AUTO_TEST_CASE(LN_2) {
    run_test(36);
    BOOST_TEST(TEST::GET_FP(2) == log(2));
}

BOOST_AUTO_TEST_CASE(LN_10) {
    run_test(42);
    BOOST_TEST(TEST::GET_FP(2) == log(10));
}

static int pw[] = {0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
BOOST_DATA_TEST_CASE(POW10_n, pw ^ bdata::xrange(14), n, i) {
    run_test(48 + 6 * i);
    mpfr_log10(cpu.FP[2], cpu.FP[2], MPFR_RNDN);
    BOOST_TEST(TEST::GET_FP(2) == n);
}
BOOST_AUTO_TEST_SUITE_END()
