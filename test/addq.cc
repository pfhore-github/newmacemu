#define BOOST_TEST_DYN_LINK
#include "inline.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_AUTO_TEST_SUITE(ADDQ)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(err) {
    TEST::SET_W(0, 0050000|  072 );
    cpu.PC = 0;
    BOOST_CHECK_THROW(decode(), DecodeError);
}

BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0050000 | 3 << 9 | 2);
    cpu.D[2] = 21;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.V = cpu.C = cpu.X = true;
    f();
    BOOST_TEST(cpu.D[2] == 24);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(i == 0);
}


BOOST_AUTO_TEST_CASE(eight) {
    TEST::SET_W(0, 0050000 | 0 << 9 | 2);
    cpu.D[2] = 21;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.V = cpu.C = cpu.X = true;
    f();
    BOOST_TEST(cpu.D[2] == 29);
}

BOOST_AUTO_TEST_CASE(V) {
    TEST::SET_W(0, 0050000 | 1 << 9 | 2);
    cpu.D[2] = 0x7f;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.V = false;
    f();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(CX) {
    TEST::SET_W(0, 0050000 | 1 << 9 | 2);
    cpu.D[2] = 0xff;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.C = false;
    cpu.X = false;
    f();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0050000 | 1 << 9 | 2);
    cpu.D[2] = 0xa0;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.N = false;
    f();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0050000 | 1 << 9 | 2);
    cpu.D[2] = 0xff;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.Z = false;
    f();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(err) {
    TEST::SET_W(0, 0050100|  072 );
    cpu.PC = 0;
    BOOST_CHECK_THROW(decode(), DecodeError);
}

BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0050100 | 3 << 9 | 2);
    cpu.D[2] = 2000;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.V = cpu.C = cpu.X = true;
    f();
    BOOST_TEST(cpu.D[2] == 2003);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(eight) {
    TEST::SET_W(0, 0050100 | 0 << 9 | 2);
    cpu.D[2] = 2100;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.V = cpu.C = cpu.X = true;
    f();
    BOOST_TEST(cpu.D[2] == 2108);
}
BOOST_AUTO_TEST_CASE(V) {
    TEST::SET_W(0, 0050100 | 1 << 9 | 2);
    cpu.D[2] = 0x7fff;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.V = false;
    f();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(CX) {
    TEST::SET_W(0, 0050100 | 1 << 9 | 2);
    cpu.D[2] = 0xffff;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.C = false;
    cpu.X = false;
    f();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0050100 | 1 << 9 | 2);
    cpu.D[2] = 0x8111;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.N = false;
    f();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0050100 | 1 << 9 | 2);
    cpu.D[2] = 0xffff;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.Z = false;
    f();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(err) {
    TEST::SET_W(0, 0050200|  072 );
    cpu.PC = 0;
    BOOST_CHECK_THROW(decode(), DecodeError);
}

BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0050200 | 1 << 9 | 2);
    cpu.D[2] = 142044;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.V = cpu.C = cpu.X = true;
    f();
    BOOST_TEST(cpu.D[2] == 142045);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(i == 0);
}


BOOST_AUTO_TEST_CASE(eight) {
    TEST::SET_W(0, 0050200 | 0 << 9 | 2);
    cpu.D[2] = 142044;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.V = cpu.C = cpu.X = true;
    f();
    BOOST_TEST(cpu.D[2] == 142052);
}
BOOST_AUTO_TEST_CASE(V) {
    TEST::SET_W(0, 0050200 | 1 << 9 | 2);
    cpu.D[2] = 0x7fffffff;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.V = false;
    f();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(CX) {
    TEST::SET_W(0, 0050200 | 2 << 9 | 2);
    cpu.D[2] = 0xffffffff;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.C = false;
    cpu.X = false;
    f();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0050200 | 2 << 9 | 2);
    cpu.D[2] = 0xff000000;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.N = false;
    f();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0050200 | 1 << 9 | 2);
    cpu.D[2] = 0xffffffff;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.Z = false;
    f();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
