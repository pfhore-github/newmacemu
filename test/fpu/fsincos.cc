#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "mpfr.h"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <math.h>
namespace bdata = boost::unit_test::data;

struct F_FSINCOS {
    F_FSINCOS() {
        // FSINCOS.X %FP3, %FP2, %FP7
        TEST::SET_W(0, 0171000);
        TEST::SET_W(2, 0B0110000 | 3 << 10 | 2 << 7 | 7);
        TEST::SET_W(4, TEST_BREAK);

        // FSINCOS.X %FP3, %FP2, %FP2
        TEST::SET_W(6, 0171000);
        TEST::SET_W(8, 0B0110000 | 3 << 10 | 2 << 7 | 2);
        TEST::SET_W(10, TEST_BREAK);

        jit_compile(0, 12);
    }
};
BOOST_FIXTURE_TEST_SUITE(FSINCOS, Prepare,
                         *boost::unit_test::fixture<F_FSINCOS>())

BOOST_AUTO_TEST_CASE(qnan) { qnan_test(0); }

BOOST_AUTO_TEST_CASE(snan) { snan_test(0); }
BOOST_DATA_TEST_CASE(inf, sg_v, sg) {
    TEST::SET_FP(3, copysign(INFINITY, sg));
    run_test(0);
    BOOST_TEST(mpfr_nan_p(cpu.FP[2]));
    BOOST_TEST(mpfr_nan_p(cpu.FP[7]));
    BOOST_TEST(cpu.FPSR.OPERR);
    BOOST_TEST(cpu.FPSR.EXC_IOP);
}

BOOST_DATA_TEST_CASE(zero, sg_v, sg) {
    TEST::SET_FP(3, copysign(0.0, sg));
    run_test(0);
    BOOST_TEST(mpfr_zero_p(cpu.FP[2]));
    BOOST_TEST(!!mpfr_signbit(cpu.FP[2]) == !!signbit(sg));
    BOOST_TEST(TEST::GET_FP(7) == 1.0);
}

BOOST_AUTO_TEST_CASE(normal) {
    TEST::SET_FP(3, M_PI);
    run_test(0);
    BOOST_TEST(TEST::GET_FP(2) == 0.0, boost::test_tools::tolerance(1e-13));
    BOOST_TEST(TEST::GET_FP(7) == -1.0);
}

BOOST_AUTO_TEST_CASE(sin_only) {
    TEST::SET_FP(3, M_PI);
    run_test(6);
    BOOST_TEST(TEST::GET_FP(2) == 0.0, boost::test_tools::tolerance(1e-13));
}

BOOST_AUTO_TEST_SUITE_END()
