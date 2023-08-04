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
namespace DISASM {
extern std::unordered_map<int, const char *> CRMAP;
extern const char *cc_n[16];
}
std::vector<int> cr_list = {
    0, 1, 2, 3, 4 ,5 ,6, 7, 0x800, 0x801, 0x803, 0x804, 0x805, 0x806, 0x807
};
BOOST_FIXTURE_TEST_SUITE(Disasm, PrepareDisasm)

BOOST_AUTO_TEST_CASE(NEGX_B) {
    TEST::SET_W(0, 0040003);
    BOOST_TEST(disasm() == "NEGX.B %D3");
}

BOOST_AUTO_TEST_CASE(NEGX_W) {
    TEST::SET_W(0, 0040103);
    BOOST_TEST(disasm() == "NEGX.W %D3");
}

BOOST_AUTO_TEST_CASE(NEGX_L) {
    TEST::SET_W(0, 0040203);
    BOOST_TEST(disasm() == "NEGX.L %D3");
}

BOOST_AUTO_TEST_CASE(MOVE_FROM_SR) {
    TEST::SET_W(0, 0040303);
    BOOST_TEST(disasm() == "MOVE.W %SR, %D3");
}

BOOST_AUTO_TEST_CASE(CLR_B) {
    TEST::SET_W(0, 0041003);
    BOOST_TEST(disasm() == "CLR.B %D3");
}

BOOST_AUTO_TEST_CASE(CLR_W) {
    TEST::SET_W(0, 0041103);
    BOOST_TEST(disasm() == "CLR.W %D3");
}

BOOST_AUTO_TEST_CASE(CLR_L) {
    TEST::SET_W(0, 0041203);
    BOOST_TEST(disasm() == "CLR.L %D3");
}

BOOST_AUTO_TEST_CASE(MOVE_FROM_CCR) {
    TEST::SET_W(0, 0041303);
    BOOST_TEST(disasm() == "MOVE.W %CCR, %D3");
}

BOOST_AUTO_TEST_CASE(NEG_B) {
    TEST::SET_W(0, 0042003);
    BOOST_TEST(disasm() == "NEG.B %D3");
}

BOOST_AUTO_TEST_CASE(NEG_W) {
    TEST::SET_W(0, 0042103);
    BOOST_TEST(disasm() == "NEG.W %D3");
}

BOOST_AUTO_TEST_CASE(NEG_L) {
    TEST::SET_W(0, 0042203);
    BOOST_TEST(disasm() == "NEG.L %D3");
}

BOOST_AUTO_TEST_CASE(MOVE_TO_CCR) {
    TEST::SET_W(0, 0042303);
    BOOST_TEST(disasm() == "MOVE.W %D3, %CCR");
}

BOOST_AUTO_TEST_CASE(NOT_B) {
    TEST::SET_W(0, 0043003);
    BOOST_TEST(disasm() == "NOT.B %D3");
}

BOOST_AUTO_TEST_CASE(NOT_W) {
    TEST::SET_W(0, 0043103);
    BOOST_TEST(disasm() == "NOT.W %D3");
}

BOOST_AUTO_TEST_CASE(NOT_L) {
    TEST::SET_W(0, 0043203);
    BOOST_TEST(disasm() == "NOT.L %D3");
}

BOOST_AUTO_TEST_CASE(MOVE_TO_SR) {
    TEST::SET_W(0, 0043303);
    BOOST_TEST(disasm() == "MOVE.W %D3, %SR");
}

BOOST_AUTO_TEST_CASE(NBCD) {
    TEST::SET_W(0, 0044003);
    BOOST_TEST(disasm() == "NBCD %D3");
}

BOOST_DATA_TEST_CASE(LINK_L, bdata::xrange(8), an) {
    auto d = 2000;
    TEST::SET_W(0, 0044010 | an);
    TEST::SET_L(2, d);
    BOOST_TEST(disasm() == fmt::format("LINK.L %A{}, #{}", an, d));
}

BOOST_DATA_TEST_CASE(SWAP, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0044100 | dn);
    BOOST_TEST(disasm() == fmt::format("SWAP %D{}", dn));
}

