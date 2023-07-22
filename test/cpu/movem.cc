#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(MOVEM, Prepare)
BOOST_AUTO_TEST_SUITE(ToMem)
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_SUITE(base)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 044222);
    TEST::SET_W(2, 0b10010111'10010111);
    BOOST_TEST(disasm() == "MOVEM.W %D0-%D2/%D4/%D7-%A2/%A4/%A7, (%A2)");
}

BOOST_AUTO_TEST_CASE(run) {
    TEST::SET_W(0, 0044220 | 3);
    TEST::SET_W(2, 0b0101010101010101);
    cpu.D[0] = 0x0001;
    cpu.D[2] = 0x0002;
    cpu.D[4] = 0x0003;
    cpu.D[6] = 0x0004;
    cpu.A[0] = 0x1000;
    cpu.A[2] = 0x1001;
    cpu.A[4] = 0x1002;
    cpu.A[6] = 0x1003;
    cpu.A[3] = 0x2000;
    auto i = decode_and_run();
    BOOST_TEST(cpu.A[3] == 0x2000);
    BOOST_TEST(TEST::GET_W(0x2000) == 0x0001);
    BOOST_TEST(TEST::GET_W(0x2002) == 0x0002);
    BOOST_TEST(TEST::GET_W(0x2004) == 0x0003);
    BOOST_TEST(TEST::GET_W(0x2006) == 0x0004);
    BOOST_TEST(TEST::GET_W(0x2008) == 0x1000);
    BOOST_TEST(TEST::GET_W(0x200A) == 0x1001);
    BOOST_TEST(TEST::GET_W(0x200C) == 0x1002);
    BOOST_TEST(TEST::GET_W(0x200E) == 0x1003);
    BOOST_TEST(i == 2);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(decr)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 044242);
    TEST::SET_W(2, 0b10010111'10010111);
    BOOST_TEST(disasm() == "MOVEM.W %A7-%A5/%A3/%A0-%D5/%D3/%D0, -(%A2)");
}

BOOST_AUTO_TEST_CASE(run) {
    TEST::SET_W(0, 0044240 | 3);
    TEST::SET_W(2, 0b1010101010101010);
    cpu.D[0] = 0x0001;
    cpu.D[2] = 0x0002;
    cpu.D[4] = 0x0003;
    cpu.D[6] = 0x0004;
    cpu.A[0] = 0x1000;
    cpu.A[2] = 0x1001;
    cpu.A[4] = 0x1002;
    cpu.A[6] = 0x1003;
    cpu.A[3] = 0x2010;
    auto i = decode_and_run();
    BOOST_TEST(TEST::GET_W(0x2000) == 0x0001);
    BOOST_TEST(TEST::GET_W(0x2002) == 0x0002);
    BOOST_TEST(TEST::GET_W(0x2004) == 0x0003);
    BOOST_TEST(TEST::GET_W(0x2006) == 0x0004);
    BOOST_TEST(TEST::GET_W(0x2008) == 0x1000);
    BOOST_TEST(TEST::GET_W(0x200A) == 0x1001);
    BOOST_TEST(TEST::GET_W(0x200C) == 0x1002);
    BOOST_TEST(TEST::GET_W(0x200E) == 0x1003);
    BOOST_TEST(cpu.A[3] == 0x2000);
    BOOST_TEST(i == 2);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_SUITE(base)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 044322);
    TEST::SET_W(2, 0b10010111'10010111);
    BOOST_TEST(disasm() == "MOVEM.L %D0-%D2/%D4/%D7-%A2/%A4/%A7, (%A2)");
}
BOOST_AUTO_TEST_CASE(run) {
    TEST::SET_W(0, 0044320 | 3);
    TEST::SET_W(2, 0b0101010101010101);
    cpu.D[0] = 0x00000001;
    cpu.D[2] = 0x00000002;
    cpu.D[4] = 0x00000003;
    cpu.D[6] = 0x00000004;
    cpu.A[0] = 0x10000000;
    cpu.A[2] = 0x10000001;
    cpu.A[4] = 0x10000002;
    cpu.A[6] = 0x10000003;
    cpu.A[3] = 0x2000;
    auto i = decode_and_run();
    BOOST_TEST(TEST::GET_L(0x2000) == 0x00000001);
    BOOST_TEST(TEST::GET_L(0x2004) == 0x00000002);
    BOOST_TEST(TEST::GET_L(0x2008) == 0x00000003);
    BOOST_TEST(TEST::GET_L(0x200C) == 0x00000004);
    BOOST_TEST(TEST::GET_L(0x2010) == 0x10000000);
    BOOST_TEST(TEST::GET_L(0x2014) == 0x10000001);
    BOOST_TEST(TEST::GET_L(0x2018) == 0x10000002);
    BOOST_TEST(TEST::GET_L(0x201C) == 0x10000003);
    BOOST_TEST(cpu.A[3] == 0x2000);
    BOOST_TEST(i == 2);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(decr)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 044342);
    TEST::SET_W(2, 0b10010111'10010111);
    BOOST_TEST(disasm() == "MOVEM.L %A7-%A5/%A3/%A0-%D5/%D3/%D0, -(%A2)");
}

