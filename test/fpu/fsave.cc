#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(FSAVE, Prepare)
BOOST_AUTO_TEST_CASE(err) {
    if(setjmp(cpu.ex_buf) == 0) {
        cpu.S = false;
        TEST::SET_W(0, 0171420);
        decode_and_run();
        BOOST_ERROR("exception unoccured");
    } else {
        BOOST_TEST(cpu.ex_n == 8);
    }
}

BOOST_AUTO_TEST_CASE(normal) {
    cpu.S = true;
    cpu.A[2] = 0x1000;
    TEST::SET_W(0, 0171422);
    auto i = decode_and_run();
    BOOST_TEST(TEST::GET_L(0x1000) == 0x41000000);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_SUITE_END()