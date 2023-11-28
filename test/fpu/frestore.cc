#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_FRESTORE {
    F_FRESTORE() {
        // FRESTORE (%A2)
        TEST::SET_W(0, 0171522);
        TEST::SET_W(2, TEST_BREAK);

        // FRESTORE (%A2)+
        TEST::SET_W(4, 0171532);
        TEST::SET_W(6, TEST_BREAK);
        jit_compile(0, 8);
    }
};
BOOST_FIXTURE_TEST_SUITE(FRESTORE, Prepare, *boost::unit_test::fixture<F_FRESTORE>())
BOOST_AUTO_TEST_CASE(err) {
    cpu.S = false;
    run_test(0);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::PRIV_ERR);
}
BOOST_AUTO_TEST_SUITE(normal)
BOOST_AUTO_TEST_CASE(reset) {
    cpu.S = true;
    cpu.A[2] = 0x100;
    TEST::SET_L(0x100, 0x00000000);
    mpfr_set_zero(cpu.FP[2], false);
    run_test(0);
    BOOST_TEST(mpfr_nan_p(cpu.FP[2]));
}

BOOST_AUTO_TEST_CASE(busy) {
    cpu.S = true;
    cpu.A[2] = 0x100;
    TEST::SET_L(0x100, 0x41600000);
    run_test(0);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::NO_ERR);
}

BOOST_AUTO_TEST_CASE(unimpl) {
    cpu.S = true;
    cpu.A[2] = 0x100;
    TEST::SET_W(0, 0171522);
    run_test(0);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::NO_ERR);
}

BOOST_AUTO_TEST_CASE(idle) {
    cpu.S = true;
    cpu.A[2] = 0x100;
    TEST::SET_L(0x100, 0x41000000);
    run_test(0);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::NO_ERR);
}

BOOST_AUTO_TEST_CASE(ferr1) {
    cpu.S = true;
    cpu.A[2] = 0x100;
    TEST::SET_L(0x100, 0x20000000);
    run_test(0);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::FORMAT_ERR);
}

BOOST_AUTO_TEST_CASE(ferr2) {
    cpu.S = true;
    cpu.A[2] = 0x100;
    TEST::SET_L(0x100, 0x41550000);
    run_test(0);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::FORMAT_ERR);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Incr)
BOOST_AUTO_TEST_CASE(err) {
    cpu.S = false;
    run_test(4);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::PRIV_ERR);
}

BOOST_AUTO_TEST_CASE(reset) {
    cpu.S = true;
    cpu.A[2] = 0x100;
    TEST::SET_L(0x100, 0x00000000);
    mpfr_set_zero(cpu.FP[2], false);
    run_test(4);
    BOOST_TEST(cpu.A[2] == 0x104);
    BOOST_TEST(mpfr_nan_p(cpu.FP[2]));
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::NO_ERR);
}

BOOST_AUTO_TEST_CASE(busy) {
    cpu.S = true;
    cpu.A[2] = 0x100;
    TEST::SET_L(0x100, 0x41600000);
    run_test(4);
    BOOST_TEST(cpu.A[2] == 0x160);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::NO_ERR);
}

BOOST_AUTO_TEST_CASE(unimpl) {
    cpu.S = true;
    cpu.A[2] = 0x100;
    TEST::SET_L(0x100, 0x41300000);
    run_test(4);
    BOOST_TEST(cpu.A[2] == 0x130);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::NO_ERR);
}

BOOST_AUTO_TEST_CASE(idle) {
    cpu.S = true;
    cpu.A[2] = 0x100;
    TEST::SET_L(0x100, 0x41000000);
    run_test(4);
    BOOST_TEST(cpu.A[2] == 0x104);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::NO_ERR);
}

BOOST_AUTO_TEST_CASE(ferr2) {
    cpu.S = true;
    cpu.A[2] = 0x100;
    TEST::SET_L(0x100, 0x41550000);
    run_test(4);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::FORMAT_ERR);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()