#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(RTD, Prepare)

BOOST_AUTO_TEST_CASE(execute) {
    TEST::SET_W(0, 0047164);
    TEST::SET_W(2, 16);
    TEST::SET_L(0x1000, 0x400);
    cpu.A[7] = 0x1000;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 0x400);
    BOOST_TEST(cpu.A[7] == 0x1014);
}

BOOST_AUTO_TEST_CASE(traced) {
    TEST::SET_W(0, 0047164);
    TEST::SET_W(2, 16);
    TEST::SET_L(0x1000, 0x400);
    cpu.A[7] = 0x1000;
    cpu.T = 1;
    BOOST_TEST(run_test() == 9);
}

BOOST_AUTO_TEST_SUITE_END()