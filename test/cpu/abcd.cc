#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include "proto.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(ABCD, Prepare)

BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
    cpu.Z = true;
    BOOST_TEST(do_abcd(0x22, 0x34, old_x) == (0x56 + old_x));
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(CX) {
    do_abcd(0x77, 0x44, false);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.Z = true;
    do_abcd(0, 0, false);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(Reg) {
    TEST::SET_W(0, 0140400 | 3 << 9 | 1);
    cpu.D[3] = 0x22;
    cpu.D[1] = 0x34;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[3] == 0x56);
}


BOOST_AUTO_TEST_CASE(Mem) {
    TEST::SET_W(0, 0140410 | 3 << 9 | 1);
    RAM[0x1000] = 0x22;
    RAM[0x1100] = 0x34;
    cpu.A[3] = 0x1001;
    cpu.A[1] = 0x1101;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(RAM[0x1000] == 0x56);
    BOOST_TEST(cpu.A[3] == 0x1000);
    BOOST_TEST(cpu.A[1] == 0x1100);
}

BOOST_AUTO_TEST_SUITE_END()
