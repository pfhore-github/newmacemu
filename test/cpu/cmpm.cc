#define BOOST_TEST_DYN_LINK
#include "inline.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_CMPM {
    F_CMPM() {
        // CMPM.B (%A1)+, (%A3)+
        TEST::SET_W(0, 0130410 | 3 << 9 | 1);
        TEST::SET_W(2, TEST_BREAK);

        // CMPM.W (%A1)+, (%A3)+
        TEST::SET_W(4, 0130510 | 3 << 9 | 1);
        TEST::SET_W(6, TEST_BREAK);

        // CMPM.L (%A1)+, (%A3)+
        TEST::SET_W(8, 0130610 | 3 << 9 | 1);
        TEST::SET_W(10, TEST_BREAK);

        jit_compile(0, 12);
    }};
BOOST_FIXTURE_TEST_SUITE(CMPM, Prepare, *boost::unit_test::fixture<F_CMPM>())



BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(Z) {
    RAM[0x100] = 0x20;
    RAM[0x110] = 0x20;
    cpu.A[3] = 0x100;
    cpu.A[1] = 0x110;
    run_test(0);
    BOOST_TEST(cpu.A[3] == 0x101);
    BOOST_TEST(cpu.A[1] == 0x111);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(N) {
    RAM[0x100] = -4;
    RAM[0x110] = 1;
    cpu.A[3] = 0x100;
    cpu.A[1] = 0x110;
    run_test(0);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(V) {
    RAM[0x100] = 0x80;
    RAM[0x110] = 1;
    cpu.A[3] = 0x100;
    cpu.A[1] = 0x110;
    run_test(0);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(C) {
    RAM[0x100] = 0;
    RAM[0x110] = 1;
    cpu.A[3] = 0x100;
    cpu.A[1] = 0x110;
    run_test(0);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0x100, 1200);
    TEST::SET_W(0x110, 1200);
    cpu.A[3] = 0x100;
    cpu.A[1] = 0x110;
    run_test(4);
    BOOST_TEST(cpu.A[3] == 0x102);
    BOOST_TEST(cpu.A[1] == 0x112);
    BOOST_TEST(cpu.Z);
}


BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0x100, -400);
    TEST::SET_W(0x110, 100);
    cpu.A[3] = 0x100;
    cpu.A[1] = 0x110;
    run_test(4);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(V) {
    TEST::SET_W(0x100, 0x8000);
    TEST::SET_W(0x110, 1);
    cpu.A[3] = 0x100;
    cpu.A[1] = 0x110;
    run_test(4);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(C) {
    TEST::SET_W(0x100, 0);
    TEST::SET_W(0x110, 1);
    cpu.A[3] = 0x100;
    cpu.A[1] = 0x110;
    run_test(4);
    BOOST_TEST(cpu.C);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_L(0x100, 120000);
    TEST::SET_L(0x110, 120000);
    cpu.A[3] = 0x100;
    cpu.A[1] = 0x110;
    run_test(8);
    BOOST_TEST(cpu.A[3] == 0x104);
    BOOST_TEST(cpu.A[1] == 0x114);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_L(0x100, -400000);
    TEST::SET_L(0x110, 100000);
    cpu.A[3] = 0x100;
    cpu.A[1] = 0x110;
    run_test(8);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(V) {
    TEST::SET_L(0x100, 0x80000000);
    TEST::SET_L(0x110, 1);
    cpu.A[3] = 0x100;
    cpu.A[1] = 0x110;
    run_test(8);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(C) {
    TEST::SET_L(0x100, 0);
    TEST::SET_L(0x110, 1);
    cpu.A[3] = 0x100;
    cpu.A[1] = 0x110;
    run_test(8);
    BOOST_TEST(cpu.C);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
