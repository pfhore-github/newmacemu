#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(ROXL, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_DATA_TEST_CASE(byReg, bdata::xrange(2), old_x) {
    cpu.D[2] = 0x21;
    cpu.D[3] = 4;
    cpu.X = old_x;
    TEST::SET_W(0, 0160460 | 3 << 9 | 2);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x11 | old_x << 3);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(byRegMod) {
    cpu.D[2] = 0x21;
    cpu.D[3] = 68;
    TEST::SET_W(0, 0160460 | 3 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x11);
}

BOOST_AUTO_TEST_CASE(byImm) {
    cpu.D[2] = 0x21;
    TEST::SET_W(0, 0160420 | 4 << 9 | 2);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x11);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(byImm0) {
    cpu.D[2] = 0x21;
    TEST::SET_W(0, 0160420 | 0 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x10);
}

BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[2] = 0x80;
    TEST::SET_W(0, 0160420 | 1 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 0x40;
    TEST::SET_W(0, 0160420 | 1 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
    TEST::SET_W(0, 0160420 | 1 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(mem) {
    TEST::SET_W(0x1000, 0);
    cpu.X = true;
    TEST::SET_W(0, 0162720 | 2);
    cpu.A[2] = 0x1000;
    auto i = decode_and_run();
    BOOST_TEST(TEST::GET_W(0x1000) == 1);
    BOOST_TEST(i == 0);
}
BOOST_DATA_TEST_CASE(byReg, bdata::xrange(2), old_x) {
    cpu.D[2] = 0x0001;
    cpu.D[3] = 1;
    cpu.X = old_x;
    TEST::SET_W(0, 0160560 | 3 << 9 | 2);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x2 | old_x);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(byRegMod) {
    cpu.D[2] = 0x1234;
    cpu.D[3] = 68;
    TEST::SET_W(0, 0160560 | 3 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x2340);
}

BOOST_AUTO_TEST_CASE(byImm) {
    cpu.D[2] = 0x1234;
    TEST::SET_W(0, 0160520 | 4 << 9 | 2);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x2340);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(byImm0) {
    cpu.D[2] = 0x1234;
    TEST::SET_W(0, 0160520 | 0 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x3409);
}

BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[2] = 0x8000;
    TEST::SET_W(0, 0160520 | 1 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 0x4000;
    TEST::SET_W(0, 0160520 | 1 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
    TEST::SET_W(0, 0160520 | 1 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_DATA_TEST_CASE(byReg, bdata::xrange(2), old_x) {
    cpu.D[2] = 0x02345678;
    cpu.D[3] = 4;
    cpu.X = old_x;
    TEST::SET_W(0, 0160660 | 3 << 9 | 2);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == (0x23456780 | old_x << 3));
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(byRegMod) {
    cpu.D[2] = 0x12345678;
    cpu.D[3] = 68;
    TEST::SET_W(0, 0160660 | 3 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x23456780);
}

BOOST_AUTO_TEST_CASE(byImm) {
    cpu.D[2] = 0x12345678;
    TEST::SET_W(0, 0160620 | 4 << 9 | 2);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x23456780);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(byImm0) {
    cpu.D[2] = 0x12345678;
    TEST::SET_W(0, 0160620 | 0 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x34567809);
}

BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[2] = 0x80000000;
    TEST::SET_W(0, 0160620 | 1 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 0x40000000;
    TEST::SET_W(0, 0160620 | 1 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
    TEST::SET_W(0, 0160620 | 1 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()