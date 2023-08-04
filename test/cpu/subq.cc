#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(SUBQ, Prepare)
// value & flags are tested at ADD
BOOST_AUTO_TEST_SUITE(Byte)


BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0050400 | 3 << 9 | 2);
    cpu.D[2] = 21;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[2] == 18);
}

BOOST_AUTO_TEST_CASE(eight) {
    TEST::SET_W(0, 0050400 | 0 << 9 | 2);
    cpu.D[2] = 21;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.D[2] == 13);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)


BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0050500 | 3 << 9 | 2);
    cpu.D[2] = 2000;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[2] == 1997);
}

BOOST_AUTO_TEST_CASE(eight) {
    TEST::SET_W(0, 0050500 | 0 << 9 | 2);
    cpu.D[2] = 2100;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.D[2] == 2092);
}

BOOST_AUTO_TEST_CASE(An) {
    TEST::SET_W(0, 0050510 | 3 << 9 | 2);
    cpu.A[2] = 2100;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.A[2] == 2097);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0050600 | 1 << 9 | 2);
    cpu.D[2] = 142044;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[2] == 142043);
}

BOOST_AUTO_TEST_CASE(eight) {
    TEST::SET_W(0, 0050600 | 0 << 9 | 2);
    cpu.D[2] = 200009;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.D[2] == 200001);
}

BOOST_AUTO_TEST_CASE(An) {
    TEST::SET_W(0, 0050610 | 3 << 9 | 2);
    cpu.A[2] = 210000;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.A[2] == 209997);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
