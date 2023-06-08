#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(EOR, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(err) {
    TEST::SET_W(0, 0130400 | 3 << 9 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0130400 | 3 << 9 | 2);
    cpu.D[3] = 0xED;
    cpu.D[2] = 0xFF;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x12);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0130400 | 3 << 9 | 2);
    cpu.D[3] = 0xff;
    cpu.D[2] = 0x12;
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0130400 | 3 << 9 | 2);
    cpu.D[3] = 0x80;
    cpu.D[2] = 0x80;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(err) {
    TEST::SET_W(0, 0130500 | 3 << 9 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0130500 | 3 << 9 | 2);
    cpu.D[3] = 0xffff;
    cpu.D[2] = 0xeded;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x1212);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0130500 | 3 << 9 | 2);
    cpu.D[3] = 0xffff;
    cpu.D[2] = 0x1234;
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0130500 | 3 << 9 | 2);
    cpu.D[3] = 0x7FFF;
    cpu.D[2] = 0x7FFF;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(err) {
    TEST::SET_W(0, 0130600 | 3 << 9 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0130600 | 3 << 9 | 2);
    cpu.D[3] = 0xffffffff;
    cpu.D[2] = 0xedededed;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x12121212);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0130600 | 3 << 9 | 2);
    cpu.D[3] = 0x12121212;
    cpu.D[2] = 0xffffffff;
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0130600 | 3 << 9 | 2);
    cpu.D[3] = 0x20000000;
    cpu.D[2] = 0x20000000;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
