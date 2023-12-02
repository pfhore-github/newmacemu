#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include "inline.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_ADDX {
    F_ADDX() {
        // ADDX.B %D3, %D1
		TEST::SET_W(0, 0150400 | 3 << 9 | 1);
        TEST::SET_W(2, TEST_BREAK);

        // ADDX.B -(%A3), -(%A1)
		TEST::SET_W(4, 0150410 | 3 << 9 | 1);
        TEST::SET_W(6, TEST_BREAK);

        // ADDX.W %D3, %D1
		TEST::SET_W(8, 0150500 | 3 << 9 | 1);
        TEST::SET_W(10, TEST_BREAK);

        // ADDX.W -(%A3), -(%A1)
        TEST::SET_W(12, 0150510 | 3 << 9 | 1);
        TEST::SET_W(14, TEST_BREAK);

        // ADDX.L %D3, %D1
        TEST::SET_W(16, 0150600 | 3 << 9 | 1);
        TEST::SET_W(18, TEST_BREAK);

        // ADDX.L -(%A3), -(%A1)
        TEST::SET_W(20, 0150610 | 3 << 9 | 1);
        TEST::SET_W(22, TEST_BREAK);

        jit_compile(0, 24);
    }};
BOOST_FIXTURE_TEST_SUITE(ADDX, Prepare, *boost::unit_test::fixture<F_ADDX>())


BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_SUITE(reg)
BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
    cpu.Z = true;
	cpu.X = old_x;
    cpu.D[3] = 22;
    cpu.D[1] = 23;
    run_test(0);
    BOOST_TEST(cpu.D[3] == 45 + old_x);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.Z = true;
	cpu.X = false;
    cpu.D[3] = 0;
    cpu.D[1] = 0;
    run_test(0);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(N) {
	cpu.X = false;
    cpu.D[3] = -5;
    cpu.D[1] = 1;
    run_test(0);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(V) {
	cpu.X = false;
    cpu.D[3] = 0x7f;
    cpu.D[1] = 1;
    run_test(0);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(CX) {
	cpu.X = false;
    cpu.D[3] = 0xff;
    cpu.D[1] = 1;
    run_test(0);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Mem)
BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
	cpu.Z = true;
    RAM[0x1000] = 22;
    RAM[0x1010] = 34;
    cpu.A[3] = 0x1001;
    cpu.A[1] = 0x1011;
    cpu.X = old_x;
    run_test(4);
    BOOST_TEST(RAM[0x1000] == 56 + old_x);
    BOOST_TEST(cpu.A[3] == 0x1000);
    BOOST_TEST(cpu.A[1] == 0x1010);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
	
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_SUITE(reg)
BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
    cpu.Z = true;
    cpu.D[3] = 2222;
    cpu.D[1] = 2323;
    cpu.X = old_x;
    run_test(8);
    BOOST_TEST(cpu.D[3] == 4545 + old_x);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.Z = true;
    cpu.D[3] = 0;
    cpu.D[1] = 0;
	cpu.X = false;
    run_test(8);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[3] = -4;
    cpu.D[1] = 1;
	cpu.X = false;
    run_test(8);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(V) {
    cpu.D[3] = 0x7fff;
    cpu.D[1] = 1;
	cpu.X = false;
    run_test(8);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[3] = 0xffff;
    cpu.D[1] = 1;
	cpu.X = false;
    run_test(8);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Mem)
BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
    TEST::SET_W(0x1000, 2222);
    TEST::SET_W(0x1010, 3434);
    cpu.A[3] = 0x1002;
    cpu.A[1] = 0x1012;
    cpu.X = old_x;
    run_test(12);
    BOOST_TEST(TEST::GET_W(0x1000) == 5656+old_x);
    BOOST_TEST(cpu.A[3] == 0x1000);
    BOOST_TEST(cpu.A[1] == 0x1010);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(Z) {
	cpu.Z = true;
    TEST::SET_W(0x1000, 0);
    TEST::SET_W(0x1010, 0);
    cpu.A[3] = 0x1002;
    cpu.A[1] = 0x1012;
    cpu.X = false;
    run_test(12);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0x1000, -4);
    TEST::SET_W(0x1010, 1);
    cpu.A[3] = 0x1002;
    cpu.A[1] = 0x1012;
    cpu.X = false;
    run_test(12);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(V) {
    TEST::SET_W(0x1000, 0x7fff);
    TEST::SET_W(0x1010, 1);
    cpu.A[3] = 0x1002;
    cpu.A[1] = 0x1012;
    cpu.X = false;
    run_test(12);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(CX) {
    TEST::SET_W(0x1000, 0xffff);
    TEST::SET_W(0x1010, 1);
    cpu.A[3] = 0x1002;
    cpu.A[1] = 0x1012;
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
    cpu.D[3] = 222222;
    cpu.D[1] = 232323;
    cpu.X = old_x;
    run_test(16);
    BOOST_TEST(cpu.D[3] == 454545 + old_x);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.Z = true;
    cpu.D[3] = 0;
    cpu.D[1] = 0;
    cpu.X = false;
    run_test(16);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[3] = -5;
    cpu.D[1] = 1;
    cpu.X = false;
    run_test(16);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(V) {
    cpu.D[3] = 0x7fffffff;
    cpu.D[1] = 1;
    cpu.X = false;
    run_test(16);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[3] = 0xffffffff;
    cpu.D[1] = 1;
    cpu.X = false;
    run_test(16);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Mem)


BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
	cpu.Z = true;
    TEST::SET_L(0x1000, 222222);
    TEST::SET_L(0x1010, 343434);
    cpu.A[3] = 0x1004;
    cpu.A[1] = 0x1014;
    cpu.X = old_x;
    run_test(20);
    BOOST_TEST(TEST::GET_L(0x1000) == 565656 + old_x);
    BOOST_TEST(cpu.A[3] == 0x1000);
    BOOST_TEST(cpu.A[1] == 0x1010);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(Z) {
	cpu.Z = true;
    TEST::SET_L(0x1000, 0);
    TEST::SET_L(0x1010, 0);
    cpu.A[3] = 0x1004;
    cpu.A[1] = 0x1014;
    cpu.X = false;
    run_test(20);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_L(0x1000, -4);
    TEST::SET_L(0x1010, 1);
    cpu.A[3] = 0x1004;
    cpu.A[1] = 0x1014;
    cpu.X = false;
    run_test(20);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(V) {
    TEST::SET_L(0x1000, 0x7fffffff);
    TEST::SET_L(0x1010, 1);
    cpu.A[3] = 0x1004;
    cpu.A[1] = 0x1014;
    cpu.X = false;
    run_test(20);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(CX) {
    TEST::SET_L(0x1000, 0xffffffff);
    TEST::SET_L(0x1010, 1);
    cpu.A[3] = 0x1004;
    cpu.A[1] = 0x1014;
    cpu.X = false;
    run_test(20);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
