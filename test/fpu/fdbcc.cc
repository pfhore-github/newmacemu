#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_FDBcc {
    F_FDBcc() {
		// L0
		TEST::SET_W(0, TEST_BREAK);
        // FDBT %D1, L0
		TEST::SET_W(2, 0171110 | 1);
		TEST::SET_W(4, 0xf);
		TEST::SET_W(6, -6);
		TEST::SET_W(8, TEST_BREAK);

		// FDBF %D1, L0
		TEST::SET_W(10, 0171110 | 1);
		TEST::SET_W(12, 0);
		TEST::SET_W(14, -14);
		TEST::SET_W(16, TEST_BREAK);

		jit_compile(0, 18);
    }
};

BOOST_FIXTURE_TEST_SUITE(FDBcc, Prepare, *boost::unit_test::fixture<F_FDBcc>())
BOOST_AUTO_TEST_CASE(Trace) {
    cpu.D[1] = 3;
    cpu.T = 1;
    run_test(2);
    BOOST_TEST(cpu.ex_n == EXCAPTION_NUMBER::TRACE);
}
BOOST_AUTO_TEST_CASE(notDecement) {
    cpu.D[1] = 3;
    cpu.FPSR.CC_Z = true;
    run_test(2);
    BOOST_TEST(cpu.PC == 10);
    BOOST_TEST(cpu.D[1] == 3);
}
BOOST_AUTO_TEST_CASE(countZero) {
    cpu.D[1] = 0;
    run_test(10);
    BOOST_TEST(cpu.PC == 18);
    BOOST_TEST(cpu.D[1] == 0xffff);
}
BOOST_AUTO_TEST_CASE(countNonZero) {
    cpu.D[1] = 4;
    run_test(10);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[1] == 3);
}
BOOST_AUTO_TEST_SUITE_END()
