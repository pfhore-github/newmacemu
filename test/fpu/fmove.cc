#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <math.h>
namespace bdata = boost::unit_test::data;
auto &operator<<(std::ostream &os, FPU_PREC f) { return os << int(f); }
struct F_FxMOVE {
    F_FxMOVE() {
        // FMOVE.X %FP3, %FP2
        TEST::SET_W(0, 0171000);
        TEST::SET_W(2, 0B0000000 | 3 << 10 | 2 << 7);
        TEST::SET_W(4, TEST_BREAK);

        // FMOVE.L (%A4), %FP2
        TEST::SET_W(6, 0171020 | 4);
        TEST::SET_W(8, 0B0000000 | 1 << 14 | 0 << 10 | 2 << 7);
        TEST::SET_W(10, TEST_BREAK);

        // FMOVE.S (%A4), %FP2
        TEST::SET_W(12, 0171020 | 4);
        TEST::SET_W(14, 0B0000000 | 1 << 14 | 1 << 10 | 2 << 7);
        TEST::SET_W(16, TEST_BREAK);

        // FMOVE.X (%A4), %FP2
        TEST::SET_W(18, 0171020 | 4);
        TEST::SET_W(20, 0B0000000 | 1 << 14 | 2 << 10 | 2 << 7);
        TEST::SET_W(22, TEST_BREAK);

        // FMOVE.P (%A4), %FP2
        TEST::SET_W(24, 0171020 | 4);
        TEST::SET_W(26, 0B0000000 | 1 << 14 | 3 << 10 | 2 << 7);
        TEST::SET_W(28, TEST_BREAK);

        // FMOVE.W (%A4), %FP2
        TEST::SET_W(30, 0171020 | 4);
        TEST::SET_W(32, 0B0000000 | 1 << 14 | 4 << 10 | 2 << 7);
        TEST::SET_W(34, TEST_BREAK);

        // FMOVE.D (%A4), %FP2
        TEST::SET_W(36, 0171020 | 4);
        TEST::SET_W(38, 0B0000000 | 1 << 14 | 5 << 10 | 2 << 7);
        TEST::SET_W(40, TEST_BREAK);

        // FMOVE.D (%A4), %FP2
        TEST::SET_W(42, 0171020 | 4);
        TEST::SET_W(44, 0B0000000 | 1 << 14 | 6 << 10 | 2 << 7);
        TEST::SET_W(46, TEST_BREAK);

        // FSMOVE %FP3, %FP2
        TEST::SET_W(48, 0171000);
        TEST::SET_W(50, 0B1000000 | 3 << 10 | 2 << 7);
        TEST::SET_W(52, TEST_BREAK);

        // FDMOVE %FP3, %FP2
        TEST::SET_W(54, 0171000);
        TEST::SET_W(56, 0B1000100 | 3 << 10 | 2 << 7);
        TEST::SET_W(58, TEST_BREAK);

        jit_compile(0, 60);
    }
};
BOOST_FIXTURE_TEST_SUITE(FxMOVE, Prepare,
                         *boost::unit_test::fixture<F_FxMOVE>())

BOOST_AUTO_TEST_SUITE(FMOVE)
BOOST_AUTO_TEST_SUITE(ToReg)
BOOST_AUTO_TEST_SUITE(RegToReg)
BOOST_AUTO_TEST_CASE(qnan) { qnan_test(0); }

BOOST_AUTO_TEST_CASE(snan) { snan_test(0); }

BOOST_DATA_TEST_CASE(inf, sg_v, sg) {
    TEST::SET_FP(3, copysign(INFINITY, sg));
    run_test(0);
    BOOST_TEST(!!mpfr_signbit(cpu.FP[2]) == !!signbit(sg));
    BOOST_TEST(mpfr_inf_p(cpu.FP[2]));
}

BOOST_DATA_TEST_CASE(zero, sg_v, sg) {
    TEST::SET_FP(3, copysign(0.0, sg));
    run_test(0);
    BOOST_TEST(!!mpfr_signbit(cpu.FP[2]) == !!signbit(sg));
    BOOST_TEST(mpfr_zero_p(cpu.FP[2]));
}

