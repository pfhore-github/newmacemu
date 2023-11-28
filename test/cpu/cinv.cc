#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include "exception.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_CINV {
	F_CINV() {
		// CINVP DC, (%A3)
		TEST::SET_W(0, 0172110 | 3);
        TEST::SET_W(2, TEST_BREAK);

		// CINVL DC, (%A3)
		TEST::SET_W(4, 0172120 | 3);
        TEST::SET_W(6, TEST_BREAK);

		// CINVA DC
		TEST::SET_W(8, 0172130);
		TEST::SET_W(10, TEST_BREAK);

		// CINVP IC, (%A3)
		TEST::SET_W(12, 0172210 | 3);
		TEST::SET_W(14, TEST_BREAK);

		// CINVL IC, (%A3)
		TEST::SET_W(16, 0172220 | 3);
		TEST::SET_W(18, TEST_BREAK);

		// CINVA IC
		TEST::SET_W(20, 0172230);
		TEST::SET_W(22, TEST_BREAK);

		// CINVP BC, (%A3)
		TEST::SET_W(24, 0172310 | 3);
		TEST::SET_W(26, TEST_BREAK);

		// CINVL BC, (%A3)
		TEST::SET_W(28, 0172320 | 3);
		TEST::SET_W(30, TEST_BREAK);

 		// CINVA BC
		TEST::SET_W(32, 0172330);
		TEST::SET_W(34, TEST_BREAK);

       jit_compile(0, 36);
	}
};
BOOST_FIXTURE_TEST_SUITE(CINV, Prepare,
                         *boost::unit_test::fixture<F_CINV>())

struct PrivPtn {
	bool s;
	int t;
	EXCEPTION_NUMBER e;
};

static PrivPtn privPtn[3] = {
	{ false, 0, EXCEPTION_NUMBER::PRIV_ERR },
	{ true, 1, EXCEPTION_NUMBER::TRACE },
	{ true, 0, EXCEPTION_NUMBER::NO_ERR },
};
auto& operator<<(std::ostream& os, const PrivPtn& p) {
	return os << "S=" << p.s << ", T=" << p.t;
}
	
BOOST_AUTO_TEST_SUITE(DC)
BOOST_DATA_TEST_CASE(Page, privPtn, p) {
    cpu.S = p.s;
    cpu.T = p.t;
	run_test(0);
    BOOST_TEST(cpu.ex_n == p.e);
}

BOOST_DATA_TEST_CASE(Line, privPtn, p) {
    cpu.S = p.s;
    cpu.T = p.t;
	run_test(4);
    BOOST_TEST(cpu.ex_n == p.e);
}

BOOST_DATA_TEST_CASE(ALL, privPtn, p) {
    cpu.S = p.s;
    cpu.T = p.t;
	run_test(8);
    BOOST_TEST(cpu.ex_n == p.e);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(IC)
BOOST_DATA_TEST_CASE(Page, privPtn, p) {
    cpu.S = p.s;
    cpu.T = p.t;
	run_test(12);
    BOOST_TEST(cpu.ex_n == p.e);
}

BOOST_DATA_TEST_CASE(Line, privPtn, p) {
    cpu.S = p.s;
    cpu.T = p.t;
	run_test(16);
    BOOST_TEST(cpu.ex_n == p.e);
}

BOOST_DATA_TEST_CASE(ALL, privPtn, p) {
    cpu.S = p.s;
    cpu.T = p.t;
	run_test(20);
    BOOST_TEST(cpu.ex_n == p.e);
}


BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(BC)
BOOST_DATA_TEST_CASE(Page, privPtn, p) {
    cpu.S = p.s;
    cpu.T = p.t;
	run_test(24);
    BOOST_TEST(cpu.ex_n == p.e);
}

BOOST_DATA_TEST_CASE(Line, privPtn, p) {
    cpu.S = p.s;
    cpu.T = p.t;
	run_test(28);
    BOOST_TEST(cpu.ex_n == p.e);
}

BOOST_DATA_TEST_CASE(ALL, privPtn, p) {
    cpu.S = p.s;
    cpu.T = p.t;
	run_test(32);
    BOOST_TEST(cpu.ex_n == p.e);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
