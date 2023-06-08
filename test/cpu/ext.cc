#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(EXT, Prepare)
BOOST_AUTO_TEST_SUITE(B2W)
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0044200 | 2 );
    cpu.D[2] = 0xFF;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0xFFFF);
    BOOST_TEST(cpu.N);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0044200 | 2 );
    cpu.D[2] = 2;
    decode_and_run();
    BOOST_TEST(!cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0044200 | 2 );
    cpu.D[2] = 0;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(W2L)
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0044300 | 2 );
    cpu.D[2] = 0xFFFF;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0xFFFFFFFF);
    BOOST_TEST(cpu.N);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0044300 | 2 );
    cpu.D[2] = 2;
    decode_and_run();
    BOOST_TEST(!cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0044300 | 2 );
    cpu.D[2] = 0;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(B2L)
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0044700 | 2 );
    cpu.D[2] = 0xFF;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0xFFFFFFFF);
    BOOST_TEST(cpu.N);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0044700 | 2 );
    cpu.D[2] = 2;
    decode_and_run();
    BOOST_TEST(!cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0044700 | 2 );
    cpu.D[2] = 0;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()