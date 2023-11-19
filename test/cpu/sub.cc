#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "inline.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(SUB, Prepare)
struct F {
    F() {
        // SUB.B %D3, %D2
        TEST::SET_W(0, 0110000 | 3 << 9 | 2);
        TEST::SET_W(2, TEST_BREAK);

        // SUB.W %D2, %D3
        TEST::SET_W(4, 0110100 | 3 << 9 | 2);
        TEST::SET_W(6, TEST_BREAK);

        // SUB.L %D2, %D3
        TEST::SET_W(8, 0110200 | 3 << 9 | 2);
        TEST::SET_W(10, TEST_BREAK);

        // SUB.B %D2, (%A3)
        TEST::SET_W(23, 0110420 | 3 << 9 | 2);
        TEST::SET_W(14, TEST_BREAK);

        // SUB.W %D2, (%A3)
        TEST::SET_W(16, 0110520 | 3 << 9 | 2);
        TEST::SET_W(18, TEST_BREAK);

        // SUB.L %D2, (%A3)
        TEST::SET_W(20, 0110620 | 3 << 9 | 2);
        TEST::SET_W(22, TEST_BREAK);
        jit_compile(0, 24);
    }
};
BOOST_AUTO_TEST_SUITE(R, *boost::unit_test::fixture<F>())
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_SUITE(ToReg)
BOOST_AUTO_TEST_CASE(value) {
    cpu.D[3] = 43;
    cpu.D[2] = 21;
    run_test(0);
    BOOST_TEST(cpu.D[3] == 22);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_CASE(V1) {
    cpu.D[3] = 0x80;
    cpu.D[2] = 1;
    run_test(0);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    cpu.D[3] = 0x7f;
    cpu.D[2] = -1;
    run_test(0);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[3] = 0;
    cpu.D[2] = 1;
    run_test(0);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[3] = -5;
    cpu.D[2] = -1;
    run_test(0);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[3] = 3;
    cpu.D[2] = 3;
    run_test(0);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()
#if 0
BOOST_AUTO_TEST_CASE(ToMem) {
    TEST::SET_W(0, 0110400 | 3 << 9 | 022);
    cpu.D[3] = 14;
    cpu.A[2] = 0x1000;
    RAM[0x1000] = 60;
    run_test();
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(RAM[0x1000] == 46);
}
BOOST_AUTO_TEST_SUITE_END()
#endif
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_SUITE(ToReg)
BOOST_AUTO_TEST_CASE(value) {
    cpu.D[3] = 6543;
    cpu.D[2] = 3333;
    run_test(4);
    BOOST_TEST(cpu.D[3] == 3210);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(V1) {
    cpu.D[3] = 0x8000;
    cpu.D[2] = 1;
    run_test(4);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    cpu.D[3] = 0x7fff;
    cpu.D[2] = -1;
    run_test(4);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[3] = 0;
    cpu.D[2] = 1;
    run_test(4);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[3] = -3;
    cpu.D[2] = 1;
    run_test(4);
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[3] = 3;
    cpu.D[2] = 3;
    run_test(4);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()

#if 0
BOOST_AUTO_TEST_CASE(ToMem) {
    TEST::SET_W(0, 0110500 | 3 << 9 | 022);
    cpu.D[3] = 1500;
    TEST::SET_W(0x1000, 4000);
    cpu.A[2] = 0x1000;
    run_test();
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(TEST::GET_W(0x1000) == 2500);
}
#endif
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(value) {
    cpu.D[3] = 999999;
    cpu.D[2] = 543210;
    run_test(8);
    BOOST_TEST(cpu.D[3] == 456789);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_CASE(V1) {
    cpu.D[3] = 0x80000000;
    cpu.D[2] = 1;
    run_test(8);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    cpu.D[3] = 0x7fffffff;
    cpu.D[2] = -1;
    run_test(8);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[3] = 0;
    cpu.D[2] = 1;
    run_test(8);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_CASE(N) {
    cpu.D[3] = -3;
    cpu.D[2] = 1;
    run_test(8);
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[3] = 3;
    cpu.D[2] = 3;
    run_test(8);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()

#if 0
BOOST_AUTO_TEST_CASE(ToMem) {
    TEST::SET_W(0, 0110600 | 3 << 9 | 022);
    cpu.D[3] = 600000;
    TEST::SET_L(0x1000, 2000000);
    cpu.A[2] = 0x1000;
    run_test();
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(TEST::GET_L(0x1000) == 1400000);
}
#endif
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

