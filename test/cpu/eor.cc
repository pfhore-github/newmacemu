#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include "inline.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(EOR, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(XOR_B(0xFF, 0x8F) == 0x70);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(op) {
    TEST::SET_W(0, 0130400 | 3 << 9 | 2);
    cpu.D[3] = 0xED;
    cpu.D[2] = 0xFF;
    run_test();
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[2] == 0x12);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(XOR_W(0xFFFF, 0x8FFF) == 0x7000);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(op) {
    TEST::SET_W(0, 0130500 | 3 << 9 | 2);
    cpu.D[3] = 0xffff;
    cpu.D[2] = 0xeded;
    run_test();
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[2] == 0x1212);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(XOR_L(0xFFFFFFFF, 0x8FFFFFFF) == 0x70000000);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(op) {
    TEST::SET_W(0, 0130600 | 3 << 9 | 2);
    cpu.D[3] = 0xffffffff;
    cpu.D[2] = 0xedededed;
    run_test();
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[2] == 0x12121212);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
