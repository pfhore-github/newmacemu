#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(CMP2, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_SUITE(Unsigned)
BOOST_AUTO_TEST_CASE(In) {
    TEST::SET_W(0, 0000320 | 2);
    TEST::SET_W(2, 3 << 12);
    RAM[0x1000] = 70;
    RAM[0x1001] = 230;
    cpu.A[2] = 0x1000;
    cpu.D[3] = 150;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(Leq) {
    TEST::SET_W(0, 0000320 | 2);
    TEST::SET_W(2, 3 << 12);
    RAM[0x1000] = 70;
    RAM[0x1001] = 230;
    cpu.A[2] = 0x1000;
    cpu.D[3] = 70;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(Geq) {
    TEST::SET_W(0, 0000320 | 2);
    TEST::SET_W(2, 3 << 12);
    RAM[0x1000] = 70;
    RAM[0x1001] = 230;
    cpu.A[2] = 0x1000;
    cpu.D[3] = 230;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.C);
}
BOOST_AUTO_TEST_CASE(Under) {
    TEST::SET_W(0, 0000320 | 2);
    TEST::SET_W(2, 3 << 12);
    RAM[0x1000] = 70;
    RAM[0x1001] = 230;
    cpu.A[2] = 0x1000;
    cpu.D[3] = 20;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(Over) {
    TEST::SET_W(0, 0000320 | 2);
    TEST::SET_W(2, 3 << 12);
    RAM[0x1000] = 70;
    RAM[0x1001] = 230;
    cpu.A[2] = 0x1000;
    cpu.D[3] = 250;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.C);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Signed)
BOOST_AUTO_TEST_CASE(In) {
    TEST::SET_W(0, 0000320 | 2);
    TEST::SET_W(2, 1 << 15 | 3 << 12);
    RAM[0x1000] = -70;
    RAM[0x1001] = 80;
    cpu.A[2] = 0x1000;
    cpu.A[3] = 10;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(Leq) {
    TEST::SET_W(0, 0000320 | 2);
    TEST::SET_W(2, 1 << 15 | 3 << 12);
    RAM[0x1000] = -70;
    RAM[0x1001] = 80;
    cpu.A[2] = 0x1000;
    cpu.A[3] = -70;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(Geq) {
    TEST::SET_W(0, 0000320 | 2);
    TEST::SET_W(2, 1 << 15 | 3 << 12);
    RAM[0x1000] = -70;
    RAM[0x1001] = 80;
    cpu.A[2] = 0x1000;
    cpu.A[3] = 80;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.C);
}
BOOST_AUTO_TEST_CASE(Under) {
    TEST::SET_W(0, 0000320 | 2);
    TEST::SET_W(2, 1 << 15 | 3 << 12);
    RAM[0x1000] = -70;
    RAM[0x1001] = 80;
    cpu.A[2] = 0x1000;
    cpu.A[3] = -100;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(Over) {
    TEST::SET_W(0, 0000320 | 2);
    TEST::SET_W(2, 1 << 15 | 3 << 12);
    RAM[0x1000] = -70;
    RAM[0x1001] = 80;
    cpu.A[2] = 0x1000;
    cpu.A[3] = 100;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.C);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_SUITE(Unsigned)
BOOST_AUTO_TEST_CASE(In) {
    TEST::SET_W(0, 0001320 | 2);
    TEST::SET_W(2, 3 << 12);
    TEST::SET_W(0x1000, 7000);
    TEST::SET_W(0x1002, 23000);
    cpu.A[2] = 0x1000;
    cpu.D[3] = 15000;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(Leq) {
    TEST::SET_W(0, 0001320 | 2);
    TEST::SET_W(2, 3 << 12);
    TEST::SET_W(0x1000, 7000);
    TEST::SET_W(0x1002, 23000);
    cpu.A[2] = 0x1000;
    cpu.D[3] = 7000;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(Geq) {
    TEST::SET_W(0, 0001320 | 2);
    TEST::SET_W(2, 3 << 12);
    TEST::SET_W(0x1000, 7000);
    TEST::SET_W(0x1002, 23000);
    cpu.A[2] = 0x1000;
    cpu.D[3] = 23000;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.C);
}
BOOST_AUTO_TEST_CASE(Under) {
    TEST::SET_W(0, 0001320 | 2);
    TEST::SET_W(2, 3 << 12);
    TEST::SET_W(0x1000, 7000);
    TEST::SET_W(0x1002, 23000);
    cpu.A[2] = 0x1000;
    cpu.D[3] = 2000;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(Over) {
    TEST::SET_W(0, 0001320 | 2);
    TEST::SET_W(2, 3 << 12);
    TEST::SET_W(0x1000, 7000);
    TEST::SET_W(0x1002, 23000);
    cpu.A[2] = 0x1000;
    cpu.D[3] = 25000;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.C);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Signed)
BOOST_AUTO_TEST_CASE(In) {
    TEST::SET_W(0, 0001320 | 2);
    TEST::SET_W(2, 1 << 15 | 3 << 12);
    TEST::SET_W(0x1000, -700);
    TEST::SET_W(0x1002, 2300);
    cpu.A[2] = 0x1000;
    cpu.A[3] = 100;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(Leq) {
    TEST::SET_W(0, 0001320 | 2);
    TEST::SET_W(2, 1 << 15 | 3 << 12);
    TEST::SET_W(0x1000, -700);
    TEST::SET_W(0x1002, 2300);
    cpu.A[2] = 0x1000;
    cpu.A[3] = -700;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(Geq) {
    TEST::SET_W(0, 0001320 | 2);
    TEST::SET_W(2, 1 << 15 | 3 << 12);
    TEST::SET_W(0x1000, -700);
    TEST::SET_W(0x1002, 2300);
    cpu.A[2] = 0x1000;
    cpu.A[3] = 2300;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.C);
}
BOOST_AUTO_TEST_CASE(Under) {
    TEST::SET_W(0, 0001320 | 2);
    TEST::SET_W(2, 1 << 15 | 3 << 12);
    TEST::SET_W(0x1000, -700);
    TEST::SET_W(0x1002, 2300);
    cpu.A[2] = 0x1000;
    cpu.A[3] = -1000;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(Over) {
    TEST::SET_W(0, 0001320 | 2);
    TEST::SET_W(2, 1 << 15 | 3 << 12);
    TEST::SET_W(0x1000, -700);
    TEST::SET_W(0x1002, 2300);
    cpu.A[2] = 0x1000;
    cpu.A[3] = 2600;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.C);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_SUITE(Unsigned)
BOOST_AUTO_TEST_CASE(In) {
    TEST::SET_W(0, 0002320 | 2);
    TEST::SET_W(2, 3 << 12);
    TEST::SET_L(0x1000, 700000);
    TEST::SET_L(0x1004, 2300000);
    cpu.A[2] = 0x1000;
    cpu.D[3] = 1500000;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(Leq) {
    TEST::SET_W(0, 0002320 | 2);
    TEST::SET_W(2, 3 << 12);
    TEST::SET_L(0x1000, 700000);
    TEST::SET_L(0x1004, 2300000);
    cpu.A[2] = 0x1000;
    cpu.D[3] = 700000;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(Geq) {
    TEST::SET_W(0, 0002320 | 2);
    TEST::SET_W(2, 3 << 12);
    TEST::SET_L(0x1000, 700000);
    TEST::SET_L(0x1004, 2300000);
    cpu.A[2] = 0x1000;
    cpu.D[3] = 2300000;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.C);
}
BOOST_AUTO_TEST_CASE(Under) {
    TEST::SET_W(0, 0002320 | 2);
    TEST::SET_W(2, 3 << 12);
    TEST::SET_L(0x1000, 700000);
    TEST::SET_L(0x1004, 2300000);
    cpu.A[2] = 0x1000;
    cpu.D[3] = 200000;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(Over) {
    TEST::SET_W(0, 0002320 | 2);
    TEST::SET_W(2, 3 << 12);
    TEST::SET_L(0x1000, 700000);
    TEST::SET_L(0x1004, 2300000);
    cpu.A[2] = 0x1000;
    cpu.D[3] = 25000000;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.C);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Signed)
BOOST_AUTO_TEST_CASE(In) {
    TEST::SET_W(0, 0002320 | 2);
    TEST::SET_W(2, 1 << 15 | 3 << 12);
    TEST::SET_L(0x1000, -70000);
    TEST::SET_L(0x1004, 230000);
    cpu.A[2] = 0x1000;
    cpu.A[3] = 10000;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(Leq) {
    TEST::SET_W(0, 0002320 | 2);
    TEST::SET_W(2, 1 << 15 | 3 << 12);
    TEST::SET_L(0x1000, -70000);
    TEST::SET_L(0x1004, 230000);
    cpu.A[2] = 0x1000;
    cpu.A[3] = -70000;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(Geq) {
    TEST::SET_W(0, 0002320 | 2);
    TEST::SET_W(2, 1 << 15 | 3 << 12);
    TEST::SET_L(0x1000, -70000);
    TEST::SET_L(0x1004, 230000);
    cpu.A[2] = 0x1000;
    cpu.A[3] = 230000;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(!cpu.C);
}
BOOST_AUTO_TEST_CASE(Under) {
    TEST::SET_W(0, 0001320 | 2);
    TEST::SET_W(2, 1 << 15 | 3 << 12);
    TEST::SET_L(0x1000, -70000);
    TEST::SET_L(0x1004, 230000);
    cpu.A[2] = 0x1000;
    cpu.A[3] = -100000;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(Over) {
    TEST::SET_W(0, 0002320 | 2);
    TEST::SET_W(2, 1 << 15 | 3 << 12);
    TEST::SET_L(0x1000, -70000);
    TEST::SET_L(0x1004, 230000);
    cpu.A[2] = 0x1000;
    cpu.A[3] = 260000;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.C);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
