#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "mpfr.h"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <math.h>
namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(FINT, Prepare)
BOOST_AUTO_TEST_CASE(qnan) {
    qnan_test(0B0000001);
}

BOOST_AUTO_TEST_CASE(snan) {
    snan_test(0B0000001);
}
BOOST_DATA_TEST_CASE(inf, sg_v, sg) {
    TEST::SET_FP(3, copysign(INFINITY, sg));
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0000001 | 3 << 10 | 2 << 7);
    run_test();
    BOOST_TEST(mpfr_inf_p(cpu.FP[2]));
    BOOST_TEST(!!mpfr_signbit(cpu.FP[2]) == !!signbit(sg));
}

BOOST_DATA_TEST_CASE(zero, sg_v, sg) {
    TEST::SET_FP(3, copysign(0.0, sg));
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0000001 | 3 << 10 | 2 << 7);
    run_test();
    BOOST_TEST(mpfr_zero_p(cpu.FP[2]));
    BOOST_TEST(!!mpfr_signbit(cpu.FP[2]) == !!signbit(sg));
}
static double args[] = { 1.3, 1.5, 2.5, 2.7, -1.3, -1.5, -2.5, -2.7};
static double expected_u[] = { 2.0, 2.0, 3.0, 3.0, -1.0, -1.0, -2.0, -2.0};
BOOST_DATA_TEST_CASE(to_p_inf, args ^ expected_u, a, e) {
    TEST::SET_FP(3, a);
    TEST::SET_W(0, 0171000);
    cpu.FPCR.RND = MPFR_RNDU;
    TEST::SET_W(2, 0B0000001 | 3 << 10 | 2 << 7);
    run_test();
    BOOST_TEST(TEST::GET_FP(2) == e);
}

static double expected_d[] = { 1.0, 1.0, 2.0, 2.0, -2.0, -2.0, -3.0, -3.0};
BOOST_DATA_TEST_CASE(to_m_inf, args ^ expected_d, a, e) {
    TEST::SET_FP(3, a);
    TEST::SET_W(0, 0171000);
    cpu.FPCR.RND = MPFR_RNDD;
    TEST::SET_W(2, 0B0000001 | 3 << 10 | 2 << 7);
    run_test();
    BOOST_TEST(TEST::GET_FP(2) == e);
}

static double expected_z[] = { 1.0, 1.0, 2.0, 2.0, -1.0, -1.0, -2.0, -2.0};
BOOST_DATA_TEST_CASE(to_zero, args ^ expected_z, a, e) {
    TEST::SET_FP(3, a);
    TEST::SET_W(0, 0171000);
    cpu.FPCR.RND = MPFR_RNDZ;
    TEST::SET_W(2, 0B0000001 | 3 << 10 | 2 << 7);
    run_test();
    BOOST_TEST(TEST::GET_FP(2) == e);
}

static double expected_n[] = { 1.0, 2.0, 2.0, 3.0, -1.0, -2.0, -2.0, -3.0};
BOOST_DATA_TEST_CASE(nearest, args ^ expected_n, a, e) {
    TEST::SET_FP(3, a);
    TEST::SET_W(0, 0171000);
    cpu.FPCR.RND = MPFR_RNDN;
    TEST::SET_W(2, 0B0000001 | 3 << 10 | 2 << 7);
    run_test();
    BOOST_TEST(TEST::GET_FP(2) == e);
}


BOOST_AUTO_TEST_SUITE_END()
