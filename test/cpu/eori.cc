#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;

struct F_EORI {
    F_EORI() {
        // EORI.B #0x40, %D2
        TEST::SET_W(0, 0005000 | 2);
        TEST::SET_W(2, 0x40);
        TEST::SET_W(4, TEST_BREAK);

        // EORI.B #0xff, %D2
        TEST::SET_W(6, 0005000 | 2);
        TEST::SET_W(8, 0xff);
        TEST::SET_W(10, TEST_BREAK);

        // EORI.W #0xff00, %D2
        TEST::SET_W(12, 0005100 | 2);
        TEST::SET_W(14, 0x0ff0);
        TEST::SET_W(16, TEST_BREAK);

        // EORI.W #0xffff, %D2
        TEST::SET_W(18, 0005100 | 2);
        TEST::SET_W(20, 0xffff);
        TEST::SET_W(22, TEST_BREAK);

        // EORI.L #0x00ff0000, %D2
        TEST::SET_W(24, 0005200 | 2);
        TEST::SET_L(26, 0x00ff0000);
        TEST::SET_W(30, TEST_BREAK);

        // EORI.L #0xffffffff, %D2
        TEST::SET_W(32, 0005200 | 2);
        TEST::SET_L(34, 0xffffffff);
        TEST::SET_W(38, TEST_BREAK);

        // EORI #ff, %CCR
        TEST::SET_W(40, 0005074);
        TEST::SET_W(42, 0xff);
        TEST::SET_W(44, TEST_BREAK);

        // EORI #0x7000, %SR
        TEST::SET_W(46, 0005174);
        TEST::SET_W(48, 0x0700);
        TEST::SET_W(50, TEST_BREAK);
        jit_compile(0, 52);
    }};
BOOST_FIXTURE_TEST_SUITE(EORI, Prepare, *boost::unit_test::fixture<F_EORI>())


BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(value) {
    cpu.D[2] = 0x6F;
    run_test(0);
    BOOST_TEST(cpu.D[2] == 0x2F);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
}
BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0xff;
    run_test(6);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 0;
    run_test(6);
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(value) {
    cpu.D[2] = 0x4fff;
    run_test(12);
    BOOST_TEST(cpu.D[2] == 0x400f);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
}
BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0xffff;
    run_test(18);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 0;
    run_test(18);
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_SUITE_END()



BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(value) {
    cpu.D[2] = 0x20ff2233;
    run_test(24);
    BOOST_TEST(cpu.D[2] == 0x20002233);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
}
BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0xffffffff;
    run_test(32);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 0x12345678;
    run_test(32);
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(CCR) {
    cpu.X = cpu.N = cpu.Z = cpu.V = cpu.C = true;
    run_test(40);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_SUITE(SR)
BOOST_AUTO_TEST_CASE(NG) {
    cpu.S = false;
	run_test(46);
	BOOST_TEST( cpu.ex_n == 8 );
}

BOOST_AUTO_TEST_CASE(OK) {
    cpu.S = true;
    cpu.I = 0;
    cpu.T = 0;
    run_test(46);
    BOOST_TEST(cpu.I == 7);
}

BOOST_AUTO_TEST_CASE(Traced) {
    cpu.S = true;
    cpu.T = 1;
	run_test(46);
	BOOST_TEST(cpu.ex_n == 9 );
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
