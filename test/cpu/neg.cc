#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(NEG, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(err) {
    TEST::SET_W(0, 0042000 | 072 );
    BOOST_CHECK_THROW(decode(), DecodeError);
}
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0042000 | 2 );
    cpu.D[2] = 0xff;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 1);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(i == 0);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0042000 | 2 );
    cpu.D[2] = 3;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == (-3 & 0xff) );
    BOOST_TEST(cpu.N);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0042000 | 2 );
    cpu.D[2] = 0;
    decode_and_run();
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(V) {
    TEST::SET_W(0, 0042000 | 2 );
    cpu.D[2] = -128;
    decode_and_run();
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(err) {
    TEST::SET_W(0, 0042100 | 072 );
    BOOST_CHECK_THROW(decode(), DecodeError);
}
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0042100 | 2 );
    cpu.D[2] = 0xffff;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 1);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0042100 | 2 );
    cpu.D[2] = 3;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == (-3 & 0xffff) );
    BOOST_TEST(cpu.N);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0042100 | 2 );
    cpu.D[2] = 0;
    decode_and_run();
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(V) {
    TEST::SET_W(0, 0042100 | 2 );
    cpu.D[2] = 0x8000;
    decode_and_run();
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(err) {
    TEST::SET_W(0, 0042200 | 072 );
    BOOST_CHECK_THROW(decode(), DecodeError);
}
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0042200 | 2 );
    cpu.D[2] = -5;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 5);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0042200 | 2 );
    cpu.D[2] = 3;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == -3 );
    BOOST_TEST(cpu.N);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0042200 | 2 );
    cpu.D[2] = 0;
    decode_and_run();
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(V) {
    TEST::SET_W(0, 0042200 | 2 );
    cpu.D[2] = 0x80000000;
    decode_and_run();
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
