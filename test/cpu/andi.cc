#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(ANDI, Prepare)
// value/flags are tested at AND
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0001003);
    TEST::SET_W(2, 0x23);
    BOOST_TEST(disasm() == "ANDI.B #0x23, %D3");
}
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0001000 | 2);
    TEST::SET_W(2, 0x40);
    cpu.D[2] = 0xFF;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x40);
    BOOST_TEST(i == 2);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0001103);
    TEST::SET_W(2, 0x2345);
    BOOST_TEST(disasm() == "ANDI.W #0x2345, %D3");
}

BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0001100 | 2);
    TEST::SET_W(2, 0x7fff);
    cpu.D[2] = 0xffff;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x7fff);
    BOOST_TEST(i == 2);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0001203);
    TEST::SET_L(2, 0x23456789);
    BOOST_TEST(disasm() == "ANDI.L #0x23456789, %D3");
}

BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0001200 | 2);
    TEST::SET_L(2, 0x7fffffff);
    cpu.D[2] = 0xffff0000;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x7fff0000);
    BOOST_TEST(i == 4);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Ccr)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0001074);
    TEST::SET_W(2, 0x0023);
    BOOST_TEST(disasm() == "ANDI.B #0x23, %CCR");
}
BOOST_AUTO_TEST_CASE(run) {
    cpu.X = cpu.N = cpu.Z = cpu.V = cpu.C = true;
    TEST::SET_W(0, 0001074);
    TEST::SET_W(2, 0);
    auto i = decode_and_run();
    BOOST_TEST(i == 2);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(SR)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0001174);
    TEST::SET_W(2, 0x1234);
    BOOST_TEST(disasm() == "ANDI.W #0x1234, %SR");
}
BOOST_AUTO_TEST_CASE(Err) {
    cpu.S = false;
    TEST::SET_W(0, 0001174);
    TEST::SET_W(2, 0xDFFF);
    decode_and_run();
    BOOST_TEST(GET_EXCEPTION() == 8);
}

BOOST_DATA_TEST_CASE(Ok, bdata::xrange(2), tr) {
    cpu.S = true;
    cpu.T = tr;
    cpu.PC = 0;
    TEST::SET_W(0, 0001174);
    TEST::SET_W(2, 0xDFFF);
    auto i = decode_and_run();
    BOOST_TEST(i == 2);
    BOOST_TEST(!cpu.S);
    BOOST_TEST(cpu.must_trace == !!tr);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
