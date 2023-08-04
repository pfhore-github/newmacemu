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


BOOST_DATA_TEST_CASE(ASR_B_IMM, bdata::xrange(8) ^ bdata::random(0, 7), imm, dn) {
    TEST::SET_W(0, 0160000 | imm << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("ASR.B #{}, %D{}", imm ? imm : 8, dn));
}

BOOST_DATA_TEST_CASE(LSR_B_IMM, bdata::xrange(8) ^ bdata::random(0, 7), imm, dn) {
    TEST::SET_W(0, 0160010 | imm << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("LSR.B #{}, %D{}", imm ? imm : 8, dn));
}

BOOST_DATA_TEST_CASE(ROXR_B_IMM, bdata::xrange(8) ^ bdata::random(0, 7), imm, dn) {
    TEST::SET_W(0, 0160020 | imm << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("ROXR.B #{}, %D{}", imm ? imm : 8, dn));
}

BOOST_DATA_TEST_CASE(ROR_B_IMM, bdata::xrange(8) ^ bdata::random(0, 7), imm, dn) {
    TEST::SET_W(0, 0160030 | imm << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("ROR.B #{}, %D{}", imm ? imm : 8, dn));
}

BOOST_DATA_TEST_CASE(ASR_B_REG, bdata::xrange(8) ^ bdata::random(0, 7), dr, dn) {
    TEST::SET_W(0, 0160040 | dr << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("ASR.B %D{}, %D{}", dr, dn));
}

BOOST_DATA_TEST_CASE(LSR_B_REG, bdata::xrange(8) ^ bdata::random(0, 7), dr, dn) {
    TEST::SET_W(0, 0160050 | dr << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("LSR.B %D{}, %D{}", dr, dn));
}

BOOST_DATA_TEST_CASE(ROXR_B_REG, bdata::xrange(8) ^ bdata::random(0, 7), dr, dn) {
    TEST::SET_W(0, 0160060 | dr << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("ROXR.B %D{}, %D{}", dr, dn));
}

BOOST_DATA_TEST_CASE(ROR_B_REG, bdata::xrange(8) ^ bdata::random(0, 7), dr, dn) {
    TEST::SET_W(0, 0160070 | dr << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("ROR.B %D{}, %D{}", dr, dn));
}


BOOST_DATA_TEST_CASE(ASR_W_IMM, bdata::xrange(8) ^ bdata::random(0, 7), imm, dn) {
    TEST::SET_W(0, 0160100 | imm << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("ASR.W #{}, %D{}", imm ? imm : 8, dn));
}

BOOST_DATA_TEST_CASE(LSR_W_IMM, bdata::xrange(8) ^ bdata::random(0, 7), imm, dn) {
    TEST::SET_W(0, 0160110 | imm << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("LSR.W #{}, %D{}", imm ? imm : 8, dn));
}

BOOST_DATA_TEST_CASE(ROXR_W_IMM, bdata::xrange(8) ^ bdata::random(0, 7), imm, dn) {
    TEST::SET_W(0, 0160120 | imm << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("ROXR.W #{}, %D{}", imm ? imm : 8, dn));
}

BOOST_DATA_TEST_CASE(ROR_W_IMM, bdata::xrange(8) ^ bdata::random(0, 7), imm, dn) {
    TEST::SET_W(0, 0160130 | imm << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("ROR.W #{}, %D{}", imm ? imm : 8, dn));
}

BOOST_DATA_TEST_CASE(ASR_W_REG, bdata::xrange(8) ^ bdata::random(0, 7), dr, dn) {
    TEST::SET_W(0, 0160140 | dr << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("ASR.W %D{}, %D{}", dr, dn));
}

BOOST_DATA_TEST_CASE(LSR_W_REG, bdata::xrange(8) ^ bdata::random(0, 7), dr, dn) {
    TEST::SET_W(0, 0160150 | dr << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("LSR.W %D{}, %D{}", dr, dn));
}

BOOST_DATA_TEST_CASE(ROXR_W_REG, bdata::xrange(8) ^ bdata::random(0, 7), dr, dn) {
    TEST::SET_W(0, 0160160 | dr << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("ROXR.W %D{}, %D{}", dr, dn));
}

