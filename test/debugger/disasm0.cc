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

BOOST_AUTO_TEST_CASE(ORI_B) {
    auto d = 0x13;
    TEST::SET_W(0, 0000001);
    TEST::SET_W(2, d);
    BOOST_TEST(disasm() == fmt::format("ORI.B #0x{:02x}, %D1", d));
}

BOOST_AUTO_TEST_CASE(ORI_CCR) {
    TEST::SET_W(0, 0000074);
    TEST::SET_W(2, 0x0023);
    BOOST_TEST(disasm() == "ORI.B #0x23, %CCR");
}

BOOST_AUTO_TEST_CASE(ORI_W) {
    auto d = 0x1234;
    TEST::SET_W(0, 0000101);
    TEST::SET_W(2, d);
    BOOST_TEST(disasm() == fmt::format("ORI.W #0x{:04x}, %D1", d));
}

BOOST_AUTO_TEST_CASE(ORI_SR) {
    TEST::SET_W(0, 0000174);
    TEST::SET_W(2, 0x1234);
    BOOST_TEST(disasm() == "ORI.W #0x1234, %SR");
}

BOOST_AUTO_TEST_CASE(ORI_L) {
    auto d = 0x12345678;
    TEST::SET_W(0, 0000201);
    TEST::SET_L(2, d);
    BOOST_TEST(disasm() == fmt::format("ORI.L #0x{:08x}, %D1", d));
}

BOOST_DATA_TEST_CASE(CMP2_B, bdata::xrange(16), rn) {
    TEST::SET_W(0, 0000320 | 2);
    TEST::SET_W(2, rn << 12);
    BOOST_TEST(disasm() ==
               fmt::format("CMP2.B (%A2), {}", RN(rn)));
}

BOOST_DATA_TEST_CASE(CHK2_B, bdata::xrange(16), rn) {
    TEST::SET_W(0, 0000320 | 2);
    TEST::SET_W(2, rn << 12 | 1 << 11);
    BOOST_TEST(disasm() ==
               fmt::format("CHK2.B (%A2), {}", RN(rn)));
}

BOOST_AUTO_TEST_CASE(ANDI_B) {
    auto d = 0x13;
    TEST::SET_W(0, 0001001);
    TEST::SET_W(2, d);
    BOOST_TEST(disasm() == fmt::format("ANDI.B #0x{:02x}, %D1", d));
}

BOOST_AUTO_TEST_CASE(ANDI_CCR) {
    TEST::SET_W(0, 0001074);
    TEST::SET_W(2, 0x0023);
    BOOST_TEST(disasm() == "ANDI.B #0x23, %CCR");
}

BOOST_AUTO_TEST_CASE(ANDI_W) {
    auto d = 0x1234;
    TEST::SET_W(0, 0001101);
    TEST::SET_W(2, d);
    BOOST_TEST(disasm() == fmt::format("ANDI.W #0x{:04x}, %D1", d));
}

BOOST_AUTO_TEST_CASE(ANDI_SR) {
    TEST::SET_W(0, 0001174);
    TEST::SET_W(2, 0x1234);
    BOOST_TEST(disasm() == "ANDI.W #0x1234, %SR");
}

BOOST_AUTO_TEST_CASE(ANDI_L) {
    auto d = 0x12345678;
    TEST::SET_W(0, 0001201);
    TEST::SET_L(2, d);
    BOOST_TEST(disasm() == fmt::format("ANDI.L #0x{:08x}, %D1", d));
}

BOOST_DATA_TEST_CASE(CMP2_W, bdata::xrange(16), rn) {
    TEST::SET_W(0, 0001320 | 2);
    TEST::SET_W(2, rn << 12);
    BOOST_TEST(disasm() ==
               fmt::format("CMP2.W (%A2), {}", RN(rn)));
}

