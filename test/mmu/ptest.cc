#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct MMU_Dafault : Prepare {
    MMU_Dafault() {
        cpu.TCR_E = true;
        cpu.ITTR[0].E = true;
        cpu.ITTR[0].S = 2;
        cpu.ITTR[0].logic_base = 0;
        cpu.ITTR[0].logic_mask = 0;
    }
};
BOOST_AUTO_TEST_SUITE(MMU_RESULT)
BOOST_DATA_TEST_CASE(R, bdata::xrange(2), v) {
    mmu_result re;
    re.R = v;
    BOOST_TEST((re.value() & 1) == v);
}
BOOST_DATA_TEST_CASE(T, bdata::xrange(2), v) {
    mmu_result re;
    re.T = v;
    BOOST_TEST((re.value() >> 1 & 1) == v);
}
BOOST_DATA_TEST_CASE(W, bdata::xrange(2), v) {
    mmu_result re;
    re.W = v;
    BOOST_TEST((re.value() >> 2 & 1) == v);
}
BOOST_DATA_TEST_CASE(M, bdata::xrange(2), v) {
    mmu_result re;
    re.M = v;
    BOOST_TEST((re.value() >> 4 & 1) == v);
}
BOOST_DATA_TEST_CASE(CM, bdata::xrange(4), v) {
    mmu_result re;
    re.CM = v;
    BOOST_TEST((re.value() >> 5 & 3) == v);
}
BOOST_DATA_TEST_CASE(S, bdata::xrange(2), v) {
    mmu_result re;
    re.S = v;
    BOOST_TEST((re.value() >> 7 & 1) == v);
}
BOOST_DATA_TEST_CASE(Ux, bdata::xrange(4), v) {
    mmu_result re;
    re.Ux = v;
    BOOST_TEST((re.value() >> 8 & 3) == v);
}
BOOST_DATA_TEST_CASE(G, bdata::xrange(2), v) {
    mmu_result re;
    re.G = v;
    BOOST_TEST((re.value() >> 10 & 1) == v);
}
BOOST_DATA_TEST_CASE(B, bdata::xrange(2), v) {
    mmu_result re;
    re.B = v;
    BOOST_TEST((re.value() >> 11 & 1) == v);
}
BOOST_AUTO_TEST_CASE(Addr) {
    mmu_result re;
    re.paddr = 0x12345;
    BOOST_TEST((re.value() >> 12) == 0x12345);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_FIXTURE_TEST_SUITE(PTESTR, MMU_Dafault)
BOOST_AUTO_TEST_CASE(err) {
    cpu.S = false;
    cpu.TCR_E = false;
    TEST::SET_W(0, 0172550);
    BOOST_TEST(run_test() == 8);
}

BOOST_AUTO_TEST_CASE(traced) {
    cpu.TCR_E = true;
    cpu.S = true;
    cpu.T = 1;
    cpu.URP = 0x1000;
    cpu.ISP = 0x100;
    cpu.DTTR[0].E = true;
    cpu.DTTR[0].S = 1;
    cpu.DTTR[0].W = false;
    cpu.DTTR[0].logic_base = 0;
    cpu.DTTR[0].logic_mask = 0xff;
    TEST::SET_L(0x1004, 0x2002);
    TEST::SET_L(0x2008, 0x3002);
    TEST::SET_L(0x300C, 0x4001);
    cpu.A[2] = (1 << 13 | 2 << 6 | 3) << 12;
    cpu.DFC = 1;
    TEST::SET_W(0, 0172552);
    un_test(0);
	BOOST_TEST(cpu.ex_n == EXCAPTION_NUMBER::TRACE );
}
BOOST_AUTO_TEST_CASE(user_data) {
    cpu.TCR_E = true;
    cpu.S = true;
    cpu.URP = 0x1000;
    TEST::SET_L(0x1004, 0x2002);
    TEST::SET_L(0x2008, 0x3002);
    TEST::SET_L(0x300C, 0x4001);
    cpu.A[2] = (1 << 13 | 2 << 6 | 3) << 12;
    cpu.DFC = 1;
    TEST::SET_W(0, 0172552);
    run_test();
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.MMUSR == 0x4001);
}

BOOST_AUTO_TEST_CASE(user_code) {
    cpu.TCR_E = true;
    cpu.S = true;
    cpu.URP = 0x1000;
    TEST::SET_L(0x1004, 0x2002);
    TEST::SET_L(0x2008, 0x3002);
    TEST::SET_L(0x300C, 0x4001);
    cpu.A[2] = (1 << 13 | 2 << 6 | 3) << 12;
    cpu.DFC = 2;
    TEST::SET_W(0, 0172552);
    run_test();
    BOOST_TEST(cpu.MMUSR == 0x4001);
}

