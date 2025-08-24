#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_FSAVE {
    F_FSAVE() {
        // FSAVE (%A2)
        TEST::SET_W(0, 0171422);
        TEST::SET_W(2, TEST_BREAK);

        jit_compile(0, 4);
    }
};
BOOST_FIXTURE_TEST_SUITE(FSAVE, Prepare, *boost::unit_test::fixture<F_FSAVE>())

BOOST_AUTO_TEST_CASE(err) {
    cpu.S = false;
    cpu.A[2] = 0x100;
    run_test(0, EXCEPTION_NUMBER::PRIV_ERR);
}

BOOST_AUTO_TEST_CASE(traced) {
    cpu.S = true;
    cpu.T = 1;
    cpu.A[2] = 0x1000;
    run_test(0, EXCEPTION_NUMBER::TRACE);
}
BOOST_AUTO_TEST_CASE(normal) {
    cpu.S = true;
    cpu.A[2] = 0x1000;
    run_test(0);
    BOOST_TEST(TEST::GET_L(0x1000) == 0x41000000);
}

BOOST_AUTO_TEST_SUITE_END()