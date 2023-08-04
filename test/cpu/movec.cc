#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <fmt/core.h>

namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(MOVEC, Prepare)
BOOST_AUTO_TEST_SUITE(FROM)

BOOST_AUTO_TEST_CASE(User) {
    cpu.S = false;
    TEST::SET_W(0, 0047172);
    TEST::SET_W(2, 0 | 3 << 12);
    BOOST_TEST(run_test() == 8);
}

BOOST_AUTO_TEST_CASE(Traced) {
    cpu.S = true;
    cpu.T = 1;
    TEST::SET_W(0, 0047172);
    TEST::SET_W(2, 0 | 3 << 12);
    BOOST_TEST(run_test() == 9);
}

BOOST_AUTO_TEST_CASE(SFC) {
    cpu.S = true;
    cpu.SFC = 2;
    TEST::SET_W(0, 0047172);
    TEST::SET_W(2, 0 | 3 << 12);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(cpu.D[3] == 2);
}

BOOST_AUTO_TEST_CASE(DFC) {
    cpu.S = true;
    cpu.DFC = 1;
    TEST::SET_W(0, 0047172);
    TEST::SET_W(2, 1 | 3 << 12);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.D[3] == 1);
}

BOOST_AUTO_TEST_CASE(CACR) {
    cpu.S = true;
    cpu.CACR_DE = true;
    TEST::SET_W(0, 0047172);
    TEST::SET_W(2, 2 | 3 << 12);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.D[3] == 0x80000000);
}

BOOST_AUTO_TEST_CASE(TC) {
    cpu.S = true;
    cpu.TCR_P = true;
    TEST::SET_W(0, 0047172);
    TEST::SET_W(2, 3 | 3 << 12);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.D[3] == 0x4000);
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

    TEST::SET_W(0, 0047172);
    TEST::SET_W(2, 4 | 3 << 12);
    BOOST_TEST(run_test() == 0);
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

    TEST::SET_W(0, 0047172);
    TEST::SET_W(2, 5 | 3 << 12);
    BOOST_TEST(run_test() == 0);
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

    TEST::SET_W(0, 0047172);
    TEST::SET_W(2, 6 | 3 << 12);
    BOOST_TEST(run_test() == 0);
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

    TEST::SET_W(0, 0047172);
    TEST::SET_W(2, 7 | 3 << 12);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.D[3] == (0x33230004 | 3 << 13 | 2 << 8 | 1 << 5));
}

BOOST_AUTO_TEST_CASE(USP) {
    cpu.S = true;
    cpu.USP = 0x2000;
    TEST::SET_W(0, 0047172);
    TEST::SET_W(2, 0x800 | 1 << 15 | 3 << 12);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.A[3] == 0x2000);
}

BOOST_AUTO_TEST_CASE(VBR) {
    cpu.S = true;
    TEST::SET_W(0, 0047172);
    TEST::SET_W(2, 0x801 | 1 << 15 | 3 << 12);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.A[3] == 0x400);
}

BOOST_DATA_TEST_CASE(MSP, bdata::xrange(2), M) {
    cpu.S = true;
    cpu.M = M;
    cpu.A[7] = 0x2000;
    cpu.MSP = 0x3000;
    TEST::SET_W(0, 0047172);
    TEST::SET_W(2, 0x803 | 1 << 15 | 3 << 12);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.A[3] == (M ? 0x2000 : 0x3000));
}

BOOST_DATA_TEST_CASE(ISP, bdata::xrange(2), M) {
    cpu.S = true;
    cpu.M = M;
    cpu.A[7] = 0x2000;
    cpu.ISP = 0x3000;
    TEST::SET_W(0, 0047172);
    TEST::SET_W(2, 0x804 | 1 << 15 | 3 << 12);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.A[3] == (!M ? 0x2000 : 0x3000));
}

BOOST_AUTO_TEST_CASE(MMUSR) {
    cpu.S = true;
    cpu.MMUSR = 0x12345678;
    TEST::SET_W(0, 0047172);
    TEST::SET_W(2, 0x805 | 3 << 12);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.D[3] == 0x12345678);
}

BOOST_AUTO_TEST_CASE(URP) {
    cpu.S = true;
    cpu.URP = 0x4000;
    TEST::SET_W(0, 0047172);
    TEST::SET_W(2, 0x806 | 1 << 15 | 3 << 12);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.A[3] == 0x4000);
}

BOOST_AUTO_TEST_CASE(SRP) {
    cpu.S = true;
    cpu.SRP = 0x5000;
    TEST::SET_W(0, 0047172);
    TEST::SET_W(2, 0x807 | 1 << 15 | 3 << 12);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.A[3] == 0x5000);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(TO)

