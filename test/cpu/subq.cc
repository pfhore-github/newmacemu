#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_SUBQ {
    F_SUBQ() {
        // SUBQ.B #3, %D2
        TEST::SET_W(0, 0050400 | 3 << 9 | 2);
        TEST::SET_W(2, TEST_BREAK);

        // SUBQ.B #8, %D2
        TEST::SET_W(4, 0050400 | 0 << 9 | 2);
        TEST::SET_W(6, TEST_BREAK);

        // SUBQ.W #3, %D2
        TEST::SET_W(8, 0050500 | 3 << 9 | 2);
        TEST::SET_W(10, TEST_BREAK);

        // SUBQ.W #8, %D2
        TEST::SET_W(12, 0050500 | 0 << 9 | 2);
        TEST::SET_W(14, TEST_BREAK);

        // SUBQ.L #3, %D2
        TEST::SET_W(16, 0050600 | 3 << 9 | 2);
        TEST::SET_W(18, TEST_BREAK);

        // SUBQ.L #8, %D2
        TEST::SET_W(20, 0050600 | 0 << 9 | 2);
        TEST::SET_W(22, TEST_BREAK);

         // SUBQ.W #3, %A2
        TEST::SET_W(24, 0050510 | 3 << 9 | 2);
        TEST::SET_W(26, TEST_BREAK);

        // SUBQ.W #8, %A2
        TEST::SET_W(28, 0050510 | 0 << 9 | 2);
        TEST::SET_W(30, TEST_BREAK);

        // SUBQ.L #3, %A2
        TEST::SET_W(32, 0050610 | 3 << 9 | 2);
        TEST::SET_W(34, TEST_BREAK);

        // SUBQ.L #8, %A2
        TEST::SET_W(36, 0050610 | 0 << 9 | 2);
        TEST::SET_W(38, TEST_BREAK);
        
        jit_compile(0, 40);
    }};
BOOST_FIXTURE_TEST_SUITE(SUBQ, Prepare, *boost::unit_test::fixture<F_SUBQ>())


BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(value) {
    cpu.D[2] = 21;
    run_test(0);
    BOOST_TEST(cpu.D[2] == 18);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}


BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 3;
    run_test(0);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = -1;
    run_test(0);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(V) {
    cpu.D[2] = -126;
    run_test(0);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[2] = 2;
    run_test(0);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(eight) {
    cpu.D[2] = 21;
    run_test(4);
    BOOST_TEST(cpu.D[2] == 13);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(value) {
    cpu.D[2] = 2000;
    run_test(8);
    BOOST_TEST(cpu.D[2] == 1997);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}


BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 3;
    run_test(8);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = -1;
    run_test(8);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(V) {
    cpu.D[2] = -32766;
    run_test(8);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[2] = 2;
    run_test(8);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_CASE(eight) {
    cpu.D[2] = 2100;
    run_test(12);
    BOOST_TEST(cpu.D[2] == 2092);
}

BOOST_AUTO_TEST_CASE(An) {
    cpu.A[2] = 2100;
    run_test(24);
    BOOST_TEST(cpu.A[2] == 2097);
}

BOOST_AUTO_TEST_CASE(An_8) {
    cpu.A[2] = 2100;
    run_test(28);
    BOOST_TEST(cpu.A[2] == 2092);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(value) {
    cpu.D[2] = 142046;
    run_test(16);
    BOOST_TEST(cpu.D[2] == 142043);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}


BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 3;
    run_test(16);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = -1;
    run_test(16);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(V) {
    cpu.D[2] = 0x80000002;
    run_test(16);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[2] = 2;
    run_test(16);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(eight) {
    cpu.D[2] = 200009;
    run_test(20);
    BOOST_TEST(cpu.D[2] == 200001);
}
BOOST_AUTO_TEST_CASE(An) {
    cpu.A[2] = 210000;
    run_test(32);
    BOOST_TEST(cpu.A[2] == 209997);
}

BOOST_AUTO_TEST_CASE(An_8) {
    cpu.A[2] = 210000;
    run_test(36);
    BOOST_TEST(cpu.A[2] == 209992);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
