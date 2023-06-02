#define BOOST_TEST_DYN_LINK
#include "inline.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_AUTO_TEST_SUITE(BFTST)
BOOST_AUTO_TEST_CASE(both_imm) {
    TEST::SET_W(0, 0164300 |  2 );
    TEST::SET_W(2, 8 << 6 | 4 );
    cpu.D[2] = 0x12345678;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.X = cpu.V = cpu.C = true;
    f();
    BOOST_TEST(i == 2);
    BOOST_TEST(cpu.X);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0164300|  2 );
    TEST::SET_W(2, 8 << 6 | 4 );
    cpu.D[2] = 0x12F45678;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0164300|  2 );
    TEST::SET_W(2, 8 << 6 | 4 );
    cpu.D[2] = 0x12045678;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(off_r) {
    TEST::SET_W(0, 0164300|  2 );
    TEST::SET_W(2, 1 << 11 | 3 << 6 | 4 );
    cpu.D[2] = 0x12045678;
    cpu.D[3] = 8;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.Z = false;
    f();
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(width_r) {
    TEST::SET_W(0, 0164300|  2 );
    TEST::SET_W(2, 1 << 5 | 8 << 6 | 3 );
    cpu.D[2] = 0x12045678;
    cpu.D[3] = 4;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.Z = false;
    f();
    BOOST_TEST(cpu.Z);
}


BOOST_AUTO_TEST_CASE(both_r) {
    TEST::SET_W(0, 0164300|  2 );
    TEST::SET_W(2, 1 << 5 | 1 << 11 | 3 << 6 | 5 );
    cpu.D[2] = 0x12045678;
    cpu.D[3] = 8;
    cpu.D[5] = 4;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.Z = false;
    f();
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(width0) {
    TEST::SET_W(0, 0164300|  2 );
    TEST::SET_W(2, 0 );
    cpu.D[2] = 0;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.Z = false;
    f();
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(width_r0) {
    TEST::SET_W(0, 0164300|  2 );
    TEST::SET_W(2, 1 << 5 | 3 );
    cpu.D[2] = 0;
    cpu.D[3] = 0;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.Z = false;
    f();
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(off_neg) {
    TEST::SET_W(0, 0164320|  2 );
    TEST::SET_W(2, 1 << 11 | 3 << 6| 4 );
    cpu.A[2] = 0x1002;
    TEST::SET_L(0x1000, 0x10345678);
    cpu.D[3] = -12;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.Z = false;
    f();
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(byte1) {
    TEST::SET_W(0, 0164320|  2 );
    TEST::SET_W(2, 4 << 6 | 4 );
    cpu.A[2] = 0x1000;
    RAM[0x1000] = 0x10;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.Z = false;
    f();
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(byte2) {
    TEST::SET_W(0, 0164320|  2 );
    TEST::SET_W(2, 4 << 6 | 8 );
    cpu.A[2] = 0x1000;
    RAM[0x1000] = 0x10;
    RAM[0x1001] = 0x04;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.Z = false;
    f();
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(byte3) {
    TEST::SET_W(0, 0164320|  2 );
    TEST::SET_W(2, 4 << 6 | 16 );
    cpu.A[2] = 0x1000;
    RAM[0x1000] = 0x10;
    RAM[0x1001] = 0x00;
    RAM[0x1002] = 0x06;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.Z = false;
    f();
    BOOST_TEST(cpu.Z);
}


BOOST_AUTO_TEST_CASE(byte4) {
    TEST::SET_W(0, 0164320|  2 );
    TEST::SET_W(2, 4 << 6 | 24 );
    cpu.A[2] = 0x1000;
    RAM[0x1000] = 0x10;
    RAM[0x1001] = 0x00;
    RAM[0x1002] = 0x00;
    RAM[0x1003] = 0x08;
    cpu.PC = 0;
    auto [f, i] = decode();
   cpu.Z = false;
    f();
    BOOST_TEST(cpu.Z);
}


BOOST_AUTO_TEST_CASE(byte5) {
    TEST::SET_W(0, 0164320|  2 );
    TEST::SET_W(2, 4 << 6 | 0 );
    cpu.A[2] = 0x1000;
    RAM[0x1000] = 0x10;
    RAM[0x1001] = 0x00;
    RAM[0x1002] = 0x00;
    RAM[0x1003] = 0x00;
    RAM[0x1004] = 0x0A;
    cpu.PC = 0;
    auto [f, i] = decode();
   cpu.Z = false;
    f();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()