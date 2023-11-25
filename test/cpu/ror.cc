#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include "inline.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_ROR {
    F_ROR() {
        // ROR.B #3, %D2
		TEST::SET_W(0, 0160030 | 3 << 9 | 2);
        TEST::SET_W(2, TEST_BREAK);

		// ROR.B #8, %D2
		TEST::SET_W(4, 0160030 | 0 << 9 | 2);
        TEST::SET_W(6, TEST_BREAK);

		// ROR.B %D3, %D2
		TEST::SET_W(8, 0160070 | 3 << 9 | 2);
        TEST::SET_W(10, TEST_BREAK);

		// ROR.W #3, %D2
		TEST::SET_W(12, 0160130 | 3 << 9 | 2);
        TEST::SET_W(14, TEST_BREAK);

		// ROR.W #8, %D2
		TEST::SET_W(16, 0160130 | 0 << 9 | 2);
        TEST::SET_W(18, TEST_BREAK);

		// ROR.W %D3, %D2
		TEST::SET_W(20, 0160170 | 3 << 9 | 2);
        TEST::SET_W(22, TEST_BREAK);

		// ROR.L #3, %D2
		TEST::SET_W(24, 0160230 | 3 << 9 | 2);
        TEST::SET_W(26, TEST_BREAK);

		// ROR.L #8, %D2
		TEST::SET_W(28, 0160230 | 0 << 9 | 2);
        TEST::SET_W(30, TEST_BREAK);

		// ROR.L %D3, %D2
		TEST::SET_W(32, 0160270 | 3 << 9 | 2);
        TEST::SET_W(34, TEST_BREAK);

		// ROR.W (%A2)
		TEST::SET_W(36, 0163320 | 2);
        TEST::SET_W(38, TEST_BREAK);
		jit_compile(0, 40);
    }};
BOOST_FIXTURE_TEST_SUITE(ROR, Prepare, *boost::unit_test::fixture<F_ROR>())


BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_SUITE(ByImm)
BOOST_AUTO_TEST_CASE(value) {
    cpu.D[2] = 2;
    run_test(0);
    BOOST_TEST(cpu.D[2] == 0x40);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 4;
    run_test(0);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
    run_test(0);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(C) {
    cpu.D[2] = 20;
    run_test(0);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(imm8) {
    cpu.D[2] = 0x80;
    run_test(4);
    BOOST_TEST(cpu.D[2] == 0x80);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(ByReg)
BOOST_AUTO_TEST_CASE(value) {
    cpu.D[2] = 1;
	cpu.D[3] = 2;
    run_test(8);
    BOOST_TEST(cpu.D[2] == 0x40);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.X);
}
BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 2;
	cpu.D[3] = 2;
    run_test(8);
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
	cpu.D[3] = 2;
    run_test(8);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(C) {
    cpu.D[2] = 2;
	cpu.D[3] = 2;
    run_test(8);
    BOOST_TEST(cpu.C);
}
BOOST_AUTO_TEST_CASE(sc0) {
	cpu.C = true;
    cpu.D[2] = 2;
	cpu.D[3] = 0;
    run_test(8);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(cpu.D[2] == 2);
}

BOOST_AUTO_TEST_CASE(sc8) {
    cpu.D[2] = 0x80;
	cpu.D[3] = 8;
    run_test(8);
    BOOST_TEST(cpu.D[2] == 0x80);
}

BOOST_AUTO_TEST_CASE(scOver8) {
    cpu.D[2] = 0x80;
	cpu.D[3] = 9;
    run_test(8);
    BOOST_TEST(cpu.D[2] == 0x40);
}

BOOST_AUTO_TEST_CASE(scOver32) {
    cpu.D[2] = 0x40;
	cpu.D[3] = 33;
    run_test(8);
    BOOST_TEST(cpu.D[2] == 0x20);
}

BOOST_AUTO_TEST_CASE(scOver64) {
    cpu.D[2] = 20;
	cpu.D[3] = 65;
    run_test(8);
    BOOST_TEST(cpu.D[2] == 10);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_SUITE(ByImm)
BOOST_AUTO_TEST_CASE(value) {
    cpu.D[2] = 2;
    run_test(12);
    BOOST_TEST(cpu.D[2] == 0x4000);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 4;
    run_test(12);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
    run_test(12);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(C) {
    cpu.D[2] = 20;
    run_test(12);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(imm8) {
    cpu.D[2] = 0x80;
    run_test(16);
    BOOST_TEST(cpu.D[2] == 0x8000);
	BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(ByReg)
BOOST_AUTO_TEST_CASE(value) {
    cpu.D[2] = 1;
	cpu.D[3] = 2;
    run_test(20);
    BOOST_TEST(cpu.D[2] == 0x4000);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.X);
}
BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 2;
	cpu.D[3] = 2;
    run_test(20);
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
	cpu.D[3] = 2;
    run_test(20);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(C) {
    cpu.D[2] = 2;
	cpu.D[3] = 2;
    run_test(20);
    BOOST_TEST(cpu.C);
}
BOOST_AUTO_TEST_CASE(sc0) {
	cpu.C = true;
    cpu.D[2] = 2;
	cpu.D[3] = 0;
    run_test(20);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(cpu.D[2] == 2);
}

BOOST_AUTO_TEST_CASE(sc16) {
    cpu.D[2] = 0x8000;
	cpu.D[3] = 16;
    run_test(20);
    BOOST_TEST(cpu.D[2] == 0x8000);
}

BOOST_AUTO_TEST_CASE(scOver16) {
    cpu.D[2] = 0x8000;
	cpu.D[3] = 17;
    run_test(20);
    BOOST_TEST(cpu.D[2] == 0x4000);
}

BOOST_AUTO_TEST_CASE(scOver32) {
    cpu.D[2] = 0x4000;
	cpu.D[3] = 33;
    run_test(20);
    BOOST_TEST(cpu.D[2] == 0x2000);
}

BOOST_AUTO_TEST_CASE(scOver64) {
    cpu.D[2] = 20;
	cpu.D[3] = 65;
    run_test(20);
    BOOST_TEST(cpu.D[2] == 10);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_SUITE(ByImm)
BOOST_AUTO_TEST_CASE(value) {
    cpu.D[2] = 2;
    run_test(24);
    BOOST_TEST(cpu.D[2] == 0x40000000);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 4;
    run_test(24);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
    run_test(24);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(C) {
    cpu.D[2] = 20;
    run_test(24);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(imm8) {
    cpu.D[2] = 0x80;
    run_test(28);
    BOOST_TEST(cpu.D[2] == 0x80000000);
	BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(ByReg)
BOOST_AUTO_TEST_CASE(value) {
    cpu.D[2] = 1;
	cpu.D[3] = 2;
    run_test(32);
    BOOST_TEST(cpu.D[2] == 0x40000000);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.X);
}
BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 2;
	cpu.D[3] = 2;
    run_test(32);
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
	cpu.D[3] = 2;
    run_test(32);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(C) {
    cpu.D[2] = 2;
	cpu.D[3] = 2;
    run_test(32);
    BOOST_TEST(cpu.C);
}
BOOST_AUTO_TEST_CASE(sc0) {
	cpu.C = true;
    cpu.D[2] = 2;
	cpu.D[3] = 0;
    run_test(32);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(cpu.D[2] == 2);
}

BOOST_AUTO_TEST_CASE(sc32) {
    cpu.D[2] = 0x80000000;
	cpu.D[3] = 32;
    run_test(32);
    BOOST_TEST(cpu.D[2] == 0x80000000);
}

BOOST_AUTO_TEST_CASE(scOver32) {
    cpu.D[2] = 0x80000000;
	cpu.D[3] = 33;
    run_test(32);
    BOOST_TEST(cpu.D[2] == 0x40000000);
}


BOOST_AUTO_TEST_CASE(scOver64) {
    cpu.D[2] = 20;
	cpu.D[3] = 65;
    run_test(32);
    BOOST_TEST(cpu.D[2] == 10);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(EA)

BOOST_AUTO_TEST_CASE(value) {
    cpu.A[2] = 0x100;
	TEST::SET_W(0x100, 0x0400);
    run_test(36);
    BOOST_TEST(TEST::GET_W(0x100) == 0x200);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.A[2] = 0x100;
	TEST::SET_W(0x100, 1);
    run_test(36);
    BOOST_TEST(TEST::GET_W(0x100) == 0x8000);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.A[2] = 0x100;
	TEST::SET_W(0x100, 0);
    run_test(36);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(C) {
    cpu.A[2] = 0x100;
	TEST::SET_W(0x100, 1);
    run_test(36);
    BOOST_TEST(cpu.C);
}


BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
