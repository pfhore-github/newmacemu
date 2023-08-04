#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(UNPK, Prepare)

BOOST_AUTO_TEST_CASE(Reg) {
    TEST::SET_W(0, 0100600 | 3 << 9 | 1);
    TEST::SET_W(2, 5);
    cpu.D[1] = 0x34;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(cpu.D[3] == 0x0309);
}


BOOST_AUTO_TEST_CASE(Mem) {
    TEST::SET_W(0, 0100610 | 3 << 9 | 1);
    TEST::SET_W(2, 5);
    RAM[0x1000] = 0x34;
    cpu.A[1] = 0x1001;
    cpu.A[3] = 0x1102;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(TEST::GET_W(0x1100) == 0x0309);
    BOOST_TEST(cpu.A[1] == 0x1000);
    BOOST_TEST(cpu.A[3] == 0x1100);
}

BOOST_AUTO_TEST_SUITE_END()
