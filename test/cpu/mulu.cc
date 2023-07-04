#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include "proto.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(MULU, Prepare)
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(MULU_W(70, 30) == 2100);
}

BOOST_AUTO_TEST_CASE(N) {
    MULU_W(0xffff, 0xffff);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    MULU_W(1, 0);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(operand) {
    TEST::SET_W(0, 0140300 | 3 << 9 | 2);
    cpu.D[3] = 70;
    cpu.D[2] = 30;
    int i = decode_and_run();
    BOOST_TEST(cpu.D[3] == 2100);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(MULU_L(2000, 7000) == 14000000);
}

BOOST_AUTO_TEST_CASE(N) {
    MULU_L(0x80000000, 1);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    MULU_L(1, 0);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(V) {
    MULU_L(0x4000'0000, 16);
    BOOST_TEST(cpu.V);
}


BOOST_AUTO_TEST_CASE(operand) {
    TEST::SET_W(0, 0046000 | 2);
    TEST::SET_W(2, 0000000 | 3 << 12);
    cpu.D[3] = 2000;
    cpu.D[2] = 7000;
    int i = decode_and_run();
    BOOST_TEST(cpu.D[3] == 14000000);
    BOOST_TEST(i == 2);
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Quad)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(MULU_LL(0x40000000, 4) == 0x1'0000'0000LL);
}

BOOST_AUTO_TEST_CASE(N) {
    MULU_LL(0xffffffff, 0xffffffff);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    MULU_LL(1, 0);
    BOOST_TEST(cpu.Z);
}


BOOST_AUTO_TEST_CASE(operand) {
    TEST::SET_W(0, 0046000 | 2);
    TEST::SET_W(2, 0002000 | 3 << 12 | 4);
    cpu.D[3] = 0x80000000;
    cpu.D[2] = 2;
    int i = decode_and_run();
    BOOST_TEST(cpu.D[3] == 0);
    BOOST_TEST(cpu.D[4] == 1);
    BOOST_TEST(i == 2);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
