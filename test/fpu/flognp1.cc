#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "mpfr.h"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <math.h>
namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(FLOGNP1, Prepare)
BOOST_AUTO_TEST_CASE(qnan) { qnan_test(0b0000110); }

BOOST_AUTO_TEST_CASE(snan) { snan_test(0b0000110); }

BOOST_AUTO_TEST_CASE(pinf) {
    TEST::SET_FP(3, INFINITY);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0b0000110 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(mpfr_inf_p(cpu.FP[2]));
    BOOST_TEST(!mpfr_signbit(cpu.FP[2]));
}

BOOST_AUTO_TEST_CASE(minf) {
    TEST::SET_FP(3, -INFINITY);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0b0000110 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(mpfr_nan_p(cpu.FP[2]));
    BOOST_TEST(cpu.FPSR.OPERR);
    BOOST_TEST(cpu.FPSR.EXC_IOP);
}

BOOST_DATA_TEST_CASE(zero, sg_v, sg) {
    TEST::SET_FP(3, copysign(0.0, sg));
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0b0000110 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(mpfr_zero_p(cpu.FP[2]));
    BOOST_TEST(!!mpfr_signbit(cpu.FP[2]) == !!signbit(sg));
}

BOOST_AUTO_TEST_CASE(normal) {
    TEST::SET_FP(3, M_E-1.0);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0b0000110 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(TEST::GET_FP(2) == 1.0);
}

BOOST_AUTO_TEST_CASE(dz) {
    TEST::SET_FP(3, -1.0);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0b0000110 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(mpfr_inf_p(cpu.FP[2]));
    BOOST_TEST(mpfr_signbit(cpu.FP[2]));
    BOOST_TEST(cpu.FPSR.DZ);
    BOOST_TEST(cpu.FPSR.EXC_DZ);
}


BOOST_AUTO_TEST_CASE(range) {
    TEST::SET_FP(3, -10.0);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0b0000110 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(mpfr_nan_p(cpu.FP[2]));
    BOOST_TEST(cpu.FPSR.OPERR);
    BOOST_TEST(cpu.FPSR.EXC_IOP);
}

BOOST_AUTO_TEST_SUITE_END()