BOOST_DATA_TEST_CASE(CHK2_W, bdata::xrange(16), rn) {
    TEST::SET_W(0, 0001320 | 2);
    TEST::SET_W(2, rn << 12 | 1 << 11);
    BOOST_TEST(disasm() ==
               fmt::format("CHK2.W (%A2), {}", RN(rn)));
}

BOOST_AUTO_TEST_CASE(SUBI_B) {
    auto d = 23;
    TEST::SET_W(0, 0002003);
    TEST::SET_W(2, d);
    BOOST_TEST(disasm() == fmt::format("SUBI.B #{}, %D3", d));
}

BOOST_AUTO_TEST_CASE(SUBI_W) {
    auto d = 2345;
    TEST::SET_W(0, 0002103);
    TEST::SET_W(2, d);
    BOOST_TEST(disasm() == fmt::format("SUBI.W #{}, %D3", d));
}

BOOST_AUTO_TEST_CASE(SUBI_L) {
    auto d = 2345678;
    TEST::SET_W(0, 0002203);
    TEST::SET_L(2, d);
    BOOST_TEST(disasm() == fmt::format("SUBI.L #{}, %D3", d));
}

BOOST_DATA_TEST_CASE(CMP2_L, bdata::xrange(16), rn) {
    TEST::SET_W(0, 0002320 | 2);
    TEST::SET_W(2, rn << 12);
    BOOST_TEST(disasm() ==
               fmt::format("CMP2.L (%A2), {}", RN(rn)));
}

BOOST_DATA_TEST_CASE(CHK2_L, bdata::xrange(16), rn) {
    TEST::SET_W(0, 0002320 | 2);
    TEST::SET_W(2, rn << 12 | 1 << 11);
    BOOST_TEST(disasm() ==
               fmt::format("CHK2.L (%A2), {}", RN(rn)));
}

BOOST_AUTO_TEST_CASE(ADDI_B) {
    auto d = 23;
    TEST::SET_W(0, 0003003);
    TEST::SET_W(2, d);
    BOOST_TEST(disasm() == fmt::format("ADDI.B #{}, %D3", d));
}

BOOST_AUTO_TEST_CASE(ADDI_W) {
    auto d = 2345;
    TEST::SET_W(0, 0003103);
    TEST::SET_W(2, d);
    BOOST_TEST(disasm() == fmt::format("ADDI.W #{}, %D3", d));
}

BOOST_AUTO_TEST_CASE(ADDI_L) {
    auto d = 2345678;
    TEST::SET_W(0, 0003203);
    TEST::SET_L(2, d);
    BOOST_TEST(disasm() == fmt::format("ADDI.L #{}, %D3", d));
}
BOOST_DATA_TEST_CASE(BTST_B_IMM, bdata::xrange(1, 7), i) {
    TEST::SET_W(0, 0004023);
    TEST::SET_W(2, i);
    BOOST_TEST(disasm() == fmt::format("BTST.B #{}, (%A3)", i ? i : 8));
}

BOOST_DATA_TEST_CASE(BTST_L_IMM, bdata::xrange(0, 31), i) {
    TEST::SET_W(0, 0004003);
    TEST::SET_W(2, i);
    BOOST_TEST(disasm() == fmt::format("BTST.L #{}, %D3", i ? i : 8));
}

BOOST_DATA_TEST_CASE(BCHG_B_IMM, bdata::xrange(0, 7), i) {
    TEST::SET_W(0, 0004123);
    TEST::SET_W(2, i);
    BOOST_TEST(disasm() == fmt::format("BCHG.B #{}, (%A3)", i ? i : 8));
}

BOOST_DATA_TEST_CASE(BCHG_L_IMM, bdata::xrange(0, 31), i) {
    TEST::SET_W(0, 0004103);
    TEST::SET_W(2, i);
    BOOST_TEST(disasm() == fmt::format("BCHG.L #{}, %D3", i ? i : 8));
}

