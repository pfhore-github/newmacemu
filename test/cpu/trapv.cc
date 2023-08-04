#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(TRAPV, Prepare)

BOOST_AUTO_TEST_CASE(t) {
    cpu.V = true;
    TEST::SET_W(0, 0047166);
    BOOST_TEST(run_test() == 7);
}
BOOST_AUTO_TEST_CASE(f) {
    cpu.V = false;
    TEST::SET_W(0, 0047166);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
}

BOOST_AUTO_TEST_SUITE_END()
