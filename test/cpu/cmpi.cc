#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(CMPI, Prepare)
struct F {
    F() {
        // CMPI.B #100, %D2 
        TEST::SET_W(0, 0006000 | 2);
        TEST::SET_W(2, 100);
        TEST::SET_W(4, TEST_BREAK);

        // CMPI.W #30000, %D2 
        TEST::SET_W(6, 0006100 | 2);
        TEST::SET_W(8, 30000);
        TEST::SET_W(10, TEST_BREAK);

        // CMPI.L #1000000000, %D2 
        TEST::SET_W(12, 0006200 | 2);
        TEST::SET_L(14, 1000000000);
        TEST::SET_W(18, TEST_BREAK);

        jit_compile(0, 20);
    }
};
BOOST_AUTO_TEST_SUITE(R, *boost::unit_test::fixture<F>())
BOOST_AUTO_TEST_SUITE(Byte)

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
BOOST_AUTO_TEST_CASE(C) {
    cpu.D[2] = 60;
    run_test(0);
    BOOST_TEST(cpu.C);
}
BOOST_AUTO_TEST_CASE(V) {
    cpu.D[2] = -120;
    run_test(0);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

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
BOOST_AUTO_TEST_CASE(C) {
    cpu.D[2] = 20000;
    run_test(6);
    BOOST_TEST(cpu.C);
}
BOOST_AUTO_TEST_CASE(V) {
    cpu.D[2] = -20000;
    run_test(6);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)


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
BOOST_AUTO_TEST_CASE(C) {
    cpu.D[2] = 900000000;
    run_test(12);
    BOOST_TEST(cpu.C);
}
BOOST_AUTO_TEST_CASE(V) {
    cpu.D[2] = -1500000000;
    run_test(12);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
