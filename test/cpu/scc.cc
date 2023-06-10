#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(Scc, Prepare)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0050300 | 0 << 8 | 4);
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[4] == 0xff);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0050300 | 1 << 8 | 4);
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0);
}
BOOST_AUTO_TEST_SUITE(HI)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0050300 | 2 << 8 | 4);
    cpu.C = false;
    cpu.Z = false;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0xff);
}
BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0050300 | 2 << 8 | 4);
    cpu.C = true;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0);
}
BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0050300 | 2 << 8 | 4);
    cpu.Z = true;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(LS)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0050300 | 3 << 8 | 4);
    cpu.C = true;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0xff);
}
BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0050300 | 3 << 8 | 4);
    cpu.Z = true;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0xff);
}
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0050300 | 3 << 8 | 4);
    cpu.Z = false;
    cpu.C = false;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(CC)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0050300 | 4 << 8 | 4);
    cpu.C = false;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0xff);
}
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0050300 | 4 << 8 | 4);
    cpu.C = true;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(CS)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0050300 | 5 << 8 | 4);
    cpu.C = true;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0xff);
}
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0050300 | 5 << 8 | 4);
    cpu.C = false;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(NE)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0050300 | 6 << 8 | 4);
    cpu.Z = false;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0xff);
}
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0050300 | 6 << 8 | 4);
    cpu.Z = true;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(EQ)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0050300 | 7 << 8 | 4);
    cpu.Z = true;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0xff);
}
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0050300 | 7 << 8 | 4);
    cpu.Z = false;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0);
}
BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(VC)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0050300 | 8 << 8 | 4);
    cpu.V = false;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0xff);
}
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0050300 | 8 << 8 | 4);
    cpu.V = true;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(VS)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0050300 | 9 << 8 | 4);
    cpu.V = true;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0xff);
}
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0050300 | 9 << 8 | 4);
    cpu.V = false;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(PL)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0050300 | 10 << 8 | 4);
    cpu.N = false;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0xff);
}
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0050300 | 10 << 8 | 4);
    cpu.N = true;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(MI)
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0050300 | 11 << 8 | 4);
    cpu.N = true;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0xff);
}
BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0050300 | 11 << 8 | 4);
    cpu.N = false;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0);
}
BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(GE)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0050300 | 12 << 8 | 4);
    cpu.N = false;
    cpu.V = false;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0xff);
}

BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0050300 | 12 << 8 | 4);
    cpu.N = true;
    cpu.V = true;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0xff);
}

BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0050300 | 12 << 8 | 4);
    cpu.N = true;
    cpu.V = false;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0);
}

BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0050300 | 12 << 8 | 4);
    cpu.N = false;
    cpu.V = true;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(LT)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0050300 | 13 << 8 | 4);
    cpu.N = true;
    cpu.V = false;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0xff);
}
BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0050300 | 13 << 8 | 4);
    cpu.N = false;
    cpu.V = true;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0xff);
}
BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0050300 | 13 << 8 | 4);
    cpu.N = false;
    cpu.V = false;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0);
}

BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0050300 | 13 << 8 | 4);
    cpu.N = true;
    cpu.V = true;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0);
}
BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(GT)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0050300 | 14 << 8 | 4);
    cpu.N = false;
    cpu.V = false;
    cpu.Z = false;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0xff);
}

BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0050300 | 14 << 8 | 4);
    cpu.N = true;
    cpu.V = true;
    cpu.Z = false;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0xff);
}

BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0050300 | 14 << 8 | 4);
    cpu.N = true;
    cpu.V = false;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0);
}

BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0050300 | 14 << 8 | 4);
    cpu.N = false;
    cpu.V = true;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0);
}

BOOST_AUTO_TEST_CASE(F3) {
    TEST::SET_W(0, 0050300 | 14 << 8 | 4);
    cpu.Z = true;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(LE)
BOOST_AUTO_TEST_CASE(T1) {
    TEST::SET_W(0, 0050300 | 15 << 8 | 4);
    cpu.Z = true;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0xff);
}
BOOST_AUTO_TEST_CASE(T2) {
    TEST::SET_W(0, 0050300 | 15 << 8 | 4);
    cpu.N = false;
    cpu.V = true;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0xff);
}
BOOST_AUTO_TEST_CASE(T3) {
    TEST::SET_W(0, 0050300 | 15 << 8 | 4);
    cpu.N = true;
    cpu.V = false;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0xff);
}
BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0050300 | 15 << 8 | 4);
    cpu.Z = false;
    cpu.N = false;
    cpu.V = false;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0);
}

BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0050300 | 15 << 8 | 4);
    cpu.Z = false;
    cpu.N = true;
    cpu.V = true;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
