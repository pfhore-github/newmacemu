#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_PEA {
    F_PEA() {
        // PEA (%A3)
        TEST::SET_W(0, 0044120 | 3);
        TEST::SET_W(2, TEST_BREAK);
        jit_compile(0, 8);
    }};
BOOST_FIXTURE_TEST_SUITE(PEA, Prepare, *boost::unit_test::fixture<F_PEA>())


BOOST_AUTO_TEST_CASE(execute) {
    cpu.A[3] = 0x200;
    cpu.A[7] = 0x104;
    run_test(0);
    BOOST_TEST(TEST::GET_L(0x100) == 0x200);
    BOOST_TEST(cpu.A[7] == 0x100);
}
BOOST_AUTO_TEST_SUITE_END()
