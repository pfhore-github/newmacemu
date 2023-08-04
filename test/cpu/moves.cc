#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(MOVES, Prepare)
BOOST_AUTO_TEST_SUITE(Load)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(NG) {
    cpu.S = false;
    TEST::SET_W(0, 0007020);
    TEST::SET_W(2, 0);
    BOOST_TEST(run_test() == 8);
}
BOOST_AUTO_TEST_CASE(OK) {
    cpu.S = true;
    RAM[0x1000] = 0x22;
    cpu.A[2] = 0x1000;
    TEST::SET_W(0, 0007022);
    TEST::SET_W(2, 3 << 12);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(cpu.D[3] == 0x22);
}

BOOST_AUTO_TEST_CASE(traced) {
    cpu.S = true;
    cpu.T = 1;
    RAM[0x1000] = 0x22;
    cpu.A[2] = 0x1000;
    TEST::SET_W(0, 0007022);
    TEST::SET_W(2, 3 << 12);
    BOOST_TEST(run_test() == 9);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(NG) {
    cpu.S = false;
    TEST::SET_W(0, 0007120);
    TEST::SET_W(2, 0);
    BOOST_TEST(run_test() == 8);
}

BOOST_AUTO_TEST_CASE(OK) {
    cpu.S = true;
    TEST::SET_W(0x1000, 0x1234);
    cpu.A[2] = 0x1000;
    TEST::SET_W(0, 0007122);
    TEST::SET_W(2, 3 << 12);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(cpu.D[3] == 0x1234);
}

BOOST_AUTO_TEST_CASE(traced) {
    cpu.S = true;
    cpu.T = 1;
    TEST::SET_W(0x1000, 0x1234);
    cpu.A[2] = 0x1000;
    TEST::SET_W(0, 0007122);
    TEST::SET_W(2, 3 << 12);
    BOOST_TEST(run_test() == 9);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(NG) {
    cpu.S = false;
    TEST::SET_W(0, 0007220);
    TEST::SET_W(2, 0);
    BOOST_TEST(run_test() == 8);
}
BOOST_AUTO_TEST_CASE(OK) {
    cpu.S = true;
    TEST::SET_L(0x1000, 0x12345678);
    cpu.A[2] = 0x1000;
    TEST::SET_W(0, 0007222);
    TEST::SET_W(2, 3 << 12);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(cpu.D[3] == 0x12345678);
}

BOOST_AUTO_TEST_CASE(traced) {
    cpu.S = true;
    cpu.T = 1;
    TEST::SET_L(0x1000, 0x12345678);
    cpu.A[2] = 0x1000;
    TEST::SET_W(0, 0007222);
    TEST::SET_W(2, 3 << 12);
    BOOST_TEST(run_test() == 9);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Staore)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(NG) {
    cpu.S = false;
    TEST::SET_W(0, 0007020);
    TEST::SET_W(2, 1 << 11);
    BOOST_TEST(run_test() == 8);
}
BOOST_AUTO_TEST_CASE(OK) {
    cpu.S = true;
    cpu.D[3] = 0x22;
    cpu.A[2] = 0x1000;
    TEST::SET_W(0, 0007022);
    TEST::SET_W(2, 3 << 12 | 1 << 11);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(RAM[0x1000] == 0x22);
}

BOOST_AUTO_TEST_CASE(traced) {
    cpu.S = true;
    cpu.T = 1;
    cpu.D[3] = 0x22;
    cpu.A[2] = 0x1000;
    TEST::SET_W(0, 0007022);
    TEST::SET_W(2, 3 << 12 | 1 << 11);
    BOOST_TEST(run_test() == 9);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(NG) {
    cpu.S = false;
    TEST::SET_W(0, 0007120);
    TEST::SET_W(2, 1 << 11);
    BOOST_TEST(run_test() == 8);
}
BOOST_AUTO_TEST_CASE(OK) {
    cpu.S = true;
    cpu.D[3] = 0x1234;
    cpu.A[2] = 0x1000;
    TEST::SET_W(0, 0007122);
    TEST::SET_W(2, 3 << 12 | 1 << 11);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(TEST::GET_W(0x1000) == 0x1234);
}

BOOST_AUTO_TEST_CASE(traced) {
    cpu.S = true;
    cpu.T = 1;
    cpu.D[3] = 0x1234;
    cpu.A[2] = 0x1000;
    TEST::SET_W(0, 0007122);
    TEST::SET_W(2, 3 << 12 | 1 << 11);
    BOOST_TEST(run_test() == 9);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(NG) {
    cpu.S = false;
    TEST::SET_W(0, 0007220);
    TEST::SET_W(2, 1 << 11);
    BOOST_TEST(run_test() == 8);
}

BOOST_AUTO_TEST_CASE(OK) {
    cpu.S = true;
    cpu.D[3] = 0x12345678;
    cpu.A[2] = 0x1000;
    TEST::SET_W(0, 0007222);
    TEST::SET_W(2, 3 << 12 | 1 << 11);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(TEST::GET_L(0x1000) == 0x12345678);
}

BOOST_AUTO_TEST_CASE(traced) {
    cpu.S = true;
    cpu.T = 1;
    cpu.D[3] = 0x12345678;
    cpu.A[2] = 0x1000;
    TEST::SET_W(0, 0007222);
    TEST::SET_W(2, 3 << 12 | 1 << 11);
    BOOST_TEST(run_test() == 9);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
