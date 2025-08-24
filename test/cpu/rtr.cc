#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_RTR {
    F_RTR() {
        // RTR
        TEST::SET_W(0, 0047167);
        TEST::SET_W(2, TEST_BREAK);

        TEST::SET_W(0x40, TEST_BREAK);
        jit_compile(0, 4);
        jit_compile(0x40, 2);
    }};
BOOST_FIXTURE_TEST_SUITE(RTR, Prepare, *boost::unit_test::fixture<F_RTR>())


BOOST_AUTO_TEST_CASE(execute) {
    TEST::SET_W(0x302, 0x1F);
    TEST::SET_L(0x304, 0x40);
    cpu.T = 0;
    cpu.A[7] = 0x302;
    run_test(0);
    BOOST_TEST(cpu.PC == 0x42);
    BOOST_TEST(cpu.A[7] == 0x308);
    BOOST_TEST(cpu.X);
    BOOST_TEST(cpu.N);
    BOOST_TEST(cpu.V);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(traced) {
    TEST::SET_W(0x302, 0x1F);
    TEST::SET_L(0x304, 0x40);
    cpu.A[7] = 0x302;
    cpu.T = 1;
    run_test(0, EXCEPTION_NUMBER::TRACE);
}

BOOST_AUTO_TEST_SUITE_END()
