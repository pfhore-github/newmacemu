#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <math.h>
namespace bdata = boost::unit_test::data;

struct F_FCMP {
    F_FCMP() {
        // FCMP.X %FP3, %FP2
        TEST::SET_W(0, 0171000);
        TEST::SET_W(2, 0B0111000 | 3 << 10 | 2 << 7);
        TEST::SET_W(4, TEST_BREAK);

        jit_compile(0, 6);
    }
};
BOOST_FIXTURE_TEST_SUITE(FCMP, Prepare, *boost::unit_test::fixture<F_FCMP>())

BOOST_AUTO_TEST_CASE(nan_normal) {
    mpfr_set_nan(cpu.FP[3]);
    run_test(0);
    BOOST_TEST(cpu.FPSR.CC_NAN);
}

BOOST_AUTO_TEST_CASE(normal_nan) {
    mpfr_set_nan(cpu.FP[2]);
    TEST::SET_FP(3, 0.0);
    run_test(0);
    BOOST_TEST(cpu.FPSR.CC_NAN);
}

BOOST_AUTO_TEST_CASE(nan_nan) {
    mpfr_set_nan(cpu.FP[2]);
    mpfr_set_nan(cpu.FP[3]);
    run_test(0);
    BOOST_TEST(cpu.FPSR.CC_NAN);
}

BOOST_DATA_TEST_CASE(inf_inf, sg_v *sg_v, sg1, sg2) {
    TEST::SET_FP(2, copysign(INFINITY, sg1));
    TEST::SET_FP(3, copysign(INFINITY, sg2));
    run_test(0);
    BOOST_TEST(cpu.FPSR.CC_Z == (sg1 == sg2));
    BOOST_TEST(cpu.FPSR.CC_N == !!signbit(sg1));
}

BOOST_DATA_TEST_CASE(inf_zero, sg_v *sg_v, sg1, sg2) {
    TEST::SET_FP(2, copysign(INFINITY, sg1));
    TEST::SET_FP(3, copysign(0.0, sg2));
    run_test(0);
    BOOST_TEST(!cpu.FPSR.CC_Z);
    BOOST_TEST(cpu.FPSR.CC_N == !!signbit(sg1));
}

BOOST_DATA_TEST_CASE(inf_normal, sg_v, sg) {
    TEST::SET_FP(2, copysign(INFINITY, sg));
    TEST::SET_FP(3, sg);
    run_test(0);
    BOOST_TEST(!cpu.FPSR.CC_Z);
    BOOST_TEST(cpu.FPSR.CC_N == !!signbit(sg));
}

BOOST_DATA_TEST_CASE(zero_inf, sg_v *sg_v, sg1, sg2) {
    TEST::SET_FP(2, copysign(0.0, sg1));
    TEST::SET_FP(3, copysign(INFINITY, sg2));
    run_test(0);
    BOOST_TEST(!cpu.FPSR.CC_Z);
    BOOST_TEST(cpu.FPSR.CC_N == !signbit(sg2));
}
BOOST_DATA_TEST_CASE(zero_zero, sg_v *sg_v, sg1, sg2) {
    TEST::SET_FP(2, copysign(0.0, sg1));
    TEST::SET_FP(3, copysign(0.0, sg2));
    run_test(0);
    BOOST_TEST(cpu.FPSR.CC_Z);
    BOOST_TEST(cpu.FPSR.CC_N == !!signbit(sg1));
}

BOOST_DATA_TEST_CASE(zero_normal, sg_v *sg_v, sg1, sg2) {
    TEST::SET_FP(2, copysign(0.0, sg1));
    TEST::SET_FP(3, copysign(1.1, sg2));
    run_test(0);
    BOOST_TEST(!cpu.FPSR.CC_Z);
    BOOST_TEST(cpu.FPSR.CC_N == !signbit(sg2));
}

BOOST_DATA_TEST_CASE(normal_inf, sg_v, sg) {
    TEST::SET_FP(3, 2.0);
    TEST::SET_FP(2, copysign(INFINITY, sg));
    run_test(0);
    BOOST_TEST(!cpu.FPSR.CC_Z);
    BOOST_TEST(cpu.FPSR.CC_N == !!signbit(sg));
}

BOOST_DATA_TEST_CASE(normal_zero, sg_v, sg) {
    TEST::SET_FP(2, 2.0);
    TEST::SET_FP(3, copysign(0.0, sg));
    run_test(0);
    BOOST_TEST(!cpu.FPSR.CC_Z);
    BOOST_TEST(!cpu.FPSR.CC_N);
}
BOOST_AUTO_TEST_CASE(EQ) {
    TEST::SET_FP(2, 3.0);
    TEST::SET_FP(3, 3.0);
    run_test(0);
    BOOST_TEST(cpu.FPSR.CC_Z);
}

BOOST_AUTO_TEST_CASE(LT) {
    TEST::SET_FP(2, 2.0);
    TEST::SET_FP(3, 3.0);
    run_test(0);
    BOOST_TEST(!cpu.FPSR.CC_Z);
    BOOST_TEST(cpu.FPSR.CC_N);
}

BOOST_AUTO_TEST_CASE(GT) {
    TEST::SET_FP(2, 3.0);
    TEST::SET_FP(3, 2.0);
    run_test(0);
    BOOST_TEST(!cpu.FPSR.CC_Z);
    BOOST_TEST(!cpu.FPSR.CC_N);
}

BOOST_AUTO_TEST_SUITE_END()
