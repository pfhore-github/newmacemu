#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_EXG {
    F_EXG() {
        // EXG %D2, %D3
        TEST::SET_W(0, 0140500 | 2 << 9 | 3);
        TEST::SET_W(2, TEST_BREAK);

        // EXG %A2, %A3
        TEST::SET_W(4, 0140510 | 2 << 9 | 3);
        TEST::SET_W(6, TEST_BREAK);

        // EXG %D2, %A3
        TEST::SET_W(8, 0140610 | 2 << 9 | 3);
        TEST::SET_W(10, TEST_BREAK);

        jit_compile(0, 12);
    }};
BOOST_FIXTURE_TEST_SUITE(EXG, Prepare, *boost::unit_test::fixture<F_EXG>())


BOOST_AUTO_TEST_CASE(D2D) {
    cpu.D[2] = 0x12345678;
    cpu.D[3] = 0x9ABCDEF0;
    run_test(0);
    BOOST_TEST(cpu.D[2] == 0x9ABCDEF0);
    BOOST_TEST(cpu.D[3] == 0x12345678);
}

BOOST_AUTO_TEST_CASE(A2A) {
    cpu.A[2] = 0x12345678;
    cpu.A[3] = 0x9ABCDEF0;
    run_test(4);
    BOOST_TEST(cpu.A[2] == 0x9ABCDEF0);
    BOOST_TEST(cpu.A[3] == 0x12345678);
}

BOOST_AUTO_TEST_CASE(D2A) {
    cpu.D[2] = 0x12345678;
    cpu.A[3] = 0x9ABCDEF0;
    run_test(8);
    BOOST_TEST(cpu.D[2] == 0x9ABCDEF0);
    BOOST_TEST(cpu.A[3] == 0x12345678);
}
BOOST_AUTO_TEST_SUITE_END()
