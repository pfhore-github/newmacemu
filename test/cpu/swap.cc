#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_SWAP {
    F_SWAP() {
        // SWAP %D2
        TEST::SET_W(0, 0044100 | 2);
        TEST::SET_W(2, TEST_BREAK);

        jit_compile(0, 4);
    }};
BOOST_FIXTURE_TEST_SUITE(SWAP, Prepare, *boost::unit_test::fixture<F_SWAP>())


BOOST_AUTO_TEST_CASE(value) {
    cpu.D[2] = 0x12345678;
    run_test(0);
    BOOST_TEST(cpu.D[2] == 0x56781234);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 0x1234A678;
    run_test(0);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
    run_test(0);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()
