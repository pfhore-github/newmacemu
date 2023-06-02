#define BOOST_TEST_DYN_LINK
#include "inline.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_AUTO_TEST_SUITE(ANDI)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(err) {
    TEST::SET_W(0, 0001000|  072 );
    cpu.PC = 0;
    BOOST_CHECK_THROW(decode(), DecodeError);
}
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0001000 | 2);
    TEST::SET_W(2, 0x40);
    cpu.D[2] = 0xFF;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.V = cpu.C = cpu.X = true;
    f();
    BOOST_TEST(cpu.D[2] == 0x40);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 2);
}
BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0001000 | 2);
    TEST::SET_W(2, 0xff);
    cpu.D[2] = 0x80;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.N = false;
    f();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0001000 | 2);
    TEST::SET_W(2, 0x7f);
    cpu.D[2] = 0x80;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.Z = false;
    f();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(err) {
    TEST::SET_W(0, 0001100|  072 );
    cpu.PC = 0;
    BOOST_CHECK_THROW(decode(), DecodeError);
}

BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0001100 | 2);
    TEST::SET_W(2, 0x7fff);
    cpu.D[2] = 0xffff;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.V = cpu.C = cpu.X = true;
    f();
    BOOST_TEST(cpu.D[2] == 0x7fff);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 2);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0001100 | 2);
    TEST::SET_W(2, 0xffff);
    cpu.D[2] = 0x8000;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.N = false;
    f();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0001100 | 2);
    TEST::SET_W(2, 0x7fff);
    cpu.D[2] = 0x8000;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.Z = false;
    f();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(err) {
    TEST::SET_W(0, 0001200|  072 );
    cpu.PC = 0;
    BOOST_CHECK_THROW(decode(), DecodeError);
}

BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0001200 | 2);
    TEST::SET_L(2, 0x7fffffff);
    cpu.D[2] = 0xffff0000;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.V = cpu.C = cpu.X = true;
    f();
    BOOST_TEST(cpu.D[2] == 0x7fff0000);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 4);
}
BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0001200 | 2);
    TEST::SET_L(2, 0x80000000);
    cpu.D[2] = 0xffffffff;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.N = false;
    f();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0001200 | 2);
    TEST::SET_L(2, 0x7fffffff);
    cpu.D[2] = 0x80000000;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.Z = false;
    f();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(ccr) {
    cpu.X = cpu.N = cpu.Z = cpu.V = cpu.C = true;
    TEST::SET_W(0, 0001074);
    TEST::SET_W(2, 0);
    cpu.D[2] = 0x80000000;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.Z = false;
    f();
    BOOST_TEST(i == 2);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_SUITE_END()
