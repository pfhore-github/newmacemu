#define BOOST_TEST_DYN_LINK
#include "inline.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
void load_fpS(uint32_t v);
void load_fpD(uint64_t v);
void load_fpX(uint64_t frac, uint16_t exp);
void load_fpP(uint32_t addr);
void store_fpr(int fpn, FPU_PREC p = FPU_PREC::AUTO);
int32_t fpu_storeL();
int16_t fpu_storeW();
int8_t fpu_storeB();
uint32_t store_fpS();
uint64_t store_fpD();
std::pair<uint64_t, uint16_t> store_fpX();
void store_fpP(uint32_t addr, int k);
BOOST_AUTO_TEST_SUITE(fpu_load)
BOOST_AUTO_TEST_SUITE(S)
BOOST_AUTO_TEST_CASE(zero) {
    load_fpS(0);
    BOOST_TEST(mpfr_zero_p(cpu.fp_tmp));
}

BOOST_DATA_TEST_CASE(fp_inf, boost::unit_test::data::xrange(2), sg) {
    load_fpS(0x7f800000 | sg << 31);
    BOOST_TEST(mpfr_inf_p(cpu.fp_tmp));
    BOOST_TEST(mpfr_sgn(cpu.fp_tmp) == (sg ? -1 : 1));
}

BOOST_AUTO_TEST_CASE(fp_nan) {
    load_fpS(0x7f891234);
    BOOST_TEST(mpfr_nan_p(cpu.fp_tmp));
    BOOST_TEST(cpu.fp_tmp_nan == 0x1224680000000000LLU);
}

BOOST_AUTO_TEST_CASE(denorm) {
    load_fpS(0x200000);
    long e;
    double d = mpfr_get_d_2exp(&e, cpu.fp_tmp, MPFR_RNDN);
    BOOST_TEST(e == -127);
    BOOST_TEST(d == 0.5);
}

