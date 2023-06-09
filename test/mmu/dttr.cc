#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
uint32_t ptest(uint32_t addr, bool sys, bool code, bool W);
namespace bdata = boost::unit_test::data;
struct AccessFault {};

BOOST_FIXTURE_TEST_SUITE(DTTR, Prepare)
BOOST_AUTO_TEST_SUITE(DTTR0)
BOOST_AUTO_TEST_SUITE(base)

BOOST_AUTO_TEST_CASE(match) {
    cpu.DTTR[0].logic_base = 0x01;
    cpu.DTTR[0].E = true;
    cpu.DTTR[0].W = true;
    cpu.DTTR[0].S = 2;
    auto v = ptest(0x01000, false, false, false);
    BOOST_TEST((v & 3) == 3);
}
BOOST_AUTO_TEST_CASE(unmatch) {
    cpu.DTTR[0].logic_base = 0x01;
    cpu.DTTR[0].E = true;
    cpu.DTTR[0].W = true;
    cpu.DTTR[0].S = 2;
    auto v = ptest(0x02000, false, false, false);
    BOOST_TEST((v & 3) != 3);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(mask) {
    cpu.DTTR[0].logic_base = 0x02;
    cpu.DTTR[0].logic_mask = 0x01;
    cpu.DTTR[0].E = true;
    cpu.DTTR[0].W = true;
    cpu.DTTR[0].S = 2;
    auto v = ptest(0x03000, false, false, false);
    BOOST_TEST((v & 3) == 3);
}
BOOST_AUTO_TEST_CASE(E) {
    cpu.DTTR[0].logic_base = 0x01;
    cpu.DTTR[0].E = false;
    cpu.DTTR[0].W = true;
    cpu.DTTR[0].S = 2;
    auto v = ptest(0x01000, false, false, false);
    BOOST_TEST((v & 3) != 3);
}
BOOST_AUTO_TEST_SUITE(S)
BOOST_DATA_TEST_CASE(unspecified, bdata::xrange(2), s) {
    cpu.DTTR[0].logic_base = 0x01;
    cpu.DTTR[0].E = true;
    cpu.DTTR[0].W = true;
    cpu.DTTR[0].S = 2;
    auto v = ptest(0x01000, s, false, false);
    BOOST_TEST((v & 3) == 3);
}

BOOST_DATA_TEST_CASE(user, bdata::xrange(2), s) {
    cpu.DTTR[0].logic_base = 0x01;
    cpu.DTTR[0].E = true;
    cpu.DTTR[0].W = true;
    cpu.DTTR[0].S = 0;
    auto v = ptest(0x01000, s, false, false);
    if(s) {
        BOOST_TEST((v & 3) != 3);
    } else {
        BOOST_TEST((v & 3) == 3);
    }
}

BOOST_DATA_TEST_CASE(sys, bdata::xrange(2), s) {
    cpu.DTTR[0].logic_base = 0x01;
    cpu.DTTR[0].E = true;
    cpu.DTTR[0].W = true;
    cpu.DTTR[0].S = 1;
    auto v = ptest(0x01000, s, false, false);
    if(!s) {
        BOOST_TEST((v & 3) != 3);
    } else {
        BOOST_TEST((v & 3) == 3);
    }
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(W) {
    cpu.DTTR[0].logic_base = 0x01;
    cpu.DTTR[0].E = true;
    cpu.DTTR[0].W = true;
    cpu.DTTR[0].S = 2;
    BOOST_TEST(ptest(0x01000, false, false, true) == 0);
}

BOOST_AUTO_TEST_CASE(Code) {
    cpu.DTTR[0].logic_base = 0x01;
    cpu.DTTR[0].E = true;
    cpu.DTTR[0].W = true;
    cpu.DTTR[0].S = 2;
    auto v = ptest(0x01000, false, true, false);
    BOOST_TEST((v & 3) != 3);
}
BOOST_AUTO_TEST_SUITE_END()
struct DTTR1_reg {
    DTTR1_reg() { cpu.DTTR[0].E = false; }
};
BOOST_FIXTURE_TEST_SUITE(DTTR1, DTTR1_reg)
BOOST_AUTO_TEST_SUITE(base)

BOOST_AUTO_TEST_CASE(match) {
    cpu.DTTR[1].logic_base = 0x02;
    cpu.DTTR[1].E = true;
    cpu.DTTR[1].W = true;
    cpu.DTTR[1].S = 2;
    auto v = ptest(0x02000, false, false, false);
    BOOST_TEST((v & 3) == 3);
}
BOOST_AUTO_TEST_CASE(unmatch) {
    cpu.DTTR[1].logic_base = 0x01;
    cpu.DTTR[1].E = true;
    cpu.DTTR[1].W = true;
    cpu.DTTR[1].S = 2;
    auto v = ptest(0x02000, false, false, false);
    BOOST_TEST((v & 3) != 3);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(mask) {
    cpu.DTTR[1].logic_base = 0x02;
    cpu.DTTR[1].logic_mask = 0x01;
    cpu.DTTR[1].E = true;
    cpu.DTTR[1].W = true;
    cpu.DTTR[1].S = 2;
    auto v = ptest(0x03000, false, false, false);
    BOOST_TEST((v & 3) == 3);
}
BOOST_AUTO_TEST_CASE(E) {
    cpu.DTTR[1].logic_base = 0x01;
    cpu.DTTR[1].E = false;
    cpu.DTTR[1].W = true;
    cpu.DTTR[1].S = 2;
    auto v = ptest(0x01000, false, false, false);
    BOOST_TEST((v & 3) != 3);
}
BOOST_AUTO_TEST_SUITE(S)
BOOST_DATA_TEST_CASE(unspecified, bdata::xrange(2), s) {
    cpu.DTTR[1].logic_base = 0x01;
    cpu.DTTR[1].E = true;
    cpu.DTTR[1].W = true;
    cpu.DTTR[1].S = 2;
    auto v = ptest(0x01000, s, false, false);
    BOOST_TEST((v & 3) == 3);
}

BOOST_DATA_TEST_CASE(user, bdata::xrange(2), s) {
    cpu.DTTR[1].logic_base = 0x01;
    cpu.DTTR[1].E = true;
    cpu.DTTR[1].W = true;
    cpu.DTTR[1].S = 0;
    auto v = ptest(0x01000, s, false, false);
    if(s) {
        BOOST_TEST((v & 3) != 3);
    } else {
        BOOST_TEST((v & 3) == 3);
    }
}

BOOST_DATA_TEST_CASE(sys, bdata::xrange(2), s) {
    cpu.DTTR[1].logic_base = 0x01;
    cpu.DTTR[1].E = true;
    cpu.DTTR[1].W = true;
    cpu.DTTR[1].S = 1;
    auto v = ptest(0x01000, s, false, false);
    if(!s) {
        BOOST_TEST((v & 3) != 3);
    } else {
        BOOST_TEST((v & 3) == 3);
    }
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(W) {
    cpu.DTTR[1].logic_base = 0x01;
    cpu.DTTR[1].E = true;
    cpu.DTTR[1].W = true;
    cpu.DTTR[1].S = 2;
    BOOST_TEST(ptest(0x01000, false, false, true) == 0);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(Code) {
    cpu.DTTR[1].logic_base = 0x01;
    cpu.DTTR[1].E = true;
    cpu.DTTR[1].S = 2;
    auto v = ptest(0x01000, false, true, false);
    BOOST_TEST((v & 3) != 3);
}
BOOST_AUTO_TEST_SUITE_END()
