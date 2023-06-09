#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(FScc, Prepare)
BOOST_AUTO_TEST_SUITE(EQ)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x1);
    cpu.FPSR.CC_Z = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
}
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x1);
    cpu.FPSR.CC_Z = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(NE)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0xE);
    cpu.FPSR.CC_Z = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
}
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0xE);
    cpu.FPSR.CC_Z = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GT)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x12);
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    cpu.FPSR.CC_N = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
}
BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x12);
    cpu.FPSR.CC_NAN = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x12);
    cpu.FPSR.CC_Z = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F3) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x12);
    cpu.FPSR.CC_N = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(NGT)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x1D);
    cpu.FPSR.CC_NAN = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x1D);
    cpu.FPSR.CC_Z = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(T3) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x1D);
    cpu.FPSR.CC_N = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x1D);
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    cpu.FPSR.CC_N = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GE)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x13);
    cpu.FPSR.CC_Z = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
}
BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x13);
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_N = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x13);
    cpu.FPSR.CC_Z = false;
    cpu.FPSR.CC_NAN = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x13);
    cpu.FPSR.CC_Z = false;
    cpu.FPSR.CC_N = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(NGE)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x1C);
    cpu.FPSR.CC_NAN = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x1C);
    cpu.FPSR.CC_Z = false;
    cpu.FPSR.CC_N = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
}

BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x1C);
    cpu.FPSR.CC_N = false;
    cpu.FPSR.CC_NAN = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x1C);
    cpu.FPSR.CC_Z = true;
    cpu.FPSR.CC_NAN = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(LT)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x14);
    cpu.FPSR.CC_N = true;
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x14);
    cpu.FPSR.CC_N = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
}

BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x14);
    cpu.FPSR.CC_NAN = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F3) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x14);
    cpu.FPSR.CC_Z = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(NLT)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x1B);
    cpu.FPSR.CC_NAN = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x1B);
    cpu.FPSR.CC_Z = true;
    cpu.FPSR.CC_N = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
}
BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x1B);
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x1B);
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_N = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(LE)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x15);
    cpu.FPSR.CC_Z = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
}
BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x15);
    cpu.FPSR.CC_N = true;
    cpu.FPSR.CC_NAN = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x15);
    cpu.FPSR.CC_N = false;
    cpu.FPSR.CC_Z = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
}

BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x15);
    cpu.FPSR.CC_NAN = true;
    cpu.FPSR.CC_Z = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(NLE)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x1A);
    cpu.FPSR.CC_NAN = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x1A);
    cpu.FPSR.CC_Z = true;
    cpu.FPSR.CC_N = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
}
BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x1A);
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x1A);
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_N = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GL)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x16);
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x16);
    cpu.FPSR.CC_NAN = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x16);
    cpu.FPSR.CC_Z = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(NGL)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x19);
    cpu.FPSR.CC_NAN = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x19);
    cpu.FPSR.CC_Z = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
}

BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x19);
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GLE)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x17);
    cpu.FPSR.CC_NAN = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x17);
    cpu.FPSR.CC_NAN = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(NGLE)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x18);
    cpu.FPSR.CC_NAN = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x18);
    cpu.FPSR.CC_NAN = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(OGT)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x2);
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    cpu.FPSR.CC_N = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x2);
    cpu.FPSR.CC_NAN = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x2);
    cpu.FPSR.CC_Z = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
}
BOOST_AUTO_TEST_CASE(F3) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x2);
    cpu.FPSR.CC_N = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ULE)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0xD);
    cpu.FPSR.CC_NAN = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0xD);
    cpu.FPSR.CC_Z = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
}

BOOST_AUTO_TEST_CASE(T3) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0xD);
    cpu.FPSR.CC_N = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
}
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0xD);
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    cpu.FPSR.CC_N = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(OGE)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x3);
    cpu.FPSR.CC_Z = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
}
BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x3);
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_N = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x3);
    cpu.FPSR.CC_Z = false;
    cpu.FPSR.CC_NAN = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x13);
    cpu.FPSR.CC_Z = false;
    cpu.FPSR.CC_N = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ULT)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0xC);
    cpu.FPSR.CC_NAN = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0xC);
    cpu.FPSR.CC_Z = false;
    cpu.FPSR.CC_N = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
}

BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0xC);
    cpu.FPSR.CC_N = false;
    cpu.FPSR.CC_NAN = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0xC);
    cpu.FPSR.CC_Z = true;
    cpu.FPSR.CC_NAN = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(OLT)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x4);
    cpu.FPSR.CC_N = true;
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x4);
    cpu.FPSR.CC_N = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
}

BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x4);
    cpu.FPSR.CC_NAN = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F3) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x4);
    cpu.FPSR.CC_Z = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(UGE)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0xB);
    cpu.FPSR.CC_NAN = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0xB);
    cpu.FPSR.CC_Z = true;
    cpu.FPSR.CC_N = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
}
BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0xB);
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0xB);
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_N = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(OLE)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x5);
    cpu.FPSR.CC_Z = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
}
BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x5);
    cpu.FPSR.CC_N = true;
    cpu.FPSR.CC_NAN = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x5);
    cpu.FPSR.CC_N = false;
    cpu.FPSR.CC_Z = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
}

BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x5);
    cpu.FPSR.CC_NAN = true;
    cpu.FPSR.CC_Z = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(UGT)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0xA);
    cpu.FPSR.CC_NAN = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0xA);
    cpu.FPSR.CC_Z = true;
    cpu.FPSR.CC_N = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
}
BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0xA);
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0xA);
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_N = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(OGL)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x6);
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x6);
    cpu.FPSR.CC_NAN = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x6);
    cpu.FPSR.CC_Z = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(UEQ)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x9);
    cpu.FPSR.CC_NAN = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x9);
    cpu.FPSR.CC_Z = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
}

BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x9);
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(OR)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x7);
    cpu.FPSR.CC_NAN = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x7);
    cpu.FPSR.CC_NAN = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(UN)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x8);
    cpu.FPSR.CC_NAN = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x8);
    cpu.FPSR.CC_NAN = false;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0);
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
}
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0xF);
    cpu.FPSR.CC_Z = true;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
}
BOOST_DATA_TEST_CASE(SF, bdata::xrange(2), is_nan) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x10);
    cpu.FPSR.CC_NAN = is_nan;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(cpu.FPSR.BSUN == is_nan);
}

BOOST_DATA_TEST_CASE(ST, bdata::xrange(2), is_nan) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x1F);
    cpu.FPSR.CC_NAN = is_nan;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(cpu.FPSR.BSUN == is_nan);
}

BOOST_AUTO_TEST_SUITE(SEQ)
BOOST_DATA_TEST_CASE(T, bdata::xrange(2), is_nan) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x11);
    cpu.FPSR.CC_Z = true;
    cpu.FPSR.CC_NAN = is_nan;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(cpu.FPSR.BSUN == is_nan);
}
BOOST_DATA_TEST_CASE(F, bdata::xrange(2), is_nan) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x11);
    cpu.FPSR.CC_Z = false;
    cpu.FPSR.CC_NAN = is_nan;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(cpu.FPSR.BSUN == is_nan);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(SNE)
BOOST_DATA_TEST_CASE(T, bdata::xrange(2), is_nan) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x1E);
    cpu.FPSR.CC_Z = false;
    cpu.FPSR.CC_NAN = is_nan;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(cpu.FPSR.BSUN == is_nan);
}
BOOST_DATA_TEST_CASE(F, bdata::xrange(2), is_nan) {
    TEST::SET_W(0, 0171100 | 1);
    TEST::SET_W(2, 0x1E);
    cpu.FPSR.CC_Z = true;
    cpu.FPSR.CC_NAN = is_nan;
    decode_and_run();
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(cpu.FPSR.BSUN == is_nan);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
