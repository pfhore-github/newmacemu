#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_TAS {
    F_TAS() {
        // TAS %D2
        TEST::SET_W(0, 0045300 | 2);
        TEST::SET_W(2, TEST_BREAK);
        jit_compile(0, 4);
    }};
BOOST_FIXTURE_TEST_SUITE(TAS, Prepare, *boost::unit_test::fixture<F_TAS>())


BOOST_AUTO_TEST_CASE(value) {
    cpu.D[2] = 1;
    run_test(0);
    BOOST_TEST(cpu.D[2] == 0x81);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 0x81;
    run_test(0);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
    run_test(0);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()
