#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(FDBcc, Prepare)
BOOST_AUTO_TEST_CASE(Trace) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0xf);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 3;
    cpu.FPSR.CC_Z = true;
    cpu.T = 1;
    un_test(0);
	BOOST_TEST(cpu.ex_n == 9 );
}
BOOST_AUTO_TEST_CASE(notDecement) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0xf);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 3;
    cpu.FPSR.CC_Z = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(cpu.D[1] == 3);
}
BOOST_AUTO_TEST_CASE(countZero) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 0;
    cpu.FPSR.CC_Z = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(cpu.D[1] == 0xffff);
}
BOOST_AUTO_TEST_CASE(countNonZero) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 4;
    cpu.FPSR.CC_Z = true;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(cpu.D[1] == 3);
}
BOOST_AUTO_TEST_SUITE(EQ)
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x1);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_Z = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x1);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_Z = false;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(NE)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0xE);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_Z = false;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0xE);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_Z = true;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GT)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x12);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    cpu.FPSR.CC_N = false;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}
BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x12);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = true;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x12);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_Z = true;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F3) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x12);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_N = true;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(NGT)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x1D);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x1D);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_Z = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(T3) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x1D);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_N = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x1D);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    cpu.FPSR.CC_N = false;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GE)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x13);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_Z = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}
BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x13);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_N = false;
    run_test();
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x13);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_Z = false;
    cpu.FPSR.CC_NAN = true;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x13);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_Z = false;
    cpu.FPSR.CC_N = true;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(NGE)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x1C);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x1C);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_Z = false;
    cpu.FPSR.CC_N = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}

BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x1C);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_N = false;
    cpu.FPSR.CC_NAN = false;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x1C);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_Z = true;
    cpu.FPSR.CC_NAN = false;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(LT)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x14);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_N = true;
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    run_test();
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x14);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_N = false;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
}

BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x14);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = true;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F3) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x14);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_Z = true;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(NLT)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x1B);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x1B);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_Z = true;
    cpu.FPSR.CC_N = false;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}
BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x1B);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x1B);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_N = true;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(LE)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x15);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_Z = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}
BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x15);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_N = true;
    cpu.FPSR.CC_NAN = false;
    run_test();
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x15);
    cpu.FPSR.CC_N = false;
    cpu.FPSR.CC_Z = false;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}

BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x15);
    cpu.FPSR.CC_NAN = true;
    cpu.FPSR.CC_Z = false;
    run_test();
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(NLE)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x1A);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x1A);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_Z = true;
    cpu.FPSR.CC_N = false;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}
BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x1A);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x1A);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_N = true;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GL)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x16);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    run_test();
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x16);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = true;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x16);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_Z = true;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(NGL)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x19);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x19);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_Z = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}

BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x19);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GLE)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x17);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = false;
    run_test();
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x17);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = true;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(NGLE)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x18);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x18);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = false;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(OGT)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x2);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    cpu.FPSR.CC_N = false;
    run_test();
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x2);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = true;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x2);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_Z = true;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
}
BOOST_AUTO_TEST_CASE(F3) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x2);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_N = true;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ULE)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0xD);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0xD);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_Z = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}

BOOST_AUTO_TEST_CASE(T3) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0xD);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_N = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0xD);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    cpu.FPSR.CC_N = false;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(OGE)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x3);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_Z = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}
BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x3);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_N = false;
    run_test();
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x3);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_Z = false;
    cpu.FPSR.CC_NAN = true;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x13);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_Z = false;
    cpu.FPSR.CC_N = true;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ULT)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0xC);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0xC);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_Z = false;
    cpu.FPSR.CC_N = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}

BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0xC);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_N = false;
    cpu.FPSR.CC_NAN = false;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0xC);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_Z = true;
    cpu.FPSR.CC_NAN = false;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(OLT)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x4);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_N = true;
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    run_test();
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x4);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_N = false;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
}

BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x4);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = true;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F3) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x4);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_Z = true;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(UGE)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0xB);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0xB);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_Z = true;
    cpu.FPSR.CC_N = false;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}
BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0xB);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0xB);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_N = true;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(OLE)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x5);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_Z = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}
BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x5);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_N = true;
    cpu.FPSR.CC_NAN = false;
    run_test();
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x5);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_N = false;
    cpu.FPSR.CC_Z = false;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
}

BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x5);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = true;
    cpu.FPSR.CC_Z = false;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(UGT)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0xA);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0xA);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_Z = true;
    cpu.FPSR.CC_N = false;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}
BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0xA);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0xA);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_N = true;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(OGL)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x6);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    run_test();
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x6);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = true;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x6);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_Z = true;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(UEQ)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x9);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x9);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_Z = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}

BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x9);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(OR)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x7);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = false;
    run_test();
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x7);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = true;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(UN)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x8);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x8);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = false;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
}
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0xF);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_Z = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}
BOOST_DATA_TEST_CASE(SF, bdata::xrange(2), is_nan) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x10);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = is_nan;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(cpu.FPSR.BSUN == is_nan);
}

BOOST_DATA_TEST_CASE(ST, bdata::xrange(2), is_nan) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x1F);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_NAN = is_nan;
    run_test();
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(cpu.FPSR.BSUN == is_nan);
}

BOOST_AUTO_TEST_SUITE(SEQ)
BOOST_DATA_TEST_CASE(T, bdata::xrange(2), is_nan) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x11);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_Z = true;
    cpu.FPSR.CC_NAN = is_nan;
    run_test();
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(cpu.FPSR.BSUN == is_nan);
}
BOOST_DATA_TEST_CASE(F, bdata::xrange(2), is_nan) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x11);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_Z = false;
    cpu.FPSR.CC_NAN = is_nan;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(cpu.FPSR.BSUN == is_nan);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(SNE)
BOOST_DATA_TEST_CASE(T, bdata::xrange(2), is_nan) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x1E);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_Z = false;
    cpu.FPSR.CC_NAN = is_nan;
    run_test();
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(cpu.FPSR.BSUN == is_nan);
}
BOOST_DATA_TEST_CASE(F, bdata::xrange(2), is_nan) {
    TEST::SET_W(0, 0171110 | 1);
    TEST::SET_W(2, 0x1E);
    TEST::SET_W(4, 0x10);
    cpu.D[1] = 2;
    cpu.FPSR.CC_Z = true;
    cpu.FPSR.CC_NAN = is_nan;
    run_test();
    BOOST_TEST(cpu.PC == 0x14);
    BOOST_TEST(cpu.FPSR.BSUN == is_nan);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
