#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <fmt/core.h>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(Bcc, Prepare)

BOOST_AUTO_TEST_CASE(untraced) {
    TEST::SET_W(0, 0060000 | 4 << 8 | 4);
    cpu.T = 0;
    cpu.C = false;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 6);
    BOOST_TEST(!cpu.must_trace);
}

BOOST_AUTO_TEST_CASE(traced) {
    TEST::SET_W(0, 0060000 | 4 << 8 | 4);
    cpu.T = 1;
    cpu.C = false;
    BOOST_TEST(run_test() == 9);
}

BOOST_AUTO_TEST_CASE(trace_not_taken) {
    TEST::SET_W(0, 0060000 | 4 << 8 | 4);
    cpu.T = 1;
    cpu.C = true;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(!cpu.must_trace);
}

BOOST_AUTO_TEST_CASE(offset1) {
    TEST::SET_W(0, 0060000 | 4 << 8 | 4);
    cpu.C = false;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 6);
}

BOOST_AUTO_TEST_CASE(offsetNeg) {
    TEST::SET_W(0, 0060000 | 4 << 8 | 0xf0);
    cpu.C = false;
    run_test();
    BOOST_TEST(cpu.PC == -14);
}

BOOST_AUTO_TEST_CASE(offset00) {
    TEST::SET_W(0, 0060000 | 4 << 8 | 0);
    TEST::SET_W(2, 0x1000);
    cpu.C = false;
    run_test();
    BOOST_TEST(cpu.PC == 0x1002);
}
BOOST_AUTO_TEST_CASE(offsetFF) {
    TEST::SET_W(0, 0060000 | 4 << 8 | 0xFF);
    TEST::SET_L(2, 0x20000);
    cpu.C = false;
    run_test();
    BOOST_TEST(cpu.PC == 0x20002);
}
BOOST_AUTO_TEST_SUITE(HI)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0060000 | 2 << 8 | 4);
    cpu.C = false;
    cpu.Z = false;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}
BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0060000 | 2 << 8 | 4);
    cpu.C = true;
    run_test();
    BOOST_TEST(cpu.PC == 2);
}
BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0060000 | 2 << 8 | 4);
    cpu.Z = true;
    run_test();
    BOOST_TEST(cpu.PC == 2);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(LS)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0060000 | 3 << 8 | 4);
    cpu.C = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}
BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0060000 | 3 << 8 | 4);
    cpu.Z = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0060000 | 3 << 8 | 4);
    cpu.Z = false;
    cpu.C = false;
    run_test();
    BOOST_TEST(cpu.PC == 2);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(CC)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0060000 | 4 << 8 | 4);
    cpu.C = false;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0060000 | 4 << 8 | 4);
    cpu.C = true;
    run_test();
    BOOST_TEST(cpu.PC == 2);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(CS)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0060000 | 5 << 8 | 4);
    cpu.C = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0060000 | 5 << 8 | 4);
    cpu.C = false;
    run_test();
    BOOST_TEST(cpu.PC == 2);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(NE)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0060000 | 6 << 8 | 4);
    cpu.Z = false;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0060000 | 6 << 8 | 4);
    cpu.Z = true;
    run_test();
    BOOST_TEST(cpu.PC == 2);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(EQ)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0060000 | 7 << 8 | 4);
    cpu.Z = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0060000 | 7 << 8 | 4);
    cpu.Z = false;
    run_test();
    BOOST_TEST(cpu.PC == 2);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(VC)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0060000 | 8 << 8 | 4);
    cpu.V = false;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0060000 | 8 << 8 | 4);
    cpu.V = true;
    run_test();
    BOOST_TEST(cpu.PC == 2);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(VS)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0060000 | 9 << 8 | 4);
    cpu.V = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0060000 | 9 << 8 | 4);
    cpu.V = false;
    run_test();
    BOOST_TEST(cpu.PC == 2);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(PL)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0060000 | 10 << 8 | 4);
    cpu.N = false;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0060000 | 10 << 8 | 4);
    cpu.N = true;
    run_test();
    BOOST_TEST(cpu.PC == 2);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(MI)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0060000 | 11 << 8 | 4);
    cpu.N = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0060000 | 11 << 8 | 4);
    cpu.N = false;
    run_test();
    BOOST_TEST(cpu.PC == 2);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GE)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0060000 | 12 << 8 | 4);
    cpu.N = false;
    cpu.V = false;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}

BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0060000 | 12 << 8 | 4);
    cpu.N = true;
    cpu.V = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}

BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0060000 | 12 << 8 | 4);
    cpu.N = true;
    cpu.V = false;
    run_test();
    BOOST_TEST(cpu.PC == 2);
}

BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0060000 | 12 << 8 | 4);
    cpu.N = false;
    cpu.V = true;
    run_test();
    BOOST_TEST(cpu.PC == 2);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(LT)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0060000 | 13 << 8 | 4);
    cpu.N = true;
    cpu.V = false;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}
BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0060000 | 13 << 8 | 4);
    cpu.N = false;
    cpu.V = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}
BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0060000 | 13 << 8 | 4);
    cpu.N = false;
    cpu.V = false;
    run_test();
    BOOST_TEST(cpu.PC == 2);
}

BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0060000 | 13 << 8 | 4);
    cpu.N = true;
    cpu.V = true;
    run_test();
    BOOST_TEST(cpu.PC == 2);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GT)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0060000 | 14 << 8 | 4);
    cpu.N = false;
    cpu.V = false;
    cpu.Z = false;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}

BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0060000 | 14 << 8 | 4);
    cpu.N = true;
    cpu.V = true;
    cpu.Z = false;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}

BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0060000 | 14 << 8 | 4);
    cpu.N = true;
    cpu.V = false;
    run_test();
    BOOST_TEST(cpu.PC == 2);
}

BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0060000 | 14 << 8 | 4);
    cpu.N = false;
    cpu.V = true;
    run_test();
    BOOST_TEST(cpu.PC == 2);
}

BOOST_AUTO_TEST_CASE(F3) {
    TEST::SET_W(0, 0060000 | 14 << 8 | 4);
    cpu.Z = true;
    run_test();
    BOOST_TEST(cpu.PC == 2);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(LE)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0060000 | 15 << 8 | 4);
    cpu.Z = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}
BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0060000 | 15 << 8 | 4);
    cpu.N = false;
    cpu.V = true;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}
BOOST_AUTO_TEST_CASE(T3) {
    TEST::SET_W(0, 0060000 | 15 << 8 | 4);
    cpu.N = true;
    cpu.V = false;
    run_test();
    BOOST_TEST(cpu.PC == 6);
}
BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0060000 | 15 << 8 | 4);
    cpu.Z = false;
    cpu.N = false;
    cpu.V = false;
    run_test();
    BOOST_TEST(cpu.PC == 2);
}

BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0060000 | 15 << 8 | 4);
    cpu.Z = false;
    cpu.N = true;
    cpu.V = true;
    run_test();
    BOOST_TEST(cpu.PC == 2);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
