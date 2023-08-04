#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/monomorphic/generators/random.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <fmt/format.h>
std::string disasm_ea(int type, int reg, int sz);
namespace bdata = boost::unit_test::data;
struct PrepareDisasm {
    PrepareDisasm() {
        cpu.PC = 0;
        cpu.TCR_E = false;
    }
};
BOOST_FIXTURE_TEST_SUITE(Disasm, PrepareDisasm)


BOOST_DATA_TEST_CASE(MOVE_B, bdata::xrange(8) , dst) {
    TEST::SET_W(0, 0010002 | dst << 9 );
    BOOST_TEST(disasm() == fmt::format("MOVE.B %D2, %D{}", dst));
}

BOOST_DATA_TEST_CASE(MOVE_L, bdata::xrange(8) , dst) {
    TEST::SET_W(0, 0020002 | dst << 9 );
    BOOST_TEST(disasm() == fmt::format("MOVE.L %D2, %D{}", dst));
}

BOOST_DATA_TEST_CASE(MOVEA_L, bdata::xrange(8), dst) {
    TEST::SET_W(0, 0020102 | dst << 9 );
    BOOST_TEST(disasm() == fmt::format("MOVEA.L %D2, %A{}", dst));
}

BOOST_DATA_TEST_CASE(MOVE_W, bdata::xrange(8) , dst) {
    TEST::SET_W(0, 0030002 | dst << 9 );
    BOOST_TEST(disasm() == fmt::format("MOVE.W %D2, %D{}", dst));
}

BOOST_DATA_TEST_CASE(MOVEA_W, bdata::xrange(8), dst) {
    TEST::SET_W(0, 0030102 | dst << 9 );
    BOOST_TEST(disasm() == fmt::format("MOVEA.W %D2, %A{}", dst));
}

BOOST_AUTO_TEST_SUITE_END()