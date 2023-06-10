#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(PACK, Prepare)
BOOST_AUTO_TEST_CASE(Reg) {
    TEST::SET_W(0, 0100500 | 3 << 9 | 1);
    TEST::SET_W(2, 5);
    cpu.D[1] = 0x0304;
    int i = decode_and_run();
    BOOST_TEST(cpu.D[3] == 0x39);
    BOOST_TEST(i == 2);
}


BOOST_AUTO_TEST_CASE(Mem) {
    TEST::SET_W(0, 0100510 | 3 << 9 | 1);
    TEST::SET_W(2, 5);
    TEST::SET_W(0x1000, 0x0304);
    cpu.A[1] = 0x1002;
    cpu.A[3] = 0x1101;
    int i = decode_and_run();
    BOOST_TEST(RAM[0x1100] == 0x39);
    BOOST_TEST(cpu.A[1] == 0x1000);
    BOOST_TEST(cpu.A[3] == 0x1100);
    BOOST_TEST(i == 2);
}

BOOST_AUTO_TEST_SUITE_END()