BOOST_AUTO_TEST_CASE(normal) {
    TEST::SET_FP(3, -2.2);
    run_test(0);
    BOOST_TEST(TEST::GET_FP(2) == -2.2);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(L) {
    TEST::SET_L(0x100, -2222222);
    cpu.A[4] = 0x100;
    run_test(6);
    BOOST_TEST(TEST::GET_FP(2) == -2222222);
}
BOOST_AUTO_TEST_SUITE(S)
static inline void run_fp_s(uint32_t v) {
    TEST::SET_L(0x100, v);
    cpu.A[4] = 0x100;
    run_test(12);
}
BOOST_DATA_TEST_CASE(zero, bdata::xrange(2), sg) {
    run_fp_s(sg << 31);
    BOOST_TEST(mpfr_zero_p(cpu.FP[2]));
    BOOST_TEST(!!mpfr_signbit(cpu.FP[2]) == sg);
}
BOOST_DATA_TEST_CASE(fp_inf, boost::unit_test::data::xrange(2), sg) {
    run_fp_s(0x7f800000 | sg << 31);
    BOOST_TEST(mpfr_inf_p(cpu.FP[2]));
    BOOST_TEST(mpfr_sgn(cpu.FP[2]) == (sg ? -1 : 1));
}

BOOST_AUTO_TEST_CASE(fp_nan) {
    run_fp_s(0x7f891234);
    BOOST_TEST(mpfr_nan_p(cpu.FP[2]));
    BOOST_TEST(cpu.FP_nan[2] == 0x5224680000000000LLU);
}

BOOST_AUTO_TEST_CASE(denorm) {
    run_fp_s(0x200000);
    long e;
    double d = mpfr_get_d_2exp(&e, cpu.FP[2], MPFR_RNDN);
    BOOST_TEST(e == -127);
    BOOST_TEST(d == 0.5);
}

BOOST_AUTO_TEST_CASE(norm) {
    run_fp_s(0x80400000 | 130 << 23);
    BOOST_TEST(TEST::GET_FP(2) == -12.0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(X)
static inline void run_fp_X(uint64_t v, uint16_t e) {
    TEST::SET_W(0x100, e);
    TEST::SET_L(0x104, v >> 32);
    TEST::SET_L(0x108, v);
    cpu.A[4] = 0x100;
    run_test(18);
}
BOOST_DATA_TEST_CASE(zero, bdata::xrange(2), sg) {
    run_fp_X(0, sg << 15);
    BOOST_TEST(mpfr_zero_p(cpu.FP[2]));
    BOOST_TEST(!!mpfr_signbit(cpu.FP[2]) == sg);
}

BOOST_DATA_TEST_CASE(fp_inf, boost::unit_test::data::xrange(2), sg) {
    run_fp_X(0, 0x7FFF | sg << 15);
    BOOST_TEST(mpfr_inf_p(cpu.FP[2]));
    BOOST_TEST(mpfr_sgn(cpu.FP[2]) == (sg ? -1 : 1));
}
BOOST_AUTO_TEST_CASE(fp_nan) {
    run_fp_X(0x123456789ABCDEF0LLU, 0x7FFF);
    BOOST_TEST(mpfr_nan_p(cpu.FP[2]));
    BOOST_TEST(cpu.FP_nan[2] == 0x523456789ABCDEF0LLU);
}
BOOST_AUTO_TEST_CASE(denorm) {
    run_fp_X(0x2000000000000000LLU, 0);
    long e;
    double d = mpfr_get_d_2exp(&e, cpu.FP[2], MPFR_RNDN);
    BOOST_TEST(e == -16384);
    BOOST_TEST(d == 0.5);
}

BOOST_AUTO_TEST_CASE(norm) {
    run_fp_X(0xC000000000000000LLU, 0x8000 | (16383 + 3));
    BOOST_TEST(TEST::GET_FP(2) == -12.0);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(P)
static inline void run_fp_P(uint32_t v1, uint32_t v2, uint32_t v3) {
    TEST::SET_L(0x100, v1);
    TEST::SET_L(0x104, v2);
    TEST::SET_L(0x108, v3);
    cpu.A[4] = 0x100;
    run_test(24);
}
BOOST_DATA_TEST_CASE(zero, bdata::xrange(2), sg) {
    run_fp_P(sg << 31, 0, 0);
    BOOST_TEST(mpfr_zero_p(cpu.FP[2]));
    BOOST_TEST(!!mpfr_signbit(cpu.FP[2]) == sg);
}
BOOST_DATA_TEST_CASE(fp_inf, boost::unit_test::data::xrange(2), sg) {
    run_fp_P(sg << 31 | 0x7FFF0000, 0, 0);
    BOOST_TEST(mpfr_inf_p(cpu.FP[2]));
    BOOST_TEST(mpfr_sgn(cpu.FP[2]) == (sg ? -1 : 1));
}
BOOST_AUTO_TEST_CASE(fp_nan) {
    run_fp_P(0x7FFF0000, 0x01234567, 0x89abcdef);
    BOOST_TEST(mpfr_nan_p(cpu.FP[2]));
    BOOST_TEST(cpu.FP_nan[2] == 0x4123456789abcdefLLU);
}

BOOST_AUTO_TEST_CASE(norm) {
    run_fp_P(0xC0120001, 0x33333333, 0x33333333);
    long e;
    double d = mpfr_get_d_2exp(&e, cpu.FP[2], MPFR_RNDN);
    BOOST_TEST(e == -39);
    BOOST_TEST(d == -0.7330077518506667);
    BOOST_TEST(cpu.FPSR.INEX1);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(W) {
    TEST::SET_W(0x100, -2222);
    cpu.A[4] = 0x100;
    run_test(30);
    BOOST_TEST(TEST::GET_FP(2) == -2222);
}

BOOST_AUTO_TEST_SUITE(D)
static inline void run_fp_d(uint64_t v) {
    TEST::SET_L(0x100, v >> 32);
    TEST::SET_L(0x104, v);
    cpu.A[4] = 0x100;
    run_test(36);
}
BOOST_DATA_TEST_CASE(zero, bdata::xrange(2), sg) {
    run_fp_d(static_cast<uint64_t>(sg) << 63);
    BOOST_TEST(mpfr_zero_p(cpu.FP[2]));
    BOOST_TEST(!!mpfr_signbit(cpu.FP[2]) == sg);
}
BOOST_DATA_TEST_CASE(fp_inf, boost::unit_test::data::xrange(2), sg) {
    run_fp_d(0x7ffLLU << 52 | static_cast<uint64_t>(sg) << 63);
    BOOST_TEST(mpfr_inf_p(cpu.FP[2]));
    BOOST_TEST(mpfr_sgn(cpu.FP[2]) == (sg ? -1 : 1));
}

BOOST_AUTO_TEST_CASE(fp_nan) {
    run_fp_d(0x7ff123456789ABCDLLU);
    BOOST_TEST(mpfr_nan_p(cpu.FP[2]));
    BOOST_TEST(cpu.FP_nan[2] == 0x523456789ABCD000LLU);
}

BOOST_AUTO_TEST_CASE(denorm) {
    run_fp_d(0x4000000000000LLU);
    long e;
    double d = mpfr_get_d_2exp(&e, cpu.FP[2], MPFR_RNDN);
    BOOST_TEST(e == -1023);
    BOOST_TEST(d == 0.5);
}

BOOST_AUTO_TEST_CASE(norm) {
    run_fp_d(0x8008000000000000LLU | 1028LL << 52);
    BOOST_TEST(TEST::GET_FP(2) == -48.0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(B) {
    RAM[0x100] = -22;
    cpu.A[4] = 0x100;
    run_test(42);
    BOOST_TEST(TEST::GET_FP(2) == -22);
}
BOOST_AUTO_TEST_SUITE_END()

#if 0
BOOST_AUTO_TEST_SUITE(ToMem)
BOOST_AUTO_TEST_SUITE(L)
BOOST_AUTO_TEST_CASE(inRange) {
    TEST::SET_FP(3, -123456.0);
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 0 << 10 | 3 << 7);
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(TEST::GET_L(0x1000) == -123456);
}

BOOST_AUTO_TEST_CASE(overflowP) {
    TEST::SET_FP(3, 1e100);
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 0 << 10 | 3 << 7);
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(TEST::GET_L(0x1000) == std::numeric_limits<int32_t>::max());
    BOOST_TEST(cpu.FPSR.OPERR);
    BOOST_TEST(cpu.FPSR.EXC_IOP);
}

BOOST_AUTO_TEST_CASE(overflowM) {
    TEST::SET_FP(3, -1e100);
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 0 << 10 | 3 << 7);
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(TEST::GET_L(0x1000) == std::numeric_limits<int32_t>::min());
    BOOST_TEST(cpu.FPSR.OPERR);
    BOOST_TEST(cpu.FPSR.EXC_IOP);
}

BOOST_AUTO_TEST_CASE(inex) {
    TEST::SET_FP(3, 1.1);
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 0 << 10 | 3 << 7);
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(TEST::GET_L(0x1000) == 1);
    BOOST_TEST(cpu.FPSR.INEX2);
    BOOST_TEST(cpu.FPSR.EXC_INEX);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(S)
BOOST_DATA_TEST_CASE(zero, boost::unit_test::data::xrange(2), sg) {
    mpfr_set_zero(cpu.FP[3], sg ? -1 : 1);
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 1 << 10 | 3 << 7);
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(TEST::GET_L(0x1000) == 0x00000000 | sg << 31);
}

BOOST_DATA_TEST_CASE(fp_inf, boost::unit_test::data::xrange(2), sg) {
    mpfr_set_inf(cpu.FP[3], sg ? -1 : 1);
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 1 << 10 | 3 << 7);
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(TEST::GET_L(0x1000) == (0x7f800000 | sg << 31));
}

BOOST_AUTO_TEST_CASE(fp_nan) {
    mpfr_set_nan(cpu.FP[3]);
    cpu.FP_nan[3] = 0x1224680000000000LLU;
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 1 << 10 | 3 << 7);
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(TEST::GET_L(0x1000) == 0x7f891234);
}
BOOST_AUTO_TEST_CASE(denorm) {
    mpfr_set_ui_2exp(cpu.FP[3], 3, -130, MPFR_RNDN);
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 1 << 10 | 3 << 7);
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(TEST::GET_L(0x1000) == 0x180000);
}

