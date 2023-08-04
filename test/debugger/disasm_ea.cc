#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/monomorphic/generators/random.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <fmt/format.h>
std::string disasm_ea(int type, int reg, int sz);
static std::string RN(int n) {
    return fmt::format("%{}{}", n & 8 ? 'A' : 'D', n & 7);
}
namespace bdata = boost::unit_test::data;
struct PrepareDisasm {
    PrepareDisasm() {
        cpu.PC = 0;
        cpu.TCR_E = false;
    }
};
BOOST_FIXTURE_TEST_SUITE(Disasm, PrepareDisasm)

BOOST_AUTO_TEST_SUITE(ea)
BOOST_DATA_TEST_CASE(dn, bdata::xrange(8), n) {
    BOOST_TEST(disasm_ea(0, n, 0) == fmt::format("%D{}", n));
}
BOOST_DATA_TEST_CASE(An, bdata::xrange(8), n) {
    BOOST_TEST(disasm_ea(1, n, 0) == fmt::format("%A{}", n));
}
BOOST_DATA_TEST_CASE(Mem, bdata::xrange(8), n) {
    BOOST_TEST(disasm_ea(2, n, 0) == fmt::format("(%A{})", n));
}
BOOST_DATA_TEST_CASE(Incr, bdata::xrange(8), n) {
    BOOST_TEST(disasm_ea(3, n, 0) == fmt::format("(%A{})+", n));
}

BOOST_DATA_TEST_CASE(Decr, bdata::xrange(8), n) {
    BOOST_TEST(disasm_ea(4, n, 0) == fmt::format("-(%A{})", n));
}
BOOST_DATA_TEST_CASE(dAn, bdata::xrange(8) ^ bdata::random(-1000, 1000), rn,
                     d) {
    TEST::SET_W(0, d);
    BOOST_TEST(disasm_ea(5, rn, 0) == fmt::format("({}, %A{})", d, rn));
    BOOST_TEST(cpu.PC == 2);
}
BOOST_DATA_TEST_CASE(extBreif,
                     bdata::xrange(16) ^ bdata::random(0, 7) ^
                         bdata::random(0, 7) ^ bdata::random(0, 7) ^
                         bdata::random(-20, 20),
                     rn, an, w_, cc_, d) {
    bool w = w_ & 1;
    auto cc = cc_ & 3;
    TEST::SET_W(0, rn << 12 | w << 11 | cc << 9 | static_cast<uint8_t>(d));
    BOOST_TEST(disasm_ea(6, an, 0) == fmt::format("({}, %A{}, {}{}*{})", d, an,
                                                  RN(rn), w ? "" : ".W",
                                                  1 << cc));
}

BOOST_AUTO_TEST_SUITE(extLong)

BOOST_DATA_TEST_CASE(a_i, bdata::xrange(2), i) {
    TEST::SET_W(0, 2 << 12 | 1 << 11 | i << 7 | 1 << 8 | 1 << 4);
    BOOST_TEST(disasm_ea(6, 1, 0) ==
               fmt::format("({}%D2*1)", i ? "" : "%A1, "));
    BOOST_TEST(cpu.PC == 2);
}

BOOST_DATA_TEST_CASE(r_i, bdata::xrange(2), i) {
    TEST::SET_W(0, 2 << 12 | 1 << 11 | i << 6 | 1 << 8 | 1 << 4);
    BOOST_TEST(disasm_ea(6, 1, 0) ==
               fmt::format("(%A1{})", i ? "" : ", %D2*1"));
    BOOST_TEST(cpu.PC == 2);
}

BOOST_AUTO_TEST_CASE(no_bd) {
    TEST::SET_W(0, 2 << 12 | 1 << 11 | 1 << 8 | 1 << 4);
    BOOST_TEST(disasm_ea(6, 1, 0) == "(%A1, %D2*1)");
    BOOST_TEST(cpu.PC == 2);
}

BOOST_AUTO_TEST_CASE(bd_w) {
    TEST::SET_W(0, 2 << 12 | 1 << 11 | 1 << 8 | 2 << 4);
    TEST::SET_W(2, 200);
    BOOST_TEST(disasm_ea(6, 1, 0) == "(200, %A1, %D2*1)");
    BOOST_TEST(cpu.PC == 4);
}

BOOST_AUTO_TEST_CASE(bd_l) {
    TEST::SET_W(0, 2 << 12 | 1 << 11 | 1 << 8 | 3 << 4);
    TEST::SET_L(2, 40000);
    BOOST_TEST(disasm_ea(6, 1, 0) == "(40000, %A1, %D2*1)");
    BOOST_TEST(cpu.PC == 6);
}

