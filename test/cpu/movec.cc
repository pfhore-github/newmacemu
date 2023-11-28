#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <fmt/core.h>

namespace bdata = boost::unit_test::data;
struct F_MOVEC {
    F_MOVEC() {
        // MOVEC %SFC, %D3
        TEST::SET_W(0, 0047172);
        TEST::SET_W(2, 0 | 3 << 12);
        TEST::SET_W(4, TEST_BREAK);

        // MOVEC %DFC, %D3
        TEST::SET_W(6, 0047172);
        TEST::SET_W(8, 1 | 3 << 12);
        TEST::SET_W(10, TEST_BREAK);

        // MOVEC %CACR, %D3
        TEST::SET_W(12, 0047172);
        TEST::SET_W(14, 2 | 3 << 12);
        TEST::SET_W(16, TEST_BREAK);

        // MOVEC %TC, %D3
        TEST::SET_W(18, 0047172);
        TEST::SET_W(20, 3 | 3 << 12);
        TEST::SET_W(22, TEST_BREAK);

        // MOVEC %ITT0, %D3
        TEST::SET_W(24, 0047172);
        TEST::SET_W(26, 4 | 3 << 12);
        TEST::SET_W(28, TEST_BREAK);

        // MOVEC %ITT1, %D3
        TEST::SET_W(30, 0047172);
        TEST::SET_W(32, 5 | 3 << 12);
        TEST::SET_W(34, TEST_BREAK);

        // MOVEC %DTT0, %D3
        TEST::SET_W(36, 0047172);
        TEST::SET_W(38, 6 | 3 << 12);
        TEST::SET_W(40, TEST_BREAK);

        // MOVEC %DTT1, %D3
        TEST::SET_W(42, 0047172);
        TEST::SET_W(44, 7 | 3 << 12);
        TEST::SET_W(46, TEST_BREAK);

        // MOVEC %USP, %A3
        TEST::SET_W(48, 0047172);
        TEST::SET_W(50, 0x800 | 1 << 15 | 3 << 12);
        TEST::SET_W(52, TEST_BREAK);

        // MOVEC %VBR, %A3
        TEST::SET_W(54, 0047172);
        TEST::SET_W(56, 0x801 | 1 << 15 | 3 << 12);
        TEST::SET_W(58, TEST_BREAK);

        // MOVEC %MSP, %A3
        TEST::SET_W(60, 0047172);
        TEST::SET_W(62, 0x803 | 1 << 15 | 3 << 12);
        TEST::SET_W(64, TEST_BREAK);

        // MOVEC %ISP, %A3
        TEST::SET_W(66, 0047172);
        TEST::SET_W(68, 0x804 | 1 << 15 | 3 << 12);
        TEST::SET_W(70, TEST_BREAK);

        // MOVEC %MMUSR, %D3
        TEST::SET_W(72, 0047172);
        TEST::SET_W(74, 0x805 | 3 << 12);
        TEST::SET_W(76, TEST_BREAK);

        // MOVEC %URP, %A3
        TEST::SET_W(78, 0047172);
        TEST::SET_W(80, 0x806 | 1 << 15 | 3 << 12);
        TEST::SET_W(82, TEST_BREAK);

        // MOVEC %SRP, %A3
        TEST::SET_W(84, 0047172);
        TEST::SET_W(86, 0x807 | 1 << 15 | 3 << 12);
        TEST::SET_W(88, TEST_BREAK);

        // MOVEC %D3, %SFC
        TEST::SET_W(90, 0047173);
        TEST::SET_W(92, 0 | 3 << 12);
        TEST::SET_W(94, TEST_BREAK);

        // MOVEC %D3, %DFC
        TEST::SET_W(96, 0047173);
        TEST::SET_W(98, 1 | 3 << 12);
        TEST::SET_W(100, TEST_BREAK);

        // MOVEC %D3, %CACR
        TEST::SET_W(102, 0047173);
        TEST::SET_W(104, 2 | 3 << 12);
        TEST::SET_W(106, TEST_BREAK);

        // MOVEC %D3, %TC
        TEST::SET_W(108, 0047173);
        TEST::SET_W(110, 3 | 3 << 12);
        TEST::SET_W(112, TEST_BREAK);

        // MOVEC %D3, %ITT0
        TEST::SET_W(114, 0047173);
        TEST::SET_W(116, 4 | 3 << 12);
        TEST::SET_W(118, TEST_BREAK);

        // MOVEC %D3, %ITT1
        TEST::SET_W(120, 0047173);
        TEST::SET_W(122, 5 | 3 << 12);
        TEST::SET_W(124, TEST_BREAK);

        // MOVEC %D3, %DTT0
        TEST::SET_W(126, 0047173);
        TEST::SET_W(128, 6 | 3 << 12);
        TEST::SET_W(130, TEST_BREAK);

        // MOVEC %D3, %DTT1
        TEST::SET_W(132, 0047173);
        TEST::SET_W(134, 7 | 3 << 12);
        TEST::SET_W(136, TEST_BREAK);

        // MOVEC %A3, %USP
        TEST::SET_W(138, 0047173);
        TEST::SET_W(140, 0x800 | 1 << 15 | 3 << 12);
        TEST::SET_W(142, TEST_BREAK);

        // MOVEC %A3, %VBR
        TEST::SET_W(144, 0047173);
        TEST::SET_W(146, 0x801 | 1 << 15 | 3 << 12);
        TEST::SET_W(148, TEST_BREAK);

        // MOVEC %A3, %MSP
        TEST::SET_W(150, 0047173);
        TEST::SET_W(152, 0x803 | 1 << 15 | 3 << 12);
        TEST::SET_W(154, TEST_BREAK);

        // MOVEC %A3, %MSP
        TEST::SET_W(156, 0047173);
        TEST::SET_W(158, 0x804 | 1 << 15 | 3 << 12);
        TEST::SET_W(160, TEST_BREAK);

        // MOVEC %D3, %MSP
        TEST::SET_W(162, 0047173);
        TEST::SET_W(164, 0x805 | 3 << 12);
        TEST::SET_W(166, TEST_BREAK);

        // MOVEC %D3, %URP
        TEST::SET_W(168, 0047173);
        TEST::SET_W(170, 0x806 | 1 << 15 | 3 << 12);
        TEST::SET_W(172, TEST_BREAK);

        // MOVEC %D3, %SRP
        TEST::SET_W(174, 0047173);
        TEST::SET_W(176, 0x807 | 1 << 15 | 3 << 12);
        TEST::SET_W(178, TEST_BREAK);

        jit_compile(0, 180);
    }};
