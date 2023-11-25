#define BOOST_TEST_DYN_LINK
#include "68040.hpp"

#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_NEGX {
    F_NEGX() {
        // NEGX.B %D2
        TEST::SET_W(0, 0040000 | 2);
        TEST::SET_W(2, TEST_BREAK);

        // NEGX.W %D2
        TEST::SET_W(4, 0040100 | 2);
        TEST::SET_W(6, TEST_BREAK);

        // NEGX.L %D2
        TEST::SET_W(8, 0040200 | 2);
        TEST::SET_W(10, TEST_BREAK);

        jit_compile(0, 12);
    }};
BOOST_FIXTURE_TEST_SUITE(NEGX, Prepare, *boost::unit_test::fixture<F_NEGX>())


BOOST_AUTO_TEST_SUITE(Byte)
BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
    cpu.D[2] = -2;
    cpu.X = old_x;
    run_test(0);
    BOOST_TEST((cpu.D[2] & 0xff) == 2 - old_x);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_CASE(V) {
    cpu.D[2] = 0x80;
    cpu.X = false;
    run_test(0);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 1;
    cpu.X = false;
    run_test(0);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
    cpu.X = false;
    run_test(0);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)

BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
    cpu.D[2] = -2;
    cpu.X = old_x;
    run_test(4);
    BOOST_TEST((cpu.D[2] & 0xffff) == 2 - old_x);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_CASE(V) {
    cpu.D[2] = 0x8000;
    cpu.X = false;
    run_test(4);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 1000;
    cpu.X = false;
    run_test(4);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
    cpu.X = false;
    run_test(4);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}


BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)

BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
    cpu.D[2] = -2;
    cpu.X = old_x;
    run_test(8);
    BOOST_TEST(cpu.D[2] == 2 - old_x);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_CASE(V) {
    cpu.D[2] = 0x80000000;
    cpu.X = false;
    run_test(8);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 100000;
    cpu.X = false;
    run_test(8);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
    cpu.X = false;
    run_test(8);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
