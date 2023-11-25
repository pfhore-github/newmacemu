#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;

struct F_ADDA {
    F_ADDA() {
        // ADDA.W %D2, %A3
		TEST::SET_W(0, 0150300 | 3 << 9 | 2 );
        TEST::SET_W(2, TEST_BREAK);

        // ADDA.L %D2, %A3
		TEST::SET_W(4, 0150700 | 3 << 9 | 2 );
        TEST::SET_W(6, TEST_BREAK);

        jit_compile(0, 8);
    }
};
BOOST_FIXTURE_TEST_SUITE(ADDA, Prepare, *boost::unit_test::fixture<F_ADDA>())
BOOST_AUTO_TEST_CASE(Word) {
    cpu.A[3] = 0x2020;
    cpu.D[2] = 0xfffe;
    run_test(0);
    BOOST_TEST(cpu.A[3] == 0x201E);
}

BOOST_AUTO_TEST_CASE(Long) {
    cpu.A[3] = 0x30000;
    cpu.D[2] = 0x20000;
    run_test(4);
    BOOST_TEST(cpu.A[3] == 0x50000);
}
BOOST_AUTO_TEST_SUITE_END()
