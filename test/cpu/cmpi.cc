#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(CMPI, Prepare)
// flag test: see cmp

BOOST_AUTO_TEST_CASE(Byte) {
    TEST::SET_W(0, 006000 | 2);
    TEST::SET_W(2, 0x10);
    cpu.D[2] = 0x10;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(Word) {
    TEST::SET_W(0, 006100 | 2);
    TEST::SET_W(2, 0x1000);
    cpu.D[2] = 0x1000;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(cpu.Z);
}


BOOST_AUTO_TEST_CASE(Long) {
    TEST::SET_W(0, 006200 | 2);
    TEST::SET_L(2, -2);
    cpu.D[2] = -2;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