BOOST_AUTO_TEST_CASE(norm) {
    load_fpS(0x80400000 | 130 << 23);
    long e;
    double d = mpfr_get_d_2exp(&e, cpu.fp_tmp, MPFR_RNDN);
    BOOST_TEST(e == 4);
    BOOST_TEST(d == -0.75);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(D)
BOOST_AUTO_TEST_CASE(zero) {
    load_fpD(0);
    BOOST_TEST(mpfr_zero_p(cpu.fp_tmp));
}

BOOST_DATA_TEST_CASE(fp_inf, boost::unit_test::data::xrange(2), sg) {
    load_fpD(0x7ffLLU << 52 | static_cast<uint64_t>(sg) << 63);
    BOOST_TEST(mpfr_inf_p(cpu.fp_tmp));
    BOOST_TEST(mpfr_sgn(cpu.fp_tmp) == (sg ? -1 : 1));
}
BOOST_AUTO_TEST_CASE(fp_nan) {
    load_fpD(0x7ff123456789ABCDLLU);
    BOOST_TEST(mpfr_nan_p(cpu.fp_tmp));
    BOOST_TEST(cpu.fp_tmp_nan == 0x123456789ABCD000LLU);
}

BOOST_AUTO_TEST_CASE(denorm) {
    load_fpD(0x4000000000000LLU);
    long e;
    double d = mpfr_get_d_2exp(&e, cpu.fp_tmp, MPFR_RNDN);
    BOOST_TEST(e == -1023);
    BOOST_TEST(d == 0.5);
}

BOOST_AUTO_TEST_CASE(norm) {
    load_fpD(0x8008000000000000LLU | 1028LL << 52);
    long e;
    double d = mpfr_get_d_2exp(&e, cpu.fp_tmp, MPFR_RNDN);
    BOOST_TEST(e == 6);
    BOOST_TEST(d == -0.75);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(X)
BOOST_AUTO_TEST_CASE(zero) {
    load_fpX(0, 0);
    BOOST_TEST(mpfr_zero_p(cpu.fp_tmp));
}

BOOST_DATA_TEST_CASE(fp_inf, boost::unit_test::data::xrange(2), sg) {
    load_fpX(0, 0x7FFF | sg << 15);
    BOOST_TEST(mpfr_inf_p(cpu.fp_tmp));
    BOOST_TEST(mpfr_sgn(cpu.fp_tmp) == (sg ? -1 : 1));
}
BOOST_AUTO_TEST_CASE(fp_nan) {
    load_fpX(0x123456789ABCDEF0LLU, 0x7FFF);
    BOOST_TEST(mpfr_nan_p(cpu.fp_tmp));
    BOOST_TEST(cpu.fp_tmp_nan == 0x123456789ABCDEF0LLU);
}

BOOST_AUTO_TEST_CASE(denorm) {
    load_fpX(0x2000000000000000LLU, 0);
    long e;
    double d = mpfr_get_d_2exp(&e, cpu.fp_tmp, MPFR_RNDN);
    BOOST_TEST(e == -16384);
    BOOST_TEST(d == 0.5);
}

BOOST_AUTO_TEST_CASE(norm) {
    load_fpX(0xC000000000000000LLU, 0x8000 | (16383 + 3));
    long e;
    double d = mpfr_get_d_2exp(&e, cpu.fp_tmp, MPFR_RNDN);
    BOOST_TEST(e == 4);
    BOOST_TEST(d == -0.75);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(P)
BOOST_AUTO_TEST_CASE(zero) {
    RAM[0x1000] = 0;
    RAM[0x1001] = 0;
    RAM[0x1002] = 0;
    RAM[0x1003] = 0;
    RAM[0x1004] = 0;
    RAM[0x1005] = 0;
    RAM[0x1006] = 0;
    RAM[0x1007] = 0;
    RAM[0x1008] = 0;
    RAM[0x1009] = 0;
    RAM[0x100A] = 0;
    RAM[0x100B] = 0;
    load_fpP(0x1000);
    BOOST_TEST(mpfr_zero_p(cpu.fp_tmp));
}
BOOST_DATA_TEST_CASE(fp_inf, boost::unit_test::data::xrange(2), sg) {
    RAM[0x1000] = 0x7f | sg << 7;
    RAM[0x1001] = 0xFF;
    RAM[0x1002] = 0;
    RAM[0x1003] = 0;
    RAM[0x1004] = 0;
    RAM[0x1005] = 0;
    RAM[0x1006] = 0;
    RAM[0x1007] = 0;
    RAM[0x1008] = 0;
    RAM[0x1009] = 0;
    RAM[0x100A] = 0;
    RAM[0x100B] = 0;
    load_fpP(0x1000);
    BOOST_TEST(mpfr_inf_p(cpu.fp_tmp));
    BOOST_TEST(mpfr_sgn(cpu.fp_tmp) == (sg ? -1 : 1));
}
BOOST_AUTO_TEST_CASE(fp_nan) {
    RAM[0x1000] = 0x7f;
    RAM[0x1001] = 0xFF;
    RAM[0x1002] = 0;
    RAM[0x1003] = 0;
    RAM[0x1004] = 0x01;
    RAM[0x1005] = 0x23;
    RAM[0x1006] = 0x45;
    RAM[0x1007] = 0x67;
    RAM[0x1008] = 0x89;
    RAM[0x1009] = 0xab;
    RAM[0x100A] = 0xcd;
    RAM[0x100B] = 0xef;
    load_fpP(0x1000);
    BOOST_TEST(mpfr_nan_p(cpu.fp_tmp));
    BOOST_TEST(cpu.fp_tmp_nan == 0x0123456789abcdefLLU);
}

BOOST_AUTO_TEST_CASE(norm) {
    RAM[0x1000] = 0xC0;
    RAM[0x1001] = 0x12;
    RAM[0x1002] = 0;
    RAM[0x1003] = 0x01;
    RAM[0x1004] = 0x33;
    RAM[0x1005] = 0x33;
    RAM[0x1006] = 0x33;
    RAM[0x1007] = 0x33;
    RAM[0x1008] = 0x33;
    RAM[0x1009] = 0x33;
    RAM[0x100A] = 0x33;
    RAM[0x100B] = 0x33;
    load_fpP(0x1000);
    long e;
    double d = mpfr_get_d_2exp(&e, cpu.fp_tmp, MPFR_RNDN);
    BOOST_TEST(e == -39);
    BOOST_TEST(d == -0.7330077518506667);
    BOOST_TEST(cpu.FPSR.INEX1);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(fpu_round)
BOOST_AUTO_TEST_SUITE(X)

BOOST_AUTO_TEST_CASE(ok) {
    mpfr_clear_flags();
    cpu.FPCR.PREC = FPU_PREC::X;
    mpfr_set_ui_2exp(cpu.fp_tmp, 1, 2, MPFR_RNDN);
    store_fpr(0);
    BOOST_TEST(!cpu.FPSR.OVFL);
    BOOST_TEST(!cpu.FPSR.UNFL);
}

BOOST_AUTO_TEST_CASE(ovfl) {
    mpfr_clear_flags();
    cpu.FPCR.PREC = FPU_PREC::X;
    mpfr_set_ui_2exp(cpu.fp_tmp, 1, 16400, MPFR_RNDN);
    store_fpr(0);
    BOOST_TEST(cpu.FPSR.OVFL);
    BOOST_TEST(mpfr_inf_p(cpu.FP[0]));
}

BOOST_AUTO_TEST_CASE(unfl1) {
    mpfr_clear_flags();
    cpu.FPCR.PREC = FPU_PREC::X;
    mpfr_set_ui_2exp(cpu.fp_tmp, 1, -16400, MPFR_RNDN);
    store_fpr(0);
    BOOST_TEST(cpu.FPSR.UNFL);
    BOOST_TEST(!mpfr_zero_p(cpu.FP[0]));
}

BOOST_AUTO_TEST_CASE(unfl2) {
    mpfr_clear_flags();
    cpu.FPCR.PREC = FPU_PREC::X;
    mpfr_set_ui_2exp(cpu.fp_tmp, 1, -18400, MPFR_RNDN);
    store_fpr(0);
    BOOST_TEST(cpu.FPSR.UNFL);
    BOOST_TEST(mpfr_zero_p(cpu.FP[0]));
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(D)

BOOST_AUTO_TEST_CASE(ok) {
    mpfr_clear_flags();
    cpu.FPCR.PREC = FPU_PREC::D;
    mpfr_set_ui_2exp(cpu.fp_tmp, 1, 2, MPFR_RNDN);
    store_fpr(0);
    BOOST_TEST(!cpu.FPSR.OVFL);
    BOOST_TEST(!cpu.FPSR.UNFL);
}

BOOST_AUTO_TEST_CASE(ovfl) {
    mpfr_clear_flags();
    cpu.FPCR.PREC = FPU_PREC::D;
    mpfr_set_ui_2exp(cpu.fp_tmp, 1, 1050, MPFR_RNDN);
    store_fpr(0);
    BOOST_TEST(cpu.FPSR.OVFL);
    BOOST_TEST(mpfr_inf_p(cpu.FP[0]));
}

BOOST_AUTO_TEST_CASE(unfl1) {
    mpfr_clear_flags();
    cpu.FPCR.PREC = FPU_PREC::D;
    mpfr_set_ui_2exp(cpu.fp_tmp, 1, -1040, MPFR_RNDN);
    store_fpr(0);
    BOOST_TEST(cpu.FPSR.UNFL);
    BOOST_TEST(!mpfr_zero_p(cpu.FP[0]));
}

BOOST_AUTO_TEST_CASE(unfl2) {
    mpfr_clear_flags();
    cpu.FPCR.PREC = FPU_PREC::D;
    mpfr_set_ui_2exp(cpu.fp_tmp, 1, -2000, MPFR_RNDN);
    store_fpr(0);
    BOOST_TEST(cpu.FPSR.UNFL);
    BOOST_TEST(mpfr_zero_p(cpu.FP[0]));
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(S)

BOOST_AUTO_TEST_CASE(ok) {
    mpfr_clear_flags();
    cpu.FPCR.PREC = FPU_PREC::S;
    mpfr_set_ui_2exp(cpu.fp_tmp, 1, 2, MPFR_RNDN);
    store_fpr(0);
    BOOST_TEST(!cpu.FPSR.OVFL);
    BOOST_TEST(!cpu.FPSR.UNFL);
}

BOOST_AUTO_TEST_CASE(ovfl) {
    mpfr_clear_flags();
    cpu.FPCR.PREC = FPU_PREC::S;
    mpfr_set_ui_2exp(cpu.fp_tmp, 1, 140, MPFR_RNDN);
    store_fpr(0);
    BOOST_TEST(cpu.FPSR.OVFL);
    BOOST_TEST(mpfr_inf_p(cpu.FP[0]));
}

BOOST_AUTO_TEST_CASE(unfl1) {
    mpfr_clear_flags();
    cpu.FPCR.PREC = FPU_PREC::S;
    mpfr_set_ui_2exp(cpu.fp_tmp, 1, -130, MPFR_RNDN);
    store_fpr(0);
    BOOST_TEST(cpu.FPSR.UNFL);
    BOOST_TEST(!mpfr_zero_p(cpu.FP[0]));
}

BOOST_AUTO_TEST_CASE(unfl2) {
    mpfr_clear_flags();
    cpu.FPCR.PREC = FPU_PREC::S;
    mpfr_set_ui_2exp(cpu.fp_tmp, 1, -160, MPFR_RNDN);
    store_fpr(0);
    BOOST_TEST(cpu.FPSR.UNFL);
    BOOST_TEST(mpfr_zero_p(cpu.FP[0]));
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(fpu_store)
BOOST_AUTO_TEST_SUITE(S)
BOOST_AUTO_TEST_CASE(zero) {
    mpfr_set_zero(cpu.fp_tmp, 0);
    BOOST_TEST(store_fpS() == 0);
}

BOOST_DATA_TEST_CASE(fp_inf, boost::unit_test::data::xrange(2), sg) {
    mpfr_set_inf(cpu.fp_tmp, sg ? -1 : 1);
    BOOST_TEST(store_fpS() == (0x7f800000 | sg << 31));
}

BOOST_AUTO_TEST_CASE(fp_nan) {
    mpfr_set_nan(cpu.fp_tmp);
    cpu.fp_tmp_nan = 0x1224680000000000LLU;
    BOOST_TEST(store_fpS() == 0x7f891234);
}

BOOST_AUTO_TEST_CASE(denorm) {
    mpfr_set_ui_2exp(cpu.fp_tmp, 3, -130, MPFR_RNDN);
    BOOST_TEST(store_fpS() == 0x180000);
}

BOOST_AUTO_TEST_CASE(norm) {
    mpfr_set_ui_2exp(cpu.fp_tmp, 3, -3, MPFR_RNDN);
    BOOST_TEST(store_fpS() == 0x3EC00000);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(D)
BOOST_AUTO_TEST_CASE(zero) {
    mpfr_set_zero(cpu.fp_tmp, 0);
    BOOST_TEST(store_fpD() == 0);
}

BOOST_DATA_TEST_CASE(fp_inf, boost::unit_test::data::xrange(2), sg) {
    mpfr_set_inf(cpu.fp_tmp, sg ? -1 : 1);
    BOOST_TEST(store_fpD() ==
               (0x7ff0000000000000LLU | static_cast<uint64_t>(sg) << 63));
}
BOOST_AUTO_TEST_CASE(fp_nan) {
    mpfr_set_nan(cpu.fp_tmp);
    cpu.fp_tmp_nan = 0x123456789ABCD000LLU;
    BOOST_TEST(store_fpD() == 0x7ff123456789ABCDLLU);
}

BOOST_AUTO_TEST_CASE(denorm) {
    mpfr_set_ui_2exp(cpu.fp_tmp, 3, -1030, MPFR_RNDN);
    BOOST_TEST(store_fpD() == 0x300000000000);
}

BOOST_AUTO_TEST_CASE(norm) {
    mpfr_set_ui_2exp(cpu.fp_tmp, 3, -3, MPFR_RNDN);
    BOOST_TEST(store_fpD() == 0x3fd8000000000000);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(X)
BOOST_AUTO_TEST_CASE(zero) {
    mpfr_set_zero(cpu.fp_tmp, 0);
    auto [f, e] = store_fpX();
    BOOST_TEST(f == 0);
    BOOST_TEST(e == 0);
}

BOOST_DATA_TEST_CASE(fp_inf, boost::unit_test::data::xrange(2), sg) {
    mpfr_set_inf(cpu.fp_tmp, sg ? -1 : 1);
    auto [f, e] = store_fpX();

    BOOST_TEST(f == 0);
    BOOST_TEST(e == 0x7fff | sg << 15);
}
BOOST_AUTO_TEST_CASE(fp_nan) {
    mpfr_set_nan(cpu.fp_tmp);
    cpu.fp_tmp_nan = 0x123456789ABCDEF0LLU;
    auto [f, e] = store_fpX();
    BOOST_TEST(f == 0x123456789ABCDEF0LLU);
    BOOST_TEST(e == 0x7fff);
}

BOOST_AUTO_TEST_CASE(denorm) {
    mpfr_set_ui_2exp(cpu.fp_tmp, 3, -16385, MPFR_RNDN);
    auto [f, e] = store_fpX();
    BOOST_TEST(f == 0x6000000000000000LLU);
    BOOST_TEST(e == 0);
}

BOOST_AUTO_TEST_CASE(norm) {
    mpfr_set_ui_2exp(cpu.fp_tmp, 3, -2, MPFR_RNDN);
    auto [f, e] = store_fpX();
    BOOST_TEST(f == 0xC000000000000000LLU);
    BOOST_TEST(e == 16382);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(P)
BOOST_AUTO_TEST_CASE(zero) {
    mpfr_set_zero(cpu.fp_tmp, 0);
    store_fpP(0x1000, 0);
    BOOST_TEST(uint32_t(RAM[0x1000]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1001]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1002]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1003]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1004]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1005]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1006]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1007]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1008]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1009]) == 0);
    BOOST_TEST(uint32_t(RAM[0x100A]) == 0);
    BOOST_TEST(uint32_t(RAM[0x100B]) == 0);
}
BOOST_DATA_TEST_CASE(fp_inf, boost::unit_test::data::xrange(2), sg) {
    mpfr_set_inf(cpu.fp_tmp, sg ? -1 : 1);
    store_fpP(0x1000, 0);
    BOOST_TEST(uint32_t(RAM[0x1000]) == (0x7F | sg << 7));
    BOOST_TEST(uint32_t(RAM[0x1001]) == 0xFF);
    BOOST_TEST(uint32_t(RAM[0x1002]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1003]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1004]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1005]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1006]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1007]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1008]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1009]) == 0);
    BOOST_TEST(uint32_t(RAM[0x100A]) == 0);
    BOOST_TEST(uint32_t(RAM[0x100B]) == 0);
}
BOOST_AUTO_TEST_CASE(fp_nan) {
    mpfr_set_nan(cpu.fp_tmp);
    cpu.fp_tmp_nan = 0x123456789ABCDEF0LLU;

    store_fpP(0x1000, 0);
    BOOST_TEST(uint32_t(RAM[0x1000]) == 0x7F);
    BOOST_TEST(uint32_t(RAM[0x1001]) == 0xFF);
    BOOST_TEST(uint32_t(RAM[0x1002]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1003]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1004]) == 0x12);
    BOOST_TEST(uint32_t(RAM[0x1005]) == 0x34);
    BOOST_TEST(uint32_t(RAM[0x1006]) == 0x56);
    BOOST_TEST(uint32_t(RAM[0x1007]) == 0x78);
    BOOST_TEST(uint32_t(RAM[0x1008]) == 0x9A);
    BOOST_TEST(uint32_t(RAM[0x1009]) == 0xBC);
    BOOST_TEST(uint32_t(RAM[0x100A]) == 0xDE);
    BOOST_TEST(uint32_t(RAM[0x100B]) == 0xF0);
}

