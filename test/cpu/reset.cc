#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(RESET, Prepare)

BOOST_AUTO_TEST_CASE(user) {
    cpu.S = false;
    TEST::SET_W(0, 0047160);
    BOOST_TEST(run_test() == 8);
}


BOOST_AUTO_TEST_SUITE_END()