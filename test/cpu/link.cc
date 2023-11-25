#define BOOST_TEST_DYN_LINK
#include "68040.hpp"

#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_LINK {
    F_LINK() {
        // LINK.W  %A2. #0x40
        TEST::SET_W(0, 0047120 | 2);
        TEST::SET_W(2, 0x40);
        TEST::SET_W(4, TEST_BREAK);

        // LINK.L %A2. #0x40
        TEST::SET_W(6, 0044010 | 2);
        TEST::SET_L(8, 0x40);
        TEST::SET_W(12, TEST_BREAK);
        jit_compile(0, 14);
    }
};
BOOST_FIXTURE_TEST_SUITE(LINK, Prepare, *boost::unit_test::fixture<F_LINK>())

BOOST_AUTO_TEST_CASE(Word) {
    cpu.A[2] = 0x110;
    cpu.A[7] = 0x104;
    run_test(0);
    BOOST_TEST(TEST::GET_L(0x100) == 0x110);
    BOOST_TEST(cpu.A[2] == 0x100);
    BOOST_TEST(cpu.A[7] == 0x140);
}

BOOST_AUTO_TEST_CASE(Long) {
    cpu.A[2] = 0x110;
    cpu.A[7] = 0x404;
    run_test(6);
    BOOST_TEST(TEST::GET_L(0x400) == 0x110);
    BOOST_TEST(cpu.A[2] == 0x400);
    BOOST_TEST(cpu.A[7] == 0x440);
}

BOOST_AUTO_TEST_SUITE_END()