BOOST_AUTO_TEST_CASE(norm) {
    mpfr_set_ui_2exp(cpu.FP[3], 3, -3, MPFR_RNDN);
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 1 << 10 | 3 << 7);
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(TEST::GET_L(0x1000) == 0x3EC00000);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(X)
BOOST_DATA_TEST_CASE(zero, boost::unit_test::data::xrange(2), sg) {
    mpfr_set_zero(cpu.FP[3], sg ? -1 : 1);
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 2 << 10 | 3 << 7);
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(TEST::GET_W(0x1000) == sg << 15);
    BOOST_TEST(TEST::GET_L(0x1004) == 0);
    BOOST_TEST(TEST::GET_L(0x1008) == 0);
}
BOOST_DATA_TEST_CASE(fp_inf, boost::unit_test::data::xrange(2), sg) {
    mpfr_set_inf(cpu.FP[3], sg ? -1 : 1);
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 2 << 10 | 3 << 7);
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(TEST::GET_W(0x1000) == 0x7fff | sg << 15);
    BOOST_TEST(TEST::GET_L(0x1004) == 0);
    BOOST_TEST(TEST::GET_L(0x1008) == 0);
}
BOOST_AUTO_TEST_CASE(fp_nan) {
    mpfr_set_nan(cpu.FP[3]);
    cpu.FP_nan[3] = 0x123456789ABCDEF0LLU;
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 2 << 10 | 3 << 7);
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(TEST::GET_W(0x1000) == 0x7fff);
    BOOST_TEST(TEST::GET_L(0x1004) == 0x12345678);
    BOOST_TEST(TEST::GET_L(0x1008) == 0x9ABCDEF0);
}

