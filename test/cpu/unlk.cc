#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(UNLK, Prepare)
BOOST_AUTO_TEST_CASE(execute) {
    TEST::SET_W(0, 0047130 | 2);
    TEST::SET_W(2, 0x40);
    cpu.A[2] = 0x1000;
    cpu.A[7] = 0x2000;
    TEST::SET_L(0x1000, 0x2500);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.A[2] == 0x2500);
    BOOST_TEST(cpu.A[7] == 0x1004);
}
BOOST_AUTO_TEST_SUITE_END()