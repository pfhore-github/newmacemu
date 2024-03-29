#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"


#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_JMP {
    F_JMP() {
        // JSR (%A3)
        TEST::SET_W(0, 0047320 | 3);
        TEST::SET_W(2, TEST_BREAK);

        TEST::SET_W(0x30, TEST_BREAK);

        jit_compile(0, 4);
        jit_compile(0x30, 2);
    }};
BOOST_FIXTURE_TEST_SUITE(JMP, Prepare, *boost::unit_test::fixture<F_JMP>())


BOOST_AUTO_TEST_CASE(execute) {
    TEST::SET_W(0, 0047320 | 3);
    cpu.A[3] = 0x30;
    run_test(0);
    BOOST_TEST(cpu.PC == 0x32);
}

BOOST_AUTO_TEST_CASE(traced) {
    TEST::SET_W(0, 0047320 | 3);
    cpu.A[3] = 0x30;
    cpu.T = 1;
	run_test(0);
	BOOST_TEST(ex_n == EXCEPTION_NUMBER::TRACE );
}
 
BOOST_AUTO_TEST_SUITE_END()
