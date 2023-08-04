#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(ADDI, Prepare)
// value & flags are tested at ADD
BOOST_AUTO_TEST_CASE(Byte) {
    TEST::SET_W(0, 0003000 | 2);
    TEST::SET_W(2, 14);
    cpu.D[2] = 21;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(cpu.D[2] == 35);
}

BOOST_AUTO_TEST_CASE(Word) {
    TEST::SET_W(0, 0003100 | 2);
    TEST::SET_W(2, 1420);
    cpu.D[2] = 2133;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(cpu.D[2] == 3553);
}

BOOST_AUTO_TEST_CASE(Long) {
    TEST::SET_W(0, 0003200 | 2);
    TEST::SET_L(2, 142044);
    cpu.D[2] = 213324;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(cpu.D[2] == 355368);
}

BOOST_AUTO_TEST_SUITE_END()
