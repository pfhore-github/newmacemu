#define BOOST_TEST_DYN_LINK
#include "test.hpp"
#include "68040.hpp"

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
extern bool is_reset;
BOOST_FIXTURE_TEST_SUITE(RESET, Prepare)
struct F {
    F() {
        // RESET
        TEST::SET_W(0, 0047160);
        TEST::SET_W(2, TEST_BREAK);

        jit_compile(0, 4);
    }
};
BOOST_AUTO_TEST_SUITE(R, *boost::unit_test::fixture<F>())
BOOST_AUTO_TEST_CASE(user) {
    cpu.S = false;
	run_test(0);
	BOOST_TEST( cpu.ex_n == 8 );
}

BOOST_AUTO_TEST_CASE(sys) {
    cpu.S = true;
    is_reset = false;
    run_test(0);
    BOOST_TEST(is_reset);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
