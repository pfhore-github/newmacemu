#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include "mmu.hpp"

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(ITTR, Prepare)
BOOST_AUTO_TEST_SUITE(ITTR0)
BOOST_AUTO_TEST_SUITE(base)

BOOST_AUTO_TEST_CASE(match) {
    cpu.ITTR[0].logic_base = 0x01;
    cpu.ITTR[0].logic_mask = 0;
    cpu.ITTR[0].E = true;
    cpu.ITTR[0].W = true;
    cpu.ITTR[0].S = 2;
    auto v = ptest(0x01000, false, true, false);
    BOOST_TEST(v.R);
    BOOST_TEST(v.T);
}
BOOST_AUTO_TEST_CASE(unmatch) {
    cpu.ITTR[0].logic_base = 0x01;
    cpu.ITTR[0].logic_mask = 0;
    cpu.ITTR[0].E = true;
    cpu.ITTR[0].W = true;
    cpu.ITTR[0].S = 2;
    auto v = ptest(0x02000, false, true, false);
    BOOST_TEST(!v.T);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(mask) {
    cpu.ITTR[0].logic_base = 0x02;
    cpu.ITTR[0].logic_mask = 0x01;
    cpu.ITTR[0].E = true;
    cpu.ITTR[0].W = true;
    cpu.ITTR[0].S = 2;
    auto v = ptest(0x03000, false, true, false);
    BOOST_TEST(v.R);
    BOOST_TEST(v.T);
}
BOOST_AUTO_TEST_CASE(E) {
    cpu.ITTR[0].logic_base = 0x01;
    cpu.ITTR[0].logic_mask = 0;
    cpu.ITTR[0].E = false;
    cpu.ITTR[0].W = true;
    cpu.ITTR[0].S = 2;
    auto v = ptest(0x01000, false, true, false);
    BOOST_TEST(!v.T);
}
BOOST_AUTO_TEST_SUITE(S)
BOOST_DATA_TEST_CASE(unspecified, bdata::xrange(2), s) {
    cpu.ITTR[0].logic_base = 0x01;
    cpu.ITTR[0].logic_mask = 0;
    cpu.ITTR[0].E = true;
    cpu.ITTR[0].W = true;
    cpu.ITTR[0].S = 2;
    auto v = ptest(0x01000, s, true, false);
    BOOST_TEST(v.R);
    BOOST_TEST(v.T);
}

BOOST_DATA_TEST_CASE(user, bdata::xrange(2), s) {
    cpu.ITTR[0].logic_base = 0x01;
    cpu.ITTR[0].logic_mask = 0;
    cpu.ITTR[0].E = true;
    cpu.ITTR[0].W = true;
    cpu.ITTR[0].S = 0;
    auto v = ptest(0x01000, s, true, false);
    if(s) {
        BOOST_TEST(!v.T);
    } else {
        BOOST_TEST(v.T);
    }
}

BOOST_DATA_TEST_CASE(sys, bdata::xrange(2), s) {
    cpu.ITTR[0].logic_base = 0x01;
    cpu.ITTR[0].logic_mask = 0;
    cpu.ITTR[0].E = true;
    cpu.ITTR[0].W = true;
    cpu.ITTR[0].S = 1;
    auto v = ptest(0x01000, s, true, false);
    if(!s) {
        BOOST_TEST(!v.T);
    } else {
        BOOST_TEST(v.T);
    }
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(W) {
    cpu.ITTR[0].logic_base = 0x01;
    cpu.ITTR[0].logic_mask = 0;
    cpu.ITTR[0].E = true;
    cpu.ITTR[0].W = true;
    cpu.ITTR[0].S = 2;
    auto v = ptest(0x01000, false, true, true);
    BOOST_TEST(!v.T);
}

BOOST_AUTO_TEST_CASE(Data) {
    cpu.ITTR[0].logic_base = 0x01;
    cpu.ITTR[0].logic_mask = 0;
    cpu.ITTR[0].E = true;
    cpu.ITTR[0].W = true;
    cpu.ITTR[0].S = 2;
    auto v = ptest(0x01000, false, false, false);
    BOOST_TEST(!v.T);
}
BOOST_AUTO_TEST_SUITE_END()
struct ITTR1_reg {
    ITTR1_reg() { cpu.ITTR[0].E = false; }
};
BOOST_FIXTURE_TEST_SUITE(ITTR1, ITTR1_reg)
BOOST_AUTO_TEST_SUITE(base)

BOOST_AUTO_TEST_CASE(match) {
    cpu.ITTR[1].logic_base = 0x02;
    cpu.ITTR[1].logic_mask = 0;
    cpu.ITTR[1].E = true;
    cpu.ITTR[1].W = true;
    cpu.ITTR[1].S = 2;
    auto v = ptest(0x02000, false, true, false);
    BOOST_TEST(v.R);
    BOOST_TEST(v.T);
}
BOOST_AUTO_TEST_CASE(unmatch) {
    cpu.ITTR[1].logic_base = 0x01;
    cpu.ITTR[1].logic_mask = 0;
    cpu.ITTR[1].E = true;
    cpu.ITTR[1].W = true;
    cpu.ITTR[1].S = 2;
    auto v = ptest(0x02000, false, true, false);
    BOOST_TEST(!v.T);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(mask) {
    cpu.ITTR[1].logic_base = 0x02;
    cpu.ITTR[1].logic_mask = 0x01;
    cpu.ITTR[1].E = true;
    cpu.ITTR[1].W = true;
    cpu.ITTR[1].S = 2;
    auto v = ptest(0x03000, false, true, false);
    BOOST_TEST(v.R);
    BOOST_TEST(v.T);
}
BOOST_AUTO_TEST_CASE(E) {
    cpu.ITTR[1].logic_base = 0x01;
    cpu.ITTR[1].logic_mask = 0;
    cpu.ITTR[1].E = false;
    cpu.ITTR[1].W = true;
    cpu.ITTR[1].S = 2;
    auto v = ptest(0x01000, false, true, false);
    BOOST_TEST(!v.T);
}
BOOST_AUTO_TEST_SUITE(S)
BOOST_DATA_TEST_CASE(unspecified, bdata::xrange(2), s) {
    cpu.ITTR[1].logic_base = 0x01;
    cpu.ITTR[1].logic_mask = 0;
    cpu.ITTR[1].E = true;
    cpu.ITTR[1].W = true;
    cpu.ITTR[1].S = 2;
    auto v = ptest(0x01000, s, true, false);
    BOOST_TEST(v.R);
    BOOST_TEST(v.T);
}

BOOST_DATA_TEST_CASE(user, bdata::xrange(2), s) {
    cpu.ITTR[1].logic_base = 0x01;
    cpu.ITTR[1].logic_mask = 0;
    cpu.ITTR[1].E = true;
    cpu.ITTR[1].W = true;
    cpu.ITTR[1].S = 0;
    auto v = ptest(0x01000, s, true, false);
    if(s) {
        BOOST_TEST(!v.T);
    } else {
        BOOST_TEST(v.T);
    }
}

BOOST_DATA_TEST_CASE(sys, bdata::xrange(2), s) {
    cpu.ITTR[1].logic_base = 0x01;
    cpu.ITTR[1].logic_mask = 0;
    cpu.ITTR[1].E = true;
    cpu.ITTR[1].W = true;
    cpu.ITTR[1].S = 1;
    auto v = ptest(0x01000, s, true, false);
    if(!s) {
        BOOST_TEST(!v.T);
    } else {
        BOOST_TEST(v.T);
    }
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(W) {
    cpu.ITTR[1].logic_base = 0x01;
    cpu.ITTR[1].logic_mask = 0;
    cpu.ITTR[1].E = true;
    cpu.ITTR[1].W = true;
    cpu.ITTR[1].S = 2;
    auto v = ptest(0x01000, false, true, true);
    BOOST_TEST(!v.T);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(Data) {
    cpu.ITTR[1].logic_base = 0x01;
    cpu.ITTR[1].logic_mask = 0;
    cpu.ITTR[1].E = true;
    cpu.ITTR[1].W = true;
    cpu.ITTR[1].S = 2;
    auto v = ptest(0x01000, false, false, false);
    BOOST_TEST(!v.T);
}
BOOST_AUTO_TEST_SUITE_END()