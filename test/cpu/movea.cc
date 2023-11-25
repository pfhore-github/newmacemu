#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_MOVEA {
    F_MOVEA() {
        // MOVEA.W %D2, %A3
        TEST::SET_W(0, 0030100 | 3 << 9 | 2);
        TEST::SET_W(2, TEST_BREAK);

        // MOVEA.L %D2, %A3
        TEST::SET_W(4, 0020100 | 3 << 9 | 2);
        TEST::SET_W(6, TEST_BREAK);

        jit_compile(0, 8);
    }};
BOOST_FIXTURE_TEST_SUITE(MOVEA, Prepare, *boost::unit_test::fixture<F_MOVEA>())


BOOST_AUTO_TEST_CASE(Word) {
    cpu.D[2] = 0xFFF3;
    run_test(0);
    BOOST_TEST(cpu.A[3] == 0xFFFFFFF3);
}

BOOST_AUTO_TEST_CASE(Long) {
    cpu.D[2] = 0xFFF3;
    run_test(4);
    BOOST_TEST(cpu.A[3] == 0x0000FFF3);
}

BOOST_AUTO_TEST_SUITE_END()
