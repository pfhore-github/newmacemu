#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(CMPI, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(size) {
    TEST::SET_W(0, 006000 | 2);
    TEST::SET_W(2, 14);
    auto [_, i] = decode();
    BOOST_TEST(i == 2);
}
BOOST_AUTO_TEST_CASE(V1) {
    TEST::SET_W(0, 006000 | 2);
    TEST::SET_W(2, -2);
    cpu.D[2] = 127;
    decode_and_run();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    TEST::SET_W(0, 006000 | 2);
    TEST::SET_W(2, 1);
    cpu.D[2] = 0x80;
    decode_and_run();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(C) {
    TEST::SET_W(0, 006000 | 2);
    TEST::SET_W(2, 6);
    cpu.D[2] = 4;
    decode_and_run();
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 006000 | 2);
    TEST::SET_W(2, 2);
    cpu.D[2] = -5;
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 006000 | 2);
    TEST::SET_W(2, 1);
    cpu.D[2] = 1;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(size) {
    TEST::SET_W(0, 006100 | 2);
    TEST::SET_W(2, 1420);
    auto [_, i] = decode();
    BOOST_TEST(i == 2);
}

BOOST_AUTO_TEST_CASE(V1) {
    TEST::SET_W(0, 006100 | 2);
    TEST::SET_W(2, -2);
    cpu.D[2] = 0x7fff;
    decode_and_run();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    TEST::SET_W(0, 006100 | 2);
    TEST::SET_W(2, 1);
    cpu.D[2] = 0x8000;
    decode_and_run();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(C) {
    TEST::SET_W(0, 006100 | 2);
    TEST::SET_W(2, 600);
    cpu.D[2] = 400;
    decode_and_run();
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 006100 | 2);
    TEST::SET_W(2, 100);
    cpu.D[2] = -500;
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 006100 | 2);
    TEST::SET_W(2, 500);
    cpu.D[2] = 500;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(size) {
    TEST::SET_W(0, 006200 | 2);
    TEST::SET_L(2, 142000);
    auto [_, i] = decode();
    BOOST_TEST(i == 4);
}
BOOST_AUTO_TEST_CASE(V1) {
    TEST::SET_W(0, 006200 | 2);
    TEST::SET_L(2, -2);
    cpu.D[2] = 0x7fffffff;
    decode_and_run();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    TEST::SET_W(0, 006200 | 2);
    TEST::SET_L(2, 1);
    cpu.D[2] = 0x80000000;
    decode_and_run();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(C) {
    TEST::SET_W(0, 006200 | 2);
    TEST::SET_L(2, 300000);
    cpu.D[2] = 100000;
    decode_and_run();
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 006200 | 2);
    TEST::SET_L(2, 10000);
    cpu.D[2] = -20000;
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 006200 | 2);
    TEST::SET_L(2, 40000);
    cpu.D[2] = 40000;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
