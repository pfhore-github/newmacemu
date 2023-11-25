#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "inline.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_BFFFO {
    F_BFFFO() {
        // BFFFO %D2{8, 4}, %D6
		TEST::SET_W(0, 0166700 |  2 );
		TEST::SET_W(2, 6 << 12 |8 << 6 | 4 );
        TEST::SET_W(4, TEST_BREAK);

        // BFFFO %D2{%D3, 4}, %D6
		TEST::SET_W(6, 0166700|  2 );
		TEST::SET_W(8, 6 << 12 |1 << 11 | 3 << 6 | 4 );
        TEST::SET_W(10, TEST_BREAK);

        // BFFFO %D2{8, %D3}, %D6
		TEST::SET_W(12, 0166700|  2 );
		TEST::SET_W(14, 6 << 12 |1 << 5 | 8 << 6 | 3 );
        TEST::SET_W(16, TEST_BREAK);

        // BFFFO %D2{%D3, %D5}, %D6
		TEST::SET_W(18, 0166700|  2 );
		TEST::SET_W(20, 6 << 12 |1 << 5 | 1 << 11 | 3 << 6 | 5 );
        TEST::SET_W(22, TEST_BREAK);

        // BFFFO (%A2){8, 4}, %D6
		TEST::SET_W(24, 0166720 | 2);
		TEST::SET_W(26, 6 << 12 |8 << 6 | 4);
        TEST::SET_W(28, TEST_BREAK);
	
        // BFFFO (%A2){%D3, 4}, %D6
		TEST::SET_W(30, 0166720 | 2);
		TEST::SET_W(32, 6 << 12 |1 << 11 | 3 << 6 | 4);
        TEST::SET_W(34, TEST_BREAK);

        // BFFFO (%A2){8, %D3}, %D6
		TEST::SET_W(36, 0166720 | 2);
		TEST::SET_W(38, 6 << 12 |1 << 5 | 8 << 6 | 3);
        TEST::SET_W(40, TEST_BREAK);

        // BFFFO (%A2){%D3, %D5}, %D6
		TEST::SET_W(42, 0166720 | 2);
		TEST::SET_W(44, 6 << 12 |1 << 5 | 1 << 11 | 3 << 6 | 5);
        TEST::SET_W(46, TEST_BREAK);
        jit_compile(0, 48);
    }};
BOOST_FIXTURE_TEST_SUITE(BFFFO, Prepare, *boost::unit_test::fixture<F_BFFFO>())




