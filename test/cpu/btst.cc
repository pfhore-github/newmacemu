#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include "proto.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(BTST, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_DATA_TEST_CASE(Z, bdata::xrange(2), z) {
    BTST_B(z << 3, 3);
    BOOST_TEST(cpu.Z != z);
}
BOOST_AUTO_TEST_CASE(ByImm) {
    TEST::SET_W(0, 0004020 | 2);
    TEST::SET_W(2, 3);
    RAM[0x1000] = 0;
    cpu.A[2] = 0x1000;
    auto i = decode_and_run();
    BOOST_TEST(i == 2);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(ByReg) {
    TEST::SET_W(0, 0000420 | 2 | 4 << 9);
    cpu.D[4] = 3;
    RAM[0x1000] = 0;
    cpu.A[2] = 0x1000;
    auto i = decode_and_run();
    BOOST_TEST(i == 0);
    BOOST_TEST(cpu.Z);
}


BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_DATA_TEST_CASE(Z, bdata::xrange(2), z) {
    BTST_L(z << 20, 20);
    BOOST_TEST(cpu.Z != z);
}
BOOST_AUTO_TEST_CASE(ByImm) {
    TEST::SET_W(0, 0004000 | 2);
    TEST::SET_W(2, 20);
    cpu.D[2] = 0;
    auto i = decode_and_run();
    BOOST_TEST(i == 2);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(ByReg) {
    TEST::SET_W(0, 0000400 | 2 | 4 << 9);
    cpu.D[2] = 0;
    cpu.D[4] = 20;
    auto i = decode_and_run();
    BOOST_TEST(i == 0);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()