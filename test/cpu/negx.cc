#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(NEGX, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(err) {
    TEST::SET_W(0, 0040000 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}
BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
    TEST::SET_W(0, 0040000 | 2);
    cpu.D[2] = 0xfe;
    cpu.X = old_x;
    cpu.Z = true;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 2 - old_x);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(i == 0);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0040000 | 2);
    cpu.D[2] = 3;
    decode_and_run();
    BOOST_TEST(cpu.D[2] == (-3 & 0xff));
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0040000 | 2);
    cpu.D[2] = 0;
    cpu.Z = true;
    decode_and_run();
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(V) {
    TEST::SET_W(0, 0040000 | 2);
    cpu.D[2] = -128;
    decode_and_run();
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(err) {
    TEST::SET_W(0, 0040100 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}
BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
    TEST::SET_W(0, 0040100 | 2);
    cpu.D[2] = 0xfffe;
    cpu.X = old_x;
    cpu.Z = true;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 2 - old_x);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0040100 | 2);
    cpu.D[2] = 3;
    decode_and_run();
    BOOST_TEST(cpu.D[2] == (-3 & 0xffff));
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0040100 | 2);
    cpu.D[2] = 0;
    cpu.Z = true;
    decode_and_run();
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(V) {
    TEST::SET_W(0, 0040100 | 2);
    cpu.D[2] = 0x8000;
    decode_and_run();
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(err) {
    TEST::SET_W(0, 0040200 | 072);
    BOOST_CHECK_THROW(decode(), DecodeError);
}
BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
    TEST::SET_W(0, 0040200 | 2);
    cpu.D[2] = -5;
    cpu.X = old_x;
    cpu.Z = true;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 5 - old_x);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0040200 | 2);
    cpu.D[2] = 3;
    decode_and_run();
    BOOST_TEST(cpu.D[2] == -3);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0040200 | 2);
    cpu.D[2] = 0;
    cpu.Z = true;
    decode_and_run();
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(V) {
    TEST::SET_W(0, 0042200 | 2);
    cpu.D[2] = 0x80000000;
    decode_and_run();
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
