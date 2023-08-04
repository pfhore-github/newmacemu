#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(EORI, Prepare)

BOOST_AUTO_TEST_CASE(Byte) {
    TEST::SET_W(0, 0005000 | 2);
    TEST::SET_W(2, 0x34);
    cpu.D[2] = 0x12;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(cpu.D[2] == 0x26);
}

BOOST_AUTO_TEST_CASE(Word) {
    TEST::SET_W(0, 0005100 | 2);
    TEST::SET_W(2, 0x1234);
    cpu.D[2] = 0x5678;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(cpu.D[2] == 0x444C);
}


BOOST_AUTO_TEST_CASE(Long) {
    TEST::SET_W(0, 0005200 | 2);
    TEST::SET_L(2, 0x12345678);
    cpu.D[2] = 0x456789ab;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(cpu.D[2] == 0x5753dfd3);
}

BOOST_AUTO_TEST_CASE(CCR) {
    cpu.X = cpu.N = cpu.Z = cpu.V = cpu.C = true;
    TEST::SET_W(0, 0005074);
    TEST::SET_W(2, 0x1f);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}
BOOST_AUTO_TEST_SUITE(SR)

BOOST_AUTO_TEST_CASE(NG) {
    cpu.S = false;
    TEST::SET_W(0, 0005174);
    TEST::SET_W(2, 0xDFFF);
    BOOST_TEST(run_test() == 8);
}

BOOST_AUTO_TEST_CASE(OK) {
    cpu.S = true;
    TEST::SET_W(0, 0005174);
    TEST::SET_W(2, 0x2000);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(!cpu.S);
}


BOOST_AUTO_TEST_CASE(Traced) {
    cpu.S = true;
    cpu.T = 1;
    TEST::SET_W(0, 0005174);
    TEST::SET_W(2, 0x2000);
    BOOST_TEST(run_test() == 9);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
