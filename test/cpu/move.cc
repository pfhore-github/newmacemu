#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(MOVE, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0010000 | 3 << 9 | 2);
    cpu.D[3] = 0x12345678;
    cpu.D[2] = 0x9ABCDE02;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[3] == 0x12345602);
}

BOOST_AUTO_TEST_CASE(imm) {
    TEST::SET_W(0, 0010074 | 3 << 9);
    TEST::SET_W(2, 0x34);
    cpu.D[3] = 0x12345678;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(cpu.D[3] == 0x12345634);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0030000 | 3 << 9 | 2);
    cpu.D[3] = 0x12345678;
    cpu.D[2] = 0x9ABC0123;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[3] == 0x12340123);
}

BOOST_AUTO_TEST_CASE(imm) {
    TEST::SET_W(0, 0030074 | 3 << 9);
    TEST::SET_W(2, 0x3456);
    cpu.D[3] = 0x12345678;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(cpu.D[3] == 0x12343456);
}

BOOST_AUTO_TEST_CASE(FromCCR) {
    cpu.X = cpu.N = cpu.Z = cpu.V = cpu.C = true;
    TEST::SET_W(0, 0041300 | 3);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[3] == 0x1F);
}

BOOST_AUTO_TEST_CASE(TO_CCR) {
    TEST::SET_W(0, 0042300 | 3);
    cpu.D[3] = 0x1F;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.X);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(cpu.V);
    BOOST_TEST(cpu.N);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_SUITE(FromSR)
BOOST_AUTO_TEST_CASE(NG) {
    cpu.S = false;
    TEST::SET_W(0, 0040300);
    BOOST_TEST(run_test() == 8);
}

BOOST_AUTO_TEST_CASE(OK) {
    cpu.S = true;
    cpu.T = 0;
    cpu.M = false;
    cpu.X = cpu.N = cpu.Z = cpu.V = cpu.C = true;
    TEST::SET_W(0, 0040300 | 3);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.D[3] == 0x201F);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(ToSR)


BOOST_AUTO_TEST_CASE(NG) {
    cpu.S = false;
    TEST::SET_W(0, 0043300);
    BOOST_TEST(run_test() == 8);
}

BOOST_AUTO_TEST_CASE(OK) {
    cpu.S = true;
    cpu.D[3] = 0x1F;
    TEST::SET_W(0, 0043300 | 3);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(!cpu.S);
    BOOST_TEST(cpu.X);
    BOOST_TEST(cpu.N);
    BOOST_TEST(cpu.V);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(Traced) {
    cpu.S = true;
    cpu.T = 1;
    cpu.D[3] = 1 << 14;
    TEST::SET_W(0, 0043300 | 3);
    BOOST_TEST(run_test() == 9);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0020000 | 3 << 9 | 2);
    cpu.D[3] = 0x12345678;
    cpu.D[2] = 0x76543210;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[3] == 0x76543210);
}

BOOST_AUTO_TEST_CASE(imm) {
    TEST::SET_W(0, 0020074 | 3 << 9);
    TEST::SET_L(2, 0xdeadbeaf);
    cpu.D[3] = 0x12345678;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(cpu.D[3] == 0xdeadbeaf);
}
BOOST_AUTO_TEST_SUITE(FomrUSP)

BOOST_AUTO_TEST_CASE(NG) {
    cpu.S = false;
    TEST::SET_W(0, 0047150);
    BOOST_TEST(run_test() == 8);
}

BOOST_AUTO_TEST_CASE(OK) {
    cpu.S = true;
    cpu.USP = 0x3000;
    TEST::SET_W(0, 0047150 | 3);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.A[3] == 0x3000);
}

BOOST_AUTO_TEST_CASE(Traced) {
    cpu.S = true;
    cpu.T = 1;
    cpu.USP = 0x3000;
    TEST::SET_W(0, 0047150 | 3);
    BOOST_TEST(run_test() == 9);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(ToUSP)
BOOST_AUTO_TEST_CASE(NG) {
    cpu.S = false;
    TEST::SET_W(0, 0047140);
    BOOST_TEST(run_test() == 8);
}

BOOST_AUTO_TEST_CASE(OK) {
    cpu.S = true;
    cpu.A[3] = 0x3000;
    TEST::SET_W(0, 0047140 | 3);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.USP == 0x3000);
}

BOOST_AUTO_TEST_CASE(Traced) {
    cpu.S = true;
    cpu.T = 1;
    cpu.A[3] = 0x3000;
    TEST::SET_W(0, 0047140 | 3);
    BOOST_TEST(run_test() == 9);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
