#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(TRAPV, Prepare)
struct F {
    F() {
        // RTD
        TEST::SET_W(0, 0047166);
        TEST::SET_W(2, TEST_BREAK);

        jit_compile(0, 4);
    }
};
BOOST_AUTO_TEST_SUITE(R, *boost::unit_test::fixture<F>())
BOOST_DATA_TEST_CASE(run, bdata::xrange(2), v) {
    cpu.V = v;
    run_test(0);
    BOOST_TEST(int(cpu.ex_n) == (v ? 7 : 0));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
