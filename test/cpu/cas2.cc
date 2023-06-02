#define BOOST_TEST_DYN_LINK
#include "inline.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_AUTO_TEST_SUITE(CAS2)

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0006374 );
    TEST::SET_W(2, 1 << 15 | 2 << 12 | 3 << 6 | 4);
    TEST::SET_W(4, 1 << 15 | 5 << 12 | 6 << 6 | 7);
    TEST::SET_W(0x1000, 0x2222);
    TEST::SET_W(0x1100, 0x3333);
    cpu.A[2] = 0x1000;
    cpu.A[5] = 0x1100;
    cpu.D[3] = 0x3030;
    cpu.D[4] = 0x2222;
    cpu.D[6] = 0x4040;
    cpu.D[7] = 0x3333;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(TEST::GET_W(0x1000) == 0x3030);
    BOOST_TEST(TEST::GET_W(0x1100) == 0x4040);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 4);
}
BOOST_AUTO_TEST_CASE(F1_C) {
   TEST::SET_W(0, 0006374 );
    TEST::SET_W(2, 1 << 15 | 2 << 12 | 3 << 6 | 4);
    TEST::SET_W(4, 1 << 15 | 5 << 12 | 6 << 6 | 7);
    TEST::SET_W(0x1000, 0x2222);
    TEST::SET_W(0x1100, 0x3333);
    cpu.A[2] = 0x1000;
    cpu.A[5] = 0x1100;
    cpu.D[3] = 0x3030;
    cpu.D[4] = 0x2227;
    cpu.D[6] = 0x4040;
    cpu.D[7] = 0x3333;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.D[4]  == 0x2222);
    BOOST_TEST(cpu.D[7] == 0x3333);
    BOOST_TEST(cpu.C);
}


BOOST_AUTO_TEST_CASE(F1_N) {
   TEST::SET_W(0, 0006374 );
    TEST::SET_W(2, 1 << 15 | 2 << 12 | 3 << 6 | 4);
    TEST::SET_W(4, 1 << 15 | 5 << 12 | 6 << 6 | 7);
    TEST::SET_W(0x1000, 0xA222);
    TEST::SET_W(0x1100, 0x3333);
    cpu.A[2] = 0x1000;
    cpu.A[5] = 0x1100;
    cpu.D[3] = 0x3030;
    cpu.D[4] = 1;
    cpu.D[6] = 0x4040;
    cpu.D[7] = 0x3333;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.D[4] == 0xA222);
    BOOST_TEST(cpu.D[7] == 0x3333);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(F1_V) {
   TEST::SET_W(0, 0006374 );
    TEST::SET_W(2, 1 << 15 | 2 << 12 | 3 << 6 | 4);
    TEST::SET_W(4, 1 << 15 | 5 << 12 | 6 << 6 | 7);
    TEST::SET_W(0x1000, 0x8000);
    TEST::SET_W(0x1100, 0x3333);
    cpu.A[2] = 0x1000;
    cpu.A[5] = 0x1100;
    cpu.D[3] = 0x3030;
    cpu.D[4] = 1;
    cpu.D[6] = 0x4040;
    cpu.D[7] = 0x3333;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.D[4] == 0x8000);
    BOOST_TEST(cpu.D[7] == 0x3333);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(F2_C) {
   TEST::SET_W(0, 0006374 );
    TEST::SET_W(2, 1 << 15 | 2 << 12 | 3 << 6 | 4);
    TEST::SET_W(4, 1 << 15 | 5 << 12 | 6 << 6 | 7);
    TEST::SET_W(0x1000, 0x2222);
    TEST::SET_W(0x1100, 0x3333);
    cpu.A[2] = 0x1000;
    cpu.A[5] = 0x1100;
    cpu.D[3] = 0x3030;
    cpu.D[4] = 0x2222;
    cpu.D[6] = 0x4040;
    cpu.D[7] = 0x3339;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.D[4]  == 0x2222);
    BOOST_TEST(cpu.D[7] == 0x3333);
    BOOST_TEST(cpu.C);
}


BOOST_AUTO_TEST_CASE(F2_N) {
   TEST::SET_W(0, 0006374 );
    TEST::SET_W(2, 1 << 15 | 2 << 12 | 3 << 6 | 4);
    TEST::SET_W(4, 1 << 15 | 5 << 12 | 6 << 6 | 7);
    TEST::SET_W(0x1000, 0x2222);
    TEST::SET_W(0x1100, 0xA333);
    cpu.A[2] = 0x1000;
    cpu.A[5] = 0x1100;
    cpu.D[3] = 0x3030;
    cpu.D[4] = 0x2222;
    cpu.D[6] = 0x4040;
    cpu.D[7] = 1;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.D[4] == 0x2222);
    BOOST_TEST(cpu.D[7] == 0xA333);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(F2_V) {
   TEST::SET_W(0, 0006374 );
    TEST::SET_W(2, 1 << 15 | 2 << 12 | 3 << 6 | 4);
    TEST::SET_W(4, 1 << 15 | 5 << 12 | 6 << 6 | 7);
    TEST::SET_W(0x1000, 0x2222);
    TEST::SET_W(0x1100, 0x8000);
    cpu.A[2] = 0x1000;
    cpu.A[5] = 0x1100;
    cpu.D[3] = 0x3030;
    cpu.D[4] = 0x2222;
    cpu.D[6] = 0x4040;
    cpu.D[7] = 1;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.D[4] == 0x2222);
    BOOST_TEST(cpu.D[7] == 0x8000);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(T) {
 TEST::SET_W(0, 0007374 );
    TEST::SET_W(2, 1 << 15 | 2 << 12 | 3 << 6 | 4);
    TEST::SET_W(4, 1 << 15 | 5 << 12 | 6 << 6 | 7);
    TEST::SET_L(0x1000, 0x22222222);
    TEST::SET_L(0x1100, 0x33333333);
    cpu.A[2] = 0x1000;
    cpu.A[5] = 0x1100;
    cpu.D[3] = 0x30303030;
    cpu.D[4] = 0x22222222;
    cpu.D[6] = 0x40404040;
    cpu.D[7] = 0x33333333;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(TEST::GET_L(0x1000) == 0x30303030);
    BOOST_TEST(TEST::GET_L(0x1100) == 0x40404040);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 4);
}

