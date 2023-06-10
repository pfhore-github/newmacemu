#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(TRAPV, Prepare)
BOOST_AUTO_TEST_CASE(t) {
    if(setjmp(cpu.ex_buf) == 0) {
        cpu.V = true;
        TEST::SET_W(0, 0047166);
        auto [f, i] = decode();
        BOOST_TEST(i == 0);
        f();
        BOOST_ERROR("exception unoccured");
    } else {
        BOOST_TEST(cpu.ex_n == 7);
    }
}
BOOST_AUTO_TEST_CASE(f) {
    cpu.V = false;
    TEST::SET_W(0, 0047166);
    decode_and_run();
    BOOST_TEST_PASSPOINT();
}

BOOST_AUTO_TEST_SUITE_END()
