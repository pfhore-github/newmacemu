#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(CAS2, Prepare)

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0006374);
    TEST::SET_W(2, 1 << 15 | 2 << 12 | 3 << 6 | 4);
    TEST::SET_W(4, 1 << 15 | 5 << 12 | 6 << 6 | 7);
    TEST::SET_W(0x1000, 0x2222);
    TEST::SET_W(0x1100, 0x3333);
    cpu.A[2] = 0x1000;
    cpu.A[5] = 0x1100;
    cpu.D[3] = 0x3030;
    cpu.D[4] = 0x2222;
    cpu.D[6] = 0x4040;
    cpu.D[7] = 0x3333;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(TEST::GET_W(0x1000) == 0x3030);
    BOOST_TEST(TEST::GET_W(0x1100) == 0x4040);
}
BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0006374);
    TEST::SET_W(2, 1 << 15 | 2 << 12 | 3 << 6 | 4);
    TEST::SET_W(4, 1 << 15 | 5 << 12 | 6 << 6 | 7);
    TEST::SET_W(0x1000, 0x2222);
    TEST::SET_W(0x1100, 0x3333);
    cpu.A[2] = 0x1000;
    cpu.A[5] = 0x1100;
    cpu.D[3] = 0x3030;
    cpu.D[4] = 0x2227;
    cpu.D[6] = 0x4040;
    cpu.D[7] = 0x3333;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.D[4] == 0x2222);
    BOOST_TEST(cpu.D[7] == 0x3333);
}

BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0006374);
    TEST::SET_W(2, 1 << 15 | 2 << 12 | 3 << 6 | 4);
    TEST::SET_W(4, 1 << 15 | 5 << 12 | 6 << 6 | 7);
    TEST::SET_W(0x1000, 0x2222);
    TEST::SET_W(0x1100, 0x3333);
    cpu.A[2] = 0x1000;
    cpu.A[5] = 0x1100;
    cpu.D[3] = 0x3030;
    cpu.D[4] = 0x2222;
    cpu.D[6] = 0x4040;
    cpu.D[7] = 0x3339;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.D[4] == 0x2222);
    BOOST_TEST(cpu.D[7] == 0x3333);
}

BOOST_AUTO_TEST_CASE(traced) {
    TEST::SET_W(0, 0006374);
    TEST::SET_W(2, 1 << 15 | 2 << 12 | 3 << 6 | 4);
    TEST::SET_W(4, 1 << 15 | 5 << 12 | 6 << 6 | 7);
    TEST::SET_W(0x1000, 0x2222);
    TEST::SET_W(0x1100, 0x3333);
    cpu.T = 1;
    BOOST_TEST(run_test() == 9);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0007374);
    TEST::SET_W(2, 1 << 15 | 2 << 12 | 3 << 6 | 4);
    TEST::SET_W(4, 1 << 15 | 5 << 12 | 6 << 6 | 7);
    TEST::SET_L(0x1000, 0x22222222);
    TEST::SET_L(0x1100, 0x33333333);
    cpu.A[2] = 0x1000;
    cpu.A[5] = 0x1100;
    cpu.D[3] = 0x30303030;
    cpu.D[4] = 0x22222222;
    cpu.D[6] = 0x40404040;
    cpu.D[7] = 0x33333333;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(TEST::GET_L(0x1000) == 0x30303030);
    BOOST_TEST(TEST::GET_L(0x1100) == 0x40404040);
}

BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0007374);
    TEST::SET_W(2, 1 << 15 | 2 << 12 | 3 << 6 | 4);
    TEST::SET_W(4, 1 << 15 | 5 << 12 | 6 << 6 | 7);
    TEST::SET_L(0x1000, 0x22222222);
    TEST::SET_L(0x1100, 0x33333333);
    cpu.A[2] = 0x1000;
    cpu.A[5] = 0x1100;
    cpu.D[3] = 0x30303030;
    cpu.D[4] = 0x22222227;
    cpu.D[6] = 0x40404040;
    cpu.D[7] = 0x33333333;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.D[4] == 0x22222222);
    BOOST_TEST(cpu.D[7] == 0x33333333);
}

BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0007374);
    TEST::SET_W(2, 1 << 15 | 2 << 12 | 3 << 6 | 4);
    TEST::SET_W(4, 1 << 15 | 5 << 12 | 6 << 6 | 7);
    TEST::SET_L(0x1000, 0x22222222);
    TEST::SET_L(0x1100, 0x33333333);
    cpu.A[2] = 0x1000;
    cpu.A[5] = 0x1100;
    cpu.D[3] = 0x30303030;
    cpu.D[4] = 0x22222222;
    cpu.D[6] = 0x40404040;
    cpu.D[7] = 0x33333339;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.D[4] == 0x22222222);
    BOOST_TEST(cpu.D[7] == 0x33333333);
}

BOOST_AUTO_TEST_CASE(traced) {
    TEST::SET_W(0, 0007374);
    TEST::SET_W(2, 1 << 15 | 2 << 12 | 3 << 6 | 4);
    TEST::SET_W(4, 1 << 15 | 5 << 12 | 6 << 6 | 7);
    TEST::SET_L(0x1000, 0x22222222);
    TEST::SET_L(0x1100, 0x33333333);
    cpu.T = 1;
    BOOST_TEST(run_test() == 9);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
