#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_DATA_TEST_CASE_F(Prepare, RTD, bdata::xrange(2), T) {
    TEST::SET_W(0, 0047164);
    TEST::SET_W(2, 16);
    TEST::SET_L(0x1000, 0x400);
    cpu.A[7] = 0x1000;
    cpu.T = T;
    auto i = decode_and_run();
    BOOST_TEST(cpu.nextpc == 0x400);
    BOOST_TEST(cpu.A[7] == 0x1014);
    BOOST_TEST(i == 2);
    BOOST_TEST(cpu.must_trace == !!T);

}

