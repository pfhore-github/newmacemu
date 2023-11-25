#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include "inline.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_ADD {
    F_ADD() {
        // ADD.B %D3, %D2
		TEST::SET_W(0, 0150000 | 3 << 9 | 2);
        TEST::SET_W(2, TEST_BREAK);

        // ADD.W %D2, %D3
		TEST::SET_W(4, 0150100 | 3 << 9 | 2);
		TEST::SET_W(6, TEST_BREAK);

        // ADD.L %D2, %D3
        TEST::SET_W(8, 0150200 | 3 << 9 | 2);
        TEST::SET_W(10, TEST_BREAK);

        // ADD.B %D3, (%A2)
		TEST::SET_W(12, 0150420 | 3 << 9 | 2);
        TEST::SET_W(14, TEST_BREAK);

        // ADD.W %D3, (%A2)
        TEST::SET_W(16, 0150520 | 3 << 9 | 2);
        TEST::SET_W(18, TEST_BREAK);

        // ADD.L %D3, (%A2)
        TEST::SET_W(20, 0150620 | 3 << 9 | 2);
        TEST::SET_W(22, TEST_BREAK);
        jit_compile(0, 24);
    }
};
BOOST_FIXTURE_TEST_SUITE(ADD, Prepare, *boost::unit_test::fixture<F_ADD>())

BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_SUITE(ToReg)
BOOST_AUTO_TEST_CASE(value) {
    cpu.D[3] = 14;
    cpu.D[2] = 21;
    run_test(0);
    BOOST_TEST(cpu.D[3] == 35);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.V);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[3] = 0;
    cpu.D[2] = 0;
    run_test(0);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[3] = -3;
    cpu.D[2] = 1;
    run_test(0);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(V1) {
    cpu.D[3] = 0x7f;
    cpu.D[2] = 1;
    run_test(0);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    cpu.D[3] = 0x80;
    cpu.D[2] = -1;
    run_test(0);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[3] = 0xff;
    cpu.D[2] = 1;
    run_test(0);
    ADD_B(0xff, 1);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ToMem)

BOOST_AUTO_TEST_CASE(value) {
    cpu.D[3] = 14;
    cpu.A[2] = 0x100;
    RAM[0x100] = 21;
    run_test(12);
    BOOST_TEST(RAM[0x100] == 35);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[3] = -4;
    cpu.A[2] = 0x100;
    RAM[0x100] = 1;
    run_test(12);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[3] = 0;
    cpu.A[2] = 0x100;
    RAM[0x100] = 0;
    run_test(12);
    BOOST_TEST(cpu.Z);
}


BOOST_AUTO_TEST_CASE(V) {
    cpu.D[3] = 0x7f;
    cpu.A[2] = 0x100;
    RAM[0x100] = 1;
    run_test(12);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[3] = 0xff;
    cpu.A[2] = 0x100;
    RAM[0x100] = 1;
    run_test(12);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_SUITE(ToReg)
BOOST_AUTO_TEST_CASE(value) {
    cpu.D[3] = 1420;
    cpu.D[2] = 2133;
    run_test(4);
    BOOST_TEST(cpu.D[3] == 3553);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[3] = 0;
    cpu.D[2] = 0;
    run_test(4);
    BOOST_TEST(cpu.Z);
}


BOOST_AUTO_TEST_CASE(V1) {
    cpu.D[3] = 0x7fff;
    cpu.D[2] = 1;
    run_test(4);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    cpu.D[3] = 0x8000;
    cpu.D[2] = -1;
    run_test(4);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[3] = 0xffff;
    cpu.D[2] = 1;
    run_test(4);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(ToMem)
BOOST_AUTO_TEST_CASE(value) {
    cpu.D[3] = 1420;
    TEST::SET_W(0x100, 2133);
    cpu.A[2] = 0x100;
    run_test(16);
    BOOST_TEST(TEST::GET_W(0x100) == 3553);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}
BOOST_AUTO_TEST_CASE(N) {
    cpu.D[3] = -4;
    TEST::SET_W(0x100, 1);
    cpu.A[2] = 0x100;
    run_test(16);
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[3] = 0;
    TEST::SET_W(0x100, 0);
    cpu.A[2] = 0x100;
    run_test(16);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(V) {
    cpu.D[3] = 0x7fff;
    TEST::SET_W(0x100, 1);
    cpu.A[2] = 0x100;
    run_test(16);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[3] = 0xffff;
    TEST::SET_W(0x100, 1);
    cpu.A[2] = 0x100;
    run_test(16);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_SUITE(ToReg)
BOOST_AUTO_TEST_CASE(value) {
    cpu.D[3] = 142044;
    cpu.D[2] = 213324;
    run_test(8);
    BOOST_TEST(cpu.D[3] == 355368);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}
BOOST_AUTO_TEST_CASE(N) {
    cpu.D[3] = -4;
    cpu.D[2] = 2;
    run_test(8);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[3] = 0;
    cpu.D[2] = 0;
    run_test(8);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(V1) {
    cpu.D[3] = 0x7fffffff;
    cpu.D[2] = 1;
    run_test(8);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    cpu.D[3] = 0x80000000;
    cpu.D[2] = -1;
    run_test(8);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[3] = 0xffffffff;
    cpu.D[2] = 1;
    run_test(8);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(ToMem)
BOOST_AUTO_TEST_CASE(value) {
    cpu.D[3] = 142044;
    TEST::SET_L(0x100, 213324);
    cpu.A[2] = 0x100;
    run_test(20);
    BOOST_TEST(TEST::GET_L(0x100) == 355368);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[3] = -3;
    TEST::SET_L(0x100, 1);
    cpu.A[2] = 0x100;
    run_test(20);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[3] = 0;
    TEST::SET_L(0x100, 0);
    cpu.A[2] = 0x100;
    run_test(20);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(V) {
    cpu.D[3] = 0x7fffffff;
    TEST::SET_L(0x100, 1);
    cpu.A[2] = 0x100;
    run_test(20);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[3] = 0xffffffff;
    TEST::SET_L(0x100, 1);
    cpu.A[2] = 0x100;
    run_test(20);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