BOOST_DATA_TEST_CASE(ROR_W_REG, bdata::xrange(8) ^ bdata::random(0, 7), dr, dn) {
    TEST::SET_W(0, 0160170 | dr << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("ROR.W %D{}, %D{}", dr, dn));
}

BOOST_DATA_TEST_CASE(ASR_L_IMM, bdata::xrange(8) ^ bdata::random(0, 7), imm, dn) {
    TEST::SET_W(0, 0160200 | imm << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("ASR.L #{}, %D{}", imm ? imm : 8, dn));
}

BOOST_DATA_TEST_CASE(LSR_L_IMM, bdata::xrange(8) ^ bdata::random(0, 7), imm, dn) {
    TEST::SET_W(0, 0160210 | imm << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("LSR.L #{}, %D{}", imm ? imm : 8, dn));
}

BOOST_DATA_TEST_CASE(ROXR_L_IMM, bdata::xrange(8) ^ bdata::random(0, 7), imm, dn) {
    TEST::SET_W(0, 0160220 | imm << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("ROXR.L #{}, %D{}", imm ? imm : 8, dn));
}

BOOST_DATA_TEST_CASE(ROR_L_IMM, bdata::xrange(8) ^ bdata::random(0, 7), imm, dn) {
    TEST::SET_W(0, 0160230 | imm << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("ROR.L #{}, %D{}", imm ? imm : 8, dn));
}

BOOST_DATA_TEST_CASE(ASR_L_REG, bdata::xrange(8) ^ bdata::random(0, 7), dr, dn) {
    TEST::SET_W(0, 0160240 | dr << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("ASR.L %D{}, %D{}", dr, dn));
}

BOOST_DATA_TEST_CASE(LSR_L_REG, bdata::xrange(8) ^ bdata::random(0, 7), dr, dn) {
    TEST::SET_W(0, 0160250 | dr << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("LSR.L %D{}, %D{}", dr, dn));
}

BOOST_DATA_TEST_CASE(ROXR_L_REG, bdata::xrange(8) ^ bdata::random(0, 7), dr, dn) {
    TEST::SET_W(0, 0160260 | dr << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("ROXR.L %D{}, %D{}", dr, dn));
}

BOOST_DATA_TEST_CASE(ROR_L_REG, bdata::xrange(8) ^ bdata::random(0, 7), dr, dn) {
    TEST::SET_W(0, 0160270 | dr << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("ROR.L %D{}, %D{}", dr, dn));
}

BOOST_AUTO_TEST_CASE(ASR_EA) {
    TEST::SET_W(0, 0160323);
    BOOST_TEST(disasm() == "ASR.W #1, (%A3)");
}

BOOST_AUTO_TEST_CASE(LSR_EA) {
    TEST::SET_W(0, 0161323);
    BOOST_TEST(disasm() == "LSR.W #1, (%A3)");
}

BOOST_AUTO_TEST_CASE(ROXR_EA) {
    TEST::SET_W(0, 0162323);
    BOOST_TEST(disasm() == "ROXR.W #1, (%A3)");
}

BOOST_AUTO_TEST_CASE(ROR_EA) {
    TEST::SET_W(0, 0163323);
    BOOST_TEST(disasm() == "ROR.W #1, (%A3)");
}
BOOST_AUTO_TEST_SUITE(BFTST)
BOOST_DATA_TEST_CASE(BOTH_IMM, bdata::xrange(32) ^ bdata::random(1, 31), off, wid) {
    TEST::SET_W(0, 0164304);
    TEST::SET_W(2, off << 6 | wid);
    BOOST_TEST(disasm() == fmt::format("BFTST %D4{{{}:{}}}", off, wid));
}

BOOST_DATA_TEST_CASE(OFF_R, bdata::xrange(8) ^ bdata::random(1,31), off_r, wid) {
    TEST::SET_W(0, 0164304);
    TEST::SET_W(2, off_r << 6 | wid | 1<<11);
    BOOST_TEST(disasm() == fmt::format("BFTST %D4{{%D{}:{}}}", off_r, wid));
}