BOOST_AUTO_TEST_CASE(decr) {
    TEST::SET_W(0, 0044340 | 3);
    TEST::SET_W(2, 0b1010101010101010);
    cpu.D[0] = 0x00000001;
    cpu.D[2] = 0x00000002;
    cpu.D[4] = 0x00000003;
    cpu.D[6] = 0x00000004;
    cpu.A[0] = 0x10000000;
    cpu.A[2] = 0x10000001;
    cpu.A[4] = 0x10000002;
    cpu.A[6] = 0x10000003;
    cpu.A[3] = 0x2020;
    auto i = decode_and_run();
    BOOST_TEST(TEST::GET_L(0x2000) == 0x00000001);
    BOOST_TEST(TEST::GET_L(0x2004) == 0x00000002);
    BOOST_TEST(TEST::GET_L(0x2008) == 0x00000003);
    BOOST_TEST(TEST::GET_L(0x200C) == 0x00000004);
    BOOST_TEST(TEST::GET_L(0x2010) == 0x10000000);
    BOOST_TEST(TEST::GET_L(0x2014) == 0x10000001);
    BOOST_TEST(TEST::GET_L(0x2018) == 0x10000002);
    BOOST_TEST(TEST::GET_L(0x201C) == 0x10000003);
    BOOST_TEST(cpu.A[3] == 0x2000);
    BOOST_TEST(i == 2);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ToReg)
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0046223);
    TEST::SET_W(2, 0b10010111'10010111);
    BOOST_TEST(disasm() == "MOVEM.W (%A3), %D0-%D2/%D4/%D7-%A2/%A4/%A7");
}
BOOST_AUTO_TEST_CASE(addr) {
    TEST::SET_W(0, 0046220 | 3);
    TEST::SET_W(2, 0b0101010101010101);

    TEST::SET_W(0x2000, 0x0001);
    TEST::SET_W(0x2002, 0x0002);
    TEST::SET_W(0x2004, 0x0003);
    TEST::SET_W(0x2006, 0x0004);
    TEST::SET_W(0x2008, 0x1000);
    TEST::SET_W(0x200A, 0x1001);
    TEST::SET_W(0x200C, 0x1002);
    TEST::SET_W(0x200E, 0x1003);
    cpu.A[3] = 0x2000;
    auto i = decode_and_run();
    BOOST_TEST(cpu.A[3] == 0x2000);
    BOOST_TEST(cpu.D[0] == 0x0001);
    BOOST_TEST(cpu.D[2] == 0x0002);
    BOOST_TEST(cpu.D[4] == 0x0003);
    BOOST_TEST(cpu.D[6] == 0x0004);
    BOOST_TEST(cpu.A[0] == 0x1000);
    BOOST_TEST(cpu.A[2] == 0x1001);
    BOOST_TEST(cpu.A[4] == 0x1002);
    BOOST_TEST(cpu.A[6] == 0x1003);
    BOOST_TEST(i == 2);
}

