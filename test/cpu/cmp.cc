#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(CMP, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(size) {
    TEST::SET_W(0, 0130000 | 3 << 9 | 2);
    auto[_, i] = decode();
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(V1) {
    TEST::SET_W(0, 0130000 | 3 << 9 | 2);
    cpu.D[3] = 127;
    cpu.D[2] = -3;
    decode_and_run();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    TEST::SET_W(0, 0130000 | 3 << 9 | 2);
    cpu.D[3] = -126;
    cpu.D[2] = 5;
    decode_and_run();
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(C) {
    TEST::SET_W(0, 0130000 | 3 << 9 | 2);
    cpu.D[3] = 2;
    cpu.D[2] = 3;
    decode_and_run();
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0130000 | 3 << 9 | 2);
    cpu.D[3] = 0xfc;
    cpu.D[2] = 1;
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0130000 | 3 << 9 | 2);
    cpu.D[3] = 1;
    cpu.D[2] = 1;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(size) {
    TEST::SET_W(0, 0130100 | 3 << 9 | 2);
    auto[_, i] = decode();
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(V1) {
    TEST::SET_W(0, 0130100 | 3 << 9 | 2);
    cpu.D[3] = 0x7fff;
    cpu.D[2] = -3;
    decode_and_run();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    TEST::SET_W(0, 0130100 | 3 << 9 | 2);
    cpu.D[3] = 0x8000;
    cpu.D[2] = 1;
    decode_and_run();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(C) {
    TEST::SET_W(0, 0130100 | 3 << 9 | 2);
    cpu.D[3] = 1000;
    cpu.D[2] = 2000;
    decode_and_run();
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0130100 | 3 << 9 | 2);
    cpu.D[3] = 0xfffc;
    cpu.D[2] = 1;
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0130100 | 3 << 9 | 2);
    cpu.D[3] = 1;
    cpu.D[2] = 1;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(size) {
    TEST::SET_W(0, 0130200 | 3 << 9 | 2);
    auto [_, i] = decode();
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(V1) {
    TEST::SET_W(0, 0130200 | 3 << 9 | 2);
    cpu.D[3] = 0x7fffffff;
    cpu.D[2] = -3;
    decode_and_run();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    TEST::SET_W(0, 0130200 | 3 << 9 | 2);
    cpu.D[3] = 0x80000000;
    cpu.D[2] = 1;
    decode_and_run();
    BOOST_TEST(cpu.V);
}


BOOST_AUTO_TEST_CASE(C) {
    TEST::SET_W(0, 0130200 | 3 << 9 | 2);
    cpu.D[3] = 50000;
    cpu.D[2] = 70000;
    decode_and_run();
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0130200 | 3 << 9 | 2);
    cpu.D[3] = 0xfffffffc;
    cpu.D[2] = 1;
   decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0130200 | 3 << 9 | 2);
    cpu.D[3] = 1;
    cpu.D[2] = 1;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