BOOST_AUTO_TEST_CASE(denorm) {
    mpfr_set_ui_2exp(cpu.FP[3], 3, -16385, MPFR_RNDN);
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 2 << 10 | 3 << 7);
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(TEST::GET_W(0x1000) == 0);
    BOOST_TEST(TEST::GET_L(0x1004) == 0x60000000);
    BOOST_TEST(TEST::GET_L(0x1008) == 0);
}

BOOST_AUTO_TEST_CASE(norm) {
    mpfr_set_ui_2exp(cpu.FP[3], 3, -2, MPFR_RNDN);
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 2 << 10 | 3 << 7);
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(TEST::GET_W(0x1000) == 16382);
    BOOST_TEST(TEST::GET_L(0x1004) == 0xC0000000);
    BOOST_TEST(TEST::GET_L(0x1008) == 0);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(P_static)
BOOST_DATA_TEST_CASE(zero, boost::unit_test::data::xrange(2), sg) {
    mpfr_set_zero(cpu.FP[3], sg ? -1 : 1);
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 3 << 10 | 3 << 7);
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(TEST::GET_L(0x1000) == sg << 31);
    BOOST_TEST(TEST::GET_L(0x1004) == 0);
    BOOST_TEST(TEST::GET_L(0x1008) == 0);
}
BOOST_DATA_TEST_CASE(fp_inf, boost::unit_test::data::xrange(2), sg) {
    mpfr_set_inf(cpu.FP[3], sg ? -1 : 1);
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 3 << 10 | 3 << 7);
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(TEST::GET_L(0x1000) == (0x7FFF0000 | sg << 31));
    BOOST_TEST(TEST::GET_L(0x1004) == 0);
    BOOST_TEST(TEST::GET_L(0x1008) == 0);
}
BOOST_AUTO_TEST_CASE(fp_nan) {
    mpfr_set_nan(cpu.FP[3]);
    cpu.FP_nan[3] = 0x123456789ABCDEF0LLU;
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 3 << 10 | 3 << 7);
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(TEST::GET_L(0x1000) == 0x7FFF0000);
    BOOST_TEST(TEST::GET_L(0x1004) == 0x12345678);
    BOOST_TEST(TEST::GET_L(0x1008) == 0x9ABCDEF0);
}

