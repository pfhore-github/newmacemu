#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "inline.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_OR {
    F_OR() {
        // OR.B %D2, %D3
        TEST::SET_W(0, 0100000 | 3 << 9 | 2);
        TEST::SET_W(2, TEST_BREAK);

        // OR.W %D2, %D3
        TEST::SET_W(4, 0100100 | 3 << 9 | 2);
        TEST::SET_W(6, TEST_BREAK);

        // OR.L %D2, %D3
        TEST::SET_W(8, 0100200 | 3 << 9 | 2);
        TEST::SET_W(10, TEST_BREAK);

        // OR.B %D2, (%A3)
        TEST::SET_W(12, 0100420 | 3 << 9 | 2);
        TEST::SET_W(14, TEST_BREAK);

        // OR.W %D2, (%A3)
        TEST::SET_W(16, 0100520 | 3 << 9 | 2);
        TEST::SET_W(18, TEST_BREAK);

        // OR.L %D2, (%A3)
        TEST::SET_W(20, 0100620 | 3 << 9 | 2);
        TEST::SET_W(22, TEST_BREAK);
        jit_compile(0, 24);
    }};
BOOST_FIXTURE_TEST_SUITE(OR, Prepare, *boost::unit_test::fixture<F_OR>())


BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_SUITE(ToReg)
BOOST_AUTO_TEST_CASE(value) {
    cpu.D[3] = 0x30;
    cpu.D[2] = 0x2F;
    run_test(0);
    BOOST_TEST(cpu.D[3] == 0x3F);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_CASE(n) {
    cpu.D[3] = 0x80;
    cpu.D[2] = 0;
    run_test(0);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(z) {
    cpu.D[3] = 0;
    cpu.D[2] = 0;
    run_test(0);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(ToMem)

BOOST_AUTO_TEST_CASE(value) {
    cpu.D[3] = 0x2F;
    cpu.A[2] = 0x1000;
    RAM[0x1000] = 0x30;
    run_test(12);
    BOOST_TEST(RAM[0x1000] == 0x3F);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_CASE(n) {
    cpu.D[3] = 0;
    cpu.A[2] = 0x1000;
    RAM[0x1000] = 0x80;
    run_test(12);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(z) {
    cpu.D[3] = 0;
    cpu.A[2] = 0x1000;
    RAM[0x1000] = 0;
    run_test(12);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_SUITE(ToReg)

BOOST_AUTO_TEST_CASE(value) {
    cpu.D[3] = 0x3030;
    cpu.D[2] = 0x2f2f;
    run_test(4);
    BOOST_TEST(cpu.D[3] == 0x3F3F);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
}
BOOST_AUTO_TEST_CASE(n) {
    cpu.D[3] = 0x8000;
    cpu.D[2] = 0;
    run_test(4);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(z) {
    cpu.D[3] = 0;
    cpu.D[2] = 0;
    run_test(4);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(ToMem)
BOOST_AUTO_TEST_CASE(value) {
    cpu.D[3] = 0x2F2F;
    TEST::SET_W(0x1000, 0x3030);
    cpu.A[2] = 0x1000;
    run_test(16);
    BOOST_TEST(TEST::GET_W(0x1000) == 0x3F3F);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_CASE(n) {
    cpu.D[3] = 0;
    TEST::SET_W(0x1000, 0x8000);
    cpu.A[2] = 0x1000;
    run_test(16);
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_CASE(z) {
    cpu.D[3] = 0;
    TEST::SET_W(0x1000, 0);
    cpu.A[2] = 0x1000;
    run_test(16);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_SUITE(ToReg)

BOOST_AUTO_TEST_CASE(value) {
    cpu.D[3] = 0x30303030;
    cpu.D[2] = 0x2f2f2f2f;
    run_test(8);
    BOOST_TEST(cpu.D[3] == 0x3f3f3f3f);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
}
BOOST_AUTO_TEST_CASE(n) {
    cpu.D[3] = 0x80000000;
    cpu.D[2] = 0;
    run_test(8);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(z) {
    cpu.D[3] = 0;
    cpu.D[2] = 0;
    run_test(8);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(ToMem)
BOOST_AUTO_TEST_CASE(value) {
    cpu.D[3] = 0x2F2F2F2F;
    TEST::SET_L(0x1000, 0x30303030);
    cpu.A[2] = 0x1000;
    run_test(20);
    BOOST_TEST(TEST::GET_L(0x1000) == 0x3F3F3F3F);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
}
BOOST_AUTO_TEST_CASE(n) {
    cpu.D[3] = 0;
    TEST::SET_L(0x1000, 0x80000000);
    cpu.A[2] = 0x1000;
    run_test(20);
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_CASE(z) {
    cpu.D[3] = 0;
    TEST::SET_L(0x1000, 0);
    cpu.A[2] = 0x1000;
    run_test(20);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
