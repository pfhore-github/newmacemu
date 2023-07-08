#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(CPUSH, Prepare)
BOOST_AUTO_TEST_CASE(inv) {
    TEST::SET_W(0, 0172040);
    BOOST_CHECK_THROW(decode(), DecodeError);
}

BOOST_DATA_TEST_CASE(DC, bdata::xrange(2) * bdata::xrange(2), tr, s) {
    cpu.S = s;
    cpu.T = tr;
    TEST::SET_W(0, 0172170);
    auto i = decode_and_run();
    BOOST_TEST(i == 0);
    if(s) {
        BOOST_TEST(cpu.must_trace == !!tr);
    } else {
        BOOST_TEST(GET_EXCEPTION() == 8);
    }
}

BOOST_DATA_TEST_CASE(IC, bdata::xrange(2) * bdata::xrange(2), tr, s) {
    cpu.S = s;
    cpu.T = tr;
    TEST::SET_W(0, 0172270);
    auto i = decode_and_run();
    BOOST_TEST(i == 0);
    if(s) {
        BOOST_TEST(cpu.must_trace == !!tr);
    } else {
        BOOST_TEST(GET_EXCEPTION() == 8);
    }
}

BOOST_DATA_TEST_CASE(BC, bdata::xrange(2) * bdata::xrange(2), tr, s) {
    cpu.S = s;
    cpu.T = tr;
    TEST::SET_W(0, 0172370);
    auto i = decode_and_run();
    BOOST_TEST(i == 0);
    if(s) {
        BOOST_TEST(cpu.must_trace == !!tr);
    } else {
        BOOST_TEST(GET_EXCEPTION() == 8);
    }
}

BOOST_DATA_TEST_CASE(Line, bdata::xrange(2) * bdata::xrange(2), tr, s) {
    cpu.S = s;
    cpu.T = tr;
    TEST::SET_W(0, 0172263);
    auto i = decode_and_run();
    BOOST_TEST(i == 0);
    if(s) {
        BOOST_TEST(cpu.must_trace == !!tr);
    } else {
        BOOST_TEST(GET_EXCEPTION() == 8);
    }
}

BOOST_DATA_TEST_CASE(Page, bdata::xrange(2) * bdata::xrange(2), tr, s) {
    cpu.S = s;
    cpu.T = tr;
    TEST::SET_W(0, 0172253);
    auto i = decode_and_run();
    BOOST_TEST(i == 0);
    if(s) {
        BOOST_TEST(cpu.must_trace == !!tr);
    } else {
        BOOST_TEST(GET_EXCEPTION() == 8);
    }
}

BOOST_AUTO_TEST_SUITE_END()