BOOST_AUTO_TEST_CASE(norm_k_err) {
    mpfr_set_d(cpu.FP[3], 12345.678765432109, MPFR_RNDN);
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 3 << 10 | 3 << 7 | 22);
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(cpu.FPSR.OPERR);
    BOOST_TEST(cpu.FPSR.EXC_IOP);
    BOOST_TEST(TEST::GET_L(0x1000) == 0x00040001);
    BOOST_TEST(TEST::GET_L(0x1004) == 0x23456787);
    BOOST_TEST(TEST::GET_L(0x1008) == 0x65432109);
}

BOOST_AUTO_TEST_CASE(norm_k_5) {
    mpfr_set_d(cpu.FP[3], 12345.678765, MPFR_RNDN);
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 3 << 10 | 3 << 7 | 5);
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(TEST::GET_L(0x1000) == 0x00040001);
    BOOST_TEST(TEST::GET_L(0x1004) == 0x23460000);
    BOOST_TEST(TEST::GET_L(0x1008) == 0);
}

BOOST_AUTO_TEST_CASE(norm_k_3) {
    mpfr_set_d(cpu.FP[3], 12345.678765, MPFR_RNDN);
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 3 << 10 | 3 << 7 | 3);
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(TEST::GET_L(0x1000) == 0x00040001);
    BOOST_TEST(TEST::GET_L(0x1004) == 0x23000000);
    BOOST_TEST(TEST::GET_L(0x1008) == 0);
}

BOOST_AUTO_TEST_CASE(norm_k_1) {
    mpfr_set_d(cpu.FP[3], 12345.678765, MPFR_RNDN);
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 3 << 10 | 3 << 7 | 1);
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(TEST::GET_L(0x1000) == 0x00040001);
    BOOST_TEST(TEST::GET_L(0x1004) == 0);
    BOOST_TEST(TEST::GET_L(0x1008) == 0);
}

