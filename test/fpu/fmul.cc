#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <math.h>
namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(FMUL, Prepare)
BOOST_AUTO_TEST_CASE(qnan) { qnan_test(0B0100011); }

BOOST_AUTO_TEST_CASE(snan) { snan_test(0B0100011); }

BOOST_AUTO_TEST_CASE(normal_nan) {
    mpfr_set_nan(cpu.FP[2]);
    cpu.FP_nan[2] = 0xffff000000000000LLU;
    TEST::SET_FP(3, 0.0);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0100011 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(mpfr_nan_p(cpu.FP[2]));
    BOOST_TEST(cpu.FP_nan[2] == 0xffff000000000000LLU);
}

BOOST_AUTO_TEST_CASE(nan_nan) {
    mpfr_set_nan(cpu.FP[2]);
    cpu.FP_nan[2] = 0xffff000000000000LLU;
    mpfr_set_nan(cpu.FP[3]);
    cpu.FP_nan[3] = 0xffffffff00000000LLU;
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0100011 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(mpfr_nan_p(cpu.FP[2]));
    BOOST_TEST(cpu.FP_nan[2] == 0xffffffff00000000LLU);
}

BOOST_DATA_TEST_CASE(inf_inf, sg_v *sg_v, sg1, sg2) {
    TEST::SET_FP(3, copysign(INFINITY, sg1));
    TEST::SET_FP(2, copysign(INFINITY, sg2));
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0100011 | 3 << 10 | 2 << 7);
    auto i = decode_and_run();
    BOOST_TEST(mpfr_inf_p(cpu.FP[2]));
    BOOST_TEST(!!mpfr_signbit(cpu.FP[2]) == !!signbit(sg1 * sg2));
    BOOST_TEST(i == 2);
}

BOOST_DATA_TEST_CASE(inf_zero, sg_v *sg_v, sg1, sg2) {
    TEST::SET_FP(3, copysign(INFINITY, sg1));
    TEST::SET_FP(2, copysign(0.0, sg2));
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0100011 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(mpfr_nan_p(cpu.FP[2]));
    BOOST_TEST(cpu.FPSR.OPERR);
    BOOST_TEST(cpu.FPSR.EXC_IOP);
}

BOOST_DATA_TEST_CASE(inf_normal, sg_v, sg) {
    TEST::SET_FP(3, copysign(INFINITY, sg));
    TEST::SET_FP(2, 1.0);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0100011 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(mpfr_inf_p(cpu.FP[2]));
    BOOST_TEST(!!mpfr_signbit(cpu.FP[2]) == !!signbit(sg));
}

BOOST_DATA_TEST_CASE(zero_inf, sg_v *sg_v, sg1, sg2) {
    TEST::SET_FP(3, copysign(0.0, sg1));
    TEST::SET_FP(2, copysign(INFINITY, sg2));
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0100011 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(mpfr_nan_p(cpu.FP[2]));
    BOOST_TEST(cpu.FPSR.OPERR);
    BOOST_TEST(cpu.FPSR.EXC_IOP);
}
BOOST_DATA_TEST_CASE(zero_zero, sg_v *sg_v, sg1, sg2) {
    TEST::SET_FP(3, copysign(0.0, sg1));
    TEST::SET_FP(2, copysign(0.0, sg2));
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0100011 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(mpfr_zero_p(cpu.FP[2]));
    BOOST_TEST(!!mpfr_signbit(cpu.FP[2]) == !!signbit(sg1 * sg2));
}

BOOST_DATA_TEST_CASE(zero_normal, sg_v, sg) {
    TEST::SET_FP(3, copysign(0.0, sg));
    TEST::SET_FP(2, 1.1);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0100011 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(mpfr_zero_p(cpu.FP[2]));
    BOOST_TEST(!!mpfr_signbit(cpu.FP[2]) == !!signbit(sg));
}

BOOST_DATA_TEST_CASE(normal_inf, sg_v, sg) {
    TEST::SET_FP(3, 2.0);
    TEST::SET_FP(2, copysign(INFINITY, sg));
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0100011 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(mpfr_inf_p(cpu.FP[2]));
    BOOST_TEST(!!mpfr_signbit(cpu.FP[2]) == !!signbit(sg));
}

BOOST_DATA_TEST_CASE(normal_zero, sg_v, sg) {
    TEST::SET_FP(3, 2.0);
    TEST::SET_FP(2, copysign(0.0, sg));
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0100011 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(mpfr_zero_p(cpu.FP[2]));
    BOOST_TEST(!!mpfr_signbit(cpu.FP[2]) == !!signbit(sg));
}
BOOST_AUTO_TEST_CASE(normal) {
    TEST::SET_FP(3, 2.0);
    TEST::SET_FP(2, 3.0);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0100011 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(TEST::GET_FP(2) == 6.0);
}

