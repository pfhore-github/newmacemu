#define BOOST_TEST_DYN_LINK
#include "68040.hpp"

#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_LEA {
    F_LEA() {
        // LEA (%A3), %A5
        TEST::SET_W(0, 0040720 | 5 << 9 | 3);
        TEST::SET_W(2, TEST_BREAK);

        // LEA (0x10, %A3), %A5
        TEST::SET_W(4, 0040750 | 5 << 9 | 3);
        TEST::SET_W(6, 0x10);
        TEST::SET_W(8, TEST_BREAK);

        // LEA (0x10, %A3, %D2.W), %A5
        TEST::SET_W(10, 0040760 | 5 << 9 | 3);
        TEST::SET_W(12, 2 << 12 | 0 << 11 | 0x10);
        TEST::SET_W(14, TEST_BREAK);

        // LEA (0x10, %A3, %D2*sc), %A5
        for(int sc = 0; sc < 4; ++sc) {
            TEST::SET_W(16 + 6 * sc, 0040760 | 5 << 9 | 3);
            TEST::SET_W(16 + 6 * sc + 2, 2 << 12 | 1 << 11 | sc << 9 | 0x10);
            TEST::SET_W(16 + 6 * sc + 4, TEST_BREAK);
        }

        // LEA (%A3, %D2), %A5
        TEST::SET_W(40, 0040760 | 5 << 9 | 3);
        TEST::SET_W(42, 2 << 12 | 1 << 11 | 1 << 8 | 1 << 4);
        TEST::SET_W(44, TEST_BREAK);

        // LEA (0x10.W, %A3, %D2), %A5
        TEST::SET_W(46, 0040760 | 5 << 9 | 3);
        TEST::SET_W(48, 2 << 12 | 1 << 11 | 1 << 8 | 2 << 4);
        TEST::SET_W(50, 0x10);
        TEST::SET_W(52, TEST_BREAK);

        // LEA (0x10.L, %A3, %D2), %A5
        TEST::SET_W(54, 0040760 | 5 << 9 | 3);
        TEST::SET_W(56, 2 << 12 | 1 << 11 | 1 << 8 | 3 << 4);
        TEST::SET_L(58, 0x10);
        TEST::SET_W(62, TEST_BREAK);

        // LEA (0x10.W, %D2), %A5
        TEST::SET_W(64, 0040760 | 5 << 9 | 3);
        TEST::SET_W(66, 2 << 12 | 1 << 11 | 1 << 8 | 1 << 7 | 2 << 4);
        TEST::SET_W(68, 0x10);
        TEST::SET_W(70, TEST_BREAK);

        // LEA (0x10.W, %A3), %A5
        TEST::SET_W(72, 0040760 | 5 << 9 | 3);
        TEST::SET_W(74, 2 << 12 | 1 << 11 | 1 << 8 | 1 << 6 | 2 << 4);
        TEST::SET_W(76, 0x10);
        TEST::SET_W(78, TEST_BREAK);

        // LEA ([0x10.W, %A3, %D2]), %A5
        TEST::SET_W(80, 0040760 | 5 << 9 | 3);
        TEST::SET_W(82, 2 << 12 | 1 << 11 | 1 << 8 | 2 << 4 | 1);
        TEST::SET_W(84, 0x10);
        TEST::SET_W(86, TEST_BREAK);

        // LEA ([0x10.W, %A3, %D2], 0x30.W), %A5
        TEST::SET_W(88, 0040760 | 5 << 9 | 3);
        TEST::SET_W(90, 2 << 12 | 1 << 11 | 1 << 8 | 2 << 4 | 2);
        TEST::SET_W(92, 0x10);
        TEST::SET_W(94, 0x30);
        TEST::SET_W(96, TEST_BREAK);


        // LEA ([0x10.W, %A3, %D2], 0x30.L), %A5
        TEST::SET_W(98, 0040760 | 5 << 9 | 3);
        TEST::SET_W(100, 2 << 12 | 1 << 11 | 1 << 8 | 2 << 4 | 3);
        TEST::SET_W(102, 0x10);
        TEST::SET_L(104, 0x30);
        TEST::SET_W(108, TEST_BREAK);

        // LEA ([0x10.W, %A3], %D2), %A5
        TEST::SET_W(110, 0040760 | 5 << 9 | 3);
        TEST::SET_W(112, 2 << 12 | 1 << 11 | 1 << 8 | 2 << 4 | 1 << 2 | 1);
        TEST::SET_W(114, 0x10);
        TEST::SET_W(116, TEST_BREAK);

        // LEA ([0x10.W, %A3], %D2, 0x30.W), %A5
        TEST::SET_W(118, 0040760 | 5 << 9 | 3);
        TEST::SET_W(120, 2 << 12 | 1 << 11 | 1 << 8 | 2 << 4 | 1 << 2 | 2);
        TEST::SET_W(122, 0x10);
        TEST::SET_W(124, 0x30);
        TEST::SET_W(126, TEST_BREAK);


        // LEA ([0x10.W, %A3], %D2, 0x30.L), %A5
        TEST::SET_W(128, 0040760 | 5 << 9 | 3);
        TEST::SET_W(130, 2 << 12 | 1 << 11 | 1 << 8 | 2 << 4 | 1 << 2 | 3);
        TEST::SET_W(132, 0x10);
        TEST::SET_L(134, 0x30);
        TEST::SET_W(138, TEST_BREAK);

        // LEA (0x100).W, %A5
        TEST::SET_W(140, 0040770 | 5 << 9 );
        TEST::SET_W(142, 0x100);
        TEST::SET_W(144, TEST_BREAK);

        // LEA (0x100).L, %A5
        TEST::SET_W(146, 0040771 | 5 << 9 );
        TEST::SET_L(148, 0x100);
        TEST::SET_W(152, TEST_BREAK);

         // LEA (0x10, %PC), %A5
        TEST::SET_W(154, 0040772 | 5 << 9 );
        TEST::SET_W(156, 0x10);
        TEST::SET_W(158, TEST_BREAK);

        // LEA (0x10, %PC, %D2.W), %A5
        TEST::SET_W(160, 0040773 | 5 << 9 );
        TEST::SET_W(162, 2 << 12 | 0 << 11 | 0x10);
        TEST::SET_W(164, TEST_BREAK);

        // LEA (0x10, %PC, %D2*sc), %A5
        for(int sc = 0; sc < 4; ++sc) {
            TEST::SET_W(166 + 6 * sc, 0040773 | 5 << 9 );
            TEST::SET_W(166 + 6 * sc + 2, 2 << 12 | 1 << 11 | sc << 9 | 0x10);
            TEST::SET_W(166 + 6 * sc + 4, TEST_BREAK);
        }

        // LEA (%PC, %D2), %A5
        TEST::SET_W(190, 0040773 | 5 << 9 );
        TEST::SET_W(192, 2 << 12 | 1 << 11 | 1 << 8 | 1 << 4);
        TEST::SET_W(194, TEST_BREAK);

        // LEA (0x10.W, %PC, %D2), %A5
        TEST::SET_W(196, 0040773 | 5 << 9 );
        TEST::SET_W(198, 2 << 12 | 1 << 11 | 1 << 8 | 2 << 4);
        TEST::SET_W(200, 0x10);
        TEST::SET_W(202, TEST_BREAK);


        // LEA (0x10.L, %PC, %D2), %A5
        TEST::SET_W(204, 0040773 | 5 << 9 );
        TEST::SET_W(206, 2 << 12 | 1 << 11 | 1 << 8 | 3 << 4);
        TEST::SET_L(208, 0x10);
        TEST::SET_W(212, TEST_BREAK);

        // LEA (0x10.W, %ZPC, %D2), %A5
        TEST::SET_W(214, 0040773 | 5 << 9 );
        TEST::SET_W(216, 2 << 12 | 1 << 11 | 1 << 8 | 1 << 7 | 2 << 4);
        TEST::SET_W(218, 0x10);
        TEST::SET_W(220, TEST_BREAK);

        // LEA (0x10.W, %PC), %A5
        TEST::SET_W(222, 0040773 | 5 << 9 );
        TEST::SET_W(224, 2 << 12 | 1 << 11 | 1 << 8 | 1 << 6 | 2 << 4);
        TEST::SET_W(224, 0x10);
        TEST::SET_W(228, TEST_BREAK);

        // LEA ([0x10.W, %PC, %D2]), %A5
        TEST::SET_W(230, 0040773 | 5 << 9);
        TEST::SET_W(232, 2 << 12 | 1 << 11 | 1 << 8 | 2 << 4 | 1);
        TEST::SET_W(234, 0x10);
        TEST::SET_W(236, TEST_BREAK);

        // LEA ([0x10.W, %PC, %D2], 0x30.W), %A5
        TEST::SET_W(238, 0040773 | 5 << 9 );
        TEST::SET_W(240, 2 << 12 | 1 << 11 | 1 << 8 | 2 << 4 | 2);
        TEST::SET_W(242, 0x10);
        TEST::SET_W(244, 0x30);
        TEST::SET_W(246, TEST_BREAK);

        // LEA ([0x10.W, %PC, %D2], 0x30.L), %A5
        TEST::SET_W(248, 0040773 | 5 << 9 );
        TEST::SET_W(250, 2 << 12 | 1 << 11 | 1 << 8 | 2 << 4 | 3);
        TEST::SET_W(252, 0x10);
        TEST::SET_L(254, 0x30);
        TEST::SET_W(258, TEST_BREAK);

        // LEA ([0x100.W, %PC], %D2), %A5
        TEST::SET_W(260, 0040773 | 5 << 9 );
        TEST::SET_W(262, 2 << 12 | 1 << 11 | 1 << 8 | 2 << 4 | 1 << 2 | 1);
        TEST::SET_W(264, 0x100);
        TEST::SET_W(266, TEST_BREAK);

        // LEA ([0x100.W, %PC], %D2, 0x30.W), %A5
        TEST::SET_W(268, 0040773 | 5 << 9 );
        TEST::SET_W(270, 2 << 12 | 1 << 11 | 1 << 8 | 2 << 4 | 1 << 2 | 2);
        TEST::SET_W(272, 0x100);
        TEST::SET_W(274, 0x30);
        TEST::SET_W(276, TEST_BREAK);

        // LEA ([0x10.W, %PC], %D2, 0x30.L), %A5
        TEST::SET_W(278, 0040773 | 5 << 9 );
        TEST::SET_W(280, 2 << 12 | 1 << 11 | 1 << 8 | 2 << 4 | 1 << 2 | 3);
        TEST::SET_W(282, 0x100);
        TEST::SET_L(284, 0x30);
        TEST::SET_W(288, TEST_BREAK);

        jit_compile(0, 290);
    }};
