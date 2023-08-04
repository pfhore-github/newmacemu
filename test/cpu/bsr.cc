#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(BSR, Prepare)

BOOST_AUTO_TEST_CASE(execute) {
    cpu.A[7] = 0x1004;
    TEST::SET_W(0, 0060400 | 4);
    BOOST_TEST(run_test() == 0);
}

BOOST_AUTO_TEST_CASE(traced) {
    cpu.A[7] = 0x1004;
    TEST::SET_W(0, 0060400 | 4);
    cpu.T = 1;
    BOOST_TEST(run_test() == 9);
}

BOOST_AUTO_TEST_CASE(offset1) {
    cpu.A[7] = 0x1004;
    TEST::SET_W(0, 0060400 | 4);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(TEST::GET_L(0x1000) == 0x2);
    BOOST_TEST(cpu.A[7] == 0x1000);
}

BOOST_AUTO_TEST_CASE(offsetNeg) {
    cpu.A[7] = 0x1004;
    TEST::SET_W(0, 0060400 | 0xf0);
    run_test();
    BOOST_TEST(cpu.PC == -14);
}

BOOST_AUTO_TEST_CASE(offset00) {
    cpu.A[7] = 0x1004;
    TEST::SET_W(0, 0060400 | 0);
    TEST::SET_W(2, 0x1000);
    run_test();
    BOOST_TEST(cpu.PC == 0x1002);
    BOOST_TEST(TEST::GET_L(0x1000) == 0x4);
    BOOST_TEST(cpu.A[7] == 0x1000);
}
BOOST_AUTO_TEST_CASE(offsetFF) {
    cpu.A[7] = 0x1004;
    TEST::SET_W(0, 0060400 | 0xFF);
    TEST::SET_L(2, 0x20000);
    run_test();
    BOOST_TEST(cpu.PC == 0x20002);
    BOOST_TEST(TEST::GET_L(0x1000) == 0x6);
    BOOST_TEST(cpu.A[7] == 0x1000);
}

BOOST_AUTO_TEST_SUITE_END()
