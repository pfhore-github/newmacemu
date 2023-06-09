#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "proto.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(ASL, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(ASL_B(3, 2) == 12);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(sc0) {
    BOOST_TEST(static_cast<uint8_t>(ASL_B(0x80, 0)) == 0x80);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(CX) {
    ASL_B(0xC0, 1);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

// 0 -> 1
BOOST_AUTO_TEST_CASE(V1) {
    ASL_B(0x40, 1);
    BOOST_TEST(cpu.V);
}

// 1 -> 0
BOOST_AUTO_TEST_CASE(V2) {
    ASL_B(0xA0, 1);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(byReg) {
    cpu.D[2] = 3;
    cpu.D[3] = 2;
    TEST::SET_W(0, 0160440 | 3 << 9 | 2);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 12);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(byRegMod) {
    cpu.D[2] = 3;
    cpu.D[3] = 66;
    TEST::SET_W(0, 0160440 | 3 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 12);
}

BOOST_AUTO_TEST_CASE(byImm) {
    cpu.D[2] = 3;
    TEST::SET_W(0, 0160400 | 2 << 9 | 2);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 12);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(byImm0) {
    cpu.D[2] = 3;
    TEST::SET_W(0, 0160400 | 0 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(ASL_W(300, 2) == 1200);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(sc0) {
    BOOST_TEST(static_cast<uint16_t>(ASL_W(0x8000, 0)) == 0x8000);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(CX) {
    ASL_W(0xC000, 1);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

// 0 -> 1
BOOST_AUTO_TEST_CASE(V1) {
    ASL_W(0x4000, 1);
    BOOST_TEST(cpu.V);
}

// 1 -> 0
BOOST_AUTO_TEST_CASE(V2) {
    ASL_W(0xA000, 1);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(mem) {
    TEST::SET_W(0x1000, 300);
    TEST::SET_W(0, 0160720 | 2);
    cpu.A[2] = 0x1000;
    auto i = decode_and_run();
    BOOST_TEST(TEST::GET_W(0x1000) == 600);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(byReg) {
    cpu.D[2] = 300;
    cpu.D[3] = 2;
    TEST::SET_W(0, 0160540 | 3 << 9 | 2);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 1200);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(byRegMod) {
    cpu.D[2] = 300;
    cpu.D[3] = 66;
    TEST::SET_W(0, 0160540 | 3 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 1200);
}

BOOST_AUTO_TEST_CASE(byImm) {
    cpu.D[2] = 300;
    TEST::SET_W(0, 0160500 | 2 << 9 | 2);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 1200);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(byImm0) {
    cpu.D[2] = 3;
    TEST::SET_W(0, 0160500 | 0 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x300);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(ASL_L(300000, 2) == 1200000);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(sc0) {
    BOOST_TEST(static_cast<uint32_t>(ASL_L(0x80000000, 0)) == 0x80000000);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(CX) {
    ASL_L(0xC0000000, 1);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

// 0 -> 1
BOOST_AUTO_TEST_CASE(V1) {
    ASL_L(0x40000000, 1);
    BOOST_TEST(cpu.V);
}

// 1 -> 0
BOOST_AUTO_TEST_CASE(V2) {
    ASL_L(0xA0000000, 1);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(byReg) {
    cpu.D[2] = 30000;
    cpu.D[3] = 2;
    TEST::SET_W(0, 0160640 | 3 << 9 | 2);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 120000);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(byRegMod) {
    cpu.D[2] = 30000;
    cpu.D[3] = 66;
    TEST::SET_W(0, 0160640 | 3 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 120000);
}

BOOST_AUTO_TEST_CASE(byImm) {
    cpu.D[2] = 30000;
    TEST::SET_W(0, 0160600 | 2 << 9 | 2);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 120000);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(byImm0) {
    cpu.D[2] = 0x1000;
    TEST::SET_W(0, 0160600 | 0 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x100000);
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()