BOOST_FIXTURE_TEST_SUITE(MOVEC, Prepare, *boost::unit_test::fixture<F_MOVEC>())


BOOST_AUTO_TEST_SUITE(FROM)

BOOST_AUTO_TEST_CASE(User) {
    cpu.S = false;
    run_test(0);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::PRIV_ERR);
}

BOOST_AUTO_TEST_CASE(Traced) {
    cpu.S = true;
    cpu.T = 1;
    run_test(0);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::TRACE);
}

BOOST_AUTO_TEST_CASE(SFC) {
    cpu.S = true;
    cpu.SFC = 2;
    run_test(0);
    BOOST_TEST(cpu.D[3] == 2);
}
BOOST_AUTO_TEST_CASE(DFC) {
    cpu.S = true;
    cpu.DFC = 1;
    run_test(6);
    BOOST_TEST(cpu.D[3] == 1);
}
BOOST_AUTO_TEST_CASE(CACR) {
    cpu.S = true;
    cpu.CACR_DE = true;
    run_test(12);
    BOOST_TEST(cpu.D[3] == 0x80000000);
}

BOOST_AUTO_TEST_CASE(TC) {
    cpu.S = true;
    cpu.TCR_P = true;
    run_test(18);
    BOOST_TEST(cpu.D[3] == 0xC000);
    cpu.TCR_P = false;
}

BOOST_AUTO_TEST_CASE(ITT0) {
    cpu.S = true;
    cpu.ITTR[0].logic_base = 0x33;
    cpu.ITTR[0].logic_mask = 0x23;
    cpu.ITTR[0].CM = 1;
    cpu.ITTR[0].U = 2;
    cpu.ITTR[0].S = 3;
    cpu.ITTR[0].W = true;
    cpu.ITTR[0].E = false;
    run_test(24);
    BOOST_TEST(cpu.D[3] == (0x33230004 | 3 << 13 | 2 << 8 | 1 << 5));
}

BOOST_AUTO_TEST_CASE(ITT1) {
    cpu.S = true;
    cpu.ITTR[1].logic_base = 0x33;
    cpu.ITTR[1].logic_mask = 0x23;
    cpu.ITTR[1].CM = 1;
    cpu.ITTR[1].U = 2;
    cpu.ITTR[1].S = 3;
    cpu.ITTR[1].W = true;
    cpu.ITTR[1].E = false;

    run_test(30);
    BOOST_TEST(cpu.D[3] == (0x33230004 | 3 << 13 | 2 << 8 | 1 << 5));
}

