#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include "proto.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(NBCD, Prepare)
BOOST_AUTO_TEST_CASE(value) {
    cpu.D[1] = 0x34;
    cpu.Z = true;
    BOOST_TEST(do_nbcd(0x34, true) == 0x65);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(cpu.X);
    BOOST_TEST(cpu.C);
}
BOOST_AUTO_TEST_CASE(Z) {
    cpu.Z = true;
    do_nbcd(0x99, true);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(CX) {
    do_nbcd(0, false);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(operand) {
    TEST::SET_W(0, 0044000 | 1);
    cpu.D[1] = 0x34;
    cpu.X = true;
    cpu.Z = true;
    int i = decode_and_run();
    BOOST_TEST(cpu.D[1] == 0x65);
    BOOST_TEST(i == 0);
}


BOOST_AUTO_TEST_SUITE_END()

