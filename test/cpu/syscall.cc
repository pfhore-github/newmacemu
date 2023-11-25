#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_SYSCALL {
    F_SYSCALL() {
        // #A1234
        TEST::SET_W(0, 0xA123);
        TEST::SET_W(2, TEST_BREAK);

        jit_compile(0, 4);
    }
};
BOOST_FIXTURE_TEST_SUITE(SYSCALL, Prepare,
                         *boost::unit_test::fixture<F_SYSCALL>())

BOOST_AUTO_TEST_CASE(execute) {
    run_test(0);
    BOOST_TEST(cpu.ex_n == 10);
}
BOOST_AUTO_TEST_SUITE_END()
