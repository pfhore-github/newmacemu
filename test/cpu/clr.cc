#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(CLR, Prepare)
struct F {
    F() {
        // CLR.B %D2
        TEST::SET_W(0, 0041000 | 2);
        TEST::SET_W(2, TEST_BREAK);

        // CLR.W %D2
        TEST::SET_W(4, 0041100 | 2);
        TEST::SET_W(6, TEST_BREAK);

        // CLR.L %D2
        TEST::SET_W(8, 0041200 | 2);
        TEST::SET_W(10, TEST_BREAK);

        jit_compile(0, 12);
    }
};
BOOST_AUTO_TEST_SUITE(R, *boost::unit_test::fixture<F>())
BOOST_AUTO_TEST_CASE(Byte) {
    cpu.D[2] = 0x12345678;
    run_test(0);
    BOOST_TEST(cpu.D[2] == 0x12345600);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(Word) {
    cpu.D[2] = 0x12345678;
    run_test(4);
    BOOST_TEST(cpu.D[2] == 0x12340000);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(Long) {
    TEST::SET_W(0, 0041200 | 2 );
    cpu.D[2] = 0x12345678;
    run_test(8);
    BOOST_TEST(cpu.D[2] == 0);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
