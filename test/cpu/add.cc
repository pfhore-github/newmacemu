#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include "proto.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(ADD, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(ADD_B(14, 21) == 35);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(V1) {
    ADD_B(0x7f, 1);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    ADD_B(0x80, -1);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(CX) {
    ADD_B(0xff, 1);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}


BOOST_AUTO_TEST_CASE(ToReg) {
    TEST::SET_W(0, 0150000 | 3 << 9 | 2);
    cpu.D[3] = 14;
    cpu.D[2] = 21;
    int i = decode_and_run();
    BOOST_TEST(cpu.D[3] == 35);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(ToMem) {
    TEST::SET_W(0, 0150400 | 3 << 9 | 022);
    cpu.D[3] = 14;
    cpu.A[2] = 0x1000;
    RAM[0x1000] = 21;
    auto i = decode_and_run();
    BOOST_TEST(RAM[0x1000] == 35);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(ToMemErr) {
    TEST::SET_W(0, 0150400 | 3 << 9 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(ADD_W(1411, 2151) == 3562);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(V1) {
    ADD_W(0x7fff, 1);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    ADD_W(0x8000, -1);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(CX) {
    ADD_W(0xffff, 1);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}


BOOST_AUTO_TEST_CASE(ToReg) {
    TEST::SET_W(0, 0150100 | 3 << 9 | 2);
    cpu.D[3] = 1420;
    cpu.D[2] = 2133;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[3] == 3553);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(ToMem) {
    TEST::SET_W(0, 0150500 | 3 << 9 | 022);
    cpu.D[3] = 1420;
    TEST::SET_W(0x1000, 2133);
    cpu.A[2] = 0x1000;
    auto i = decode_and_run();
    BOOST_TEST(TEST::GET_W(0x1000) == 3553);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(ToMemErr) {
    TEST::SET_W(0, 0150500 | 3 << 9 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(ADD_L(141135, 215104) == 356239);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(V1) {
    ADD_L(0x7fffffff, 1);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    ADD_L(0x80000000, -1);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(CX) {
    ADD_L(0xffffffff, 1);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}


BOOST_AUTO_TEST_CASE(ToReg) {
    TEST::SET_W(0, 0150200 | 3 << 9 | 2);
    cpu.D[3] = 142044;
    cpu.D[2] = 213324;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[3] == 355368);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(ToMem) {
    TEST::SET_W(0, 0150600 | 3 << 9 | 022);
    cpu.D[3] = 142044;
    TEST::SET_L(0x1000, 213324);
    cpu.A[2] = 0x1000;
    auto i = decode_and_run();
    BOOST_TEST(TEST::GET_L(0x1000) == 355368);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(ToMemErr) {
    TEST::SET_W(0, 0150600 | 3 << 9 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
