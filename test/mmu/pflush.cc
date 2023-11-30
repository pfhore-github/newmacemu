#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_PFLUSHx {
	F_PFLUSHx() {
		// FPLUSHN (%A3)
		TEST::SET_W(0, 0172400 | 3);
        TEST::SET_W(2, TEST_BREAK);

		// FPLUSH (%A3)
		TEST::SET_W(4, 0172410 | 3);
        TEST::SET_W(6, TEST_BREAK);

		// FPLUSHAN
		TEST::SET_W(8, 0172420);
        TEST::SET_W(10, TEST_BREAK);

		// FPLUSHA
		TEST::SET_W(12, 0172430);
        TEST::SET_W(14, TEST_BREAK);

        jit_compile(0, 16);
	}
};
BOOST_FIXTURE_TEST_SUITE(PFLUSHx, Prepare,
                         *boost::unit_test::fixture<F_PFLUSHx>())

BOOST_AUTO_TEST_SUITE(PFLUSHN)
BOOST_AUTO_TEST_CASE(err) {
    cpu.S = false;
	run_test(0);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::PRIV_ERR);
}
BOOST_AUTO_TEST_CASE(traced) {
	cpu.S = true;
    cpu.T = 1;
    cpu.DFC = 2;
    cpu.A[3] = 0x1 << 12;
	run_test(0);
	BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::TRACE );
}

BOOST_AUTO_TEST_CASE(user) {
    cpu.S = true;
    cpu.DFC = 2;
    cpu.l_atc[1][1] = {2, 0, false, 0, false, false, true};
    cpu.l_atc[1][2] = {2, 0, false, 0, false, false, true};
    cpu.l_atc[0][1] = {3, 0, false, 0, false, false, true};
    cpu.l_atc[0][2] = {3, 0, false, 0, false, false, true};
    cpu.g_atc[1][1] = {4, 0, false, 0, false, false, true};
    cpu.g_atc[1][2] = {4, 0, false, 0, false, false, true};
    cpu.g_atc[0][1] = {5, 0, false, 0, false, false, true};
    cpu.g_atc[0][2] = {5, 0, false, 0, false, false, true};
    cpu.A[3] = 0x1 << 12;
    run_test(0);
    BOOST_TEST(!cpu.l_atc[0].contains(1));
    BOOST_TEST(cpu.l_atc[1].contains(1));
    BOOST_TEST(cpu.g_atc[0].contains(1));
    BOOST_TEST(cpu.g_atc[1].contains(1));
    BOOST_TEST(cpu.l_atc[0].contains(2));
    BOOST_TEST(cpu.l_atc[1].contains(2));
    BOOST_TEST(cpu.g_atc[0].contains(2));
    BOOST_TEST(cpu.g_atc[1].contains(2));
}

BOOST_AUTO_TEST_CASE(sys) {
    cpu.S = true;
    cpu.DFC = 5;
    cpu.l_atc[1][1] = {2, 0, false, 0, false, false, true};
    cpu.l_atc[1][2] = {2, 0, false, 0, false, false, true};
    cpu.l_atc[0][1] = {3, 0, false, 0, false, false, true};
    cpu.l_atc[0][2] = {3, 0, false, 0, false, false, true};
    cpu.g_atc[1][1] = {4, 0, false, 0, false, false, true};
    cpu.g_atc[1][2] = {4, 0, false, 0, false, false, true};
    cpu.g_atc[0][1] = {5, 0, false, 0, false, false, true};
    cpu.g_atc[0][2] = {5, 0, false, 0, false, false, true};
    cpu.A[3] = 0x1 << 12;
    run_test(0);
    BOOST_TEST(cpu.l_atc[0].contains(1));
    BOOST_TEST(!cpu.l_atc[1].contains(1));
    BOOST_TEST(cpu.g_atc[0].contains(1));
    BOOST_TEST(cpu.g_atc[1].contains(1));
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(PFLUSH)
BOOST_AUTO_TEST_CASE(err) {
    cpu.S = false;
	run_test(4);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::PRIV_ERR);
}

BOOST_AUTO_TEST_CASE(traced) {
    cpu.S = true;
    cpu.T = 1;
    cpu.DFC = 2;
    cpu.A[3] = 0x1 << 12;
	run_test(4);
	BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::TRACE );
}

