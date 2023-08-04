#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(NOP, Prepare)
BOOST_AUTO_TEST_CASE(execute) {
    TEST::SET_W(0, 0047161);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
}
BOOST_AUTO_TEST_CASE(trace_always) {
    TEST::SET_W(0, 0047161);
    cpu.T = 2;
    BOOST_TEST(run_test() == 9);
}

BOOST_AUTO_TEST_CASE(tracecd) {
    TEST::SET_W(0, 0047161);
    cpu.T = 1;
    BOOST_TEST(run_test() == 9);
}
BOOST_AUTO_TEST_SUITE_END()
