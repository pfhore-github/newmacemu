#define BOOST_TEST_DYN_LINK
#include "inline.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_AUTO_TEST_SUITE(ADDA)

BOOST_AUTO_TEST_CASE(Word) {
    TEST::SET_W(0, 0150300 | 3 << 9 | 2 );
    cpu.A[3] = 0x2020;
    cpu.D[2] = 0xfffe;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.A[3] == 0x201E);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(Long) {
    TEST::SET_W(0, 0150700 | 3 << 9 | 2 );
    cpu.A[3] = 0x30000;
    cpu.D[2] = 0x20000;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.A[3] == 0x50000);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_SUITE_END()