BOOST_DATA_TEST_CASE(BCLR_B_IMM, bdata::xrange(0, 7), i) {
    TEST::SET_W(0, 0004223);
    TEST::SET_W(2, i);
    BOOST_TEST(disasm() == fmt::format("BCLR.B #{}, (%A3)", i ? i : 8));
}

BOOST_DATA_TEST_CASE(BCLR_L_IMM, bdata::xrange(0, 31), i) {
    TEST::SET_W(0, 0004203);
    TEST::SET_W(2, i);
    BOOST_TEST(disasm() == fmt::format("BCLR.L #{}, %D3", i ? i : 8));
}

BOOST_DATA_TEST_CASE(BSET_B_IMM, bdata::xrange(0, 7), i) {
    TEST::SET_W(0, 0004323);
    TEST::SET_W(2, i);
    BOOST_TEST(disasm() == fmt::format("BSET.B #{}, (%A3)", i ? i : 8));
}

BOOST_DATA_TEST_CASE(BSET_L_IMM, bdata::xrange(0, 31), i) {
    TEST::SET_W(0, 0004303);
    TEST::SET_W(2, i);
    BOOST_TEST(disasm() == fmt::format("BSET.L #{}, %D3", i ? i : 8));
}

BOOST_AUTO_TEST_CASE(EORI_B) {
    auto d = 0x13;
    TEST::SET_W(0, 0005003);
    TEST::SET_W(2, d);
    BOOST_TEST(disasm() == fmt::format("EORI.B #0x{:02x}, %D3", d));
}

BOOST_AUTO_TEST_CASE(EORI_CCR) {
    TEST::SET_W(0, 0005074);
    TEST::SET_W(2, 0x0023);
    BOOST_TEST(disasm() == "EORI.B #0x23, %CCR");
}

BOOST_AUTO_TEST_CASE(EORI_W) {
    auto d = 0x1234;
    TEST::SET_W(0, 0005103);
    TEST::SET_W(2, d);
    BOOST_TEST(disasm() == fmt::format("EORI.W #0x{:04x}, %D3", d));
}

BOOST_AUTO_TEST_CASE(EORI_CR) {
    TEST::SET_W(0, 0005174);
    TEST::SET_W(2, 0x1234);
    BOOST_TEST(disasm() == "EORI.W #0x1234, %SR");
}

BOOST_AUTO_TEST_CASE(EORI_L) {
    auto d = 0x12345678;
    TEST::SET_W(0, 0005203);
    TEST::SET_L(2, d);
    BOOST_TEST(disasm() == fmt::format("EORI.L #0x{:08x}, %D3", d));
}

BOOST_DATA_TEST_CASE(CAS_B, bdata::xrange(8) ^ bdata::random(0, 7), dc, du) {
    TEST::SET_W(0, 0005320 | 2);
    TEST::SET_W(2, du << 6 | dc);
    BOOST_TEST(disasm() == fmt::format("CAS.B %D{}, %D{}, (%A2)", dc, du));
}

BOOST_AUTO_TEST_CASE(CMPI_B) {
    auto d = 23;
    TEST::SET_W(0, 0006003);
    TEST::SET_W(2, d);
    BOOST_TEST(disasm() == fmt::format("CMPI.B #{}, %D3", d));
}

BOOST_AUTO_TEST_CASE(CMPI_W) {
    auto d = 2345;
    TEST::SET_W(0, 0006103);
    TEST::SET_W(2, d);
    BOOST_TEST(disasm() == fmt::format("CMPI.W #{}, %D3", d));
}

BOOST_AUTO_TEST_CASE(CMPI_L) {
    auto d = 234567;
    TEST::SET_W(0, 0006203);
    TEST::SET_L(2, d);
    BOOST_TEST(disasm() == fmt::format("CMPI.L #{}, %D3", d));
}

BOOST_DATA_TEST_CASE(CAS_W, bdata::xrange(8) ^ bdata::random(0, 7), dc, du) {
    TEST::SET_W(0, 0006320 | 2);
    TEST::SET_W(2, du << 6 | dc);
    BOOST_TEST(disasm() == fmt::format("CAS.W %D{}, %D{}, (%A2)", dc, du));
}