BOOST_AUTO_TEST_CASE(user) {
    cpu.S = true;
    cpu.DFC = 2;
    cpu.l_atc[1][1] = {2, 0, false, 0, false, false, true};
    cpu.l_atc[1][2] = {2, 0, false, 0, false, false, true};
    cpu.l_atc[0][1] = {3, 0, false, 0, false, false, true};
    cpu.l_atc[0][2] = {3, 0, false, 0, false, false, true};
    cpu.g_atc[1][1] = {4, 0, false, 0, false, false, true};
    cpu.g_atc[1][2] = {4, 0, false, 0, false, false, true};
    cpu.g_atc[0][1] = {5, 0, false, 0, false, false, true};
    cpu.g_atc[0][2] = {5, 0, false, 0, false, false, true};
    cpu.A[3] = 0x1 << 12;
	run_test(4);
    BOOST_TEST(!cpu.l_atc[0].contains(1));
    BOOST_TEST(cpu.l_atc[1].contains(1));
    BOOST_TEST(!cpu.g_atc[0].contains(1));
    BOOST_TEST(cpu.g_atc[1].contains(1));
    BOOST_TEST(cpu.l_atc[0].contains(2));
    BOOST_TEST(cpu.l_atc[1].contains(2));
    BOOST_TEST(cpu.g_atc[0].contains(2));
    BOOST_TEST(cpu.g_atc[1].contains(2));
}

BOOST_AUTO_TEST_CASE(sys) {
    cpu.S = true;
    cpu.DFC = 5;
    cpu.l_atc[1][1] = {2, 0, false, 0, false, false, true};
    cpu.l_atc[1][2] = {2, 0, false, 0, false, false, true};
    cpu.l_atc[0][1] = {3, 0, false, 0, false, false, true};
    cpu.l_atc[0][2] = {3, 0, false, 0, false, false, true};
    cpu.g_atc[1][1] = {4, 0, false, 0, false, false, true};
    cpu.g_atc[1][2] = {4, 0, false, 0, false, false, true};
    cpu.g_atc[0][1] = {5, 0, false, 0, false, false, true};
    cpu.g_atc[0][2] = {5, 0, false, 0, false, false, true};
    cpu.A[3] = 0x1 << 12;
	run_test(4);
    BOOST_TEST(cpu.l_atc[0].contains(1));
    BOOST_TEST(!cpu.l_atc[1].contains(1));
    BOOST_TEST(cpu.g_atc[0].contains(1));
    BOOST_TEST(!cpu.g_atc[1].contains(1));
    BOOST_TEST(cpu.l_atc[0].contains(2));
    BOOST_TEST(cpu.l_atc[1].contains(2));
    BOOST_TEST(cpu.g_atc[0].contains(2));
    BOOST_TEST(cpu.g_atc[1].contains(2));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(PFLUSHAN)
BOOST_AUTO_TEST_CASE(err) {
    cpu.S = false;
    run_test(8);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::PRIV_ERR);
}

BOOST_AUTO_TEST_CASE(traced) {
    cpu.S = true;
    cpu.T = 1;
    cpu.DFC = 2;
    run_test(8);
	BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::TRACE );
}

BOOST_AUTO_TEST_CASE(user) {
    cpu.S = true;
    cpu.DFC = 2;
    cpu.l_atc[1][1] = {2, 0, false, 0, false, false, true};
    cpu.l_atc[1][2] = {2, 0, false, 0, false, false, true};
    cpu.l_atc[0][1] = {3, 0, false, 0, false, false, true};
    cpu.l_atc[0][2] = {3, 0, false, 0, false, false, true};
    cpu.g_atc[1][1] = {4, 0, false, 0, false, false, true};
    cpu.g_atc[1][2] = {4, 0, false, 0, false, false, true};
    cpu.g_atc[0][1] = {5, 0, false, 0, false, false, true};
    cpu.g_atc[0][2] = {5, 0, false, 0, false, false, true};
    run_test(8);
    BOOST_TEST(cpu.l_atc[0].empty());
    BOOST_TEST(cpu.g_atc[0].contains(1));
    BOOST_TEST(cpu.l_atc[1].contains(1));
    BOOST_TEST(cpu.g_atc[1].contains(1));
    BOOST_TEST(cpu.g_atc[0].contains(2));
    BOOST_TEST(cpu.l_atc[1].contains(2));
    BOOST_TEST(cpu.g_atc[1].contains(2));
}

