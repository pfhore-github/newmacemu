#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include "inline.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(AND, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(AND_B(0x70, 0x3F) == 0x30);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(N) {
    AND_B(0x80, 0x80);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    AND_B(0x80, 0x7F);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(ToReg) {
    TEST::SET_W(0, 0140000 | 3 << 9 | 2);
    cpu.D[3] = 0x70;
    cpu.D[2] = 0x3F;
    run_test();
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[3] == 0x30);
}

BOOST_AUTO_TEST_CASE(ToMem) {
    TEST::SET_W(0, 0140400 | 3 << 9 | 022);
    cpu.D[3] = 0x3F;
    cpu.A[2] = 0x1000;
    RAM[0x1000] = 0x70;
    run_test();
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(RAM[0x1000] == 0x30);
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(AND_W(0x7F7F, 0x3080) == 0x3000);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(N) {
    AND_W(0x8000, 0x8000);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    AND_W(0x8000, 0x7FFF);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(ToReg) {
    TEST::SET_W(0, 0140100 | 3 << 9 | 2);
    cpu.D[3] = 0x7F7F;
    cpu.D[2] = 0x3080;
    run_test();
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[3] == 0x3000);
}

BOOST_AUTO_TEST_CASE(ToMem) {
    TEST::SET_W(0, 0140500 | 3 << 9 | 022);
    cpu.D[3] = 0x3FFF;
    TEST::SET_W(0x1000, 0x7000);
    cpu.A[2] = 0x1000;
    run_test();
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(TEST::GET_W(0x1000) == 0x3000);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(AND_L(0x7FFFFFFF, 0x30000000) == 0x30000000);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(N) {
    AND_L(0x80000000, 0x80000000);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    AND_L(0x80000000, 0x7FFFFFFF);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(ToReg) {
    TEST::SET_W(0, 0140200 | 3 << 9 | 2);
    cpu.D[3] = 0x7FFFFFFF;
    cpu.D[2] = 0x30000000;
    run_test();
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[3] == 0x30000000);
}


BOOST_AUTO_TEST_CASE(ToMem) {
    TEST::SET_W(0, 0140600 | 3 << 9 | 022);
    cpu.D[3] = 0x3FFFFFFF;
    TEST::SET_L(0x1000, 0x70000000);
    cpu.A[2] = 0x1000;
    run_test();
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(TEST::GET_L(0x1000) == 0x30000000);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