BOOST_AUTO_TEST_CASE(norm_k_5) {
    mpfr_set_d(cpu.fp_tmp, 12345.678765, MPFR_RNDN);
    store_fpP(0x1000, 5);
    BOOST_TEST(uint32_t(RAM[0x1000]) == 0x00);
    BOOST_TEST(uint32_t(RAM[0x1001]) == 0x04);
    BOOST_TEST(uint32_t(RAM[0x1002]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1003]) == 0x01);
    BOOST_TEST(uint32_t(RAM[0x1004]) == 0x23);
    BOOST_TEST(uint32_t(RAM[0x1005]) == 0x46);
    BOOST_TEST(uint32_t(RAM[0x1006]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1007]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1008]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1009]) == 0);
    BOOST_TEST(uint32_t(RAM[0x100A]) == 0);
    BOOST_TEST(uint32_t(RAM[0x100B]) == 0);
}

BOOST_AUTO_TEST_CASE(norm_k_3) {
    mpfr_set_d(cpu.fp_tmp, 12345.678765, MPFR_RNDN);
    store_fpP(0x1000, 3);
    BOOST_TEST(uint32_t(RAM[0x1000]) == 0x00);
    BOOST_TEST(uint32_t(RAM[0x1001]) == 0x04);
    BOOST_TEST(uint32_t(RAM[0x1002]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1003]) == 0x01);
    BOOST_TEST(uint32_t(RAM[0x1004]) == 0x23);
    BOOST_TEST(uint32_t(RAM[0x1005]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1006]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1007]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1008]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1009]) == 0);
    BOOST_TEST(uint32_t(RAM[0x100A]) == 0);
    BOOST_TEST(uint32_t(RAM[0x100B]) == 0);
}

