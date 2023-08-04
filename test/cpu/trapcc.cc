#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <fmt/core.h>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
void trap_ng() {
    BOOST_TEST(run_test() == 7);
}
void trap_ok() {
    BOOST_TEST(run_test() == 0);
}

BOOST_FIXTURE_TEST_SUITE(TRAPcc, Prepare)

BOOST_AUTO_TEST_CASE(noext) {
    TEST::SET_W(0, 0050370 | 1 << 8 | 4);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
}

BOOST_AUTO_TEST_CASE(extW) {
    TEST::SET_W(0, 0050370 | 1 << 8 | 2);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
}

BOOST_AUTO_TEST_CASE(extL) {
    TEST::SET_W(0, 0050370 | 1 << 8 | 3);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 6);
}
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0050370 | 0 << 8 | 4);
    trap_ng();
}
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0050370 | 1 << 8 | 4);
    trap_ok();
}
BOOST_AUTO_TEST_SUITE(HI)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0050370 | 2 << 8 | 4);
    cpu.C = false;
    cpu.Z = false;
    trap_ng();
}
BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0050370 | 2 << 8 | 4);
    cpu.C = true;
    trap_ok();
}
BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0050370 | 2 << 8 | 4);
    cpu.Z = true;
    trap_ok();
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(LS)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0050370 | 3 << 8 | 4);
    cpu.C = true;
    trap_ng();
}
BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0050370 | 3 << 8 | 4);
    cpu.Z = true;
    trap_ng();
}
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0050370 | 3 << 8 | 4);
    cpu.Z = false;
    cpu.C = false;
    trap_ok();
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(CC)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0050370 | 4 << 8 | 4);
    cpu.C = false;
    trap_ng();
}
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0050370 | 4 << 8 | 4);
    cpu.C = true;
    trap_ok();
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(CS)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0050370 | 5 << 8 | 4);
    cpu.C = true;
    trap_ng();
}
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0050370 | 5 << 8 | 4);
    cpu.C = false;
    trap_ok();
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(NE)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0050370 | 6 << 8 | 4);
    cpu.Z = false;
    trap_ng();
}
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0050370 | 6 << 8 | 4);
    cpu.Z = true;
    trap_ok();
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(EQ)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0050370 | 7 << 8 | 4);
    cpu.Z = true;
    trap_ng();
}
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0050370 | 7 << 8 | 4);
    cpu.Z = false;
    trap_ok();
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(VC)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0050370 | 8 << 8 | 4);
    cpu.V = false;
    trap_ng();
}
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0050370 | 8 << 8 | 4);
    cpu.V = true;
    trap_ok();
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(VS)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0050370 | 9 << 8 | 4);
    cpu.V = true;
    trap_ng();
}
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0050370 | 9 << 8 | 4);
    cpu.V = false;
    trap_ok();
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(PL)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0050370 | 10 << 8 | 4);
    cpu.N = false;
    trap_ng();
}
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0050370 | 10 << 8 | 4);
    cpu.N = true;
    trap_ok();
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(MI)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0050370 | 11 << 8 | 4);
    cpu.N = true;
    trap_ng();
}
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0050370 | 11 << 8 | 4);
    cpu.N = false;
    trap_ok();
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GE)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0050370 | 12 << 8 | 4);
    cpu.N = false;
    cpu.V = false;
    trap_ng();
}

BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0050370 | 12 << 8 | 4);
    cpu.N = true;
    cpu.V = true;
    trap_ng();
}

BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0050370 | 12 << 8 | 4);
    cpu.N = true;
    cpu.V = false;
    trap_ok();
}

BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0050370 | 12 << 8 | 4);
    cpu.N = false;
    cpu.V = true;
    trap_ok();
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(LT)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0050370 | 13 << 8 | 4);
    cpu.N = true;
    cpu.V = false;
    trap_ng();
}
BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0050370 | 13 << 8 | 4);
    cpu.N = false;
    cpu.V = true;
    trap_ng();
}
BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0050370 | 13 << 8 | 4);
    cpu.N = false;
    cpu.V = false;
    trap_ok();
}

BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0050370 | 13 << 8 | 4);
    cpu.N = true;
    cpu.V = true;
    trap_ok();
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GT)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0050370 | 14 << 8 | 4);
    cpu.N = false;
    cpu.V = false;
    cpu.Z = false;
    trap_ng();
}

BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0050370 | 14 << 8 | 4);
    cpu.N = true;
    cpu.V = true;
    cpu.Z = false;
    trap_ng();
}

BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0050370 | 14 << 8 | 4);
    cpu.N = true;
    cpu.V = false;
    trap_ok();
}

BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0050370 | 14 << 8 | 4);
    cpu.N = false;
    cpu.V = true;
    trap_ok();
}

BOOST_AUTO_TEST_CASE(F3) {
    TEST::SET_W(0, 0050370 | 14 << 8 | 4);
    cpu.Z = true;
    trap_ok();
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(LE)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0050370 | 15 << 8 | 4);
    cpu.Z = true;
    trap_ng();
}
BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0050370 | 15 << 8 | 4);
    cpu.N = false;
    cpu.V = true;
    trap_ng();
}
BOOST_AUTO_TEST_CASE(T3) {
    TEST::SET_W(0, 0050370 | 15 << 8 | 4);
    cpu.N = true;
    cpu.V = false;
    trap_ng();
}
BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0050370 | 15 << 8 | 4);
    cpu.Z = false;
    cpu.N = false;
    cpu.V = false;
    trap_ok();
}

BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0050370 | 15 << 8 | 4);
    cpu.Z = false;
    cpu.N = true;
    cpu.V = true;
    trap_ok();
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
