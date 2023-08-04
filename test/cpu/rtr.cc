#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(RTR, Prepare)

BOOST_AUTO_TEST_CASE(execute) {
    TEST::SET_W(0, 0047167);
    TEST::SET_W(0x1000, 0x1F);
    TEST::SET_L(0x1002, 0x400);
    cpu.A[7] = 0x1000;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 0x400);
    BOOST_TEST(cpu.A[7] == 0x1006);
    BOOST_TEST(cpu.X);
    BOOST_TEST(cpu.N);
    BOOST_TEST(cpu.V);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(traced) {
    TEST::SET_W(0, 0047167);
    TEST::SET_W(0x1000, 0x1F);
    TEST::SET_L(0x1002, 0x400);
    cpu.A[7] = 0x1000;
    cpu.T = 1;
    BOOST_TEST(run_test() == 9);
}

BOOST_AUTO_TEST_SUITE_END()
