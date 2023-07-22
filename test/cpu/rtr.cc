#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_DATA_TEST_CASE_F(Prepare, RTR, bdata::xrange(2), T) {
    TEST::SET_W(0, 0047167);
    TEST::SET_W(0x1000, 0x1F);
    TEST::SET_L(0x1002, 0x400);
    cpu.A[7] = 0x1000;
    cpu.T = T;
    decode_and_run();
    BOOST_TEST(cpu.PC == 0x400);
    BOOST_TEST(cpu.A[7] == 0x1006);
    BOOST_TEST(cpu.X);
    BOOST_TEST(cpu.N);
    BOOST_TEST(cpu.V);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(cpu.must_trace == !!T);
}
