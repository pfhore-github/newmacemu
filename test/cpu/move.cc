#define BOOST_TEST_DYN_LINK
#include "68040.hpp"

#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(MOVE, Prepare)
struct F {
    F() {
        // MOVE.B %D2, %D3
        TEST::SET_W(0, 0010000 | 3 << 9 | 2);
        TEST::SET_W(2, TEST_BREAK);

        // MOVE.B (%A2), %D3
        TEST::SET_W(4, 0010020 | 3 << 9 | 2);
        TEST::SET_W(6, TEST_BREAK);

        // MOVE.B (%A2+), %D3
        TEST::SET_W(8, 0010030 | 3 << 9 | 2);
        TEST::SET_W(10, TEST_BREAK);

        // MOVE.B (%A7+), %D3
        TEST::SET_W(12, 0010030 | 3 << 9 | 7);
        TEST::SET_W(14, TEST_BREAK);

        // MOVE.B (-%A2), %D3
        TEST::SET_W(16, 0010040 | 3 << 9 | 2);
        TEST::SET_W(18, TEST_BREAK);

        // MOVE.B (-%A7), %D3
        TEST::SET_W(20, 0010040 | 3 << 9 | 7);
        TEST::SET_W(22, TEST_BREAK);

        // MOVE.B #0x34, %D3
        TEST::SET_W(24, 0010074 | 3 << 9);
        TEST::SET_W(26, 0x34);
        TEST::SET_W(28, TEST_BREAK);

        // MOVE.W %D2, %D3
        TEST::SET_W(30, 0030000 | 3 << 9 | 2);
        TEST::SET_W(32, TEST_BREAK);

        // MOVE.W %A2, %D3
        TEST::SET_W(34, 0030010 | 3 << 9 | 2);
        TEST::SET_W(36, TEST_BREAK);

        // MOVE.W (%A2), %D3
        TEST::SET_W(38, 0030020 | 3 << 9 | 2);
        TEST::SET_W(40, TEST_BREAK);

        // MOVE.W (%A2+), %D3
        TEST::SET_W(42, 0030030 | 3 << 9 | 2);
        TEST::SET_W(44, TEST_BREAK);

        // MOVE.W (-%A2), %D3
        TEST::SET_W(46, 0030040 | 3 << 9 | 2);
        TEST::SET_W(48, TEST_BREAK);

        // MOVE.W #0x3456, %D3
        TEST::SET_W(50, 0030074 | 3 << 9);
        TEST::SET_W(52, 0x3456);
        TEST::SET_W(54, TEST_BREAK);

        // MOVE.W %CCR, %D3
        TEST::SET_W(56, 0041300 | 3);
        TEST::SET_W(58, TEST_BREAK);

        // MOVE.W %SR, %D3
        TEST::SET_W(60, 0040300 | 3);
        TEST::SET_W(62, TEST_BREAK);

        // MOVE.L %D2, %D3
        TEST::SET_W(64, 0020000 | 3 << 9 | 2);
        TEST::SET_W(66, TEST_BREAK);

        // MOVE.L %A2, %D3
        TEST::SET_W(68, 0020010 | 3 << 9 | 2);
        TEST::SET_W(70, TEST_BREAK);

        // MOVE.L (%A2), %D3
        TEST::SET_W(72, 0020020 | 3 << 9 | 2);
        TEST::SET_W(74, TEST_BREAK);

        // MOVE.L (%A2+), %D3
        TEST::SET_W(76, 0020030 | 3 << 9 | 2);
        TEST::SET_W(78, TEST_BREAK);

        // MOVE.L (-%A2), %D3
        TEST::SET_W(80, 0020040 | 3 << 9 | 2);
        TEST::SET_W(82, TEST_BREAK);

        // MOVE.L #0x3456789A, %D3
        TEST::SET_W(84, 0020074 | 3 << 9);
        TEST::SET_L(86, 0x3456789A);
        TEST::SET_W(90, TEST_BREAK);

        // MOVE %USP, %A3
        TEST::SET_W(92, 0047150 | 3);
        TEST::SET_W(94, TEST_BREAK);

        // MOVE.B %D3, (%A2)
        TEST::SET_W(96, 0010200 | 2 << 9 | 3);
        TEST::SET_W(98, TEST_BREAK);

        // MOVE.B (0x100), (0x200)
        TEST::SET_W(100, 0011771);
        TEST::SET_L(102, 0x100);
        TEST::SET_L(106, 0x200);
        TEST::SET_W(110, TEST_BREAK);

        // MOVE.W %D3, (%A2)
        TEST::SET_W(112, 0030200 | 2 << 9 | 3);
        TEST::SET_W(114, TEST_BREAK);

        // MOVE.W (0x100), (0x200)
        TEST::SET_W(116, 0031771);
        TEST::SET_L(118, 0x100);
        TEST::SET_L(122, 0x200);
        TEST::SET_W(126, TEST_BREAK);

        // MOVE.W %D3, %CCR
        TEST::SET_W(128, 0042300 | 3);
        TEST::SET_W(130, TEST_BREAK);

        // MOVE.W %D3, %SR
        TEST::SET_W(132, 0043300 | 3);
        TEST::SET_W(134, TEST_BREAK);

        // MOVE.L %D3, (%A2)
        TEST::SET_W(136, 0020200 | 2 << 9 | 3);
        TEST::SET_W(138, TEST_BREAK);

        // MOVE.L (0x100), (0x200)
        TEST::SET_W(140, 0021771);
        TEST::SET_L(142, 0x100);
        TEST::SET_L(146, 0x200);
        TEST::SET_W(150, TEST_BREAK);

        // MOVE %A3, %USP
        TEST::SET_W(152, 0047140 | 3);
        TEST::SET_W(154, TEST_BREAK);

        jit_compile(0, 156);
    }
};
BOOST_AUTO_TEST_SUITE(R, *boost::unit_test::fixture<F>())
BOOST_AUTO_TEST_SUITE(Load)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(dr) {
    cpu.D[3] = 0x12345678;
    cpu.D[2] = 0x9ABCDE02;
    cpu.C = cpu.V = cpu.X = true;
    run_test(0);
    BOOST_TEST(cpu.D[3] == 0x12345602);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(cpu.X);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 0x80;
    run_test(0);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
    run_test(0);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(mem) {
    RAM[0x100] = 0x02;
    cpu.A[2] = 0x100;
    run_test(4);
    BOOST_TEST(cpu.D[3] == 0x02);
}

BOOST_AUTO_TEST_CASE(inc) {
    RAM[0x100] = 0x02;
    cpu.A[2] = 0x100;
    run_test(8);
    BOOST_TEST(cpu.D[3] == 0x02);
    BOOST_TEST(cpu.A[2] == 0x101);
}

BOOST_AUTO_TEST_CASE(pop) {
    RAM[0x100] = 0x02;
    cpu.A[7] = 0x100;
    run_test(12);
    BOOST_TEST(cpu.D[3] == 0x02);
    BOOST_TEST(cpu.A[7] == 0x102);
}

BOOST_AUTO_TEST_CASE(dec) {
    RAM[0x100] = 0x02;
    cpu.D[3] = 0x12345678;
    cpu.A[2] = 0x101;
    run_test(16);
    BOOST_TEST(cpu.D[3] == 0x12345602);
    BOOST_TEST(cpu.A[2] == 0x100);
}

BOOST_AUTO_TEST_CASE(push) {
    RAM[0x100] = 0x02;
    cpu.A[7] = 0x102;
    run_test(20);
    BOOST_TEST(cpu.D[3] == 0x02);
    BOOST_TEST(cpu.A[7] == 0x100);
}

BOOST_AUTO_TEST_CASE(imm) {
    cpu.D[3] = 0x12345678;
    run_test(24);
    BOOST_TEST(cpu.D[3] == 0x12345634);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(dr) {
    cpu.D[3] = 0x12345678;
    cpu.D[2] = 0x9ABC1234;
    cpu.C = cpu.V = cpu.X = true;
    run_test(30);
    BOOST_TEST(cpu.D[3] == 0x12341234);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(cpu.X);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 0x8000;
    run_test(30);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
    run_test(30);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(ar) {
    cpu.D[3] = 0x12345678;
    cpu.A[2] = 0x9ABCDE02;
    run_test(34);
    BOOST_TEST(cpu.D[3] == 0x1234DE02);
}

BOOST_AUTO_TEST_CASE(mem) {
    TEST::SET_W(0x100, 0xabcd);
    cpu.A[2] = 0x100;
    run_test(38);
    BOOST_TEST(cpu.D[3] == 0xabcd);
}

BOOST_AUTO_TEST_CASE(inc) {
    TEST::SET_W(0x100, 0xabcd);
    cpu.A[2] = 0x100;
    run_test(42);
    BOOST_TEST(cpu.D[3] == 0xabcd);
    BOOST_TEST(cpu.A[2] == 0x102);
}

BOOST_AUTO_TEST_CASE(dec) {
    TEST::SET_W(0x100, 0xabcd);
    cpu.A[2] = 0x102;
    run_test(46);
    BOOST_TEST(cpu.D[3] == 0xabcd);
    BOOST_TEST(cpu.A[2] == 0x100);
}

BOOST_AUTO_TEST_CASE(imm) {
    cpu.D[3] = 0x12345678;
    run_test(50);
    BOOST_TEST(cpu.D[3] == 0x12343456);
}

BOOST_AUTO_TEST_CASE(CCR) {
    cpu.X = cpu.N = cpu.Z = cpu.V = cpu.C = true;
    run_test(56);
    BOOST_TEST(cpu.D[3] == 0x1F);
}

BOOST_AUTO_TEST_SUITE(SR)
BOOST_AUTO_TEST_CASE(NG) {
    cpu.S = false;
    run_test(60);
    BOOST_TEST(cpu.ex_n == 8);
}

BOOST_AUTO_TEST_CASE(OK) {
    cpu.S = true;
    cpu.T = 0;
    cpu.M = false;
    cpu.X = cpu.N = cpu.Z = cpu.V = cpu.C = true;
    run_test(60);
    BOOST_TEST(cpu.D[3] == 0x201F);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(dr) {
    cpu.D[2] = 0x1ABC1234;
    cpu.C = cpu.V = cpu.X = true;
    run_test(64);
    BOOST_TEST(cpu.D[3] == 0x1ABC1234);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(cpu.X);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_CASE(N) {
    cpu.D[2] = 0x80000000;
    run_test(64);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.D[2] = 0;
    run_test(64);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(ar) {
    cpu.A[2] = 0x9ABCDE02;
    run_test(68);
    BOOST_TEST(cpu.D[3] == 0x9ABCDE02);
}

BOOST_AUTO_TEST_CASE(mem) {
    TEST::SET_L(0x100, 0xabcd0123);
    cpu.A[2] = 0x100;
    run_test(72);
    BOOST_TEST(cpu.D[3] == 0xabcd0123);
}

BOOST_AUTO_TEST_CASE(inc) {
    TEST::SET_L(0x100, 0xabcd0123);
    cpu.A[2] = 0x100;
    run_test(76);
    BOOST_TEST(cpu.D[3] == 0xabcd0123);
    BOOST_TEST(cpu.A[2] == 0x104);
}

BOOST_AUTO_TEST_CASE(dec) {
    TEST::SET_L(0x100, 0xabcd0123);
    cpu.A[2] = 0x104;
    run_test(80);
    BOOST_TEST(cpu.D[3] == 0xabcd0123);
    BOOST_TEST(cpu.A[2] == 0x100);
}

BOOST_AUTO_TEST_CASE(imm) {
    cpu.D[3] = 0x12345678;
    run_test(84);
    BOOST_TEST(cpu.D[3] == 0x3456789A);
}

BOOST_AUTO_TEST_SUITE(USP)

BOOST_AUTO_TEST_CASE(NG) {
    cpu.S = false;
    run_test(92);
    BOOST_TEST(cpu.ex_n == 8);
}

BOOST_AUTO_TEST_CASE(OK) {
    cpu.S = true;
    cpu.USP = 0x300;
    run_test(92);
    BOOST_TEST(cpu.A[3] == 0x300);
}

BOOST_AUTO_TEST_CASE(Traced) {
    cpu.S = true;
    cpu.T = 1;
    run_test(92);
    BOOST_TEST(cpu.ex_n == 9);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Store)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(mem) {
    RAM[0x100] = 0x02;
    cpu.D[3] = 0x12345678;
    cpu.A[2] = 0x100;
    run_test(96);
    BOOST_TEST(RAM[0x100] == 0x78);
}

BOOST_AUTO_TEST_CASE(mem2mem) {
    RAM[0x100] = 0x02;
    RAM[0x200] = 0xff;
    run_test(100);
    BOOST_TEST(RAM[0x200] == 0x02);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(mem) {
    TEST::SET_W(0x100, 0x0234);
    cpu.D[3] = 0x12345678;
    cpu.A[2] = 0x100;
    run_test(112);
    BOOST_TEST(TEST::GET_W(0x100) == 0x5678);
}

BOOST_AUTO_TEST_CASE(mem2mem) {
    TEST::SET_W(0x100, 0x1234);
    TEST::SET_W(0x200, 0x5678);
    run_test(116);
    BOOST_TEST(TEST::GET_W(0x200) == 0x1234);
}

BOOST_AUTO_TEST_CASE(CCR) {
    cpu.D[3] = 0x1F;
    run_test(128);
    BOOST_TEST(cpu.X);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(cpu.V);
    BOOST_TEST(cpu.N);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_SUITE(SR)

BOOST_AUTO_TEST_CASE(NG) {
    cpu.S = false;
    run_test(132);
    BOOST_TEST(cpu.ex_n == 8);
}

BOOST_AUTO_TEST_CASE(OK) {
    cpu.S = true;
    cpu.D[3] = 0x1F;
    run_test(132);
    BOOST_TEST(!cpu.S);
    BOOST_TEST(cpu.X);
    BOOST_TEST(cpu.N);
    BOOST_TEST(cpu.V);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(Traced) {
    cpu.S = true;
    cpu.T = 1;
    cpu.D[3] = 1 << 14;
    run_test(132);
    BOOST_TEST(cpu.ex_n == 9);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(mem) {
    TEST::SET_L(0x100, 0x02347654);
    cpu.D[3] = 0x12345678;
    cpu.A[2] = 0x100;
    run_test(136);
    BOOST_TEST(TEST::GET_L(0x100) == 0x12345678);
}

BOOST_AUTO_TEST_CASE(mem2mem) {
    TEST::SET_L(0x100, 0x12345678);
    TEST::SET_L(0x200, 0x56789ABC);
    run_test(140);
    BOOST_TEST(TEST::GET_L(0x200) == 0x12345678);
}
BOOST_AUTO_TEST_SUITE(USP)
BOOST_AUTO_TEST_CASE(NG) {
    cpu.S = false;
    run_test(152);
    BOOST_TEST(cpu.ex_n == 8);
}

BOOST_AUTO_TEST_CASE(OK) {
    cpu.S = true;
    cpu.A[3] = 0x300;
    run_test(152);
    BOOST_TEST(cpu.USP == 0x300);
}

BOOST_AUTO_TEST_CASE(Traced) {
    cpu.S = true;
    cpu.T = 1;
    run_test(152);
    BOOST_TEST(cpu.ex_n == 9);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
