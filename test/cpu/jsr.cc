#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(JSR, Prepare)


BOOST_AUTO_TEST_CASE(execute) {
    TEST::SET_W(0, 0047220 | 3);
    cpu.A[3] = 0x2000;
    cpu.A[7] = 0x1004;

    run_test();
    BOOST_TEST(cpu.PC == 0x2000);
    BOOST_TEST(TEST::GET_L(0x1000) == 2);
    BOOST_TEST(cpu.A[7] == 0x1000);
}

BOOST_AUTO_TEST_CASE(traced) {
    TEST::SET_W(0, 0047220 | 3);
    cpu.A[3] = 0x2000;
    cpu.A[7] = 0x1004;
    
    cpu.T = 1;
    BOOST_TEST(run_test() == 9);
}
BOOST_AUTO_TEST_SUITE_END()