BOOST_AUTO_TEST_CASE(ovfl) {
    mpfr_set_ui_2exp(cpu.FP[3], 1, 16383, MPFR_RNDN);
    mpfr_set_ui_2exp(cpu.FP[2], 1, 16383, MPFR_RNDN);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0100011 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(mpfr_inf_p(cpu.FP[2]));
    BOOST_TEST(cpu.FPSR.OVFL);
}

BOOST_AUTO_TEST_CASE(unfl) {
    mpfr_set_si_2exp(cpu.FP[3], 1, -16385, MPFR_RNDN);
    mpfr_set_si_2exp(cpu.FP[2], 1, -16385, MPFR_RNDN);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0100011 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(cpu.FPSR.UNFL);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FSMUL, Prepare, *boost::unit_test::tolerance(1e-5))
BOOST_AUTO_TEST_CASE(qnan) {
    mpfr_set_nan(cpu.FP[3]);
    cpu.FP_nan[3] = 0xffff000000000000LLU;
    TEST::SET_FP(2, 0.0);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B1100011 | 3 << 10 | 2 << 7);
    int i = decode_and_run();
    BOOST_TEST(mpfr_nan_p(cpu.FP[2]));
    BOOST_TEST(cpu.FP_nan[2] == 0xffff000000000000LLU);
    BOOST_TEST(i == 2);
}

BOOST_AUTO_TEST_CASE(snan) {
    mpfr_set_nan(cpu.FP[3]);
    cpu.FP_nan[3] = 0x0fff000000000000LLU;
    TEST::SET_FP(2, 0.0);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B1100011 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(mpfr_nan_p(cpu.FP[2]));
    BOOST_TEST(cpu.FP_nan[2] == 0x4fff000000000000LLU);
    BOOST_TEST(cpu.FPSR.S_NAN);
}

BOOST_AUTO_TEST_CASE(normal_nan) {
    mpfr_set_nan(cpu.FP[2]);
    cpu.FP_nan[2] = 0xffff000000000000LLU;
    TEST::SET_FP(3, 0.0);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B1100011 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(mpfr_nan_p(cpu.FP[2]));
    BOOST_TEST(cpu.FP_nan[2] == 0xffff000000000000LLU);
}

BOOST_AUTO_TEST_CASE(nan_nan) {
    mpfr_set_nan(cpu.FP[2]);
    cpu.FP_nan[2] = 0xffff000000000000LLU;
    mpfr_set_nan(cpu.FP[3]);
    cpu.FP_nan[3] = 0xffffffff00000000LLU;
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B1100011 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(mpfr_nan_p(cpu.FP[2]));
    BOOST_TEST(cpu.FP_nan[2] == 0xffffffff00000000LLU);
}

BOOST_DATA_TEST_CASE(inf_inf, sg_v *sg_v, sg1, sg2) {
    TEST::SET_FP(3, copysign(INFINITY, sg1));
    TEST::SET_FP(2, copysign(INFINITY, sg2));
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B1100011 | 3 << 10 | 2 << 7);
    int i = decode_and_run();
    BOOST_TEST(mpfr_inf_p(cpu.FP[2]));
    BOOST_TEST(!!mpfr_signbit(cpu.FP[2]) == !!signbit(sg1 * sg2));
    BOOST_TEST(i == 2);
}

BOOST_DATA_TEST_CASE(inf_zero, sg_v *sg_v, sg1, sg2) {
    TEST::SET_FP(3, copysign(INFINITY, sg1));
    TEST::SET_FP(2, copysign(0.0, sg2));
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B1100011 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(mpfr_nan_p(cpu.FP[2]));
    BOOST_TEST(cpu.FPSR.OPERR);
    BOOST_TEST(cpu.FPSR.EXC_IOP);
}

BOOST_DATA_TEST_CASE(inf_normal, sg_v, sg) {
    TEST::SET_FP(3, copysign(INFINITY, sg));
    TEST::SET_FP(2, 1.0);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B1100011 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(mpfr_inf_p(cpu.FP[2]));
    BOOST_TEST(!!mpfr_signbit(cpu.FP[2]) == !!signbit(sg));
}

BOOST_DATA_TEST_CASE(zero_inf, sg_v *sg_v, sg1, sg2) {
    TEST::SET_FP(3, copysign(0.0, sg1));
    TEST::SET_FP(2, copysign(INFINITY, sg2));
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B1100011 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(mpfr_nan_p(cpu.FP[2]));
    BOOST_TEST(cpu.FPSR.OPERR);
    BOOST_TEST(cpu.FPSR.EXC_IOP);
}

