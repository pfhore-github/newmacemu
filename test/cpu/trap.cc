#define BOOST_TEST_DYN_LINK
#include "68040.hpp"

#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_TRAP {
    F_TRAP() {
        // TRAP #xx
        for(int i = 0; i < 16; ++i) {
            TEST::SET_W(4 * i, 0047100 | i);
            TEST::SET_W(4 * i + 2, TEST_BREAK);
        }
        jit_compile(0, 4*16);
    }};
BOOST_FIXTURE_TEST_SUITE(TRAP, Prepare, *boost::unit_test::fixture<F_TRAP>())


BOOST_DATA_TEST_CASE(execute, bdata::xrange(16), v) {
	run_test(4*v);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER(int(EXCEPTION_NUMBER::TRAP0) + v ));
}
BOOST_AUTO_TEST_SUITE_END()
