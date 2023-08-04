#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include "proto.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(ROL, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(ROL_B(0x87, 2) == 0x1E);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(C) {
    ROL_B(0x80, 1);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(c0) {
    cpu.C = true;
    ROL_B(0x80, 0);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(byReg) {
    cpu.D[2] = 0x21;
    cpu.D[3] = 4;
    TEST::SET_W(0, 0160470 | 3 << 9 | 2);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[2] == 0x12);
}

BOOST_AUTO_TEST_CASE(byRegMod) {
    cpu.D[2] = 0x21;
    cpu.D[3] = 68;
    TEST::SET_W(0, 0160470 | 3 << 9 | 2);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.D[2] == 0x12);
}

BOOST_AUTO_TEST_CASE(byImm) {
    cpu.D[2] = 0x21;
    TEST::SET_W(0, 0160430 | 4 << 9 | 2);
     BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[2] == 0x12);
}

BOOST_AUTO_TEST_CASE(byImm0) {
    cpu.D[2] = 3;
    TEST::SET_W(0, 0160430 | 0 << 9 | 2);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.D[2] == 3);
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(ROL_W(0x4321, 4) == 0x3214);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(C) {
    ROL_W(0x8000, 1);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(c0) {
    cpu.C = true;
    ROL_W(0x8000, 0);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(mem) {
    TEST::SET_W(0x1000, 0x8000);
    TEST::SET_W(0, 0163720 | 2);
    cpu.A[2] = 0x1000;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(TEST::GET_W(0x1000) == 1);
}
BOOST_AUTO_TEST_CASE(byReg) {
    cpu.D[2] = 0x2468;
    cpu.D[3] = 4;
    TEST::SET_W(0, 0160570 | 3 << 9 | 2);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[2] == 0x4682);
}

BOOST_AUTO_TEST_CASE(byRegMod) {
    cpu.D[2] = 0x1234;
    cpu.D[3] = 68;
    TEST::SET_W(0, 0160570 | 3 << 9 | 2);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.D[2] == 0x2341);
}

BOOST_AUTO_TEST_CASE(byImm) {
    cpu.D[2] = 0x1234;
    TEST::SET_W(0, 0160530 | 4 << 9 | 2);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[2] == 0x2341);
}

BOOST_AUTO_TEST_CASE(byImm0) {
    cpu.D[2] = 0x1234;
    TEST::SET_W(0, 0160530 | 0 << 9 | 2);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.D[2] == 0x3412);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(ROL_L(0x43210987, 4) == 0x32109874);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(C) {
    ROL_L(0x80000000, 1);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(c0) {
    cpu.C = true;
    ROL_L(0x80000000, 0);
    BOOST_TEST(!cpu.C);
}
BOOST_AUTO_TEST_CASE(byReg) {
    cpu.D[2] = 0xE2345678;
    cpu.D[3] = 4;
    TEST::SET_W(0, 0160670 | 3 << 9 | 2);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[2] == 0x2345678E);
}

BOOST_AUTO_TEST_CASE(byRegMod) {
    cpu.D[2] = 0x12345678;
    cpu.D[3] = 68;
    TEST::SET_W(0, 0160670 | 3 << 9 | 2);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.D[2] == 0x23456781);
}

BOOST_AUTO_TEST_CASE(byImm) {
    cpu.D[2] = 0x12345678;
    TEST::SET_W(0, 0160630 | 4 << 9 | 2);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[2] == 0x23456781);
}

BOOST_AUTO_TEST_CASE(byImm0) {
    cpu.D[2] = 0x12345678;
    TEST::SET_W(0, 0160630 | 0 << 9 | 2);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.D[2] == 0x34567812);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
