#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(BRA, Prepare)

BOOST_AUTO_TEST_CASE(execute) {
    TEST::SET_W(0, 0060000 | 4);
    BOOST_TEST(run_test() == 0);
}

BOOST_AUTO_TEST_CASE(traced) {
    TEST::SET_W(0, 0060000 | 4);
    cpu.T = 1;
    BOOST_TEST(run_test() == 9);
}

BOOST_AUTO_TEST_CASE(addressError) {
    TEST::SET_W(0, 0060000 | 5);
    BOOST_TEST(run_test() == 3);
}

BOOST_AUTO_TEST_CASE(offset1) {
    TEST::SET_W(0, 0060000 | 4);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 6);
}

BOOST_AUTO_TEST_CASE(offsetNeg) {
    TEST::SET_W(0, 0060000 | 0xfe);
    run_test();
    BOOST_TEST(cpu.PC == 0);
}

BOOST_AUTO_TEST_CASE(offset00) {
    TEST::SET_W(0, 0060000 | 0);
    TEST::SET_W(2, 0x1000);
    run_test();
    BOOST_TEST(cpu.PC == 0x1002);
}
BOOST_AUTO_TEST_CASE(offsetFF) {
    TEST::SET_W(0, 0060000 | 0xFF);
    TEST::SET_L(2, 0x20000);
    run_test();
    BOOST_TEST(cpu.PC == 0x20002);
}

BOOST_AUTO_TEST_SUITE_END()
