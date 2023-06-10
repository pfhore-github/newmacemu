#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_DATA_TEST_CASE_F(Prepare, TRAP, bdata::xrange(16), v) {
    if(setjmp(cpu.ex_buf) == 0) {
        TEST::SET_W(0, 0047100 | v);
        auto [f, i] = decode();
        BOOST_TEST(i == 0);
        f();
        BOOST_ERROR("exception unoccured");
    } else {
        BOOST_TEST(cpu.ex_n == 32+v);
    }
}
