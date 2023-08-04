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
namespace DISASM {
extern const char *cc_n[16];
}
BOOST_FIXTURE_TEST_SUITE(Disasm, PrepareDisasm)

BOOST_DATA_TEST_CASE(ADDQ_W, bdata::xrange(0, 7), i) {
    TEST::SET_W(0, 0050103 | i << 9);
    BOOST_TEST(disasm() == fmt::format("ADDQ.W #{}, %D3", i ? i : 8));
}

BOOST_DATA_TEST_CASE(ADDQ_L, bdata::xrange(0, 7), i) {
    TEST::SET_W(0, 0050203 | i << 9);
    BOOST_TEST(disasm() == fmt::format("ADDQ.L #{}, %D3", i ? i : 8));
}

BOOST_DATA_TEST_CASE(SUBQ_B, bdata::xrange(0, 7), i) {
    TEST::SET_W(0, 0050403 | i << 9);
    BOOST_TEST(disasm() == fmt::format("SUBQ.B #{}, %D3", i ? i : 8));
}

BOOST_DATA_TEST_CASE(SUBQ_W, bdata::xrange(0, 7), i) {
    TEST::SET_W(0, 0050503 | i << 9);
    BOOST_TEST(disasm() == fmt::format("SUBQ.W #{}, %D3", i ? i : 8));
}

BOOST_DATA_TEST_CASE(SUBQ_L, bdata::xrange(0, 7), i) {
    TEST::SET_W(0, 0050603 | i << 9);
    BOOST_TEST(disasm() == fmt::format("SUBQ.L #{}, %D3", i ? i : 8));
}

BOOST_DATA_TEST_CASE(Scc, bdata::xrange(16), cc) {
    TEST::SET_W(0, 0050304 | cc << 8);
    BOOST_TEST(disasm() == fmt::format("S{} %D4", DISASM::cc_n[cc]));
}

BOOST_DATA_TEST_CASE(DBcc, bdata::xrange(16), cc) {
    TEST::SET_W(0, 0050314 | cc << 8);
    TEST::SET_W(2, 100);
    BOOST_TEST(disasm() == fmt::format("DB{} %D4, #100", DISASM::cc_n[cc]));
}

BOOST_DATA_TEST_CASE(TRAPcc_W, bdata::xrange(16), cc) {
    TEST::SET_W(0, 0050372 | cc << 8);
    TEST::SET_W(2, 0x40);
    BOOST_TEST(disasm() == fmt::format("TRAP{} #0x0040", DISASM::cc_n[cc]));
}

BOOST_DATA_TEST_CASE(TRAPcc_L, bdata::xrange(16), cc) {
    TEST::SET_W(0, 0050373 | cc << 8);
    TEST::SET_L(2, 0x12345678);
    BOOST_TEST(disasm() == fmt::format("TRAP{} #0x12345678", DISASM::cc_n[cc]));
}

BOOST_DATA_TEST_CASE(TRAPcc, bdata::xrange(16), cc) {
    TEST::SET_W(0, 0050374 | cc << 8);
    BOOST_TEST(disasm() == fmt::format("TRAP{}", DISASM::cc_n[cc]));
}

BOOST_AUTO_TEST_CASE(BRA_B) {
    TEST::SET_W(0, 0060000 | 0x30);
    BOOST_TEST(disasm() == "BRA #000032");
}

BOOST_AUTO_TEST_CASE(BRA_W) {
    TEST::SET_W(0, 0060000);
    TEST::SET_W(2, 0x1000);
    BOOST_TEST(disasm() == "BRA #001002");
}

BOOST_AUTO_TEST_CASE(BRA_L) {
    TEST::SET_W(0, 0060000 | 0xff);
    TEST::SET_L(2, 0x100000);
    BOOST_TEST(disasm() == "BRA #100002");
}

BOOST_AUTO_TEST_CASE(BSR_B) {
    TEST::SET_W(0, 0060400 | 0x30);
    BOOST_TEST(disasm() == "BSR #000032");
}

BOOST_AUTO_TEST_CASE(BSR_W) {
    TEST::SET_W(0, 0060400);
    TEST::SET_W(2, 0x1000);
    BOOST_TEST(disasm() == "BSR #001002");
}

BOOST_AUTO_TEST_CASE(BSR_L) {
    TEST::SET_W(0, 0060400 | 0xff);
    TEST::SET_L(2, 0x100000);
    BOOST_TEST(disasm() == "BSR #100002");
}

BOOST_DATA_TEST_CASE(Bcc_B, bdata::xrange(2,16), cc) {
    TEST::SET_W(0, 0060000 | 40 |cc << 8);
    BOOST_TEST(disasm() == fmt::format("B{} #00002a", DISASM::cc_n[cc]));
}

BOOST_DATA_TEST_CASE(Bcc_W, bdata::xrange(2,16), cc) {
    TEST::SET_W(0, 0060000|cc << 8);
    TEST::SET_W(2, 0x1000);
    BOOST_TEST(disasm() == fmt::format("B{} #001002", DISASM::cc_n[cc]));
}

BOOST_DATA_TEST_CASE(Bcc_L, bdata::xrange(2,16), cc) {
    TEST::SET_W(0, 0060000 | 0xff|cc << 8);
    TEST::SET_L(2, 0x20000);
    BOOST_TEST(disasm() == fmt::format("B{} #020002", DISASM::cc_n[cc]));
}

BOOST_DATA_TEST_CASE(MOVEQ, bdata::xrange(8), dn) {
    auto d = -7;
    TEST::SET_W(0, 0070000 | dn << 9 | static_cast<uint8_t>(d));
    BOOST_TEST(disasm() == fmt::format("MOVEQ #{}, %D{}", d, dn));
}

BOOST_AUTO_TEST_SUITE_END()