BOOST_FIXTURE_TEST_SUITE(LEA, Prepare, *boost::unit_test::fixture<F_LEA>())


BOOST_AUTO_TEST_CASE(an) {
    cpu.A[3] = 0x2000;
    run_test(0);
    BOOST_TEST(cpu.A[5] == 0x2000);
}
BOOST_AUTO_TEST_CASE(d_an) {
    cpu.A[3] = 0x2000;
    run_test(4);
    BOOST_TEST(cpu.A[5] == 0x2010);
}

BOOST_AUTO_TEST_CASE(d_an_ri_w) {
    cpu.A[3] = 0x2000;
    cpu.D[2] = 0xfffe; // (-2)
    run_test(10);
    BOOST_TEST(cpu.A[5] == 0x2010 - 2);
}

BOOST_DATA_TEST_CASE(d_an_ri_sc, bdata::xrange(4), sc) {
    cpu.A[3] = 0x2000;
    cpu.D[2] = 3;
    run_test(16 + 6 * sc);
    BOOST_TEST(cpu.A[5] == 0x2010 + (3 << sc));
}

BOOST_AUTO_TEST_CASE(an_rn) {
    cpu.A[3] = 0x2000;
    cpu.D[2] = 3;
    run_test(40);
    BOOST_TEST(cpu.A[5] == 0x2003);
}

