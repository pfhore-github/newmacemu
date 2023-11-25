#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <math.h>
namespace bdata = boost::unit_test::data;
struct F_FxABS {
    F_FxABS() {
        // FABS.X %FP3, %FP2
        TEST::SET_W(0, 0171000);
        TEST::SET_W(2, 0B0011000 | 3 << 10 | 2 << 7);
        TEST::SET_W(4, TEST_BREAK);

        // FSABS %FP3, %FP2
        TEST::SET_W(6, 0171000);
        TEST::SET_W(8, 0B1011000 | 3 << 10 | 2 << 7);
        TEST::SET_W(10, TEST_BREAK);

        // FDABS %FP3, %FP2
        TEST::SET_W(12, 0171000);
        TEST::SET_W(14, 0B1011100 | 3 << 10 | 2 << 7);
        TEST::SET_W(16, TEST_BREAK);

        jit_compile(0, 18);
    }
};
BOOST_FIXTURE_TEST_SUITE(FxABS, Prepare, *boost::unit_test::fixture<F_FxABS>())

BOOST_AUTO_TEST_SUITE(FABS)
BOOST_AUTO_TEST_CASE(qnan) { qnan_test(0); }

BOOST_AUTO_TEST_CASE(snan) { snan_test(0); }

BOOST_DATA_TEST_CASE(inf, sg_v, sg) {
    TEST::SET_FP(3, copysign(INFINITY, sg));
    run_test(0);
    BOOST_TEST(!mpfr_signbit(cpu.FP[2]));
    BOOST_TEST(mpfr_inf_p(cpu.FP[2]));
}

BOOST_DATA_TEST_CASE(zero, sg_v, sg) {
    TEST::SET_FP(3, copysign(0.0, sg));
    run_test(0);
    BOOST_TEST(!mpfr_signbit(cpu.FP[2]));
    BOOST_TEST(mpfr_zero_p(cpu.FP[2]));
}

BOOST_DATA_TEST_CASE(normal, sg_v, sg) {
    TEST::SET_FP(3, copysign(2.2, sg));
    run_test(0);
    BOOST_TEST(TEST::GET_FP(2) == 2.2);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(FSABS)
BOOST_AUTO_TEST_CASE(qnan) {
    mpfr_set_nan(cpu.FP[3]);
    cpu.FP_nan[3] = 0xffff000000000000LLU;
    run_test(6);
    BOOST_TEST(mpfr_nan_p(cpu.FP[2]));
    BOOST_TEST(cpu.FP_nan[2] == 0xffff000000000000LLU);
}

BOOST_AUTO_TEST_CASE(snan) {
    mpfr_set_nan(cpu.FP[3]);
    cpu.FP_nan[3] = 0x0fff000000000000LLU;
    run_test(6);
    BOOST_TEST(mpfr_nan_p(cpu.FP[2]));
    BOOST_TEST(cpu.FP_nan[2] == 0x4fff000000000000LLU);
    BOOST_TEST(cpu.FPSR.S_NAN);
}

BOOST_DATA_TEST_CASE(inf, sg_v, sg) {
    TEST::SET_FP(3, copysign(INFINITY, sg));
    run_test(6);
    BOOST_TEST(!mpfr_signbit(cpu.FP[2]));
    BOOST_TEST(mpfr_inf_p(cpu.FP[2]));
}

BOOST_DATA_TEST_CASE(zero, sg_v, sg) {
    TEST::SET_FP(3, copysign(0.0, sg));
    run_test(6);
    BOOST_TEST(!mpfr_signbit(cpu.FP[2]));
    BOOST_TEST(mpfr_zero_p(cpu.FP[2]));
}

BOOST_DATA_TEST_CASE(normal, sg_v, sg) {
    TEST::SET_FP(3, copysignf(2.2f, sg));
    run_test(6);
    BOOST_TEST(TEST::GET_FP(2) == 2.2f, boost::test_tools::tolerance(1e-5));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(FDABS)
BOOST_AUTO_TEST_CASE(qnan) {
    mpfr_set_nan(cpu.FP[3]);
    cpu.FP_nan[3] = 0xffff000000000000LLU;
    run_test(12);
    BOOST_TEST(mpfr_nan_p(cpu.FP[2]));
    BOOST_TEST(cpu.FP_nan[2] == 0xffff000000000000LLU);
}

BOOST_AUTO_TEST_CASE(snan) {
    mpfr_set_nan(cpu.FP[3]);
    cpu.FP_nan[3] = 0x0fff000000000000LLU;
    run_test(12);
    BOOST_TEST(mpfr_nan_p(cpu.FP[2]));
    BOOST_TEST(cpu.FP_nan[2] == 0x4fff000000000000LLU);
    BOOST_TEST(cpu.FPSR.S_NAN);
}
BOOST_DATA_TEST_CASE(inf, sg_v, sg) {
    TEST::SET_FP(3, copysign(INFINITY, sg));
    run_test(12);
    BOOST_TEST(!mpfr_signbit(cpu.FP[2]));
    BOOST_TEST(mpfr_inf_p(cpu.FP[2]));
}

BOOST_DATA_TEST_CASE(zero, sg_v, sg) {
    TEST::SET_FP(3, copysign(0.0, sg));
    run_test(12);
    BOOST_TEST(!mpfr_signbit(cpu.FP[2]));
    BOOST_TEST(mpfr_zero_p(cpu.FP[2]));
}

BOOST_DATA_TEST_CASE(normal, sg_v, sg) {
    TEST::SET_FP(3, copysign(2.2, sg));
    run_test(12);
    BOOST_TEST(TEST::GET_FP(2) == 2.2);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()