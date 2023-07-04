#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(CMPI, Prepare)
// flag test: see cmp
BOOST_AUTO_TEST_CASE(Byte) {
    TEST::SET_W(0, 006000 | 2);
    TEST::SET_W(2, 0x10);
    cpu.D[2] = 0x10;
    auto i = decode_and_run();
    BOOST_TEST(cpu.Z);
    BOOST_TEST(i == 2);
}



BOOST_AUTO_TEST_CASE(Word) {
    TEST::SET_W(0, 006100 | 2);
    TEST::SET_W(2, 0x1000);
    cpu.D[2] = 0x1000;
    auto i = decode_and_run();
    BOOST_TEST(cpu.Z);
    BOOST_TEST(i == 2);
}



BOOST_AUTO_TEST_CASE(Long) {
    TEST::SET_W(0, 006200 | 2);
    TEST::SET_L(2, -2);
    cpu.D[2] = -2;
    auto i = decode_and_run();
    BOOST_TEST(cpu.Z);
    BOOST_TEST(i == 4);
}

BOOST_AUTO_TEST_SUITE_END()
