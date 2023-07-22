#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_DATA_TEST_CASE_F(Prepare, TRAP, bdata::xrange(16), v) {
    TEST::SET_W(0, 0047100 | v);
    decode_and_run();
    BOOST_TEST(GET_EXCEPTION() == 32 + v);
}
