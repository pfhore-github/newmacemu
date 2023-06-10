#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(OR, Prepare)
BOOST_AUTO_TEST_SUITE(ToReg)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0100000 | 3 << 9 | 2);
    cpu.D[3] = 0x30;
    cpu.D[2] = 0x2F;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[3] == 0x3F);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0100000 | 3 << 9 | 2);
    cpu.D[3] = 0x1f;
    cpu.D[2] = 0x80;
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0100000 | 3 << 9 | 2);
    cpu.D[3] = 0x00;
    cpu.D[2] = 0x00;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0100100 | 3 << 9 | 2);
    cpu.D[3] = 0x3030;
    cpu.D[2] = 0x2f2f;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[3] == 0x3F3F);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0100100 | 3 << 9 | 2);
    cpu.D[3] = 0x8000;
    cpu.D[2] = 0x1000;
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0100100 | 3 << 9 | 2);
    cpu.D[3] = 0;
    cpu.D[2] = 0;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0100200 | 3 << 9 | 2);
    cpu.D[3] = 0x30303030;
    cpu.D[2] = 0x2f2f2f2f;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[3] == 0x3f3f3f3f);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0100200 | 3 << 9 | 2);
    cpu.D[3] = 0;
    cpu.D[2] = 0x80000000;
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0100200 | 3 << 9 | 2);
    cpu.D[3] = 0;
    cpu.D[2] = 0;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(ToMem)
BOOST_AUTO_TEST_CASE(Byte) {
    TEST::SET_W(0, 0100400 | 3 << 9 | 022);
    cpu.D[3] = 0x2F;
    cpu.A[2] = 0x1000;
    RAM[0x1000] = 0x30;
    auto i = decode_and_run();
    BOOST_TEST(RAM[0x1000] == 0x3F);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(Byte_err) {
    TEST::SET_W(0, 0100400 | 3 << 9 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}
BOOST_AUTO_TEST_CASE(Word) {
    TEST::SET_W(0, 0100500 | 3 << 9 | 022);
    cpu.D[3] = 0x2F2F;
    TEST::SET_W(0x1000, 0x3030);
    cpu.A[2] = 0x1000;
    auto i = decode_and_run();
    BOOST_TEST(TEST::GET_W(0x1000) == 0x3F3F);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(Word_err) {
    TEST::SET_W(0, 0100500 | 3 << 9 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}

BOOST_AUTO_TEST_CASE(Long) {
    TEST::SET_W(0, 0100600 | 3 << 9 | 022);
    cpu.D[3] = 0x2F2F2F2F;
    TEST::SET_L(0x1000, 0x30303030);
    cpu.A[2] = 0x1000;
    auto i = decode_and_run();
    BOOST_TEST(TEST::GET_L(0x1000) == 0x3F3F3F3F);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(Long_err) {
    TEST::SET_W(0, 0100600 | 3 << 9 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
