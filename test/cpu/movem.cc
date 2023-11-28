#define BOOST_TEST_DYN_LINK
#include "68040.hpp"

#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_MOVEM {
    F_MOVEM() {
        // MOVEM.W %D0-%D3/%A0-%A3, (%A5)
        TEST::SET_W(0, 0044220 | 5);
        TEST::SET_W(2, 0b0000111100001111);
        TEST::SET_W(6, TEST_BREAK);

        // MOVEM.W %D0-%D3/%A0-%A3, (-%A5)
        TEST::SET_W(8, 0044240 | 5);
        TEST::SET_W(10, 0b1111000011110000);
        TEST::SET_W(12, TEST_BREAK);

        // MOVEM.L %D0-%D3/%A0-%A3, (%A5)
        TEST::SET_W(14, 0044320 | 5);
        TEST::SET_W(16, 0b0000111100001111);
        TEST::SET_W(18, TEST_BREAK);

        // MOVEM.L %D0-%D3/%A0-%A3, (-%A5)
        TEST::SET_W(20, 0044340 | 5);
        TEST::SET_W(22, 0b1111000011110000);
        TEST::SET_W(24, TEST_BREAK);

        // MOVEM.W (%A5), %D0-%D3/%A0-%A3
        TEST::SET_W(26, 0046220 | 5);
        TEST::SET_W(28, 0b0000111100001111);
        TEST::SET_W(30, TEST_BREAK);

        // MOVEM.W (%A5+), %D0-%D3/%A0-%A3
        TEST::SET_W(32, 0046230 | 5);
        TEST::SET_W(34, 0b0000111100001111);
        TEST::SET_W(36, TEST_BREAK);

        // MOVEM.L (%A5), %D0-%D3/%A0-%A3
        TEST::SET_W(38, 0046320 | 5);
        TEST::SET_W(40, 0b0000111100001111);
        TEST::SET_W(42, TEST_BREAK);

        // MOVEM.L (%A5+), %D0-%D3/%A0-%A3
        TEST::SET_W(44, 0046330 | 5);
        TEST::SET_W(46, 0b0000111100001111);
        TEST::SET_W(48, TEST_BREAK);

        jit_compile(0, 50);
    }};
BOOST_FIXTURE_TEST_SUITE(MOVEM, Prepare, *boost::unit_test::fixture<F_MOVEM>())


BOOST_AUTO_TEST_SUITE(Store)
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_SUITE(base)
BOOST_AUTO_TEST_CASE(ok) {
    cpu.D[0] = 0x0001;
    cpu.D[1] = 0x0002;
    cpu.D[2] = 0x0003;
    cpu.D[3] = 0x0004;
    cpu.A[0] = 0x1000;
    cpu.A[1] = 0x1001;
    cpu.A[2] = 0x1002;
    cpu.A[3] = 0x1003;
    cpu.A[5] = 0x200;
    run_test(0);
    BOOST_TEST(cpu.A[5] == 0x200);
    BOOST_TEST(TEST::GET_W(0x200) == 0x0001);
    BOOST_TEST(TEST::GET_W(0x202) == 0x0002);
    BOOST_TEST(TEST::GET_W(0x204) == 0x0003);
    BOOST_TEST(TEST::GET_W(0x206) == 0x0004);
    BOOST_TEST(TEST::GET_W(0x208) == 0x1000);
    BOOST_TEST(TEST::GET_W(0x20A) == 0x1001);
    BOOST_TEST(TEST::GET_W(0x20C) == 0x1002);
    BOOST_TEST(TEST::GET_W(0x20E) == 0x1003);
}

BOOST_AUTO_TEST_CASE(fail) {
    cpu.A[5] = 0xFFE;
	run_test(0);
	BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::AFAULT);
	BOOST_TEST(cpu.fault_SSW & SSW_CM);
	BOOST_TEST(cpu.ex_addr == 0x1000);
    BOOST_TEST(cpu.EA == 0xFFE);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(decr)
BOOST_AUTO_TEST_CASE(ok) {
    cpu.D[0] = 0x0001;
    cpu.D[1] = 0x0002;
    cpu.D[2] = 0x0003;
    cpu.D[3] = 0x0004;
    cpu.A[0] = 0x1000;
    cpu.A[1] = 0x1001;
    cpu.A[2] = 0x1002;
    cpu.A[3] = 0x1003;
    cpu.A[5] = 0x210;
    run_test(8);
    BOOST_TEST(TEST::GET_W(0x200) == 0x0001);
    BOOST_TEST(TEST::GET_W(0x202) == 0x0002);
    BOOST_TEST(TEST::GET_W(0x204) == 0x0003);
    BOOST_TEST(TEST::GET_W(0x206) == 0x0004);
    BOOST_TEST(TEST::GET_W(0x208) == 0x1000);
    BOOST_TEST(TEST::GET_W(0x20A) == 0x1001);
    BOOST_TEST(TEST::GET_W(0x20C) == 0x1002);
    BOOST_TEST(TEST::GET_W(0x20E) == 0x1003);
    BOOST_TEST(cpu.A[5] == 0x200);
}
BOOST_AUTO_TEST_CASE(fail) {
    cpu.A[5] = 0x4;
	run_test(8);
	BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::AFAULT);
	BOOST_TEST(cpu.fault_SSW & SSW_CM);
	BOOST_TEST(cpu.ex_addr == 0xfffffffe);
    BOOST_TEST(cpu.EA == 2);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_SUITE(base)

