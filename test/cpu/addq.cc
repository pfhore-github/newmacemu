#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_ADDQ {
    F_ADDQ() {
        // ADDQ.B #3, %D2
        TEST::SET_W(0, 0050000 | 3 << 9 | 2);
        TEST::SET_W(2, TEST_BREAK);

        // ADDQ.B #8, %D2
        TEST::SET_W(4, 0050000 | 0 << 9 | 2);
        TEST::SET_W(6, TEST_BREAK);

        // ADDQ.W #3, %D2
        TEST::SET_W(8, 0050100 | 3 << 9 | 2);
        TEST::SET_W(10, TEST_BREAK);

        // ADDQ.W #8, %D2
        TEST::SET_W(12, 0050100 | 0 << 9 | 2);
        TEST::SET_W(14, TEST_BREAK);

        // ADDQ.L #3, %D2
        TEST::SET_W(16, 0050200 | 3 << 9 | 2);
        TEST::SET_W(18, TEST_BREAK);

        // ADDQ.L #8, %D2
        TEST::SET_W(20, 0050200 | 0 << 9 | 2);
        TEST::SET_W(22, TEST_BREAK);

        // ADDQ.W #3, %A2
        TEST::SET_W(24, 0050110 | 3 << 9 | 2);
        TEST::SET_W(26, TEST_BREAK);

        // ADDQ.W #8, %A2
        TEST::SET_W(28, 0050110 | 0 << 9 | 2);
        TEST::SET_W(30, TEST_BREAK);

        // ADDQ.L #3, %A2
        TEST::SET_W(32, 0050210 | 3 << 9 | 2);
        TEST::SET_W(34, TEST_BREAK);

        // ADDQ.L #8, %A2
        TEST::SET_W(36, 0050210 | 0 << 9 | 2);
        TEST::SET_W(38, TEST_BREAK);

        jit_compile(0, 40);
    }
};
BOOST_FIXTURE_TEST_SUITE(ADDQ, Prepare, *boost::unit_test::fixture<F_ADDQ>())
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(value) {
    cpu.D[2] = 21;
    run_test(0);
    BOOST_TEST(cpu.D[2] == 24);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = -3;
    run_test(0);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = -8;
    run_test(0);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(V) {
    cpu.D[2] = 126;
    run_test(0);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[2] = -2;
    run_test(0);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(eight) {
    cpu.D[2] = 21;
    run_test(4);
    BOOST_TEST(cpu.D[2] == 29);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(value) {
    cpu.D[2] = 2000;
    run_test(8);
    BOOST_TEST(cpu.D[2] == 2003);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = -3;
    run_test(8);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = -8;
    run_test(8);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(V) {
    cpu.D[2] = 32765;
    run_test(8);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[2] = -2;
    run_test(8);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}



BOOST_AUTO_TEST_CASE(eight) {
    cpu.D[2] = 2100;
    run_test(12);
    BOOST_TEST(cpu.D[2] == 2108);
}

BOOST_AUTO_TEST_CASE(An) {
    cpu.A[2] = 2100;
    run_test(24);
    BOOST_TEST(cpu.A[2] == 2103);
}

BOOST_AUTO_TEST_CASE(An_8) {
    cpu.A[2] = 2100;
    run_test(28);
    BOOST_TEST(cpu.A[2] == 2108);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(value) {
    cpu.D[2] = 100000;
    run_test(16);
    BOOST_TEST(cpu.D[2] == 100003);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = -3;
    run_test(16);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = -8;
    run_test(16);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(V) {
    cpu.D[2] = 0x7ffffffe;
    run_test(16);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[2] = -2;
    run_test(16);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_CASE(eight) {
    cpu.D[2] = 142044;
    run_test(20);
    BOOST_TEST(cpu.D[2] == 142052);
}

BOOST_AUTO_TEST_CASE(An) {
    cpu.A[2] = 210000;
    run_test(32);
    BOOST_TEST(cpu.A[2] == 210003);
}

BOOST_AUTO_TEST_CASE(An_8) {
    cpu.A[2] = 210000;
    run_test(36);
    BOOST_TEST(cpu.A[2] == 210008);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