BOOST_AUTO_TEST_CASE(norm_k_1) {
    mpfr_set_d(cpu.fp_tmp, 12345.678765, MPFR_RNDN);
    store_fpP(0x1000, 1);
    BOOST_TEST(uint32_t(RAM[0x1000]) == 0x00);
    BOOST_TEST(uint32_t(RAM[0x1001]) == 0x04);
    BOOST_TEST(uint32_t(RAM[0x1002]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1003]) == 0x01);
    BOOST_TEST(uint32_t(RAM[0x1004]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1005]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1006]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1007]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1008]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1009]) == 0);
    BOOST_TEST(uint32_t(RAM[0x100A]) == 0);
    BOOST_TEST(uint32_t(RAM[0x100B]) == 0);
}


BOOST_AUTO_TEST_CASE(norm_k_0) {
    mpfr_set_d(cpu.fp_tmp, 12345.678765, MPFR_RNDN);
    store_fpP(0x1000, 0);
    BOOST_TEST(uint32_t(RAM[0x1000]) == 0x00);
    BOOST_TEST(uint32_t(RAM[0x1001]) == 0x04);
    BOOST_TEST(uint32_t(RAM[0x1002]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1003]) == 0x01);
    BOOST_TEST(uint32_t(RAM[0x1004]) == 0x23);
    BOOST_TEST(uint32_t(RAM[0x1005]) == 0x46);
    BOOST_TEST(uint32_t(RAM[0x1006]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1007]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1008]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1009]) == 0);
    BOOST_TEST(uint32_t(RAM[0x100A]) == 0);
    BOOST_TEST(uint32_t(RAM[0x100B]) == 0);
}


