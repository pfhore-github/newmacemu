#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(MOVES, Prepare)
BOOST_AUTO_TEST_SUITE(From)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(err) {
    cpu.S = false;
    TEST::SET_W(0, 0007020);
    TEST::SET_W(2, 0);
    decode_and_run();
    BOOST_TEST(GET_EXCEPTION() == 8);
}
BOOST_DATA_TEST_CASE(value, bdata::xrange(2), T) {
    cpu.S = true;
    cpu.T = T;
    RAM[0x1000] = 0x22;
    cpu.A[2] = 0x1000;
    TEST::SET_W(0, 0007022);
    TEST::SET_W(2, 3 << 12);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[3] == 0x22);
    BOOST_TEST(i == 2);
    BOOST_TEST(cpu.must_trace == !!T);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(err) {
    cpu.S = false;
    TEST::SET_W(0, 0007120);
    TEST::SET_W(2, 0);
    decode_and_run();
    BOOST_TEST(GET_EXCEPTION() == 8);
}
BOOST_DATA_TEST_CASE(value, bdata::xrange(2), T) {
    cpu.S = true;
    cpu.T = T;
    TEST::SET_W(0x1000, 0x1234);
    cpu.A[2] = 0x1000;
    TEST::SET_W(0, 0007122);
    TEST::SET_W(2, 3 << 12);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[3] == 0x1234);
    BOOST_TEST(i == 2);
    BOOST_TEST(cpu.must_trace == !!T);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(err) {
    cpu.S = false;
    TEST::SET_W(0, 0007220);
    TEST::SET_W(2, 0);
    decode_and_run();
    BOOST_TEST(GET_EXCEPTION() == 8);
}
BOOST_DATA_TEST_CASE(value, bdata::xrange(2), T) {
    cpu.S = true;
    cpu.T = T;
    TEST::SET_L(0x1000, 0x12345678);
    cpu.A[2] = 0x1000;
    TEST::SET_W(0, 0007222);
    TEST::SET_W(2, 3 << 12);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[3] == 0x12345678);
    BOOST_TEST(i == 2);
    BOOST_TEST(cpu.must_trace == !!T);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(To)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(err) {
    cpu.S = false;
    TEST::SET_W(0, 0007020);
    TEST::SET_W(2, 1 << 11);
    decode_and_run();
    BOOST_TEST(GET_EXCEPTION() == 8);
}
BOOST_DATA_TEST_CASE(value, bdata::xrange(2), T) {
    cpu.S = true;
    cpu.T = T;
    cpu.D[3] = 0x22;
    cpu.A[2] = 0x1000;
    TEST::SET_W(0, 0007022);
    TEST::SET_W(2, 3 << 12 | 1 << 11);
    auto i = decode_and_run();
    BOOST_TEST(RAM[0x1000] == 0x22);
    BOOST_TEST(i == 2);
    BOOST_TEST(cpu.must_trace == !!T);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(err) {
    cpu.S = false;
    TEST::SET_W(0, 0007120);
    TEST::SET_W(2, 1 << 11);
    decode_and_run();
    BOOST_TEST(GET_EXCEPTION() == 8);
}
BOOST_DATA_TEST_CASE(value, bdata::xrange(2), T) {
    cpu.S = true;
    cpu.T = T;
    cpu.D[3] = 0x1234;
    cpu.A[2] = 0x1000;
    TEST::SET_W(0, 0007122);
    TEST::SET_W(2, 3 << 12 | 1 << 11);
    auto i = decode_and_run();
    BOOST_TEST(TEST::GET_W(0x1000) == 0x1234);
    BOOST_TEST(i == 2);
    BOOST_TEST(cpu.must_trace == !!T);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(err) {
    cpu.S = false;
    TEST::SET_W(0, 0007220);
    TEST::SET_W(2, 1 << 11);
    decode_and_run();
    BOOST_TEST(GET_EXCEPTION() == 8);
}
BOOST_DATA_TEST_CASE(value, bdata::xrange(2), T) {
    cpu.S = true;
    cpu.T = T;
    cpu.D[3] = 0x12345678;
    cpu.A[2] = 0x1000;
    TEST::SET_W(0, 0007222);
    TEST::SET_W(2, 3 << 12 | 1 << 11);
    auto i = decode_and_run();
    BOOST_TEST(TEST::GET_L(0x1000) == 0x12345678);
    BOOST_TEST(i == 2);
    BOOST_TEST(cpu.must_trace == !!T);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
