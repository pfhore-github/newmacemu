#define BOOST_TEST_DYN_LINK
#include "inline.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(ABCD, Prepare)
BOOST_AUTO_TEST_SUITE(Reg)
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0140400 | 3 << 9 | 1);
    cpu.D[3] = 0x22;
    cpu.D[1] = 0x34;
    cpu.X = true;
    cpu.Z = true;
    int i = decode_and_run();
    BOOST_TEST(cpu.D[3] == 0x57);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0140400 | 3 << 9 | 1);
    cpu.Z = true;
    cpu.D[3] = 0;
    cpu.D[1] = 0;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(CX) {
    TEST::SET_W(0, 0140400 | 3 << 9 | 1);
    cpu.D[3] = 0x94;
    cpu.D[1] = 0x17;
    decode_and_run();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Mem)
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0140410 | 3 << 9 | 1);
    RAM[0x1000] = 0x22;
    RAM[0x1100] = 0x34;
    cpu.A[3] = 0x1001;
    cpu.A[1] = 0x1101;
    cpu.X = true;
    cpu.Z = true;
    int i = decode_and_run();
    BOOST_TEST(RAM[0x1000] == 0x57);
    BOOST_TEST(cpu.A[3] == 0x1000);
    BOOST_TEST(cpu.A[1] == 0x1100);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0140410 | 3 << 9 | 1);
    cpu.Z = true;
    RAM[0x1000] = 0;
    RAM[0x1100] = 0;
    cpu.A[3] = 0x1001;
    cpu.A[1] = 0x1101;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(CX) {
    TEST::SET_W(0, 0140410 | 3 << 9 | 1);
    RAM[0x1000] = 0x94;
    RAM[0x1100] = 0x17;
    cpu.A[3] = 0x1001;
    cpu.A[1] = 0x1101;
    decode_and_run();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
