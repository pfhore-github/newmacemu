#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_RTS {
    F_RTS() {
        // RTS
        TEST::SET_W(0, 0047165);
        TEST::SET_W(2, TEST_BREAK);

        TEST::SET_W(0x40, TEST_BREAK);
        jit_compile(0, 2);
        jit_compile(0x40, 2);
    }};
BOOST_FIXTURE_TEST_SUITE(RTS, Prepare, *boost::unit_test::fixture<F_RTS>())


BOOST_AUTO_TEST_CASE(execute) {
    TEST::SET_W(0, 0047165);
    TEST::SET_L(0x300, 0x40);
    cpu.A[7] = 0x300;
    run_test(0);
    BOOST_TEST(cpu.PC == 0x42);
    BOOST_TEST(cpu.A[7] == 0x304);
}

BOOST_AUTO_TEST_CASE(traced) {
    TEST::SET_W(0, 0047165);
    TEST::SET_L(0x1000, 0x400);
    cpu.A[7] = 0x1000;
    cpu.T = 1;
    run_test(0, EXCEPTION_NUMBER::TRACE );
}
BOOST_AUTO_TEST_SUITE_END()
