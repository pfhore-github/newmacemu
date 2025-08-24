#define BOOST_TEST_DYN_LINK
#include "68040.hpp"

#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_ORI {
    F_ORI() {
        // ORI.B #0x1f, %D2
        TEST::SET_W(0, 0000000 | 2);
        TEST::SET_W(2, 0x1f);
        TEST::SET_W(4, TEST_BREAK);

        // ORI.B #0, %D2
        TEST::SET_W(6, 0000000 | 2);
        TEST::SET_W(8, 0);
        TEST::SET_W(10, TEST_BREAK);

        // ORI.W #0x1fff, %D2
        TEST::SET_W(12, 0000100 | 2);
        TEST::SET_W(14, 0x1fff);
        TEST::SET_W(16, TEST_BREAK);

        // ORI.W #0, %D2
        TEST::SET_W(18, 0000100 | 2);
        TEST::SET_W(20, 0);
        TEST::SET_W(22, TEST_BREAK);

        // ORI.L #0x1fffffff, %D2
        TEST::SET_W(24, 0000200 | 2);
        TEST::SET_L(26, 0x1fffffff);
        TEST::SET_W(30, TEST_BREAK);

        // ORI.L #0, %D2
        TEST::SET_W(32, 0000200 | 2);
        TEST::SET_L(34, 0);
        TEST::SET_W(38, TEST_BREAK);

        // ORI #0x1F, %CCR
        TEST::SET_W(40, 0000074);
        TEST::SET_W(42, 0x1F);
        TEST::SET_W(44, TEST_BREAK);

        // ORI #0x0700, %SR
        TEST::SET_W(46, 0000174);
        TEST::SET_W(48, 0x0700);
        TEST::SET_W(50, TEST_BREAK);
        jit_compile(0, 52);
    }};
BOOST_FIXTURE_TEST_SUITE(ORI, Prepare, *boost::unit_test::fixture<F_ORI>())



BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(value) {
    cpu.D[2] = 0x20;
    run_test(0);
    BOOST_TEST(cpu.D[2] == 0x3f);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
}
BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
    run_test(6);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 0x80;
    run_test(6);
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(value) {
    cpu.D[2] = 0x2000;
    run_test(12);
    BOOST_TEST(cpu.D[2] == 0x3fff);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
}
BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
    run_test(18);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 0x8000;
    run_test(18);
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(value) {
    cpu.D[2] = 0x20000000;
    run_test(24);
    BOOST_TEST(cpu.D[2] == 0x3fffffff);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
}
BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
    run_test(32);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 0x80000000;
    run_test(32);
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(CCR) {
    cpu.X = cpu.N = cpu.Z = cpu.V = cpu.C = false;
    run_test(40);
    BOOST_TEST(cpu.X);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(cpu.N);
    BOOST_TEST(cpu.V);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_SUITE(SR)
BOOST_AUTO_TEST_CASE(NG) {
    cpu.S = false;
	run_test(46,  EXCEPTION_NUMBER::PRIV_ERR);
}
BOOST_AUTO_TEST_CASE(OK) {
    cpu.S = true;
    cpu.T = 0;
    run_test(46);
    BOOST_TEST(cpu.I == 7);
}

BOOST_AUTO_TEST_CASE(Traced) {
    cpu.S = true;
    cpu.T = 1;
	run_test(46, EXCEPTION_NUMBER::TRACE );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
