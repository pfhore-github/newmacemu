#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(UNPK, Prepare)
struct F {
    F() {
        // UNPK %D1, %D3, #5
        TEST::SET_W(0, 0100600 | 3 << 9 | 1);
        TEST::SET_W(2, 5);
        TEST::SET_W(4, TEST_BREAK);

        // UNPK -(%A1), -(%A3), #5
        TEST::SET_W(6, 0100610 | 3 << 9 | 1);
        TEST::SET_W(8, 5);
        TEST::SET_W(10, TEST_BREAK);

        jit_compile(0, 12);
    }
};
BOOST_AUTO_TEST_SUITE(R, *boost::unit_test::fixture<F>())
BOOST_AUTO_TEST_CASE(Reg) {
    TEST::SET_W(2, 5);
    cpu.D[1] = 0x34;
    run_test(0);
    BOOST_TEST(cpu.D[3] == 0x0309);
}

BOOST_AUTO_TEST_CASE(Mem) {
    RAM[0x100] = 0x34;
    cpu.A[1] = 0x101;
    cpu.A[3] = 0x112;
    run_test(6);
    BOOST_TEST(TEST::GET_W(0x110) == 0x0309);
    BOOST_TEST(cpu.A[1] == 0x100);
    BOOST_TEST(cpu.A[3] == 0x110);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
