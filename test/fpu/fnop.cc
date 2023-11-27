#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_FNOP {
    F_FNOP() {
		// FNOP
		TEST::SET_W(0, 0171200);
		TEST::SET_W(2, 0);
		TEST::SET_W(4, TEST_BREAK);

		jit_compile(0, 6);
    }
};

BOOST_FIXTURE_TEST_CASE(FNOP, Prepare, *boost::unit_test::fixture<F_FNOP>()) {
    run_test(0);
    BOOST_TEST(cpu.PC == 0x6);
}
