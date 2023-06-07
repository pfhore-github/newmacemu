
#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(BKPT, Prepare)
BOOST_AUTO_TEST_CASE(run) {
    if(setjmp(cpu.ex_buf) == 0) {
        TEST::SET_W(0, 0044110);
        auto [f, i] = decode();
        BOOST_TEST(i == 0);
        f();
        BOOST_ERROR("exception unoccured");
    } else {
        BOOST_TEST(cpu.ex_n == 4);
    }
}
BOOST_AUTO_TEST_SUITE_END()