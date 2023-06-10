#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(SUBA, Prepare)

BOOST_AUTO_TEST_CASE(Word) {
    TEST::SET_W(0, 0110300 | 3 << 9 | 2 );
    cpu.A[3] = 0x2020;
    cpu.D[2] = 0xfffe;
    auto i = decode_and_run();
    BOOST_TEST(cpu.A[3] == 0x2022);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(Long) {
    TEST::SET_W(0, 0110700 | 3 << 9 | 2 );
    cpu.A[3] = 0x30000;
    cpu.D[2] = 0x20000;
    auto i = decode_and_run();
    BOOST_TEST(cpu.A[3] == 0x10000);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_SUITE_END()
