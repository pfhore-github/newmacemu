#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "mpfr.h"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <math.h>
namespace bdata = boost::unit_test::data;

struct F_FCOSH {
    F_FCOSH() {
        // FCOSH.X %FP3, %FP2
        TEST::SET_W(0, 0171000);
        TEST::SET_W(2, 0B0011001 | 3 << 10 | 2 << 7);
        TEST::SET_W(4, TEST_BREAK);

        jit_compile(0, 6);
    }
};
BOOST_FIXTURE_TEST_SUITE(FCOSH, Prepare, *boost::unit_test::fixture<F_FCOSH>())

BOOST_AUTO_TEST_CASE(qnan) { qnan_test(0); }

BOOST_AUTO_TEST_CASE(snan) { snan_test(0); }
BOOST_DATA_TEST_CASE(inf, sg_v, sg) {
    TEST::SET_FP(3, copysign(INFINITY, sg));
    run_test(0);
    BOOST_TEST(mpfr_inf_p(cpu.FP[2]));
    BOOST_TEST(!mpfr_signbit(cpu.FP[2]));
}

BOOST_DATA_TEST_CASE(zero, sg_v, sg) {
    TEST::SET_FP(3, copysign(0.0, sg));
    run_test(0);
    BOOST_TEST(TEST::GET_FP(2) == 1.0);
}

BOOST_AUTO_TEST_CASE(normal) {
    TEST::SET_FP(3, 1.0);
    run_test(0);
    BOOST_TEST(TEST::GET_FP(2) == 1.54308063481524,
               boost::test_tools::tolerance(1e-11));
}

BOOST_AUTO_TEST_CASE(ovfl) {
    TEST::SET_FP(3, 1e100);
    run_test(0);
    BOOST_TEST(mpfr_inf_p(cpu.FP[2]));
    BOOST_TEST(cpu.FPSR.OVFL);
    BOOST_TEST(cpu.FPSR.EXC_OVFL);
}

BOOST_AUTO_TEST_SUITE_END()
