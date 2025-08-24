#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_TRAPV {
    F_TRAPV() {
        // RTD
        TEST::SET_W(0, 0047166);
        TEST::SET_W(2, TEST_BREAK);

        jit_compile(0, 4);
    }};
BOOST_FIXTURE_TEST_SUITE(TRAPV, Prepare, *boost::unit_test::fixture<F_TRAPV>())


BOOST_DATA_TEST_CASE(run, bdata::xrange(2), v) {
    cpu.V = v;
    run_test(0, v ? EXCEPTION_NUMBER::TRAPx : EXCEPTION_NUMBER::NO_ERR);
}

BOOST_AUTO_TEST_SUITE_END()
