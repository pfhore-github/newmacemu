#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "inline.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(CMP, Prepare)
struct F {
    F() {
        // CMP.B %D2, %D3
        TEST::SET_W(0, 0130000 | 3 << 9 | 2);
        TEST::SET_W(2, TEST_BREAK);

        // CMP.W %D2, %D3
        TEST::SET_W(4, 0130100 | 3 << 9 | 2);
        TEST::SET_W(6, TEST_BREAK);

        // CMP.L %D2, %D3
        TEST::SET_W(8, 0130200 | 3 << 9 | 2);
        TEST::SET_W(10, TEST_BREAK);

        jit_compile(0, 12);
    }
};
BOOST_AUTO_TEST_SUITE(R, *boost::unit_test::fixture<F>())
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(V1) {
    cpu.D[3] = 127;
    cpu.D[2] = -3;
    run_test(0);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    cpu.D[3] = -126;
    cpu.D[2] = 7;
    run_test(0);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(C) {
    cpu.D[3] = 0;
    cpu.D[2] = 1;
    run_test(0);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[3] = -4;
    cpu.D[2] = 1;
    run_test(0);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[3] = 4;
    cpu.D[2] = 4;
    run_test(0);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(V1) {
    cpu.D[3] = 0x7fff;
    cpu.D[2] = -3;
    run_test(4);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    cpu.D[3] = 0x8002;
    cpu.D[2] = 7;
    run_test(4);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(C) {
    cpu.D[3] = -600;
    cpu.D[2] = -500;
    run_test(4);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[3] = -400;
    cpu.D[2] = 1;
    run_test(4);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[3] = 1000;
    cpu.D[2] = 1000;
    run_test(4);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(V1) {
    cpu.D[3] = 0x7fffffff;
    cpu.D[2] = -3;
    run_test(8);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    cpu.D[3] = 0x80000002;
    cpu.D[2] = 7;
    run_test(8);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(C) {
    cpu.D[3] = 30000;
    cpu.D[2] = 50000;
    run_test(8);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[3] = -30000;
    cpu.D[2] = 2000;
    run_test(8);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[3] = 30000;
    cpu.D[2] = 30000;
    run_test(8);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
