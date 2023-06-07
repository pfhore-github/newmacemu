#define BOOST_TEST_DYN_LINK
#include "inline.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(CMPM, Prepare)

BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(size) {
    TEST::SET_W(0, 0130410 | 3 << 9 | 1);
    auto [_, i] = decode();
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0130410 | 3 << 9 | 1);
    RAM[0x1000] = -12;
    RAM[0x1100] = 2;
    cpu.A[3] = 0x1000;
    cpu.A[1] = 0x1100;
    decode_and_run();
    BOOST_TEST(cpu.A[3] == 0x1001);
    BOOST_TEST(cpu.A[1] == 0x1101);
    BOOST_TEST(cpu.N);
}


BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0130410 | 3 << 9 | 1);
    RAM[0x1000] = 12;
    RAM[0x1100] = 12;
    cpu.A[3] = 0x1000;
    cpu.A[1] = 0x1100;
    decode_and_run();
    BOOST_TEST(cpu.A[3] == 0x1001);
    BOOST_TEST(cpu.A[1] == 0x1101);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(V1) {
    TEST::SET_W(0, 0130410 | 3 << 9 | 1);
    RAM[0x1000] = 0x7f;
    RAM[0x1100] = -1;
    cpu.A[3] = 0x1000;
    cpu.A[1] = 0x1100;
    decode_and_run();
    BOOST_TEST(cpu.A[3] == 0x1001);
    BOOST_TEST(cpu.A[1] == 0x1101);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    TEST::SET_W(0, 0130410 | 3 << 9 | 1);
    RAM[0x1000] = 0x80;
    RAM[0x1100] = 1;
    cpu.A[3] = 0x1000;
    cpu.A[1] = 0x1100;
    decode_and_run();
    BOOST_TEST(cpu.A[3] == 0x1001);
    BOOST_TEST(cpu.A[1] == 0x1101);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(C) {
    TEST::SET_W(0, 0130410 | 3 << 9 | 1);
    RAM[0x1000] = 4;
    RAM[0x1100] = 6;
    cpu.A[3] = 0x1000;
    cpu.A[1] = 0x1100;
    decode_and_run();
    BOOST_TEST(cpu.A[3] == 0x1001);
    BOOST_TEST(cpu.A[1] == 0x1101);
    BOOST_TEST(cpu.C);
}
BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(size) {
    TEST::SET_W(0, 0130510 | 3 << 9 | 1);
    auto [_, i] = decode();
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0130510 | 3 << 9 | 1);
    TEST::SET_W(0x1000, -1200);
    TEST::SET_W(0x1100, 200);
    cpu.A[3] = 0x1000;
    cpu.A[1] = 0x1100;
    decode_and_run();
    BOOST_TEST(cpu.A[3] == 0x1002);
    BOOST_TEST(cpu.A[1] == 0x1102);
    BOOST_TEST(cpu.N);
}


BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0130510 | 3 << 9 | 1);
    TEST::SET_W(0x1000, 1200);
    TEST::SET_W(0x1100, 1200);
    cpu.A[3] = 0x1000;
    cpu.A[1] = 0x1100;
    decode_and_run();
    BOOST_TEST(cpu.A[3] == 0x1002);
    BOOST_TEST(cpu.A[1] == 0x1102);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(V1) {
    TEST::SET_W(0, 0130510 | 3 << 9 | 1);
    TEST::SET_W(0x1000, 0x7fff);
    TEST::SET_W(0x1100, -1);
    cpu.A[3] = 0x1000;
    cpu.A[1] = 0x1100;
    decode_and_run();
    BOOST_TEST(cpu.A[3] == 0x1002);
    BOOST_TEST(cpu.A[1] == 0x1102);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    TEST::SET_W(0, 0130510 | 3 << 9 | 1);
    TEST::SET_W(0x1000, 0x8000);
    TEST::SET_W(0x1100, 1);
    cpu.A[3] = 0x1000;
    cpu.A[1] = 0x1100;
    decode_and_run();
    BOOST_TEST(cpu.A[3] == 0x1002);
    BOOST_TEST(cpu.A[1] == 0x1102);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(C) {
    TEST::SET_W(0, 0130510 | 3 << 9 | 1);
    TEST::SET_W(0x1000, 4000);
    TEST::SET_W(0x1100, 6000);
    cpu.A[3] = 0x1000;
    cpu.A[1] = 0x1100;
    decode_and_run();
    BOOST_TEST(cpu.A[3] == 0x1002);
    BOOST_TEST(cpu.A[1] == 0x1102);
    BOOST_TEST(cpu.C);
}
BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(size) {
    TEST::SET_W(0, 0130610 | 3 << 9 | 1);
    auto [_, i] = decode();
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0130610 | 3 << 9 | 1);
    TEST::SET_L(0x1000, -120000);
    TEST::SET_L(0x1100, 20000);
    cpu.A[3] = 0x1000;
    cpu.A[1] = 0x1100;
    decode_and_run();
    BOOST_TEST(cpu.A[3] == 0x1004);
    BOOST_TEST(cpu.A[1] == 0x1104);
    BOOST_TEST(cpu.N);
}


BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0130610 | 3 << 9 | 1);
    TEST::SET_L(0x1000, 120000);
    TEST::SET_L(0x1100, 120000);
    cpu.A[3] = 0x1000;
    cpu.A[1] = 0x1100;
    decode_and_run();
    BOOST_TEST(cpu.A[3] == 0x1004);
    BOOST_TEST(cpu.A[1] == 0x1104);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(V1) {
    TEST::SET_W(0, 0130610 | 3 << 9 | 1);
    TEST::SET_L(0x1000, 0x7fffffff);
    TEST::SET_L(0x1100, -1);
    cpu.A[3] = 0x1000;
    cpu.A[1] = 0x1100;
    decode_and_run();
    BOOST_TEST(cpu.A[3] == 0x1004);
    BOOST_TEST(cpu.A[1] == 0x1104);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    TEST::SET_W(0, 0130610 | 3 << 9 | 1);
    TEST::SET_L(0x1000, 0x80000000);
    TEST::SET_L(0x1100, 1);
    cpu.A[3] = 0x1000;
    cpu.A[1] = 0x1100;
    decode_and_run();
    BOOST_TEST(cpu.A[3] == 0x1004);
    BOOST_TEST(cpu.A[1] == 0x1104);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(C) {
    TEST::SET_W(0, 0130610 | 3 << 9 | 1);
    TEST::SET_L(0x1000, 4000000);
    TEST::SET_L(0x1100, 6000000);
    cpu.A[3] = 0x1000;
    cpu.A[1] = 0x1100;
    decode_and_run();
    BOOST_TEST(cpu.A[3] == 0x1004);
    BOOST_TEST(cpu.A[1] == 0x1104);
    BOOST_TEST(cpu.C);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