BOOST_DATA_TEST_CASE(zero_zero, sg_v *sg_v *RND_MODES, sg1, sg2, rd) {
    TEST::SET_FP(3, copysign(0.0, sg1));
    TEST::SET_FP(2, copysign(0.0, sg2));
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B1100011 | 3 << 10 | 2 << 7);
    cpu.FPCR.RND = rd;
    decode_and_run();
    BOOST_TEST(mpfr_zero_p(cpu.FP[2]));
    BOOST_TEST(!!mpfr_signbit(cpu.FP[2]) == !!signbit(sg1 * sg2));
}

BOOST_DATA_TEST_CASE(zero_normal, sg_v, sg) {
    TEST::SET_FP(3, copysign(0.0, sg));
    TEST::SET_FP(2, 1.1);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B1100011 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(mpfr_zero_p(cpu.FP[2]));
    BOOST_TEST(!!mpfr_signbit(cpu.FP[2]) == !!signbit(sg));
}

BOOST_DATA_TEST_CASE(normal_inf, sg_v, sg) {
    TEST::SET_FP(3, 2.0);
    TEST::SET_FP(2, copysign(INFINITY, sg));
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B1100011 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(mpfr_inf_p(cpu.FP[2]));
    BOOST_TEST(!!mpfr_signbit(cpu.FP[2]) == !!signbit(sg));
}

BOOST_DATA_TEST_CASE(normal_zero, sg_v, sg) {
    TEST::SET_FP(3, 2.0);
    TEST::SET_FP(2, copysign(0.0, sg));
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B1100011 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(mpfr_zero_p(cpu.FP[2]));
    BOOST_TEST(!!mpfr_signbit(cpu.FP[2]) == !!signbit(sg));
}
BOOST_AUTO_TEST_CASE(normal) {
    TEST::SET_FP(3, 2.0);
    TEST::SET_FP(2, 3.0);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B1100011 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(TEST::GET_FP(2) == 6.0);
}

BOOST_AUTO_TEST_CASE(ovfl) {
    mpfr_set_ui_2exp(cpu.FP[3], 1, 127, MPFR_RNDN);
    mpfr_set_ui_2exp(cpu.FP[2], 1, 127, MPFR_RNDN);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B1100011 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(mpfr_inf_p(cpu.FP[2]));
    BOOST_TEST(cpu.FPSR.OVFL);
}

BOOST_AUTO_TEST_CASE(unfl) {
    mpfr_set_si_2exp(cpu.FP[3], 1, -127, MPFR_RNDN);
    mpfr_set_si_2exp(cpu.FP[2], 1, -127, MPFR_RNDN);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B1100011 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(cpu.FPSR.UNFL);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FDMUL, Prepare)
BOOST_AUTO_TEST_CASE(qnan) {
    mpfr_set_nan(cpu.FP[3]);
    cpu.FP_nan[3] = 0xffff000000000000LLU;
    TEST::SET_FP(2, 0.0);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B1100111 | 3 << 10 | 2 << 7);
    int i = decode_and_run();
    BOOST_TEST(mpfr_nan_p(cpu.FP[2]));
    BOOST_TEST(cpu.FP_nan[2] == 0xffff000000000000LLU);
    BOOST_TEST(i == 2);
}

BOOST_AUTO_TEST_CASE(snan) {
    mpfr_set_nan(cpu.FP[3]);
    cpu.FP_nan[3] = 0x0fff000000000000LLU;
    TEST::SET_FP(2, 0.0);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B1100111 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(mpfr_nan_p(cpu.FP[2]));
    BOOST_TEST(cpu.FP_nan[2] == 0x4fff000000000000LLU);
    BOOST_TEST(cpu.FPSR.S_NAN);
}

BOOST_AUTO_TEST_CASE(normal_nan) {
    mpfr_set_nan(cpu.FP[2]);
    cpu.FP_nan[2] = 0xffff000000000000LLU;
    TEST::SET_FP(3, 0.0);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B1100111 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(mpfr_nan_p(cpu.FP[2]));
    BOOST_TEST(cpu.FP_nan[2] == 0xffff000000000000LLU);
}

BOOST_AUTO_TEST_CASE(nan_nan) {
    mpfr_set_nan(cpu.FP[2]);
    cpu.FP_nan[2] = 0xffff000000000000LLU;
    mpfr_set_nan(cpu.FP[3]);
    cpu.FP_nan[3] = 0xffffffff00000000LLU;
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B1100111 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(mpfr_nan_p(cpu.FP[2]));
    BOOST_TEST(cpu.FP_nan[2] == 0xffffffff00000000LLU);
}

