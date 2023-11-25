#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_TST {
    F_TST() {
        // TST.B %D2
        TEST::SET_W(0, 0045000 | 2);
        TEST::SET_W(2, TEST_BREAK);

        // TST.W %D2
        TEST::SET_W(4, 0045100 | 2);
        TEST::SET_W(6, TEST_BREAK);

        // TST.L %D2
        TEST::SET_W(8, 0045200 | 2);
        TEST::SET_W(10, TEST_BREAK);
        jit_compile(0, 12);
    }};
BOOST_FIXTURE_TEST_SUITE(TST, Prepare, *boost::unit_test::fixture<F_TST>())


BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(normal) {
    cpu.D[2] = 1;
    run_test(0);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 0x80;
    run_test(0);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
    run_test(0);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(normal) {
    cpu.D[2] = 1;
    run_test(4);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 0x8000;
    run_test(4);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
    run_test(4);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(normal) {
    cpu.D[2] = 1;
    run_test(8);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 0x80000000;
    run_test(8);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
    run_test(8);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