BOOST_AUTO_TEST_CASE(norm_k_0) {
    mpfr_set_d(cpu.FP[3], 12345.678765, MPFR_RNDN);
    mpfr_set_d(cpu.FP[3], 12345.678765, MPFR_RNDN);
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 3 << 10 | 3 << 7 | 0);
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(TEST::GET_L(0x1000) == 0x00040001);
    BOOST_TEST(TEST::GET_L(0x1004) == 0x23460000);
    BOOST_TEST(TEST::GET_L(0x1008) == 0);
}

BOOST_AUTO_TEST_CASE(norm_k_m1) {
    mpfr_set_d(cpu.FP[3], 12345.678765, MPFR_RNDN);
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 3 << 10 | 3 << 7 | ((-1) & 0x7f));
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(TEST::GET_L(0x1000) == 0x00040001);
    BOOST_TEST(TEST::GET_L(0x1004) == 0x23457000);
    BOOST_TEST(TEST::GET_L(0x1008) == 0);
}

BOOST_AUTO_TEST_CASE(norm_k_m3) {
    mpfr_set_d(cpu.FP[3], 12345.678765, MPFR_RNDN);
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 3 << 10 | 3 << 7 | ((-3) & 0x7f));
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(TEST::GET_L(0x1000) == 0x00040001);
    BOOST_TEST(TEST::GET_L(0x1004) == 0x23456790);
    BOOST_TEST(TEST::GET_L(0x1008) == 0);
}

BOOST_AUTO_TEST_CASE(norm_k_m5) {
    mpfr_set_d(cpu.FP[3], 12345.678765, MPFR_RNDN);
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 3 << 10 | 3 << 7 | ((-5) & 0x7f));
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(TEST::GET_L(0x1000) == 0x00040001);
    BOOST_TEST(TEST::GET_L(0x1004) == 0x23456787);
    BOOST_TEST(TEST::GET_L(0x1008) == 0x70000000);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(W)
BOOST_AUTO_TEST_CASE(inRange) {
    TEST::SET_FP(3, -1234.0);
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 4 << 10 | 3 << 7);
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(static_cast<int16_t>(TEST::GET_W(0x1000)) == -1234);
}

BOOST_AUTO_TEST_CASE(overflowP) {
    TEST::SET_FP(3, 2000000);
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 4 << 10 | 3 << 7);
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(TEST::GET_W(0x1000) == std::numeric_limits<int16_t>::max());
    BOOST_TEST(cpu.FPSR.OPERR);
    BOOST_TEST(cpu.FPSR.EXC_IOP);
}

BOOST_AUTO_TEST_CASE(overflowM) {
    TEST::SET_FP(3, -2000000);
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 4 << 10 | 3 << 7);
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(static_cast<int16_t>(TEST::GET_W(0x1000)) ==
               std::numeric_limits<int16_t>::min());
    BOOST_TEST(cpu.FPSR.OPERR);
    BOOST_TEST(cpu.FPSR.EXC_IOP);
}

BOOST_AUTO_TEST_CASE(inex) {
    TEST::SET_FP(3, 1.1);
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 4 << 10 | 3 << 7);
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(TEST::GET_W(0x1000) == 1);
    BOOST_TEST(cpu.FPSR.INEX2);
    BOOST_TEST(cpu.FPSR.EXC_INEX);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(D)
BOOST_DATA_TEST_CASE(zero, boost::unit_test::data::xrange(2), sg) {
    mpfr_set_zero(cpu.FP[3], sg ? -1 : 1);
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 5 << 10 | 3 << 7);
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(TEST::GET_L(0x1000) == 0x00000000 | sg << 31);
    BOOST_TEST(TEST::GET_L(0x1004) == 0x00000000 | sg << 31);
}

BOOST_DATA_TEST_CASE(fp_inf, boost::unit_test::data::xrange(2), sg) {
    mpfr_set_inf(cpu.FP[3], sg ? -1 : 1);
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 5 << 10 | 3 << 7);
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(TEST::GET_L(0x1000) == (0x7ff00000 | sg << 31));
    BOOST_TEST(TEST::GET_L(0x1004) == 0);
}

