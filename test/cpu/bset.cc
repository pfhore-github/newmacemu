#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include "proto.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(BSET, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(BSET_B(0, 3) == 8);
}

// Z; see BTST/Z
BOOST_AUTO_TEST_CASE(ByImmErr) {
    TEST::SET_W(0, 0004300 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}
BOOST_AUTO_TEST_CASE(ByImm) {
    TEST::SET_W(0, 0004320 | 2);
    TEST::SET_W(2, 3);
    RAM[0x1000] = 0;
    cpu.A[2] = 0x1000;
    auto i = decode_and_run();
    BOOST_TEST(i == 2);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(RAM[0x1000] == 8);
}
BOOST_AUTO_TEST_CASE(ByRegerr) {
    TEST::SET_W(0, 0000700 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}
BOOST_AUTO_TEST_CASE(ByReg) {
    TEST::SET_W(0, 0000720 | 2 | 4 << 9);
    cpu.D[4] = 3;
    RAM[0x1000] = 0;
    cpu.A[2] = 0x1000;
    auto i = decode_and_run();
    BOOST_TEST(i == 0);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(RAM[0x1000] == 8);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(BSET_L(0, 20) == 0x00100000);
}

// Z; see BTST/Z


BOOST_AUTO_TEST_CASE(ByImm) {
    TEST::SET_W(0, 0004300 | 2);
    TEST::SET_W(2, 20);
    cpu.D[2] = 0;
    auto i = decode_and_run();
    BOOST_TEST(i == 2);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(cpu.D[2] == 0x00100000);
}
BOOST_AUTO_TEST_CASE(ByReg) {
    TEST::SET_W(0, 0000700 | 2 | 4 << 9);
    cpu.D[2] = 0;
    cpu.D[4] = 20;
    auto i = decode_and_run();
    BOOST_TEST(i == 0);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(cpu.D[2] == 0x00100000);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()