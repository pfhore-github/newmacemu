#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(MOVE16, Prepare)
BOOST_AUTO_TEST_CASE(reg2reg) {
    TEST::SET_W(0, 0173040 | 3);
    TEST::SET_W(2, 0x8000 | 2 << 12);
    cpu.A[3] = 0x407;
    cpu.A[2] = 0x603;
    TEST::SET_L(0x400, 0x01234567);
    TEST::SET_L(0x404, 0x89ABCDEF);
    TEST::SET_L(0x408, 0xFEDCBA98);
    TEST::SET_L(0x40C, 0x76543210);
    auto i = decode_and_run();
    BOOST_TEST(cpu.A[3] == 0x417);
    BOOST_TEST(cpu.A[2] == 0x613);
    BOOST_TEST(TEST::GET_L(0x600) == 0x01234567);
    BOOST_TEST(TEST::GET_L(0x604) == 0x89ABCDEF);
    BOOST_TEST(TEST::GET_L(0x608) == 0xFEDCBA98);
    BOOST_TEST(TEST::GET_L(0x60C) == 0x76543210);
    BOOST_TEST(i == 2);
}

BOOST_AUTO_TEST_CASE(imm2reg) {
    TEST::SET_W(0, 0173030 | 2);
    TEST::SET_L(2, 0x403);
    cpu.A[2] = 0x607;
    TEST::SET_L(0x400, 0x01234567);
    TEST::SET_L(0x404, 0x89ABCDEF);
    TEST::SET_L(0x408, 0xFEDCBA98);
    TEST::SET_L(0x40C, 0x76543210);
    auto i = decode_and_run();
    BOOST_TEST(cpu.A[2] == 0x607);
    BOOST_TEST(TEST::GET_L(0x600) == 0x01234567);
    BOOST_TEST(TEST::GET_L(0x604) == 0x89ABCDEF);
    BOOST_TEST(TEST::GET_L(0x608) == 0xFEDCBA98);
    BOOST_TEST(TEST::GET_L(0x60C) == 0x76543210);
    BOOST_TEST(i == 4);
}


BOOST_AUTO_TEST_CASE(imm2incr) {
    TEST::SET_W(0, 0173010 | 2);
    TEST::SET_L(2, 0x403);
    cpu.A[2] = 0x607;
    TEST::SET_L(0x400, 0x01234567);
    TEST::SET_L(0x404, 0x89ABCDEF);
    TEST::SET_L(0x408, 0xFEDCBA98);
    TEST::SET_L(0x40C, 0x76543210);
    auto i = decode_and_run();
    BOOST_TEST(cpu.A[2] == 0x617);
    BOOST_TEST(TEST::GET_L(0x600) == 0x01234567);
    BOOST_TEST(TEST::GET_L(0x604) == 0x89ABCDEF);
    BOOST_TEST(TEST::GET_L(0x608) == 0xFEDCBA98);
    BOOST_TEST(TEST::GET_L(0x60C) == 0x76543210);
    BOOST_TEST(i == 4);
}

BOOST_AUTO_TEST_CASE(reg2imm) {
    TEST::SET_W(0, 0173020 | 2);
    TEST::SET_L(2, 0x603);
    cpu.A[2] = 0x407;
    TEST::SET_L(0x400, 0x01234567);
    TEST::SET_L(0x404, 0x89ABCDEF);
    TEST::SET_L(0x408, 0xFEDCBA98);
    TEST::SET_L(0x40C, 0x76543210);
    auto i = decode_and_run();
    BOOST_TEST(cpu.A[2] == 0x407);
    BOOST_TEST(TEST::GET_L(0x600) == 0x01234567);
    BOOST_TEST(TEST::GET_L(0x604) == 0x89ABCDEF);
    BOOST_TEST(TEST::GET_L(0x608) == 0xFEDCBA98);
    BOOST_TEST(TEST::GET_L(0x60C) == 0x76543210);
    BOOST_TEST(i == 4);
}


BOOST_AUTO_TEST_CASE(incr2imm) {
    TEST::SET_W(0, 0173000 | 2);
    TEST::SET_L(2, 0x603);
    cpu.A[2] = 0x407;
    TEST::SET_L(0x400, 0x01234567);
    TEST::SET_L(0x404, 0x89ABCDEF);
    TEST::SET_L(0x408, 0xFEDCBA98);
    TEST::SET_L(0x40C, 0x76543210);
    auto i = decode_and_run();
    BOOST_TEST(cpu.A[2] == 0x417);
    BOOST_TEST(TEST::GET_L(0x600) == 0x01234567);
    BOOST_TEST(TEST::GET_L(0x604) == 0x89ABCDEF);
    BOOST_TEST(TEST::GET_L(0x608) == 0xFEDCBA98);
    BOOST_TEST(TEST::GET_L(0x60C) == 0x76543210);
    BOOST_TEST(i == 4);
}
BOOST_AUTO_TEST_SUITE_END()

