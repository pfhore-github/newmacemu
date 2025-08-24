#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_ILLEGAL {
    F_ILLEGAL() {
        // BKPT #2
        TEST::SET_W(0, 0045374);
        TEST::SET_W(2, TEST_BREAK);
        jit_compile(0, 4);
    }};
BOOST_FIXTURE_TEST_SUITE(ILLEGAL, Prepare, *boost::unit_test::fixture<F_ILLEGAL>())


BOOST_AUTO_TEST_CASE(execute) {
	run_test(0, EXCEPTION_NUMBER::ILLEGAL_OP);	
}
BOOST_AUTO_TEST_SUITE_END()
