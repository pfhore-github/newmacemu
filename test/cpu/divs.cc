#define BOOST_TEST_DYN_LINK
#include "68040.hpp"

#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_DIVS {
    F_DIVS() {
        // DIVS.W %D3, %D2
        TEST::SET_W(0, 0100700 | 3 << 9 | 2);
        TEST::SET_W(2, TEST_BREAK);

        // DIVS.L %D2, %D3
        TEST::SET_W(4, 0046100 | 2);
        TEST::SET_W(6, 0004000 | 3 << 12 | 3);
        TEST::SET_W(8, TEST_BREAK);

        // DIVSL.L %D2, %D4:%D3
        TEST::SET_W(10, 0046100 | 2);
        TEST::SET_W(12, 0004000 | 3 << 12 | 4);
        TEST::SET_W(14, TEST_BREAK);

        // DIVS.L %D2, %D4:%D3
        TEST::SET_W(16, 0046100 | 2);
        TEST::SET_W(18, 0006000 | 3 << 12 | 4);
        TEST::SET_W(20, TEST_BREAK);
        jit_compile(0, 22);
    }};
BOOST_FIXTURE_TEST_SUITE(DIVS, Prepare, *boost::unit_test::fixture<F_DIVS>())


BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(value) {
    cpu.D[3] = -40;
    cpu.D[2] = -3;
    run_test(0);
    BOOST_TEST(cpu.D[3] == (0xffff0000 | 13));
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[3] = -40;
    cpu.D[2] = 3;
    run_test(0);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[3] = -2;
    cpu.D[2] = 3;
    run_test(0);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(V1) {
    cpu.D[3] = -400000;
    cpu.D[2] = 2;
    run_test(0);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    cpu.D[3] = 0x80000000;
    cpu.D[2] = -1;
    run_test(0);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(Div0) {
    cpu.D[3] = -1;
    cpu.D[2] = 0;
    run_test(0);
    BOOST_TEST(cpu.ex_n == EXCAPTION_NUMBER::DIV0);
}


BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(nomod) {
    cpu.D[3] = -200000;
    cpu.D[2] = -7;
    run_test(4);
    BOOST_TEST(cpu.D[3] == 28571);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(value) {
    cpu.D[3] = -200000;
    cpu.D[2] = 7;
    run_test(10);
    BOOST_TEST(cpu.D[3] == -28571);
    BOOST_TEST(cpu.D[4] == -3);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[3] = -10;
    cpu.D[2] = 2;
    run_test(10);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[3] = 0;
    cpu.D[2] = 2;
    run_test(10);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(V) {
    cpu.D[3] = 0x80000000;
    cpu.D[2] = -1;
    run_test(10);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(Div0) {
    cpu.D[3] = 1;
    cpu.D[2] = 0;
    run_test(10);
    BOOST_TEST(cpu.ex_n == EXCAPTION_NUMBER::DIV0);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Quad)

BOOST_AUTO_TEST_CASE(value) {
    cpu.D[4] = 2;
    cpu.D[3] = 0x540BE400;
    cpu.D[2] = -9;
    run_test(16);
    BOOST_TEST(cpu.D[3] == -1111111111);
    BOOST_TEST(cpu.D[4] == 1);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[4] = -1;
    cpu.D[3] = 0;
    cpu.D[2] = 256;
    run_test(16);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[4] = 0;
    cpu.D[3] = 1;
    cpu.D[2] = 2;
    run_test(16);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(V) {
    cpu.D[4] = 2;
    cpu.D[3] = 0;
    cpu.D[2] = -1;
    run_test(16);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(Div0) {
    cpu.D[4] = 0;
    cpu.D[3] = -100;
    cpu.D[2] = 0;
    run_test(16);
    BOOST_TEST(cpu.ex_n == EXCAPTION_NUMBER::DIV0);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
