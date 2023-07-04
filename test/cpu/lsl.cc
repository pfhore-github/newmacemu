#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include "proto.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(LSL, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(LSL_B(3, 2) == 12);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(C0) {
    cpu.C = cpu.X = true;
    BOOST_TEST(LSL_B(3, 0) == 3);
    BOOST_TEST(cpu.X);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(CX) {
    LSL_B(0x80, 1);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}


BOOST_AUTO_TEST_CASE(byReg) {
    cpu.D[2] = 3;
    cpu.D[3] = 2;
    TEST::SET_W(0, 0160450 | 3 << 9 | 2);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 12);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(byRegMod) {
    cpu.D[2] = 3;
    cpu.D[3] = 66;
    TEST::SET_W(0, 0160450 | 3 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 12);
}

BOOST_AUTO_TEST_CASE(byImm) {
    cpu.D[2] = 3;
    TEST::SET_W(0, 0160410 | 2 << 9 | 2);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 12);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(byImm0) {
    cpu.D[2] = 3;
    TEST::SET_W(0, 0160410 | 0 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0);
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(LSL_W(300, 2) == 1200);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(C0) {
    cpu.C = cpu.X = true;
    BOOST_TEST(LSL_W(300, 0) == 300);
    BOOST_TEST(cpu.X);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(CX) {
    LSL_W(0x8000, 1);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}


BOOST_AUTO_TEST_CASE(mem) {
    TEST::SET_W(0x1000, 300);
    TEST::SET_W(0, 0161720 | 2);
    cpu.A[2] = 0x1000;
    auto i = decode_and_run();
    BOOST_TEST(TEST::GET_W(0x1000) == 600);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(byReg) {
    cpu.D[2] = 300;
    cpu.D[3] = 2;
    TEST::SET_W(0, 0160550 | 3 << 9 | 2);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 1200);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(byRegMod) {
    cpu.D[2] = 300;
    cpu.D[3] = 66;
    TEST::SET_W(0, 0160550 | 3 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 1200);
}

BOOST_AUTO_TEST_CASE(byImm) {
    cpu.D[2] = 300;
    TEST::SET_W(0, 0160510 | 2 << 9 | 2);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 1200);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(byImm0) {
    cpu.D[2] = 3;
    TEST::SET_W(0, 0160510 | 0 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x300);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(LSL_L(300000, 2) == 1200000);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(C0) {
    cpu.C = cpu.X = true;
    BOOST_TEST(LSL_L(30000, 0) == 30000);
    BOOST_TEST(cpu.X);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(CX) {
    LSL_L(0x80000000, 1);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_CASE(byReg) {
    cpu.D[2] = 30000;
    cpu.D[3] = 2;
    TEST::SET_W(0, 0160650 | 3 << 9 | 2);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 120000);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(byRegMod) {
    cpu.D[2] = 30000;
    cpu.D[3] = 66;
    TEST::SET_W(0, 0160650 | 3 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 120000);
}

BOOST_AUTO_TEST_CASE(byImm) {
    cpu.D[2] = 30000;
    TEST::SET_W(0, 0160610 | 2 << 9 | 2);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 120000);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(byImm0) {
    cpu.D[2] = 0x1000;
    TEST::SET_W(0, 0160610 | 0 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x100000);
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()