BOOST_AUTO_TEST_CASE(norm_k_m1) {
    mpfr_set_d(cpu.fp_tmp, 12345.678765, MPFR_RNDN);
    store_fpP(0x1000, -1);
    BOOST_TEST(uint32_t(RAM[0x1000]) == 0x00);
    BOOST_TEST(uint32_t(RAM[0x1001]) == 0x04);
    BOOST_TEST(uint32_t(RAM[0x1002]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1003]) == 0x01);
    BOOST_TEST(uint32_t(RAM[0x1004]) == 0x23);
    BOOST_TEST(uint32_t(RAM[0x1005]) == 0x45);
    BOOST_TEST(uint32_t(RAM[0x1006]) == 0x70);
    BOOST_TEST(uint32_t(RAM[0x1007]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1008]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1009]) == 0);
    BOOST_TEST(uint32_t(RAM[0x100A]) == 0);
    BOOST_TEST(uint32_t(RAM[0x100B]) == 0);
}


BOOST_AUTO_TEST_CASE(norm_k_m3) {
    mpfr_set_d(cpu.fp_tmp, 12345.678765, MPFR_RNDN);
    store_fpP(0x1000, -3);
    BOOST_TEST(uint32_t(RAM[0x1000]) == 0x00);
    BOOST_TEST(uint32_t(RAM[0x1001]) == 0x04);
    BOOST_TEST(uint32_t(RAM[0x1002]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1003]) == 0x01);
    BOOST_TEST(uint32_t(RAM[0x1004]) == 0x23);
    BOOST_TEST(uint32_t(RAM[0x1005]) == 0x45);
    BOOST_TEST(uint32_t(RAM[0x1006]) == 0x67);
    BOOST_TEST(uint32_t(RAM[0x1007]) == 0x90);
    BOOST_TEST(uint32_t(RAM[0x1008]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1009]) == 0);
    BOOST_TEST(uint32_t(RAM[0x100A]) == 0);
    BOOST_TEST(uint32_t(RAM[0x100B]) == 0);
}

BOOST_AUTO_TEST_CASE(norm_k_m5) {
    mpfr_set_d(cpu.fp_tmp, 12345.678765, MPFR_RNDN);
    store_fpP(0x1000, -5);
    BOOST_TEST(uint32_t(RAM[0x1000]) == 0x00);
    BOOST_TEST(uint32_t(RAM[0x1001]) == 0x04);
    BOOST_TEST(uint32_t(RAM[0x1002]) == 0);
    BOOST_TEST(uint32_t(RAM[0x1003]) == 0x01);
    BOOST_TEST(uint32_t(RAM[0x1004]) == 0x23);
    BOOST_TEST(uint32_t(RAM[0x1005]) == 0x45);
    BOOST_TEST(uint32_t(RAM[0x1006]) == 0x67);
    BOOST_TEST(uint32_t(RAM[0x1007]) == 0x87);
    BOOST_TEST(uint32_t(RAM[0x1008]) == 0x70);
    BOOST_TEST(uint32_t(RAM[0x1009]) == 0);
    BOOST_TEST(uint32_t(RAM[0x100A]) == 0);
    BOOST_TEST(uint32_t(RAM[0x100B]) == 0);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

