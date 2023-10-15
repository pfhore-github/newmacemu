#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
mmu_result ptest(uint32_t addr, bool sys, bool code, bool W);
namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_CASE(MMU_Disabled, Prepare) {
    cpu.TCR_E = false;
    auto v = ptest(0x00300000 >> 12, false, false, false);
    BOOST_TEST(v.paddr ==  0x0300);
    BOOST_TEST(v.R);
}

