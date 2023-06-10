#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(ORI, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(err) {
    TEST::SET_W(0, 0000000 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0000000 | 2);
    TEST::SET_W(2, 0x1F);
    cpu.D[2] = 0x20;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x3F);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 2);
}
BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0000000 | 2);
    TEST::SET_W(2, 0);
    cpu.D[2] = 0x80;
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0000000 | 2);
    TEST::SET_W(2, 0);
    cpu.D[2] = 0;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(err) {
    TEST::SET_W(0, 0000100 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}

BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0000100 | 2);
    TEST::SET_W(2, 0x2020);
    cpu.D[2] = 0x1F1F;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x3F3F);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 2);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0000100 | 2);
    TEST::SET_W(2, 0);
    cpu.D[2] = 0x8000;
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0000100 | 2);
    TEST::SET_W(2, 0);
    cpu.D[2] = 0;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(err) {
    TEST::SET_W(0, 0000200 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}

BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0000200 | 2);
    TEST::SET_L(2, 0x20202020);
    cpu.D[2] = 0x1F1F1F1F;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x3F3F3F3F);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 4);
}
BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0000200 | 2);
    TEST::SET_L(2, 0x80000000);
    cpu.D[2] = 0;
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0000200 | 2);
    TEST::SET_L(2, 0);
    cpu.D[2] = 0;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(ccr) {
    cpu.X = cpu.N = cpu.Z = cpu.V = cpu.C = false;
    TEST::SET_W(0, 0000074);
    TEST::SET_W(2, 0x1F);
    auto i = decode_and_run();
    BOOST_TEST(i == 2);
    BOOST_TEST(cpu.X);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(cpu.N);
    BOOST_TEST(cpu.V);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_SUITE_END()
