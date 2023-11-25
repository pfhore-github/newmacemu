#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_JSR {
    F_JSR() {
        // JSR (%A3)
        TEST::SET_W(0, 0047220 | 3);
        TEST::SET_W(2, TEST_BREAK);

        TEST::SET_W(0x30, TEST_BREAK);

        jit_compile(0, 4);
        jit_compile(0x30, 2);
    }
};

BOOST_FIXTURE_TEST_SUITE(JSR, Prepare, *boost::unit_test::fixture<F_JSR>())


BOOST_AUTO_TEST_CASE(execute) {
    cpu.A[3] = 0x30;
    cpu.A[7] = 0x104;

    run_test(0);
    BOOST_TEST(cpu.PC == 0x32);
    BOOST_TEST(TEST::GET_L(0x100) == 2);
    BOOST_TEST(cpu.A[7] == 0x100);
}

BOOST_AUTO_TEST_CASE(traced) {
    cpu.A[3] = 0x30;
    cpu.A[7] = 0x104;
    
    cpu.T = 1;
	run_test(0);
	BOOST_TEST(cpu.ex_n == 9 );
}
BOOST_AUTO_TEST_SUITE_END()
