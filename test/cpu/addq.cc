#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(ADDQ, Prepare)
// value & flags are tested at ADD
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(err) {
    TEST::SET_W(0, 0050000 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}

BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0050000 | 3 << 9 | 2);
    cpu.D[2] = 21;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 24);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(An) {
    TEST::SET_W(0, 0050010 | 3 << 9 | 2);
     BOOST_CHECK_THROW(decode(), DecodeError);
}

BOOST_AUTO_TEST_CASE(eight) {
    TEST::SET_W(0, 0050000 | 0 << 9 | 2);
    cpu.D[2] = 21;
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 29);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(err) {
    TEST::SET_W(0, 0050100 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}

BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0050100 | 3 << 9 | 2);
    cpu.D[2] = 2000;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 2003);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(An) {
    TEST::SET_W(0, 0050110 | 3 << 9 | 2);
    cpu.A[2] = 2100;
    auto i = decode_and_run();
    BOOST_TEST(cpu.A[2] == 2103);
    BOOST_TEST(i == 0);
}


BOOST_AUTO_TEST_CASE(eight) {
    TEST::SET_W(0, 0050100 | 0 << 9 | 2);
    cpu.D[2] = 2100;
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 2108);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(err) {
    TEST::SET_W(0, 0050200 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}

BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0050200 | 1 << 9 | 2);
    cpu.D[2] = 142044;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 142045);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(eight) {
    TEST::SET_W(0, 0050200 | 0 << 9 | 2);
    cpu.D[2] = 142044;
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 142052);
}

BOOST_AUTO_TEST_CASE(An) {
    TEST::SET_W(0, 0050210 | 3 << 9 | 2);
    cpu.A[2] = 210000;
    auto i = decode_and_run();
    BOOST_TEST(cpu.A[2] == 210003);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
