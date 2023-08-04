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

BOOST_DATA_TEST_CASE(OR_B_TO_REG, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0100004 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("OR.B %D4, %D{}", dn));
}

BOOST_DATA_TEST_CASE(OR_W_TO_REG, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0100104 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("OR.W %D4, %D{}", dn));
}

BOOST_DATA_TEST_CASE(OR_L_TO_REG, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0100204 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("OR.L %D4, %D{}", dn));
}

BOOST_DATA_TEST_CASE(DIVU_W, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0100304 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("DIVU.W %D4, %D{}", dn));
}

BOOST_DATA_TEST_CASE(SBCD_D, bdata::xrange(8) ^ bdata::random(0, 7), dx, dy) {
    TEST::SET_W(0, 0100400 | dx << 9 | dy);
    BOOST_TEST(disasm() == fmt::format("SBCD %D{}, %D{}", dx, dy));
}

BOOST_DATA_TEST_CASE(SBCD_M, bdata::xrange(8) ^ bdata::random(0, 7), dx, dy) {
    TEST::SET_W(0, 0100410 | dx << 9 | dy);
    BOOST_TEST(disasm() == fmt::format("SBCD -(%A{}), -(%A{})", dx, dy));
}

BOOST_DATA_TEST_CASE(OR_B_TO_MEM, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0100424 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("OR.B %D{}, (%A4)", dn));
}

BOOST_DATA_TEST_CASE(PACK_D, bdata::xrange(8) ^ bdata::random(0, 7), dx, dy) {
    auto d = 100;
    TEST::SET_W(0, 0100500 | dx | dy << 9);
    TEST::SET_W(2, d);
    BOOST_TEST(disasm() == fmt::format("PACK %D{}, %D{}, #{}", dx, dy, d));
}

BOOST_DATA_TEST_CASE(PACK_M, bdata::xrange(8) ^ bdata::random(0, 7), dx, dy) {
    auto d = 100;
    TEST::SET_W(0, 0100510 | dx | dy << 9);
    TEST::SET_W(2, d);
    BOOST_TEST(disasm() == fmt::format("PACK -(%A{}), -(%A{}), #{}", dx, dy, d));
}

BOOST_DATA_TEST_CASE(OR_W_TO_MEM, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0100524 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("OR.W %D{}, (%A4)", dn));
}

BOOST_DATA_TEST_CASE(UNPK_D, bdata::xrange(8) ^ bdata::random(0, 7), dx, dy) {
    auto d = 100;
    TEST::SET_W(0, 0100600 | dx | dy << 9);
    TEST::SET_W(2, d);
    BOOST_TEST(disasm() == fmt::format("UNPK %D{}, %D{}, #{}", dx, dy, d));
}

BOOST_DATA_TEST_CASE(UNPK_M, bdata::xrange(8) ^ bdata::random(0, 7), dx, dy) {
    auto d = 100;
    TEST::SET_W(0, 0100610 | dx | dy << 9);
    TEST::SET_W(2, d);
    BOOST_TEST(disasm() == fmt::format("UNPK -(%A{}), -(%A{}), #{}", dx, dy, d));
}

BOOST_DATA_TEST_CASE(OR_L_TO_MEM, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0100624 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("OR.L %D{}, (%A4)", dn));
}

BOOST_DATA_TEST_CASE(DIVS_W, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0100704 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("DIVS.W %D4, %D{}", dn));
}

BOOST_DATA_TEST_CASE(SUB_B_TO_REG, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0110004 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("SUB.B %D4, %D{}", dn));
}

BOOST_DATA_TEST_CASE(SUB_W_TO_REG, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0110104 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("SUB.W %D4, %D{}", dn));
}

BOOST_DATA_TEST_CASE(SUB_L_TO_REG, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0110204 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("SUB.L %D4, %D{}", dn));
}

BOOST_DATA_TEST_CASE(SUBA_W, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0110304 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("SUBA.W %D4, %A{}", dn));
}

BOOST_DATA_TEST_CASE(SUBX_B_D, bdata::xrange(8) ^ bdata::random(0, 7), dx, dy) {
    TEST::SET_W(0, 0110400 | dx << 9 | dy);
    BOOST_TEST(disasm() == fmt::format("SUBX.B %D{}, %D{}", dx, dy));
}

BOOST_DATA_TEST_CASE(SUBX_B_M, bdata::xrange(8) ^ bdata::random(0, 7), dx, dy) {
    TEST::SET_W(0, 0110410 | dx << 9 | dy);
    BOOST_TEST(disasm() == fmt::format("SUBX.B -(%A{}), -(%A{})", dx, dy));
}

BOOST_DATA_TEST_CASE(SUB_B_TO_MEM, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0110424 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("SUB.B %D{}, (%A4)", dn));
}

BOOST_DATA_TEST_CASE(SUBX_W_D, bdata::xrange(8) ^ bdata::random(0, 7), dx, dy) {
    TEST::SET_W(0, 0110500 | dx << 9 | dy);
    BOOST_TEST(disasm() == fmt::format("SUBX.W %D{}, %D{}", dx, dy));
}

