#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_CASE(JMP, Prepare) {
    TEST::SET_W(0, 0047320 | 3);
    cpu.A[3] = 0x2000;
    auto i = decode_and_run();
    BOOST_TEST(cpu.nextpc == 0x2000);
    BOOST_TEST(i == 0);

}

