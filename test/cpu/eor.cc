#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "inline.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(EOR, Prepare)
struct F {
    F() {
        // EOR.B %D3, %D2
        TEST::SET_W(0, 0130400 | 3 << 9 | 2);
        TEST::SET_W(2, TEST_BREAK);

        // EOR.W %D3, %D2
        TEST::SET_W(4, 0130500 | 3 << 9 | 2);
        TEST::SET_W(6, TEST_BREAK);

        // EOR.L %D3, %D2
        TEST::SET_W(8, 0130600 | 3 << 9 | 2);
        TEST::SET_W(10, TEST_BREAK);

        jit_compile(0, 12);
    }
};
BOOST_AUTO_TEST_SUITE(R, *boost::unit_test::fixture<F>())
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(value) {
    cpu.D[3] = 0xED;
    cpu.D[2] = 0xFF;
    run_test(0);
    BOOST_TEST(cpu.D[2] == 0x12);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[3] = 0;
    cpu.D[2] = 0x80;
    run_test(0);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[3] = 0x40;
    cpu.D[2] = 0x40;
    run_test(0);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(value) {
    cpu.D[3] = 0xffff;
    cpu.D[2] = 0xeded;
    run_test(4);
    BOOST_TEST(cpu.D[2] == 0x1212);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[3] = 0;
    cpu.D[2] = 0x8000;
    run_test(4);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[3] = 0x4000;
    cpu.D[2] = 0x4000;
    run_test(4);
    BOOST_TEST(cpu.Z);
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(value) {
    cpu.D[3] = 0xffffffff;
    cpu.D[2] = 0xedededed;
    run_test(8);
    BOOST_TEST(cpu.D[2] == 0x12121212);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
}


BOOST_AUTO_TEST_CASE(N) {
    cpu.D[3] = 0;
    cpu.D[2] = 0x80000000;
    run_test(8);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[3] = 0x40000000;
    cpu.D[2] = 0x40000000;
    run_test(8);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
