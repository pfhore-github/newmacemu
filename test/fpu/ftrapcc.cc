#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_FTRAPcc {
    F_FTRAPcc() {
		// FTRAPF
		TEST::SET_W(0, 0171170 | 4);
		TEST::SET_W(2, 0);
        TEST::SET_W(4, TEST_BREAK);

		// FTRAPF #0x100
		TEST::SET_W(6, 0171170 | 2);
		TEST::SET_W(8, 0);
		TEST::SET_W(10, 0x100);
        TEST::SET_W(12, TEST_BREAK);

		// FTRAPF #0x100.L
		TEST::SET_W(14, 0171170 | 3);
		TEST::SET_W(16, 0);
		TEST::SET_L(18, 0x100);
        TEST::SET_W(22, TEST_BREAK);

		// FTRAPT
		TEST::SET_W(24, 0171170 | 4);
		TEST::SET_W(26, 0xF);
        TEST::SET_W(28, TEST_BREAK);

		jit_compile(0, 30);
    }
};
BOOST_FIXTURE_TEST_SUITE(FTRAPcc, Prepare, *boost::unit_test::fixture<F_FTRAPcc>())
BOOST_AUTO_TEST_CASE(ImmNone) {
    run_test(0);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::NO_ERR);
    BOOST_TEST(cpu.PC == 6);
}

BOOST_AUTO_TEST_CASE(Imm16) {
    run_test(6);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::NO_ERR);
    BOOST_TEST(cpu.PC == 14);
}

BOOST_AUTO_TEST_CASE(Imm32) {
    run_test(14);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::NO_ERR);
    BOOST_TEST(cpu.PC == 24);
}

BOOST_AUTO_TEST_CASE(TRAP) {
    run_test(24);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::TRAPx);
}

BOOST_AUTO_TEST_SUITE_END()