BOOST_AUTO_TEST_CASE(ok) {
    cpu.D[0] = 0x00000001;
    cpu.D[1] = 0x00000002;
    cpu.D[2] = 0x00000003;
    cpu.D[3] = 0x00000004;
    cpu.A[0] = 0x10000000;
    cpu.A[1] = 0x10000001;
    cpu.A[2] = 0x10000002;
    cpu.A[3] = 0x10000003;
    cpu.A[5] = 0x200;
    run_test(14);
    BOOST_TEST(TEST::GET_L(0x200) == 0x00000001);
    BOOST_TEST(TEST::GET_L(0x204) == 0x00000002);
    BOOST_TEST(TEST::GET_L(0x208) == 0x00000003);
    BOOST_TEST(TEST::GET_L(0x20C) == 0x00000004);
    BOOST_TEST(TEST::GET_L(0x210) == 0x10000000);
    BOOST_TEST(TEST::GET_L(0x214) == 0x10000001);
    BOOST_TEST(TEST::GET_L(0x218) == 0x10000002);
    BOOST_TEST(TEST::GET_L(0x21C) == 0x10000003);
    BOOST_TEST(cpu.A[5] == 0x200);
}
BOOST_AUTO_TEST_CASE(fail) {
    cpu.A[5] = 0xFFC;
	run_test(14);
	BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::AFAULT);
	BOOST_TEST(cpu.fault_SSW & SSW_CM);
	BOOST_TEST(cpu.ex_addr == 0x1000);
    BOOST_TEST(cpu.EA == 0xFFC);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(decr)

BOOST_AUTO_TEST_CASE(ok) {
    cpu.D[0] = 0x00000001;
    cpu.D[1] = 0x00000002;
    cpu.D[2] = 0x00000003;
    cpu.D[3] = 0x00000004;
    cpu.A[0] = 0x10000000;
    cpu.A[1] = 0x10000001;
    cpu.A[2] = 0x10000002;
    cpu.A[3] = 0x10000003;
    cpu.A[5] = 0x220;
    run_test(20);
    BOOST_TEST(TEST::GET_L(0x200) == 0x00000001);
    BOOST_TEST(TEST::GET_L(0x204) == 0x00000002);
    BOOST_TEST(TEST::GET_L(0x208) == 0x00000003);
    BOOST_TEST(TEST::GET_L(0x20C) == 0x00000004);
    BOOST_TEST(TEST::GET_L(0x210) == 0x10000000);
    BOOST_TEST(TEST::GET_L(0x214) == 0x10000001);
    BOOST_TEST(TEST::GET_L(0x218) == 0x10000002);
    BOOST_TEST(TEST::GET_L(0x21C) == 0x10000003);
    BOOST_TEST(cpu.A[5] == 0x200);
}
BOOST_AUTO_TEST_CASE(fail) {
    cpu.A[5] = 0x4;
	run_test(20);
	BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::AFAULT);
	BOOST_TEST(cpu.fault_SSW & SSW_CM);
	BOOST_TEST(cpu.ex_addr == 0xfffffffc);
    BOOST_TEST(cpu.EA == 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Load)
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_SUITE(addr)

BOOST_AUTO_TEST_CASE(ok) {

    TEST::SET_W(0x200, 0x0001);
    TEST::SET_W(0x202, 0x0002);
    TEST::SET_W(0x204, 0x0003);
    TEST::SET_W(0x206, 0x0004);
    TEST::SET_W(0x208, 0x1000);
    TEST::SET_W(0x20A, 0x1001);
    TEST::SET_W(0x20C, 0x1002);
    TEST::SET_W(0x20E, 0x1003);
    cpu.A[5] = 0x200;
    run_test(26);
    BOOST_TEST(cpu.A[5] == 0x200);
    BOOST_TEST(cpu.D[0] == 0x0001);
    BOOST_TEST(cpu.D[1] == 0x0002);
    BOOST_TEST(cpu.D[2] == 0x0003);
    BOOST_TEST(cpu.D[3] == 0x0004);
    BOOST_TEST(cpu.A[0] == 0x1000);
    BOOST_TEST(cpu.A[1] == 0x1001);
    BOOST_TEST(cpu.A[2] == 0x1002);
    BOOST_TEST(cpu.A[3] == 0x1003);
}
BOOST_AUTO_TEST_CASE(fail) {
    cpu.A[5] = 0x2ffc;
	run_test(26);
	BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::AFAULT);
	BOOST_TEST(cpu.fault_SSW & SSW_CM);
	BOOST_TEST(cpu.ex_addr == 0x3000);
    BOOST_TEST(cpu.EA == 0x2ffc);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(incr)
