#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(CMPA, Prepare)

struct F {
    F() {
        // CMPA.W %D2, A3
        TEST::SET_W(0, 0130300 | 3 << 9 | 2);
        TEST::SET_W(2, TEST_BREAK);
        
        // CMPA.L %D2, %A3
        TEST::SET_W(4, 0130700 | 3 << 9 | 2);
        TEST::SET_W(6, TEST_BREAK);

        jit_compile(0, 8);
    }
};
BOOST_AUTO_TEST_SUITE(R, *boost::unit_test::fixture<F>())
BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(V) {
    cpu.A[3] = 0x80000000;
    cpu.D[2] = 7;
    run_test(0);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(C) {
    cpu.A[3] = 0;
    cpu.D[2] = 1;
    run_test(0);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.A[3] = -400;
    cpu.D[2] = 1;
    run_test(0);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.A[3] = 0xfffffffe;
    cpu.D[2] = 0xfffe;
    run_test(0);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(V) {
    cpu.A[3] = 0x80000000;
    cpu.D[2] = 7;
    run_test(4);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(C) {
    cpu.A[3] = 0;
    cpu.D[2] = 1;
    run_test(4);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.A[3] = -400;
    cpu.D[2] = 1;
    run_test(4);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.A[3] = -2;
    cpu.D[2] = -2;
    run_test(4);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