BOOST_DATA_TEST_CASE(CAS2_W,
                     bdata::xrange(8) ^ bdata::random(0, 7) ^
                         bdata::random(0, 15) ^ bdata::random(0, 7) ^
                         bdata::random(0, 7) ^ bdata::random(0, 15),
                     dc1, du1, rn1, dc2, du2, rn2) {
    TEST::SET_W(0, 0006374);
    TEST::SET_W(2, rn1 << 12 | du1 << 6 | dc1);
    TEST::SET_W(4, rn2 << 12 | du2 << 6 | dc2);
    BOOST_TEST(disasm() ==
               fmt::format("CAS2.W %D{}:%D{}, %D{}:%D{}, ({}:{})", dc1,
                           dc2, du1, du2, RN(rn1), RN(rn2)));
}

BOOST_DATA_TEST_CASE(MOVES_B_LOAD, bdata::xrange(16), rn) {
    TEST::SET_W(0, 0007024);
    TEST::SET_W(2, rn << 12);
    BOOST_TEST(disasm() == fmt::format("MOVES.B (%A4), {}", RN(rn)));
}

BOOST_DATA_TEST_CASE(MOVES_B_STORE, bdata::xrange(16), rn) {
    TEST::SET_W(0, 0007024);
    TEST::SET_W(2, rn << 12 | 0x0800);
    BOOST_TEST(disasm() == fmt::format("MOVES.B {}, (%A4)", RN(rn)));
}

BOOST_DATA_TEST_CASE(MOVES_W_LOAD, bdata::xrange(16), rn) {
    TEST::SET_W(0, 0007124);
    TEST::SET_W(2, rn << 12);
    BOOST_TEST(disasm() == fmt::format("MOVES.W (%A4), {}", RN(rn)));
}

BOOST_DATA_TEST_CASE(MOVES_W_STORE, bdata::xrange(16), rn) {
    TEST::SET_W(0, 0007124);
    TEST::SET_W(2, rn << 12 | 0x0800);
    BOOST_TEST(disasm() == fmt::format("MOVES.W {}, (%A4)", RN(rn)));
}

BOOST_DATA_TEST_CASE(MOVES_L_LOAD, bdata::xrange(16), rn) {
    TEST::SET_W(0, 0007224);
    TEST::SET_W(2, rn << 12);
    BOOST_TEST(disasm() == fmt::format("MOVES.L (%A4), {}", RN(rn)));
}

BOOST_DATA_TEST_CASE(MOVES_L_STORE, bdata::xrange(16), rn) {
    TEST::SET_W(0, 0007224);
    TEST::SET_W(2, rn << 12 | 0x0800);
    BOOST_TEST(disasm() == fmt::format("MOVES.L {}, (%A4)", RN(rn)));
}

BOOST_DATA_TEST_CASE(CAS_L, bdata::xrange(8) ^ bdata::random(0, 7), dc, du) {
    TEST::SET_W(0, 0007320 | 2);
    TEST::SET_W(2, du << 6 | dc);
    BOOST_TEST(disasm() == fmt::format("CAS.L %D{}, %D{}, (%A2)", dc, du));
}

BOOST_DATA_TEST_CASE(CAS2_L,
                     bdata::xrange(8) ^ bdata::random(0, 7) ^
                         bdata::random(0, 15) ^ bdata::random(0, 7) ^
                         bdata::random(0, 7) ^ bdata::random(0, 15),
                     dc1, du1, rn1, dc2, du2, rn2) {
    TEST::SET_W(0, 0007374);
    TEST::SET_W(2, rn1 << 12 | du1 << 6 | dc1);
    TEST::SET_W(4, rn2 << 12 | du2 << 6 | dc2);
    BOOST_TEST(disasm() ==
               fmt::format("CAS2.L %D{}:%D{}, %D{}:%D{}, ({}:{})", dc1,
                           dc2, du1, du2, RN(rn1), RN(rn2)));
}

