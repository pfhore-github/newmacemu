#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(NOP, Prepare)
struct F {
    F() {
        // NOP
        TEST::SET_W(0, 0047161);
        TEST::SET_W(2, TEST_BREAK);

        jit_compile(0, 4);
    }
};
BOOST_AUTO_TEST_SUITE(R, *boost::unit_test::fixture<F>())
BOOST_AUTO_TEST_CASE(execute) {
	run_test(0);
	BOOST_TEST(cpu.ex_n == 0 );
}
BOOST_AUTO_TEST_CASE(trace_always) {
    cpu.T = 2;
	run_test(0);
	BOOST_TEST(cpu.ex_n == 9 );
}

BOOST_AUTO_TEST_CASE(tracecd) {
    cpu.T = 1;
	run_test(0);
	BOOST_TEST(cpu.ex_n == 9 );
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