BOOST_AUTO_TEST_CASE(an_rn_bnw) {
    cpu.A[3] = 0x2000;
    cpu.D[2] = 3;
    run_test(46);
    BOOST_TEST(cpu.A[5] == 0x2013);
}

BOOST_AUTO_TEST_CASE(an_rn_bnl) {
    cpu.A[3] = 0x2000;
    cpu.D[2] = 3;
    run_test(54);
    BOOST_TEST(cpu.A[5] == 0x2013);
}

BOOST_AUTO_TEST_CASE(bd_ri) {
    cpu.D[2] = 3;
    run_test(64);
    BOOST_TEST(cpu.A[5] == 0x13);
}

BOOST_AUTO_TEST_CASE(bd_ai) {
    cpu.A[3] = 0x2000;
    run_test(72);
    BOOST_TEST(cpu.A[5] == 0x2010);
}

BOOST_AUTO_TEST_CASE(pre_indirect) {
    cpu.A[3] = 0x200;
    cpu.D[2] = 0x20;
    TEST::SET_L(0x230, 0x3000);
    run_test(80);
    BOOST_TEST(cpu.A[5] == 0x3000);
}

BOOST_AUTO_TEST_CASE(pre_odW) {
    cpu.A[3] = 0x200;
    cpu.D[2] = 0x20;
    TEST::SET_L(0x230, 0x3000);
    run_test(88);
    BOOST_TEST(cpu.A[5] == 0x3030);
}

BOOST_AUTO_TEST_CASE(pre_odL) {
    cpu.A[3] = 0x200;
    cpu.D[2] = 0x20;
    TEST::SET_L(0x230, 0x3000);
    run_test(98);
    BOOST_TEST(cpu.A[5] == 0x3030);
}


