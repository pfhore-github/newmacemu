#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(CAS, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0005320 | 2);
    TEST::SET_W(2, 3 << 6 | 4);
    RAM[0x1000] = 0x22;
    cpu.A[2] = 0x1000;
    cpu.D[3] = 0x30;
    cpu.D[4] = 0x22;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(RAM[0x1000] == 0x30);
}

BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0005320 | 2);
    TEST::SET_W(2, 3 << 6 | 4);
    RAM[0x1000] = 0x22;
    cpu.A[2] = 0x1000;
    cpu.D[3] = 0x30;
    cpu.D[4] = 0x35;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.D[4] == 0x22);
}

BOOST_AUTO_TEST_CASE(traced) {
    TEST::SET_W(0, 0005320 | 2);
    TEST::SET_W(2, 3 << 6 | 4);
    cpu.T = 1;
    BOOST_TEST(run_test() == 9);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)


BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0006320 | 2);
    TEST::SET_W(2, 3 << 6 | 4);
    TEST::SET_W(0x1000, 0x2222);
    cpu.A[2] = 0x1000;
    cpu.D[3] = 0x3030;
    cpu.D[4] = 0x2222;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(TEST::GET_W(0x1000) == 0x3030);
}

BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0006320 | 2);
    TEST::SET_W(2, 3 << 6 | 4);
    TEST::SET_W(0x1000, 0x2222);
    cpu.A[2] = 0x1000;
    cpu.D[3] = 0x3030;
    cpu.D[4] = 0x3535;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.D[4] == 0x2222);
}

BOOST_AUTO_TEST_CASE(traced) {
    TEST::SET_W(0, 0006320 | 2);
    TEST::SET_W(2, 3 << 6 | 4);
    cpu.T = 1;
    BOOST_TEST(run_test() == 9);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)


BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0007320 | 2);
    TEST::SET_W(2, 3 << 6 | 4);
    TEST::SET_L(0x1000, 0x22222222);
    cpu.A[2] = 0x1000;
    cpu.D[3] = 0x30303030;
    cpu.D[4] = 0x35353535;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(cpu.D[4] == 0x22222222);
}

BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0007320 | 2);
    TEST::SET_W(2, 3 << 6 | 4);
    TEST::SET_L(0x1000, 0x22222222);
    cpu.A[2] = 0x1000;
    cpu.D[3] = 0x30303030;
    cpu.D[4] = 0x22222222;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(TEST::GET_L(0x1000) == 0x30303030);
}

BOOST_AUTO_TEST_CASE(traced) {
    TEST::SET_W(0, 0007320 | 2);
    TEST::SET_W(2, 3 << 6 | 4);
    cpu.T = 1;
    BOOST_TEST(run_test() == 9);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
