#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(MOVEP, Prepare)
BOOST_AUTO_TEST_SUITE(ToMem)
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0003615);
    TEST::SET_W(2, 50);
    BOOST_TEST(disasm() == "MOVEP.W %D3, (#50, %A5)");
}
BOOST_AUTO_TEST_CASE(Decode) {
    TEST::SET_W(0, 0000610 | 3 << 9 | 2);
    TEST::SET_W(2, 0x100);
    cpu.D[3] = 0x1234;
    cpu.A[2] = 0x1000;
    auto i = decode_and_run();
    BOOST_TEST(RAM[0x1100] == 0x12);
    BOOST_TEST(RAM[0x1102] == 0x34);
    BOOST_TEST(i == 2);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0003715);
    TEST::SET_W(2, 50);
    BOOST_TEST(disasm() == "MOVEP.L %D3, (#50, %A5)");
}
BOOST_AUTO_TEST_CASE(Decode) {
    TEST::SET_W(0, 0000710 | 3 << 9 | 2);
    TEST::SET_W(2, 0x100);
    cpu.D[3] = 0x12345678;
    cpu.A[2] = 0x1000;
    auto i = decode_and_run();
    BOOST_TEST(RAM[0x1100] == 0x12);
    BOOST_TEST(RAM[0x1102] == 0x34);
    BOOST_TEST(RAM[0x1104] == 0x56);
    BOOST_TEST(RAM[0x1106] == 0x78);
    BOOST_TEST(i == 2);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ToReg)
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0003415);
    TEST::SET_W(2, 50);
    BOOST_TEST(disasm() == "MOVEP.W (#50, %A5), %D3");
}
BOOST_AUTO_TEST_CASE(Decode) {
    TEST::SET_W(0, 0000410 | 3 << 9 | 2);
    TEST::SET_W(2, 0x100);
    cpu.A[2] = 0x1000;
    TEST::SET_L(0x1100, 0x12345678);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[3] == 0x1256);
    BOOST_TEST(i == 2);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0003515);
    TEST::SET_W(2, 50);
    BOOST_TEST(disasm() == "MOVEP.L (#50, %A5), %D3");
}

BOOST_AUTO_TEST_CASE(Decode) {
    TEST::SET_W(0, 0000510 | 3 << 9 | 2);
    TEST::SET_W(2, 0x100);
    cpu.D[3] = 0x12345678;
    cpu.A[2] = 0x1000;
    TEST::SET_L(0x1100, 0x12345678);
    TEST::SET_L(0x1104, 0x9ABCDEF0);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[3] == 0x12569ADE);
    BOOST_TEST(i == 2);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
