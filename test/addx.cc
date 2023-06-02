#define BOOST_TEST_DYN_LINK
#include "inline.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_AUTO_TEST_SUITE(ADDX)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_SUITE(Reg)
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0150400 | 3 << 9 | 1);
    cpu.D[3] = 22;
    cpu.D[1] = 23;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.Z = true;
    cpu.V = cpu.C = true;
    cpu.X = true;
    f();
    BOOST_TEST(cpu.D[3] == 46);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0150400 | 3 << 9 | 1);
    cpu.D[3] = 0;
    cpu.D[1] = 0;
    cpu.Z = true;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(V) {
    TEST::SET_W(0, 0150400 | 3 << 9 | 1);
    cpu.X = false;
    cpu.D[3] = 0x7f;
    cpu.D[1] = 1;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(CX) {
    TEST::SET_W(0, 0150400 | 3 << 9 | 1);
    cpu.X = false;
    cpu.D[3] = 0xff;
    cpu.D[1] = 1;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0150400 | 3 << 9 | 1);
    cpu.X = false;
    cpu.D[3] = 0xc0;
    cpu.D[1] = 1;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Mem)
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0150410 | 3 << 9 | 1);
    RAM[0x1000] = 22;
    RAM[0x1100] = 34;
    cpu.A[3] = 0x1001;
    cpu.A[1] = 0x1101;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.Z = true;
    cpu.V = cpu.C = true;
    cpu.X = true;
    f();
    BOOST_TEST(RAM[0x1000] == 57);
    BOOST_TEST(cpu.A[3] == 0x1000);
    BOOST_TEST(cpu.A[1] == 0x1100);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0150410 | 3 << 9 | 1);
    RAM[0x1000] = 0;
    RAM[0x1100] = 0;
    cpu.A[3] = 0x1001;
    cpu.A[1] = 0x1101;
    cpu.X = false;
    cpu.Z = true;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(CX) {
    TEST::SET_W(0, 0150410 | 3 << 9 | 1);
    RAM[0x1000] = 0xff;
    RAM[0x1100] = 1;
    cpu.A[3] = 0x1001;
    cpu.A[1] = 0x1101;
    cpu.X = false;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(V) {
    TEST::SET_W(0, 0150410 | 3 << 9 | 1);
    RAM[0x1000] = 0x7f;
    RAM[0x1100] = 1;
    cpu.A[3] = 0x1001;
    cpu.A[1] = 0x1101;
    cpu.X = false;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0150410 | 3 << 9 | 1);
    RAM[0x1000] = 0xe0;
    RAM[0x1100] = 1;
    cpu.A[3] = 0x1001;
    cpu.A[1] = 0x1101;
    cpu.X = false;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_SUITE(Reg)
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0150500 | 3 << 9 | 1);
    cpu.D[3] = 2222;
    cpu.D[1] = 2323;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.Z = true;
    cpu.V = cpu.C = true;
    cpu.X = true;
    f();
    BOOST_TEST(cpu.D[3] == 4546);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0150500 | 3 << 9 | 1);
    cpu.D[3] = 0;
    cpu.D[1] = 0;
    cpu.Z = true;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(V) {
    TEST::SET_W(0, 0150500 | 3 << 9 | 1);
    cpu.X = false;
    cpu.D[3] = 0x7fff;
    cpu.D[1] = 1;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(CX) {
    TEST::SET_W(0, 0150500 | 3 << 9 | 1);
    cpu.X = false;
    cpu.D[3] = 0xffff;
    cpu.D[1] = 1;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0150500 | 3 << 9 | 1);
    cpu.X = false;
    cpu.D[3] = 0xc000;
    cpu.D[1] = 1;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Mem)
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0150510 | 3 << 9 | 1);
    TEST::SET_W(0x1000, 2222);
    TEST::SET_W(0x1100, 3434);
    cpu.A[3] = 0x1002;
    cpu.A[1] = 0x1102;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.Z = true;
    cpu.V = cpu.C = true;
    cpu.X = true;
    f();
    BOOST_TEST(TEST::GET_W(0x1000) == 5657);
    BOOST_TEST(cpu.A[3] == 0x1000);
    BOOST_TEST(cpu.A[1] == 0x1100);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0150510 | 3 << 9 | 1);
    TEST::SET_W(0x1000, 0);
    TEST::SET_W(0x1100, 0);
    cpu.A[3] = 0x1002;
    cpu.A[1] = 0x1102;
    cpu.X = false;
    cpu.Z = true;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(CX) {
    TEST::SET_W(0, 0150510 | 3 << 9 | 1);
    TEST::SET_W(0x1000, 0xffff);
    TEST::SET_W(0x1100, 1);
    cpu.A[3] = 0x1002;
    cpu.A[1] = 0x1102;
    cpu.X = false;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(V) {
    TEST::SET_W(0, 0150510 | 3 << 9 | 1);
    TEST::SET_W(0x1000, 0x7fff);
    TEST::SET_W(0x1100, 1);
    cpu.A[3] = 0x1002;
    cpu.A[1] = 0x1102;
    cpu.X = false;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0150510 | 3 << 9 | 1);
    TEST::SET_W(0x1000, 0xa000);
    TEST::SET_W(0x1100, 1);
    cpu.A[3] = 0x1002;
    cpu.A[1] = 0x1102;
    cpu.X = false;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_SUITE(Reg)
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0150600 | 3 << 9 | 1);
    cpu.D[3] = 222222;
    cpu.D[1] = 232323;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.Z = true;
    cpu.V = cpu.C = true;
    cpu.X = true;
    f();
    BOOST_TEST(cpu.D[3] == 454546);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0150600 | 3 << 9 | 1);
    cpu.D[3] = 0;
    cpu.D[1] = 0;
    cpu.Z = true;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(V) {
    TEST::SET_W(0, 0150600 | 3 << 9 | 1);
    cpu.X = false;
    cpu.D[3] = 0x7fffffff;
    cpu.D[1] = 1;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(CX) {
    TEST::SET_W(0, 0150600 | 3 << 9 | 1);
    cpu.X = false;
    cpu.D[3] = 0xffffffff;
    cpu.D[1] = 1;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0150600 | 3 << 9 | 1);
    cpu.X = false;
    cpu.D[3] = 0xc0000000;
    cpu.D[1] = 1;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Mem)
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0150610 | 3 << 9 | 1);
    TEST::SET_L(0x1000, 222222);
    TEST::SET_L(0x1100, 343434);
    cpu.A[3] = 0x1004;
    cpu.A[1] = 0x1104;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.Z = true;
    cpu.V = cpu.C = true;
    cpu.X = true;
    f();
    BOOST_TEST(TEST::GET_L(0x1000) == 565657);
    BOOST_TEST(cpu.A[3] == 0x1000);
    BOOST_TEST(cpu.A[1] == 0x1100);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0150610 | 3 << 9 | 1);
    TEST::SET_L(0x1000, 0);
    TEST::SET_L(0x1100, 0);
    cpu.A[3] = 0x1004;
    cpu.A[1] = 0x1104;
    cpu.X = false;
    cpu.Z = true;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(CX) {
    TEST::SET_W(0, 0150610 | 3 << 9 | 1);
    TEST::SET_L(0x1000, 0xffffffff);
    TEST::SET_L(0x1100, 1);
    cpu.A[3] = 0x1004;
    cpu.A[1] = 0x1104;
    cpu.X = false;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(V) {
    TEST::SET_W(0, 0150610 | 3 << 9 | 1);
    TEST::SET_L(0x1000, 0x7fffffff);
    TEST::SET_L(0x1100, 1);
    cpu.A[3] = 0x1004;
    cpu.A[1] = 0x1104;
    cpu.X = false;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0150610 | 3 << 9 | 1);
    TEST::SET_L(0x1000, 0xa0000000);
    TEST::SET_L(0x1100, 0);
    cpu.A[3] = 0x1004;
    cpu.A[1] = 0x1104;
    cpu.X = false;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