BOOST_AUTO_TEST_CASE(pre_no_od) {
    TEST::SET_W(0, 2 << 12 | 1 << 11 | 1 << 8 | 1 << 4 | 1);
    BOOST_TEST(disasm_ea(6, 1, 0) == "([%A1, %D2*1])");
    BOOST_TEST(cpu.PC == 2);
}

BOOST_AUTO_TEST_CASE(od_w) {
    TEST::SET_W(0, 2 << 12 |1 << 11 |  1 << 8 | 1 << 4 | 2);
    TEST::SET_W(2, 100);
    BOOST_TEST(disasm_ea(6, 1, 0) == "([%A1, %D2*1], 100)");
    BOOST_TEST(cpu.PC == 4);
}

BOOST_AUTO_TEST_CASE(od_l) {
    TEST::SET_W(0, 2 << 12 |1 << 11 |  1 << 8 | 1 << 4 | 3);
    TEST::SET_L(2, 10000);
    BOOST_TEST(disasm_ea(6, 1, 0) == "([%A1, %D2*1], 10000)");
    BOOST_TEST(cpu.PC == 6);
}

BOOST_AUTO_TEST_CASE(post) {
    TEST::SET_W(0, 2 << 12 | 1 << 11 | 1 << 8 | 1 << 4 | 1 << 2 | 1);
    BOOST_TEST(disasm_ea(6, 1, 0) == "([%A1], %D2*1)");
    BOOST_TEST(cpu.PC == 2);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(iaddr_w) {
    TEST::SET_W(0, 0x100);
    BOOST_TEST(disasm_ea(7, 0, 0) == "(0x100)");
    BOOST_TEST(cpu.PC == 2);
}
BOOST_AUTO_TEST_CASE(iaddr_l) {
    TEST::SET_L(0, 0x10000);
    BOOST_TEST(disasm_ea(7, 1, 0) == "(0x10000)");
    BOOST_TEST(cpu.PC == 4);
}

BOOST_AUTO_TEST_CASE(pc_d) {
    TEST::SET_W(0, 50);
    BOOST_TEST(disasm_ea(7, 2, 0) == "(50, %PC)");
    BOOST_TEST(cpu.PC == 2);
}

BOOST_AUTO_TEST_CASE(pc_ext) {
    TEST::SET_W(0, 2 << 12 | 1 << 11 | 0xFE);
    BOOST_TEST(disasm_ea(7, 3, 0) == "(-2, %PC, %D2*1)");
    BOOST_TEST(cpu.PC == 2);
}

BOOST_AUTO_TEST_CASE(immB) {
    cpu.PC = 0;
    TEST::SET_W(0, 30);
    BOOST_TEST(disasm_ea(7, 4, 1) == "#30");
    BOOST_TEST(cpu.PC == 2);
}

BOOST_AUTO_TEST_CASE(immW) {
    cpu.PC = 0;
    TEST::SET_W(0, 3000);
    BOOST_TEST(disasm_ea(7, 4, 2) == "#3000");
    BOOST_TEST(cpu.PC == 2);
}

BOOST_AUTO_TEST_CASE(immL) {
    cpu.PC = 0;
    TEST::SET_L(0, 300000);
    BOOST_TEST(disasm_ea(7, 4, 4) == "#300000");
    BOOST_TEST(cpu.PC == 4);
}

BOOST_AUTO_TEST_CASE(immF) {
    cpu.PC = 0;
    TEST::SET_L(0, std::bit_cast<uint32_t>(1.5f));
    BOOST_TEST(disasm_ea(7, 4, -4) == "#1.5");
    BOOST_TEST(cpu.PC == 4);
}

BOOST_AUTO_TEST_CASE(immD) {
    cpu.PC = 0;
    uint64_t vv = std::bit_cast<uint64_t>(1.5);
    TEST::SET_L(0, vv >> 32);
    TEST::SET_L(4, vv);
    BOOST_TEST(disasm_ea(7, 4, -8) == "#1.5");
    BOOST_TEST(cpu.PC == 8);
}

BOOST_AUTO_TEST_CASE(immX) {
    cpu.PC = 0;
    TEST::SET_W(0, 0x8000 | (16383 + 3));
    TEST::SET_L(4, 0xC0000000);
    TEST::SET_L(8, 0);
    BOOST_TEST(disasm_ea(7, 4, -12) == "#-12.000000");
    BOOST_TEST(cpu.PC == 12);
}

BOOST_AUTO_TEST_CASE(immP) {
    cpu.PC = 0;
    TEST::SET_L(0, 0x00010001);
    TEST::SET_L(4, 0x33330000);
    TEST::SET_L(8, 0);
    BOOST_TEST(disasm_ea(7, 4, 12) == "#13.333000");
    BOOST_TEST(cpu.PC == 12);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()