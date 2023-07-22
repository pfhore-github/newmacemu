#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(MOVE, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0010000 | 3 << 9 | 2);
    BOOST_TEST(disasm() == "MOVE.B %D2, %D3");
}
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0010000 | 3 << 9 | 2);
    cpu.D[3] = 0x12345678;
    cpu.D[2] = 0x9ABCDE02;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[3] == 0x12345602);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(imm) {
    TEST::SET_W(0, 0010074 | 3 << 9);
    TEST::SET_W(2, 0x34);
    cpu.D[3] = 0x12345678;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[3] == 0x12345634);
    BOOST_TEST(i == 2);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0030000 | 3 << 9 | 2);
    BOOST_TEST(disasm() == "MOVE.W %D2, %D3");
}
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0030000 | 3 << 9 | 2);
    cpu.D[3] = 0x12345678;
    cpu.D[2] = 0x9ABC0123;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[3] == 0x12340123);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(imm) {
    TEST::SET_W(0, 0030074 | 3 << 9);
    TEST::SET_W(2, 0x3456);
    cpu.D[3] = 0x12345678;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[3] == 0x12343456);
    BOOST_TEST(i == 2);
}
BOOST_AUTO_TEST_SUITE(FromCCR)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0041303);
    BOOST_TEST(disasm() == "MOVE.W %CCR, %D3");
}
BOOST_AUTO_TEST_CASE(run) {
    cpu.X = cpu.N = cpu.Z = cpu.V = cpu.C = true;
    TEST::SET_W(0, 0041300 | 3);
    auto i = decode_and_run();
    BOOST_TEST(i == 0);
    BOOST_TEST(cpu.D[3] == 0x1F);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(ToCCR)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0042303);
    BOOST_TEST(disasm() == "MOVE.W %D3, %CCR");
}
BOOST_AUTO_TEST_CASE(run) {
    TEST::SET_W(0, 0042300 | 3);
    cpu.D[3] = 0x1F;
    auto i = decode_and_run();
    BOOST_TEST(i == 0);
    BOOST_TEST(cpu.X);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(cpu.V);
    BOOST_TEST(cpu.N);
    BOOST_TEST(cpu.C);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(FromSR)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0040303);
    BOOST_TEST(disasm() == "MOVE.W %SR, %D3");
}
BOOST_AUTO_TEST_CASE(Err) {
    cpu.S = false;
    TEST::SET_W(0, 0040300);
    decode_and_run();
    BOOST_TEST(GET_EXCEPTION() == 8);
}

BOOST_AUTO_TEST_CASE(Ok) {
    cpu.S = true;
    cpu.T = 0;
    cpu.M = false;
    cpu.X = cpu.N = cpu.Z = cpu.V = cpu.C = true;
    TEST::SET_W(0, 0040300 | 3);
    auto i = decode_and_run();
    BOOST_TEST(i == 0);
    BOOST_TEST(cpu.D[3] == 0x201F);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(ToSR)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0043303);
    BOOST_TEST(disasm() == "MOVE.W %D3, %SR");
}

BOOST_AUTO_TEST_CASE(Err) {
    cpu.S = false;
    TEST::SET_W(0, 0043300);
    decode_and_run();
    BOOST_TEST(GET_EXCEPTION() == 8);
}

BOOST_AUTO_TEST_CASE(Ok) {
    cpu.S = true;
    cpu.D[3] = 0x1F;
    TEST::SET_W(0, 0043300 | 3);
    auto i = decode_and_run();
    BOOST_TEST(i == 0);
    BOOST_TEST(!cpu.S);
    BOOST_TEST(cpu.X);
    BOOST_TEST(cpu.N);
    BOOST_TEST(cpu.V);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0020000 | 3 << 9 | 2);
    BOOST_TEST(disasm() == "MOVE.L %D2, %D3");
}
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0020000 | 3 << 9 | 2);
    cpu.D[3] = 0x12345678;
    cpu.D[2] = 0x76543210;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[3] == 0x76543210);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(imm) {
    TEST::SET_W(0, 0020074 | 3 << 9);
    TEST::SET_L(2, 0xdeadbeaf);
    cpu.D[3] = 0x12345678;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[3] == 0xdeadbeaf);
    BOOST_TEST(i == 4);
}
BOOST_AUTO_TEST_SUITE(FomrUSP)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0047154);
    BOOST_TEST(disasm() == "MOVE %USP, %A4");
}
BOOST_AUTO_TEST_CASE(Err) {
    cpu.S = false;
    TEST::SET_W(0, 0047150);
    decode_and_run();
    BOOST_TEST(GET_EXCEPTION() == 8);
}

BOOST_DATA_TEST_CASE(Ok, bdata::xrange(2), T) {
    cpu.S = true;
    cpu.T = T;
    cpu.USP = 0x3000;
    TEST::SET_W(0, 0047150 | 3);
    auto i = decode_and_run();
    BOOST_TEST(i == 0);
    BOOST_TEST(cpu.A[3] == 0x3000);
    BOOST_TEST(cpu.must_trace == !!T);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(ToUSP)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0047144);
    BOOST_TEST(disasm() == "MOVE %A4, %USP");
}
BOOST_AUTO_TEST_CASE(Err) {
    cpu.S = false;
    TEST::SET_W(0, 0047140);
    decode_and_run();
    BOOST_TEST(GET_EXCEPTION() == 8);
}

BOOST_DATA_TEST_CASE(Ok, bdata::xrange(2), T) {
    cpu.S = true;
    cpu.T = T;
    cpu.A[3] = 0x3000;
    TEST::SET_W(0, 0047140 | 3);
    auto i = decode_and_run();
    BOOST_TEST(i == 0);
    BOOST_TEST(cpu.USP == 0x3000);
    BOOST_TEST(cpu.must_trace == !!T);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
