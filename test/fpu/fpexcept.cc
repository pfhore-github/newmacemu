#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(FP_exception, Prepare)


BOOST_AUTO_TEST_CASE(BSUN) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x2000;
    TEST::SET_L(0x2000 | (48 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x100C;
    cpu.EA = 0x4500;
    cpu.nextpc = 0x4;
    TEST::SET_W(0, 0171200 | 0x12);
    TEST::SET_W(2, 0x100);
    cpu.FPCR.BSUN = true;
    cpu.FPSR.CC_NAN = true;
    decode_and_run();

    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x4);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x3000 | (48 << 2)));
    BOOST_TEST(TEST::GET_L(0x1008) == 0x4500);
    BOOST_TEST(cpu.nextpc == 0x3000);
}

BOOST_AUTO_TEST_CASE(INEX1) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x2000;
    TEST::SET_L(0x2000 | (49 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x100C;
    cpu.PC = 0;
    cpu.nextpc = 0x4;
    cpu.FPCR.INEX1 = true;

    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 0B0000000 | 1 << 14 | 3 << 10 | 2 << 7);
    TEST::SET_L(0x1000, 0xC0120001);
    TEST::SET_L(0x1004, 0x33333333);
    TEST::SET_L(0x1008, 0x33333333);
    cpu.A[4] = 0x1000;
    decode_and_run();

    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x4);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x3000 | (49 << 2)));
    BOOST_TEST(TEST::GET_L(0x1008) == 0x1000);
    BOOST_TEST(cpu.nextpc == 0x3000);
}

BOOST_AUTO_TEST_CASE(INEX2) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x2000;
    TEST::SET_L(0x2000 | (49 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x100C;
    cpu.PC = 0;
    cpu.nextpc = 0x4;
    cpu.FPCR.INEX2 = true;

    TEST::SET_FP(3, 1.1);
    TEST::SET_W(0, 0171020 | 4);
    TEST::SET_W(2, 3 << 13 | 0 << 10 | 3 << 7);
    cpu.A[4] = 0x1000;
    decode_and_run();

    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x4);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x3000 | (49 << 2)));
    BOOST_TEST(TEST::GET_L(0x1008) == 0x1000);
    BOOST_TEST(cpu.nextpc == 0x3000);
}
BOOST_AUTO_TEST_CASE(DV0) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x2000;
    TEST::SET_L(0x2000 | (50 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x100C;
    cpu.PC = 0;
    cpu.nextpc = 0x4;
    cpu.EA = 0x4500;

    cpu.FPCR.DZ = true;
    TEST::SET_FP(3, 2.0);
    TEST::SET_FP(2, copysign(0.0, 1));
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0100000 | 3 << 10 | 2 << 7);

    decode_and_run();    
    
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x4);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x3000 | (50 << 2)));
    BOOST_TEST(TEST::GET_L(0x1008) == 0x4500);
    BOOST_TEST(cpu.nextpc == 0x3000);
}

BOOST_AUTO_TEST_CASE(UNFL) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x2000;
    TEST::SET_L(0x2000 | (51 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x100C;
    cpu.PC = 0;
    cpu.nextpc = 0x4;
    cpu.EA = 0x4500;

    cpu.FPCR.UNFL = true;
    mpfr_set_si_2exp(cpu.FP[3], 1, -16385, MPFR_RNDN);
    mpfr_set_si_2exp(cpu.FP[2], 1, -16385, MPFR_RNDN);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0100011 | 3 << 10 | 2 << 7);
    decode_and_run();

    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x4);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x3000 | (51 << 2)));
    BOOST_TEST(TEST::GET_L(0x1008) == 0x4500);
    BOOST_TEST(cpu.nextpc == 0x3000);
}

BOOST_AUTO_TEST_CASE(OPERR) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x2000;
    TEST::SET_L(0x2000 | (52 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x100C;
    cpu.PC = 0;
    cpu.nextpc = 0x4;
    cpu.EA = 0x4500;

    cpu.FPCR.OPERR = true;
    TEST::SET_FP(3, copysign(INFINITY, 1));
    TEST::SET_FP(2, copysign(INFINITY, -1));
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0100010 | 3 << 10 | 2 << 7);
    decode_and_run();

    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x4);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x3000 | (52 << 2)));
    BOOST_TEST(TEST::GET_L(0x1008) == 0x4500);
    BOOST_TEST(cpu.nextpc == 0x3000);
}

BOOST_AUTO_TEST_CASE(OVFL) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x2000;
    TEST::SET_L(0x2000 | (53 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x100C;
    cpu.PC = 0;
    cpu.nextpc = 0x4;
    cpu.EA = 0x4500;

    cpu.FPCR.OVFL = true;
    mpfr_set_ui_2exp(cpu.FP[3], 1, 16383, MPFR_RNDN);
    mpfr_set_ui_2exp(cpu.FP[2], 1, 16383, MPFR_RNDN);
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B0100011 | 3 << 10 | 2 << 7);
    decode_and_run();

    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x4);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x3000 | (53 << 2)));
    BOOST_TEST(TEST::GET_L(0x1008) == 0x4500);
    BOOST_TEST(cpu.nextpc == 0x3000);
}

BOOST_AUTO_TEST_CASE(SNAN_) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x2000;
    TEST::SET_L(0x2000 | (54 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x100C;
    cpu.PC = 0;
    cpu.nextpc = 0x4;
    cpu.EA = 0x4500;

    cpu.FPCR.S_NAN = true;
    mpfr_set_nan(cpu.FP[3]);
    cpu.FP_nan[3] = 0x0fff000000000000LLU;
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0B1011000 | 3 << 10 | 2 << 7);
    decode_and_run();

    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x4);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x3000 | (54 << 2)));
    BOOST_TEST(TEST::GET_L(0x1008) == 0x4500);
    BOOST_TEST(cpu.nextpc == 0x3000);
}
BOOST_AUTO_TEST_SUITE_END()