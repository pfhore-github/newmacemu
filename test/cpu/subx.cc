#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(SUBX, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_SUITE(Reg)
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0110400 | 3 << 9 | 1);
    cpu.D[1] = 52;
    cpu.D[3] = 15;
    cpu.X = true;
    cpu.Z = true;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[1] == 36);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0110400 | 3 << 9 | 1);
    cpu.D[1] = 3;
    cpu.D[3] = 3;
    cpu.Z = true;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(V1) {
    TEST::SET_W(0, 0110400 | 3 << 9 | 1);
    cpu.D[1] = 0x80;
    cpu.D[3] = 1;
    decode_and_run();
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(V2) {
    TEST::SET_W(0, 0110400 | 3 << 9 | 1);
    cpu.D[1] = 0x7f;
    cpu.D[3] = -1;
    decode_and_run();
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(CX) {
    TEST::SET_W(0, 0110400 | 3 << 9 | 1);
    cpu.X = false;
    cpu.D[1] = 0;
    cpu.D[3] = 1;
    decode_and_run();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0110400 | 3 << 9 | 1);
    cpu.X = false;
    cpu.D[1] = -10;
    cpu.D[3] = 1;
    decode_and_run();
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Mem)
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0110410 | 3 << 9 | 1);
    RAM[0x1000] = 60;
    RAM[0x1100] = 34;
    cpu.A[1] = 0x1001;
    cpu.A[3] = 0x1101;
    cpu.X = true;
    cpu.Z = true;
    auto i = decode_and_run();
    BOOST_TEST(RAM[0x1000] == 25);
    BOOST_TEST(cpu.A[1] == 0x1000);
    BOOST_TEST(cpu.A[3] == 0x1100);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0110410 | 3 << 9 | 1);
    RAM[0x1000] = 30;
    RAM[0x1100] = 30;
    cpu.A[1] = 0x1101;
    cpu.A[3] = 0x1001;
    cpu.Z = true;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(CX) {
    TEST::SET_W(0, 0110410 | 3 << 9 | 1);
    RAM[0x1000] = 0;
    RAM[0x1100] = 1;
    cpu.A[1] = 0x1001;
    cpu.A[3] = 0x1101;
    decode_and_run();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(V1) {
    TEST::SET_W(0, 0110410 | 3 << 9 | 1);
    RAM[0x1000] = 0x80;
    RAM[0x1100] = 1;
    cpu.A[1] = 0x1001;
    cpu.A[3] = 0x1101;
    decode_and_run();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    TEST::SET_W(0, 0110410 | 3 << 9 | 1);
    RAM[0x1000] = 0x7f;
    RAM[0x1100] = -1;
    cpu.A[1] = 0x1001;
    cpu.A[3] = 0x1101;
    decode_and_run();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0110410 | 3 << 9 | 1);
    RAM[0x1000] = -20;
    RAM[0x1100] = 1;
    cpu.A[1] = 0x1001;
    cpu.A[3] = 0x1101;
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_SUITE(Reg)
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0110500 | 3 << 9 | 1);
    cpu.D[1] = 4050;
    cpu.D[3] = 2010;
    cpu.X = true;
    cpu.Z = true;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[1] == 2039);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0110500 | 3 << 9 | 1);
    cpu.D[1] = 300;
    cpu.D[3] = 300;
    cpu.Z = true;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(V1) {
    TEST::SET_W(0, 0110500 | 3 << 9 | 1);
    cpu.D[1] = 0x8000;
    cpu.D[3] = 1;
    decode_and_run();
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(V2) {
    TEST::SET_W(0, 0110500 | 3 << 9 | 1);
    cpu.D[1] = 0x7fff;
    cpu.D[3] = -1;
    decode_and_run();
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(CX) {
    TEST::SET_W(0, 0110500 | 3 << 9 | 1);
    cpu.D[1] = 0;
    cpu.D[3] = 1;
    decode_and_run();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0110500 | 3 << 9 | 1);
    cpu.D[1] = -5000;
    cpu.D[3] = 1000;
    decode_and_run();
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Mem)
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0110510 | 3 << 9 | 1);
    TEST::SET_W(0x1000, 6789);
    TEST::SET_W(0x1100, 3434);
    cpu.A[1] = 0x1002;
    cpu.A[3] = 0x1102;
    cpu.Z = true;
    cpu.X = true;
    auto i = decode_and_run();
    BOOST_TEST(TEST::GET_W(0x1000) == 3354);
    BOOST_TEST(cpu.A[1] == 0x1000);
    BOOST_TEST(cpu.A[3] == 0x1100);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0110510 | 3 << 9 | 1);
    TEST::SET_W(0x1000, 1000);
    TEST::SET_W(0x1100, 1000);
    cpu.A[1] = 0x1002;
    cpu.A[3] = 0x1102;
    cpu.Z = true;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(CX) {
    TEST::SET_W(0, 0110510 | 3 << 9 | 1);
    TEST::SET_W(0x1000, 0);
    TEST::SET_W(0x1100, 1);
    cpu.A[1] = 0x1002;
    cpu.A[3] = 0x1102;
    decode_and_run();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(V1) {
    TEST::SET_W(0, 0110510 | 3 << 9 | 1);
    TEST::SET_W(0x1000, 0x8000);
    TEST::SET_W(0x1100, 1);
    cpu.A[1] = 0x1002;
    cpu.A[3] = 0x1102;
    decode_and_run();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    TEST::SET_W(0, 0110510 | 3 << 9 | 1);
    TEST::SET_W(0x1000, 0x7fff);
    TEST::SET_W(0x1100, -1);
    cpu.A[1] = 0x1002;
    cpu.A[3] = 0x1102;
    decode_and_run();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0110510 | 3 << 9 | 1);
    TEST::SET_W(0x1000, -200);
    TEST::SET_W(0x1100, 1);
    cpu.A[1] = 0x1002;
    cpu.A[3] = 0x1102;
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_SUITE(Reg)
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0110600 | 3 << 9 | 1);
    cpu.D[1] = 987654;
    cpu.D[3] = 112233;
    cpu.X = true;
    cpu.Z = true;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[1] == 875420);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0110600 | 3 << 9 | 1);
    cpu.D[1] = 100000;
    cpu.D[3] = 100000;
    cpu.Z = true;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(V1) {
    TEST::SET_W(0, 0110600 | 3 << 9 | 1);
    cpu.X = false;
    cpu.D[1] = 0x80000000;
    cpu.D[3] = 1;
    decode_and_run();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    TEST::SET_W(0, 0110600 | 3 << 9 | 1);
    cpu.X = false;
    cpu.D[1] = 0x7fffffff;
    cpu.D[3] = -1;
    decode_and_run();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(CX) {
    TEST::SET_W(0, 0110600 | 3 << 9 | 1);
    cpu.X = false;
    cpu.D[1] = 0;
    cpu.D[3] = 1;
    decode_and_run();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0110600 | 3 << 9 | 1);
    cpu.X = false;
    cpu.D[1] = -5000000;
    cpu.D[3] = 1;
    decode_and_run();
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Mem)
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0110610 | 3 << 9 | 1);
    TEST::SET_L(0x1000, 87654321);
    TEST::SET_L(0x1100, 11223344);
    cpu.A[1] = 0x1004;
    cpu.A[3] = 0x1104;
    cpu.Z = true;
    cpu.X = true;
    auto i = decode_and_run();
    BOOST_TEST(TEST::GET_L(0x1000) == 76430976);
    BOOST_TEST(cpu.A[1] == 0x1000);
    BOOST_TEST(cpu.A[3] == 0x1100);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0110610 | 3 << 9 | 1);
    TEST::SET_L(0x1000, 400000);
    TEST::SET_L(0x1100, 400000);
    cpu.A[1] = 0x1004;
    cpu.A[3] = 0x1104;
    cpu.Z = true;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(CX) {
    TEST::SET_W(0, 0110610 | 3 << 9 | 1);
    TEST::SET_L(0x1000, 0);
    TEST::SET_L(0x1100, 1);
    cpu.A[1] = 0x1004;
    cpu.A[3] = 0x1104;
    decode_and_run();
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(V1) {
    TEST::SET_W(0, 0110610 | 3 << 9 | 1);
    TEST::SET_L(0x1000, 0x80000000);
    TEST::SET_L(0x1100, 1);
    cpu.A[1] = 0x1004;
    cpu.A[3] = 0x1104;
    decode_and_run();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(V2) {
    TEST::SET_W(0, 0110610 | 3 << 9 | 1);
    TEST::SET_L(0x1000, 0x7fffffff);
    TEST::SET_L(0x1100, -1);
    cpu.A[1] = 0x1004;
    cpu.A[3] = 0x1104;
    decode_and_run();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0110610 | 3 << 9 | 1);
    TEST::SET_L(0x1000, -5000000);
    TEST::SET_L(0x1100, 0);
    cpu.A[1] = 0x1004;
    cpu.A[3] = 0x1104;
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
