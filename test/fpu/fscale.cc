#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <math.h>
namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(FSCALE, Prepare)
BOOST_AUTO_TEST_CASE(qnan) { qnan_test(0B0100110); }

BOOST_AUTO_TEST_CASE(snan) { snan_test(0B0100110); }

BOOST_AUTO_TEST_CASE(normal_nan) {
    mpfr_set_nan(cpu.FP[2]);
    cpu.FP_nan[2] = 0xffff000000000000LLU;
    TEST::SET_FP(3, 0.0);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0100110 | 3 << 10 | 2 << 7);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(mpfr_nan_p(cpu.FP[2]));
    BOOST_TEST(cpu.FP_nan[2] == 0xffff000000000000LLU);
}

BOOST_AUTO_TEST_CASE(nan_nan) {
    mpfr_set_nan(cpu.FP[2]);
    cpu.FP_nan[2] = 0xffff000000000000LLU;
    mpfr_set_nan(cpu.FP[3]);
    cpu.FP_nan[3] = 0xffffffff00000000LLU;
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0100110 | 3 << 10 | 2 << 7);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(mpfr_nan_p(cpu.FP[2]));
    BOOST_TEST(cpu.FP_nan[2] == 0xffffffff00000000LLU);
}

BOOST_DATA_TEST_CASE(inf_inf, sg_v *sg_v, sg1, sg2) {
    TEST::SET_FP(3, copysign(INFINITY, sg1));
    TEST::SET_FP(2, copysign(INFINITY, sg2));
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0100110 | 3 << 10 | 2 << 7);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(mpfr_nan_p(cpu.FP[2]));
    BOOST_TEST(cpu.FPSR.OPERR);
    BOOST_TEST(cpu.FPSR.EXC_IOP);
}

BOOST_DATA_TEST_CASE(inf_zero, sg_v *sg_v, sg1, sg2) {
    TEST::SET_FP(3, copysign(INFINITY, sg1));
    TEST::SET_FP(2, copysign(0.0, sg2));
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0100110 | 3 << 10 | 2 << 7);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(mpfr_nan_p(cpu.FP[2]));
    BOOST_TEST(cpu.FPSR.OPERR);
    BOOST_TEST(cpu.FPSR.EXC_IOP);
}

BOOST_DATA_TEST_CASE(inf_normal, sg_v, sg) {
    TEST::SET_FP(3, copysign(INFINITY, sg));
    TEST::SET_FP(2, 1.0);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0100110 | 3 << 10 | 2 << 7);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(mpfr_nan_p(cpu.FP[2]));
    BOOST_TEST(cpu.FPSR.OPERR);
    BOOST_TEST(cpu.FPSR.EXC_IOP);
}

BOOST_DATA_TEST_CASE(zero_inf, sg_v *sg_v, sg1, sg2) {
    TEST::SET_FP(3, copysign(0.0, sg1));
    TEST::SET_FP(2, copysign(INFINITY, sg2));
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0100110 | 3 << 10 | 2 << 7);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(mpfr_inf_p(cpu.FP[2]));
    BOOST_TEST(!!mpfr_signbit(cpu.FP[2]) == !!signbit(sg2));
}
BOOST_DATA_TEST_CASE(zero_zero, sg_v *sg_v, sg1, sg2) {
    TEST::SET_FP(3, copysign(0.0, sg1));
    TEST::SET_FP(2, copysign(0.0, sg2));
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0100110 | 3 << 10 | 2 << 7);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(mpfr_zero_p(cpu.FP[2]));
    BOOST_TEST(!!mpfr_signbit(cpu.FP[2]) == !!signbit(sg2));
}

BOOST_DATA_TEST_CASE(zero_normal, sg_v, sg) {
    TEST::SET_FP(3, copysign(0.0, sg));
    TEST::SET_FP(2, 1.1);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0100110 | 3 << 10 | 2 << 7);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(TEST::GET_FP(2) == 1.1);
}

BOOST_DATA_TEST_CASE(normal_inf, sg_v, sg) {
    TEST::SET_FP(3, 2.0);
    TEST::SET_FP(2, copysign(INFINITY, sg));
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0100110 | 3 << 10 | 2 << 7);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(mpfr_inf_p(cpu.FP[2]));
    BOOST_TEST(!!mpfr_signbit(cpu.FP[2]) == !!signbit(sg));
}

BOOST_DATA_TEST_CASE(normal_zero, sg_v, sg) {
    TEST::SET_FP(3, 2.0);
    TEST::SET_FP(2, copysign(0.0, sg));
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0100110 | 3 << 10 | 2 << 7);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(mpfr_zero_p(cpu.FP[2]));
    BOOST_TEST(!!mpfr_signbit(cpu.FP[2]) == !!signbit(sg));
}
BOOST_AUTO_TEST_CASE(normal) {
    TEST::SET_FP(3, 4.0);
    TEST::SET_FP(2, 3.0);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0100110 | 3 << 10 | 2 << 7);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(TEST::GET_FP(2) == 48.0);
}

BOOST_AUTO_TEST_CASE(ovfl) {
   TEST::SET_FP(3, 22000.0);
    TEST::SET_FP(2, 3.0);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0100110 | 3 << 10 | 2 << 7);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(mpfr_inf_p(cpu.FP[2]));
    BOOST_TEST(cpu.FPSR.OVFL);
}

BOOST_AUTO_TEST_CASE(unfl) {
   TEST::SET_FP(3, -22000.0);
    TEST::SET_FP(2, 3.0);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0100110 | 3 << 10 | 2 << 7);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.FPSR.UNFL);
}
BOOST_AUTO_TEST_SUITE_END()
