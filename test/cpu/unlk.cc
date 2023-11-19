#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(UNLK, Prepare)
struct F {
    F() {
        // UNLK  %A2
        TEST::SET_W(0, 0047130 | 2);
        TEST::SET_W(2, TEST_BREAK);

        jit_compile(0, 4);
    }
};
BOOST_AUTO_TEST_SUITE(R, *boost::unit_test::fixture<F>())
BOOST_AUTO_TEST_CASE(execute) {
    cpu.A[2] = 0x100;
    cpu.A[7] = 0x200;
    TEST::SET_L(0x100, 0x250);
    run_test(0);
    BOOST_TEST(cpu.A[2] == 0x250);
    BOOST_TEST(cpu.A[7] == 0x104);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()