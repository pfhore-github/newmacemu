#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(CMP2, Prepare)
struct F {
    F() {
        // CMP2.B (%A2), %D3
        TEST::SET_W(0, 0000320 | 2);
        TEST::SET_W(2, 3 << 12);
        TEST::SET_W(4, TEST_BREAK);

        // CMP2.B (%A2), %A3
        TEST::SET_W(6, 0000320 | 2);
        TEST::SET_W(8, 1 << 15 | 3 << 12);
        TEST::SET_W(10, TEST_BREAK);

        // CMP2.W (%A2), %D3
        TEST::SET_W(12, 0001320 | 2);
        TEST::SET_W(14, 3 << 12);
        TEST::SET_W(16, TEST_BREAK);

        // CMP2.W (%A2), %A3
        TEST::SET_W(18, 0001320 | 2);
        TEST::SET_W(20, 1 << 15 | 3 << 12);
        TEST::SET_W(22, TEST_BREAK);
        
        // CMP2.L (%A2), %D3
        TEST::SET_W(24, 0002320 | 2);
        TEST::SET_W(26, 3 << 12);
        TEST::SET_W(28, TEST_BREAK);

        // CMP2.L (%A2), %A3
        TEST::SET_W(30, 0002320 | 2);
        TEST::SET_W(32, 1 << 15 | 3 << 12);
        TEST::SET_W(34, TEST_BREAK);

        jit_compile(0, 36);
    }
};
BOOST_AUTO_TEST_SUITE(R, *boost::unit_test::fixture<F>())
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_SUITE(Unsigned)
BOOST_AUTO_TEST_CASE(In) {
    RAM[0x100] = 70;
    RAM[0x101] = 230;
    cpu.A[2] = 0x100;
    cpu.D[3] = 150;
    run_test(0);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(Leq) {
    RAM[0x100] = 70;
    RAM[0x101] = 230;
    cpu.A[2] = 0x100;
    cpu.D[3] = 70;
    run_test(0);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(Geq) {
    RAM[0x100] = 70;
    RAM[0x101] = 230;
    cpu.A[2] = 0x100;
    cpu.D[3] = 230;
    run_test(0);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.C);
}
BOOST_AUTO_TEST_CASE(Under) {
    RAM[0x100] = 70;
    RAM[0x101] = 230;
    cpu.A[2] = 0x100;
    cpu.D[3] = 20;
    run_test(0);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(Over) {
    RAM[0x100] = 70;
    RAM[0x101] = 230;
    cpu.A[2] = 0x100;
    cpu.D[3] = 250;
    run_test(0);
    BOOST_TEST(cpu.C);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Signed)
BOOST_AUTO_TEST_CASE(In) {
    RAM[0x100] = -70;
    RAM[0x101] = 80;
    cpu.A[2] = 0x100;
    cpu.A[3] = 10;
    run_test(6);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(Leq) {
    RAM[0x100] = -70;
    RAM[0x101] = 80;
    cpu.A[2] = 0x100;
    cpu.A[3] = -70;
    run_test(6);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(Geq) {
    RAM[0x100] = -70;
    RAM[0x101] = 80;
    cpu.A[2] = 0x100;
    cpu.A[3] = 80;
    run_test(6);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.C);
}
BOOST_AUTO_TEST_CASE(Under) {
    RAM[0x100] = -70;
    RAM[0x101] = 80;
    cpu.A[2] = 0x100;
    cpu.A[3] = -100;
    run_test(6);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(Over) {
    RAM[0x100] = -70;
    RAM[0x101] = 80;
    cpu.A[2] = 0x100;
    cpu.A[3] = 100;
    run_test(6);
    BOOST_TEST(cpu.C);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_SUITE(Unsigned)
BOOST_AUTO_TEST_CASE(In) {
    TEST::SET_W(0x100, 7000);
    TEST::SET_W(0x102, 23000);
    cpu.A[2] = 0x100;
    cpu.D[3] = 15000;
    run_test(12);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(Leq) {
    TEST::SET_W(0x100, 7000);
    TEST::SET_W(0x102, 23000);
    cpu.A[2] = 0x100;
    cpu.D[3] = 7000;
    run_test(12);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(Geq) {
    TEST::SET_W(0x100, 7000);
    TEST::SET_W(0x102, 23000);
    cpu.A[2] = 0x100;
    cpu.D[3] = 23000;
    run_test(12);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.C);
}
BOOST_AUTO_TEST_CASE(Under) {
    TEST::SET_W(0x100, 7000);
    TEST::SET_W(0x102, 23000);
    cpu.A[2] = 0x100;
    cpu.D[3] = 2000;
    run_test(12);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(Over) {
    TEST::SET_W(0x100, 7000);
    TEST::SET_W(0x102, 23000);
    cpu.A[2] = 0x100;
    cpu.D[3] = 25000;
    run_test(12);
    BOOST_TEST(cpu.C);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Signed)
BOOST_AUTO_TEST_CASE(In) {
    TEST::SET_W(0x100, -700);
    TEST::SET_W(0x102, 2300);
    cpu.A[2] = 0x100;
    cpu.A[3] = 100;
    run_test(18);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(Leq) {
    TEST::SET_W(0x100, -700);
    TEST::SET_W(0x102, 2300);
    cpu.A[2] = 0x100;
    cpu.A[3] = -700;
    run_test(18);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(Geq) {
    TEST::SET_W(0x100, -700);
    TEST::SET_W(0x102, 2300);
    cpu.A[2] = 0x100;
    cpu.A[3] = 2300;
    run_test(18);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.C);
}
BOOST_AUTO_TEST_CASE(Under) {
    TEST::SET_W(0x100, -700);
    TEST::SET_W(0x102, 2300);
    cpu.A[2] = 0x100;
    cpu.A[3] = -1000;
    run_test(18);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(Over) {
    TEST::SET_W(0x100, -700);
    TEST::SET_W(0x102, 2300);
    cpu.A[2] = 0x100;
    cpu.A[3] = 2600;
    run_test(18);
    BOOST_TEST(cpu.C);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_SUITE(Unsigned)
BOOST_AUTO_TEST_CASE(In) {
    TEST::SET_L(0x100, 700000);
    TEST::SET_L(0x104, 2300000);
    cpu.A[2] = 0x100;
    cpu.D[3] = 1500000;
    run_test(24);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(Leq) {
    TEST::SET_L(0x100, 700000);
    TEST::SET_L(0x104, 2300000);
    cpu.A[2] = 0x100;
    cpu.D[3] = 700000;
    run_test(24);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(Geq) {
    TEST::SET_L(0x100, 700000);
    TEST::SET_L(0x104, 2300000);
    cpu.A[2] = 0x100;
    cpu.D[3] = 2300000;
    run_test(24);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.C);
}
BOOST_AUTO_TEST_CASE(Under) {
    TEST::SET_L(0x100, 700000);
    TEST::SET_L(0x104, 2300000);
    cpu.A[2] = 0x100;
    cpu.D[3] = 200000;
    run_test(24);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(Over) {
    TEST::SET_L(0x100, 700000);
    TEST::SET_L(0x104, 2300000);
    cpu.A[2] = 0x100;
    cpu.D[3] = 25000000;
    run_test(24);
    BOOST_TEST(cpu.C);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Signed)
BOOST_AUTO_TEST_CASE(In) {
    TEST::SET_L(0x100, -70000);
    TEST::SET_L(0x104, 230000);
    cpu.A[2] = 0x100;
    cpu.A[3] = 10000;
    run_test(30);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(Leq) {
    TEST::SET_L(0x100, -70000);
    TEST::SET_L(0x104, 230000);
    cpu.A[2] = 0x100;
    cpu.A[3] = -70000;
    run_test(30);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(Geq) {
    TEST::SET_L(0x100, -70000);
    TEST::SET_L(0x104, 230000);
    cpu.A[2] = 0x100;
    cpu.A[3] = 230000;
    run_test(30);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.C);
}
BOOST_AUTO_TEST_CASE(Under) {
    TEST::SET_L(0x100, -70000);
    TEST::SET_L(0x104, 230000);
    cpu.A[2] = 0x100;
    cpu.A[3] = -100000;
    run_test(30);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(Over) {
    TEST::SET_L(0x100, -70000);
    TEST::SET_L(0x104, 230000);
    cpu.A[2] = 0x100;
    cpu.A[3] = 260000;
    run_test(30);
    BOOST_TEST(cpu.C);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()