BOOST_DATA_TEST_CASE(BTST_L_REG, bdata::xrange(8), r) {
    TEST::SET_W(0, 0000405 | r << 9);
    BOOST_TEST(disasm() == fmt::format("BTST.L %D{}, %D5", r));
}

BOOST_DATA_TEST_CASE(MOVEP_W_LOAD, bdata::xrange(8) ^ bdata::random(0, 7), x, y) {
    auto p = 50;
    TEST::SET_W(0, 0000410 | x << 9 | y);
    TEST::SET_W(2, p);
    BOOST_TEST(disasm() == fmt::format("MOVEP.W (#{}, %A{}), %D{}", p, y, x));
}

BOOST_DATA_TEST_CASE(BTST_B_REG, bdata::xrange(8), r) {
    TEST::SET_W(0, 0000425 | r << 9);
    BOOST_TEST(disasm() == fmt::format("BTST.B %D{}, (%A5)", r));
}




BOOST_DATA_TEST_CASE(BCHG_L_REG, bdata::xrange(8), r) {
    TEST::SET_W(0, 0000505 | r << 9);
    BOOST_TEST(disasm() == fmt::format("BCHG.L %D{}, %D5", r));
}


BOOST_DATA_TEST_CASE(MOVEP_L_LOAD, bdata::xrange(8) ^ bdata::random(0, 7), x, y) {
    auto p = 50;
    TEST::SET_W(0, 0000510 | x << 9 | y);
    TEST::SET_W(2, p);
    BOOST_TEST(disasm() == fmt::format("MOVEP.L (#{}, %A{}), %D{}", p, y, x));
}

BOOST_DATA_TEST_CASE(BCHG_B_REG, bdata::xrange(8), r) {
    TEST::SET_W(0, 0000525 | r << 9);
    BOOST_TEST(disasm() == fmt::format("BCHG.B %D{}, (%A5)", r));
}



BOOST_DATA_TEST_CASE(BCLR_L_REG, bdata::xrange(8), r) {
    TEST::SET_W(0, 0000605 | r << 9);
    BOOST_TEST(disasm() == fmt::format("BCLR.L %D{}, %D5", r));
}

BOOST_DATA_TEST_CASE(MOVEP_W_STORE, bdata::xrange(8) ^ bdata::random(0, 7), x, y) {
    auto p = 50;
    TEST::SET_W(0, 0000610 | x << 9 | y);
    TEST::SET_W(2, p);
    BOOST_TEST(disasm() == fmt::format("MOVEP.W %D{}, (#{}, %A{})", x, p, y));
}

BOOST_DATA_TEST_CASE(BCLR_B_REG, bdata::xrange(8), r) {
    TEST::SET_W(0, 0000625 | r << 9);
    BOOST_TEST(disasm() == fmt::format("BCLR.B %D{}, (%A5)", r));
}


BOOST_DATA_TEST_CASE(BSET_L_REG, bdata::xrange(8), r) {
    TEST::SET_W(0, 0000705 | r << 9);
    BOOST_TEST(disasm() == fmt::format("BSET.L %D{}, %D5", r));
}

BOOST_DATA_TEST_CASE(MOVEP_L_STORE, bdata::xrange(8) ^ bdata::random(0, 7), x, y) {
    auto p = 50;
    TEST::SET_W(0, 0000710 | x << 9 | y);
    TEST::SET_W(2, p);
    BOOST_TEST(disasm() == fmt::format("MOVEP.L %D{}, (#{}, %A{})", x, p, y));
}

BOOST_DATA_TEST_CASE(BSET_B_REG, bdata::xrange(8), r) {
    TEST::SET_W(0, 0000725 | r << 9);
    BOOST_TEST(disasm() == fmt::format("BSET.B %D{}, (%A5)", r));
}


BOOST_AUTO_TEST_SUITE_END()