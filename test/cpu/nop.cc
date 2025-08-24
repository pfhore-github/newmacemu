#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_NOP {
    F_NOP() {
        // NOP
        TEST::SET_W(0, 0047161);
        TEST::SET_W(2, TEST_BREAK);

        jit_compile(0, 4);
    }};
BOOST_FIXTURE_TEST_SUITE(NOP, Prepare, *boost::unit_test::fixture<F_NOP>())


BOOST_AUTO_TEST_CASE(execute) {
	run_test(0, EXCEPTION_NUMBER::NO_ERR );
}
BOOST_AUTO_TEST_CASE(trace_always) {
    cpu.T = 2;
	run_test(0, EXCEPTION_NUMBER::TRACE );
}

BOOST_AUTO_TEST_CASE(tracecd) {
    cpu.T = 1;
	run_test(0, EXCEPTION_NUMBER::TRACE );
}
BOOST_AUTO_TEST_SUITE_END()
