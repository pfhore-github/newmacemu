#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(CPUSH, Prepare)
BOOST_AUTO_TEST_SUITE(DC)
BOOST_DATA_TEST_CASE(Page, bdata::xrange(2) * bdata::xrange(2), tr, s) {
    cpu.S = s;
    cpu.T = tr;
    TEST::SET_W(0, 0172153);
    decode_and_run();
    if(s) {
        BOOST_TEST(cpu.must_trace == !!tr);
    } else {
        BOOST_TEST(GET_EXCEPTION() == 8);
    }
}

BOOST_DATA_TEST_CASE(Line, bdata::xrange(2) * bdata::xrange(2), tr, s) {
    cpu.S = s;
    cpu.T = tr;
    TEST::SET_W(0, 0172163);
    decode_and_run();
    if(s) {
        BOOST_TEST(cpu.must_trace == !!tr);
    } else {
        BOOST_TEST(GET_EXCEPTION() == 8);
    }
}


BOOST_DATA_TEST_CASE(ALL, bdata::xrange(2) * bdata::xrange(2), tr, s) {
    cpu.S = s;
    cpu.T = tr;
    TEST::SET_W(0, 0172170);
    decode_and_run();
    if(s) {
        BOOST_TEST(cpu.must_trace == !!tr);
    } else {
        BOOST_TEST(GET_EXCEPTION() == 8);
    }
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(IC)
BOOST_DATA_TEST_CASE(Page, bdata::xrange(2) * bdata::xrange(2), tr, s) {
    cpu.S = s;
    cpu.T = tr;
    TEST::SET_W(0, 0172253);
    decode_and_run();
    if(s) {
        BOOST_TEST(cpu.must_trace == !!tr);
    } else {
        BOOST_TEST(GET_EXCEPTION() == 8);
    }
}

BOOST_DATA_TEST_CASE(Line, bdata::xrange(2) * bdata::xrange(2), tr, s) {
    cpu.S = s;
    cpu.T = tr;
    TEST::SET_W(0, 0172260);
    decode_and_run();
    if(s) {
        BOOST_TEST(cpu.must_trace == !!tr);
    } else {
        BOOST_TEST(GET_EXCEPTION() == 8);
    }
}

BOOST_DATA_TEST_CASE(ALL, bdata::xrange(2) * bdata::xrange(2), tr, s) {
    cpu.S = s;
    cpu.T = tr;
    TEST::SET_W(0, 0172270);
    decode_and_run();
    if(s) {
        BOOST_TEST(cpu.must_trace == !!tr);
    } else {
        BOOST_TEST(GET_EXCEPTION() == 8);
    }
}


BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(BC)
BOOST_DATA_TEST_CASE(Page, bdata::xrange(2) * bdata::xrange(2), tr, s) {
    cpu.S = s;
    cpu.T = tr;
    TEST::SET_W(0, 0172353);
    decode_and_run();
    if(s) {
        BOOST_TEST(cpu.must_trace == !!tr);
    } else {
        BOOST_TEST(GET_EXCEPTION() == 8);
    }
}

BOOST_DATA_TEST_CASE(Line, bdata::xrange(2) * bdata::xrange(2), tr, s) {
    cpu.S = s;
    cpu.T = tr;
    TEST::SET_W(0, 0172360);
    decode_and_run();
    if(s) {
        BOOST_TEST(cpu.must_trace == !!tr);
    } else {
        BOOST_TEST(GET_EXCEPTION() == 8);
    }
}

BOOST_DATA_TEST_CASE(All, bdata::xrange(2) * bdata::xrange(2), tr, s) {
    cpu.S = s;
    cpu.T = tr;
    TEST::SET_W(0, 0172370);
    decode_and_run();
    if(s) {
        BOOST_TEST(cpu.must_trace == !!tr);
    } else {
        BOOST_TEST(GET_EXCEPTION() == 8);
    }
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()