BOOST_AUTO_TEST_SUITE(D)
BOOST_AUTO_TEST_CASE(both_imm) {
    cpu.D[2] = 0x12345678;
    cpu.V = cpu.C = true;
    run_test(0);
    BOOST_TEST(cpu.D[6] == 10);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_CASE(off_r) {
    cpu.D[2] = 0x12345678;
    cpu.D[3] = 8;
    run_test(6);
    BOOST_TEST(cpu.D[6] == 10);
}

BOOST_AUTO_TEST_CASE(width_r) {
    cpu.D[2] = 0x12345678;
    cpu.D[3] = 4;
    run_test(12);
    BOOST_TEST(cpu.D[6] == 10);
}

BOOST_AUTO_TEST_CASE(both_r) {
    cpu.D[2] = 0x12345678;
    cpu.D[3] = 8;
    cpu.D[5] = 4;
    run_test(18);
    BOOST_TEST(cpu.D[6] == 10);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0x12005678;
    cpu.D[3] = 8;
    cpu.D[5] = 8;
    run_test(18);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(cpu.D[6] == 16);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 0x12805678;
    cpu.D[3] = 8;
    cpu.D[5] = 8;
    run_test(18);
    BOOST_TEST(cpu.N);
    BOOST_TEST(cpu.D[6] == 8);
}

BOOST_AUTO_TEST_CASE(overwrap) {
    cpu.D[2] = 0x02345670;
    cpu.D[3] = 28;
    cpu.D[5] = 8;
    run_test(18);
    BOOST_TEST(cpu.D[6] == 36);
}

BOOST_AUTO_TEST_CASE(offset_negative) {
    cpu.D[2] = 0x12345678;
    cpu.D[3] = -4;
    cpu.D[5] = 8;
    run_test(18);
    BOOST_TEST(cpu.D[6] == -4);
}

BOOST_AUTO_TEST_CASE(size_zero) {
    cpu.D[2] = 0;
    cpu.D[3] = -4;
    cpu.D[5] = 0;
    run_test(18);
    BOOST_TEST(cpu.D[6] == 28);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(A)
BOOST_AUTO_TEST_CASE(both_imm) {
    TEST::SET_L(0x100, 0x12345678);
    cpu.A[2] = 0x100;
    run_test(24);
    BOOST_TEST(cpu.D[6] == 10);
}


BOOST_AUTO_TEST_CASE(off_r) {
    cpu.A[2] = 0x100;
    TEST::SET_L(0x100, 0x12345678);
    cpu.D[3] = 8;
    run_test(30);
    BOOST_TEST(cpu.D[6] == 10);
}

BOOST_AUTO_TEST_CASE(width_r) {
    cpu.A[2] = 0x100;
    TEST::SET_L(0x100, 0x12345678);
    cpu.D[3] = 4;
    run_test(36);
    BOOST_TEST(cpu.D[6] == 10);
}

BOOST_AUTO_TEST_CASE(both_r) {
    cpu.A[2] = 0x100;
    TEST::SET_L(0x100, 0x12345678);
    cpu.D[3] = 8;
    cpu.D[5] = 4;
    run_test(42);
    BOOST_TEST(cpu.D[6] == 10);
}

BOOST_AUTO_TEST_CASE(Z) {
	cpu.A[2] = 0x100;
    RAM[0x100] = 0x10;
    cpu.D[3] = 4;
    cpu.D[5] = 4;
    run_test(42);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(cpu.D[6] == 8);
}

BOOST_AUTO_TEST_CASE(N) {
    RAM[0x100] = 0x10;
    cpu.D[2] = 0x1F;
    cpu.D[3] = 4;
    cpu.D[5] = 4;
    run_test(42);
    BOOST_TEST(cpu.N);
    BOOST_TEST(cpu.D[6] == 4);
}

BOOST_AUTO_TEST_CASE(byte1) {
	cpu.A[2] = 0x100;
    RAM[0x100] = 0x12;
    cpu.D[3] = 4;
    cpu.D[5] = 4;
    run_test(42);
    BOOST_TEST(cpu.D[6] == 6);
}

BOOST_AUTO_TEST_CASE(byte2) {
    RAM[0x100] = 0x10;
    RAM[0x101] = 0x14;
	cpu.A[2] = 0x100;
    cpu.D[3] = 4;
    cpu.D[5] = 8;
    run_test(42);
    BOOST_TEST(cpu.D[6] == 11);
}

BOOST_AUTO_TEST_CASE(byte3) {
    RAM[0x100] = 0x10;
    RAM[0x101] = 0x00;
    RAM[0x102] = 0x16;
	cpu.A[2] = 0x100;
    cpu.D[3] = 4;
    cpu.D[5] = 16;
    run_test(42);
    BOOST_TEST(cpu.D[6] == 19);
}

BOOST_AUTO_TEST_CASE(byte4) {
    RAM[0x100] = 0x10;
    RAM[0x101] = 0x00;
    RAM[0x102] = 0x00;
    RAM[0x103] = 0x18;
	cpu.A[2] = 0x100;
    cpu.D[3] = 4;
    cpu.D[5] = 24;
    run_test(42);
    BOOST_TEST(cpu.D[6] == 27);
}

BOOST_AUTO_TEST_CASE(byte5) {
    RAM[0x100] = 0x10;
    RAM[0x101] = 0x00;
    RAM[0x102] = 0x00;
    RAM[0x103] = 0x00;
    RAM[0x104] = 0x1A;
	cpu.A[2] = 0x100;
    cpu.D[3] = 4;
    cpu.D[5] = 0;
    run_test(42);
    BOOST_TEST(cpu.D[6] == 35);
}

BOOST_AUTO_TEST_CASE(offset_neg) {
    TEST::SET_L(0x100, 0x12345678);
	cpu.A[2] = 0x104;
    cpu.D[3] = -12;
    cpu.D[5] = 4;
    run_test(42);
    BOOST_TEST(cpu.D[6] == -11);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
