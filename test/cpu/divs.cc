#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "proto.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(DIVS, Prepare)
BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(value) {
    auto [q, r] = DIVS_W(-40, 3);
    BOOST_TEST(q == -13);
    BOOST_TEST(r == -1);
}

BOOST_AUTO_TEST_CASE(N) {
    DIVS_W(-40, 3);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    DIVS_W(2, 3);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(V1) {
    DIVS_W(0x200000, 2);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    DIVS_W(0x80000000, -1);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(Div0) {
    if(setjmp(cpu.ex_buf) == 0) {
        DIVS_W(3, 0);
        BOOST_ERROR("exception unoccured");
    } else {
        BOOST_TEST(cpu.ex_n == 5);
    }
}

BOOST_AUTO_TEST_CASE(operand) {
    TEST::SET_W(0, 0100700 | 3 << 9 | 2);
    cpu.D[3] = -40;
    cpu.D[2] = 3;
    int i = decode_and_run();
    BOOST_TEST(cpu.D[3] == (0xffff0000 | (-13 & 0xffff)));
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(value) {
    auto [q, r] = DIVS_L(-200000, -7);
    BOOST_TEST(q == 28571);
    BOOST_TEST(r == -3);
}

BOOST_AUTO_TEST_CASE(N) {
    DIVS_L(-200000, 3);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    DIVS_L(2, 3);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(V) {
    DIVS_L(0x80000000, -1);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(Div0) {
    if(setjmp(cpu.ex_buf) == 0) {
        DIVS_L(3, 0);
        BOOST_ERROR("exception unoccured");
    } else {
        BOOST_TEST(cpu.ex_n == 5);
    }
}

BOOST_AUTO_TEST_CASE(operand) {
    TEST::SET_W(0, 0046100 | 2);
    TEST::SET_W(2, 0004000 | 3 << 12 | 4);
    cpu.D[3] = -200000;
    cpu.D[2] = 7;
    int i = decode_and_run();
    BOOST_TEST(cpu.D[3] == -28571);
    BOOST_TEST(cpu.D[4] == -3);
    BOOST_TEST(i == 2);
}

BOOST_AUTO_TEST_CASE(operand_nomod) {
    TEST::SET_W(0, 0046100 | 2);
    TEST::SET_W(2, 0004000 | 3 << 12 | 3);
    cpu.D[3] = -200000;
    cpu.D[2] = 7;
    decode_and_run();
    BOOST_TEST(cpu.D[3] == -28571);
}



BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Quad)

BOOST_AUTO_TEST_CASE(value) {
    auto [q, r] = DIVS_LL(-1000000000, -3);
    BOOST_TEST(q == 333333333);
    BOOST_TEST(r == -1);
}

BOOST_AUTO_TEST_CASE(N) {
    DIVS_LL(-200000, 3);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    DIVS_LL(2, 3);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(V) {
    DIVS_LL(0x200000000LL, -1);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(Div0) {
    if(setjmp(cpu.ex_buf) == 0) {
        DIVS_LL(3, 0);
        BOOST_ERROR("exception unoccured");
    } else {
        BOOST_TEST(cpu.ex_n == 5);
    }
}
BOOST_AUTO_TEST_CASE(operand) {
    TEST::SET_W(0, 0046100 | 2);
    TEST::SET_W(2, 0006000 | 3 << 12 | 4);
    cpu.D[3] = 0;
    cpu.D[4] = -1;
    cpu.D[2] = -3;
    int i = decode_and_run();
    BOOST_TEST(cpu.D[3] == 1431655765);
    BOOST_TEST(cpu.D[4] == -1);
    BOOST_TEST(i == 2);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
