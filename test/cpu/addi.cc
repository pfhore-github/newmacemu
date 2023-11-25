#define BOOST_TEST_DYN_LINK
#include "68040.hpp"

#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;

struct F_ADDI {
    F_ADDI() {
        // ADDI.B #100, %D2
        TEST::SET_W(0, 0003000 | 2);
        TEST::SET_W(2, 100);
        TEST::SET_W(4, TEST_BREAK);

        // ADDI.W #30000, %D2
        TEST::SET_W(6, 0003100 | 2);
        TEST::SET_W(8, 30000);
        TEST::SET_W(10, TEST_BREAK);

        // ADDI.W #0x70000000, %D2
        TEST::SET_W(12, 0003200 | 2);
        TEST::SET_L(14, 0x70000000);
        TEST::SET_W(18, TEST_BREAK);

        jit_compile(0, 20);
    }
};
BOOST_FIXTURE_TEST_SUITE(ADDI, Prepare, *boost::unit_test::fixture<F_ADDI>())

BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(value) {
    cpu.D[2] = 20;
    run_test(0);
    BOOST_TEST(cpu.D[2] == 120);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
}
BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = -100;
    run_test(0);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = -110;
    run_test(0);
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[2] = -92;
    run_test(0);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_CASE(V) {
    cpu.D[2] = 60;
    run_test(0);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(value) {
    cpu.D[2] = 2000;
    run_test(6);
    BOOST_TEST(cpu.D[2] == 32000);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = -30000;
    run_test(6);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = -32000;
    run_test(6);
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[2] = -12000;
    run_test(6);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_CASE(V) {
    cpu.D[2] = 10000;
    run_test(6);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(value) {
    cpu.D[2] = 0x05000000;
    run_test(12);
    BOOST_TEST(cpu.D[2] == 0x75000000);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = -0x70000000;
    run_test(12);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = -0x72000000;
    run_test(12);
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[2] = -0x32000000;
    run_test(12);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_CASE(V) {
    cpu.D[2] = 0x30000000;
    run_test(12);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
