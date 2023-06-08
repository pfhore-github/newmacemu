#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(NBCD, Prepare)
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0044000 | 1);
    cpu.D[1] = 0x34;
    cpu.X = true;
    cpu.Z = true;
    int i = decode_and_run();
    BOOST_TEST(cpu.D[1] == 0x65);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(cpu.X);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(cpu.C);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0044000 | 1);
    cpu.Z = true;
    cpu.X = true;
    cpu.D[1] = 0x99;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(CX) {
    TEST::SET_W(0, 0044000 | 1);
    cpu.D[3] = 0;
    cpu.X = false;
    decode_and_run();
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}
BOOST_AUTO_TEST_SUITE_END()

