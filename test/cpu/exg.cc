#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(EXG, Prepare)

BOOST_AUTO_TEST_CASE(D2D) {
    TEST::SET_W(0, 0140500 | 2 << 9 | 3 );
    cpu.D[2] = 0x12345678;
    cpu.D[3] = 0x9ABCDEF0;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[2] == 0x9ABCDEF0);
    BOOST_TEST(cpu.D[3] == 0x12345678);
}

BOOST_AUTO_TEST_CASE(A2A) {
    TEST::SET_W(0, 0140510 | 2 << 9 | 3 );
    cpu.A[2] = 0x12345678;
    cpu.A[3] = 0x9ABCDEF0;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.A[2] == 0x9ABCDEF0);
    BOOST_TEST(cpu.A[3] == 0x12345678);
}

BOOST_AUTO_TEST_CASE(D2A) {
    TEST::SET_W(0, 0140610 | 2 << 9 | 3 );
    cpu.D[2] = 0x12345678;
    cpu.A[3] = 0x9ABCDEF0;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[2] == 0x9ABCDEF0);
    BOOST_TEST(cpu.A[3] == 0x12345678);
}
BOOST_AUTO_TEST_SUITE_END()
