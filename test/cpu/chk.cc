#define BOOST_TEST_DYN_LINK
#include "68040.hpp"

#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_CHK {
    F_CHK() {
        // CHK.W %D2, %D3
        TEST::SET_W(0, 0040600 | 3 << 9 | 2);
        TEST::SET_W(2, TEST_BREAK);

        // CHK.L %D2, %D3
        TEST::SET_W(4, 0040400 | 3 << 9 | 2);
        TEST::SET_W(6, TEST_BREAK);
        jit_compile(0, 8);
    }};
BOOST_FIXTURE_TEST_SUITE(CHK, Prepare, *boost::unit_test::fixture<F_CHK>())


BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(In) {
    cpu.D[3] = 1000;
    cpu.D[2] = 2000;
    run_test(0);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::NO_ERR);
}

BOOST_AUTO_TEST_CASE(Under) {
    cpu.D[3] = -300;
    cpu.D[2] = 2000;
    run_test(0);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::CHK_FAIL);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Over) {
    TEST::SET_W(0, 0040600 | 3 << 9 | 2);
    cpu.D[3] = 2300;
    cpu.D[2] = 2000;
    run_test(0);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::CHK_FAIL);
    BOOST_TEST(!cpu.N);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(In) {
    cpu.D[3] = 100000;
    cpu.D[2] = 200000;
    run_test(4);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::NO_ERR);
}

BOOST_AUTO_TEST_CASE(Under) {
    cpu.D[3] = -30000;
    cpu.D[2] = 200000;
    run_test(4);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::CHK_FAIL);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Over) {
    cpu.D[3] = 230000;
    cpu.D[2] = 200000;
    run_test(4);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::CHK_FAIL);
    BOOST_TEST(!cpu.N);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

