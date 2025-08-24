#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_FBcc {
    F_FBcc() {
		// L0
		TEST::SET_W(0, TEST_BREAK);
        // FBT %D1, L0
		TEST::SET_W(2, 0171200 | 0xF);
		TEST::SET_W(4, -4);
		TEST::SET_W(6, TEST_BREAK);

		// DBF %D1, L0
		TEST::SET_W(8, 0171200 | 0);
		TEST::SET_W(10, -10);
		TEST::SET_W(12, TEST_BREAK);

        // FBT %D1, L0.L
		TEST::SET_W(14, 0171300 | 0xF);
		TEST::SET_L(16, -16);
		TEST::SET_W(20, TEST_BREAK);

		jit_compile(0, 22);
    }
};

BOOST_FIXTURE_TEST_SUITE(FBcc, Prepare, *boost::unit_test::fixture<F_FBcc>())
BOOST_AUTO_TEST_CASE(untraced) {
    cpu.T = 0;
    run_test(2);
    BOOST_TEST(!cpu.must_trace);
    BOOST_TEST(cpu.PC == 2);
}

BOOST_AUTO_TEST_CASE(traced) {
    cpu.T = 1;
    run_test(2, EXCEPTION_NUMBER::TRACE);
}

BOOST_AUTO_TEST_CASE(trace_not_taken) {
    cpu.T = 1;
    cpu.FPSR.CC_Z = false;
    run_test(8);
    BOOST_TEST(!cpu.must_trace);
}

BOOST_AUTO_TEST_CASE(unbranch) {
    cpu.FPSR.CC_Z = false;
    run_test(8);
    BOOST_TEST(cpu.PC == 14);
}

BOOST_AUTO_TEST_CASE(offsetL) {
    run_test(14);
    BOOST_TEST(cpu.PC == 2);
}
BOOST_AUTO_TEST_SUITE_END()
