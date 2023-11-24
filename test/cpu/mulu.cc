#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(MULU, Prepare)
struct F {
    F() {
        // MULU.W %D2, %D3
        TEST::SET_W(0, 0140300 | 3 << 9 | 2);
        TEST::SET_W(2, TEST_BREAK);

        // MULU.L %D2, %D3
        TEST::SET_W(4, 0046000 | 2);
        TEST::SET_W(6, 0000000 | 3 << 12);
        TEST::SET_W(8, TEST_BREAK);

        // MULU.L %D2, %D3-%D4
        TEST::SET_W(10, 0046000 | 2);
        TEST::SET_W(12, 0002000 | 3 << 12 | 4);
        TEST::SET_W(14, TEST_BREAK);
        jit_compile(0, 16);
    }
};
BOOST_AUTO_TEST_SUITE(R, *boost::unit_test::fixture<F>())
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(value) { 
    cpu.D[3] = 70;
    cpu.D[2] = 30;
    run_test(0);
    BOOST_TEST(cpu.D[3] == 2100);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[3] = 0xffff;
    cpu.D[2] = 0xffff;
    run_test(0);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[3] = 1;
    cpu.D[2] = 0;
    run_test(0);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(value) {
    cpu.D[3] = 2000;
    cpu.D[2] = 7000;
    run_test(4);
    BOOST_TEST(cpu.D[3] == 14000000);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
}
BOOST_AUTO_TEST_CASE(N) {
    cpu.D[3] = -1;
    cpu.D[2] = 1;
    run_test(4);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[3] = 0;
    cpu.D[2] = 1;
    run_test(4);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(V) {
    cpu.D[3] = -1;
    cpu.D[2] = 2;
    run_test(4);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Quad)

BOOST_AUTO_TEST_CASE(value) {
    cpu.D[3] = 0x40000000;
    cpu.D[2] = 4;
    run_test(10);
    BOOST_TEST(cpu.D[3] == 0);
    BOOST_TEST(cpu.D[4] == 1);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[3] = -1;
    cpu.D[2] = -1;
    run_test(10);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[3] = 0;
    cpu.D[2] = -1;
    run_test(10);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
