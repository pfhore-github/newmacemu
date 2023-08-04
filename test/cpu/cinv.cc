#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(CINV, Prepare)
BOOST_AUTO_TEST_SUITE(DC)
BOOST_DATA_TEST_CASE(Page, bdata::xrange(2) * bdata::xrange(2), tr, s) {
    cpu.S = s;
    cpu.T = tr;
    TEST::SET_W(0, 0172113);
    BOOST_TEST(run_test() == (!s ? 8 : tr ? 9 : 0));
}

BOOST_DATA_TEST_CASE(Line, bdata::xrange(2) * bdata::xrange(2), tr, s) {
    cpu.S = s;
    cpu.T = tr;
    TEST::SET_W(0, 0172123);
    BOOST_TEST(run_test() == (!s ? 8 : tr ? 9 : 0));
}

BOOST_DATA_TEST_CASE(ALL, bdata::xrange(2) * bdata::xrange(2), tr, s) {
    cpu.S = s;
    cpu.T = tr;
    TEST::SET_W(0, 0172130);
    BOOST_TEST(run_test() == (!s ? 8 : tr ? 9 : 0));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(IC)
BOOST_DATA_TEST_CASE(Page, bdata::xrange(2) * bdata::xrange(2), tr, s) {
    cpu.S = s;
    cpu.T = tr;
    TEST::SET_W(0, 0172213);
    BOOST_TEST(run_test() == (!s ? 8 : tr ? 9 : 0));
}

BOOST_DATA_TEST_CASE(Line, bdata::xrange(2) * bdata::xrange(2), tr, s) {
    cpu.S = s;
    cpu.T = tr;
    TEST::SET_W(0, 0172220);
    BOOST_TEST(run_test() == (!s ? 8 : tr ? 9 : 0));
}

BOOST_DATA_TEST_CASE(ALL, bdata::xrange(2) * bdata::xrange(2), tr, s) {
    cpu.S = s;
    cpu.T = tr;
    TEST::SET_W(0, 0172230);
    BOOST_TEST(run_test() == (!s ? 8 : tr ? 9 : 0));
}


BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(BC)
BOOST_DATA_TEST_CASE(Page, bdata::xrange(2) * bdata::xrange(2), tr, s) {
    cpu.S = s;
    cpu.T = tr;
    TEST::SET_W(0, 0172313);
    BOOST_TEST(run_test() == (!s ? 8 : tr ? 9 : 0));
}

BOOST_DATA_TEST_CASE(Line, bdata::xrange(2) * bdata::xrange(2), tr, s) {
    cpu.S = s;
    cpu.T = tr;
    TEST::SET_W(0, 0172320);
    BOOST_TEST(run_test() == (!s ? 8 : tr ? 9 : 0));
}

BOOST_DATA_TEST_CASE(All, bdata::xrange(2) * bdata::xrange(2), tr, s) {
    cpu.S = s;
    cpu.T = tr;
    TEST::SET_W(0, 0172330);
    BOOST_TEST(run_test() == (!s ? 8 : tr ? 9 : 0));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()