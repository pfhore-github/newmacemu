#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(PACK, Prepare)
struct F {
    F() {
        // PACK %D1, %D3, #5
        TEST::SET_W(0, 0100500 | 3 << 9 | 1);
        TEST::SET_W(2, 5);
        TEST::SET_W(4, TEST_BREAK);

        // PACK -(%A1), -(%A3), #5
        TEST::SET_W(6, 0100510 | 3 << 9 | 1);
        TEST::SET_W(8, 5);
        TEST::SET_W(10, TEST_BREAK);

        jit_compile(0, 12);
    }
};
BOOST_AUTO_TEST_SUITE(R, *boost::unit_test::fixture<F>())
BOOST_AUTO_TEST_CASE(Reg) {
    cpu.D[1] = 0x0304;
    run_test(0);
    BOOST_TEST(cpu.D[3] == 0x39);
}

BOOST_AUTO_TEST_CASE(Mem) {
    TEST::SET_W(0x100, 0x0304);
    cpu.A[1] = 0x102;
    cpu.A[3] = 0x111;
    run_test(6);
    BOOST_TEST(RAM[0x110] == 0x39);
    BOOST_TEST(cpu.A[1] == 0x100);
    BOOST_TEST(cpu.A[3] == 0x110);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
