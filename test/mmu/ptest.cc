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

        // PTESTW (%A2)
        TEST::SET_W(4, 0172510 | 2);
        TEST::SET_W(6, TEST_BREAK);

        jit_compile(0, 8);
    }
};
BOOST_FIXTURE_TEST_SUITE(PTESTx, Prepare,
                         *boost::unit_test::fixture<F_PTESTx>())
BOOST_AUTO_TEST_SUITE(PTESTR)
BOOST_AUTO_TEST_CASE(err) {
    cpu.S = false;
    run_test(0, EXCEPTION_NUMBER::PRIV_ERR);
}

BOOST_AUTO_TEST_CASE(traced) {
    cpu.S = true;
    cpu.T = 1;
    run_test(0, EXCEPTION_NUMBER::TRACE);
}
BOOST_AUTO_TEST_SUITE(R)
BOOST_AUTO_TEST_CASE(P) {
    cpu.S = true;
    cpu.A[2] = 0;
    cpu.DFC = 6;
    run_test(0);
    BOOST_TEST(cpu.MMUSR & 1);
    BOOST_TEST(!(cpu.MMUSR & 1 << 11));
}

BOOST_AUTO_TEST_CASE(TTR) {
    cpu.S = true;
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
    cpu.S = true;
    cpu.A[2] = 0x4000;
    run_test(0);
    BOOST_TEST(!(cpu.MMUSR & 1));
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(T)
BOOST_AUTO_TEST_CASE(P) {
    cpu.S = true;
    cpu.A[2] = 0;
    cpu.DFC = 6;
    run_test(0);
    BOOST_TEST(!(cpu.MMUSR & 2));
}

BOOST_AUTO_TEST_CASE(TTR) {
    cpu.S = true;
    cpu.TCR_E = true;
    cpu.ITTR[0].E = true;
    cpu.ITTR[0].S = 2;
    cpu.ITTR[0].logic_base = 0x10;
    cpu.ITTR[0].logic_mask = 0;
    cpu.A[2] = 0x10000000;
    cpu.DFC = 6;
    run_test(0);
    BOOST_TEST(cpu.MMUSR & 3);
}
BOOST_AUTO_TEST_CASE(X) {
    cpu.S = true;
    cpu.A[2] = 0x3000;
    run_test(0);
    BOOST_TEST(!(cpu.MMUSR & 2));
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(W)
BOOST_AUTO_TEST_CASE(T) {
    cpu.S = true;
    cpu.A[2] = 0;
    cpu.DFC = 6;
    run_test(0);
    BOOST_TEST(cpu.MMUSR & 4);
}

BOOST_AUTO_TEST_CASE(F) {
    cpu.S = true;
    cpu.A[2] = 0x1000;
    run_test(0);
    BOOST_TEST(!(cpu.MMUSR & 4));
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(U) {
    cpu.S = true;
    cpu.DFC = 1;
    cpu.A[2] = 0;
    run_test(0);
    BOOST_TEST(TEST::GET_L(0x8000) & 8);
    BOOST_TEST(TEST::GET_L(0x8200) & 8);
    BOOST_TEST(TEST::GET_L(0x8300) & 8);
}

BOOST_DATA_TEST_CASE(CM, bdata::xrange(4), v) {
    cpu.S = true;
    TEST::SET_L(0x8304, 0x1019 | v << 5);
    cpu.A[2] = 0x1000;
    cpu.DFC = 1;
    run_test(0);
    BOOST_TEST((cpu.MMUSR >> 5 & 3) == v);
}

BOOST_AUTO_TEST_SUITE(S)
BOOST_AUTO_TEST_CASE(T) {
    cpu.S = true;
    cpu.A[2] = 0x3000;
    cpu.DFC = 1;
    run_test(0);
    BOOST_TEST(cpu.MMUSR & 1 << 7);
}

BOOST_AUTO_TEST_CASE(F) {
    cpu.S = true;
    cpu.A[2] = 0x1000;
    cpu.DFC = 1;
    run_test(0);
    BOOST_TEST(!(cpu.MMUSR & 1 << 7));
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_DATA_TEST_CASE(Ux, bdata::xrange(4), v) {
    cpu.S = true;
    TEST::SET_L(0x8304, 0x1019 | v << 8);
    cpu.A[2] = 0x1000;
    cpu.DFC = 1;
    run_test(0);
    BOOST_TEST((cpu.MMUSR >> 8 & 3) == v);
}

BOOST_DATA_TEST_CASE(G, bdata::xrange(2), v) {
    cpu.S = true;
    TEST::SET_L(0x8304, 0x1019 | v << 10);
    cpu.A[2] = 0x1000;
    cpu.DFC = 1;
    run_test(0);
    BOOST_TEST((cpu.MMUSR >> 10 & 1) == v);
}

BOOST_AUTO_TEST_CASE(B) {
    cpu.S = true;
    TEST::SET_L(0x8004, 0xEFFFFFF2); // ATC-BUS ERROR
    cpu.A[2] = 0x2000000;
    cpu.DFC = 1;
    run_test(0);
    BOOST_TEST(cpu.MMUSR == 1 << 11);
}

BOOST_AUTO_TEST_CASE(Addr) {
    cpu.S = true;
    cpu.A[2] = 0x5000;
    cpu.DFC = 1;
    run_test(0);
    BOOST_TEST(cpu.MMUSR >> 12 == 5);
}

BOOST_AUTO_TEST_CASE(UserCode) {
    cpu.S = true;
    cpu.A[2] = 0x1000;
    cpu.DFC = 2;
    run_test(0);
    BOOST_TEST(cpu.MMUSR >> 12 == 1);
}

BOOST_AUTO_TEST_CASE(SysData) {
    cpu.S = true;
    cpu.A[2] = 0x6000;
    cpu.DFC = 5;
    run_test(0);
    BOOST_TEST(cpu.MMUSR >> 12 == 7);
}

BOOST_AUTO_TEST_CASE(SysCode) {
    cpu.S = true;
    cpu.A[2] = 0x6000;
    cpu.DFC = 6;
    run_test(0);
    BOOST_TEST(cpu.MMUSR >> 12 == 7);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(PTESTW)
BOOST_AUTO_TEST_CASE(err) {
    cpu.S = false;
    run_test(4, EXCEPTION_NUMBER::PRIV_ERR);
}

BOOST_AUTO_TEST_CASE(traced) {
    cpu.S = true;
    cpu.T = 1;
    run_test(4, EXCEPTION_NUMBER::TRACE);
}

BOOST_AUTO_TEST_CASE(M) {
    cpu.S = true;
    cpu.DFC = 1;
    TEST::SET_L(0x8304, 0x1009);
    cpu.A[2] = 0x1000;
    run_test(4);
    BOOST_TEST(TEST::GET_L(0x8304) & 0x10);
    BOOST_TEST(cpu.MMUSR & 0x10);
}


BOOST_AUTO_TEST_CASE(Addr) {
    cpu.S = true;
    cpu.A[2] = 0x5000;
    cpu.DFC = 1;
    run_test(4);
    BOOST_TEST(cpu.MMUSR >> 12 == 5);
}

BOOST_AUTO_TEST_CASE(UserCode) {
    cpu.S = true;
    cpu.A[2] = 0x1000;
    cpu.DFC = 2;
    run_test(4);
    BOOST_TEST(cpu.MMUSR >> 12 == 1);
}

BOOST_AUTO_TEST_CASE(SysData) {
    cpu.S = true;
    cpu.A[2] = 0x6000;
    cpu.DFC = 5;
    run_test(4);
    BOOST_TEST(cpu.MMUSR >> 12 == 7);
}

BOOST_AUTO_TEST_CASE(SysCode) {
    cpu.S = true;
    cpu.A[2] = 0x6000;
    cpu.DFC = 6;
    run_test(4);
    BOOST_TEST(cpu.MMUSR >> 12 == 7);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
