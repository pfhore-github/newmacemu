#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(TST, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(vc) {
    TEST::SET_W(0, 0045000 | 2);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0045000 | 2);
    cpu.D[2] = 0x80;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0045000 | 2);
    cpu.D[2] = 0;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(vc) {
    TEST::SET_W(0, 0045100 | 2);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0045100 | 2);
    cpu.D[2] = 0x8000;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0045100 | 2);
    cpu.D[2] = 0;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(vc) {
    TEST::SET_W(0, 0045200 | 2);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0045200 | 2);
    cpu.D[2] = 0x80000000;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0045200 | 2);
    cpu.D[2] = 0;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
