#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(BFCHG, Prepare)
BOOST_AUTO_TEST_CASE(Err) {
    TEST::SET_W(0, 0165300|  072 );
    BOOST_CHECK_THROW(decode(), DecodeError);
}
BOOST_AUTO_TEST_CASE(both_imm) {
    TEST::SET_W(0, 0165300|  2 );
    TEST::SET_W(2, 8 << 6 | 4 );
    cpu.D[2] = 0x12345678;
    cpu.V = cpu.C = true;
    auto i = decode_and_run();
    BOOST_TEST(i == 2);
    BOOST_TEST(cpu.D[2] == 0x12C45678);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0165300|  2 );
    TEST::SET_W(2, 8 << 6 | 4 );
    cpu.D[2] = 0x12F45678;
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0165300|  2 );
    TEST::SET_W(2, 8 << 6 | 4 );
    cpu.D[2] = 0x12045678;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(off_r) {
    TEST::SET_W(0, 0165300|  2 );
    TEST::SET_W(2, 1 << 11 | 3 << 6 | 4 );
    cpu.D[2] = 0x12345678;
    cpu.D[3] = 8;
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x12C45678);
}

BOOST_AUTO_TEST_CASE(width_r) {
    TEST::SET_W(0, 0165300|  2 );
    TEST::SET_W(2, 1 << 5 | 8 << 6 | 3 );
    cpu.D[2] = 0x12345678;
    cpu.D[3] = 4;
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x12C45678);
}


BOOST_AUTO_TEST_CASE(both_r) {
    TEST::SET_W(0, 0165300|  2 );
    TEST::SET_W(2, 1 << 5 | 1 << 11 | 3 << 6 | 5 );
    cpu.D[2] = 0x12345678;
    cpu.D[3] = 8;
    cpu.D[5] = 4;
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x12C45678);
}

BOOST_AUTO_TEST_CASE(width0) {
    TEST::SET_W(0, 0165300|  2 );
    TEST::SET_W(2, 0 );
    cpu.D[2] = 0x12345678;
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0xEDCBA987);
}

BOOST_AUTO_TEST_CASE(width_r0) {
    TEST::SET_W(0, 0165300|  2 );
    TEST::SET_W(2, 1 << 5 | 3 );
    cpu.D[2] = 0x12345678;
    cpu.D[3] = 0;
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0xEDCBA987);
}

BOOST_AUTO_TEST_CASE(off_neg) {
    TEST::SET_W(0, 0165320|  2 );
    TEST::SET_W(2, 1 << 11 | 3 << 6| 4 );
    cpu.A[2] = 0x1002;
    TEST::SET_L(0x1000, 0x12345678);
    cpu.D[3] = -12;
    decode_and_run();
    BOOST_TEST(TEST::GET_L(0x1000) == 0x1D345678);
}

BOOST_AUTO_TEST_CASE(byte1) {
    TEST::SET_W(0, 0165320|  2 );
    TEST::SET_W(2, 4 << 6 | 4 );
    cpu.A[2] = 0x1000;
    RAM[0x1000] = 0x12;
    decode_and_run();
    BOOST_TEST(RAM[0x1000] == 0x1D);
}

BOOST_AUTO_TEST_CASE(byte2) {
    TEST::SET_W(0, 0165320|  2 );
    TEST::SET_W(2, 4 << 6 | 8 );
    cpu.A[2] = 0x1000;
    RAM[0x1000] = 0x12;
    RAM[0x1001] = 0x34;
    decode_and_run();
    BOOST_TEST(RAM[0x1000] == 0x1D);
    BOOST_TEST(RAM[0x1001] == 0xC4);
}

BOOST_AUTO_TEST_CASE(byte3) {
    TEST::SET_W(0, 0165320|  2 );
    TEST::SET_W(2, 4 << 6 | 16 );
    cpu.A[2] = 0x1000;
    RAM[0x1000] = 0x12;
    RAM[0x1001] = 0x34;
    RAM[0x1002] = 0x56;
    decode_and_run();
    BOOST_TEST(RAM[0x1000] == 0x1D);
    BOOST_TEST(RAM[0x1001] == 0xCB);
    BOOST_TEST(RAM[0x1002] == 0xA6);
}


BOOST_AUTO_TEST_CASE(byte4) {
    TEST::SET_W(0, 0165320|  2 );
    TEST::SET_W(2, 4 << 6 | 24 );
    cpu.A[2] = 0x1000;
    RAM[0x1000] = 0x12;
    RAM[0x1001] = 0x34;
    RAM[0x1002] = 0x56;
    RAM[0x1003] = 0x78;
    decode_and_run();
    BOOST_TEST(RAM[0x1000] == 0x1D);
    BOOST_TEST(RAM[0x1001] == 0xCB);
    BOOST_TEST(RAM[0x1002] == 0xA9);
    BOOST_TEST(RAM[0x1003] == 0x88);
}


BOOST_AUTO_TEST_CASE(byte5) {
    TEST::SET_W(0, 0165320|  2 );
    TEST::SET_W(2, 4 << 6 | 0 );
    cpu.A[2] = 0x1000;
    RAM[0x1000] = 0x12;
    RAM[0x1001] = 0x34;
    RAM[0x1002] = 0x56;
    RAM[0x1003] = 0x78;
    RAM[0x1004] = 0x9A;
    decode_and_run();
    BOOST_TEST(RAM[0x1000] == 0x1D);
    BOOST_TEST(RAM[0x1001] == 0xCB);
    BOOST_TEST(RAM[0x1002] == 0xA9);
    BOOST_TEST(RAM[0x1003] == 0x87);
    BOOST_TEST(RAM[0x1004] == 0x6A);
}
BOOST_AUTO_TEST_SUITE_END()