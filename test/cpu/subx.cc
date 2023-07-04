#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include "proto.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(SUBX, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
    cpu.Z = true;
    BOOST_TEST(SUBX_B(50, 21, old_x) == 29 - old_x);
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.Z = true;
    SUBX_B(10, 10, false);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(Reg) {
    TEST::SET_W(0, 0110400 | 3 << 9 | 1);
    cpu.D[1] = 52;
    cpu.D[3] = 15;
    cpu.X = true;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[1] == 36);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(Mem) {
    TEST::SET_W(0, 0110410 | 3 << 9 | 1);
    RAM[0x1000] = 60;
    RAM[0x1100] = 34;
    cpu.A[1] = 0x1001;
    cpu.A[3] = 0x1101;
    cpu.X = true;
    auto i = decode_and_run();
    BOOST_TEST(RAM[0x1000] == 25);
    BOOST_TEST(cpu.A[1] == 0x1000);
    BOOST_TEST(cpu.A[3] == 0x1100);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
    cpu.Z = true;
    BOOST_TEST(SUBX_W(2524, 1113, old_x) == 1411 - old_x);
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.Z = true;
    SUBX_W(700, 700, false);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(Reg) {
    TEST::SET_W(0, 0110500 | 3 << 9 | 1);
    cpu.D[1] = 4050;
    cpu.D[3] = 2010;
    cpu.X = true;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[1] == 2039);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(Mem) {
    TEST::SET_W(0, 0110510 | 3 << 9 | 1);
    TEST::SET_W(0x1000, 6789);
    TEST::SET_W(0x1100, 3434);
    cpu.A[1] = 0x1002;
    cpu.A[3] = 0x1102;
    cpu.X = true;
    auto i = decode_and_run();
    BOOST_TEST(TEST::GET_W(0x1000) == 3354);
    BOOST_TEST(cpu.A[1] == 0x1000);
    BOOST_TEST(cpu.A[3] == 0x1100);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
    cpu.Z = true;
    BOOST_TEST(SUBX_L(520000, 130000, old_x) == 390000 - old_x);
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.Z = true;
    SUBX_L(500000, 500000, false);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(Reg) {
    TEST::SET_W(0, 0110600 | 3 << 9 | 1);
    cpu.D[1] = 987654;
    cpu.D[3] = 112233;
    cpu.X = true;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[1] == 875420);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(Mem) {
    TEST::SET_W(0, 0110610 | 3 << 9 | 1);
    TEST::SET_L(0x1000, 87654321);
    TEST::SET_L(0x1100, 11223344);
    cpu.A[1] = 0x1004;
    cpu.A[3] = 0x1104;
    cpu.X = true;
    auto i = decode_and_run();
    BOOST_TEST(TEST::GET_L(0x1000) == 76430976);
    BOOST_TEST(cpu.A[1] == 0x1000);
    BOOST_TEST(cpu.A[3] == 0x1100);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
