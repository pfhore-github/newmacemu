#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(CINV, Prepare)
BOOST_AUTO_TEST_CASE(inv) {
    TEST::SET_W(0, 0172000);
    BOOST_CHECK_THROW(decode(), DecodeError);
}

BOOST_DATA_TEST_CASE(DC, bdata::xrange(2) * bdata::xrange(2), tr, s) {
    cpu.S = s;
    cpu.T = tr;
    TEST::SET_W(0, 0172130);
    if(setjmp(cpu.ex_buf) == 0) {
        auto i = decode_and_run();
        BOOST_TEST(i == 0);
        BOOST_TEST(s);
        BOOST_TEST(cpu.must_trace == !!tr);
    } else {
        BOOST_TEST(!s);
        BOOST_TEST(cpu.ex_n == 8);
    }
}

BOOST_DATA_TEST_CASE(IC, bdata::xrange(2) * bdata::xrange(2), tr, s) {
    cpu.S = s;
    cpu.T = tr;
    TEST::SET_W(0, 0172230);
    if(setjmp(cpu.ex_buf) == 0) {
        auto i = decode_and_run();
        BOOST_TEST(i == 0);
        BOOST_TEST(s);
        BOOST_TEST(cpu.must_trace == !!tr);
    } else {
        BOOST_TEST(!s);
        BOOST_TEST(cpu.ex_n == 8);
    }
}

BOOST_DATA_TEST_CASE(BC, bdata::xrange(2) * bdata::xrange(2), tr, s) {
    cpu.S = s;
    cpu.T = tr;
    TEST::SET_W(0, 0172330);
    if(setjmp(cpu.ex_buf) == 0) {
        auto i = decode_and_run();
        BOOST_TEST(i == 0);
        BOOST_TEST(s);
        BOOST_TEST(cpu.must_trace == !!tr);
    } else {
        BOOST_TEST(!s);
        BOOST_TEST(cpu.ex_n == 8);
    }
}

BOOST_DATA_TEST_CASE(Line, bdata::xrange(2) * bdata::xrange(2), tr, s) {
    cpu.S = s;
    cpu.T = tr;
    TEST::SET_W(0, 0172223);
    if(setjmp(cpu.ex_buf) == 0) {
        auto i = decode_and_run();
        BOOST_TEST(i == 0);
        BOOST_TEST(s);
        BOOST_TEST(cpu.must_trace == !!tr);
    } else {
        BOOST_TEST(!s);
        BOOST_TEST(cpu.ex_n == 8);
    }
}

BOOST_DATA_TEST_CASE(Page, bdata::xrange(2) * bdata::xrange(2), tr, s) {
    cpu.S = s;
    cpu.T = tr;
    TEST::SET_W(0, 0172213);
    if(setjmp(cpu.ex_buf) == 0) {
        auto i = decode_and_run();
        BOOST_TEST(i == 0);
        BOOST_TEST(s);
        BOOST_TEST(cpu.must_trace == !!tr);
    } else {
        BOOST_TEST(!s);
        BOOST_TEST(cpu.ex_n == 8);
    }
}

BOOST_AUTO_TEST_SUITE_END()
