#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include "proto.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(ROR, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(ROR_B(0x89, 2) == 0x62);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(C) {
    ROR_B(1, 1);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(c0) {
    cpu.C = true;
    ROR_B(1, 0);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(byReg) {
    cpu.D[2] = 0x34;
    cpu.D[3] = 4;
    TEST::SET_W(0, 0160070 | 3 << 9 | 2);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x43);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(byRegMod) {
    cpu.D[2] = 0x12;
    cpu.D[3] = 68;
    TEST::SET_W(0, 0160070 | 3 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x21);
}

BOOST_AUTO_TEST_CASE(byImm) {
    cpu.D[2] = 0x12;
    TEST::SET_W(0, 0160030 | 4 << 9 | 2);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x21);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(byImm0) {
    cpu.D[2] = 0x12;
    TEST::SET_W(0, 0160030 | 0 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x12);
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(ROR_W(0x4321, 4) == 0x1432);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(C) {
    ROR_W(0x100, 9);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(c0) {
    cpu.C = true;
    ROR_W(0x100, 0);
    BOOST_TEST(!cpu.C);
}
BOOST_AUTO_TEST_CASE(mem) {
    TEST::SET_W(0x1000, 1);
    TEST::SET_W(0, 0163320 | 2);
    cpu.A[2] = 0x1000;
    auto i = decode_and_run();
    BOOST_TEST(TEST::GET_W(0x1000) == 0x8000);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(byReg) {
    cpu.D[2] = 0x1230;
    cpu.D[3] = 4;
    TEST::SET_W(0, 0160170 | 3 << 9 | 2);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x0123);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(byRegMod) {
    cpu.D[2] = 0x1234;
    cpu.D[3] = 68;
    TEST::SET_W(0, 0160170 | 3 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x4123);
}

BOOST_AUTO_TEST_CASE(byImm) {
    cpu.D[2] = 0x1234;
    TEST::SET_W(0, 0160130 | 4 << 9 | 2);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x4123);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(byImm0) {
    cpu.D[2] = 0x1234;
    TEST::SET_W(0, 0160130 | 0 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x3412);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(ROR_L(0x43210987, 4) == 0x74321098);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(C) {
    ROR_L(0x10000, 17);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(c0) {
    cpu.C = true;
    ROR_L(0x10000, 0);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(byReg) {
    cpu.D[2] = 0x12345670;
    cpu.D[3] = 4;
    TEST::SET_W(0, 0160270 | 3 << 9 | 2);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x01234567);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(byRegMod) {
    cpu.D[2] = 0x12345678;
    cpu.D[3] = 68;
    TEST::SET_W(0, 0160270 | 3 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x81234567);
}

BOOST_AUTO_TEST_CASE(byImm) {
    cpu.D[2] = 0x12345678;
    TEST::SET_W(0, 0160230 | 4 << 9 | 2);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x81234567);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(byImm0) {
    cpu.D[2] = 0x12345678;
    TEST::SET_W(0, 0160230 | 0 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x78123456);
}


BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()