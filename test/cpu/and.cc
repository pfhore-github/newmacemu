#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "inline.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_AND {
    F_AND() {
        // AND.B %D2, %D3
        TEST::SET_W(0, 0140000 | 3 << 9 | 2);
        TEST::SET_W(2, TEST_BREAK);

        // AND.W %D2, %D3
        TEST::SET_W(4, 0140100 | 3 << 9 | 2);
        TEST::SET_W(6, TEST_BREAK);

        // AND.L %D2, %D3
        TEST::SET_W(8, 0140200 | 3 << 9 | 2);
        TEST::SET_W(10, TEST_BREAK);

        // AND.B %D3, (%A2)
        TEST::SET_W(12, 0140420 | 3 << 9 | 2);
        TEST::SET_W(14, TEST_BREAK);

        // AND.W %D3, (%A2)
        TEST::SET_W(16, 0140520 | 3 << 9 | 2);
        TEST::SET_W(18, TEST_BREAK);

        // AND.L %D3, (%A2)
        TEST::SET_W(20, 0140620 | 3 << 9 | 2);
        TEST::SET_W(22, TEST_BREAK);
        jit_compile(0, 24);
    }};
BOOST_FIXTURE_TEST_SUITE(AND, Prepare, *boost::unit_test::fixture<F_AND>())


BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_SUITE(ToReg)
BOOST_AUTO_TEST_CASE(value) {
    cpu.D[3] = 0x70;
    cpu.D[2] = 0x3F;
    run_test(0);
    BOOST_TEST(cpu.D[3] == 0x30);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[3] = 0x80;
    cpu.D[2] = 0x80;
    run_test(0);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[3] = 0x80;
    cpu.D[2] = 0x7F;
    run_test(0);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(ToMem)
BOOST_AUTO_TEST_CASE(value) {
    cpu.D[3] = 0x3F;
    cpu.A[2] = 0x100;
    RAM[0x100] = 0x70;
    run_test(12);
    BOOST_TEST(RAM[0x100] == 0x30);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[3] = 0x80;
    cpu.A[2] = 0x100;
    RAM[0x100] = 0x80;
    run_test(12);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[3] = 0x7F;
    cpu.A[2] = 0x100;
    RAM[0x100] = 0x80;
    run_test(12);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_SUITE(ToReg)
BOOST_AUTO_TEST_CASE(value) {
    cpu.D[3] = 0x7F7F;
    cpu.D[2] = 0x3080;
    run_test(4);
    BOOST_TEST(cpu.D[3] == 0x3000);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[3] = 0x8000;
    cpu.D[2] = 0x8000;
    run_test(4);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[3] = 0x8000;
    cpu.D[2] = 0x7FFF;
    run_test(4);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ToMem)
BOOST_AUTO_TEST_CASE(value) {
    cpu.D[3] = 0x3FFF;
    TEST::SET_W(0x100, 0x7000);
    cpu.A[2] = 0x100;
    run_test(16);
    BOOST_TEST(TEST::GET_W(0x100) == 0x3000);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[3] = 0x8000;
    TEST::SET_W(0x100, 0x8000);
    cpu.A[2] = 0x100;
    run_test(16);
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[3] = 0x7FFF;
    TEST::SET_W(0x100, 0x8000);
    cpu.A[2] = 0x100;
    run_test(16);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_SUITE(ToReg)
BOOST_AUTO_TEST_CASE(value) {
    cpu.D[3] = 0x7FFFFFFF;
    cpu.D[2] = 0x30000000;
    run_test(8);
    BOOST_TEST(cpu.D[3] == 0x30000000);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[3] = 0x80000000;
    cpu.D[2] = 0x80000000;
    run_test(8);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[3] = 0x80000000;
    cpu.D[2] = 0x7FFFFFFF;
    run_test(8);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ToMem)
BOOST_AUTO_TEST_CASE(value) {
    cpu.D[3] = 0x3FFFFFFF;
    TEST::SET_L(0x100, 0x70000000);
    cpu.A[2] = 0x100;
    run_test(20);
    BOOST_TEST(TEST::GET_L(0x100) == 0x30000000);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
}
BOOST_AUTO_TEST_CASE(N) {
    cpu.D[3] = 0x80000000;
    TEST::SET_L(0x100, 0x80000000);
    cpu.A[2] = 0x100;
    run_test(20);
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[3] = 0x7FFFFFFF;
    TEST::SET_L(0x100, 0x80000000);
    cpu.A[2] = 0x100;
    run_test(20);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
