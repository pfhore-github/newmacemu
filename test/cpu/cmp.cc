#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "inline.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(CMP, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(V1) {
    CMP_B(127, -3);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    CMP_B(-126, 7);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(C) {
    CMP_B(-6, -5);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(N) {
    CMP_B(-4, 1);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    CMP_B(1, 1);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(execute) {
    TEST::SET_W(0, 0130000 | 3 << 9 | 2);
    run_test();
    BOOST_TEST(cpu.PC == 2);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(V1) {
    CMP_W(0x7fff, -3);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    CMP_W(0x8002, 7);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(C) {
    CMP_W(-600, -500);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(N) {
    CMP_W(-400, 1);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    CMP_W(1000, 1000);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(execute) {
    TEST::SET_W(0, 0130100 | 3 << 9 | 2);
    run_test();
    BOOST_TEST(cpu.PC == 2);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(V1) {
    CMP_L(0x7fffffff, -3);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    CMP_L(0x80000002, 7);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(C) {
    CMP_L(-60000, -50000);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(N) {
    CMP_L(-400000, 1);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    CMP_L(100000, 100000);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(execute) {
    TEST::SET_W(0, 0130200 | 3 << 9 | 2);
    run_test();
    BOOST_TEST(cpu.PC == 2);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