BOOST_AUTO_TEST_CASE(DTT0) {
    cpu.S = true;
    cpu.DTTR[0].logic_base = 0x33;
    cpu.DTTR[0].logic_mask = 0x23;
    cpu.DTTR[0].CM = 1;
    cpu.DTTR[0].U = 2;
    cpu.DTTR[0].S = 3;
    cpu.DTTR[0].W = true;
    cpu.DTTR[0].E = false;

    run_test(36);
    BOOST_TEST(cpu.D[3] == (0x33230004 | 3 << 13 | 2 << 8 | 1 << 5));
}

BOOST_AUTO_TEST_CASE(DTT1) {
    cpu.S = true;
    cpu.DTTR[1].logic_base = 0x33;
    cpu.DTTR[1].logic_mask = 0x23;
    cpu.DTTR[1].CM = 1;
    cpu.DTTR[1].U = 2;
    cpu.DTTR[1].S = 3;
    cpu.DTTR[1].W = true;
    cpu.DTTR[1].E = false;

    run_test(42);
    BOOST_TEST(cpu.D[3] == (0x33230004 | 3 << 13 | 2 << 8 | 1 << 5));
}

BOOST_AUTO_TEST_CASE(USP) {
    cpu.S = true;
    cpu.USP = 0x200;
    run_test(48);
    BOOST_TEST(cpu.A[3] == 0x200);
}

BOOST_AUTO_TEST_CASE(VBR) {
    cpu.S = true;
    run_test(54);
    BOOST_TEST(cpu.A[3] == cpu.VBR);
}

BOOST_DATA_TEST_CASE(MSP, bdata::xrange(2), M) {
    cpu.S = true;
    cpu.M = M;
    cpu.A[7] = 0x2000;
    cpu.MSP = 0x3000;
    run_test(60);
    BOOST_TEST(cpu.A[3] == (M ? 0x2000 : 0x3000));
}

BOOST_DATA_TEST_CASE(ISP, bdata::xrange(2), M) {
    cpu.S = true;
    cpu.M = M;
    cpu.A[7] = 0x2000;
    cpu.ISP = 0x3000;
    run_test(66);
    BOOST_TEST(cpu.A[3] == (!M ? 0x2000 : 0x3000));
}

BOOST_AUTO_TEST_CASE(MMUSR) {
    cpu.S = true;
    cpu.MMUSR = 0x12345678;
    run_test(72);
    BOOST_TEST(cpu.D[3] == 0x12345678);
}

BOOST_AUTO_TEST_CASE(URP) {
    cpu.S = true;
    run_test(78);
    BOOST_TEST(cpu.A[3] == cpu.URP);
}

BOOST_AUTO_TEST_CASE(SRP) {
    cpu.S = true;
    run_test(84);
    BOOST_TEST(cpu.A[3] == cpu.SRP);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(TO)

BOOST_AUTO_TEST_CASE(User) {
    cpu.S = false;
    run_test(90);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::PRIV_ERR);
}

BOOST_AUTO_TEST_CASE(Traced) {
    cpu.S = true;
    cpu.T = 1;
    run_test(90);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::TRACE);
}

BOOST_AUTO_TEST_CASE(SFC) {
    cpu.S = true;
    cpu.D[3] = 1;
    run_test(90);
    BOOST_TEST(cpu.SFC == 1);
}

BOOST_AUTO_TEST_CASE(DFC) {
    cpu.S = true;
    cpu.D[3] = 1;
    run_test(96);
    BOOST_TEST(cpu.DFC == 1);
}

BOOST_AUTO_TEST_CASE(CACR) {
    cpu.S = true;
    cpu.D[3] = 0x8FFF7FFF;
    run_test(102);
    BOOST_TEST(cpu.CACR_DE);
}

BOOST_AUTO_TEST_CASE(TC) {
    cpu.S = true;
    cpu.D[3] = 0xC000;
    cpu.TCR_P = true;
    run_test(108);
    BOOST_TEST(cpu.TCR_P);
}

