#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_PTESTx {
    F_PTESTx() {
        // PTESTR (%A2)
        TEST::SET_W(0, 0172550 | 2);
        TEST::SET_W(2, TEST_BREAK);

        jit_compile(0, 4);
    }
};
BOOST_FIXTURE_TEST_SUITE(PTESTx, Prepare,
                         *boost::unit_test::fixture<F_PTESTx>())
BOOST_AUTO_TEST_SUITE(PTESTR)
BOOST_AUTO_TEST_CASE(err) {
    cpu.S = false;
    cpu.TCR_E = false;
    run_test(0);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::PRIV_ERR);
}

BOOST_AUTO_TEST_CASE(traced) {
    cpu.S = true;
    cpu.T = 1;
    run_test(0);
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::TRACE);
}
BOOST_AUTO_TEST_SUITE(R)
BOOST_AUTO_TEST_CASE(P) {
    cpu.A[2] = 0;
    cpu.DFC = 6;
    run_test(0);
    BOOST_TEST(cpu.MMUSR & 1);
    BOOST_TEST(!(cpu.MMUSR & 1 << 11));
}

BOOST_AUTO_TEST_CASE(TTR) {
    cpu.TCR_E = true;
    cpu.ITTR[0].E = true;
    cpu.ITTR[0].S = 2;
    cpu.ITTR[0].logic_base = 0x10;
    cpu.ITTR[0].logic_mask = 0;
    cpu.A[2] = 0x10000000;
    cpu.DFC = 6;
    run_test(0);
    BOOST_TEST(cpu.MMUSR & 1);
    BOOST_TEST(!(cpu.MMUSR & 1 << 11));
}

BOOST_AUTO_TEST_CASE(X) {
    cpu.A[2] = 0x3000;
    run_test(0);
    BOOST_TEST(!(cpu.MMUSR & 1));
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(T)
BOOST_AUTO_TEST_CASE(P) {
    cpu.A[2] = 0;
    cpu.DFC = 6;
    run_test(0);
    BOOST_TEST(!(cpu.MMUSR & 2));
}

BOOST_AUTO_TEST_CASE(TTR) {
    cpu.TCR_E = true;
    cpu.ITTR[0].E = true;
    cpu.ITTR[0].S = 2;
    cpu.ITTR[0].logic_base = 0x10;
    cpu.ITTR[0].logic_mask = 0;
    cpu.A[2] = 0x10000000;
    cpu.A[2] = 0x10000000;
    cpu.DFC = 6;
    run_test(0);
    BOOST_TEST(cpu.MMUSR & 2);
}
BOOST_AUTO_TEST_CASE(X) {
    cpu.A[2] = 0x3000;
    run_test(0);
    BOOST_TEST(!(cpu.MMUSR & 2));
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(W)
BOOST_AUTO_TEST_CASE(T) {
    cpu.A[2] = 0x1000;
    cpu.DFC = 6;
    run_test(0);
    BOOST_TEST(cpu.MMUSR & 4);
}

BOOST_AUTO_TEST_CASE(F) {
    cpu.A[2] = 0;
    run_test(0);
    BOOST_TEST(!(cpu.MMUSR & 4));
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(U) {
    TEST::SET_L(0x8300, 0x0411); // TEST Area
    cpu.A[2] = 0;
    run_test(0);
    BOOST_TEST(cpu.MMUSR & 8);
    BOOST_TEST(TEST::GET_L(0x8000) & 8);
    BOOST_TEST(TEST::GET_L(0x8200) & 8);
    BOOST_TEST(TEST::GET_L(0x8300) & 8);
}
BOOST_DATA_TEST_CASE(M, bdata::xrange(2), v) {
    TEST::SET_L(0x8300, 1 | v << 4);
    cpu.A[2] = 0;
    cpu.DFC = 6;
    run_test(0);
    BOOST_TEST((cpu.MMUSR >> 4 & 1) == v);
}

BOOST_DATA_TEST_CASE(CM, bdata::xrange(4), v) {
    TEST::SET_L(0x8300, 1 | v << 5);
    cpu.A[2] = 0;
    cpu.DFC = 6;
    run_test(0);
    BOOST_TEST((cpu.MMUSR >> 5 & 3) == v);
}

BOOST_AUTO_TEST_SUITE(S)
BOOST_AUTO_TEST_CASE(T) {
    cpu.A[2] = 0x2000;
    cpu.DFC = 6;
    run_test(0);
    BOOST_TEST(cpu.MMUSR & 1 << 7);
}

BOOST_AUTO_TEST_CASE(F) {
    cpu.A[2] = 0;
    run_test(0);
    BOOST_TEST(!(cpu.MMUSR & 1 << 7));
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_DATA_TEST_CASE(Ux, bdata::xrange(4), v) {
    TEST::SET_L(0x8300, 1 | v << 8);
    cpu.A[2] = 0;
    cpu.DFC = 6;
    run_test(0);
    BOOST_TEST((cpu.MMUSR >> 8 & 3) == v);
}

BOOST_DATA_TEST_CASE(G, bdata::xrange(2), v) {
    TEST::SET_L(0x8300, 1 | v << 10);
    cpu.A[2] = 0;
    cpu.DFC = 6;
    run_test(0);
    BOOST_TEST((cpu.MMUSR >> 10 & 1) == v);
}

BOOST_AUTO_TEST_CASE(B) {
    cpu.TCR_E = true;
    TEST::SET_L(0x8004, 0xFFFFFFF2); // ATC-BUS ERROR
    cpu.A[2] = 0x2000000;
    cpu.DFC = 6;
    run_test(0);
    BOOST_TEST(cpu.MMUSR == 1 << 11);
}

BOOST_AUTO_TEST_CASE(Addr) {
    cpu.A[2] = 0x1000;
    cpu.DFC = 6;
    run_test(0);
    BOOST_TEST(cpu.MMUSR >> 12 == 1);
}

BOOST_AUTO_TEST_SUITE_END()
#if 0
BOOST_FIXTURE_TEST_SUITE(PTESTR, MMU_Dafault)



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
    BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::TRACE);
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
#endif
BOOST_AUTO_TEST_SUITE_END()