BOOST_AUTO_TEST_CASE(fp_nan) {
    mpfr_set_nan(cpu.FP[3]);
    cpu.FP_nan[3] = 0x123456789ABCD000LLU;
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 5 << 10 | 3 << 7);
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(TEST::GET_L(0x1000) == 0x7ff12345);
    BOOST_TEST(TEST::GET_L(0x1004) == 0x6789ABCD);
}
BOOST_AUTO_TEST_CASE(denorm) {
    mpfr_set_ui_2exp(cpu.FP[3], 3, -1030, MPFR_RNDN);
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 5 << 10 | 3 << 7);
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(TEST::GET_L(0x1000) == 0x3000);
    BOOST_TEST(TEST::GET_L(0x1004) == 0);
}

BOOST_AUTO_TEST_CASE(norm) {
    mpfr_set_ui_2exp(cpu.FP[3], 3, -3, MPFR_RNDN);
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 5 << 10 | 3 << 7);
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(TEST::GET_L(0x1000) == 0x3fd80000);
    BOOST_TEST(TEST::GET_L(0x1004) == 0);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(B)
BOOST_AUTO_TEST_CASE(inRange) {
    TEST::SET_FP(3, -12.0);
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 6 << 10 | 3 << 7);
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(static_cast<int8_t>(RAM[0x1000]) == -12);
}

BOOST_AUTO_TEST_CASE(overflowP) {
    TEST::SET_FP(3, 400);
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 6 << 10 | 3 << 7);
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(static_cast<int8_t>(RAM[0x1000]) ==
               std::numeric_limits<int8_t>::max());
    BOOST_TEST(cpu.FPSR.OPERR);
    BOOST_TEST(cpu.FPSR.EXC_IOP);
}

BOOST_AUTO_TEST_CASE(overflowM) {
    TEST::SET_FP(3, -2000);
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 6 << 10 | 3 << 7);
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(static_cast<int8_t>(RAM[0x1000]) ==
               std::numeric_limits<int8_t>::min());
    BOOST_TEST(cpu.FPSR.OPERR);
    BOOST_TEST(cpu.FPSR.EXC_IOP);
}

BOOST_AUTO_TEST_CASE(inex) {
    TEST::SET_FP(3, 1.1);
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 6 << 10 | 3 << 7);
    cpu.A[4] = 0x1000;
    run_test();
    BOOST_TEST(static_cast<int8_t>(RAM[0x1000]) == 1);
    BOOST_TEST(cpu.FPSR.INEX2);
    BOOST_TEST(cpu.FPSR.EXC_INEX);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(P_dynamic) {

    mpfr_set_d(cpu.FP[3], 12345.678765, MPFR_RNDN);
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 7 << 10 | 3 << 7 | 2 << 4);
    cpu.A[4] = 0x1000;
    cpu.D[2] = -1;
    run_test();
    BOOST_TEST(TEST::GET_L(0x1000) == 0x00040001);
    BOOST_TEST(TEST::GET_L(0x1004) == 0x23457000);
    BOOST_TEST(TEST::GET_L(0x1008) == 0);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(FromCR)

BOOST_AUTO_TEST_CASE(CR) {
    TEST::SET_W(0, 0171000 | 3);
    TEST::SET_W(2, 0120000 | 4 << 10);
    cpu.FPCR.OPERR = true;
    cpu.FPCR.DZ = true;
    cpu.FPCR.PREC = FPU_PREC::S;
    cpu.FPCR.RND = MPFR_RNDZ;
    run_test();
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(cpu.D[3] == 0x2450);
}

BOOST_AUTO_TEST_CASE(SR) {
    TEST::SET_W(0, 0171000 | 3);
    TEST::SET_W(2, 0120000 | 2 << 10);
    cpu.FPSR.CC_Z = true;
    cpu.FPSR.QuatSign = true;
    cpu.FPSR.Quat = 22;
    cpu.FPSR.OVFL = true;
    cpu.FPSR.EXC_IOP = true;
    run_test();
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(cpu.D[3] == (0x4801080 | 22 << 16));
}

