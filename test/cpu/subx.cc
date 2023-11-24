#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "inline.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(SUBX, Prepare)
struct F {
    F() {
        // SUBX.B %D3, %D1
        TEST::SET_W(0, 0110400 | 3 << 9 | 1);
        TEST::SET_W(2, TEST_BREAK);

        // SUBX.B -(%A3), -(%A1)
        TEST::SET_W(4, 0110410 | 3 << 9 | 1);
        TEST::SET_W(6, TEST_BREAK);

        // SUBX.W %D3, %D1
        TEST::SET_W(8, 0110500 | 3 << 9 | 1);
        TEST::SET_W(10, TEST_BREAK);

        // SUBX.W -(%A3), -(%A1)
        TEST::SET_W(12, 0110510 | 3 << 9 | 1);
        TEST::SET_W(14, TEST_BREAK);

        // SUBX.L %D3, %D1
        TEST::SET_W(16, 0110600 | 3 << 9 | 1);
        TEST::SET_W(18, TEST_BREAK);

        // SUBX.L -(%A3), -(%A1)
        TEST::SET_W(20, 0110610 | 3 << 9 | 1);
        TEST::SET_W(22, TEST_BREAK);

        jit_compile(0, 24);
    }
};
BOOST_AUTO_TEST_SUITE(R, *boost::unit_test::fixture<F>())
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_SUITE(reg)

BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
    cpu.D[1] = 50;
    cpu.D[3] = 21;
    cpu.Z = true;
    cpu.X = old_x;
    run_test(0);
    BOOST_TEST(cpu.D[1] == 29 - old_x);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.Z = true;
    cpu.D[1] = 10;
    cpu.D[3] = 10;
    cpu.X = false;
    run_test(0);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[1] = -4;
    cpu.D[3] = 3;
    cpu.X = false;
    run_test(0);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(V) {
    cpu.D[1] = 0x80;
    cpu.D[3] = 1;
    cpu.X = false;
    run_test(0);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[1] = 0;
    cpu.D[3] = 1;
    cpu.X = false;
    run_test(0);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Mem)

BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
    RAM[0x100] = 60;
    RAM[0x110] = 34;
    cpu.A[1] = 0x101;
    cpu.A[3] = 0x111;
    cpu.X = old_x;
    run_test(4);
    BOOST_TEST(RAM[0x100] == 26 - old_x);
    BOOST_TEST(cpu.A[1] == 0x100);
    BOOST_TEST(cpu.A[3] == 0x110);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.Z = true;
    cpu.X = false;
    RAM[0x100] = 10;
    RAM[0x110] = 10;
    cpu.A[1] = 0x101;
    cpu.A[3] = 0x111;
    run_test(4);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.X = false;
    RAM[0x100] = -10;
    RAM[0x110] = 5;
    cpu.A[1] = 0x101;
    cpu.A[3] = 0x111;
    run_test(4);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(V) {
    cpu.X = false;
    RAM[0x100] = 0x80;
    RAM[0x110] = 1;
    cpu.A[1] = 0x101;
    cpu.A[3] = 0x111;
    run_test(4);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(CX) {
    cpu.X = false;
    RAM[0x100] = 0;
    RAM[0x110] = 1;
    cpu.A[1] = 0x101;
    cpu.A[3] = 0x111;
    run_test(4);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_SUITE(Reg)
BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
    cpu.D[1] = 4050;
    cpu.D[3] = 2010;
    cpu.Z = true;
    cpu.X = old_x;
    run_test(8);
    BOOST_TEST(cpu.D[1] == 2040 - old_x);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[1] = 700;
    cpu.D[3] = 700;
    cpu.Z = true;
    cpu.X = false;
    run_test(8);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[1] = -1000;
    cpu.D[3] = 200;
    cpu.X = false;
    run_test(8);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(V) {
    cpu.D[1] = 0x8000;
    cpu.D[3] = 1;
    cpu.X = false;
    run_test(8);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(C) {
    cpu.D[1] = 0;
    cpu.D[3] = 1;
    cpu.X = false;
    run_test(8);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Mem)
BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
    TEST::SET_W(0x100, 6789);
    TEST::SET_W(0x110, 3434);
    cpu.A[1] = 0x102;
    cpu.A[3] = 0x112;
    cpu.X = old_x;
    cpu.Z = true;
    run_test(12);
    BOOST_TEST(TEST::GET_W(0x100) == 3355 - old_x);
    BOOST_TEST(cpu.A[1] == 0x100);
    BOOST_TEST(cpu.A[3] == 0x110);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0x100, 700);
    TEST::SET_W(0x110, 700);
    cpu.A[1] = 0x102;
    cpu.A[3] = 0x112;
    cpu.Z = true;
    cpu.X = false;
    run_test(12);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0x100, -700);
    TEST::SET_W(0x110, -200);
    cpu.A[1] = 0x102;
    cpu.A[3] = 0x112;
    cpu.Z = true;
    cpu.X = false;
    run_test(12);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(V) {
    TEST::SET_W(0x100, 0x8000);
    TEST::SET_W(0x110, 1);
    cpu.A[1] = 0x102;
    cpu.A[3] = 0x112;
    cpu.Z = true;
    cpu.X = false;
    run_test(12);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(CX) {
    TEST::SET_W(0x100, 0);
    TEST::SET_W(0x110, 1);
    cpu.A[1] = 0x102;
    cpu.A[3] = 0x112;
    cpu.Z = true;
    cpu.X = false;
    run_test(12);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_SUITE(Reg)
BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
    cpu.Z = true;
    cpu.D[1] = 987654;
    cpu.D[3] = 112233;
    cpu.X = old_x;
    run_test(16);
    BOOST_TEST(cpu.D[1] == 875421 - old_x);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.Z = true;
    cpu.D[1] = 500000;
    cpu.D[3] = 500000;
    cpu.X = false;
    run_test(16);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.Z = true;
    cpu.D[1] = -500000;
    cpu.D[3] = 500000;
    cpu.X = false;
    run_test(16);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(V) {
    cpu.Z = true;
    cpu.D[1] = 0x80000000;
    cpu.D[3] = 1;
    cpu.X = false;
    run_test(16);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(CX) {
    cpu.Z = true;
    cpu.D[1] = 0;
    cpu.D[3] = 1;
    cpu.X = false;
    run_test(16);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Mem)
BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
    cpu.Z = true;
    TEST::SET_L(0x100, 87654321);
    TEST::SET_L(0x110, 11223344);
    cpu.A[1] = 0x104;
    cpu.A[3] = 0x114;
    cpu.X = old_x ;
    run_test(20);
    BOOST_TEST(TEST::GET_L(0x100) == 76430977 - old_x);
    BOOST_TEST(cpu.A[1] == 0x100);
    BOOST_TEST(cpu.A[3] == 0x110);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
