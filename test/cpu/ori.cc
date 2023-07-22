#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(ORI, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0000003);
    TEST::SET_W(2, 0x23);
    BOOST_TEST(disasm() == "ORI.B #0x23, %D3");
}
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0000000 | 2);
    TEST::SET_W(2, 0x1F);
    cpu.D[2] = 0x20;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x3F);
    BOOST_TEST(i == 2);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0000103);
    TEST::SET_W(2, 0x2345);
    BOOST_TEST(disasm() == "ORI.W #0x2345, %D3");
}

BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0000100 | 2);
    TEST::SET_W(2, 0x2020);
    cpu.D[2] = 0x1F1F;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x3F3F);
    BOOST_TEST(i == 2);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0000203);
    TEST::SET_L(2, 0x23456789);
    BOOST_TEST(disasm() == "ORI.L #0x23456789, %D3");
}

BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0000200 | 2);
    TEST::SET_L(2, 0x20202020);
    cpu.D[2] = 0x1F1F1F1F;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x3F3F3F3F);
    BOOST_TEST(i == 4);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(CCR)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0000074);
    TEST::SET_W(2, 0x0023);
    BOOST_TEST(disasm() == "ORI.B #0x23, %CCR");
}
BOOST_AUTO_TEST_CASE(run) {
    cpu.X = cpu.N = cpu.Z = cpu.V = cpu.C = false;
    TEST::SET_W(0, 0000074);
    TEST::SET_W(2, 0x1F);
    auto i = decode_and_run();
    BOOST_TEST(i == 2);
    BOOST_TEST(cpu.X);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(cpu.N);
    BOOST_TEST(cpu.V);
    BOOST_TEST(cpu.C);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(SR)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0000174);
    TEST::SET_W(2, 0x1234);
    BOOST_TEST(disasm() == "ORI.W #0x1234, %SR");
}
BOOST_AUTO_TEST_CASE(Err) {
    cpu.S = false;
    TEST::SET_W(0, 0000174);
    TEST::SET_W(2, 0xDFFF);
    decode_and_run();
    BOOST_TEST(GET_EXCEPTION() == 8);
}

BOOST_DATA_TEST_CASE(Ok, bdata::xrange(2), tr) {
    cpu.S = true;
    cpu.T = tr;
    TEST::SET_W(0, 0000174);
    TEST::SET_W(2, 1 << 12);
    auto i = decode_and_run();
    BOOST_TEST(i == 2);
    BOOST_TEST(cpu.M);
    BOOST_TEST(cpu.must_trace == !!tr);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