BOOST_AUTO_TEST_CASE(IAR) {
    cpu.PC = 0x1000;
    TEST::SET_W(0x1000, 0171000 | 3);
    TEST::SET_W(0x1002, 0120000 | 1 << 10);
    run_test();
    BOOST_TEST(cpu.PC == 0x1004);
    BOOST_TEST(cpu.D[3] == 0x1000);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ToCR)

BOOST_AUTO_TEST_CASE(CR) {
    TEST::SET_W(0, 0171000 | 3);
    TEST::SET_W(2, 0100000 | 4 << 10);
    cpu.D[3] = 0x2450;
    run_test();
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(cpu.FPCR.OPERR);
    BOOST_TEST(cpu.FPCR.DZ);
    BOOST_TEST(cpu.FPCR.PREC == FPU_PREC::S);
    BOOST_TEST(cpu.FPCR.RND == MPFR_RNDZ);
}

BOOST_AUTO_TEST_CASE(SR) {
    TEST::SET_W(0, 0171000 | 3);
    TEST::SET_W(2, 0100000 | 2 << 10);
    cpu.D[3] = (0x4801080 | 22 << 16);
    run_test();
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(cpu.FPSR.CC_Z);
    BOOST_TEST(cpu.FPSR.QuatSign);
    BOOST_TEST(cpu.FPSR.Quat == 22);
    BOOST_TEST(cpu.FPSR.OVFL);
    BOOST_TEST(cpu.FPSR.EXC_IOP);
}

BOOST_AUTO_TEST_CASE(IAR) {
    TEST::SET_W(0, 0171000 | 3);
    TEST::SET_W(2, 0100000 | 1 << 10);
    cpu.D[3] = 0x1000;
    run_test();
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(cpu.FPIAR == 0x1000);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
#endif
BOOST_AUTO_TEST_SUITE(FSMOVE)
BOOST_AUTO_TEST_CASE(qnan) { qnan_test(48); }

BOOST_AUTO_TEST_CASE(snan) { snan_test(48); }

BOOST_DATA_TEST_CASE(inf, sg_v, sg) {
    TEST::SET_FP(3, copysign(INFINITY, sg));
    run_test(48);
    BOOST_TEST(!!mpfr_signbit(cpu.FP[2]) == !!signbit(sg));
    BOOST_TEST(mpfr_inf_p(cpu.FP[2]));
}

BOOST_DATA_TEST_CASE(zero, sg_v, sg) {
    TEST::SET_FP(3, copysign(0.0, sg));
    run_test(48);
    BOOST_TEST(!!mpfr_signbit(cpu.FP[2]) == !!signbit(sg));
    BOOST_TEST(mpfr_zero_p(cpu.FP[2]));
}

BOOST_AUTO_TEST_CASE(normal) {
    TEST::SET_FP(3, -2.2);
    run_test(48);
    BOOST_TEST(TEST::GET_FP(2) == -2.2f, boost::test_tools::tolerance(1e-5));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(FDMOVE)
BOOST_AUTO_TEST_CASE(qnan) { qnan_test(54); }

BOOST_AUTO_TEST_CASE(snan) { snan_test(54); }

BOOST_DATA_TEST_CASE(inf, sg_v, sg) {
    TEST::SET_FP(3, copysign(INFINITY, sg));
    run_test(54);
    BOOST_TEST(!!mpfr_signbit(cpu.FP[2]) == !!signbit(sg));
    BOOST_TEST(mpfr_inf_p(cpu.FP[2]));
}

BOOST_DATA_TEST_CASE(zero, sg_v, sg) {
    TEST::SET_FP(3, copysign(0.0, sg));
    run_test(54);
    BOOST_TEST(!!mpfr_signbit(cpu.FP[2]) == !!signbit(sg));
    BOOST_TEST(mpfr_zero_p(cpu.FP[2]));
}

BOOST_AUTO_TEST_CASE(normal) {
    TEST::SET_FP(3, -2.2);
    run_test(54);
    BOOST_TEST(TEST::GET_FP(2) == -2.2);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()