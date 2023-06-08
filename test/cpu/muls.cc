#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(MULS, Prepare)
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0140700 | 3 << 9 | 2);
    cpu.D[3] = -70;
    cpu.D[2] = -30;
    int i = decode_and_run();
    BOOST_TEST(cpu.D[3] == 2100);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0140700 | 3 << 9 | 2);
    cpu.D[3] = -40;
    cpu.D[2] = 3;
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0140700 | 3 << 9 | 2);
    cpu.D[3] = 0;
    cpu.D[2] = 3;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0046000 | 2);
    TEST::SET_W(2, 0004000 | 3 << 12);
    cpu.D[3] = -2000;
    cpu.D[2] = -7000;
    int i = decode_and_run();
    BOOST_TEST(cpu.D[3] == 14000000);
    BOOST_TEST(i == 2);
}


BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0046000 | 2);
    TEST::SET_W(2, 0004000 | 3 << 12);
    cpu.D[3] = -2000;
    cpu.D[2] = 7000;
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0046000 | 2);
    TEST::SET_W(2, 0004000 | 3 << 12 );
    cpu.D[3] = 0;
    cpu.D[2] = 7;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(V) {
    TEST::SET_W(0, 0046000 | 2);
    TEST::SET_W(2, 0004000 | 3 << 12);
    cpu.D[3] = 0x40000000;
    cpu.D[2] = 0x40000000;
    decode_and_run();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Quad)
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0046000 | 2);
    TEST::SET_W(2, 0006000 | 3 << 12 | 4);
    cpu.D[3] = 0x40000000;
    cpu.D[2] = 4;
    int i = decode_and_run();
    BOOST_TEST(cpu.D[3] == 0);
    BOOST_TEST(cpu.D[4] == 1);
    BOOST_TEST(i == 2);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0046000 | 2);
    TEST::SET_W(2, 0006000 | 3 << 12 | 4);
    cpu.D[3] = 0x40000000;
    cpu.D[2] = -4;
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0046000 | 2);
    TEST::SET_W(2, 0006000 | 3 << 12 | 4);
    cpu.D[3] = 0x40000000;
    cpu.D[2] = 0;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
