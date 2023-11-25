#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_EXT {
    F_EXT() {
        // EXT.W %D2
        TEST::SET_W(0, 0044200 | 2);
        TEST::SET_W(2, TEST_BREAK);

        // EXT.L %D2
        TEST::SET_W(4, 0044300 | 2);
        TEST::SET_W(6, TEST_BREAK);

        // EXTB.L %D2
        TEST::SET_W(8, 0044700 | 2);
        TEST::SET_W(10, TEST_BREAK);

        jit_compile(0, 12);
    }
};
BOOST_FIXTURE_TEST_SUITE(EXT, Prepare, *boost::unit_test::fixture<F_EXT>())

BOOST_AUTO_TEST_SUITE(B2W)

BOOST_AUTO_TEST_CASE(value) {
    cpu.D[2] = 0xFF;
    run_test(0);
    BOOST_TEST(cpu.D[2] == 0xFFFF);
    BOOST_TEST(cpu.N);
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 2;
    run_test(0);
    BOOST_TEST(!cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
    run_test(0);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(W2L)

BOOST_AUTO_TEST_CASE(value) {
    cpu.D[2] = 0xFFFF;
    run_test(4);
    BOOST_TEST(cpu.D[2] == 0xFFFFFFFF);
    BOOST_TEST(cpu.N);
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 2;
    run_test(4);
    BOOST_TEST(!cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
    run_test(4);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(B2L)

BOOST_AUTO_TEST_CASE(value) {
    cpu.D[2] = 0xFF;
    run_test(8);
    BOOST_TEST(cpu.D[2] == 0xFFFFFFFF);
    BOOST_TEST(cpu.N);
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 2;
    run_test(8);
    BOOST_TEST(!cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
    run_test(8);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