BOOST_AUTO_TEST_CASE(post_indirect) {
    cpu.A[3] = 0x200;
    cpu.D[2] = 0x20;
    TEST::SET_L(0x210, 0x3000);
    run_test(110);
    BOOST_TEST(cpu.A[5] == 0x3020);
}

BOOST_AUTO_TEST_CASE(post_odW) {
    cpu.A[3] = 0x200;
    cpu.D[2] = 0x20;
    TEST::SET_L(0x210, 0x3000);
    run_test(118);
    BOOST_TEST(cpu.A[5] == 0x3050);
}

BOOST_AUTO_TEST_CASE(post_odL) {
    cpu.A[3] = 0x200;
    cpu.D[2] = 0x20;
    TEST::SET_L(0x210, 0x3000);
    run_test(128);
    BOOST_TEST(cpu.A[5] == 0x3050);
}

BOOST_AUTO_TEST_CASE(immW) {
    run_test(140);
    BOOST_TEST(cpu.A[5] == 0x100);
}

BOOST_AUTO_TEST_CASE(immL) {
    run_test(146);
    BOOST_TEST(cpu.A[5] == 0x100);
}

BOOST_AUTO_TEST_CASE(d_pc) {
    run_test(154);
    BOOST_TEST(cpu.A[5] == 156 + 0x10);
}

BOOST_AUTO_TEST_CASE(d_pc_ri_w) {
    cpu.D[2] = 0xfffe; // (-2)
    run_test(160);
    BOOST_TEST(cpu.A[5] == 162 + 0x10 - 2);
}

BOOST_DATA_TEST_CASE(d_pc_ri_sc, bdata::xrange(4), sc) {
    cpu.D[2] = 3;
    run_test(166 + 6 * sc);
    BOOST_TEST(cpu.A[5] == 168 + 6 * sc + 0x10 + (3 << sc));
}

BOOST_AUTO_TEST_CASE(pc_rn) {
    cpu.D[2] = 3;
    run_test(190);
    BOOST_TEST(cpu.A[5] == 192 + 3);
}

BOOST_AUTO_TEST_CASE(pc_rn_bnw) {
    cpu.D[2] = 3;
    run_test(196);
    BOOST_TEST(cpu.A[5] == 198 + 0x10 + 3);
}

BOOST_AUTO_TEST_CASE(pc_rn_bnl) {
    cpu.D[2] = 3;
    run_test(204);
    BOOST_TEST(cpu.A[5] == 206 + 0x10 + 3);
}

BOOST_AUTO_TEST_CASE(zpc_bd_ri) {
    cpu.D[2] = 3;
    run_test(214);
    BOOST_TEST(cpu.A[5] == 0x13);
}


BOOST_AUTO_TEST_CASE(bd_pc) {
    run_test(222);
    BOOST_TEST(cpu.A[5] == 224 + 0x10);
}

BOOST_AUTO_TEST_CASE(pc_pre_indirect) {
    cpu.D[2] = 0x100;
    TEST::SET_L(232 + 0x110, 0x3000);
    run_test(230);
    BOOST_TEST(cpu.A[5] == 0x3000);
}

BOOST_AUTO_TEST_CASE(pc_pre_odW) {
    cpu.D[2] = 0x100;
    TEST::SET_L(240 + 0x110, 0x3000);
    run_test(238);
    BOOST_TEST(cpu.A[5] == 0x3030);
}

BOOST_AUTO_TEST_CASE(pc_pre_odL) {
    cpu.D[2] = 0x100;
    TEST::SET_L(250 + 0x110, 0x3000);
    run_test(248);
    BOOST_TEST(cpu.A[5] == 0x3030);
}

BOOST_AUTO_TEST_CASE(pc_post_indirect) {
    cpu.D[2] = 0x20;
    TEST::SET_L(262 + 0x100, 0x3000);
    run_test(260);
    BOOST_TEST(cpu.A[5] == 0x3020);
}

BOOST_AUTO_TEST_CASE(pc_post_odW) {
    cpu.D[2] = 0x20;
    TEST::SET_L(270 + 0x100, 0x3000);
    run_test(268);
    BOOST_TEST(cpu.A[5] == 0x3050);
}

BOOST_AUTO_TEST_CASE(pc_post_odL) {
    cpu.D[2] = 0x20;
    TEST::SET_L(280 + 0x100, 0x3000);
    run_test(278);
    BOOST_TEST(cpu.A[5] == 0x3050);
}

BOOST_AUTO_TEST_SUITE_END()
