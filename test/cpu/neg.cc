#define BOOST_TEST_DYN_LINK
#include "68040.hpp"

#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_NEG {
    F_NEG() {
        // NEG.B %D2
        TEST::SET_W(0, 0042000 | 2);
        TEST::SET_W(2, TEST_BREAK);

        // NEG.W %D2
        TEST::SET_W(4, 0042100 | 2);
        TEST::SET_W(6, TEST_BREAK);

        // NEG.L %D2
        TEST::SET_W(8, 0042200 | 2);
        TEST::SET_W(10, TEST_BREAK);

        jit_compile(0, 12);
    }};
BOOST_FIXTURE_TEST_SUITE(NEG, Prepare, *boost::unit_test::fixture<F_NEG>())



BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(value) {
    cpu.D[2] = -2;
    run_test(0);
    BOOST_TEST((cpu.D[2] & 0xff) == 2);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_CASE(V) {
    cpu.D[2] = 0x80;
    run_test(0);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 3;
    run_test(0);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
    run_test(0);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)


BOOST_AUTO_TEST_CASE(value) {
    cpu.D[2] = -200;
    run_test(4);
    BOOST_TEST((cpu.D[2] & 0xffff) == 200);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_CASE(V) {
    cpu.D[2] = 0x8000;
    run_test(4);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 100;
    run_test(4);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
    run_test(4);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)



BOOST_AUTO_TEST_CASE(value) {
    cpu.D[2] = -200000;
    run_test(8);
    BOOST_TEST(cpu.D[2] == 200000);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
}


BOOST_AUTO_TEST_CASE(V) {
    cpu.D[2] = 0x80000000;
    run_test(8);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 10000;
    run_test(8);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
    run_test(8);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
