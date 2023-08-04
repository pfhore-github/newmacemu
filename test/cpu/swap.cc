#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(SWAP, Prepare)

BOOST_AUTO_TEST_CASE(execute) {
    TEST::SET_W(0, 0044100 | 2);
    cpu.D[2] = 0x12345678;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[2] == 0x56781234);
}

BOOST_AUTO_TEST_SUITE_END()