BOOST_AUTO_TEST_CASE(sys_data) {
    cpu.TCR_E = true;
    cpu.S = true;
    cpu.SRP = 0x1000;
    TEST::SET_L(0x1004, 0x2002);
    TEST::SET_L(0x2008, 0x3002);
    TEST::SET_L(0x300C, 0x4001);
    cpu.A[2] = (1 << 13 | 2 << 6 | 3) << 12;
    cpu.DFC = 5;
    TEST::SET_W(0, 0172552);
    run_test();
    BOOST_TEST(cpu.MMUSR == 0x4001);
}

BOOST_AUTO_TEST_CASE(sys_code) {
    cpu.TCR_E = true;
    cpu.S = true;
    cpu.SRP = 0x1000;
    TEST::SET_L(0x1004, 0x2002);
    TEST::SET_L(0x2008, 0x3002);
    TEST::SET_L(0x300C, 0x4001);
    cpu.A[2] = (1 << 13 | 2 << 6 | 3) << 12;
    cpu.DFC = 6;
    TEST::SET_W(0, 0172552);
    run_test();
    BOOST_TEST(cpu.MMUSR == 0x4001);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(PTESTW, MMU_Dafault)
BOOST_AUTO_TEST_CASE(err) {
    cpu.S = false;
    cpu.TCR_E = false;
    TEST::SET_W(0, 0172510);
    BOOST_TEST(run_test() == 8);
}

BOOST_AUTO_TEST_CASE(traced) {
    cpu.TCR_E = true;
    cpu.S = true;
    cpu.T = 1;
    cpu.URP = 0x1000;
    cpu.ISP = 0x100;
    cpu.DTTR[0].E = true;
    cpu.DTTR[0].S = 1;
    cpu.DTTR[0].W = false;
    cpu.DTTR[0].logic_base = 0;
    cpu.DTTR[0].logic_mask = 0xff;
    TEST::SET_L(0x1004, 0x2002);
    TEST::SET_L(0x2008, 0x3002);
    TEST::SET_L(0x300C, 0x4001);
    cpu.A[2] = (1 << 13 | 2 << 6 | 3) << 12;
    cpu.DFC = 1;
    TEST::SET_W(0, 0172512);
    un_test(0);
	BOOST_TEST(cpu.ex_n == EXCAPTION_NUMBER::TRACE );
}

BOOST_AUTO_TEST_CASE(user_data) {
    cpu.TCR_E = true;
    cpu.S = true;
    cpu.URP = 0x1000;
    TEST::SET_L(0x1004, 0x2002);
    TEST::SET_L(0x2008, 0x3002);
    TEST::SET_L(0x300C, 0x4001);
    cpu.A[2] = (1 << 13 | 2 << 6 | 3) << 12;
    cpu.DFC = 1;
    TEST::SET_W(0, 0172512);
    run_test();
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.MMUSR == (0x4001 | 1 << 4));
}

BOOST_AUTO_TEST_CASE(user_code) {
    cpu.TCR_E = true;
    cpu.S = true;
    cpu.URP = 0x1000;
    TEST::SET_L(0x1004, 0x2002);
    TEST::SET_L(0x2008, 0x3002);
    TEST::SET_L(0x300C, 0x4001);
    cpu.A[2] = (1 << 13 | 2 << 6 | 3) << 12;
    cpu.DFC = 2;
    TEST::SET_W(0, 0172512);
    run_test();
    BOOST_TEST(cpu.MMUSR == (0x4001 | 1 << 4));
}

BOOST_AUTO_TEST_CASE(sys_data) {
    cpu.TCR_E = true;
    cpu.S = true;
    cpu.SRP = 0x1000;
    TEST::SET_L(0x1004, 0x2002);
    TEST::SET_L(0x2008, 0x3002);
    TEST::SET_L(0x300C, 0x4001);
    cpu.A[2] = (1 << 13 | 2 << 6 | 3) << 12;
    cpu.DFC = 5;
    TEST::SET_W(0, 0172512);
    run_test();
    BOOST_TEST(cpu.MMUSR == (0x4001 | 1 << 4));
}

BOOST_AUTO_TEST_CASE(sys_code) {
    cpu.TCR_E = true;
    cpu.S = true;
    cpu.SRP = 0x1000;
    TEST::SET_L(0x1004, 0x2002);
    TEST::SET_L(0x2008, 0x3002);
    TEST::SET_L(0x300C, 0x4001);
    cpu.A[2] = (1 << 13 | 2 << 6 | 3) << 12;
    cpu.DFC = 6;
    TEST::SET_W(0, 0172512);
    run_test();
    BOOST_TEST(cpu.MMUSR == (0x4001 | 1 << 4));
}

