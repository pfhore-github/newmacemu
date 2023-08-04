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
static char ea_x[] = {'L', 'S', 'X', 'P', 'W', 'D', 'B'};
static int fops[] = {0x0,  0x1,  0x2,  0x3,  0x4,  0x6,  0x8,  0x9,  0xA,  0xC,
                     0xD,  0xE,  0xF,  0x10, 0x11, 0x12, 0x14, 0x15, 0x16, 0x18,
                     0x19, 0x1A, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
                     0x28, 0x38, 0x40, 0x41, 0x44, 0x45, 0x58, 0x5A, 0x5C, 0x5E,
                     0x60, 0x62, 0x63, 0x64, 0x66, 0x67, 0x68, 0x6C};
namespace DISASM {
extern std::unordered_map<int, const char *> fop;
}
BOOST_FIXTURE_TEST_SUITE(Disasm, PrepareDisasm)
BOOST_AUTO_TEST_SUITE(FOP)
BOOST_DATA_TEST_CASE(FROM_REG, fops *bdata::xrange(8) ^ bdata::random(0, 7), op,
                     fpm, fpn) {
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, op | fpm << 10 | fpn << 7);
    BOOST_TEST(disasm() ==
               fmt::format("{}.X %FP{}, %FP{}", DISASM::fop[op], fpm, fpn));
}

BOOST_DATA_TEST_CASE(FROM_EA, fops *bdata::xrange(7) ^ bdata::random(0, 7), op,
                     sz, fpn) {
    TEST::SET_W(0, 0171022);
    TEST::SET_W(2, op | 1 << 14 | sz << 10 | fpn << 7);
    BOOST_TEST(disasm() == fmt::format("{}.{} (%A2), %FP{}", DISASM::fop[op],
                                       ea_x[sz], fpn));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(FSINCOS)
BOOST_DATA_TEST_CASE(FROM_REG,
                     bdata::xrange(7) ^ bdata::random(0, 7) ^
                         bdata::random(0, 7),
                     fpm, fps, fpc) {
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0x30 | fpc | fpm << 10 | fps << 7);
    BOOST_TEST(disasm() ==
               fmt::format("FSINCOS.X %FP{}, %FP{}, %FP{}", fpm, fpc, fps));
}

BOOST_DATA_TEST_CASE(FROM_EA,
                     bdata::xrange(7) ^ bdata::random(0, 7) ^
                         bdata::random(0, 7),
                     sz, fps, fpc) {
    TEST::SET_W(0, 0171022);
    TEST::SET_W(2, 0x30 | fpc | 1 << 14 | sz << 10 | fps << 7);
    BOOST_TEST(disasm() == fmt::format("FSINCOS.{} (%A2), %FP{}, %FP{}",
                                       ea_x[sz], fpc, fps));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(FTST)
BOOST_DATA_TEST_CASE(FROM_REG, bdata::xrange(7), fpm) {
    TEST::SET_W(0, 0171000);
    TEST::SET_W(2, 0x3A | fpm << 10);
    BOOST_TEST(disasm() == fmt::format("FTST.X %FP{}", fpm));
}

BOOST_DATA_TEST_CASE(FROM_EA, bdata::xrange(7), sz) {
    TEST::SET_W(0, 0171022);
    TEST::SET_W(2, 0x3A | 1 << 14 | sz << 10);
    BOOST_TEST(disasm() == fmt::format("FTST.{} (%A2)", ea_x[sz]));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
