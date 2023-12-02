#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_BSR {
    F_BSR() {
        // BSR #2
        TEST::SET_W(0, 0060400 | 2);
        TEST::SET_W(2, TEST_BREAK);
        TEST::SET_W(4, TEST_BREAK);

        jit_compile(0, 6);
    }};
BOOST_FIXTURE_TEST_SUITE(BSR, Prepare, *boost::unit_test::fixture<F_BSR>())



BOOST_AUTO_TEST_CASE(traced) {
    cpu.T = 1;
    run_test(0);
	BOOST_TEST(ex_n == EXCEPTION_NUMBER::TRACE );
}

BOOST_AUTO_TEST_CASE(not_traced) {
    run_test(0);
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(TEST::GET_L(0x5FFC) == 0x2);
    BOOST_TEST(cpu.A[7] == 0x5FFC);
}


BOOST_AUTO_TEST_SUITE_END()
