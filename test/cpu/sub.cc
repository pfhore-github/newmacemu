#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include "proto.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(SUB, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(SUB_B(49, 13) == 36);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(V1) {
    SUB_B(0x80, 1);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    SUB_B(0x7f, -1);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(CX) {
    SUB_B(0, 1);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(ToReg) {
    TEST::SET_W(0, 0110000 | 3 << 9 | 2);
    cpu.D[3] = 43;
    cpu.D[2] = 21;
    int i = decode_and_run();
    BOOST_TEST(cpu.D[3] == 22);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(ToMem) {
    TEST::SET_W(0, 0110400 | 3 << 9 | 022);
    cpu.D[3] = 14;
    cpu.A[2] = 0x1000;
    RAM[0x1000] = 60;
    auto i = decode_and_run();
    BOOST_TEST(RAM[0x1000] == 46);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(ToMemErr) {
    TEST::SET_W(0, 0110400 | 3 << 9 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(SUB_W(1234, 571) == 663);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(V1) {
    SUB_W(0x8000, 1);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    SUB_W(0x7fff, -1);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(CX) {
    SUB_W(0, 1);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(ToReg) {
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

BOOST_AUTO_TEST_CASE(ToMem) {
    TEST::SET_W(0, 0110500 | 3 << 9 | 022);
    cpu.D[3] = 1500;
    TEST::SET_W(0x1000, 4000);
    cpu.A[2] = 0x1000;
    auto i = decode_and_run();
    BOOST_TEST(TEST::GET_W(0x1000) == 2500);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(ToMemErr) {
    TEST::SET_W(0, 0110500 | 3 << 9 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(SUB_L(1234567, 749541) == 485026);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(V1) {
    SUB_L(0x80000000, 1);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    SUB_L(0x7fffffff, -1);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(CX) {
    SUB_L(0, 1);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(ToReg) {
    TEST::SET_W(0, 0110200 | 3 << 9 | 2);
    cpu.D[3] = 999999;
    cpu.D[2] = 543210;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[3] == 456789);
    BOOST_TEST(i == 0);
}


BOOST_AUTO_TEST_CASE(ToMem) {
    TEST::SET_W(0, 0110600 | 3 << 9 | 022);
    cpu.D[3] = 600000;
    TEST::SET_L(0x1000, 2000000);
    cpu.A[2] = 0x1000;
    auto i = decode_and_run();
    BOOST_TEST(TEST::GET_L(0x1000) == 1400000);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(ToMemErr) {
    TEST::SET_W(0, 0110600 | 3 << 9 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
