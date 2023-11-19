#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include "inline.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(ADDX, Prepare)
// CX/N/V are tested at ADD
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
    cpu.Z = true;
    BOOST_TEST(ADDX_B(14, 21, old_x) == 35 + old_x);
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.Z = true;
    ADDX_B(0, 0, false);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(Reg) {
    TEST::SET_W(0, 0150400 | 3 << 9 | 1);
    cpu.D[3] = 22;
    cpu.D[1] = 23;
    run_test();
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[3] == 45);
}

BOOST_AUTO_TEST_CASE(Mem) {
    TEST::SET_W(0, 0150410 | 3 << 9 | 1);
    RAM[0x1000] = 22;
    RAM[0x1100] = 34;
    cpu.A[3] = 0x1001;
    cpu.A[1] = 0x1101;
    cpu.X = true;
    run_test();
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(RAM[0x1000] == 57);
    BOOST_TEST(cpu.A[3] == 0x1000);
    BOOST_TEST(cpu.A[1] == 0x1100);
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
    cpu.Z = true;
    BOOST_TEST(ADDX_W(1400, 2101, old_x) == 3501 + old_x);
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.Z = true;
    ADDX_W(0, 0, false);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(Reg) {
    TEST::SET_W(0, 0150500 | 3 << 9 | 1);
    cpu.D[3] = 2222;
    cpu.D[1] = 2323;
    cpu.X = true;
    run_test();
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[3] == 4546);
}
BOOST_AUTO_TEST_CASE(Mem) {
    TEST::SET_W(0, 0150510 | 3 << 9 | 1);
    TEST::SET_W(0x1000, 2222);
    TEST::SET_W(0x1100, 3434);
    cpu.A[3] = 0x1002;
    cpu.A[1] = 0x1102;
    cpu.X = true;
    run_test();
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(TEST::GET_W(0x1000) == 5657);
    BOOST_TEST(cpu.A[3] == 0x1000);
    BOOST_TEST(cpu.A[1] == 0x1100);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
    cpu.Z = true;
    BOOST_TEST(ADDX_L(140000, 210001, old_x) == 350001 + old_x);
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.Z = true;
    ADDX_L(0, 0, false);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(Reg) {
    TEST::SET_W(0, 0150600 | 3 << 9 | 1);
    cpu.D[3] = 222222;
    cpu.D[1] = 232323;
    cpu.X = true;
    run_test();
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.D[3] == 454546);
}

BOOST_AUTO_TEST_CASE(Mem) {
    TEST::SET_W(0, 0150610 | 3 << 9 | 1);
    TEST::SET_L(0x1000, 222222);
    TEST::SET_L(0x1100, 343434);
    cpu.A[3] = 0x1004;
    cpu.A[1] = 0x1104;
    cpu.X = true;
    run_test();
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(TEST::GET_L(0x1000) == 565657);
    BOOST_TEST(cpu.A[3] == 0x1000);
    BOOST_TEST(cpu.A[1] == 0x1100);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
