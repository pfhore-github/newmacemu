#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "mpfr.h"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <math.h>
namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(FTANH, Prepare)
BOOST_AUTO_TEST_CASE(qnan) {
    qnan_test(0B0001001);
}

BOOST_AUTO_TEST_CASE(snan) {
    snan_test(0B0001001);
}
BOOST_DATA_TEST_CASE(inf, sg_v, sg) {
    TEST::SET_FP(3, copysign(INFINITY, sg));
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0001001 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(TEST::GET_FP(2) == sg);
}

BOOST_DATA_TEST_CASE(zero, sg_v, sg) {
    TEST::SET_FP(3, copysign(0.0, sg));
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0001001 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(mpfr_zero_p(cpu.FP[2]));
    BOOST_TEST(!!mpfr_signbit(cpu.FP[2]) == !!signbit(sg));
}

BOOST_AUTO_TEST_CASE(normal, *boost::unit_test::tolerance(1e-7)) {
    TEST::SET_FP(3, 1.0);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0001001 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(TEST::GET_FP(2) == 0.761594155955765);
}

BOOST_AUTO_TEST_SUITE_END()