BOOST_AUTO_TEST_CASE(F1_C) {
   TEST::SET_W(0, 0007374 );
    TEST::SET_W(2, 1 << 15 | 2 << 12 | 3 << 6 | 4);
    TEST::SET_W(4, 1 << 15 | 5 << 12 | 6 << 6 | 7);
    TEST::SET_L(0x1000, 0x22222222);
    TEST::SET_L(0x1100, 0x33333333);
    cpu.A[2] = 0x1000;
    cpu.A[5] = 0x1100;
    cpu.D[3] = 0x30303030;
    cpu.D[4] = 0x22222227;
    cpu.D[6] = 0x40404040;
    cpu.D[7] = 0x33333333;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.D[4] == 0x22222222);
    BOOST_TEST(cpu.D[7] == 0x33333333);
    BOOST_TEST(cpu.C);
}


BOOST_AUTO_TEST_CASE(F1_N) {
   TEST::SET_W(0, 0007374 );
    TEST::SET_W(2, 1 << 15 | 2 << 12 | 3 << 6 | 4);
    TEST::SET_W(4, 1 << 15 | 5 << 12 | 6 << 6 | 7);
    TEST::SET_L(0x1000, 0xA2222222);
    TEST::SET_L(0x1100, 0x33333333);
    cpu.A[2] = 0x1000;
    cpu.A[5] = 0x1100;
    cpu.D[3] = 0x30303030;
    cpu.D[4] = 1;
    cpu.D[6] = 0x40404040;
    cpu.D[7] = 0x33333333;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.D[4] == 0xA2222222);
    BOOST_TEST(cpu.D[7] == 0x33333333);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(F1_V) {
   TEST::SET_W(0, 0007374 );
    TEST::SET_W(2, 1 << 15 | 2 << 12 | 3 << 6 | 4);
    TEST::SET_W(4, 1 << 15 | 5 << 12 | 6 << 6 | 7);
    TEST::SET_L(0x1000, 0x80000000);
    TEST::SET_L(0x1100, 0x33333333);
    cpu.A[2] = 0x1000;
    cpu.A[5] = 0x1100;
    cpu.D[3] = 0x30303030;
    cpu.D[4] = 1;
    cpu.D[6] = 0x40404040;
    cpu.D[7] = 0x33333333;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.D[4] == 0x80000000);
    BOOST_TEST(cpu.D[7] == 0x33333333);
    BOOST_TEST(cpu.V);
}


BOOST_AUTO_TEST_CASE(F2_C) {
   TEST::SET_W(0, 0007374 );
    TEST::SET_W(2, 1 << 15 | 2 << 12 | 3 << 6 | 4);
    TEST::SET_W(4, 1 << 15 | 5 << 12 | 6 << 6 | 7);
    TEST::SET_L(0x1000, 0x22222222);
    TEST::SET_L(0x1100, 0x33333333);
    cpu.A[2] = 0x1000;
    cpu.A[5] = 0x1100;
    cpu.D[3] = 0x30303030;
    cpu.D[4] = 0x22222222;
    cpu.D[6] = 0x40404040;
    cpu.D[7] = 0x33333339;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.D[4]  == 0x22222222);
    BOOST_TEST(cpu.D[7] == 0x33333333);
    BOOST_TEST(cpu.C);
}


BOOST_AUTO_TEST_CASE(F2_N) {
   TEST::SET_W(0, 0007374 );
    TEST::SET_W(2, 1 << 15 | 2 << 12 | 3 << 6 | 4);
    TEST::SET_W(4, 1 << 15 | 5 << 12 | 6 << 6 | 7);
    TEST::SET_L(0x1000, 0x22222222);
    TEST::SET_L(0x1100, 0xA3333333);
    cpu.A[2] = 0x1000;
    cpu.A[5] = 0x1100;
    cpu.D[3] = 0x30303030;
    cpu.D[4] = 0x22222222;
    cpu.D[6] = 0x40404040;
    cpu.D[7] = 1;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.D[4] == 0x22222222);
    BOOST_TEST(cpu.D[7] == 0xA3333333);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(F2_V) {
   TEST::SET_W(0, 0007374 );
    TEST::SET_W(2, 1 << 15 | 2 << 12 | 3 << 6 | 4);
    TEST::SET_W(4, 1 << 15 | 5 << 12 | 6 << 6 | 7);
    TEST::SET_L(0x1000, 0x22222222);
    TEST::SET_L(0x1100, 0x80000000);
    cpu.A[2] = 0x1000;
    cpu.A[5] = 0x1100;
    cpu.D[3] = 0x30303030;
    cpu.D[4] = 0x22222222;
    cpu.D[6] = 0x40404040;
    cpu.D[7] = 1;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.D[4] == 0x22222222);
    BOOST_TEST(cpu.D[7] == 0x80000000);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
