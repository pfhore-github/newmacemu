#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(CMPA, Prepare)
BOOST_AUTO_TEST_CASE(Word) {
    TEST::SET_W(0, 0130300 | 3 << 9 | 2);
    cpu.A[3] = 0xfffffffe;
    cpu.D[2] = 0xfffe;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(Long) {
    TEST::SET_W(0, 0130700 | 3 << 9 | 2);
    cpu.A[3] = -2;
    cpu.D[2] = -2;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()
