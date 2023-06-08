#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(ADDI, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(err) {
    TEST::SET_W(0, 0003000 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0003000 | 2);
    TEST::SET_W(2, 14);
    cpu.D[2] = 21;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 35);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(i == 2);
}
BOOST_AUTO_TEST_CASE(V1) {
    TEST::SET_W(0, 0003000 | 2);
    TEST::SET_W(2, 127);
    cpu.D[2] = 3;
    decode_and_run();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    TEST::SET_W(0, 0003000 | 2);
    TEST::SET_W(2, 0x80);
    cpu.D[2] = 0xff;
    decode_and_run();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(CX) {
    TEST::SET_W(0, 0003000 | 2);
    TEST::SET_W(2, 0xfc);
    cpu.D[2] = 12;
    decode_and_run();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0003000 | 2);
    TEST::SET_W(2, 0xfc);
    cpu.D[2] = 1;
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0003000 | 2);
    TEST::SET_W(2, 0xff);
    cpu.D[2] = 1;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(err) {
    TEST::SET_W(0, 0003100 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}

BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0003100 | 2);
    TEST::SET_W(2, 1420);
    cpu.D[2] = 2133;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 3553);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(i == 2);
}
BOOST_AUTO_TEST_CASE(V1) {
    TEST::SET_W(0, 0003100 | 2);
    TEST::SET_W(2, 0x7fff);
    cpu.D[2] = 3;
    decode_and_run();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    TEST::SET_W(0, 0003100 | 2);
    TEST::SET_W(2, 0x8000);
    cpu.D[2] = 0xffff;
    decode_and_run();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(CX) {
    TEST::SET_W(0, 0003100 | 2);
    TEST::SET_W(2, 0xfffc);
    cpu.D[2] = 12;
    decode_and_run();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0003100 | 2);
    TEST::SET_W(2, 0xfffc);
    cpu.D[2] = 1;
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0003100 | 2);
    TEST::SET_W(2, 0xffff);
    cpu.D[2] = 1;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(err) {
    TEST::SET_W(0, 0003200 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}

BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0003200 | 2);
    TEST::SET_L(2, 142044);
    cpu.D[2] = 213324;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 355368);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(i == 4);
}
BOOST_AUTO_TEST_CASE(V1) {
    TEST::SET_W(0, 0003200 | 2);
    TEST::SET_L(2, 0x7fffffff);
    cpu.D[2] = 3;
    decode_and_run();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    TEST::SET_W(0, 0003200 | 2);
    TEST::SET_L(2, 0x80000000);
    cpu.D[2] = -1;
    decode_and_run();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(CX) {
    TEST::SET_W(0, 0003200 | 2);
    TEST::SET_L(2, 0xfffffffc);
    cpu.D[2] = 12;
    decode_and_run();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0003200 | 2);
    TEST::SET_L(2, 0xfffffffc);
    cpu.D[2] = 1;
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0003200 | 2);
    TEST::SET_L(2, 0xffffffff);
    cpu.D[2] = 1;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