BOOST_AUTO_TEST_CASE(ok) {

    TEST::SET_W(0x200, 0x0001);
    TEST::SET_W(0x202, 0x0002);
    TEST::SET_W(0x204, 0x0003);
    TEST::SET_W(0x206, 0x0004);
    TEST::SET_W(0x208, 0x1000);
    TEST::SET_W(0x20A, 0x1001);
    TEST::SET_W(0x20C, 0x1002);
    TEST::SET_W(0x20E, 0x1003);
    cpu.A[5] = 0x200;
    run_test(32);
    BOOST_TEST(cpu.A[5] == 0x210);
    BOOST_TEST(cpu.D[0] == 0x0001);
    BOOST_TEST(cpu.D[1] == 0x0002);
    BOOST_TEST(cpu.D[2] == 0x0003);
    BOOST_TEST(cpu.D[3] == 0x0004);
    BOOST_TEST(cpu.A[0] == 0x1000);
    BOOST_TEST(cpu.A[1] == 0x1001);
    BOOST_TEST(cpu.A[2] == 0x1002);
    BOOST_TEST(cpu.A[3] == 0x1003);
}
BOOST_AUTO_TEST_CASE(fail) {
    cpu.A[5] = 0x2ffc;
	run_test(32);
	BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::AFAULT);
	BOOST_TEST(cpu.fault_SSW & SSW_CM);
	BOOST_TEST(cpu.ex_addr == 0x3000);
    BOOST_TEST(cpu.EA == 0x2ffc);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_SUITE(addr)

BOOST_AUTO_TEST_CASE(ok) {

    TEST::SET_L(0x200, 0x00000001);
    TEST::SET_L(0x204, 0x00000002);
    TEST::SET_L(0x208, 0x00000003);
    TEST::SET_L(0x20C, 0x00000004);
    TEST::SET_L(0x210, 0x10000000);
    TEST::SET_L(0x214, 0x10000001);
    TEST::SET_L(0x218, 0x10000002);
    TEST::SET_L(0x21C, 0x10000003);
    cpu.A[5] = 0x200;
    run_test(38);
    BOOST_TEST(cpu.A[5] == 0x200);
    BOOST_TEST(cpu.D[0] == 0x00000001);
    BOOST_TEST(cpu.D[1] == 0x00000002);
    BOOST_TEST(cpu.D[2] == 0x00000003);
    BOOST_TEST(cpu.D[3] == 0x00000004);
    BOOST_TEST(cpu.A[0] == 0x10000000);
    BOOST_TEST(cpu.A[1] == 0x10000001);
    BOOST_TEST(cpu.A[2] == 0x10000002);
    BOOST_TEST(cpu.A[3] == 0x10000003);
}
BOOST_AUTO_TEST_CASE(fail) {
    cpu.A[5] = 0x2ffc;
	run_test(38);
	BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::AFAULT);
	BOOST_TEST(cpu.fault_SSW & SSW_CM);
	BOOST_TEST(cpu.ex_addr == 0x3000);
    BOOST_TEST(cpu.EA == 0x2ffc);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(incr)
BOOST_AUTO_TEST_CASE(ok) {

    TEST::SET_L(0x200, 0x00000001);
    TEST::SET_L(0x204, 0x00000002);
    TEST::SET_L(0x208, 0x00000003);
    TEST::SET_L(0x20C, 0x00000004);
    TEST::SET_L(0x210, 0x10000000);
    TEST::SET_L(0x214, 0x10000001);
    TEST::SET_L(0x218, 0x10000002);
    TEST::SET_L(0x21C, 0x10000003);
    cpu.A[5] = 0x200;
    run_test(44);
    BOOST_TEST(cpu.A[5] == 0x220);
    BOOST_TEST(cpu.D[0] == 0x00000001);
    BOOST_TEST(cpu.D[1] == 0x00000002);
    BOOST_TEST(cpu.D[2] == 0x00000003);
    BOOST_TEST(cpu.D[3] == 0x00000004);
    BOOST_TEST(cpu.A[0] == 0x10000000);
    BOOST_TEST(cpu.A[1] == 0x10000001);
    BOOST_TEST(cpu.A[2] == 0x10000002);
    BOOST_TEST(cpu.A[3] == 0x10000003);
}
BOOST_AUTO_TEST_CASE(fail) {
    cpu.A[5] = 0x2ffc;
	run_test(44);
	BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::AFAULT);
	BOOST_TEST(cpu.fault_SSW & SSW_CM);
	BOOST_TEST(cpu.ex_addr == 0x3000);
    BOOST_TEST(cpu.EA == 0x2ffc);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
