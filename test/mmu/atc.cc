#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
mmu_result ptest(uint32_t addr, bool sys, bool code, bool W);
namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(ATC, Prepare) 
BOOST_AUTO_TEST_CASE(Resident)
{
    cpu.TCR_E = true;
    cpu.l_atc[0][0x00300] = {
        0x20,
        0,
        false,
        0,
        true, false, true,
    };
    auto v = ptest(0x00300, false, false, false);
    BOOST_TEST(v.paddr == 0x20);
    BOOST_TEST(v.R);
    BOOST_TEST(v.M);
    BOOST_TEST(!v.G);
    BOOST_TEST(!v.S);
}

BOOST_AUTO_TEST_CASE(ResidentG)
{
    cpu.TCR_E = true;
    cpu.g_atc[0][0x00300] = {
        0x20,
        0,
        false,
        0,
        true, false, true,
    };
    auto v = ptest(0x00300, false, false, false);
    BOOST_TEST(v.paddr == 0x20);
    BOOST_TEST(v.R);
    BOOST_TEST(v.M);
    BOOST_TEST(v.G);
    BOOST_TEST(!v.S);
}

BOOST_AUTO_TEST_CASE(ResidentS)
{
    cpu.TCR_E = true;
    cpu.l_atc[1][0x00300] = {
        0x20,
        0,
        false,
        0,
        true, false, true,
    };
    auto v = ptest(0x00300, true, false, false);
    BOOST_TEST(v.paddr == 0x20);
    BOOST_TEST(v.R);
    BOOST_TEST(v.M);
    BOOST_TEST(!v.G);
    BOOST_TEST(!v.S);
}

BOOST_AUTO_TEST_CASE(ResidentSG)
{
    cpu.TCR_E = true;
    cpu.g_atc[1][0x00300] = {
        0x20,
        0,
        false,
        0,
        true, false, true,
    };
    auto v = ptest(0x00300, true, false, false);
    BOOST_TEST(v.paddr == 0x20);
    BOOST_TEST(v.R);
    BOOST_TEST(v.M);
    BOOST_TEST(v.G);
    BOOST_TEST(!v.S);
}

BOOST_AUTO_TEST_CASE(NonResident)
{
    cpu.TCR_E = true;
    cpu.l_atc[0][0x00300] = {
        0,
        0,
        false,
        0,
        false, false, false,
    };
    auto v = ptest(0x00300, false, false, false);
    BOOST_TEST(!v.R);
}

BOOST_AUTO_TEST_SUITE_END()