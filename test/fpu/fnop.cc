#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_CASE(FNOP, Prepare) {
    TEST::SET_W(0, 0171200 | 0xF);
    TEST::SET_W(2, 0);
    auto i = decode_and_run();
    BOOST_TEST(cpu.nextpc == 0x2);
    BOOST_TEST(i == 2);

}

