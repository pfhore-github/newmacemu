#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_CASE(PEA, Prepare) {
    TEST::SET_W(0, 0044120 | 3);
    cpu.A[3] = 0x2000;
    cpu.A[7] = 0x1004;
    auto i = decode_and_run();
    BOOST_TEST(TEST::GET_L(0x1000)  == 0x2000);
    BOOST_TEST(cpu.A[7] == 0x1000);
    BOOST_TEST(i == 0);

}

