#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <fmt/core.h>
namespace bdata = boost::unit_test::data;
struct F_DBcc {
    F_DBcc() {
        // DBT (%A3), %D4, #0x10
        TEST::SET_W(0, 0050310 | 0 << 8 | 4);
        TEST::SET_W(2, 10);
        TEST::SET_W(4, TEST_BREAK);

        // DBF (%A3), %D4, #0x10
        TEST::SET_W(6, 0050310 | 1 << 8 | 4);
        TEST::SET_W(8, 4);
        TEST::SET_W(10, TEST_BREAK);
        TEST::SET_W(12, TEST_BREAK);

        jit_compile(0, 14);
    }};
BOOST_FIXTURE_TEST_SUITE(DBcc, Prepare, *boost::unit_test::fixture<F_DBcc>())


BOOST_DATA_TEST_CASE(T, bdata::xrange(2), tr) {
    cpu.C = false;
    cpu.T = tr;
    run_test(0);
    BOOST_TEST(cpu.PC == 6);
}
BOOST_AUTO_TEST_CASE(F1) {
    cpu.D[4] = 5;
    run_test(6);
    BOOST_TEST(cpu.D[4] == 4);
    BOOST_TEST(cpu.PC == 14);
}

BOOST_AUTO_TEST_CASE(F2) {
    cpu.D[4] = 0;
    run_test(6);
    BOOST_TEST(cpu.D[4] == 0xffff);
    BOOST_TEST(cpu.PC == 12);
}

BOOST_AUTO_TEST_CASE(F_traced) {
    cpu.D[4] = 5;
    cpu.T = 1;
    run_test(6);
    BOOST_TEST(ex_n == EXCEPTION_NUMBER::TRACE);
    BOOST_TEST(cpu.D[4] == 4);
}
BOOST_AUTO_TEST_SUITE_END()