BOOST_AUTO_TEST_CASE(ITT0) {
    cpu.S = true;
    cpu.D[3] = (0x33230004 | 3 << 13 | 2 << 8 | 1 << 5);

    run_test(114);
    BOOST_TEST(cpu.ITTR[0].logic_base == 0x33);
    BOOST_TEST(cpu.ITTR[0].logic_mask = 0x23);
    BOOST_TEST(cpu.ITTR[0].CM = 1);
    BOOST_TEST(cpu.ITTR[0].U = 2);
    BOOST_TEST(cpu.ITTR[0].S = 3);
    BOOST_TEST(cpu.ITTR[0].W);
    BOOST_TEST(!cpu.ITTR[0].E);
}

BOOST_AUTO_TEST_CASE(ITT1) {
    cpu.S = true;
    cpu.D[3] = (0x33230004 | 3 << 13 | 2 << 8 | 1 << 5);

    TEST::SET_W(0, 0047173);
    TEST::SET_W(2, 5 | 3 << 12);
    run_test(120);
    BOOST_TEST(cpu.ITTR[1].logic_base == 0x33);
    BOOST_TEST(cpu.ITTR[1].logic_mask = 0x23);
    BOOST_TEST(cpu.ITTR[1].CM = 1);
    BOOST_TEST(cpu.ITTR[1].U = 2);
    BOOST_TEST(cpu.ITTR[1].S = 3);
    BOOST_TEST(cpu.ITTR[1].W);
    BOOST_TEST(!cpu.ITTR[1].E);
}

BOOST_AUTO_TEST_CASE(DTT0) {
    cpu.S = true;
    cpu.D[3] = (0x33230004 | 3 << 13 | 2 << 8 | 1 << 5);
    run_test(126);
    BOOST_TEST(cpu.DTTR[0].logic_base == 0x33);
    BOOST_TEST(cpu.DTTR[0].logic_mask = 0x23);
    BOOST_TEST(cpu.DTTR[0].CM = 1);
    BOOST_TEST(cpu.DTTR[0].U = 2);
    BOOST_TEST(cpu.DTTR[0].S = 3);
    BOOST_TEST(cpu.DTTR[0].W);
    BOOST_TEST(!cpu.DTTR[0].E);
}

BOOST_AUTO_TEST_CASE(DTT1) {
    cpu.S = true;
    cpu.D[3] = (0x33230004 | 3 << 13 | 2 << 8 | 1 << 5);
    run_test(132);
    BOOST_TEST(cpu.DTTR[1].logic_base == 0x33);
    BOOST_TEST(cpu.DTTR[1].logic_mask = 0x23);
    BOOST_TEST(cpu.DTTR[1].CM = 1);
    BOOST_TEST(cpu.DTTR[1].U = 2);
    BOOST_TEST(cpu.DTTR[1].S = 3);
    BOOST_TEST(cpu.DTTR[1].W);
    BOOST_TEST(!cpu.DTTR[1].E);
}

BOOST_AUTO_TEST_CASE(USP) {
    cpu.S = true;
    cpu.A[3] = 0x2000;
    run_test(138);
    BOOST_TEST(cpu.USP == 0x2000);
}

BOOST_AUTO_TEST_CASE(VBR) {
    cpu.S = true;
    cpu.A[3] = 0x3000;
    run_test(144);
    BOOST_TEST(cpu.VBR == 0x3000);
}

BOOST_DATA_TEST_CASE(MSP, bdata::xrange(2), M) {
    cpu.S = true;
    cpu.M = M;
    cpu.A[3] = 0x3000;
    run_test(150);
    BOOST_TEST((M ? cpu.A[7] : cpu.MSP) == 0x3000);
}

BOOST_DATA_TEST_CASE(ISP, bdata::xrange(2), M) {
    cpu.S = true;
    cpu.M = M;
    cpu.A[3] = 0x3000;
    run_test(156);
    BOOST_TEST((M ? cpu.ISP : cpu.A[7]) == 0x3000);
}

BOOST_AUTO_TEST_CASE(MMUSR) {
    cpu.S = true;
    cpu.D[3] = 0x12345678;
    run_test(162);
    BOOST_TEST(cpu.MMUSR == 0x12345678);
}

BOOST_AUTO_TEST_CASE(URP) {
    cpu.S = true;
    cpu.TCR_E = false;
    cpu.A[3] = 0x4000;
    run_test(168);
    BOOST_TEST(cpu.URP == 0x4000);
}

BOOST_AUTO_TEST_CASE(SRP) {
    cpu.S = true;
    cpu.TCR_E = false;
    cpu.A[3] = 0x5000;
    run_test(174);
    BOOST_TEST(cpu.SRP == 0x5000);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
