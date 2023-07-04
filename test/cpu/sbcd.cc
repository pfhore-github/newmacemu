#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include "proto.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(SBCD, Prepare)
BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
    cpu.Z = true;
    BOOST_TEST(do_sbcd(0x71, 0x34, old_x) == (0x37 - old_x));
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(CX) {
    do_sbcd(0x30, 0x44, false);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.Z = true;
    do_sbcd(0x10, 0x10, false);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(Reg) {
    TEST::SET_W(0, 0100400 | 3 << 9 | 1);
    cpu.D[1] = 0x57;
    cpu.D[3] = 0x22;
    cpu.X = true;
    cpu.Z = true;
    int i = decode_and_run();
    BOOST_TEST(cpu.D[1] == 0x34);
    BOOST_TEST(i == 0);
}


BOOST_AUTO_TEST_CASE(Mem) {
    TEST::SET_W(0, 0100410 | 3 << 9 | 1);
    RAM[0x1000] = 0x47;
    RAM[0x1100] = 0x22;
    cpu.A[3] = 0x1101;
    cpu.A[1] = 0x1001;
    cpu.X = true;
    cpu.Z = true;
    int i = decode_and_run();
    BOOST_TEST(RAM[0x1000] == 0x24);
    BOOST_TEST(cpu.A[1] == 0x1000);
    BOOST_TEST(cpu.A[3] == 0x1100);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_SUITE_END()