BOOST_DATA_TEST_CASE(WIDTH_R, bdata::xrange(32) ^ bdata::random(0,7), off, wid_r) {
    TEST::SET_W(0, 0164304);
    TEST::SET_W(2, off << 6 | wid_r | 1<<5);
    BOOST_TEST(disasm() == fmt::format("BFTST %D4{{{}:%D{}}}", off, wid_r));
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(BFCHG)
BOOST_DATA_TEST_CASE(BOTH_IMM, bdata::xrange(32) ^ bdata::random(1, 31), off, wid) {
    TEST::SET_W(0, 0165304);
    TEST::SET_W(2, off << 6 | wid);
    BOOST_TEST(disasm() == fmt::format("BFCHG %D4{{{}:{}}}", off, wid));
}

BOOST_DATA_TEST_CASE(OFF_R, bdata::xrange(8) ^ bdata::random(1,31), off_r, wid) {
    TEST::SET_W(0, 0165304);
    TEST::SET_W(2, off_r << 6 | wid | 1<<11);
    BOOST_TEST(disasm() == fmt::format("BFCHG %D4{{%D{}:{}}}", off_r, wid));
}

BOOST_DATA_TEST_CASE(WIDTH_R, bdata::xrange(32) ^ bdata::random(0,7), off, wid_r) {
    TEST::SET_W(0, 0165304);
    TEST::SET_W(2, off << 6 | wid_r | 1<<5);
    BOOST_TEST(disasm() == fmt::format("BFCHG %D4{{{}:%D{}}}", off, wid_r));
}
BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(BFCLR)
BOOST_DATA_TEST_CASE(BOTH_IMM, bdata::xrange(32) ^ bdata::random(1, 31), off, wid) {
    TEST::SET_W(0, 0166304);
    TEST::SET_W(2, off << 6 | wid);
    BOOST_TEST(disasm() == fmt::format("BFCLR %D4{{{}:{}}}", off, wid));
}

BOOST_DATA_TEST_CASE(OFF_R, bdata::xrange(8) ^ bdata::random(1,31), off_r, wid) {
    TEST::SET_W(0, 0166304);
    TEST::SET_W(2, off_r << 6 | wid | 1<<11);
    BOOST_TEST(disasm() == fmt::format("BFCLR %D4{{%D{}:{}}}", off_r, wid));
}

BOOST_DATA_TEST_CASE(WIDTH_R, bdata::xrange(32) ^ bdata::random(0,7), off, wid_r) {
    TEST::SET_W(0, 0166304);
    TEST::SET_W(2, off << 6 | wid_r | 1<<5);
    BOOST_TEST(disasm() == fmt::format("BFCLR %D4{{{}:%D{}}}", off, wid_r));
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(BFSET)
BOOST_DATA_TEST_CASE(BOTH_IMM, bdata::xrange(32) ^ bdata::random(1, 31), off, wid) {
    TEST::SET_W(0, 0167304);
    TEST::SET_W(2, off << 6 | wid);
    BOOST_TEST(disasm() == fmt::format("BFSET %D4{{{}:{}}}", off, wid));
}

BOOST_DATA_TEST_CASE(OFF_R, bdata::xrange(8) ^ bdata::random(1,31), off_r, wid) {
    TEST::SET_W(0, 0167304);
    TEST::SET_W(2, off_r << 6 | wid | 1<<11);
    BOOST_TEST(disasm() == fmt::format("BFSET %D4{{%D{}:{}}}", off_r, wid));
}

BOOST_DATA_TEST_CASE(WIDTH_R, bdata::xrange(32) ^ bdata::random(0,7), off, wid_r) {
    TEST::SET_W(0, 0167304);
    TEST::SET_W(2, off << 6 | wid_r | 1<<5);
    BOOST_TEST(disasm() == fmt::format("BFSET %D4{{{}:%D{}}}", off, wid_r));
}
BOOST_AUTO_TEST_SUITE_END()



BOOST_DATA_TEST_CASE(ASL_B_IMM, bdata::xrange(8) ^ bdata::random(0, 7), imm, dn) {
    TEST::SET_W(0, 0160400 | imm << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("ASL.B #{}, %D{}", imm ? imm : 8, dn));
}

BOOST_DATA_TEST_CASE(LSL_B_IMM, bdata::xrange(8) ^ bdata::random(0, 7), imm, dn) {
    TEST::SET_W(0, 0160410 | imm << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("LSL.B #{}, %D{}", imm ? imm : 8, dn));
}

BOOST_DATA_TEST_CASE(ROXL_B_IMM, bdata::xrange(8) ^ bdata::random(0, 7), imm, dn) {
    TEST::SET_W(0, 0160420 | imm << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("ROXL.B #{}, %D{}", imm ? imm : 8, dn));
}

BOOST_DATA_TEST_CASE(ROL_B_IMM, bdata::xrange(8) ^ bdata::random(0, 7), imm, dn) {
    TEST::SET_W(0, 0160430 | imm << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("ROL.B #{}, %D{}", imm ? imm : 8, dn));
}

BOOST_DATA_TEST_CASE(ASL_B_REG, bdata::xrange(8) ^ bdata::random(0, 7), dr, dn) {
    TEST::SET_W(0, 0160440 | dr << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("ASL.B %D{}, %D{}", dr, dn));
}

BOOST_DATA_TEST_CASE(LSL_B_REG, bdata::xrange(8) ^ bdata::random(0, 7), dr, dn) {
    TEST::SET_W(0, 0160450 | dr << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("LSL.B %D{}, %D{}", dr, dn));
}

BOOST_DATA_TEST_CASE(ROXL_B_REG, bdata::xrange(8) ^ bdata::random(0, 7), dr, dn) {
    TEST::SET_W(0, 0160460 | dr << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("ROXL.B %D{}, %D{}", dr, dn));
}

BOOST_DATA_TEST_CASE(ROL_B_REG, bdata::xrange(8) ^ bdata::random(0, 7), dr, dn) {
    TEST::SET_W(0, 0160470 | dr << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("ROL.B %D{}, %D{}", dr, dn));
}


BOOST_DATA_TEST_CASE(ASL_W_IMM, bdata::xrange(8) ^ bdata::random(0, 7), imm, dn) {
    TEST::SET_W(0, 0160500 | imm << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("ASL.W #{}, %D{}", imm ? imm : 8, dn));
}

BOOST_DATA_TEST_CASE(LSL_W_IMM, bdata::xrange(8) ^ bdata::random(0, 7), imm, dn) {
    TEST::SET_W(0, 0160510 | imm << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("LSL.W #{}, %D{}", imm ? imm : 8, dn));
}

BOOST_DATA_TEST_CASE(ROXL_W_IMM, bdata::xrange(8) ^ bdata::random(0, 7), imm, dn) {
    TEST::SET_W(0, 0160520 | imm << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("ROXL.W #{}, %D{}", imm ? imm : 8, dn));
}

BOOST_DATA_TEST_CASE(ROL_W_IMM, bdata::xrange(8) ^ bdata::random(0, 7), imm, dn) {
    TEST::SET_W(0, 0160530 | imm << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("ROL.W #{}, %D{}", imm ? imm : 8, dn));
}

BOOST_DATA_TEST_CASE(ASL_W_REG, bdata::xrange(8) ^ bdata::random(0, 7), dr, dn) {
    TEST::SET_W(0, 0160540 | dr << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("ASL.W %D{}, %D{}", dr, dn));
}

BOOST_DATA_TEST_CASE(LSL_W_REG, bdata::xrange(8) ^ bdata::random(0, 7), dr, dn) {
    TEST::SET_W(0, 0160550 | dr << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("LSL.W %D{}, %D{}", dr, dn));
}

BOOST_DATA_TEST_CASE(ROXL_W_REG, bdata::xrange(8) ^ bdata::random(0, 7), dr, dn) {
    TEST::SET_W(0, 0160560 | dr << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("ROXL.W %D{}, %D{}", dr, dn));
}

BOOST_DATA_TEST_CASE(ROL_W_REG, bdata::xrange(8) ^ bdata::random(0, 7), dr, dn) {
    TEST::SET_W(0, 0160570 | dr << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("ROL.W %D{}, %D{}", dr, dn));
}

BOOST_DATA_TEST_CASE(ASL_L_IMM, bdata::xrange(8) ^ bdata::random(0, 7), imm, dn) {
    TEST::SET_W(0, 0160600 | imm << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("ASL.L #{}, %D{}", imm ? imm : 8, dn));
}

BOOST_DATA_TEST_CASE(LSL_L_IMM, bdata::xrange(8) ^ bdata::random(0, 7), imm, dn) {
    TEST::SET_W(0, 0160610 | imm << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("LSL.L #{}, %D{}", imm ? imm : 8, dn));
}

BOOST_DATA_TEST_CASE(ROXL_L_IMM, bdata::xrange(8) ^ bdata::random(0, 7), imm, dn) {
    TEST::SET_W(0, 0160620 | imm << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("ROXL.L #{}, %D{}", imm ? imm : 8, dn));
}

BOOST_DATA_TEST_CASE(ROL_L_IMM, bdata::xrange(8) ^ bdata::random(0, 7), imm, dn) {
    TEST::SET_W(0, 0160630 | imm << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("ROL.L #{}, %D{}", imm ? imm : 8, dn));
}

BOOST_DATA_TEST_CASE(ASL_L_REG, bdata::xrange(8) ^ bdata::random(0, 7), dr, dn) {
    TEST::SET_W(0, 0160640 | dr << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("ASL.L %D{}, %D{}", dr, dn));
}

BOOST_DATA_TEST_CASE(LSL_L_REG, bdata::xrange(8) ^ bdata::random(0, 7), dr, dn) {
    TEST::SET_W(0, 0160650 | dr << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("LSL.L %D{}, %D{}", dr, dn));
}

BOOST_DATA_TEST_CASE(ROXL_L_REG, bdata::xrange(8) ^ bdata::random(0, 7), dr, dn) {
    TEST::SET_W(0, 0160660 | dr << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("ROXL.L %D{}, %D{}", dr, dn));
}

BOOST_DATA_TEST_CASE(ROL_L_REG, bdata::xrange(8) ^ bdata::random(0, 7), dr, dn) {
    TEST::SET_W(0, 0160670 | dr << 9 | dn);
    BOOST_TEST(disasm() == fmt::format("ROL.L %D{}, %D{}", dr, dn));
}

BOOST_AUTO_TEST_CASE(ASL_EA) {
    TEST::SET_W(0, 0160723);
    BOOST_TEST(disasm() == "ASL.W #1, (%A3)");
}

BOOST_AUTO_TEST_CASE(LSL_EA) {
    TEST::SET_W(0, 0161723);
    BOOST_TEST(disasm() == "LSL.W #1, (%A3)");
}

BOOST_AUTO_TEST_CASE(ROXL_EA) {
    TEST::SET_W(0, 0162723);
    BOOST_TEST(disasm() == "ROXL.W #1, (%A3)");
}

BOOST_AUTO_TEST_CASE(ROL_EA) {
    TEST::SET_W(0, 0163723);
    BOOST_TEST(disasm() == "ROL.W #1, (%A3)");
}


BOOST_AUTO_TEST_SUITE(BFEXTU)
BOOST_DATA_TEST_CASE(BOTH_IMM, bdata::xrange(32) ^ bdata::random(1, 31) ^ bdata::random(0,7), off, wid, du) {
    TEST::SET_W(0, 0164704);
    TEST::SET_W(2, du << 12 | off << 6 | wid);
    BOOST_TEST(disasm() == fmt::format("BFEXTU %D4{{{}:{}}}, %D{}", off, wid, du));
}

BOOST_DATA_TEST_CASE(OFF_R, bdata::xrange(8) ^ bdata::random(1,31) ^ bdata::random(0,7), off_r, wid, du) {
    TEST::SET_W(0, 0164704);
    TEST::SET_W(2, du << 12 | off_r << 6 | wid | 1<<11);
    BOOST_TEST(disasm() == fmt::format("BFEXTU %D4{{%D{}:{}}}, %D{}", off_r, wid, du));
}

BOOST_DATA_TEST_CASE(WIDTH_R, bdata::xrange(32) ^ bdata::random(0,7)^ bdata::random(0,7), off, wid_r, du) {
    TEST::SET_W(0, 0164704);
    TEST::SET_W(2, du << 12 | off << 6 | wid_r | 1<<5);
    BOOST_TEST(disasm() == fmt::format("BFEXTU %D4{{{}:%D{}}}, %D{}", off, wid_r, du));
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(BFEXTS)
BOOST_DATA_TEST_CASE(BOTH_IMM, bdata::xrange(32) ^ bdata::random(1, 31) ^ bdata::random(0,7), off, wid, du) {
    TEST::SET_W(0, 0165704);
    TEST::SET_W(2, du << 12 | off << 6 | wid);
    BOOST_TEST(disasm() == fmt::format("BFEXTS %D4{{{}:{}}}, %D{}", off, wid, du));
}

BOOST_DATA_TEST_CASE(OFF_R, bdata::xrange(8) ^ bdata::random(1,31) ^ bdata::random(0,7), off_r, wid, du) {
    TEST::SET_W(0, 0165704);
    TEST::SET_W(2, du << 12 | off_r << 6 | wid | 1<<11);
    BOOST_TEST(disasm() == fmt::format("BFEXTS %D4{{%D{}:{}}}, %D{}", off_r, wid, du));
}

BOOST_DATA_TEST_CASE(WIDTH_R, bdata::xrange(32) ^ bdata::random(0,7)^ bdata::random(0,7), off, wid_r, du) {
    TEST::SET_W(0, 0165704);
    TEST::SET_W(2, du << 12 | off << 6 | wid_r | 1<<5);
    BOOST_TEST(disasm() == fmt::format("BFEXTS %D4{{{}:%D{}}}, %D{}", off, wid_r, du));
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(BFFFO)
BOOST_DATA_TEST_CASE(BOTH_IMM, bdata::xrange(32) ^ bdata::random(1, 31) ^ bdata::random(0,7), off, wid, du) {
    TEST::SET_W(0, 0166704);
    TEST::SET_W(2, du << 12 | off << 6 | wid);
    BOOST_TEST(disasm() == fmt::format("BFFFO %D4{{{}:{}}}, %D{}", off, wid, du));
}

BOOST_DATA_TEST_CASE(OFF_R, bdata::xrange(8) ^ bdata::random(1,31) ^ bdata::random(0,7), off_r, wid, du) {
    TEST::SET_W(0, 0166704);
    TEST::SET_W(2, du << 12 | off_r << 6 | wid | 1<<11);
    BOOST_TEST(disasm() == fmt::format("BFFFO %D4{{%D{}:{}}}, %D{}", off_r, wid, du));
}

BOOST_DATA_TEST_CASE(WIDTH_R, bdata::xrange(32) ^ bdata::random(0,7)^ bdata::random(0,7), off, wid_r, du) {
    TEST::SET_W(0, 0166704);
    TEST::SET_W(2, du << 12 | off << 6 | wid_r | 1<<5);
    BOOST_TEST(disasm() == fmt::format("BFFFO %D4{{{}:%D{}}}, %D{}", off, wid_r, du));
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(BFINS)
BOOST_DATA_TEST_CASE(BOTH_IMM, bdata::xrange(32) ^ bdata::random(1, 31) ^ bdata::random(0,7), off, wid, du) {
    TEST::SET_W(0, 0167704);
    TEST::SET_W(2, du << 12 | off << 6 | wid);
    BOOST_TEST(disasm() == fmt::format("BFINS %D{}, %D4{{{}:{}}}", du, off, wid));
}

BOOST_DATA_TEST_CASE(OFF_R, bdata::xrange(8) ^ bdata::random(1,31) ^ bdata::random(0,7), off_r, wid, du) {
    TEST::SET_W(0, 0167704);
    TEST::SET_W(2, du << 12 | off_r << 6 | wid | 1<<11);
    BOOST_TEST(disasm() == fmt::format("BFINS %D{}, %D4{{%D{}:{}}}", du, off_r, wid));
}

BOOST_DATA_TEST_CASE(WIDTH_R, bdata::xrange(32) ^ bdata::random(0,7)^ bdata::random(0,7), off, wid_r, du) {
    TEST::SET_W(0, 0167704);
    TEST::SET_W(2, du << 12 | off << 6 | wid_r | 1<<5);
    BOOST_TEST(disasm() == fmt::format("BFINS %D{}, %D4{{{}:%D{}}}", du, off, wid_r));
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()