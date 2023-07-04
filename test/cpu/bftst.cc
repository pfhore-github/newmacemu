#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include "proto.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(BFTST, Prepare)
BOOST_AUTO_TEST_SUITE(D)

BOOST_AUTO_TEST_CASE(overwrap) {
    BFTST_D(0x12345678, 28, 8);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(negative) {
    BFTST_D(0x12345678, -4, 8);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(zero) {
    BFTST_D(0x12345678, -4, 0);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    BFTST_D(0x12005678, 8, 8);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(N) {
    BFTST_D(0x12805678, 8, 8);
    BOOST_TEST(cpu.N);
}


BOOST_AUTO_TEST_CASE(both_imm) {
    TEST::SET_W(0, 0164300 |  2 );
    TEST::SET_W(2, 8 << 6 | 4 );
    cpu.D[2] = 0x12345678;
    cpu.V = cpu.C = true;
    auto i = decode_and_run();
    BOOST_TEST(i == 2);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
}


BOOST_AUTO_TEST_CASE(off_r) {
    TEST::SET_W(0, 0164300|  2 );
    TEST::SET_W(2, 1 << 11 | 3 << 6 | 4 );
    cpu.D[2] = 0x12045678;
    cpu.D[3] = 8;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(width_r) {
    TEST::SET_W(0, 0164300|  2 );
    TEST::SET_W(2, 1 << 5 | 8 << 6 | 3 );
    cpu.D[2] = 0x12045678;
    cpu.D[3] = 4;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}


BOOST_AUTO_TEST_CASE(both_r) {
    TEST::SET_W(0, 0164300|  2 );
    TEST::SET_W(2, 1 << 5 | 1 << 11 | 3 << 6 | 5 );
    cpu.D[2] = 0x12045678;
    cpu.D[3] = 8;
    cpu.D[5] = 4;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(off_neg) {
    TEST::SET_W(0, 0164320|  2 );
    TEST::SET_W(2, 1 << 11 | 3 << 6| 4 );
    cpu.A[2] = 0x1002;
    TEST::SET_L(0x1000, 0x10345678);
    cpu.D[3] = -12;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(A)
BOOST_AUTO_TEST_CASE(byte1) {
    RAM[0x1000] = 0x10;
    BFTST_M(0x1000, 4, 4);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(byte2) {
    RAM[0x1000] = 0x10;
    RAM[0x1001] = 0x04;
    BFTST_M(0x1000, 4, 8);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(byte3) {
    RAM[0x1000] = 0x10;
    RAM[0x1001] = 0x00;
    RAM[0x1002] = 0x06;
    BFTST_M(0x1000, 4, 16);
    BOOST_TEST(cpu.Z);
}


BOOST_AUTO_TEST_CASE(byte4) {
    RAM[0x1000] = 0x10;
    RAM[0x1001] = 0x00;
    RAM[0x1002] = 0x00;
    RAM[0x1003] = 0x08;
    BFTST_M(0x1000, 4, 24);
    BOOST_TEST(cpu.Z);
}


BOOST_AUTO_TEST_CASE(byte5) {
    RAM[0x1000] = 0x10;
    RAM[0x1001] = 0x00;
    RAM[0x1002] = 0x00;
    RAM[0x1003] = 0x00;
    RAM[0x1004] = 0x0A;
    BFTST_M(0x1000, 4, 0);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(Z) {
    RAM[0x1000] = 0x10;
    RAM[0x1001] = 0x02;
    BFTST_M(0x1000, 4, 8);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(N) {
    RAM[0x1000] = 0x18;
    RAM[0x1001] = 0x02;
    BFTST_M(0x1000, 4, 8);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(both_imm) {
    TEST::SET_W(0, 0164320 | 2);
    TEST::SET_W(2, 8 << 6 | 4);
    TEST::SET_L(0x1000, 0x12045678);
    cpu.A[2] = 0x1000;
    auto i = decode_and_run();
    BOOST_TEST(i == 2);
    BOOST_TEST(cpu.Z);
}


BOOST_AUTO_TEST_CASE(off_r) {
    TEST::SET_W(0, 0164320 | 2);
    TEST::SET_W(2, 1 << 11 | 3 << 6 | 4);
    cpu.A[2] = 0x1000;
    TEST::SET_L(0x1000, 0x12045678);
    cpu.D[3] = 8;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(width_r) {
    TEST::SET_W(0, 0164320 | 2);
    TEST::SET_W(2, 1 << 5 | 8 << 6 | 3);
    cpu.A[2] = 0x1000;
    TEST::SET_L(0x1000, 0x12045678);
    cpu.D[3] = 4;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(both_r) {
    TEST::SET_W(0, 0164320 | 2);
    TEST::SET_W(2, 1 << 5 | 1 << 11 | 3 << 6 | 5);
    cpu.A[2] = 0x1000;
    TEST::SET_L(0x1000, 0x12045678);
    cpu.D[3] = 8;
    cpu.D[5] = 4;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}


BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()