BOOST_AUTO_TEST_CASE(BKPT) {
    TEST::SET_W(0, 0044112);
    BOOST_TEST(disasm() == "BKPT");
}

BOOST_AUTO_TEST_CASE(PEA) {
    TEST::SET_W(0, 0044122);
    BOOST_TEST(disasm() == "PEA (%A2)");
}

BOOST_DATA_TEST_CASE(EXT_W, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0044200 | dn);
    BOOST_TEST(disasm() == fmt::format("EXT.W %D{}", dn));
}

BOOST_AUTO_TEST_CASE(MOVEM_W_STORE) {
    TEST::SET_W(0, 044222);
    TEST::SET_W(2, 0b10010111'10010111);
    BOOST_TEST(disasm() == "MOVEM.W %D0-%D2/%D4/%D7-%A2/%A4/%A7, (%A2)");
}

BOOST_AUTO_TEST_CASE(MOVEM_W_STORE_DECR) {
    TEST::SET_W(0, 044242);
    TEST::SET_W(2, 0b10010111'10010111);
    BOOST_TEST(disasm() == "MOVEM.W %A7-%A5/%A3/%A0-%D5/%D3/%D0, -(%A2)");
}

BOOST_DATA_TEST_CASE(EXT_L, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0044300 | dn);
    BOOST_TEST(disasm() == fmt::format("EXT.L %D{}", dn));
}

BOOST_AUTO_TEST_CASE(MOVEM_L_STORE) {
    TEST::SET_W(0, 044322);
    TEST::SET_W(2, 0b10010111'10010111);
    BOOST_TEST(disasm() == "MOVEM.L %D0-%D2/%D4/%D7-%A2/%A4/%A7, (%A2)");
}

BOOST_AUTO_TEST_CASE(MOVEM_L_STORE_DECR) {
    TEST::SET_W(0, 044342);
    TEST::SET_W(2, 0b10010111'10010111);
    BOOST_TEST(disasm() == "MOVEM.L %A7-%A5/%A3/%A0-%D5/%D3/%D0, -(%A2)");
}

BOOST_AUTO_TEST_CASE(TST_B) {
    TEST::SET_W(0, 0045003);
    BOOST_TEST(disasm() == "TST.B %D3");
}

BOOST_AUTO_TEST_CASE(TST_W) {
    TEST::SET_W(0, 0045103);
    BOOST_TEST(disasm() == "TST.W %D3");
}

BOOST_AUTO_TEST_CASE(TST_L) {
    TEST::SET_W(0, 0045203);
    BOOST_TEST(disasm() == "TST.L %D3");
}

BOOST_AUTO_TEST_CASE(TAS) {
    TEST::SET_W(0, 0045303);
    BOOST_TEST(disasm() == "TAS %D3");
}

BOOST_AUTO_TEST_CASE(ILLEGAL) {
    TEST::SET_W(0, 0045374);
    BOOST_TEST(disasm() == "ILLEGAL");
}

BOOST_DATA_TEST_CASE(MULU_L, bdata::xrange(8), dl) {
    TEST::SET_W(0, 0046003);
    TEST::SET_W(2, dl << 12);
    BOOST_TEST(disasm() == fmt::format("MULU.L %D3, %D{}", dl));
}

BOOST_DATA_TEST_CASE(MULU_LL, bdata::xrange(8) ^ bdata::random(0, 7), dl, dh) {
    TEST::SET_W(0, 0046003);
    TEST::SET_W(2, 1 << 10 | dl << 12 | dh);
    BOOST_TEST(disasm() == fmt::format("MULU.L %D3, %D{}-%D{}", dh, dl));
}

BOOST_DATA_TEST_CASE(MULS_L, bdata::xrange(8), dl) {
    TEST::SET_W(0, 0046003);
    TEST::SET_W(2, 1 << 11 | dl << 12);
    BOOST_TEST(disasm() == fmt::format("MULS.L %D3, %D{}", dl));
}

BOOST_DATA_TEST_CASE(MULS_LL, bdata::xrange(8) ^ bdata::random(0, 7), dl, dh) {
    TEST::SET_W(0, 0046003);
    TEST::SET_W(2, 1 << 10 | 1 << 11 | dl << 12 | dh);
    BOOST_TEST(disasm() == fmt::format("MULS.L %D3, %D{}-%D{}", dh, dl));
}

BOOST_DATA_TEST_CASE(DIVU_L_NOMOD, bdata::xrange(8), dq) {
    TEST::SET_W(0, 0046103);
    TEST::SET_W(2, dq << 12 | dq);
    BOOST_TEST(disasm() == fmt::format("DIVU.L %D3, %D{}", dq));
}
BOOST_DATA_TEST_CASE(DIVUL, bdata::xrange(8) ^ bdata::random(0, 7), dq, dr_) {
    auto dr = dq == dr_ ? (dq+1) & 7 : dr_;
    TEST::SET_W(0, 0046103);
    TEST::SET_W(2,  dq << 12| dr);
    BOOST_TEST(disasm() == fmt::format("DIVUL.L %D3, %D{}:%D{}", dr, dq));
}

BOOST_DATA_TEST_CASE(DIVU_L, bdata::xrange(8) ^ bdata::random(0, 7), dq, dr_) {
    auto dr = dq == dr_ ? (dq+1) & 7 : dr_;
    TEST::SET_W(0, 0046103);
    TEST::SET_W(2,  1 << 10 | dq << 12| dr);
    BOOST_TEST(disasm() == fmt::format("DIVU.L %D3, %D{}:%D{}", dr, dq));
}

BOOST_DATA_TEST_CASE(DIVS_L_NOMOD, bdata::xrange(8), dq) {
    TEST::SET_W(0, 0046103);
    TEST::SET_W(2, 1 << 11| dq << 12 | dq);
    BOOST_TEST(disasm() == fmt::format("DIVS.L %D3, %D{}", dq));
}
BOOST_DATA_TEST_CASE(DIVSL, bdata::xrange(8) ^ bdata::random(0, 7), dq, dr_) {
    auto dr = dq == dr_ ? (dq+1) & 7 : dr_;
    TEST::SET_W(0, 0046103);
    TEST::SET_W(2,  1 << 11| dq << 12| dr);
    BOOST_TEST(disasm() == fmt::format("DIVSL.L %D3, %D{}:%D{}", dr, dq));
}

BOOST_DATA_TEST_CASE(DIVS_L, bdata::xrange(8) ^ bdata::random(0, 7), dq, dr_) {
    auto dr = dq == dr_ ? (dq+1) & 7 : dr_;
    TEST::SET_W(0, 0046103);
    TEST::SET_W(2,  1 << 11 | 1 << 10 | dq << 12| dr);
    BOOST_TEST(disasm() == fmt::format("DIVS.L %D3, %D{}:%D{}", dr, dq));
}


BOOST_AUTO_TEST_CASE(MOVEM_W_LOAD) {
    TEST::SET_W(0, 0046223);
    TEST::SET_W(2, 0b10010111'10010111);
    BOOST_TEST(disasm() == "MOVEM.W (%A3), %D0-%D2/%D4/%D7-%A2/%A4/%A7");
}

BOOST_AUTO_TEST_CASE(MOVEM_L_LOAD) {
    TEST::SET_W(0, 0046323);
    TEST::SET_W(2, 0b10010111'10010111);
    BOOST_TEST(disasm() == "MOVEM.L (%A3), %D0-%D2/%D4/%D7-%A2/%A4/%A7");
}

BOOST_DATA_TEST_CASE(TRAP, bdata::xrange(16), v) {
    TEST::SET_W(0, 0047100 | v);
    BOOST_TEST(disasm() == fmt::format("TRAP #{}", v));
}

BOOST_DATA_TEST_CASE(LINK, bdata::xrange(8), an) {
    auto d = 2000;
    TEST::SET_W(0, 0047120 | an);
    TEST::SET_W(2, d);
    BOOST_TEST(disasm() == fmt::format("LINK.W %A{}, #{}", an, d));
}

BOOST_DATA_TEST_CASE(UNLK, bdata::xrange(8), an) {
    TEST::SET_W(0, 0047130 | an);
    BOOST_TEST(disasm() == fmt::format("UNLK %A{}", an));
}

BOOST_DATA_TEST_CASE(MOVE_TO_USP, bdata::xrange(8), an) {
    TEST::SET_W(0, 0047140 | an);
    BOOST_TEST(disasm() == fmt::format("MOVE %A{}, %USP", an));
}

BOOST_DATA_TEST_CASE(MOVE_FROM_USP, bdata::xrange(8), an) {
    TEST::SET_W(0, 0047150 | an);
    BOOST_TEST(disasm() == fmt::format("MOVE %USP, %A{}", an));
}

BOOST_AUTO_TEST_CASE(RESET) {
    TEST::SET_W(0, 0047160);
    BOOST_TEST(disasm() == "RESET");
}

BOOST_AUTO_TEST_CASE(NOP) {
    TEST::SET_W(0, 0047161);
    BOOST_TEST(disasm() == "NOP");
}

BOOST_AUTO_TEST_CASE(STOP) {
    TEST::SET_W(0, 0047162);
    TEST::SET_W(2, 0xf);
    BOOST_TEST(disasm() == "STOP #000f");
}

BOOST_AUTO_TEST_CASE(RTE) {
    TEST::SET_W(0, 0047163);
    BOOST_TEST(disasm() == "RTE");
}

BOOST_AUTO_TEST_CASE(RTD) {
    TEST::SET_W(0, 0047164);
    TEST::SET_W(2, 100);
    BOOST_TEST(disasm() == "RTD #100");
}

BOOST_AUTO_TEST_CASE(RTS) {
    TEST::SET_W(0, 0047165);
    BOOST_TEST(disasm() == "RTS");
}

BOOST_AUTO_TEST_CASE(TRAPV) {
    TEST::SET_W(0, 0047166);
    BOOST_TEST(disasm() == "TRAPV");
}

BOOST_AUTO_TEST_CASE(RTR) {
    TEST::SET_W(0, 0047167);
    BOOST_TEST(disasm() == "RTR");
}

BOOST_DATA_TEST_CASE(MOVEC_LOAD, cr_list * bdata::xrange(16), cr, rn) {
    TEST::SET_W(0, 0047172);
    TEST::SET_W(2, cr | rn << 12);
    BOOST_TEST(disasm() == fmt::format("MOVEC %{}, {}", DISASM::CRMAP[cr], RN(rn)));
}

BOOST_DATA_TEST_CASE(MOVEC_LOAD_STORE, cr_list* bdata::xrange(16), cr, rn) {
    TEST::SET_W(0, 0047173);
    TEST::SET_W(2, cr | rn << 12);
    BOOST_TEST(disasm() == fmt::format("MOVEC {}, %{}", RN(rn), DISASM::CRMAP[cr]));
}

BOOST_AUTO_TEST_CASE(JSR) {
    TEST::SET_W(0, 0047222);
    BOOST_TEST(disasm() == "JSR (%A2)");
}

BOOST_AUTO_TEST_CASE(JMP) {
    TEST::SET_W(0, 0047322);
    BOOST_TEST(disasm() == "JMP (%A2)");
}

BOOST_DATA_TEST_CASE(CHK_L, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0040403 | dn<< 9);
    BOOST_TEST(disasm() == fmt::format("CHK.L %D3, %D{}", dn));
}

BOOST_DATA_TEST_CASE(CHK_W, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0040603 | dn<< 9);
    BOOST_TEST(disasm() == fmt::format("CHK.W %D3, %D{}", dn));
}

BOOST_DATA_TEST_CASE(EXTB_L, bdata::xrange(8), dn) {
    TEST::SET_W(0, 0044700 | dn);
    BOOST_TEST(disasm() == fmt::format("EXTB.L %D{}", dn));
}

BOOST_DATA_TEST_CASE(LEA, bdata::xrange(8), an) {
    TEST::SET_W(0, 0040722 | an << 9);
    BOOST_TEST(disasm() == fmt::format("LEA (%A2), %A{}", an));
}


BOOST_AUTO_TEST_SUITE_END()