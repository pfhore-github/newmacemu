#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "proto.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(BFINS, Prepare)
BOOST_AUTO_TEST_SUITE(D)
BOOST_AUTO_TEST_CASE(normal) {
    BOOST_TEST(BFINS_D(0x12345678, 8, 8, 0xAB) == 0x12AB5678);
}

BOOST_AUTO_TEST_CASE(overwrap) {
    BOOST_TEST(BFINS_D(0x12345678, 28, 8, 0xAB) == 0xB234567A);
}

BOOST_AUTO_TEST_CASE(negative) {
    BOOST_TEST(BFINS_D(0x12345678, -4, 8, 0xAB) == 0xB234567A);
}

BOOST_AUTO_TEST_CASE(zero) {
    BOOST_TEST(BFINS_D(0x12345678, 4, 0, 0x9ABCDEF0) == 0x09ABCDEF);
}

BOOST_AUTO_TEST_CASE(both_imm) {
    TEST::SET_W(0, 0167700 | 2);
    TEST::SET_W(2, 5 << 12 | 8 << 6 | 4);
    cpu.D[2] = 0x12345678;
    cpu.D[5] = 0x7;
    cpu.V = cpu.C = true;
    auto i = decode_and_run();
    BOOST_TEST(i == 2);
    BOOST_TEST(cpu.D[2] == 0x12745678);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_CASE(off_r) {
    TEST::SET_W(0, 0167700 | 2);
    TEST::SET_W(2, 5 << 12 | 1 << 11 | 3 << 6 | 4);
    cpu.D[2] = 0x12345678;
    cpu.D[3] = 8;
    cpu.D[5] = 0xF;
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x12F45678);
}

BOOST_AUTO_TEST_CASE(width_r) {
    TEST::SET_W(0, 0167700 | 2);
    TEST::SET_W(2, 5 << 12 | 1 << 5 | 8 << 6 | 3);
    cpu.D[2] = 0x12345678;
    cpu.D[3] = 4;
    cpu.D[5] = 0xF;
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x12F45678);
}

BOOST_AUTO_TEST_CASE(both_r) {
    TEST::SET_W(0, 0167700 | 2);
    TEST::SET_W(2, 4 << 12 | 1 << 5 | 1 << 11 | 3 << 6 | 5);
    cpu.D[2] = 0x12345678;
    cpu.D[3] = 8;
    cpu.D[5] = 4;
    cpu.D[4] = 0xF;
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x12F45678);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(m)
BOOST_AUTO_TEST_CASE(byte1) {
    TEST::SET_L(0x1000, 0x12345678);
    TEST::SET_L(0x1004, 0x9ABCDEF0);
    BFINS_M(0x1000, 4, 4, 0x8);
    BOOST_TEST(TEST::GET_L(0x1000) == 0x18345678);
}

BOOST_AUTO_TEST_CASE(byte2) {
    TEST::SET_L(0x1000, 0x12345678);
    TEST::SET_L(0x1004, 0x9ABCDEF0);
    BFINS_M(0x1000, 4, 8, 0x89);
    BOOST_TEST(TEST::GET_L(0x1000) == 0x18945678);
}

BOOST_AUTO_TEST_CASE(byte3) {
    TEST::SET_L(0x1000, 0x12345678);
    TEST::SET_L(0x1004, 0x9ABCDEF0);
    BFINS_M(0x1000, 4, 16, 0x89AB);
    BOOST_TEST(TEST::GET_L(0x1000) == 0x189AB678);
}

BOOST_AUTO_TEST_CASE(byte4) {
    TEST::SET_L(0x1000, 0x12345678);
    TEST::SET_L(0x1004, 0x9ABCDEF0);
    BFINS_M(0x1000, 4, 24, 0x89ABCD);
    BOOST_TEST(TEST::GET_L(0x1000) == 0x189ABCD8);
}

BOOST_AUTO_TEST_CASE(byte5) {
    TEST::SET_L(0x1000, 0x12345678);
    TEST::SET_L(0x1004, 0x9ABCDEF0);
    BFINS_M(0x1000, 4, 0, 0x89ABCDEF);
    BOOST_TEST(TEST::GET_L(0x1000) == 0x189ABCDE);
    BOOST_TEST(TEST::GET_L(0x1004) == 0xFABCDEF0);
}

BOOST_AUTO_TEST_CASE(off_neg) {
    TEST::SET_L(0x1000, 0x12345678);
    BFINS_M(0x1004, -12, 4, 0xF);
    BOOST_TEST(TEST::GET_L(0x1000) == 0x12345F78);
}
// Z; see bftst/Z
// N; see bftst/N
BOOST_AUTO_TEST_CASE(both_imm) {
    TEST::SET_W(0, 0167720 | 2);
    TEST::SET_W(2, 5 << 12 | 8 << 6 | 4);
    TEST::SET_L(0x1000, 0x12345678);
    cpu.A[2] = 0x1000;
    cpu.D[5] = 0x7;
    auto i = decode_and_run();
    BOOST_TEST(i == 2);
    BOOST_TEST(TEST::GET_L(0x1000) == 0x12745678);
}

BOOST_AUTO_TEST_CASE(off_r) {
    TEST::SET_W(0, 0167720 | 2);
    TEST::SET_W(2, 5 << 12 | 1 << 11 | 3 << 6 | 4);
    TEST::SET_L(0x1000, 0x12345678);
    cpu.A[2] = 0x1000;
    cpu.D[3] = 8;
    cpu.D[5] = 0xF;
    decode_and_run();
    BOOST_TEST(TEST::GET_L(0x1000) == 0x12F45678);
}

BOOST_AUTO_TEST_CASE(width_r) {
    TEST::SET_W(0, 0167720 | 2);
    TEST::SET_W(2, 5 << 12 | 1 << 5 | 8 << 6 | 3);
    TEST::SET_L(0x1000, 0x12345678);
    cpu.A[2] = 0x1000;
    cpu.D[3] = 4;
    cpu.D[5] = 0xF;
    decode_and_run();
    BOOST_TEST(TEST::GET_L(0x1000) == 0x12F45678);
}

BOOST_AUTO_TEST_CASE(both_r) {
    TEST::SET_W(0, 0167720 | 2);
    TEST::SET_W(2, 4 << 12 | 1 << 5 | 1 << 11 | 3 << 6 | 5);
    TEST::SET_L(0x1000, 0x12345678);
    cpu.A[2] = 0x1000;
    cpu.D[3] = 8;
    cpu.D[5] = 4;
    cpu.D[4] = 0xF;
    decode_and_run();
    BOOST_TEST(TEST::GET_L(0x1000) == 0x12F45678);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(Err) {
    TEST::SET_W(0, 0167700 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}

BOOST_AUTO_TEST_SUITE_END()