BOOST_DATA_TEST_CASE(inf_inf, sg_v *sg_v, sg1, sg2) {
    TEST::SET_FP(3, copysign(INFINITY, sg1));
    TEST::SET_FP(2, copysign(INFINITY, sg2));
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B1100111 | 3 << 10 | 2 << 7);
    int i = decode_and_run();
    BOOST_TEST(mpfr_inf_p(cpu.FP[2]));
    BOOST_TEST(!!mpfr_signbit(cpu.FP[2]) == !!signbit(sg1 * sg2));
    BOOST_TEST(i == 2);
}

BOOST_DATA_TEST_CASE(inf_zero, sg_v *sg_v, sg1, sg2) {
    TEST::SET_FP(3, copysign(INFINITY, sg1));
    TEST::SET_FP(2, copysign(0.0, sg2));
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B1100111 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(mpfr_nan_p(cpu.FP[2]));
    BOOST_TEST(cpu.FPSR.OPERR);
    BOOST_TEST(cpu.FPSR.EXC_IOP);
}

BOOST_DATA_TEST_CASE(inf_normal, sg_v, sg) {
    TEST::SET_FP(3, copysign(INFINITY, sg));
    TEST::SET_FP(2, 1.0);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B1100111 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(mpfr_inf_p(cpu.FP[2]));
    BOOST_TEST(!!mpfr_signbit(cpu.FP[2]) == !!signbit(sg));
}

BOOST_DATA_TEST_CASE(zero_inf, sg_v *sg_v, sg1, sg2) {
    TEST::SET_FP(3, copysign(0.0, sg1));
    TEST::SET_FP(2, copysign(INFINITY, sg2));
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B1100111 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(mpfr_nan_p(cpu.FP[2]));
    BOOST_TEST(cpu.FPSR.OPERR);
    BOOST_TEST(cpu.FPSR.EXC_IOP);
}
BOOST_DATA_TEST_CASE(zero_zero, sg_v *sg_v, sg1, sg2) {
    TEST::SET_FP(3, copysign(0.0, sg1));
    TEST::SET_FP(2, copysign(0.0, sg2));
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B1100111 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(mpfr_zero_p(cpu.FP[2]));
    BOOST_TEST(!!mpfr_signbit(cpu.FP[2]) == !!signbit(sg1 * sg2));
}

BOOST_DATA_TEST_CASE(zero_normal, sg_v, sg) {
    TEST::SET_FP(3, copysign(0.0, sg));
    TEST::SET_FP(2, 1.1);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B1100111 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(mpfr_zero_p(cpu.FP[2]));
    BOOST_TEST(!!mpfr_signbit(cpu.FP[2]) == !!signbit(sg));
}

BOOST_DATA_TEST_CASE(normal_inf, sg_v, sg) {
    TEST::SET_FP(3, 2.0);
    TEST::SET_FP(2, copysign(INFINITY, sg));
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B1100111 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(mpfr_inf_p(cpu.FP[2]));
    BOOST_TEST(!!mpfr_signbit(cpu.FP[2]) == !!signbit(sg));
}

BOOST_DATA_TEST_CASE(normal_zero, sg_v, sg) {
    TEST::SET_FP(3, 2.0);
    TEST::SET_FP(2, copysign(0.0, sg));
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B1100111 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(mpfr_zero_p(cpu.FP[2]));
    BOOST_TEST(!!mpfr_signbit(cpu.FP[2]) == !!signbit(sg));
}
BOOST_AUTO_TEST_CASE(normal) {
    TEST::SET_FP(3, 2.0);
    TEST::SET_FP(2, 3.0);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B1100111 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(TEST::GET_FP(2) == 6.0);
}

BOOST_AUTO_TEST_CASE(ovfl) {
    mpfr_set_ui_2exp(cpu.FP[3], 1, 1023, MPFR_RNDN);
    mpfr_set_ui_2exp(cpu.FP[2], 1, 1023, MPFR_RNDN);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B1100111 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(mpfr_inf_p(cpu.FP[2]));
    BOOST_TEST(cpu.FPSR.OVFL);
}

BOOST_AUTO_TEST_CASE(unfl) {
    mpfr_set_si_2exp(cpu.FP[3], 1, -1023, MPFR_RNDN);
    mpfr_set_si_2exp(cpu.FP[2], 1, -1023, MPFR_RNDN);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B1100111 | 3 << 10 | 2 << 7);
    decode_and_run();
    BOOST_TEST(cpu.FPSR.UNFL);
}
BOOST_AUTO_TEST_SUITE_END()
