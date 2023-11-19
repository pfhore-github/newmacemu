#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "inline.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(BFFFO, Prepare)

BOOST_AUTO_TEST_SUITE(D)
BOOST_AUTO_TEST_CASE(normal) { BOOST_TEST(BFFFO_D(0x12345678, 8, 8) == 10); }

BOOST_AUTO_TEST_CASE(overwrap) { BOOST_TEST(BFFFO_D(0x12345677, 28, 8) == 29); }

BOOST_AUTO_TEST_CASE(negative) { BOOST_TEST(BFFFO_D(0x12345677, -8, 8) == -7); }

BOOST_AUTO_TEST_CASE(zero) { BOOST_TEST(BFFFO_D(0x12345678, 4, 0) == 6); }
BOOST_AUTO_TEST_CASE(both_imm) {
    TEST::SET_W(0, 0166700 | 2);
    TEST::SET_W(2, 5 << 12 | 8 << 6 | 4);
    cpu.D[2] = 0x12345678;
    cpu.V = cpu.C = true;
    run_test();
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(cpu.D[5] == 10);
}

BOOST_AUTO_TEST_CASE(off_r) {
    TEST::SET_W(0, 0166700 | 2);
    TEST::SET_W(2, 5 << 12 | 1 << 11 | 3 << 6 | 4);
    cpu.D[2] = 0x12345678;
    cpu.D[3] = 8;
    run_test();
    BOOST_TEST(cpu.D[5] == 10);
}

BOOST_AUTO_TEST_CASE(width_r) {
    TEST::SET_W(0, 0166700 | 2);
    TEST::SET_W(2, 5 << 12 | 1 << 5 | 8 << 6 | 3);
    cpu.D[2] = 0x12345678;
    cpu.D[3] = 4;
    run_test();
    BOOST_TEST(cpu.D[5] == 10);
}

BOOST_AUTO_TEST_CASE(both_r) {
    TEST::SET_W(0, 0166700 | 2);
    TEST::SET_W(2, 5 << 12 | 1 << 5 | 1 << 11 | 3 << 6 | 5);
    cpu.D[2] = 0x12345678;
    cpu.D[3] = 8;
    cpu.D[5] = 4;
    run_test();
    BOOST_TEST(cpu.D[5] == 10);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(m)

BOOST_AUTO_TEST_CASE(byte1) {
    RAM[0x1000] = 0x12;
    BOOST_TEST(BFFFO_M(0x1000, 4, 4) == 6);
}

BOOST_AUTO_TEST_CASE(byte2) {
    TEST::SET_W(0x1000, 0x1234);
    BOOST_TEST(BFFFO_M(0x1000, 4, 8) == 6);
}

BOOST_AUTO_TEST_CASE(byte3) {
    TEST::SET_L(0x1000, 0x12345678);
    BOOST_TEST(BFFFO_M(0x1000, 4, 16) == 6);
}

BOOST_AUTO_TEST_CASE(byte4) {
    TEST::SET_L(0x1000, 0x12345678);
    BOOST_TEST(BFFFO_M(0x1000, 4, 24) == 6);
}

BOOST_AUTO_TEST_CASE(byte5) {
    TEST::SET_L(0x1000, 0x12345678);
    RAM[0x1004] = 0x9A;
    BOOST_TEST(BFFFO_M(0x1000, 4, 0) == 6);
}

BOOST_AUTO_TEST_CASE(off_neg) {
    TEST::SET_L(0x1000, 0x12345678);
    BOOST_TEST(BFFFO_M(0x1004, -12, 4) == -11);
}

// Z; see bftst/Z
// N; see bftst/N
BOOST_AUTO_TEST_CASE(both_imm) {
    TEST::SET_W(0, 0166720 | 2);
    TEST::SET_W(2, 5 << 12 | 4 << 6 | 4);
    RAM[0x1000] = 0x12;
    cpu.A[2] = 0x1000;
    run_test();
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(cpu.D[5] == 6);
}

BOOST_AUTO_TEST_CASE(off_r) {
    TEST::SET_W(0, 0166720 | 2);
    TEST::SET_W(2, 5 << 12 | 1 << 11 | 3 << 6 | 4);
    TEST::SET_L(0x1000, 0x12345678);
    RAM[0x1000] = 0x12;
    cpu.A[2] = 0x1000;
    cpu.D[3] = 4;
    run_test();
    BOOST_TEST(cpu.D[5] == 6);
}

BOOST_AUTO_TEST_CASE(width_r) {
    TEST::SET_W(0, 0166720 | 2);
    TEST::SET_W(2, 5 << 12 | 1 << 5 | 4 << 6 | 3);
    TEST::SET_L(0x1000, 0x12345678);
    RAM[0x1000] = 0x12;
    cpu.A[2] = 0x1000;
    cpu.D[3] = 4;
    run_test();
    BOOST_TEST(cpu.D[5] == 6);
}

BOOST_AUTO_TEST_CASE(both_r) {
    TEST::SET_W(0, 0166720 | 2);
    TEST::SET_W(2, 5 << 12 | 1 << 5 | 1 << 11 | 3 << 6 | 5);
    TEST::SET_L(0x1000, 0x12345678);
    RAM[0x1000] = 0x12;
    cpu.A[2] = 0x1000;
    cpu.D[3] = 4;
    cpu.D[5] = 4;
    run_test();
    BOOST_TEST(cpu.D[5] == 6);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
