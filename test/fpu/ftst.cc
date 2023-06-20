#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <math.h>
namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(FTST, Prepare)
BOOST_AUTO_TEST_CASE(nan) {
    mpfr_set_nan(cpu.FP[3]);
    cpu.FP_nan[3] = 0xffff000000000000LLU;
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0111010 | 3 << 10 );
    int i = decode_and_run();
    BOOST_TEST(cpu.FPSR.CC_NAN);
    BOOST_TEST(i == 2);
}

BOOST_DATA_TEST_CASE(inf, sg_v , sg) {
    TEST::SET_FP(3, copysign(INFINITY, sg));
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0111010 | 3 << 10 );
    decode_and_run();
    BOOST_TEST(cpu.FPSR.CC_I);
    BOOST_TEST(cpu.FPSR.CC_N == !!signbit(sg));
}

BOOST_DATA_TEST_CASE(zero, sg_v,  sg) {
    TEST::SET_FP(3, copysign(0.0, sg));
     TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0111010 | 3 << 10 );
    decode_and_run();
    BOOST_TEST(cpu.FPSR.CC_Z);
    BOOST_TEST(cpu.FPSR.CC_N == !!signbit(sg));
}

BOOST_DATA_TEST_CASE(normal, sg_v,  sg) {
    TEST::SET_FP(3, copysign(3.0, sg));
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0111010 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(!cpu.FPSR.CC_Z);
    BOOST_TEST(cpu.FPSR.CC_N == !!signbit(sg));
}

BOOST_AUTO_TEST_SUITE_END()
