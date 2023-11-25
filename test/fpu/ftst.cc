#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <math.h>
namespace bdata = boost::unit_test::data;

struct F_FTST {
    F_FTST() {
        // FTST.X %FP3
        TEST::SET_W(0, 0171000);
        TEST::SET_W(2, 0B0111010 | 3 << 10);
        TEST::SET_W(4, TEST_BREAK);

        jit_compile(0, 6);
    }
};
BOOST_FIXTURE_TEST_SUITE(FTST, Prepare, *boost::unit_test::fixture<F_FTST>())

BOOST_AUTO_TEST_CASE(nan) {
    mpfr_set_nan(cpu.FP[3]);
    cpu.FP_nan[3] = 0xffff000000000000LLU;
    run_test(0);
    BOOST_TEST(cpu.FPSR.CC_NAN);
}

BOOST_DATA_TEST_CASE(inf, sg_v, sg) {
    TEST::SET_FP(3, copysign(INFINITY, sg));
    run_test(0);
    BOOST_TEST(cpu.FPSR.CC_I);
    BOOST_TEST(cpu.FPSR.CC_N == !!signbit(sg));
}

BOOST_DATA_TEST_CASE(zero, sg_v, sg) {
    TEST::SET_FP(3, copysign(0.0, sg));
    run_test(0);
    BOOST_TEST(cpu.FPSR.CC_Z);
    BOOST_TEST(cpu.FPSR.CC_N == !!signbit(sg));
}

BOOST_DATA_TEST_CASE(normal, sg_v, sg) {
    TEST::SET_FP(3, copysign(3.0, sg));
    run_test(0);
    BOOST_TEST(!cpu.FPSR.CC_Z);
    BOOST_TEST(cpu.FPSR.CC_N == !!signbit(sg));
}

BOOST_AUTO_TEST_SUITE_END()
