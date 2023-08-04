#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include "proto.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(ASR, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(ASR_B(0x40, 2) == 0x10);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(sc0) {
    BOOST_TEST(static_cast<uint8_t>(ASR_B(1, 0)) == 1);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(CX) {
    ASR_B(1, 1);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(negative) {
    BOOST_TEST(static_cast<uint8_t>(ASR_B(0x80, 2)) == 0xE0);
    BOOST_TEST(cpu.N);
}


BOOST_AUTO_TEST_CASE(byReg) {
    cpu.D[2] = 0x40;
    cpu.D[3] = 2;
    TEST::SET_W(0, 0160040 | 3 << 9 | 2);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[2] == 0x10);
}

BOOST_AUTO_TEST_CASE(byRegMod) {
    cpu.D[2] = 0x40;
    cpu.D[3] = 66;
    TEST::SET_W(0, 0160040 | 3 << 9 | 2);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.D[2] == 0x10);
}

BOOST_AUTO_TEST_CASE(byImm) {
    cpu.D[2] = 0x40;
    TEST::SET_W(0, 0160000 | 2 << 9 | 2);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[2] == 0x10);
}

BOOST_AUTO_TEST_CASE(byImm0) {
    cpu.D[2] = 1;
    TEST::SET_W(0, 0160000 | 0 << 9 | 2);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.D[2] == 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(ASR_W(0x4000, 2) == 0x1000);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(sc0) {
    BOOST_TEST(static_cast<uint8_t>(ASR_W(1, 0)) == 1);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(CX) {
    ASR_W(1, 1);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(negative) {
    BOOST_TEST(static_cast<uint16_t>(ASR_W(0x8000, 2)) == 0xE000);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(mem) {
    TEST::SET_W(0x1000, 300);
    TEST::SET_W(0, 0160320 | 2);
    cpu.A[2] = 0x1000;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(TEST::GET_W(0x1000) == 150);
}
BOOST_AUTO_TEST_CASE(byReg) {
    cpu.D[2] = 0x400;
    cpu.D[3] = 2;
    TEST::SET_W(0, 0160140 | 3 << 9 | 2);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[2] == 0x100);
}

BOOST_AUTO_TEST_CASE(byRegMod) {
    cpu.D[2] = 0x400;
    cpu.D[3] = 66;
    TEST::SET_W(0, 0160140 | 3 << 9 | 2);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.D[2] == 0x100);
}

BOOST_AUTO_TEST_CASE(byImm) {
    cpu.D[2] = 0x400;
    TEST::SET_W(0, 0160100 | 2 << 9 | 2);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[2] == 0x100);
}

BOOST_AUTO_TEST_CASE(byImm0) {
    cpu.D[2] = 0x400;
    TEST::SET_W(0, 0160100 | 0 << 9 | 2);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.D[2] == 0x4);
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(ASR_L(0x40000000, 2) == 0x10000000);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(sc0) {
    BOOST_TEST(static_cast<uint8_t>(ASR_L(1, 0)) == 1);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(CX) {
    ASR_L(1, 1);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(negative) {
    BOOST_TEST(static_cast<uint32_t>(ASR_L(0x80000000, 2)) == 0xE0000000);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(byReg) {
    cpu.D[2] = 0x10000;
    cpu.D[3] = 2;
    TEST::SET_W(0, 0160240 | 3 << 9 | 2);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[2] == 0x4000);
}

BOOST_AUTO_TEST_CASE(byRegMod) {
    cpu.D[2] = 0x10000;
    cpu.D[3] = 66;
    TEST::SET_W(0, 0160240 | 3 << 9 | 2);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.D[2] == 0x4000);
}

BOOST_AUTO_TEST_CASE(byImm) {
    cpu.D[2] = 0x10000;
    TEST::SET_W(0, 0160200 | 2 << 9 | 2);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[2] == 0x4000);
}

BOOST_AUTO_TEST_CASE(byImm0) {
    cpu.D[2] = 0x10000;
    TEST::SET_W(0, 0160200 | 0 << 9 | 2);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.D[2] == 0x100);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()