#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(CLR, Prepare)

BOOST_AUTO_TEST_CASE(Byte) {
    TEST::SET_W(0, 0041000 | 2 );
    cpu.D[2] = 0x12345678;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[2] == 0x12345600);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(Word) {
    TEST::SET_W(0, 0041100 | 2 );
    cpu.D[2] = 0x12345678;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[2] == 0x12340000);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(Long) {
    TEST::SET_W(0, 0041200 | 2 );
    cpu.D[2] = 0x12345678;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[2] == 0);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_SUITE_END()