BOOST_AUTO_TEST_CASE(incr) {
    TEST::SET_W(0, 0046230 | 3);
    TEST::SET_W(2, 0b0101010101010101);

    TEST::SET_W(0x2000, 0x0001);
    TEST::SET_W(0x2002, 0x0002);
    TEST::SET_W(0x2004, 0x0003);
    TEST::SET_W(0x2006, 0x0004);
    TEST::SET_W(0x2008, 0x1000);
    TEST::SET_W(0x200A, 0x1001);
    TEST::SET_W(0x200C, 0x1002);
    TEST::SET_W(0x200E, 0x1003);
    cpu.A[3] = 0x2000;
    auto i = decode_and_run();
    BOOST_TEST(cpu.A[3] == 0x2010);
    BOOST_TEST(cpu.D[0] == 0x0001);
    BOOST_TEST(cpu.D[2] == 0x0002);
    BOOST_TEST(cpu.D[4] == 0x0003);
    BOOST_TEST(cpu.D[6] == 0x0004);
    BOOST_TEST(cpu.A[0] == 0x1000);
    BOOST_TEST(cpu.A[2] == 0x1001);
    BOOST_TEST(cpu.A[4] == 0x1002);
    BOOST_TEST(cpu.A[6] == 0x1003);
    BOOST_TEST(i == 2);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0046323);
    TEST::SET_W(2, 0b10010111'10010111);
    BOOST_TEST(disasm() == "MOVEM.L (%A3), %D0-%D2/%D4/%D7-%A2/%A4/%A7");
}

BOOST_AUTO_TEST_CASE(addr) {
    TEST::SET_W(0, 0046320 | 3);
    TEST::SET_W(2, 0b0101010101010101);

    TEST::SET_L(0x2000, 0x00000001);
    TEST::SET_L(0x2004, 0x00000002);
    TEST::SET_L(0x2008, 0x00000003);
    TEST::SET_L(0x200C, 0x00000004);
    TEST::SET_L(0x2010, 0x10000000);
    TEST::SET_L(0x2014, 0x10000001);
    TEST::SET_L(0x2018, 0x10000002);
    TEST::SET_L(0x201C, 0x10000003);
    cpu.A[3] = 0x2000;
    auto i = decode_and_run();
    BOOST_TEST(cpu.A[3] == 0x2000);
    BOOST_TEST(cpu.D[0] == 0x00000001);
    BOOST_TEST(cpu.D[2] == 0x00000002);
    BOOST_TEST(cpu.D[4] == 0x00000003);
    BOOST_TEST(cpu.D[6] == 0x00000004);
    BOOST_TEST(cpu.A[0] == 0x10000000);
    BOOST_TEST(cpu.A[2] == 0x10000001);
    BOOST_TEST(cpu.A[4] == 0x10000002);
    BOOST_TEST(cpu.A[6] == 0x10000003);
    BOOST_TEST(i == 2);
}

BOOST_AUTO_TEST_CASE(incr) {
   TEST::SET_W(0, 0046330 | 3);
    TEST::SET_W(2, 0b0101010101010101);

    TEST::SET_L(0x2000, 0x00000001);
    TEST::SET_L(0x2004, 0x00000002);
    TEST::SET_L(0x2008, 0x00000003);
    TEST::SET_L(0x200C, 0x00000004);
    TEST::SET_L(0x2010, 0x10000000);
    TEST::SET_L(0x2014, 0x10000001);
    TEST::SET_L(0x2018, 0x10000002);
    TEST::SET_L(0x201C, 0x10000003);
    cpu.A[3] = 0x2000;
    auto i = decode_and_run();
    BOOST_TEST(cpu.A[3] == 0x2020);
    BOOST_TEST(cpu.D[0] == 0x00000001);
    BOOST_TEST(cpu.D[2] == 0x00000002);
    BOOST_TEST(cpu.D[4] == 0x00000003);
    BOOST_TEST(cpu.D[6] == 0x00000004);
    BOOST_TEST(cpu.A[0] == 0x10000000);
    BOOST_TEST(cpu.A[2] == 0x10000001);
    BOOST_TEST(cpu.A[4] == 0x10000002);
    BOOST_TEST(cpu.A[6] == 0x10000003);
    BOOST_TEST(i == 2);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
