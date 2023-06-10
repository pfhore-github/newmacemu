#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(ROXR, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_DATA_TEST_CASE(byReg, bdata::xrange(2), old_x) {
    cpu.D[2] = 0x21;
    cpu.D[3] = 4;
    cpu.X = old_x;
    TEST::SET_W(0, 0160060 | 3 << 9 | 2);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x22 | old_x << 5);
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
    TEST::SET_W(0, 0160060 | 3 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x22);
}

BOOST_AUTO_TEST_CASE(byImm) {
    cpu.D[2] = 0x21;
    TEST::SET_W(0, 0160020 | 4 << 9 | 2);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x22);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(byImm0) {
    cpu.D[2] = 0x21;
    TEST::SET_W(0, 0160020 | 0 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x42);
}

BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[2] = 1;
    TEST::SET_W(0, 0160020 | 1 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 0;
    cpu.X = true;
    TEST::SET_W(0, 0160020 | 1 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
    TEST::SET_W(0, 0160020 | 1 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(mem) {
    TEST::SET_W(0x1000, 0);
    cpu.X = true;
    TEST::SET_W(0, 0162320 | 2);
    cpu.A[2] = 0x1000;
    auto i = decode_and_run();
    BOOST_TEST(TEST::GET_W(0x1000) == 0x8000);
    BOOST_TEST(i == 0);
}
BOOST_DATA_TEST_CASE(byReg, bdata::xrange(2), old_x) {
    cpu.D[2] = 0x1230;
    cpu.D[3] = 4;
    cpu.X = old_x;
    TEST::SET_W(0, 0160160 | 3 << 9 | 2);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x0123 | old_x << 11);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(byRegMod) {
    cpu.D[2] = 0x1230;
    cpu.D[3] = 68;
    TEST::SET_W(0, 0160160 | 3 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x0123);
}

BOOST_AUTO_TEST_CASE(byImm) {
    cpu.D[2] = 0x1230;
    TEST::SET_W(0, 0160120 | 4 << 9 | 2);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x0123);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(byImm0) {
    cpu.D[2] = 0x1234;
    TEST::SET_W(0, 0160120 | 0 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x6812);
}

BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[2] = 1;
    TEST::SET_W(0, 0160120 | 1 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 0;
    cpu.X = true;
    TEST::SET_W(0, 0160120 | 1 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
    TEST::SET_W(0, 0160120 | 1 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_DATA_TEST_CASE(byReg, bdata::xrange(2), old_x) {
    cpu.D[2] = 0x12345670;
    cpu.D[3] = 4;
    cpu.X = old_x;
    TEST::SET_W(0, 0160260 | 3 << 9 | 2);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == (0x01234567 | old_x << 28));
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(byRegMod) {
    cpu.D[2] = 0x12345670;
    cpu.D[3] = 68;
    TEST::SET_W(0, 0160260 | 3 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x01234567);
}

BOOST_AUTO_TEST_CASE(byImm) {
    cpu.D[2] = 0x12345670;
    TEST::SET_W(0, 0160220 | 4 << 9 | 2);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x01234567);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(byImm0) {
    cpu.D[2] = 0x12345614;
    TEST::SET_W(0, 0160220 | 0 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x28123456);
}

BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[2] = 1;
    TEST::SET_W(0, 0160220 | 1 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 0;
    cpu.X = true;
    TEST::SET_W(0, 0160220 | 1 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
    TEST::SET_W(0, 0160220 | 1 << 9 | 2);
    decode_and_run();
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()