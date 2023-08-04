#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "proto.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(NEG, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(NEG_B(-1) == 1);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(V) {
    NEG_B(0x80);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(operand) {
    TEST::SET_W(0, 0042000 | 2);
    cpu.D[2] = 2;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[2] == 0xfe);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)


BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(NEG_W(-1000) == 1000);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(V) {
    NEG_W(0x8000);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(operand) {
    TEST::SET_W(0, 0042100 | 2);
    cpu.D[2] = 0xffff;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[2] == 1);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)


BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(NEG_L(-10000) == 10000);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(V) {
    NEG_L(0x80000000);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(operand) {
    TEST::SET_W(0, 0042200 | 2);
    cpu.D[2] = -5;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[2] == 5);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
