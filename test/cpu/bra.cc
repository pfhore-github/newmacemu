#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(BRA, Prepare)

BOOST_DATA_TEST_CASE(traced, bdata::xrange(2), T) {
    TEST::SET_W(0, 0060000 | 4);
    cpu.T = T;
    decode_and_run();
    BOOST_TEST(cpu.must_trace == !!T);
}

BOOST_AUTO_TEST_CASE(offset1) {
    TEST::SET_W(0, 0060000 | 4);
    auto i = decode_and_run();
    BOOST_TEST(cpu.nextpc == 6);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(offsetNeg) {
    TEST::SET_W(0, 0060000 | 0xfe);
    auto i = decode_and_run();
    BOOST_TEST(cpu.nextpc == 0);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(offset00) {
    TEST::SET_W(0, 0060000  | 0);
    TEST::SET_W(2, 0x1000);
    auto i = decode_and_run();
    BOOST_TEST(cpu.nextpc == 0x1002);
    BOOST_TEST(i == 2);
}
BOOST_AUTO_TEST_CASE(offsetFF) {
    TEST::SET_W(0, 0060000  | 0xFF);
    TEST::SET_L(2, 0x20000);
    auto i = decode_and_run();
    BOOST_TEST(cpu.nextpc == 0x20002);
    BOOST_TEST(i == 4);
}

BOOST_AUTO_TEST_SUITE_END()
