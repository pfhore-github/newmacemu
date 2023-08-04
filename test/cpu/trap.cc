#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(TRAP, Prepare)

BOOST_DATA_TEST_CASE(execute, bdata::xrange(16), v) {
    TEST::SET_W(0, 0047100 | v);
    BOOST_TEST(run_test() == 32 + v);
}
BOOST_AUTO_TEST_SUITE_END()