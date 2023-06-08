#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(ADD, Prepare)
BOOST_AUTO_TEST_SUITE(ToReg)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0150000 | 3 << 9 | 2);
    cpu.D[3] = 14;
    cpu.D[2] = 21;
    int i = decode_and_run();
    BOOST_TEST(cpu.D[3] == 35);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(V1) {
    TEST::SET_W(0, 0150000 | 3 << 9 | 2);
    cpu.D[3] = 127;
    cpu.D[2] = 3;
    decode_and_run();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    TEST::SET_W(0, 0150000 | 3 << 9 | 2);
    cpu.D[3] = -126;
    cpu.D[2] = -5;
    decode_and_run();
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(CX) {
    TEST::SET_W(0, 0150000 | 3 << 9 | 2);
    cpu.D[3] = 0xfc;
    cpu.D[2] = 12;
    decode_and_run();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0150000 | 3 << 9 | 2);
    cpu.D[3] = 0xfc;
    cpu.D[2] = 1;
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0150000 | 3 << 9 | 2);
    cpu.D[3] = 0xff;
    cpu.D[2] = 1;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0150100 | 3 << 9 | 2);
    cpu.D[3] = 1420;
    cpu.D[2] = 2133;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[3] == 3553);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(V1) {
    TEST::SET_W(0, 0150100 | 3 << 9 | 2);
    cpu.D[3] = 0x7fff;
    cpu.D[2] = 3;
    decode_and_run();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    TEST::SET_W(0, 0150100 | 3 << 9 | 2);
    cpu.D[3] = 0x8000;
    cpu.D[2] = 0xffff;
    decode_and_run();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(CX) {
    TEST::SET_W(0, 0150100 | 3 << 9 | 2);
    cpu.D[3] = 0xfffc;
    cpu.D[2] = 12;
    decode_and_run();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0150100 | 3 << 9 | 2);
    cpu.D[3] = 0xfffc;
    cpu.D[2] = 1;
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0150100 | 3 << 9 | 2);
    cpu.D[3] = 0xffff;
    cpu.D[2] = 1;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0150200 | 3 << 9 | 2);
    cpu.D[3] = 142044;
    cpu.D[2] = 213324;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[3] == 355368);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(V1) {
    TEST::SET_W(0, 0150200 | 3 << 9 | 2);
    cpu.D[3] = 0x7fffffff;
    cpu.D[2] = 3;
    decode_and_run();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    TEST::SET_W(0, 0150200 | 3 << 9 | 2);
    cpu.D[3] = -1;
    cpu.D[2] = 0x80000000;
    decode_and_run();
    BOOST_TEST(cpu.V);
}


BOOST_AUTO_TEST_CASE(CX) {
    TEST::SET_W(0, 0150200 | 3 << 9 | 2);
    cpu.D[3] = 0xfffffffc;
    cpu.D[2] = 12;
    decode_and_run();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0150200 | 3 << 9 | 2);
    cpu.D[3] = 0xfffffffc;
    cpu.D[2] = 1;
   decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0150200 | 3 << 9 | 2);
    cpu.D[3] = 0xffffffff;
    cpu.D[2] = 1;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(ToMem)
BOOST_AUTO_TEST_CASE(Byte) {
    TEST::SET_W(0, 0150400 | 3 << 9 | 022);
    cpu.D[3] = 14;
    cpu.A[2] = 0x1000;
    RAM[0x1000] = 21;
    auto i = decode_and_run();
    BOOST_TEST(RAM[0x1000] == 35);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(Byte_err) {
    TEST::SET_W(0, 0150400 | 3 << 9 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}
BOOST_AUTO_TEST_CASE(Word) {
    TEST::SET_W(0, 0150500 | 3 << 9 | 022);
    cpu.D[3] = 1420;
    TEST::SET_W(0x1000, 2133);
    cpu.A[2] = 0x1000;
    auto i = decode_and_run();
    BOOST_TEST(TEST::GET_W(0x1000) == 3553);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(Word_err) {
    TEST::SET_W(0, 0150500 | 3 << 9 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}

BOOST_AUTO_TEST_CASE(Long) {
    TEST::SET_W(0, 0150600 | 3 << 9 | 022);
    cpu.D[3] = 142044;
    TEST::SET_L(0x1000, 213324);
    cpu.A[2] = 0x1000;
    auto i = decode_and_run();
    BOOST_TEST(TEST::GET_L(0x1000) == 355368);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(Long_err) {
    TEST::SET_W(0, 0150600 | 3 << 9 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
