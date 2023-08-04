#define BOOST_TEST_DYN_LINK
#include "proto.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(CMPM, Prepare)


// flags; see cmp
BOOST_AUTO_TEST_CASE(Byte) {
    TEST::SET_W(0, 0130410 | 3 << 9 | 1);
    RAM[0x1000] = 0x20;
    RAM[0x1100] = 0x20;
    cpu.A[3] = 0x1000;
    cpu.A[1] = 0x1100;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.A[3] == 0x1001);
    BOOST_TEST(cpu.A[1] == 0x1101);
    BOOST_TEST(cpu.Z);
}






BOOST_AUTO_TEST_CASE(Word) {
    TEST::SET_W(0, 0130510 | 3 << 9 | 1);
    TEST::SET_W(0x1000, 1200);
    TEST::SET_W(0x1100, 1200);
    cpu.A[3] = 0x1000;
    cpu.A[1] = 0x1100;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.A[3] == 0x1002);
    BOOST_TEST(cpu.A[1] == 0x1102);
    BOOST_TEST(cpu.Z);
}


BOOST_AUTO_TEST_CASE(Long) {
    TEST::SET_W(0, 0130610 | 3 << 9 | 1);
    TEST::SET_L(0x1000, 120000);
    TEST::SET_L(0x1100, 120000);
    cpu.A[3] = 0x1000;
    cpu.A[1] = 0x1100;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.A[3] == 0x1004);
    BOOST_TEST(cpu.A[1] == 0x1104);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()