BOOST_DATA_TEST_CASE(SUBX_W_M, bdata::xrange(8) ^ bdata::random(0, 7), dx, dy) {
    TEST::SET_W(0, 0110510 | dx << 9 | dy);
    BOOST_TEST(disasm() == fmt::format("SUBX.W -(%A{}), -(%A{})", dx, dy));
}

BOOST_DATA_TEST_CASE(SUB_W_TO_MEM, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0110524 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("SUB.W %D{}, (%A4)", dn));
}

BOOST_DATA_TEST_CASE(SUBX_L_D, bdata::xrange(8) ^ bdata::random(0, 7), dx, dy) {
    TEST::SET_W(0, 0110600 | dx << 9 | dy);
    BOOST_TEST(disasm() == fmt::format("SUBX.L %D{}, %D{}", dx, dy));
}

BOOST_DATA_TEST_CASE(SUBX_L_M, bdata::xrange(8) ^ bdata::random(0, 7), dx, dy) {
    TEST::SET_W(0, 0110610 | dx << 9 | dy);
    BOOST_TEST(disasm() == fmt::format("SUBX.L -(%A{}), -(%A{})", dx, dy));
}

BOOST_DATA_TEST_CASE(SUB_L_TO_MEM, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0110624 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("SUB.L %D{}, (%A4)", dn));
}

BOOST_DATA_TEST_CASE(SUBA_L, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0110704 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("SUBA.L %D4, %A{}", dn));
}

BOOST_AUTO_TEST_CASE(SYSCALL) {
    TEST::SET_W(0, 0xA123);
    BOOST_TEST(disasm() == "SYSCALL #0xa123");
}

BOOST_DATA_TEST_CASE(CMP_B, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0130004 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("CMP.B %D4, %D{}", dn));
}

BOOST_DATA_TEST_CASE(CMP_W, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0130104 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("CMP.W %D4, %D{}", dn));
}

BOOST_DATA_TEST_CASE(CMP_L, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0130204 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("CMP.L %D4, %D{}", dn));
}

BOOST_DATA_TEST_CASE(CMPA_W, bdata::xrange(8), an) {
    TEST::SET_W(0, 0130304 | an << 9);
    BOOST_TEST(disasm() == fmt::format("CMPA.W %D4, %A{}", an));
}

BOOST_DATA_TEST_CASE(EOR_B, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0130404 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("EOR.B %D{}, %D4", dn));
}

BOOST_DATA_TEST_CASE(CMPM_B, bdata::xrange(8) ^ bdata::random(0, 7), ax, ay) {
    TEST::SET_W(0, 0130410 | ax << 9 | ay);
    BOOST_TEST(disasm() == fmt::format("CMPM.B (%A{})+, (%A{})+", ay, ax));
}

BOOST_DATA_TEST_CASE(EOR_W, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0130504 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("EOR.W %D{}, %D4", dn));
}

BOOST_DATA_TEST_CASE(CMPM_W, bdata::xrange(8) ^ bdata::random(0, 7), ax, ay) {
    TEST::SET_W(0, 0130510 | ax << 9 | ay);
    BOOST_TEST(disasm() == fmt::format("CMPM.W (%A{})+, (%A{})+", ay, ax));
}

BOOST_DATA_TEST_CASE(EOR_L, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0130604 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("EOR.L %D{}, %D4", dn));
}

BOOST_DATA_TEST_CASE(CMPM_L, bdata::xrange(8) ^ bdata::random(0, 7), ax, ay) {
    TEST::SET_W(0, 0130610 | ax << 9 | ay);
    BOOST_TEST(disasm() == fmt::format("CMPM.L (%A{})+, (%A{})+", ay, ax));
}

BOOST_DATA_TEST_CASE(CMPA_L, bdata::xrange(8), an) {
    TEST::SET_W(0, 0130704 | an << 9);
    BOOST_TEST(disasm() == fmt::format("CMPA.L %D4, %A{}", an));
}

BOOST_DATA_TEST_CASE(AND_B_TO_REG, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0140004 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("AND.B %D4, %D{}", dn));
}

BOOST_DATA_TEST_CASE(AND_W_TO_REG, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0140104 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("AND.W %D4, %D{}", dn));
}

BOOST_DATA_TEST_CASE(AND_L_TO_REG, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0140204 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("AND.L %D4, %D{}", dn));
}

BOOST_DATA_TEST_CASE(MULU_W, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0140303 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("MULU.W %D3, %D{}", dn));
}

BOOST_DATA_TEST_CASE(ABCD_D, bdata::xrange(8) ^ bdata::random(0, 7), dx, dy) {
    TEST::SET_W(0, 0140400 | dx << 9 | dy);
    BOOST_TEST(disasm() == fmt::format("ABCD %D{}, %D{}", dy, dx));
}

