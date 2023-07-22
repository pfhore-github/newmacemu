#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "proto.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(NEGX, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0040003);
    BOOST_TEST(disasm() == "NEGX.B %D3");
}
BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
    BOOST_TEST(NEGX_B(-2, old_x) == 2 - old_x);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}


BOOST_AUTO_TEST_CASE(V) {
    NEGX_B(0x80, false);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(operand) {
    TEST::SET_W(0, 0040000 | 2);
    cpu.D[2] = 0xfe;
    cpu.Z = true;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 2);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0040103);
    BOOST_TEST(disasm() == "NEGX.W %D3");
}

BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
    BOOST_TEST(NEGX_W(-200, old_x) == 200 - old_x);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}


BOOST_AUTO_TEST_CASE(V) {
    NEGX_W(0x8000, false);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(operand) {
    TEST::SET_W(0, 0040100 | 2);
    cpu.D[2] = 0xfffe;
    cpu.Z = true;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2]);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0040203);
    BOOST_TEST(disasm() == "NEGX.L %D3");
}

BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
    BOOST_TEST(NEGX_L(-200000, old_x) == 200000 - old_x);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(V) {
    NEGX_L(0x80000000, false);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(operand) {
    TEST::SET_W(0, 0040200 | 2);
    cpu.D[2] = -5;
    cpu.Z = true;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 5);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
