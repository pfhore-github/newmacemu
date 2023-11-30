#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_NBCD {
    F_NBCD() {
        // NBCD %D2
        TEST::SET_W(0, 0044000 | 2);
        TEST::SET_W(2, TEST_BREAK);

        jit_compile(0, 4);
    }
};
BOOST_FIXTURE_TEST_SUITE(NBCD, Prepare, *boost::unit_test::fixture<F_NBCD>())

BOOST_AUTO_TEST_CASE(value) {
    cpu.D[2] = 0x34;
    cpu.Z = true;
    cpu.X = true;
    run_test(0);
    BOOST_TEST(cpu.D[2] == 0x65);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(cpu.X);
    BOOST_TEST(cpu.C);
}
BOOST_AUTO_TEST_CASE(c1) {
    cpu.D[2] = 0x30;
    cpu.X = false;
    run_test(0);
    BOOST_TEST(cpu.D[2] == 0x70);
}

BOOST_AUTO_TEST_CASE(Z1) {
    cpu.Z = true;
    cpu.D[2] = 0x00;
    cpu.X = false;
    run_test(0);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(Z2) {
    cpu.Z = true;
    cpu.D[2] = 0x99;
    cpu.X = true;
    run_test(0);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[2] = 0;
    cpu.X = false;
    run_test(0);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_SUITE_END()
