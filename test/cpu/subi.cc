#define BOOST_TEST_DYN_LINK
#include "68040.hpp"

#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;

struct F_SUBI {
    F_SUBI() {
        // SUBI.B #100, %D2
        TEST::SET_W(0, 0002000 | 2);
        TEST::SET_W(2, 100);
        TEST::SET_W(4, TEST_BREAK);

        // SUBI.W #30000, %D2
        TEST::SET_W(6, 0002100 | 2);
        TEST::SET_W(8, 30000);
        TEST::SET_W(10, TEST_BREAK);

        // SUBI.L #1000000000, %D2
        TEST::SET_W(12, 0002200 | 2);
        TEST::SET_L(14, 1000000000);
        TEST::SET_W(18, TEST_BREAK);

        jit_compile(0, 20);
    }};
BOOST_FIXTURE_TEST_SUITE(SUBI, Prepare, *boost::unit_test::fixture<F_SUBI>())


BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(value) {
    cpu.D[2] = 120;
    run_test(0);
    BOOST_TEST(cpu.D[2] == 20);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 100;
    run_test(0);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = -10;
    run_test(0);
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[2] = 60;
    run_test(0);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_CASE(V) {
    cpu.D[2] = -120;
    run_test(0);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(value) {
    cpu.D[2] = 31000;
    run_test(6);
    BOOST_TEST(cpu.D[2] == 1000);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 30000;
    run_test(6);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = -100;
    run_test(6);
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[2] = 20000;
    run_test(6);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_CASE(V) {
    cpu.D[2] = -20000;
    run_test(6);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(value) {
    cpu.D[2] = 1100000000;
    run_test(12);
    BOOST_TEST(cpu.D[2] == 100000000);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 1000000000;
    run_test(12);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = -100;
    run_test(12);
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[2] = 900000000;
    run_test(12);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_CASE(V) {
    cpu.D[2] = -1500000000;
    run_test(12);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
