#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "mmu.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
void set_v(atc_entry *atc, bool g) {
    atc[1].laddr = 1;
    atc[1].V = true;
    atc[1].G = g;
    atc[2].laddr = 2;
    atc[2].V = true;
    atc[2].G = g;
}
struct F_PFLUSHx {
    F_PFLUSHx() {
        // FPLUSHN (%A3)
        TEST::SET_W(0, 0172400 | 3);
        TEST::SET_W(2, TEST_BREAK);

        // FPLUSH (%A3)
        TEST::SET_W(4, 0172410 | 3);
        TEST::SET_W(6, TEST_BREAK);

        // FPLUSHAN
        TEST::SET_W(8, 0172420);
        TEST::SET_W(10, TEST_BREAK);

        // FPLUSHA
        TEST::SET_W(12, 0172430);
        TEST::SET_W(14, TEST_BREAK);

        jit_compile(0, 16);
    }
};
BOOST_FIXTURE_TEST_SUITE(PFLUSHx, Prepare,
                         *boost::unit_test::fixture<F_PFLUSHx>())

BOOST_AUTO_TEST_SUITE(PFLUSHN)
BOOST_AUTO_TEST_CASE(err) {
    run_test(0);
    BOOST_TEST(ex_n == EXCEPTION_NUMBER::PRIV_ERR);
}
BOOST_AUTO_TEST_CASE(traced) {
    cpu.S = true;
    cpu.T = 1;
    cpu.DFC = 2;
    cpu.A[3] = 0x1 << 12;
    run_test(0);
    BOOST_TEST(ex_n == EXCEPTION_NUMBER::TRACE);
}

BOOST_DATA_TEST_CASE(user, bdata::xrange(2), g) {
    cpu.S = true;
    cpu.DFC = 2;
    for(int i = 0; i < 2; ++i) {
        set_v(d_atc[i], g);
        set_v(i_atc[i], g);
    }
    cpu.A[3] = 0x1 << 12;
    run_test(0);
    BOOST_TEST(d_atc[0][1].V == g);
    BOOST_TEST(i_atc[0][1].V == g);
    BOOST_TEST(d_atc[0][2].V);
    BOOST_TEST(i_atc[0][2].V);
    BOOST_TEST(d_atc[1][1].V == g);
    BOOST_TEST(i_atc[1][1].V == g);
    BOOST_TEST(d_atc[1][2].V);
    BOOST_TEST(i_atc[1][2].V);
}

