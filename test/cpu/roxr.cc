#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include "inline.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_ROXR {
    F_ROXR() {
        // ROXR.B #3, %D2
		TEST::SET_W(0, 0160020 | 3 << 9 | 2);
        TEST::SET_W(2, TEST_BREAK);

		// ROXR.B #8, %D2
		TEST::SET_W(4, 0160020 | 0 << 9 | 2);
        TEST::SET_W(6, TEST_BREAK);

		// ROXR.B %D3, %D2
		TEST::SET_W(8, 0160060 | 3 << 9 | 2);
        TEST::SET_W(10, TEST_BREAK);

		// ROXR.W #3, %D2
		TEST::SET_W(12, 0160120 | 3 << 9 | 2);
        TEST::SET_W(14, TEST_BREAK);

		// ROXR.W #8, %D2
		TEST::SET_W(16, 0160120 | 0 << 9 | 2);
        TEST::SET_W(18, TEST_BREAK);

		// ROXR.W %D3, %D2
		TEST::SET_W(20, 0160160 | 3 << 9 | 2);
        TEST::SET_W(22, TEST_BREAK);

		// ROXR.L #3, %D2
		TEST::SET_W(24, 0160220 | 3 << 9 | 2);
        TEST::SET_W(26, TEST_BREAK);

		// ROXR.L #8, %D2
		TEST::SET_W(28, 0160220 | 0 << 9 | 2);
        TEST::SET_W(30, TEST_BREAK);

		// ROXR.L %D3, %D2
		TEST::SET_W(32, 0160260 | 3 << 9 | 2);
        TEST::SET_W(34, TEST_BREAK);

		// ROXR.W (%A2)
		TEST::SET_W(36, 0162320 | 2);
        TEST::SET_W(38, TEST_BREAK);
		jit_compile(0, 40);

    }};
BOOST_FIXTURE_TEST_SUITE(ROXR, Prepare, *boost::unit_test::fixture<F_ROXR>())


BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_SUITE(ByImm)

BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
    cpu.D[2] = 40;
	cpu.X = old_x;
    run_test(0);
    BOOST_TEST(cpu.D[2] == (5 | old_x << 5));
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 2;
    run_test(0);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
    run_test(0);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[2] = 20;
    run_test(0);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(imm8) {
    cpu.D[2] = 0x80;
    run_test(4);
    BOOST_TEST(cpu.D[2] == 0);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(ByReg)
BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
    cpu.D[2] = 40;
	cpu.D[3] = 2;
	cpu.X = old_x;
    run_test(8);
    BOOST_TEST(cpu.D[2] == 10 | old_x << 6);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.X);
}
BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 1;
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
BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[2] = 2;
	cpu.D[3] = 2;
    run_test(8);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
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
    BOOST_TEST(cpu.D[2] == 0);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(scOver8) {
    cpu.D[2] = 0x80;
	cpu.D[3] = 9;
    run_test(8);
    BOOST_TEST(cpu.D[2] == 0x80);
}

BOOST_AUTO_TEST_CASE(scOver32) {
    cpu.D[2] = 0x40;
	cpu.D[3] = 37;
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

BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
    cpu.D[2] = 40;
	cpu.X = old_x;
    run_test(12);
    BOOST_TEST(cpu.D[2] == (5 | old_x << 13));
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 2;
    run_test(12);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
    run_test(12);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[2] = 20;
    run_test(12);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(imm8) {
    cpu.D[2] = 0x80;
    run_test(16);
    BOOST_TEST(cpu.D[2] == 0);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(ByReg)
BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
    cpu.D[2] = 40;
	cpu.D[3] = 2;
	cpu.X = old_x;
    run_test(20);
    BOOST_TEST(cpu.D[2] == 10 | old_x << 14);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.X);
}
BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 1;
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
BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[2] = 2;
	cpu.D[3] = 2;
    run_test(20);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
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
    BOOST_TEST(cpu.D[2] == 0);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(scOver17) {
    cpu.D[2] = 0x8000;
	cpu.D[3] = 17;
    run_test(20);
    BOOST_TEST(cpu.D[2] == 0x8000);
}

BOOST_AUTO_TEST_CASE(scOver32) {
    cpu.D[2] = 0x40;
	cpu.D[3] = 35;
    run_test(20);
    BOOST_TEST(cpu.D[2] == 0x20);
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

BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
    cpu.D[2] = 40;
	cpu.X = old_x;
    run_test(24);
    BOOST_TEST(cpu.D[2] == (5 | old_x << 29));
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 2;
    run_test(24);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
    run_test(24);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[2] = 20;
    run_test(24);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(imm8) {
    cpu.D[2] = 0x80;
    run_test(28);
    BOOST_TEST(cpu.D[2] == 0);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(ByReg)
BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
    cpu.D[2] = 40;
	cpu.D[3] = 2;
	cpu.X = old_x;
    run_test(32);
    BOOST_TEST(cpu.D[2] == 10 | old_x << 30);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.X);
}
BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 1;
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
BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[2] = 2;
	cpu.D[3] = 2;
    run_test(32);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_CASE(sc0) {
	cpu.C = true;
    cpu.D[2] = 2;
	cpu.D[3] = 0;
    run_test(32);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(cpu.D[2] == 2);
}

BOOST_AUTO_TEST_CASE(sc33) {
    cpu.D[2] = 0x80000000;
	cpu.D[3] = 32;
    run_test(32);
    BOOST_TEST(cpu.D[2] == 0);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(scOver32) {
    cpu.D[2] = 0x40000000;
	cpu.D[3] = 34;
    run_test(32);
    BOOST_TEST(cpu.D[2] == 0x20000000);
}

BOOST_AUTO_TEST_CASE(scOver64) {
    cpu.D[2] = 20;
	cpu.D[3] = 65;
    run_test(20);
    BOOST_TEST(cpu.D[2] == 10);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(EA)

BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
	cpu.X = old_x;
    cpu.A[2] = 0x1000;
	TEST::SET_W(0x1000, 2000);
    run_test(36);
    BOOST_TEST(TEST::GET_W(0x1000) == 1000 | old_x << 15);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(cpu.N == old_x);
    BOOST_TEST(!cpu.X);
}


BOOST_AUTO_TEST_CASE(Z) {
    cpu.A[2] = 0x1000;
	TEST::SET_W(0x1000, 0);
    run_test(36);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(CX) {
    cpu.A[2] = 0x1000;
	TEST::SET_W(0x1000, 1);
    run_test(36);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
