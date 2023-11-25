#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_BRA {
    F_BRA() {
        // BRA #40
        TEST::SET_W(0, 0060000 | 40);
        TEST::SET_W(2, TEST_BREAK);

        // BRA #3
        TEST::SET_W(4, 0060000 | 3);
        TEST::SET_W(6, TEST_BREAK);

        // BRA #-4
        TEST::SET_W(8, TEST_BREAK);
        TEST::SET_W(10, 0060000 | 0xfc);
        TEST::SET_W(12, TEST_BREAK);

        // BRA.W #26
        TEST::SET_W(14, 0060000 | 0);
        TEST::SET_W(16, 26);
        TEST::SET_W(18, TEST_BREAK);

        // BRA.W #100
        TEST::SET_W(20, 0060000 | 0);
        TEST::SET_W(22, 100);
        TEST::SET_W(24, TEST_BREAK);

        // BRA.L #14
        TEST::SET_W(26, 0060000 | 0xff);
        TEST::SET_L(28, 14);
        TEST::SET_W(32, TEST_BREAK);

        // BRA.L #86
        TEST::SET_W(34, 0060000 | 0xff);
        TEST::SET_L(36, 86);
        TEST::SET_W(40, TEST_BREAK);


        TEST::SET_W(42, TEST_BREAK);

        TEST::SET_W(122, TEST_BREAK);

        jit_compile(0, 44);
        jit_compile(122, 2);
    }};
BOOST_FIXTURE_TEST_SUITE(BRA, Prepare, *boost::unit_test::fixture<F_BRA>())



BOOST_AUTO_TEST_CASE(traced) {
    cpu.T = 1;
    run_test(0);
    BOOST_TEST(cpu.ex_n == 9);
}

BOOST_AUTO_TEST_CASE(addressError) {
    run_test(4);
    BOOST_TEST(cpu.ex_n == 3);
}

BOOST_AUTO_TEST_CASE(offset1) {
    run_test(0);
    BOOST_TEST(cpu.PC == 44);
}
BOOST_AUTO_TEST_CASE(offsetNeg) {
    run_test(10);
    BOOST_TEST(cpu.PC == 10);
}
BOOST_AUTO_TEST_CASE(offset00) {
    run_test(14);
    BOOST_TEST(cpu.PC == 44);
}

BOOST_AUTO_TEST_CASE(offset00_oo) {
    run_test(20);
    BOOST_TEST(cpu.PC == 124);
}

BOOST_AUTO_TEST_CASE(offsetFF) {
    run_test(26);
    BOOST_TEST(cpu.PC == 44);
}

BOOST_AUTO_TEST_CASE(offsetFF_oo) {
    run_test(34);
    BOOST_TEST(cpu.PC == 124);
}

BOOST_AUTO_TEST_SUITE_END()