BOOST_AUTO_TEST_CASE(User) {
    cpu.S = false;
    TEST::SET_W(0, 0047173);
    TEST::SET_W(2, 0 | 3 << 12);
    BOOST_TEST(run_test() == 8);
}

BOOST_AUTO_TEST_CASE(Traced) {
    cpu.S = true;
    cpu.T = 1;
    TEST::SET_W(0, 0047173);
    TEST::SET_W(2, 0 | 3 << 12);
    BOOST_TEST(run_test() == 9);
}

BOOST_AUTO_TEST_CASE(SFC) {
    cpu.S = true;
    cpu.D[3] = 2;
    cpu.SFC = 2;
    TEST::SET_W(0, 0047173);
    TEST::SET_W(2, 0 | 3 << 12);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(cpu.SFC == 2);
}

BOOST_AUTO_TEST_CASE(DFC) {
    cpu.S = true;
    cpu.D[3] = 1;
    TEST::SET_W(0, 0047173);
    TEST::SET_W(2, 1 | 3 << 12);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.DFC == 1);
}

BOOST_AUTO_TEST_CASE(CACR) {
    cpu.S = true;
    cpu.D[3] = 0x8FFF7FFF;
    TEST::SET_W(0, 0047173);
    TEST::SET_W(2, 2 | 3 << 12);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.CACR_DE);
}

BOOST_AUTO_TEST_CASE(TC) {
    cpu.S = true;
    cpu.D[3] = 0x4000;
    cpu.TCR_P = true;
    TEST::SET_W(0, 0047173);
    TEST::SET_W(2, 3 | 3 << 12);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.TCR_P);
}

BOOST_AUTO_TEST_CASE(ITT0) {
    cpu.S = true;
    cpu.D[3] = (0x33230004 | 3 << 13 | 2 << 8 | 1 << 5);

    TEST::SET_W(0, 0047173);
    TEST::SET_W(2, 4 | 3 << 12);
    BOOST_TEST(run_test() == 0);
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
    BOOST_TEST(run_test() == 0);
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
    TEST::SET_W(0, 0047173);
    TEST::SET_W(2, 6 | 3 << 12);
    BOOST_TEST(run_test() == 0);
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

    TEST::SET_W(0, 0047173);
    TEST::SET_W(2, 7 | 3 << 12);
    BOOST_TEST(run_test() == 0);
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
    TEST::SET_W(0, 0047173);
    TEST::SET_W(2, 0x800 | 1 << 15 | 3 << 12);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.USP == 0x2000);
}

BOOST_AUTO_TEST_CASE(VBR) {
    cpu.S = true;
    cpu.A[3] = 0x3000;
    TEST::SET_W(0, 0047173);
    TEST::SET_W(2, 0x801 | 1 << 15 | 3 << 12);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.VBR == 0x3000);
}

BOOST_DATA_TEST_CASE(MSP, bdata::xrange(2), M) {
    cpu.S = true;
    cpu.M = M;
    cpu.A[3] = 0x3000;
    TEST::SET_W(0, 0047173);
    TEST::SET_W(2, 0x803 | 1 << 15 | 3 << 12);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST((M ? cpu.A[7] : cpu.MSP) == 0x3000);
}

BOOST_DATA_TEST_CASE(ISP, bdata::xrange(2), M) {
    cpu.S = true;
    cpu.M = M;
    cpu.A[3] = 0x3000;
    TEST::SET_W(0, 0047173);
    TEST::SET_W(2, 0x804 | 1 << 15 | 3 << 12);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST((M ? cpu.ISP : cpu.A[7]) == 0x3000);
}

BOOST_AUTO_TEST_CASE(MMUSR) {
    cpu.S = true;
    cpu.D[3] = 0x12345678;
    TEST::SET_W(0, 0047173);
    TEST::SET_W(2, 0x805 | 3 << 12);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.MMUSR == 0x12345678);
}

BOOST_AUTO_TEST_CASE(URP) {
    cpu.S = true;
    cpu.A[3] = 0x4000;
    TEST::SET_W(0, 0047173);
    TEST::SET_W(2, 0x806 | 1 << 15 | 3 << 12);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.URP == 0x4000);
}

BOOST_AUTO_TEST_CASE(SRP) {
    cpu.S = true;
    cpu.A[3] = 0x5000;
    TEST::SET_W(0, 0047173);
    TEST::SET_W(2, 0x807 | 1 << 15 | 3 << 12);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.SRP == 0x5000);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
