#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(SUBA, Prepare)
struct F {
    F() {
        // SUBA.W %D2, %A3
        TEST::SET_W(0, 0110300 | 3 << 9 | 2);
        TEST::SET_W(2, TEST_BREAK);

        // SUBA.L %D2, %A3
        TEST::SET_W(4, 0110700 | 3 << 9 | 2);
        TEST::SET_W(6, TEST_BREAK);

        jit_compile(0, 8);
    }
};
BOOST_AUTO_TEST_SUITE(R, *boost::unit_test::fixture<F>())
BOOST_AUTO_TEST_CASE(Word) {
    cpu.A[3] = 0x2020;
    cpu.D[2] = 0xfffe;
    run_test(0);
    BOOST_TEST(cpu.A[3] == 0x2022);
}

BOOST_AUTO_TEST_CASE(Long) {
    cpu.A[3] = 0x30000;
    cpu.D[2] = 0x20000;
    run_test(4);
    BOOST_TEST(cpu.A[3] == 0x10000);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
