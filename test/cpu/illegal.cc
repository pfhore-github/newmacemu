#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_CASE(ILLEGAL, Prepare) {
    TEST::SET_W(0, 0045374);
    BOOST_CHECK_THROW(decode(), DecodeError);
}
