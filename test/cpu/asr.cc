#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(ASR, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(byReg) {
    cpu.D[2] = 0x40;
    cpu.D[3] = 2;
    TEST::SET_W(0, 0160040 | 3 << 9 | 2);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x10);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(byRegMod) {
    cpu.D[2] = 0x40;
    cpu.D[3] = 66;
    TEST::SET_W(0, 0160040 | 3 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x10);
}

BOOST_AUTO_TEST_CASE(byImm) {
    cpu.D[2] = 0x40;
    TEST::SET_W(0, 0160000 | 2 << 9 | 2);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x10);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(byImm0) {
    cpu.D[2] = 1;
    TEST::SET_W(0, 0160000 | 0 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0);
}

BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[2] = 1;
    TEST::SET_W(0, 0160000 | 1 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 0x80;
    TEST::SET_W(0, 0160000 | 1 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
    TEST::SET_W(0, 0160000 | 1 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(mem) {
    TEST::SET_W(0x1000, 300);
    TEST::SET_W(0, 0160320 | 2);
    cpu.A[2] = 0x1000;
    auto i =     decode_and_run();
    BOOST_TEST(TEST::GET_W(0x1000) == 150);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(byReg) {
    cpu.D[2] = 0x400;
    cpu.D[3] = 2;
    TEST::SET_W(0, 0160140 | 3 << 9 | 2);
    auto i =     decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x100);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(byRegMod) {
    cpu.D[2] = 0x400;
    cpu.D[3] = 66;
    TEST::SET_W(0, 0160140 | 3 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x100);
}

BOOST_AUTO_TEST_CASE(byImm) {
    cpu.D[2] = 0x400;
    TEST::SET_W(0, 0160100 | 2 << 9 | 2);
    auto i =     decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x100);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(byImm0) {
    cpu.D[2] = 0x400;
    TEST::SET_W(0, 0160100 | 0 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x4);
}

BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[2] = 1;
    TEST::SET_W(0, 0160100 | 1 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 0xC000;
    TEST::SET_W(0, 0160100 | 1 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
    TEST::SET_W(0, 0160100 | 1 << 9 | 2);
    cpu.PC = 0;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(byReg) {
    cpu.D[2] = 0x10000;
    cpu.D[3] = 2;
    TEST::SET_W(0, 0160240 | 3 << 9 | 2);
    auto i =     decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x4000);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(byRegMod) {
    cpu.D[2] = 0x10000;
    cpu.D[3] = 66;
    TEST::SET_W(0, 0160240 | 3 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x4000);
}

BOOST_AUTO_TEST_CASE(byImm) {
    cpu.D[2] = 0x10000;
    TEST::SET_W(0, 0160200 | 2 << 9 | 2);
    auto i =     decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x4000);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(byImm0) {
    cpu.D[2] = 0x10000;
    TEST::SET_W(0, 0160200 | 0 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x100);
}

BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[2] = 1;
    TEST::SET_W(0, 0160200 | 1 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 0xC0000000;
    TEST::SET_W(0, 0160200 | 1 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
    TEST::SET_W(0, 0160200 | 1 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()