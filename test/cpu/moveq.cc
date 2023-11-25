#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_MOVEQ {
    F_MOVEQ() {
        // MOVEQ #10, %D3
        TEST::SET_W(0, 0070000 | 3 << 9 | 10);
        TEST::SET_W(2, TEST_BREAK);

        // MOVEQ #-1, %D3
        TEST::SET_W(4, 0070000 | 3 << 9 | 0xff);
        TEST::SET_W(6, TEST_BREAK);

        // MOVEQ #0, %D3
        TEST::SET_W(8, 0070000 | 3 << 9 | 0);
        TEST::SET_W(10, TEST_BREAK);

        jit_compile(0, 12);
    }};
BOOST_FIXTURE_TEST_SUITE(MOVEQ, Prepare, *boost::unit_test::fixture<F_MOVEQ>())


BOOST_AUTO_TEST_CASE(positive) {
    run_test(0);
    BOOST_TEST(cpu.D[3] == 10);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(negative) {
    run_test(4);
    BOOST_TEST(cpu.D[3] == -1);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(zero) {
    run_test(8);
    BOOST_TEST(cpu.D[3] == 0);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()


