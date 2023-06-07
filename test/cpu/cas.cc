#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(CAS, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(error) {
    TEST::SET_W(0, 0005300 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0005320 | 2);
    TEST::SET_W(2, 3 << 6 | 4);
    RAM[0x1000] = 0x22;
    cpu.A[2] = 0x1000;
    cpu.D[3] = 0x30;
    cpu.D[4] = 0x22;
    auto i = decode_and_run();
    BOOST_TEST(RAM[0x1000] == 0x30);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 2);
}

BOOST_AUTO_TEST_CASE(C) {
    TEST::SET_W(0, 0005320 | 2);
    TEST::SET_W(2, 3 << 6 | 4);
    RAM[0x1000] = 0x22;
    cpu.A[2] = 0x1000;
    cpu.D[3] = 0x30;
    cpu.D[4] = 0x25;
    decode_and_run();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.D[4] == 0x22);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0005320 | 2);
    TEST::SET_W(2, 3 << 6 | 4);
    RAM[0x1000] = 0xE0;
    cpu.A[2] = 0x1000;
    cpu.D[3] = 0x30;
    cpu.D[4] = 1;
    decode_and_run();
    BOOST_TEST(cpu.N);
    BOOST_TEST(cpu.D[4] == 0xE0);
}

BOOST_AUTO_TEST_CASE(V) {
    TEST::SET_W(0, 0005320 | 2);
    TEST::SET_W(2, 3 << 6 | 4);
    RAM[0x1000] = 0x80;
    cpu.A[2] = 0x1000;
    cpu.D[3] = 0x30;
    cpu.D[4] = 1;
    decode_and_run();
    BOOST_TEST(cpu.V);
    BOOST_TEST(cpu.D[4] == 0x80);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(error) {
    TEST::SET_W(0, 0006300 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0006320 | 2);
    TEST::SET_W(2, 3 << 6 | 4);
    TEST::SET_W(0x1000, 0x2222);
    cpu.A[2] = 0x1000;
    cpu.D[3] = 0x3030;
    cpu.D[4] = 0x2222;
    auto i = decode_and_run();
    BOOST_TEST(TEST::GET_W(0x1000) == 0x3030);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 2);
}

BOOST_AUTO_TEST_CASE(C) {
    TEST::SET_W(0, 0006320 | 2);
    TEST::SET_W(2, 3 << 6 | 4);
    TEST::SET_W(0x1000, 0x2222);
    cpu.A[2] = 0x1000;
    cpu.D[3] = 0x3030;
    cpu.D[4] = 0x2525;
    decode_and_run();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.D[4] == 0x2222);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0006320 | 2);
    TEST::SET_W(2, 3 << 6 | 4);
    TEST::SET_W(0x1000, 0xE000);
    cpu.A[2] = 0x1000;
    cpu.D[3] = 0x3000;
    cpu.D[4] = 1;
    decode_and_run();
    BOOST_TEST(cpu.N);
    BOOST_TEST(cpu.D[4] == 0xE000);
}

BOOST_AUTO_TEST_CASE(V) {
    TEST::SET_W(0, 0006320 | 2);
    TEST::SET_W(2, 3 << 6 | 4);
    TEST::SET_W(0x1000, 0x8000);
    cpu.A[2] = 0x1000;
    cpu.D[3] = 0x3000;
    cpu.D[4] = 1;
    decode_and_run();
    BOOST_TEST(cpu.V);
    BOOST_TEST(cpu.D[4] == 0x8000);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(error) {
    TEST::SET_W(0, 0007300 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0007320 | 2);
    TEST::SET_W(2, 3 << 6 | 4);
    TEST::SET_L(0x1000, 0x22222222);
    cpu.A[2] = 0x1000;
    cpu.D[3] = 0x30303030;
    cpu.D[4] = 0x22222222;
    auto i = decode_and_run();
    BOOST_TEST(TEST::GET_L(0x1000) == 0x30303030);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 2);
}

BOOST_AUTO_TEST_CASE(C) {
    TEST::SET_W(0, 0007320 | 2);
    TEST::SET_W(2, 3 << 6 | 4);
    TEST::SET_L(0x1000, 0x22222222);
    cpu.A[2] = 0x1000;
    cpu.D[3] = 0x30303030;
    cpu.D[4] = 0x25252525;
    decode_and_run();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.D[4] == 0x22222222);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0007320 | 2);
    TEST::SET_W(2, 3 << 6 | 4);
    TEST::SET_L(0x1000, 0xE0000000);
    cpu.A[2] = 0x1000;
    cpu.D[3] = 0x30000000;
    cpu.D[4] = 1;
    decode_and_run();
    BOOST_TEST(cpu.N);
    BOOST_TEST(cpu.D[4] == 0xE0000000);
}

BOOST_AUTO_TEST_CASE(V) {
    TEST::SET_W(0, 0007320 | 2);
    TEST::SET_W(2, 3 << 6 | 4);
    TEST::SET_L(0x1000, 0x80000000);
    cpu.A[2] = 0x1000;
    cpu.D[3] = 0x30000000;
    cpu.D[4] = 1;
    decode_and_run();
    BOOST_TEST(cpu.V);
    BOOST_TEST(cpu.D[4] == 0x80000000);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
