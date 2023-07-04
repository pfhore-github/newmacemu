#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(SUBQ, Prepare)
// value & flags are tested at ADD
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(err) {
    TEST::SET_W(0, 0050400 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}

BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0050400 | 3 << 9 | 2);
    cpu.D[2] = 21;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 18);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(eight) {
    TEST::SET_W(0, 0050400 | 0 << 9 | 2);
    cpu.D[2] = 21;
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 13);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(err) {
    TEST::SET_W(0, 0050500 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}

BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0050500 | 3 << 9 | 2);
    cpu.D[2] = 2000;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 1997);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(eight) {
    TEST::SET_W(0, 0050500 | 0 << 9 | 2);
    cpu.D[2] = 2100;
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 2092);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(err) {
    TEST::SET_W(0, 0050600 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}

BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0050600 | 1 << 9 | 2);
    cpu.D[2] = 142044;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 142043);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(eight) {
    TEST::SET_W(0, 0050600 | 0 << 9 | 2);
    cpu.D[2] = 200009;
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 200001);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
