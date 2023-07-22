#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include "proto.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(BCLR, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(BCLR_B(0xFF, 3) == 0xF7);
}
// Z; see BTST/Z

BOOST_AUTO_TEST_SUITE(ByImm)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0004223);
    TEST::SET_W(2, 5);
    BOOST_TEST(disasm() == "BCLR.B #5, (%A3)");
    cpu.PC = 0;
    TEST::SET_W(2, 0);
    BOOST_TEST(disasm() == "BCLR.B #8, (%A3)");
}
BOOST_AUTO_TEST_CASE(Decode) {
    TEST::SET_W(0, 0004220 | 2);
    TEST::SET_W(2, 3);
    RAM[0x1000] = 0xff;
    cpu.A[2] = 0x1000;
    auto i = decode_and_run();
    BOOST_TEST(i == 2);
    BOOST_TEST(RAM[0x1000] == 0xF7);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(ByReg)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0003625);
    BOOST_TEST(disasm() == "BCLR.B %D3, (%A5)");
}
BOOST_AUTO_TEST_CASE(Decode) {
    TEST::SET_W(0, 0000620 | 2 | 4 << 9);
    cpu.D[4] = 3;
    RAM[0x1000] = 0xff;
    cpu.A[2] = 0x1000;
    auto i = decode_and_run();
    BOOST_TEST(i == 0);
    BOOST_TEST(RAM[0x1000] == 0xF7);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(BCLR_L(0xffffffff, 20) == 0xFFEFFFFF);
}

// Z; see BTST/Z
BOOST_AUTO_TEST_SUITE(ByImm)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0004203);
    TEST::SET_W(2, 5);
    BOOST_TEST(disasm() == "BCLR.L #5, %D3");
}

BOOST_AUTO_TEST_CASE(Decode) {
    TEST::SET_W(0, 0004200 | 2);
    TEST::SET_W(2, 20);
    cpu.D[2] = 0xffffffff;
    auto i = decode_and_run();
    BOOST_TEST(i == 2);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(cpu.D[2] == 0xFFEFFFFF);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(ByReg)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0003605);
    BOOST_TEST(disasm() == "BCLR.L %D3, %D5");
}
BOOST_AUTO_TEST_CASE(Decode) {
    TEST::SET_W(0, 0000600 | 2 | 4 << 9);
    cpu.D[2] = 0xffffffff;
    cpu.D[4] = 20;
    auto i = decode_and_run();
    BOOST_TEST(i == 0);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(cpu.D[2] == 0xFFEFFFFF);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()