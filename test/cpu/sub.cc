#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(SUB, Prepare)
BOOST_AUTO_TEST_SUITE(ToReg)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0110000 | 3 << 9 | 2);
    cpu.D[3] = 43;
    cpu.D[2] = 21;
    int i = decode_and_run();
    BOOST_TEST(cpu.D[3] == 22);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(V1) {
    TEST::SET_W(0, 0110000 | 3 << 9 | 2);
    cpu.D[3] = 0x80;
    cpu.D[2] = 1;
    decode_and_run();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    TEST::SET_W(0, 0110000 | 3 << 9 | 2);
    cpu.D[3] = 0x7f;
    cpu.D[2] = -1;
    decode_and_run();
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(CX) {
    TEST::SET_W(0, 0110000 | 3 << 9 | 2);
    cpu.D[3] = 0;
    cpu.D[2] = 1;
    decode_and_run();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0110000 | 3 << 9 | 2);
    cpu.D[3] = -5;
    cpu.D[2] = 2;
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0110000 | 3 << 9 | 2);
    cpu.D[3] = 3;
    cpu.D[2] = 3;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0110100 | 3 << 9 | 2);
    cpu.D[3] = 6543;
    cpu.D[2] = 3333;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[3] == 3210);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(V1) {
    TEST::SET_W(0, 0110100 | 3 << 9 | 2);
    cpu.D[3] = 0x8000;
    cpu.D[2] = 1;
    decode_and_run();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    TEST::SET_W(0, 0110100 | 3 << 9 | 2);
    cpu.D[3] = 0x7fff;
    cpu.D[2] = -1;
    decode_and_run();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(CX) {
    TEST::SET_W(0, 0110100 | 3 << 9 | 2);
    cpu.D[3] = 0;
    cpu.D[2] = 1;
    decode_and_run();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0110100 | 3 << 9 | 2);
    cpu.D[3] = -222;
    cpu.D[2] = 5;
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0110100 | 3 << 9 | 2);
    cpu.D[3] = 400;
    cpu.D[2] = 400;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0110200 | 3 << 9 | 2);
    cpu.D[3] = 999999;
    cpu.D[2] = 543210;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[3] == 456789);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(V1) {
    TEST::SET_W(0, 0110200 | 3 << 9 | 2);
    cpu.D[3] = 0x80000000;
    cpu.D[2] = 1;
    decode_and_run();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    TEST::SET_W(0, 0110200 | 3 << 9 | 2);
    cpu.D[3] = 0x7fffffff;
    cpu.D[2] = -1;
    decode_and_run();
    BOOST_TEST(cpu.V);
}


BOOST_AUTO_TEST_CASE(CX) {
    TEST::SET_W(0, 0110200 | 3 << 9 | 2);
    cpu.D[3] = 0;
    cpu.D[2] = 1;
    decode_and_run();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0110200 | 3 << 9 | 2);
    cpu.D[3] = -300000;
    cpu.D[2] = 1;
   decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0110200 | 3 << 9 | 2);
    cpu.D[3] = 4444444;
    cpu.D[2] = 4444444;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(ToMem)
BOOST_AUTO_TEST_CASE(Byte) {
    TEST::SET_W(0, 0110400 | 3 << 9 | 022);
    cpu.D[3] = 14;
    cpu.A[2] = 0x1000;
    RAM[0x1000] = 60;
    auto i = decode_and_run();
    BOOST_TEST(RAM[0x1000] == 46);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(Byte_err) {
    TEST::SET_W(0, 0110400 | 3 << 9 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}
BOOST_AUTO_TEST_CASE(Word) {
    TEST::SET_W(0, 0110500 | 3 << 9 | 022);
    cpu.D[3] = 1500;
    TEST::SET_W(0x1000, 4000);
    cpu.A[2] = 0x1000;
    auto i = decode_and_run();
    BOOST_TEST(TEST::GET_W(0x1000) == 2500);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(Word_err) {
    TEST::SET_W(0, 0110500 | 3 << 9 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}

BOOST_AUTO_TEST_CASE(Long) {
    TEST::SET_W(0, 0110600 | 3 << 9 | 022);
    cpu.D[3] = 600000;
    TEST::SET_L(0x1000, 2000000);
    cpu.A[2] = 0x1000;
    auto i = decode_and_run();
    BOOST_TEST(TEST::GET_L(0x1000) == 1400000);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(Long_err) {
    TEST::SET_W(0, 0110600 | 3 << 9 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()