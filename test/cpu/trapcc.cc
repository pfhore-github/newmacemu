#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;

struct F_TRAPcc {
    F_TRAPcc() {
        // TRAPF
        TEST::SET_W(0, 0050370 | 1 << 8 | 4);
        TEST::SET_W(2, TEST_BREAK);

        // TRAPF #10.w
        TEST::SET_W(4, 0050370 | 1 << 8 | 2);
        TEST::SET_W(6, 10);
        TEST::SET_W(8, TEST_BREAK);

        // TRAPF #10.l
        TEST::SET_W(10, 0050370 | 1 << 8 | 3);
        TEST::SET_L(12, 10);
        TEST::SET_W(16, TEST_BREAK);

        // TRAPT
        TEST::SET_W(18, 0050370 | 0 << 8 | 4);
        TEST::SET_W(20, TEST_BREAK);

        jit_compile(0, 22);
    }};
BOOST_FIXTURE_TEST_SUITE(TRAPcc, Prepare, *boost::unit_test::fixture<F_TRAPcc>())


BOOST_AUTO_TEST_CASE(noext) {
    run_test(0, EXCEPTION_NUMBER::NO_ERR);
    BOOST_TEST(cpu.PC == 4);
}

BOOST_AUTO_TEST_CASE(extW) {
    run_test(4, EXCEPTION_NUMBER::NO_ERR);
    BOOST_TEST(cpu.PC == 10);
}

BOOST_AUTO_TEST_CASE(extL) {
    run_test(10, EXCEPTION_NUMBER::NO_ERR);
    BOOST_TEST(cpu.PC == 18);
}
BOOST_AUTO_TEST_CASE(T) {
    run_test(18, EXCEPTION_NUMBER::TRAPx);
}

BOOST_AUTO_TEST_SUITE_END()
