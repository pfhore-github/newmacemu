#define BOOST_TEST_DYN_LINK
#include "inline.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_AUTO_TEST_SUITE(BFINS)
BOOST_AUTO_TEST_CASE(Err) {
    TEST::SET_W(0, 0167700|  072 );
    cpu.PC = 0;
    BOOST_CHECK_THROW(decode(), DecodeError);
}
BOOST_AUTO_TEST_CASE(both_imm) {
    TEST::SET_W(0, 0167700 | 2);
    TEST::SET_W(2, 5 << 12 | 8 << 6 | 4);
    cpu.D[2] = 0x12345678;
    cpu.D[5] = 0x7;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.X = cpu.V = cpu.C = true;
    f();
    BOOST_TEST(i == 2);
    BOOST_TEST(cpu.D[2] == 0x12745678);
    BOOST_TEST(cpu.X);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0167700 | 2);
    TEST::SET_W(2, 5 << 12 | 8 << 6 | 4);
    cpu.D[2] = 0x12345678;
    cpu.D[5] = 0xF;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.X = cpu.V = cpu.C = true;
    f();
    BOOST_TEST(cpu.D[2] == 0x12F45678);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0167700 | 2);
    TEST::SET_W(2, 5 << 12 | 8 << 6 | 4);
    cpu.D[2] = 0x12345678;
    cpu.D[5] = 0;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.D[2] == 0x12045678);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(off_r) {
    TEST::SET_W(0, 0167700 | 2);
    TEST::SET_W(2, 5 << 12 | 1 << 11 | 3 << 6 | 4);
    cpu.D[2] = 0x12345678;
    cpu.D[3] = 8;
    cpu.D[5] = 0xF;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.D[2] == 0x12F45678);
}

BOOST_AUTO_TEST_CASE(width_r) {
    TEST::SET_W(0, 0167700 | 2);
    TEST::SET_W(2, 5 << 12 | 1 << 5 | 8 << 6 | 3);
    cpu.D[2] = 0x12345678;
    cpu.D[3] = 4;
    cpu.D[5] = 0xF;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.D[2] == 0x12F45678);
}

BOOST_AUTO_TEST_CASE(both_r) {
    TEST::SET_W(0, 0167700 | 2);
    TEST::SET_W(2, 4 << 12 | 1 << 5 | 1 << 11 | 3 << 6 | 5);
    cpu.D[2] = 0x12345678;
    cpu.D[3] = 8;
    cpu.D[5] = 4;
    cpu.D[4] = 0xF;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.D[2] == 0x12F45678);
}

BOOST_AUTO_TEST_CASE(width0) {
    TEST::SET_W(0, 0167700 | 2);
    TEST::SET_W(2, 5 << 12 | 4 << 6);
    cpu.D[2] = 0x12345678;
    cpu.D[5] = 0x9ABCDEF0;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.D[2] == 0x09ABCDEF);
}

BOOST_AUTO_TEST_CASE(width_r0) {
    TEST::SET_W(0, 0167700 | 2);
    TEST::SET_W(2, 5 << 12 | 4 << 6 | 1 << 5 | 3);
    cpu.D[2] = 0x12345678;
    cpu.D[3] = 0;
    cpu.D[5] = 0x9ABCDEF0;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.X = cpu.V = cpu.C = true;
    f();
    BOOST_TEST(cpu.D[2] == 0x09ABCDEF);
}

BOOST_AUTO_TEST_CASE(off_neg) {
    TEST::SET_W(0, 0167720 | 2);
    TEST::SET_W(2, 5 << 12 | 1 << 11 | 3 << 6 | 4);
    cpu.A[2] = 0x1002;
    TEST::SET_L(0x1000, 0x12345678);
    cpu.D[3] = -12;
    cpu.D[5] = 0xF;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(TEST::GET_L(0x1000) == 0x1F345678);
}
BOOST_AUTO_TEST_CASE(byte1) {
    TEST::SET_W(0, 0167720 | 2);
    TEST::SET_W(2, 5 << 12 | 4 << 6 | 4);
    cpu.A[2] = 0x1000;
    RAM[0x1000] = 0x12;
    cpu.D[5] = 0xF;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(RAM[0x1000] == 0x1F);
}

BOOST_AUTO_TEST_CASE(byte2) {
    TEST::SET_W(0, 0167720 | 2);
    TEST::SET_W(2, 5 << 12 | 4 << 6 | 8);
    cpu.A[2] = 0x1000;
    RAM[0x1000] = 0x12;
    RAM[0x1001] = 0x34;
    cpu.D[5] = 0xFF;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(RAM[0x1000] == 0x1F);
    BOOST_TEST(RAM[0x1001] == 0xF4);
}

BOOST_AUTO_TEST_CASE(byte3) {
    TEST::SET_W(0, 0167720 | 2);
    TEST::SET_W(2, 5 << 12 | 4 << 6 | 16);
    cpu.A[2] = 0x1000;
    RAM[0x1000] = 0x12;
    RAM[0x1001] = 0x34;
    RAM[0x1002] = 0x56;
    cpu.D[5] = 0xFFFF;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(RAM[0x1000] == 0x1F);
    BOOST_TEST(RAM[0x1001] == 0xFF);
    BOOST_TEST(RAM[0x1002] == 0xF6);
}

BOOST_AUTO_TEST_CASE(byte4) {
    TEST::SET_W(0, 0167720 | 2);
    TEST::SET_W(2, 5 << 12 | 4 << 6 | 24);
    cpu.A[2] = 0x1000;
    RAM[0x1000] = 0x12;
    RAM[0x1001] = 0x34;
    RAM[0x1002] = 0x56;
    RAM[0x1003] = 0x78;
    cpu.D[5] = 0xFFFFFF;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(RAM[0x1000] == 0x1F);
    BOOST_TEST(RAM[0x1001] == 0xFF);
    BOOST_TEST(RAM[0x1002] == 0xFF);
    BOOST_TEST(RAM[0x1003] == 0xF8);
}

BOOST_AUTO_TEST_CASE(byte5) {
    TEST::SET_W(0, 0167720 | 2);
    TEST::SET_W(2, 5 << 12 |4 << 6 | 0);
    cpu.A[2] = 0x1000;
    RAM[0x1000] = 0x12;
    RAM[0x1001] = 0x34;
    RAM[0x1002] = 0x56;
    RAM[0x1003] = 0x78;
    RAM[0x1004] = 0x9A;
    cpu.D[5] = 0xFFFFFFFF;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(RAM[0x1000] == 0x1F);
    BOOST_TEST(RAM[0x1001] == 0xFF);
    BOOST_TEST(RAM[0x1002] == 0xFF);
    BOOST_TEST(RAM[0x1003] == 0xFF);
    BOOST_TEST(RAM[0x1004] == 0xFA);
}
BOOST_AUTO_TEST_SUITE_END()