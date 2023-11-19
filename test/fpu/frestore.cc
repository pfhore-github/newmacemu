#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(FRESTORE, Prepare)
BOOST_AUTO_TEST_CASE(err) {
    cpu.S = false;
    TEST::SET_W(0, 0171520);
    BOOST_TEST(run_test() == 8);
}
BOOST_AUTO_TEST_SUITE(normal)
BOOST_AUTO_TEST_CASE(reset) {
    cpu.S = true;
    cpu.A[2] = 0x1000;
    TEST::SET_W(0, 0171522);
    TEST::SET_L(0x1000, 0x00000000);
    mpfr_set_zero(cpu.FP[2], false);
    run_test();
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(mpfr_nan_p(cpu.FP[2]));
}

BOOST_AUTO_TEST_CASE(busy) {
    cpu.S = true;
    cpu.A[2] = 0x1000;
    TEST::SET_W(0, 0171522);
    TEST::SET_L(0x1000, 0x41600000);
    run_test();
}

BOOST_AUTO_TEST_CASE(unimpl) {
    cpu.S = true;
    cpu.A[2] = 0x1000;
    TEST::SET_W(0, 0171522);
    TEST::SET_L(0x1000, 0x41300000);
    run_test();
}

BOOST_AUTO_TEST_CASE(idle) {
    cpu.S = true;
    cpu.A[2] = 0x1000;
    TEST::SET_W(0, 0171522);
    TEST::SET_L(0x1000, 0x41000000);
    run_test();
}

BOOST_AUTO_TEST_CASE(ferr1) {
    cpu.S = true;
    cpu.A[2] = 0x1000;
    TEST::SET_W(0, 0171522);
    TEST::SET_L(0x1000, 0x20000000);
    BOOST_TEST(run_test() == 14);
}

BOOST_AUTO_TEST_CASE(ferr2) {
    cpu.S = true;
    cpu.A[2] = 0x1000;
    TEST::SET_W(0, 0171522);
    TEST::SET_L(0x1000, 0x41550000);
    BOOST_TEST(run_test() == 14);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Incr)
BOOST_AUTO_TEST_CASE(err) {
    cpu.S = false;
    TEST::SET_W(0, 0171530);
    BOOST_TEST(run_test() == 8);
}

BOOST_AUTO_TEST_CASE(reset) {
    cpu.S = true;
    cpu.A[2] = 0x1000;
    TEST::SET_W(0, 0171532);
    TEST::SET_L(0x1000, 0x00000000);
    mpfr_set_zero(cpu.FP[2], false);
    run_test();
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.A[2] == 0x1004);
    BOOST_TEST(mpfr_nan_p(cpu.FP[2]));
}

BOOST_AUTO_TEST_CASE(busy) {
    cpu.S = true;
    cpu.A[2] = 0x1000;
    TEST::SET_W(0, 0171532);
    TEST::SET_L(0x1000, 0x41600000);
    run_test();
    BOOST_TEST(cpu.A[2] == 0x1060);
}

BOOST_AUTO_TEST_CASE(unimpl) {
    cpu.S = true;
    cpu.A[2] = 0x1000;
    TEST::SET_W(0, 0171532);
    TEST::SET_L(0x1000, 0x41300000);
    run_test();
    BOOST_TEST(cpu.A[2] == 0x1030);
}

BOOST_AUTO_TEST_CASE(idle) {
    cpu.S = true;
    cpu.A[2] = 0x1000;
    TEST::SET_W(0, 0171532);
    TEST::SET_L(0x1000, 0x41000000);
    run_test();
    BOOST_TEST(cpu.A[2] == 0x1004);
}

BOOST_AUTO_TEST_CASE(ferr2) {
    cpu.S = true;
    cpu.A[2] = 0x1000;
    TEST::SET_W(0, 0171532);
    TEST::SET_L(0x1000, 0x41550000);
    BOOST_TEST(run_test() == 14);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()