BOOST_DATA_TEST_CASE(sys, bdata::xrange(2), g) {
    cpu.S = true;
    cpu.DFC = 5;
    for(int i = 0; i < 2; ++i) {
        set_v(d_atc[i], g);
        set_v(i_atc[i], g);
    }
    cpu.A[3] = 0x1 << 12;
    run_test(0);
    BOOST_TEST(d_atc[0][1].V);
    BOOST_TEST(i_atc[0][1].V);
    BOOST_TEST(d_atc[0][2].V);
    BOOST_TEST(i_atc[0][2].V);
    BOOST_TEST(d_atc[1][1].V == g);
    BOOST_TEST(i_atc[1][1].V == g);
    BOOST_TEST(d_atc[1][2].V);
    BOOST_TEST(i_atc[1][2].V);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(PFLUSH)
BOOST_AUTO_TEST_CASE(err) {
    run_test(4);
    BOOST_TEST(ex_n == EXCEPTION_NUMBER::PRIV_ERR);
}

BOOST_AUTO_TEST_CASE(traced) {
    cpu.T = 1;
    cpu.S = true;
    cpu.DFC = 2;
    cpu.A[3] = 0x1 << 12;
    run_test(4);
    BOOST_TEST(ex_n == EXCEPTION_NUMBER::TRACE);
}

BOOST_DATA_TEST_CASE(user, bdata::xrange(2), g) {
    cpu.S = true;
    cpu.DFC = 2;
    for(int i = 0; i < 2; ++i) {
        set_v(d_atc[i], g);
        set_v(i_atc[i], g);
    }
    cpu.A[3] = 0x1 << 12;
    run_test(4);
    BOOST_TEST(!d_atc[0][1].V);
    BOOST_TEST(!i_atc[0][1].V);
    BOOST_TEST(d_atc[0][2].V);
    BOOST_TEST(i_atc[0][2].V);
    BOOST_TEST(!d_atc[1][1].V);
    BOOST_TEST(!i_atc[1][1].V);
    BOOST_TEST(d_atc[1][2].V);
    BOOST_TEST(i_atc[1][2].V);
}

BOOST_DATA_TEST_CASE(sys, bdata::xrange(2), g) {
    cpu.S = true;
    cpu.DFC = 5;
    for(int i = 0; i < 2; ++i) {
        set_v(d_atc[i], g);
        set_v(i_atc[i], g);
    }
    cpu.A[3] = 0x1 << 12;
    run_test(4);
    BOOST_TEST(d_atc[0][1].V);
    BOOST_TEST(i_atc[0][1].V);
    BOOST_TEST(d_atc[0][2].V);
    BOOST_TEST(i_atc[0][2].V);
    BOOST_TEST(!d_atc[1][1].V);
    BOOST_TEST(!i_atc[1][1].V);
    BOOST_TEST(d_atc[1][2].V);
    BOOST_TEST(i_atc[1][2].V);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(PFLUSHAN)
BOOST_AUTO_TEST_CASE(err) {
    run_test(8);
    BOOST_TEST(ex_n == EXCEPTION_NUMBER::PRIV_ERR);
}

BOOST_AUTO_TEST_CASE(traced) {
    cpu.S = true;
    cpu.T = 1;
    cpu.DFC = 2;
    run_test(8);
    BOOST_TEST(ex_n == EXCEPTION_NUMBER::TRACE);
}

BOOST_DATA_TEST_CASE(user, bdata::xrange(2), g) {
    cpu.S = true;
    cpu.DFC = 2;
    for(int i = 0; i < 2; ++i) {
        set_v(d_atc[i], g);
        set_v(i_atc[i], g);
    }
    run_test(8);
    BOOST_TEST(d_atc[0][1].V == g);
    BOOST_TEST(i_atc[0][1].V == g);
    BOOST_TEST(d_atc[0][2].V == g);
    BOOST_TEST(i_atc[0][2].V == g);
    BOOST_TEST(d_atc[1][1].V == g);
    BOOST_TEST(i_atc[1][1].V == g);
    BOOST_TEST(d_atc[1][2].V == g);
    BOOST_TEST(i_atc[1][2].V == g);
}

BOOST_DATA_TEST_CASE(sys, bdata::xrange(2), g) {
    cpu.S = true;
    cpu.DFC = 5;
    for(int i = 0; i < 2; ++i) {
        set_v(d_atc[i], g);
        set_v(i_atc[i], g);
    }
    run_test(8);
    BOOST_TEST(d_atc[0][1].V);
    BOOST_TEST(i_atc[0][1].V);
    BOOST_TEST(d_atc[0][2].V);
    BOOST_TEST(i_atc[0][2].V);
    BOOST_TEST(d_atc[1][1].V == g);
    BOOST_TEST(i_atc[1][1].V == g);
    BOOST_TEST(d_atc[1][2].V == g);
    BOOST_TEST(i_atc[1][2].V == g);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(PFLUSHA)
BOOST_AUTO_TEST_CASE(err) {
    run_test(12);
    BOOST_TEST(ex_n == EXCEPTION_NUMBER::PRIV_ERR);
}

BOOST_AUTO_TEST_CASE(traced) {
    cpu.S = true;
    cpu.T = 1;
    cpu.DFC = 2;
    run_test(12);
    BOOST_TEST(ex_n == EXCEPTION_NUMBER::TRACE);
}

BOOST_DATA_TEST_CASE(user, bdata::xrange(2), g) {
    cpu.S = true;
    cpu.DFC = 2;
    for(int i = 0; i < 2; ++i) {
        set_v(d_atc[i], g);
        set_v(i_atc[i], g);
    }
    run_test(12);
    BOOST_TEST(!d_atc[0][1].V);
    BOOST_TEST(!i_atc[0][1].V);
    BOOST_TEST(!d_atc[0][2].V);
    BOOST_TEST(!i_atc[0][2].V);
    BOOST_TEST(!d_atc[1][1].V);
    BOOST_TEST(!i_atc[1][1].V);
    BOOST_TEST(!d_atc[1][2].V);
    BOOST_TEST(!i_atc[1][2].V);
}

BOOST_DATA_TEST_CASE(sys, bdata::xrange(2), g) {
    cpu.S = true;
    cpu.DFC = 5;
    for(int i = 0; i < 2; ++i) {
        set_v(d_atc[i], g);
        set_v(i_atc[i], g);
    }
    run_test(12);
    BOOST_TEST(d_atc[0][1].V);
    BOOST_TEST(i_atc[0][1].V);
    BOOST_TEST(d_atc[0][2].V);
    BOOST_TEST(i_atc[0][2].V);
    BOOST_TEST(!d_atc[1][1].V);
    BOOST_TEST(!i_atc[1][1].V);
    BOOST_TEST(!d_atc[1][2].V);
    BOOST_TEST(!i_atc[1][2].V);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
