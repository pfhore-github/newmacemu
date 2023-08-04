#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(SUBI, Prepare)
// value & flags are tested at SUB
BOOST_AUTO_TEST_CASE(Byte) {
    TEST::SET_W(0, 0002000 | 2);
    TEST::SET_W(2, 14);
    cpu.D[2] = 21;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(cpu.D[2] == 7);
}

BOOST_AUTO_TEST_CASE(Word) {
    TEST::SET_W(0, 0002100 | 2);
    TEST::SET_W(2, 213);
    cpu.D[2] = 3456;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(cpu.D[2] == 3243);
}

BOOST_AUTO_TEST_CASE(Long) {
    TEST::SET_W(0, 0002200 | 2);
    TEST::SET_L(2, 44444);
    cpu.D[2] = 123456789;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(cpu.D[2] == 123412345);
}
BOOST_AUTO_TEST_SUITE_END()
