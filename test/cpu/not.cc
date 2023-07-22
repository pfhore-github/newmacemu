#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include "proto.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(NOT, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0043003);
    BOOST_TEST(disasm() == "NOT.B %D3");
}

BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(NOT_B(0xED) == 0x12);
}

BOOST_AUTO_TEST_CASE(operand) {
    TEST::SET_W(0, 0043000 | 2);
    cpu.D[2] = 0xed;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x12);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0043103);
    BOOST_TEST(disasm() == "NOT.W %D3");
}
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(NOT_W(0xedcb) == 0x1234);
}

BOOST_AUTO_TEST_CASE(operand) {
    TEST::SET_W(0, 0043100 | 2);
    cpu.D[2] = 0xedcb;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x1234);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0043203);
    BOOST_TEST(disasm() == "NOT.L %D3");
}
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(NOT_L(0xfedcba98) == 0x01234567);
}
BOOST_AUTO_TEST_CASE(operand) {
    TEST::SET_W(0, 0043200 | 2);
    cpu.D[2] = 0xfedcba98;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x01234567);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