BOOST_AUTO_TEST_CASE(sys) {
    cpu.S = true;
    cpu.DFC = 5;
    cpu.l_atc[1][1] = {2, 0, false, 0, false, false, true};
    cpu.l_atc[1][2] = {2, 0, false, 0, false, false, true};
    cpu.l_atc[0][1] = {3, 0, false, 0, false, false, true};
    cpu.l_atc[0][2] = {3, 0, false, 0, false, false, true};
    cpu.g_atc[1][1] = {4, 0, false, 0, false, false, true};
    cpu.g_atc[1][2] = {4, 0, false, 0, false, false, true};
    cpu.g_atc[0][1] = {5, 0, false, 0, false, false, true};
    cpu.g_atc[0][2] = {5, 0, false, 0, false, false, true};
    run_test(8);
    BOOST_TEST(!cpu.l_atc[1].contains(1));
    BOOST_TEST(cpu.g_atc[0].contains(1));
    BOOST_TEST(cpu.l_atc[0].contains(1));
    BOOST_TEST(cpu.g_atc[1].contains(1));
    BOOST_TEST(cpu.l_atc[0].contains(2));
    BOOST_TEST(cpu.g_atc[0].contains(2));
    BOOST_TEST(cpu.g_atc[1].contains(2));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(PFLUSHA)
BOOST_AUTO_TEST_CASE(err) {
    cpu.S = false;
	run_test(12);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::PRIV_ERR);
}

BOOST_AUTO_TEST_CASE(traced) {
    cpu.S = true;
    cpu.T = 1;
    cpu.DFC = 2;
    run_test(12);
	BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::TRACE );
}

BOOST_AUTO_TEST_CASE(user) {
    cpu.S = true;
    cpu.DFC = 2;
    cpu.l_atc[1][1] = {2, 0, false, 0, false, false, true};
    cpu.l_atc[1][2] = {2, 0, false, 0, false, false, true};
    cpu.l_atc[0][1] = {3, 0, false, 0, false, false, true};
    cpu.l_atc[0][2] = {3, 0, false, 0, false, false, true};
    cpu.g_atc[1][1] = {4, 0, false, 0, false, false, true};
    cpu.g_atc[1][2] = {4, 0, false, 0, false, false, true};
    cpu.g_atc[0][1] = {5, 0, false, 0, false, false, true};
    cpu.g_atc[0][2] = {5, 0, false, 0, false, false, true};
    run_test(12);
    BOOST_TEST(cpu.l_atc[0].empty());
    BOOST_TEST(cpu.g_atc[0].empty());
    BOOST_TEST(cpu.l_atc[1].contains(1));
    BOOST_TEST(cpu.g_atc[1].contains(1));
    BOOST_TEST(cpu.l_atc[1].contains(2));
    BOOST_TEST(cpu.g_atc[1].contains(2));
}

BOOST_AUTO_TEST_CASE(sys) {
    cpu.S = true;
    cpu.DFC = 5;
    cpu.l_atc[1][1] = {2, 0, false, 0, false, false, true};
    cpu.l_atc[1][2] = {2, 0, false, 0, false, false, true};
    cpu.l_atc[0][1] = {3, 0, false, 0, false, false, true};
    cpu.l_atc[0][2] = {3, 0, false, 0, false, false, true};
    cpu.g_atc[1][1] = {4, 0, false, 0, false, false, true};
    cpu.g_atc[1][2] = {4, 0, false, 0, false, false, true};
    cpu.g_atc[0][1] = {5, 0, false, 0, false, false, true};
    cpu.g_atc[0][2] = {5, 0, false, 0, false, false, true};
    run_test(12);
    BOOST_TEST(!cpu.l_atc[1].contains(1));
    BOOST_TEST(!cpu.g_atc[1].contains(1));
    BOOST_TEST(cpu.g_atc[0].contains(1));
    BOOST_TEST(cpu.l_atc[0].contains(1));
    BOOST_TEST(cpu.l_atc[0].contains(2));
    BOOST_TEST(cpu.g_atc[0].contains(2));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
