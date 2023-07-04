#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_DATA_TEST_CASE_F(Prepare, NOP, bdata::xrange(2), T) {
    TEST::SET_W(0, 0047161);
    cpu.T = T;
    decode_and_run();
    BOOST_TEST(cpu.must_trace == !!T);
}

