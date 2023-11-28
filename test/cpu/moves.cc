#define BOOST_TEST_DYN_LINK
#include "68040.hpp"

#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;

struct F_MOVES {
    F_MOVES() {
        // MOVES.B (%A2), %D3
        TEST::SET_W(0, 0007022);
        TEST::SET_W(2, 3 << 12);
        TEST::SET_W(4, TEST_BREAK);

        // MOVES.W (%A2), %D3
        TEST::SET_W(6, 0007122);
        TEST::SET_W(8, 3 << 12);
        TEST::SET_W(10, TEST_BREAK);

        // MOVES.L (%A2), %D3
        TEST::SET_W(12, 0007222);
        TEST::SET_W(14, 3 << 12);
        TEST::SET_W(16, TEST_BREAK);

        // MOVES.B %D3, (%A2)
        TEST::SET_W(18, 0007022);
        TEST::SET_W(20, 3 << 12 | 1 << 11);
        TEST::SET_W(22, TEST_BREAK);

        // MOVES.W %D3, (%A2)
        TEST::SET_W(24, 0007122);
        TEST::SET_W(26, 3 << 12 | 1 << 11);
        TEST::SET_W(28, TEST_BREAK);

        // MOVES.L %D3, (%A2)
        TEST::SET_W(30, 0007222);
        TEST::SET_W(32, 3 << 12 | 1 << 11);
        TEST::SET_W(34, TEST_BREAK);

        jit_compile(0, 36);
    }};
BOOST_FIXTURE_TEST_SUITE(MOVES, Prepare, *boost::unit_test::fixture<F_MOVES>())


BOOST_AUTO_TEST_SUITE(Load)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(NG) {
    cpu.S = false;
	run_test(0);
	BOOST_TEST( cpu.ex_n == EXCEPTION_NUMBER::PRIV_ERR );
}
BOOST_AUTO_TEST_CASE(OK) {
    cpu.S = true;
    RAM[0x100] = 0x22;
    cpu.A[2] = 0x100;
    run_test(0);
    BOOST_TEST(cpu.D[3] == 0x22);
}

BOOST_AUTO_TEST_CASE(fault) {
    cpu.S = true;
    cpu.A[2] = 0x3000;
    cpu.SFC = 4;
    run_test(0);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::AFAULT);
    BOOST_TEST((cpu.fault_SSW & TT_MASK) == TT_ALT);
    BOOST_TEST((cpu.fault_SSW & 7) == 4);
}

BOOST_AUTO_TEST_CASE(traced) {
    cpu.S = true;
    cpu.T = 1;
	run_test(0);
	BOOST_TEST( cpu.ex_n == EXCEPTION_NUMBER::TRACE );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(NG) {
    cpu.S = false;
	run_test(6);
	BOOST_TEST( cpu.ex_n == EXCEPTION_NUMBER::PRIV_ERR );
}

BOOST_AUTO_TEST_CASE(OK) {
    cpu.S = true;
    TEST::SET_W(0x100, 0x1234);
    cpu.A[2] = 0x100;
    run_test(6);
    BOOST_TEST(cpu.D[3] == 0x1234);
}

BOOST_AUTO_TEST_CASE(fault) {
    cpu.S = true;
    cpu.A[2] = 0x3000;
    cpu.SFC = 4;
    run_test(6);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::AFAULT);
    BOOST_TEST((cpu.fault_SSW & TT_MASK) == TT_ALT);
    BOOST_TEST((cpu.fault_SSW & 7) == 4);
}

BOOST_AUTO_TEST_CASE(traced) {
    cpu.S = true;
    cpu.T = 1;
	run_test(6);
	BOOST_TEST( cpu.ex_n == EXCEPTION_NUMBER::TRACE );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(NG) {
    cpu.S = false;
   	run_test(12);
	BOOST_TEST( cpu.ex_n == EXCEPTION_NUMBER::PRIV_ERR );
}

BOOST_AUTO_TEST_CASE(OK) {
    cpu.S = true;
    TEST::SET_L(0x100, 0x12345678);
    cpu.A[2] = 0x100;
    cpu.SFC = 4;
    run_test(12);
    BOOST_TEST(cpu.D[3] == 0x12345678);
    BOOST_TEST((cpu.fault_SSW & TT_MASK) == TT_ALT);
    BOOST_TEST((cpu.fault_SSW & 7) == 4);
}

BOOST_AUTO_TEST_CASE(traced) {
    cpu.S = true;
    cpu.T = 1;
	run_test(12);
	BOOST_TEST( cpu.ex_n == EXCEPTION_NUMBER::TRACE );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Store)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(NG) {
    cpu.S = false;
	run_test(18);
	BOOST_TEST( cpu.ex_n == EXCEPTION_NUMBER::PRIV_ERR );
}
BOOST_AUTO_TEST_CASE(OK) {
    cpu.S = true;
    cpu.D[3] = 0x22;
    cpu.A[2] = 0x100;
    cpu.DFC = 4;
    run_test(18);
    BOOST_TEST(RAM[0x100] == 0x22);
    BOOST_TEST((cpu.fault_SSW & TT_MASK) == TT_ALT);
    BOOST_TEST((cpu.fault_SSW & 7) == 4);
}

BOOST_AUTO_TEST_CASE(traced) {
    cpu.S = true;
    cpu.T = 1;
	run_test(18);
	BOOST_TEST( cpu.ex_n == EXCEPTION_NUMBER::TRACE );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(NG) {
    cpu.S = false;
	run_test(24);
	BOOST_TEST( cpu.ex_n == EXCEPTION_NUMBER::PRIV_ERR );
}
BOOST_AUTO_TEST_CASE(OK) {
    cpu.S = true;
    cpu.D[3] = 0x1234;
    cpu.A[2] = 0x100;
    cpu.DFC = 4;
    run_test(24);
    BOOST_TEST(TEST::GET_W(0x100) == 0x1234);
    BOOST_TEST((cpu.fault_SSW & TT_MASK) == TT_ALT);
    BOOST_TEST((cpu.fault_SSW & 7) == 4);
}

BOOST_AUTO_TEST_CASE(traced) {
    cpu.S = true;
    cpu.T = 1;
	run_test(24);
	BOOST_TEST( cpu.ex_n == EXCEPTION_NUMBER::TRACE );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(NG) {
    cpu.S = false;
	run_test(30);
	BOOST_TEST( cpu.ex_n == EXCEPTION_NUMBER::PRIV_ERR );
}

BOOST_AUTO_TEST_CASE(OK) {
    cpu.S = true;
    cpu.D[3] = 0x12345678;
    cpu.A[2] = 0x100;
    cpu.DFC = 4;
    run_test(30);
    BOOST_TEST(TEST::GET_L(0x100) == 0x12345678);
    BOOST_TEST((cpu.fault_SSW & TT_MASK) == TT_ALT);
    BOOST_TEST((cpu.fault_SSW & 7) == 4);
}

BOOST_AUTO_TEST_CASE(traced) {
    cpu.S = true;
    cpu.T = 1;
	run_test(30);
	BOOST_TEST( cpu.ex_n == EXCEPTION_NUMBER::TRACE );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
