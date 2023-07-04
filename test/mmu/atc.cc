#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
uint32_t ptest(uint32_t addr, bool sys, bool code, bool W);
namespace bdata = boost::unit_test::data;
struct AccessFault {};

BOOST_FIXTURE_TEST_SUITE(ATC, Prepare) 
BOOST_AUTO_TEST_CASE(Resident)
{
    cpu.TCR_E = true;
    cpu.u_atc[0x00300] = {
        0x20,
        0,
        false,
        0,
        true, false, true,
    };
    auto v = ptest(0x00300, false, false, false);
    BOOST_TEST(v ==  (0x20001 | 1 << 4));
}

BOOST_AUTO_TEST_CASE(ResidentG)
{
    cpu.TCR_E = true;
    cpu.ug_atc[0x00300] = {
        0x20,
        0,
        false,
        0,
        true, false, true,
    };
    auto v = ptest(0x00300, false, false, false);
    BOOST_TEST(v ==  (0x20001 | 1 << 4 | 1 << 10));
}

BOOST_AUTO_TEST_CASE(ResidentS)
{
    cpu.TCR_E = true;
    cpu.s_atc[0x00300] = {
        0x20,
        0,
        false,
        0,
        true, false, true,
    };
    auto v = ptest(0x00300, true, false, false);
    BOOST_TEST(v ==  (0x20001 | 1 << 4));
}

BOOST_AUTO_TEST_CASE(ResidentSG)
{
    cpu.TCR_E = true;
    cpu.sg_atc[0x00300] = {
        0x20,
        0,
        false,
        0,
        true, false, true,
    };
    auto v = ptest(0x00300, true, false, false);
    BOOST_TEST(v ==  (0x20001 | 1 << 4 | 1 << 10));
}

BOOST_AUTO_TEST_CASE(NonResident)
{
    cpu.TCR_E = true;
    cpu.u_atc[0x00300] = {
        0,
        0,
        false,
        0,
        false, false, false,
    };
    auto v = ptest(0x00300, false, false, false);
    BOOST_TEST(v == 0);
}

BOOST_AUTO_TEST_SUITE_END()