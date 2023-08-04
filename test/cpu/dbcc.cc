#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <fmt/core.h>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
namespace DISASM {
extern const char *cc_n[16];
}
BOOST_FIXTURE_TEST_SUITE(DBcc, Prepare)

BOOST_DATA_TEST_CASE(T, bdata::xrange(2), tr) {
    TEST::SET_W(0, 0050310 | 0 << 8 | 4);
    TEST::SET_W(2, 0x30);
    cpu.C = false;
    cpu.T = tr;
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
}

BOOST_AUTO_TEST_CASE(F1) {
    TEST::SET_W(0, 0050310 | 1 << 8 | 4);
    cpu.D[4] = 5;
    TEST::SET_W(2, 0x30);
    run_test();
    BOOST_TEST(cpu.D[4] == 4);
    BOOST_TEST(cpu.PC == 0x32);
}

BOOST_AUTO_TEST_CASE(F2) {
    TEST::SET_W(0, 0050310 | 1 << 8 | 4);
    cpu.D[4] = 0;
    TEST::SET_W(2, 0x30);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(cpu.D[4] == 0xffff);
}

BOOST_AUTO_TEST_CASE(F_traced) {
    TEST::SET_W(0, 0050310 | 1 << 8 | 4);
    cpu.D[4] = 5;
    TEST::SET_W(2, 0x30);
    cpu.T = 1;
    BOOST_TEST(run_test() == 9);
    BOOST_TEST(cpu.D[4] == 4);
}

BOOST_AUTO_TEST_SUITE_END()
