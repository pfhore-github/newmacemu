#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
uint32_t ptest(uint32_t addr, bool sys, bool code, bool W);
namespace bdata = boost::unit_test::data;
struct AccessFault {};
struct MMU_Dafault: Prepare {
    MMU_Dafault() {
        cpu.TCR_E = true;
        cpu.ITTR[0].E = true;
        cpu.ITTR[0].logic_base = 0;
        cpu.ITTR[0].logic_mask = 0;
    }
};
BOOST_FIXTURE_TEST_SUITE(PTESTR, MMU_Dafault)
BOOST_AUTO_TEST_CASE(err) {
    if(setjmp(cpu.ex_buf) == 0) {
        cpu.S = false;
        cpu.TCR_E = false;
        TEST::SET_W(0, 0172550);
        decode_and_run();
        BOOST_ERROR("exception unoccured");
    } else {
        BOOST_TEST(cpu.ex_n == 8);
    }
}
BOOST_DATA_TEST_CASE(user_data, bdata::xrange(2), T) {
    cpu.TCR_E = true;
    cpu.S = true;
    cpu.T = T;
    cpu.URP = 0x1000;
    TEST::SET_L(0x1004, 0x2002);
    TEST::SET_L(0x2008, 0x3002);
    TEST::SET_L(0x300C, 0x4001);
    cpu.A[2] = (1 << 13 | 2 << 6 | 3) << 12;
    cpu.DFC = 1;
    TEST::SET_W(0, 0172552);
    auto i = decode_and_run();
    BOOST_TEST(i == 0);
    BOOST_TEST(cpu.MMUSR == 0x4001 );
    BOOST_TEST(cpu.must_trace == !!T);
}

BOOST_DATA_TEST_CASE(user_code, bdata::xrange(2), T) {
    cpu.TCR_E = true;
    cpu.S = true;
    cpu.T = T;
    cpu.URP = 0x1000;
    TEST::SET_L(0x1004, 0x2002);
    TEST::SET_L(0x2008, 0x3002);
    TEST::SET_L(0x300C, 0x4001);
    cpu.A[2] = (1 << 13 | 2 << 6 | 3) << 12;
    cpu.DFC = 2;
    TEST::SET_W(0, 0172552);
    auto i = decode_and_run();
    BOOST_TEST(i == 0);
    BOOST_TEST(cpu.MMUSR == 0x4001 );
    BOOST_TEST(cpu.must_trace == !!T);
}


BOOST_DATA_TEST_CASE(sys_data, bdata::xrange(2), T) {
    cpu.TCR_E = true;
    cpu.S = true;
    cpu.T = T;
    cpu.SRP = 0x1000;
    TEST::SET_L(0x1004, 0x2002);
    TEST::SET_L(0x2008, 0x3002);
    TEST::SET_L(0x300C, 0x4001);
    cpu.A[2] = (1 << 13 | 2 << 6 | 3) << 12;
    cpu.DFC = 5;
    TEST::SET_W(0, 0172552);
    auto i = decode_and_run();
    BOOST_TEST(i == 0);
    BOOST_TEST(cpu.MMUSR == 0x4001);
    BOOST_TEST(cpu.must_trace == !!T);
}

BOOST_DATA_TEST_CASE(sys_code, bdata::xrange(2), T) {
    cpu.TCR_E = true;
    cpu.S = true;
    cpu.T = T;
    cpu.SRP = 0x1000;
    TEST::SET_L(0x1004, 0x2002);
    TEST::SET_L(0x2008, 0x3002);
    TEST::SET_L(0x300C, 0x4001);
    cpu.A[2] = (1 << 13 | 2 << 6 | 3) << 12;
    cpu.DFC = 6;
    TEST::SET_W(0, 0172552);
    auto i = decode_and_run();
    BOOST_TEST(i == 0);
    BOOST_TEST(cpu.MMUSR == 0x4001 );
    BOOST_TEST(cpu.must_trace == !!T);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_FIXTURE_TEST_SUITE(PTESTW, MMU_Dafault)
BOOST_AUTO_TEST_CASE(err) {
    if(setjmp(cpu.ex_buf) == 0) {
        cpu.S = false;
        cpu.TCR_E = false;
        TEST::SET_W(0, 0172510);
        decode_and_run();
        BOOST_ERROR("exception unoccured");
    } else {
        BOOST_TEST(cpu.ex_n == 8);
    }
}
BOOST_DATA_TEST_CASE(user_data, bdata::xrange(2), T) {
    cpu.TCR_E = true;
    cpu.S = true;
    cpu.T = T;
    cpu.URP = 0x1000;
    TEST::SET_L(0x1004, 0x2002);
    TEST::SET_L(0x2008, 0x3002);
    TEST::SET_L(0x300C, 0x4001);
    cpu.A[2] = (1 << 13 | 2 << 6 | 3) << 12;
    cpu.DFC = 1;
    TEST::SET_W(0, 0172512);
    auto i = decode_and_run();
    BOOST_TEST(i == 0);
    BOOST_TEST(cpu.MMUSR == (0x4001 | 1 << 4));
    BOOST_TEST(cpu.must_trace == !!T);
}

BOOST_DATA_TEST_CASE(user_code, bdata::xrange(2), T) {
    cpu.TCR_E = true;
    cpu.S = true;
    cpu.T = T;
    cpu.URP = 0x1000;
    TEST::SET_L(0x1004, 0x2002);
    TEST::SET_L(0x2008, 0x3002);
    TEST::SET_L(0x300C, 0x4001);
    cpu.A[2] = (1 << 13 | 2 << 6 | 3) << 12;
    cpu.DFC = 2;
    TEST::SET_W(0, 0172512);
    auto i = decode_and_run();
    BOOST_TEST(i == 0);
    BOOST_TEST(cpu.MMUSR == (0x4001 | 1 << 4));
    BOOST_TEST(cpu.must_trace == !!T);
}


BOOST_DATA_TEST_CASE(sys_data, bdata::xrange(2), T) {
    cpu.TCR_E = true;
    cpu.S = true;
    cpu.T = T;
    cpu.SRP = 0x1000;
    TEST::SET_L(0x1004, 0x2002);
    TEST::SET_L(0x2008, 0x3002);
    TEST::SET_L(0x300C, 0x4001);
    cpu.A[2] = (1 << 13 | 2 << 6 | 3) << 12;
    cpu.DFC = 5;
    TEST::SET_W(0, 0172512);
    auto i = decode_and_run();
    BOOST_TEST(i == 0);
    BOOST_TEST(cpu.MMUSR == (0x4001 | 1 << 4));
    BOOST_TEST(cpu.must_trace == !!T);
}

BOOST_DATA_TEST_CASE(sys_code, bdata::xrange(2), T) {
    cpu.TCR_E = true;
    cpu.S = true;
    cpu.T = T;
    cpu.SRP = 0x1000;
    TEST::SET_L(0x1004, 0x2002);
    TEST::SET_L(0x2008, 0x3002);
    TEST::SET_L(0x300C, 0x4001);
    cpu.A[2] = (1 << 13 | 2 << 6 | 3) << 12;
    cpu.DFC = 6;
    TEST::SET_W(0, 0172512);
    auto i = decode_and_run();
    BOOST_TEST(i == 0);
    BOOST_TEST(cpu.MMUSR == (0x4001 | 1 << 4));
    BOOST_TEST(cpu.must_trace == !!T);
}

BOOST_AUTO_TEST_SUITE_END()
