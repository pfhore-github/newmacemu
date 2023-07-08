#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
static bool is_reset = false;
void bus_reset() { is_reset = true; }
BOOST_FIXTURE_TEST_SUITE(RESET, Prepare)
BOOST_AUTO_TEST_CASE(user) {
    is_reset = false;
    cpu.S = false;
    TEST::SET_W(0, 0047160);
    decode_and_run();
    BOOST_TEST(GET_EXCEPTION() == 8);
}

BOOST_AUTO_TEST_CASE(sys) {
    is_reset = false;
    cpu.S = true;
    TEST::SET_W(0, 0047160);
    auto i = decode_and_run();
    BOOST_TEST(i == 0);
    BOOST_TEST(is_reset);
}
BOOST_AUTO_TEST_SUITE_END()