#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "inline.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(BFCLR, Prepare)
BOOST_AUTO_TEST_SUITE(D)
BOOST_AUTO_TEST_CASE(normal) {
    BOOST_TEST(BFCLR_D(0x12345678, 8, 8) == 0x12005678);
}

BOOST_AUTO_TEST_CASE(overwrap) {
    BOOST_TEST(BFCLR_D(0x12345678, 28, 8) == 0x02345670);
}

BOOST_AUTO_TEST_CASE(negative) {
    BOOST_TEST(BFCLR_D(0x12345678, -4, 8) == 0x02345670);
}

BOOST_AUTO_TEST_CASE(zero) {
    BOOST_TEST(BFCLR_D(0x12345678, 4, 0) == 0);
}
// Z; see bftst/Z
// N; see bftst/N

BOOST_AUTO_TEST_CASE(both_imm) {
    TEST::SET_W(0, 0166300 | 2);
    TEST::SET_W(2, 8 << 6 | 4);
    cpu.D[2] = 0x12345678;
    run_test();
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(cpu.D[2] == 0x12045678);
}   



BOOST_AUTO_TEST_CASE(off_r) {
    TEST::SET_W(0, 0166300 | 2);
    TEST::SET_W(2, 1 << 11 | 3 << 6 | 4);
    cpu.D[2] = 0x12345678;
    cpu.D[3] = 8;
    run_test();
    BOOST_TEST(cpu.D[2] == 0x12045678);
}

BOOST_AUTO_TEST_CASE(width_r) {
    TEST::SET_W(0, 0166300 | 2);
    TEST::SET_W(2, 1 << 5 | 8 << 6 | 3);
    cpu.D[2] = 0x12345678;
    cpu.D[3] = 4;
    run_test();
    BOOST_TEST(cpu.D[2] == 0x12045678);
}

BOOST_AUTO_TEST_CASE(both_r) {
    TEST::SET_W(0, 0166300 | 2);
    TEST::SET_W(2, 1 << 5 | 1 << 11 | 3 << 6 | 5);
    cpu.D[2] = 0x12345678;
    cpu.D[3] = 8;
    cpu.D[5] = 4;
    run_test();
    BOOST_TEST(cpu.D[2] == 0x12045678);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(A)
BOOST_AUTO_TEST_CASE(byte1) {
    RAM[0x1000] = 0x12;
    BFCLR_M(0x1000, 4, 4);
    BOOST_TEST(RAM[0x1000] == 0x10);
}

BOOST_AUTO_TEST_CASE(byte2) {
    TEST::SET_W(0x1000, 0x1234);
    BFCLR_M(0x1000, 4, 8);
    BOOST_TEST(TEST::GET_W(0x1000) == 0x1004);
}

BOOST_AUTO_TEST_CASE(byte3) {
    TEST::SET_L(0x1000, 0x12345678);
    BFCLR_M(0x1000, 4, 16);
    BOOST_TEST(TEST::GET_L(0x1000) == 0x10000678);
}

BOOST_AUTO_TEST_CASE(byte4) {
    TEST::SET_L(0x1000, 0x12345678);
    BFCLR_M(0x1000, 4, 24);
    BOOST_TEST(TEST::GET_L(0x1000) == 0x10000008);
}

BOOST_AUTO_TEST_CASE(byte5) {
    TEST::SET_L(0x1000, 0x12345678);
    RAM[0x1004] = 0x9A;
    BFCLR_M(0x1000, 4, 0);
    BOOST_TEST(TEST::GET_L(0x1000) == 0x10000000);
    BOOST_TEST(RAM[0x1004] == 0x0A);
}

BOOST_AUTO_TEST_CASE(off_neg) {
    TEST::SET_L(0x1000, 0x12345678);
    BFCLR_M(0x1004, -12, 4);
    BOOST_TEST(TEST::GET_L(0x1000) == 0x12345078);
}


// Z; see bftst/Z
// N; see bftst/N

BOOST_AUTO_TEST_CASE(both_imm) {
    TEST::SET_W(0, 0166320 | 2);
    cpu.A[2] = 0x1000;
    TEST::SET_W(2, 8 << 6 | 4);
    TEST::SET_L(0x1000, 0x12345678);
    run_test();
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(TEST::GET_L(0x1000) == 0x12045678);
}


BOOST_AUTO_TEST_CASE(off_r) {
    TEST::SET_W(0, 0166320 | 2);
    TEST::SET_W(2, 1 << 11 | 3 << 6 | 4);
    cpu.A[2] = 0x1000;
    TEST::SET_L(0x1000, 0x12345678);
    cpu.D[3] = 8;
    run_test();
    BOOST_TEST(TEST::GET_L(0x1000) == 0x12045678);
}

BOOST_AUTO_TEST_CASE(width_r) {
    TEST::SET_W(0, 0166320 | 2);
    TEST::SET_W(2, 1 << 5 | 8 << 6 | 3);
    cpu.A[2] = 0x1000;
    TEST::SET_L(0x1000, 0x12345678);
    cpu.D[3] = 4;
    run_test();
    BOOST_TEST(TEST::GET_L(0x1000) == 0x12045678);
}

BOOST_AUTO_TEST_CASE(both_r) {
    TEST::SET_W(0, 0166320 | 2);
    TEST::SET_W(2, 1 << 5 | 1 << 11 | 3 << 6 | 5);
    cpu.A[2] = 0x1000;
    TEST::SET_L(0x1000, 0x12345678);
    cpu.D[3] = 8;
    cpu.D[5] = 4;
    run_test();
    BOOST_TEST(TEST::GET_L(0x1000) == 0x12045678);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()