BOOST_DATA_TEST_CASE(ABCD_M, bdata::xrange(8) ^ bdata::random(0, 7), dx, dy) {
    TEST::SET_W(0, 0140410 | dx << 9 | dy);
    BOOST_TEST(disasm() == fmt::format("ABCD -(%A{}), -(%A{})", dy, dx));
}

BOOST_DATA_TEST_CASE(AND_B_TO_MEM, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0140424 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("AND.B %D{}, (%A4)", dn));
}

BOOST_DATA_TEST_CASE(EXG_DD, bdata::xrange(8) ^ bdata::random(0, 7), dx, dy) {
    TEST::SET_W(0, 0140500 | dx << 9 | dy);
    BOOST_TEST(disasm() == fmt::format("EXG %D{}, %D{}", dx, dy));
}

BOOST_DATA_TEST_CASE(EXG_AA, bdata::xrange(8) ^ bdata::random(0, 7), ax, ay) {
    TEST::SET_W(0, 0140510 | ax << 9 | ay);
    BOOST_TEST(disasm() == fmt::format("EXG %A{}, %A{}", ax, ay));
}

BOOST_DATA_TEST_CASE(AND_W_TO_MEM, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0140524 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("AND.W %D{}, (%A4)", dn));
}

BOOST_DATA_TEST_CASE(EXG_DA, bdata::xrange(8) ^ bdata::random(0, 7), dx, ay) {
    TEST::SET_W(0, 0140610 | dx << 9 | ay);
    BOOST_TEST(disasm() == fmt::format("EXG %D{}, %A{}", dx, ay));
}

BOOST_DATA_TEST_CASE(AND_L_TO_MEM, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0140624 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("AND.L %D{}, (%A4)", dn));
}

BOOST_DATA_TEST_CASE(MULS_W, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0140703 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("MULS.W %D3, %D{}", dn));
}


BOOST_DATA_TEST_CASE(ADD_B_TO_REG, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0150004 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("ADD.B %D4, %D{}", dn));
}

BOOST_DATA_TEST_CASE(ADD_W_TO_REG, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0150104 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("ADD.W %D4, %D{}", dn));
}

BOOST_DATA_TEST_CASE(ADD_L_TO_REG, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0150204 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("ADD.L %D4, %D{}", dn));
}

BOOST_DATA_TEST_CASE(ADDA_W, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0150304 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("ADDA.W %D4, %A{}", dn));
}

BOOST_DATA_TEST_CASE(ADDX_B_D, bdata::xrange(8) ^ bdata::random(0, 7), dx, dy) {
    TEST::SET_W(0, 0150400 | dx << 9 | dy);
    BOOST_TEST(disasm() == fmt::format("ADDX.B %D{}, %D{}", dx, dy));
}

BOOST_DATA_TEST_CASE(ADDX_B_M, bdata::xrange(8) ^ bdata::random(0, 7), dx, dy) {
    TEST::SET_W(0, 0150410 | dx << 9 | dy);
    BOOST_TEST(disasm() == fmt::format("ADDX.B -(%A{}), -(%A{})", dx, dy));
}

BOOST_DATA_TEST_CASE(ADD_B_TO_MEM, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0150424 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("ADD.B %D{}, (%A4)", dn));
}

BOOST_DATA_TEST_CASE(ADDX_W_D, bdata::xrange(8) ^ bdata::random(0, 7), dx, dy) {
    TEST::SET_W(0, 0150500 | dx << 9 | dy);
    BOOST_TEST(disasm() == fmt::format("ADDX.W %D{}, %D{}", dx, dy));
}

BOOST_DATA_TEST_CASE(ADDX_W_M, bdata::xrange(8) ^ bdata::random(0, 7), dx, dy) {
    TEST::SET_W(0, 0150510 | dx << 9 | dy);
    BOOST_TEST(disasm() == fmt::format("ADDX.W -(%A{}), -(%A{})", dx, dy));
}

BOOST_DATA_TEST_CASE(ADD_W_TO_MEM, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0150524 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("ADD.W %D{}, (%A4)", dn));
}

BOOST_DATA_TEST_CASE(ADDX_L_D, bdata::xrange(8) ^ bdata::random(0, 7), dx, dy) {
    TEST::SET_W(0, 0150600 | dx << 9 | dy);
    BOOST_TEST(disasm() == fmt::format("ADDX.L %D{}, %D{}", dx, dy));
}

BOOST_DATA_TEST_CASE(ADDX_L_M, bdata::xrange(8) ^ bdata::random(0, 7), dx, dy) {
    TEST::SET_W(0, 0150610 | dx << 9 | dy);
    BOOST_TEST(disasm() == fmt::format("ADDX.L -(%A{}), -(%A{})", dx, dy));
}

BOOST_DATA_TEST_CASE(ADD_L_TO_MEM, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0150624 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("ADD.L %D{}, (%A4)", dn));
}

BOOST_DATA_TEST_CASE(ADDA_L, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0150704 | dn << 9);
    BOOST_TEST(disasm() == fmt::format("ADDA.L %D4, %A{}", dn));
}

BOOST_AUTO_TEST_SUITE_END()