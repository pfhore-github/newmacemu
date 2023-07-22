#include "68040.hpp"
#include "bus.hpp"
#include "exception.hpp"
#include "memory.hpp"
#include "proto.hpp"
#include <errno.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <memory>
#include <tuple>
#include <utility>
const char *sz_array[3] = {
    "B",
    "W",
    "L",
};

struct restore_pc {
    uint32_t pc;
    ~restore_pc() { cpu.PC = pc; }
};
std::string disasm_parseExt(int reg) {

    uint16_t next = FETCH();
    int ri = next >> 12 & 15;
    bool ri_w = next & 1 << 11;
    int ri_c = next >> 9 & 3;
    std::string base = reg >= 0 ? fmt::format("%A{}", reg) : "%PC";
    std::string rn_v =
        fmt::format("%R{}{}*{}", ri, ri_w ? ".W" : "", 1 << ri_c);
    if(!(next & 1 << 8)) {
        int8_t d = next & 0xff;
        return fmt::format("({}, {}, {})", d, base, rn_v);
    }
    std::vector<std::string> v1, v2;

    switch(next >> 4 & 3) {
    case 1:
        break;
    case 2:
        v1.push_back(fmt::format("{}", static_cast<int16_t>(FETCH())));
        break;
    case 3:
        v1.push_back(fmt::format("{}", static_cast<int32_t>(FETCH32())));
        break;
    }
    if(!(next & 1 << 7)) {
        v1.push_back(base);
    }
    int32_t od_v = 0;
    switch(next & 3) {
    case 0:
        if(!(next & 1 << 6)) {
            v1.push_back(rn_v);
        }
        return fmt::format("({})", fmt::join(v1, ", "));
    case 1:
        break;
    case 2:
        od_v = static_cast<int16_t>(FETCH());
        break;
    case 3:
        od_v = FETCH32();
        break;
    }
    if(!(next & 1 << 6)) {
        if(!(next & 1 << 2)) {
            v1.push_back(rn_v);
            v2.push_back(fmt::format("[{}]", fmt::join(v1, ", ")));
        } else {
            v2.push_back(fmt::format("[{}]", fmt::join(v1, ", ")));
            v2.push_back(rn_v);
        }
    }
    if(od_v) {
        v2.push_back(fmt::format("{}", od_v));
    }
    return fmt::format("({})", fmt::join(v2, ", "));
}
void load_fpX(uint64_t frac, uint16_t exp);
void load_fpP(uint32_t addr);
std::string disasm_ea(int type, int reg, int sz) {
    switch(type) {
    case 0:
        return fmt::format("%D{}", reg);
    case 1:
        return fmt::format("%A{}", reg);
    case 2:
        return fmt::format("(%A{})", reg);
    case 3:
        return fmt::format("(%A{})+", reg);
    case 4:
        return fmt::format("-(%A{})", reg);
    case 5: {
        int16_t d = FETCH();
        return fmt::format("({:d}, %A{})", d, reg);
    }
    case 6:
        return disasm_parseExt(reg);
    case 7:
        switch(reg) {
        case 0:
            return fmt::format("(0x{:x})", FETCH());
        case 1:
            return fmt::format("(0x{:x})", FETCH32());
        case 2: {
            int16_t d = FETCH();
            return fmt::format("({}, %PC)", d);
        }
        case 3:
            return disasm_parseExt(-1);
        case 4:
            switch(sz) {
            case 1:
                return fmt::format("#{}", static_cast<int8_t>(FETCH()));
            case 2:
                return fmt::format("#{}", static_cast<int16_t>(FETCH()));
            case 4:
                return fmt::format("#{}", static_cast<int32_t>(FETCH32()));
            case -4: // float
                return fmt::format("#{}", std::bit_cast<float>(FETCH32()));
            case -8: // double
            {
                uint64_t v1 = FETCH32();
                v1 = v1 << 32 | FETCH32();
                return fmt::format("#{}", std::bit_cast<double>(v1));
            }
            case -12: // EXT
            {
                uint16_t exp = FETCH32();
                uint64_t frac = FETCH32();
                frac = frac << 32 | FETCH32();
                load_fpX(frac, exp);
                char *p;
                mpfr_asprintf(&p, "%NR", cpu.fp_tmp);
                auto ss = fmt::format("#{}", p);
                mpfr_free_str(p);
                return ss;
            }
            case 13: // P
            {
                load_fpP(cpu.PC);
                cpu.PC += 12;
                char *p;
                mpfr_asprintf(&p, "%NR", cpu.fp_tmp);
                auto ss = fmt::format("#{}", p);
                mpfr_free_str(p);
                return ss;
            }
            }
        }
    }
    ILLEGAL_OP();
}

std::string disasm() {
    uint32_t pc = cpu.PC;
    restore_pc p(pc);
}
#if 0

namespace OP {
string ORI_B::disasm() {
     return fmt::format("ORI.B #0x{:02x}, {}", imm, dst->disasm());
}
string ANDI_B::disasm() {
     return fmt::format("ANDI.B #0x{:02x}, {}", imm, dst->disasm());
}
string SUBI_B::disasm() {
     return fmt::format("SUBI.B #{}, {}", imm, dst->disasm());
}
string ADDI_B::disasm() {
     return fmt::format("ADDI.B #{}, {}", imm, dst->disasm());
}
string EORI_B::disasm() {
     return fmt::format("EORI.B #0x{:02x}, {}", imm, dst->disasm());
}
string CMPI_B::disasm() {
     return fmt::format("CMPI.B #{}, {}", imm, dst->disasm());
}

string ORI_W::disasm() {
     return fmt::format("ORI.W #0x{:04x}, {}", imm, dst->disasm());
}
string ANDI_W::disasm() {
     return fmt::format("ANDI.W #0x{:04x}, {}", imm, dst->disasm());
}
string SUBI_W::disasm() {
     return fmt::format("SUBI.W #{}, {}", imm, dst->disasm());
}
string ADDI_W::disasm() {
     return fmt::format("ADDI.W #{}, {}", imm, dst->disasm());
}
string EORI_W::disasm() {
     return fmt::format("EORI.W #0x{:04x}, {}", imm, dst->disasm());
}
string CMPI_W::disasm() {
     return fmt::format("CMPI.W #{}, {}", imm, dst->disasm());
}

string ORI_L::disasm() {
     return fmt::format("ORI.L #0x{:08x}, {}", imm, dst->disasm());
}
string ANDI_L::disasm() {
     return fmt::format("ANDI.L #0x{:08x}, {}", imm, dst->disasm());
}
string SUBI_L::disasm() {
     return fmt::format("SUBI.L #{}, {}", imm, dst->disasm());
}
string ADDI_L::disasm() {
     return fmt::format("ADDI.L #{}, {}", imm, dst->disasm());
}
string EORI_L::disasm() {
     return fmt::format("EORI.L #0x{:08x}, {}", imm, dst->disasm());
}
string CMPI_L::disasm() {
     return fmt::format("CMPI.L #{}, {}", imm, dst->disasm());
}

std::string CMP2_B_U::disasm() {
     return fmt::format("CMP2.B {}, {}", ea->disasm(), rn->disasm());
}
std::string CMP2_B_S::disasm() {
     return fmt::format("CMP2.B {}, {}", ea->disasm(), rn->disasm());
}
std::string CHK2_B_U::disasm() {
     return fmt::format("CHK2.B {}, {}", ea->disasm(), rn->disasm());
}
std::string CHK2_B_S::disasm() {
     return fmt::format("CHK2.B {}, {}", ea->disasm(), rn->disasm());
}

std::string CMP2_W_U::disasm() {
     return fmt::format("CMP2.W {}, {}", ea->disasm(), rn->disasm());
}
std::string CMP2_W_S::disasm() {
     return fmt::format("CMP2.W {}, {}", ea->disasm(), rn->disasm());
}
std::string CHK2_W_U::disasm() {
     return fmt::format("CHK2.W {}, {}", ea->disasm(), rn->disasm());
}
std::string CHK2_W_S::disasm() {
     return fmt::format("CHK2.W {}, {}", ea->disasm(), rn->disasm());
}

std::string CMP2_L_U::disasm() {
     return fmt::format("CMP2.L {}, {}", ea->disasm(), rn->disasm());
}
std::string CMP2_L_S::disasm() {
     return fmt::format("CMP2.L {}, {}", ea->disasm(), rn->disasm());
}
std::string CHK2_L_U::disasm() {
     return fmt::format("CHK2.L {}, {}", ea->disasm(), rn->disasm());
}
std::string CHK2_L_S::disasm() {
     return fmt::format("CHK2.L {}, {}", ea->disasm(), rn->disasm());
}

string MOVES_B_ToMem::disasm() {
     return fmt::format("MOVES.B, {}, {}", rn->disasm(), ea->disasm());
}

string MOVES_B_FromMem::disasm() {
     return fmt::format("MOVES.B, {}, {}", ea->disasm(), rn->disasm());
}

string MOVES_W_ToMem::disasm() {
     return fmt::format("MOVES.W, {}, {}", rn->disasm(), ea->disasm());
}

string MOVES_W_FromMem::disasm() {
     return fmt::format("MOVES.W, {}, {}", ea->disasm(), rn->disasm());
}

string MOVES_L_ToMem::disasm() {
     return fmt::format("MOVES.L, {}, {}", rn->disasm(), ea->disasm());
}

string MOVES_L_FromMem::disasm() {
     return fmt::format("MOVES.L, {}, {}", ea->disasm(), rn->disasm());
}

string BTST_L_I::disasm() {
     return fmt::format("BTST.L #{}, {}", sc, dst->disasm());
}
string BTST_B_I::disasm() {
     return fmt::format("BTST.B #{}, {}", sc, dst->disasm());
}

string BCHG_L_I::disasm() {
     return fmt::format("BCHG.L #{}, {}", sc, dst->disasm());
}
string BCHG_B_I::disasm() {
     return fmt::format("BCHG.B #{}, {}", sc, dst->disasm());
}

string BCLR_L_I::disasm() {
     return fmt::format("BCLR.L #{}, {}", sc, dst->disasm());
}
string BCLR_B_I::disasm() {
     return fmt::format("BCLR.B #{}, {}", sc, dst->disasm());
}

string BSET_L_I::disasm() {
     return fmt::format("BSET.L #{}, {}", sc, dst->disasm());
}
string BSET_B_I::disasm() {
     return fmt::format("BSET.B #{}, {}", sc, dst->disasm());
}

string BTST_L_R::disasm() {
     return fmt::format("BTST.L %D{}, {}", sc_r, dst->disasm());
}
string BTST_B_R::disasm() {
     return fmt::format("BTST.B %D{}, {}", sc_r, dst->disasm());
}

string BCHG_L_R::disasm() {
     return fmt::format("BCHG.L %D{}, {}", sc_r, dst->disasm());
}
string BCHG_B_R::disasm() {
     return fmt::format("BCHG.B %D{}, {}", sc_r, dst->disasm());
}

string BCLR_L_R::disasm() {
     return fmt::format("BCLR.L %D{}, {}", sc_r, dst->disasm());
}
string BCLR_B_R::disasm() {
     return fmt::format("BCLR.B %D{}, {}", sc_r, dst->disasm());
}

string BSET_L_R::disasm() {
     return fmt::format("BSET.L %D{}, {}", sc_r, dst->disasm());
}
string BSET_B_R::disasm() {
     return fmt::format("BSET.B %D{}, {}", sc_r, dst->disasm());
}

string CAS_B::disasm() {
     return fmt::format("CAS.B %D{}, %D{}, {}", dc, du, ea->disasm());
}

string CAS_W::disasm() {
     return fmt::format("CAS.W %D{}, %D{}, {}", dc, du, ea->disasm());
}

string CAS_L::disasm() {
     return fmt::format("CAS.L %D{}, %D{}, {}", dc, du, ea->disasm());
}

string CAS2_W::disasm() {
     return fmt::format("CAS2.W %D{}:%D{}, %D{}:%D{}, %R{}:%R{}", dc[0], dc[1],
                        du[0], du[1], rn[0], rn[1]);
}

string CAS2_L::disasm() {
     return fmt::format("CAS2.L %D{}:%D{}, %D{}:%D{}, %R{}:%R{}", dc[0], dc[1],
                        du[0], du[1], rn[0], rn[1]);
}

string MOVEP_W_FromMem::disasm() {
     return fmt::format("MOVEP.W  ({}, %A{}), %D{}", disp, ay, dx);
}

string MOVEP_W_ToMem::disasm() {
     return fmt::format("MOVEP.W %D{}, ({}, %A{})", dx, disp, ay);
}

string MOVEP_L_FromMem::disasm() {
     return fmt::format("MOVEP.L  ({}, %A{}), %D{}", disp, ay, dx);
}

string MOVEP_L_ToMem::disasm() {
     return fmt::format("MOVEP.L %D{}, ({}, %A{})", dx, disp, ay);
}

string MOVE_B::disasm() {
     return fmt::format("MOVE.B {}, {}", src->disasm(), dst->disasm());
}

string MOVE_W::disasm() {
     return fmt::format("MOVE.W {}, {}", src->disasm(), dst->disasm());
}

string MOVE_L::disasm() {
     return fmt::format("MOVE.L {}, {}", src->disasm(), dst->disasm());
}

string MOVEA_W::disasm() {
     return fmt::format("MOVEA.W {}, {}", src->disasm(), dst->disasm());
}

string MOVEA_L::disasm() {
     return fmt::format("MOVEA.L {}, {}", src->disasm(), dst->disasm());
}

string NEGX_B::disasm() { return fmt::format("NEGX.B {}", ea->disasm()); }

string NEGX_W::disasm() { return fmt::format("NEGX.W {}", ea->disasm()); }
string NEGX_L::disasm() { return fmt::format("NEGX.L {}", ea->disasm()); }

string CLR_B::disasm() { return fmt::format("CLR.B {}", ea->disasm()); }

string CLR_W::disasm() { return fmt::format("CLR.W {}", ea->disasm()); }
string CLR_L::disasm() { return fmt::format("CLR.L {}", ea->disasm()); }

string NEG_B::disasm() { return fmt::format("NEG.B {}", ea->disasm()); }
string NEG_W::disasm() { return fmt::format("NEG.W {}", ea->disasm()); }
string NEG_L::disasm() { return fmt::format("NEG.L {}", ea->disasm()); }

string NOT_B::disasm() { return fmt::format("NOT.B {}", ea->disasm()); }
string NOT_W::disasm() { return fmt::format("NOT.W {}", ea->disasm()); }
string NOT_L::disasm() { return fmt::format("NOT.L {}", ea->disasm()); }

string TST_B::disasm() { return fmt::format("TST.B {}", ea->disasm()); }

string TST_W::disasm() { return fmt::format("TST.W {}", ea->disasm()); }
string TST_L::disasm() { return fmt::format("TST.L {}", ea->disasm()); }

string NBCD::disasm() { return fmt::format("NBCD {}", ea->disasm()); }

string SWAP::disasm() { return fmt::format("SWAP %D{}", dn); }
string EXT_W::disasm() { return fmt::format("EXT.W %D{}", dn); }
string EXT_L::disasm() { return fmt::format("EXT.L %D{}", dn); }
string EXTB_L::disasm() { return fmt::format("EXTB.L %D{}", dn); }

string MOVEM_W_TO_MEM_DECR::disasm() {
     std::vector<std::string> regList;
     for(int i = 15; i >= 0; --i) {
         if(regs.test(15 - i)) {
             if(i > 7) {
                 regList.push_back(fmt::format("%A{}", i & 7));
             } else {
                 regList.push_back(fmt::format("%D{}", i));
             }
         }
     }
     return fmt::format("MOVEM.W {}, -(%A{})", fmt::join(regList, "/"), reg);
}

string MOVEM_L_TO_MEM_DECR::disasm() {
     std::vector<std::string> regList;
     for(int i = 15; i >= 0; --i) {
         if(regs.test(15 - i)) {
             if(i > 7) {
                 regList.push_back(fmt::format("%A{}", i & 7));
             } else {
                 regList.push_back(fmt::format("%D{}", i));
             }
         }
     }
     return fmt::format("MOVEM.L {}, -(%A{})", fmt::join(regList, "/"), reg);
}

string MOVEM_W_FROM_MEM_INCR::disasm() {
     std::vector<std::string> regList;
     for(int i = 0; i < 16; ++i) {
         if(regs.test(i)) {
             if(i > 7) {
                 regList.push_back(fmt::format("%A{}", i & 7));
             } else {
                 regList.push_back(fmt::format("%D{}", i));
             }
         }
     }
     return fmt::format("MOVEM.W (%A{})+, {}", reg, fmt::join(regList, "/"));
}

string MOVEM_L_FROM_MEM_INCR::disasm() {
     std::vector<std::string> regList;
     for(int i = 0; i < 16; ++i) {
         if(regs.test(i)) {
             if(i > 7) {
                 regList.push_back(fmt::format("%A{}", i & 7));
             } else {
                 regList.push_back(fmt::format("%D{}", i));
             }
         }
     }
     return fmt::format("MOVEM.L (%A{})+, {}", reg, fmt::join(regList, "/"));
}
string MOVEM_W_TO_MEM_ADDR::disasm() {
     std::vector<std::string> regList;
     for(int i = 0; i < 16; ++i) {
         if(regs.test(i)) {
             if(i > 7) {
                 regList.push_back(fmt::format("%A{}", i & 7));
             } else {
                 regList.push_back(fmt::format("%D{}", i));
             }
         }
     }
     return fmt::format("MOVEM.W {}, {}", fmt::join(regList, "/"),
                        ea->disasm());
}

string MOVEM_L_TO_MEM_ADDR::disasm() {
     std::vector<std::string> regList;
     for(int i = 0; i < 16; ++i) {
         if(regs.test(i)) {
             if(i > 7) {
                 regList.push_back(fmt::format("%A{}", i & 7));
             } else {
                 regList.push_back(fmt::format("%D{}", i));
             }
         }
     }
     return fmt::format("MOVEM.L {}, {}", fmt::join(regList, "/"),
                        ea->disasm());
}

string MOVEM_W_FROM_MEM_ADDR::disasm() {
     std::vector<std::string> regList;
     for(int i = 0; i < 16; ++i) {
         if(regs.test(i)) {
             if(i > 7) {
                 regList.push_back(fmt::format("%A{}", i & 7));
             } else {
                 regList.push_back(fmt::format("%D{}", i));
             }
         }
     }
     return fmt::format("MOVEM.W {}, {}", ea->disasm(),
                        fmt::join(regList, "/"));
}

string MOVEM_L_FROM_MEM_ADDR::disasm() {
     std::vector<std::string> regList;
     for(int i = 0; i < 16; ++i) {
         if(regs.test(i)) {
             if(i > 7) {
                 regList.push_back(fmt::format("%A{}", i & 7));
             } else {
                 regList.push_back(fmt::format("%D{}", i));
             }
         }
     }
     return fmt::format("MOVEM.L {}, {}", ea->disasm(),
                        fmt::join(regList, "/"));
}

string MULU_L::disasm() {
     return fmt::format("MULU.L {}, %D{}", src->disasm(), low);
}
string MULU_LL::disasm() {
     return fmt::format("MULU.L {}, %D{}-%D{}", src->disasm(), high, low);
}

string MULS_L::disasm() {
     return fmt::format("MULS.L {}, %D{}", src->disasm(), low);
}
string MULS_LL::disasm() {
     return fmt::format("MULS.L {}, %D{}-%D{}", src->disasm(), high, low);
}

string DIVU_L::disasm() {
     if(low == high) {
         return fmt::format("DIVU.L {}, %D{}", src->disasm(), low);
     } else {
         return fmt::format("DIVU.L {}, %D{}:%D{}", src->disasm(), high, low);
     }
}
string DIVU_LL::disasm() {
     return fmt::format("DIVUL.L {}, %D{}:%D{}", src->disasm(), high, low);
}

string DIVS_L::disasm() {
     if(low == high) {
         return fmt::format("DIVS.L {}, %D{}", src->disasm(), low);
     } else {
         return fmt::format("DIVS.L {}, %D{}:%D{}", src->disasm(), high, low);
     }
}
string DIVS_LL::disasm() {
     return fmt::format("DIVSL.L {}, %D{}:%D{}", src->disasm(), high, low);
}

string BKPT::disasm() { return "BKPT"; }
string PEA::disasm() { return fmt::format("PEA {}", ea->disasm()); }

string LINK_L::disasm() { return fmt::format("LINK.L %A{}, {}", an, disp); }
string LINK_W::disasm() { return fmt::format("LINK.W %A{}, {}", an, disp); }
string TRAP::disasm() { return fmt::format("TRAP #{}", dn); }
string UNLK::disasm() { return fmt::format("UNLK %A{}", dn); }
string MoveFromUSP::disasm() { return fmt::format("MOVE.L %USP, %A{}", dn); }
string MoveToUSP::disasm() { return fmt::format("MOVE.L %A{}, %USP", dn); }
string RESET::disasm() { return "RESET"; }
string NOP::disasm() { return "NOP"; }
string STOP::disasm() { return fmt::format("STOP #{:04x}", i); }
string RTE::disasm() { return "RTE"; }
string RTD::disasm() { return fmt::format("RTD #{}", i); }
string RTS::disasm() { return "RTS"; }
string TRAPV::disasm() { return "TRAPV"; }
string RTR::disasm() { return "RTR"; }
static std::unordered_map<uint16_t, const char *> CR_MAP = {
    {0, "%SFC"},       {1, "%DFC"},     {2, "%CACR"},    {3, "%TCR"},
    {4, "%ITTR0"},     {5, "%ITTR1"},   {6, "%DTTR0"},   {7, "%DTTR1"},
    {0x800, "%USP"},   {0x801, "%VBR"}, {0x803, "%MSP"}, {0x804, "%ISP"},
    {0x805, "%MMUSR"}, {0x806, "%URP"}, {0x807, "%SRP"},
};
string MoveCFromCC::disasm() {
     return fmt::format("MOVEC {}, %R{}", CR_MAP[cc], rn);
}
string MoveCToCC::disasm() {
     return fmt::format("MOVEC %R{}, {}", rn, CR_MAP[cc]);
}

string JSR::disasm() { return fmt::format("JSR {}", ea->disasm()); }

string TAS::disasm() { return fmt::format("TAS {}", ea->disasm()); }

string JMP::disasm() { return fmt::format("JMP {}", ea->disasm()); }

string CHK_W::disasm() {
     return fmt::format("CHK.W {}, {}", ea->disasm(), rn->disasm());
}

string CHK_L::disasm() {
     return fmt::format("CHK.W {}, {}", ea->disasm(), rn->disasm());
}

string LEA::disasm() {
     return fmt::format("LEA {}, {}", ea->disasm(), rn->disasm());
}

string ADDQ_B::disasm() {
     return fmt::format("ADDQ.B #{}, {}", imm, dst->disasm());
}

string ADDQ_W::disasm() {
     return fmt::format("ADDQ.W #{}, {}", imm, dst->disasm());
}

string ADDQ_W_A::disasm() {
     return fmt::format("ADDQ.W #{}, {}", imm, dst->disasm());
}

string ADDQ_L::disasm() {
     return fmt::format("ADDQ.L #{}, {}", imm, dst->disasm());
}

string ADDQ_L_A::disasm() {
     return fmt::format("ADDQ.L #{}, {}", imm, dst->disasm());
}

string SUBQ_B::disasm() {
     return fmt::format("SUBQ.B #{}, {}", imm, dst->disasm());
}

string SUBQ_W::disasm() {
     return fmt::format("SUBQ.W #{}, {}", imm, dst->disasm());
}

string SUBQ_W_A::disasm() {
     return fmt::format("SUBQ.W #{}, {}", imm, dst->disasm());
}

string SUBQ_L::disasm() {
     return fmt::format("SUBQ.L #{}, {}", imm, dst->disasm());
}

string SUBQ_L_A::disasm() {
     return fmt::format("SUBQ.L #{}, {}", imm, dst->disasm());
}
static const char *CC[] = {"T",
                           "F",
                           "HI",
                           "LS",
                           "CC",
                           "CS"
                           "NE",
                           "EQ",
                           "VC",
                           "VS",
                           "PL",
                           "MI",
                           "GE",
                           "LT",
                           "GT"};
string DBcc::disasm() {
     return fmt::format("DB{} %D{}, {:x}", CC[cond], reg, cpu.PC + 2 + disp);
}

string TRAPcc::disasm() { return fmt::format("TRAP{}", CC[cond]); }

string Scc::disasm() {
     return fmt::format("TRAP{} {}", CC[cond], ea->disasm());
}

string BRA::disasm() { return fmt::format("BRA {:x}", cpu.PC + 2 + disp); }

string BSR::disasm() { return fmt::format("BSR {:x}", cpu.PC + 2 + disp); }

string Bcc::disasm() {
     return fmt::format("B{} {:x}", CC[cc], cpu.PC + 2 + disp);
}
string MOVEQ::disasm() { return fmt::format("MOVEQ #{}, %D{}", data, dn); }
string OR_B::disasm() {
     return fmt::format("OR.B {}, {}", ea->disasm(), rn->disasm());
}
string OR_W::disasm() {
     return fmt::format("OR.W {}, {}", ea->disasm(), rn->disasm());
}
string OR_L::disasm() {
     return fmt::format("OR.L {}, {}", ea->disasm(), rn->disasm());
}
string DIVU_W::disasm() {
     return fmt::format("DIVU.W {}, {}", ea->disasm(), rn->disasm());
}
string SBCD_D::disasm() { return fmt::format("SBCD %D{}, %D{}", rx, ry); }
string SBCD_M::disasm() { return fmt::format("SBCD -(%A{}), -(%A{})", rx, ry); }
string OR_B_ToMem::disasm() {
     return fmt::format("OR.B {}, {}", rn->disasm(), ea->disasm());
}
string PACK_D::disasm() {
     return fmt::format("PACK %D{}, %D{}, #{}", rx, ry, adj);
}
string PACK_M::disasm() {
     return fmt::format("PACK -(%A{}), -(%A{}), #{}", rx, ry, adj);
}
string OR_W_ToMem::disasm() {
     return fmt::format("OR.W {}, {}", rn->disasm(), ea->disasm());
}
string UNPK_D::disasm() {
     return fmt::format("UNPK %D{}, %D{}, #{}", rx, ry, adj);
}
string UNPK_M::disasm() {
     return fmt::format("UNPK -(%A{}), -(%A{}), #{}", rx, ry, adj);
}
string OR_L_ToMem::disasm() {
     return fmt::format("OR.L {}, {}", rn->disasm(), ea->disasm());
}
string DIVS_W::disasm() {
     return fmt::format("DIVS.W {}, {}", ea->disasm(), rn->disasm());
}

string SUB_B::disasm() {
     return fmt::format("SUB.B {}, {}", ea->disasm(), rn->disasm());
}
string SUB_W::disasm() {
     return fmt::format("SUB.W {}, {}", ea->disasm(), rn->disasm());
}
string SUB_L::disasm() {
     return fmt::format("SUB.L {}, {}", ea->disasm(), rn->disasm());
}
string SUBA_W::disasm() {
     return fmt::format("SUBA.W {}, {}", ea->disasm(), rn->disasm());
}
string SUBA_L::disasm() {
     return fmt::format("SUBA.L {}, {}", ea->disasm(), rn->disasm());
}

string SUBX_B_D::disasm() { return fmt::format("SUBX.B %D{}, %D{}", rx, ry); }
string SUBX_B_M::disasm() {
     return fmt::format("SUBX.B -(%A{}), -(%A{})", rx, ry);
}

string SUBX_W_D::disasm() { return fmt::format("SUBX.W %D{}, %D{}", rx, ry); }
string SUBX_W_M::disasm() {
     return fmt::format("SUBX.W -(%A{}), -(%A{})", rx, ry);
}

string SUBX_L_D::disasm() { return fmt::format("SUBX.L %D{}, %D{}", rx, ry); }
string SUBX_L_M::disasm() {
     return fmt::format("SUBX.L -(%A{}), -(%A{})", rx, ry);
}
string SUB_B_ToMem::disasm() {
     return fmt::format("SUB.B {}, {}", rn->disasm(), ea->disasm());
}
string SUB_W_ToMem::disasm() {
     return fmt::format("SUB.W {}, {}", rn->disasm(), ea->disasm());
}
string SUB_L_ToMem::disasm() {
     return fmt::format("SUB.L {}, {}", rn->disasm(), ea->disasm());
}
string ALINE::disasm() { return fmt::format("SYSCALL #{:04x}", i); }
string CMP_B::disasm() {
     return fmt::format("CMP.B {}, {}", ea->disasm(), rn->disasm());
}
string CMP_W::disasm() {
     return fmt::format("CMP.W {}, {}", ea->disasm(), rn->disasm());
}
string CMP_L::disasm() {
     return fmt::format("CMP.L {}, {}", ea->disasm(), rn->disasm());
}
string CMPA_W::disasm() {
     return fmt::format("CMPA.W {}, {}", ea->disasm(), rn->disasm());
}
string CMPA_L::disasm() {
     return fmt::format("CMPA.L {}, {}", ea->disasm(), rn->disasm());
}
string CMPM_B::disasm() {
     return fmt::format("CMPM.B (%A{})+, (%A{})+", ry, rx);
}
string CMPM_W::disasm() {
     return fmt::format("CMPM.W (%A{})+, (%A{})+", ry, rx);
}
string CMPM_L::disasm() {
     return fmt::format("CMPM.L (%A{})+, (%A{})+", ry, rx);
}

string EOR_B::disasm() {
     return fmt::format("EOR.B {}, {}", rn->disasm(), ea->disasm());
}
string EOR_W::disasm() {
     return fmt::format("EOR.W {}, {}", rn->disasm(), ea->disasm());
}
string EOR_L::disasm() {
     return fmt::format("EOR.L {}, {}", rn->disasm(), ea->disasm());
}

string AND_B::disasm() {
     return fmt::format("AND.B {}, {}", ea->disasm(), rn->disasm());
}
string AND_W::disasm() {
     return fmt::format("AND.W {}, {}", ea->disasm(), rn->disasm());
}
string AND_L::disasm() {
     return fmt::format("AND.L {}, {}", ea->disasm(), rn->disasm());
}
string MULU_W::disasm() {
     return fmt::format("MULU.W {}, {}", ea->disasm(), rn->disasm());
}
string ABCD_D::disasm() { return fmt::format("ABCD %D{}, %D{}", ry, rx); }
string ABCD_M::disasm() { return fmt::format("ABCD -(%A{}), -(%A{})", ry, rx); }
string AND_B_ToMem::disasm() {
     return fmt::format("AND.B {}, {}", rn->disasm(), ea->disasm());
}
string AND_W_ToMem::disasm() {
     return fmt::format("AND.W {}, {}", rn->disasm(), ea->disasm());
}
string AND_L_ToMem::disasm() {
     return fmt::format("AND.L {}, {}", rn->disasm(), ea->disasm());
}
string MULS_W::disasm() {
     return fmt::format("MULS.W {}, {}", ea->disasm(), rn->disasm());
}
string EXG_DD::disasm() { return fmt::format("EXG %D{}, %D{}", rx, ry); }
string EXG_AA::disasm() { return fmt::format("EXG %A{}, %A{}", rx, ry); }
string EXG_DA::disasm() { return fmt::format("EXG %D{}, %A{}", rx, ry); }

string ADD_B::disasm() {
     return fmt::format("ADD.B {}, {}", ea->disasm(), rn->disasm());
}
string ADD_W::disasm() {
     return fmt::format("ADD.W {}, {}", ea->disasm(), rn->disasm());
}
string ADD_L::disasm() {
     return fmt::format("ADD.L {}, {}", ea->disasm(), rn->disasm());
}
string ADDA_W::disasm() {
     return fmt::format("ADDA.W {}, {}", ea->disasm(), rn->disasm());
}
string ADDA_L::disasm() {
     return fmt::format("ADDA.L {}, {}", ea->disasm(), rn->disasm());
}

string ADDX_B_D::disasm() { return fmt::format("ADDX.B %D{}, %D{}", rx, ry); }
string ADDX_B_M::disasm() {
     return fmt::format("ADDX.B -(%A{}), -(%A{})", rx, ry);
}

string ADDX_W_D::disasm() { return fmt::format("ADDX.W %D{}, %D{}", rx, ry); }
string ADDX_W_M::disasm() {
     return fmt::format("ADDX.W -(%A{}), -(%A{})", rx, ry);
}

string ADDX_L_D::disasm() { return fmt::format("ADDX.L %D{}, %D{}", rx, ry); }
string ADDX_L_M::disasm() {
     return fmt::format("ADDX.L -(%A{}), -(%A{})", rx, ry);
}
string ADD_B_ToMem::disasm() {
     return fmt::format("ADD.B {}, {}", rn->disasm(), ea->disasm());
}
string ADD_W_ToMem::disasm() {
     return fmt::format("ADD.W {}, {}", rn->disasm(), ea->disasm());
}
string ADD_L_ToMem::disasm() {
     return fmt::format("ADD.L {}, {}", rn->disasm(), ea->disasm());
}

string ASR_B_I::disasm() { return fmt::format("ASR.B #{}, %D{}", rx, ry); }
string ASR_B_R::disasm() { return fmt::format("ASR.B %D{}, %D{}", rx, ry); }
string LSR_B_I::disasm() { return fmt::format("LSR.B #{}, %D{}", rx, ry); }
string LSR_B_R::disasm() { return fmt::format("LSR.B %D{}, %D{}", rx, ry); }
string ROXR_B_I::disasm() { return fmt::format("ROXR.B #{}, %D{}", rx, ry); }
string ROXR_B_R::disasm() { return fmt::format("ROXR.B %D{}, %D{}", rx, ry); }
string ROR_B_I::disasm() { return fmt::format("ROR.B #{}, %D{}", rx, ry); }
string ROR_B_R::disasm() { return fmt::format("ROR.B %D{}, %D{}", rx, ry); }

string ASL_B_I::disasm() { return fmt::format("ASL.B #{}, %D{}", rx, ry); }
string ASL_B_R::disasm() { return fmt::format("ASL.B %D{}, %D{}", rx, ry); }
string LSL_B_I::disasm() { return fmt::format("LSL.B #{}, %D{}", rx, ry); }
string LSL_B_R::disasm() { return fmt::format("LSL.B %D{}, %D{}", rx, ry); }
string ROXL_B_I::disasm() { return fmt::format("ROXL.B #{}, %D{}", rx, ry); }
string ROXL_B_R::disasm() { return fmt::format("ROXL.B %D{}, %D{}", rx, ry); }
string ROL_B_I::disasm() { return fmt::format("ROL.B #{}, %D{}", rx, ry); }
string ROL_B_R::disasm() { return fmt::format("ROL.B %D{}, %D{}", rx, ry); }

string ASR_W_I::disasm() { return fmt::format("ASR.W #{}, %D{}", rx, ry); }
string ASR_W_R::disasm() { return fmt::format("ASR.W %D{}, %D{}", rx, ry); }
string LSR_W_I::disasm() { return fmt::format("LSR.W #{}, %D{}", rx, ry); }
string LSR_W_R::disasm() { return fmt::format("LSR.W %D{}, %D{}", rx, ry); }
string ROXR_W_I::disasm() { return fmt::format("ROXR.W #{}, %D{}", rx, ry); }
string ROXR_W_R::disasm() { return fmt::format("ROXR.W %D{}, %D{}", rx, ry); }
string ROR_W_I::disasm() { return fmt::format("ROR.W #{}, %D{}", rx, ry); }
string ROR_W_R::disasm() { return fmt::format("ROR.W %D{}, %D{}", rx, ry); }

string ASL_W_I::disasm() { return fmt::format("ASL.W #{}, %D{}", rx, ry); }
string ASL_W_R::disasm() { return fmt::format("ASL.W %D{}, %D{}", rx, ry); }
string LSL_W_I::disasm() { return fmt::format("LSL.W #{}, %D{}", rx, ry); }
string LSL_W_R::disasm() { return fmt::format("LSL.W %D{}, %D{}", rx, ry); }
string ROXL_W_I::disasm() { return fmt::format("ROXL.W #{}, %D{}", rx, ry); }
string ROXL_W_R::disasm() { return fmt::format("ROXL.W %D{}, %D{}", rx, ry); }
string ROL_W_I::disasm() { return fmt::format("ROL.W #{}, %D{}", rx, ry); }
string ROL_W_R::disasm() { return fmt::format("ROL.W %D{}, %D{}", rx, ry); }

string ASR_L_I::disasm() { return fmt::format("ASR.L #{}, %D{}", rx, ry); }
string ASR_L_R::disasm() { return fmt::format("ASR.L %D{}, %D{}", rx, ry); }
string LSR_L_I::disasm() { return fmt::format("LSR.L #{}, %D{}", rx, ry); }
string LSR_L_R::disasm() { return fmt::format("LSR.L %D{}, %D{}", rx, ry); }
string ROXR_L_I::disasm() { return fmt::format("ROXR.L #{}, %D{}", rx, ry); }
string ROXR_L_R::disasm() { return fmt::format("ROXR.L %D{}, %D{}", rx, ry); }
string ROR_L_I::disasm() { return fmt::format("ROR.L #{}, %D{}", rx, ry); }
string ROR_L_R::disasm() { return fmt::format("ROR.L %D{}, %D{}", rx, ry); }

string ASL_L_I::disasm() { return fmt::format("ASL.L #{}, %D{}", rx, ry); }
string ASL_L_R::disasm() { return fmt::format("ASL.L %D{}, %D{}", rx, ry); }
string LSL_L_I::disasm() { return fmt::format("LSL.L #{}, %D{}", rx, ry); }
string LSL_L_R::disasm() { return fmt::format("LSL.L %D{}, %D{}", rx, ry); }
string ROXL_L_I::disasm() { return fmt::format("ROXL.L #{}, %D{}", rx, ry); }
string ROXL_L_R::disasm() { return fmt::format("ROXL.L %D{}, %D{}", rx, ry); }
string ROL_L_I::disasm() { return fmt::format("ROL.L #{}, %D{}", rx, ry); }
string ROL_L_R::disasm() { return fmt::format("ROL.L %D{}, %D{}", rx, ry); }
string ASR_EA::disasm() { return fmt::format("ASR.W {}", ea->disasm()); }
string LSR_EA::disasm() { return fmt::format("LSR.W {}", ea->disasm()); }
string ROR_EA::disasm() { return fmt::format("ROR.W {}", ea->disasm()); }
string ROXR_EA::disasm() { return fmt::format("ROXR.W {}", ea->disasm()); }
string ASL_EA::disasm() { return fmt::format("ASL.W {}", ea->disasm()); }
string LSL_EA::disasm() { return fmt::format("LSL.W {}", ea->disasm()); }
string ROL_EA::disasm() { return fmt::format("ROL.W {}", ea->disasm()); }
string ROXL_EA::disasm() { return fmt::format("ROXL.W {}", ea->disasm()); }

string BFTST_D::disasm() {
     std::string of_s =
         Do ? fmt::format("%D{}", offset & 7) : fmt::format("{}", offset);
     std::string wi_s = Do ? fmt::format("%D{}", width & 7)
                           : fmt::format("{}", width ? width : 32);
     return fmt::format("BFTST {}{{{}:{}}}", ea->disasm(), of_s, wi_s);
}

string BFTST_M::disasm() {
     std::string of_s =
         Do ? fmt::format("%D{}", offset & 7) : fmt::format("{}", offset);
     std::string wi_s = Do ? fmt::format("%D{}", width & 7)
                           : fmt::format("{}", width ? width : 32);
     return fmt::format("BFTST {}{{{}:{}}}", ea->disasm(), of_s, wi_s);
}

string BFCHG_D::disasm() {
     std::string of_s =
         Do ? fmt::format("%D{}", offset & 7) : fmt::format("{}", offset);
     std::string wi_s = Do ? fmt::format("%D{}", width & 7)
                           : fmt::format("{}", width ? width : 32);
     return fmt::format("BFCHG {}{{{}:{}}}", ea->disasm(), of_s, wi_s);
}

string BFCHG_M::disasm() {
     std::string of_s =
         Do ? fmt::format("%D{}", offset & 7) : fmt::format("{}", offset);
     std::string wi_s = Do ? fmt::format("%D{}", width & 7)
                           : fmt::format("{}", width ? width : 32);
     return fmt::format("BFCHG {}{{{}:{}}}", ea->disasm(), of_s, wi_s);
}

string BFCLR_D::disasm() {
     std::string of_s =
         Do ? fmt::format("%D{}", offset & 7) : fmt::format("{}", offset);
     std::string wi_s = Do ? fmt::format("%D{}", width & 7)
                           : fmt::format("{}", width ? width : 32);
     return fmt::format("BFCLR {}{{{}:{}}}", ea->disasm(), of_s, wi_s);
}

string BFCLR_M::disasm() {
     std::string of_s =
         Do ? fmt::format("%D{}", offset & 7) : fmt::format("{}", offset);
     std::string wi_s = Do ? fmt::format("%D{}", width & 7)
                           : fmt::format("{}", width ? width : 32);
     return fmt::format("BFCLR {}{{{}:{}}}", ea->disasm(), of_s, wi_s);
}

string BFSET_D::disasm() {
     std::string of_s =
         Do ? fmt::format("%D{}", offset & 7) : fmt::format("{}", offset);
     std::string wi_s = Do ? fmt::format("%D{}", width & 7)
                           : fmt::format("{}", width ? width : 32);
     return fmt::format("BFSET {}{{{}:{}}}", ea->disasm(), of_s, wi_s);
}

string BFSET_M::disasm() {
     std::string of_s =
         Do ? fmt::format("%D{}", offset & 7) : fmt::format("{}", offset);
     std::string wi_s = Do ? fmt::format("%D{}", width & 7)
                           : fmt::format("{}", width ? width : 32);
     return fmt::format("BFSET {}{{{}:{}}}", ea->disasm(), of_s, wi_s);
}

string BFEXTU_D::disasm() {
     std::string of_s =
         Do ? fmt::format("%D{}", offset & 7) : fmt::format("{}", offset);
     std::string wi_s = Do ? fmt::format("%D{}", width & 7)
                           : fmt::format("{}", width ? width : 32);
     return fmt::format("BFEXTU {}{{{}:{}}}, %D{}", ea->disasm(), of_s, wi_s,
                        dn);
}

string BFEXTU_M::disasm() {
     std::string of_s =
         Do ? fmt::format("%D{}", offset & 7) : fmt::format("{}", offset);
     std::string wi_s = Do ? fmt::format("%D{}", width & 7)
                           : fmt::format("{}", width ? width : 32);
     return fmt::format("BFEXTU {}{{{}:{}}}, %D{}", ea->disasm(), of_s, wi_s,
                        dn);
}

string BFEXTS_D::disasm() {
     std::string of_s =
         Do ? fmt::format("%D{}", offset & 7) : fmt::format("{}", offset);
     std::string wi_s = Do ? fmt::format("%D{}", width & 7)
                           : fmt::format("{}", width ? width : 32);
     return fmt::format("BFEXTS {}{{{}:{}}}, %D{}", ea->disasm(), of_s, wi_s,
                        dn);
}

string BFEXTS_M::disasm() {
     std::string of_s =
         Do ? fmt::format("%D{}", offset & 7) : fmt::format("{}", offset);
     std::string wi_s = Do ? fmt::format("%D{}", width & 7)
                           : fmt::format("{}", width ? width : 32);
     return fmt::format("BFEXTS {}{{{}:{}}}, %D{}", ea->disasm(), of_s, wi_s,
                        dn);
}

string BFFFO_D::disasm() {
     std::string of_s =
         Do ? fmt::format("%D{}", offset & 7) : fmt::format("{}", offset);
     std::string wi_s = Do ? fmt::format("%D{}", width & 7)
                           : fmt::format("{}", width ? width : 32);
     return fmt::format("BFFFO {}{{{}:{}}}, %D{}", ea->disasm(), of_s, wi_s,
                        dn);
}

string BFFFO_M::disasm() {
     std::string of_s =
         Do ? fmt::format("%D{}", offset & 7) : fmt::format("{}", offset);
     std::string wi_s = Do ? fmt::format("%D{}", width & 7)
                           : fmt::format("{}", width ? width : 32);
     return fmt::format("BFFFO {}{{{}:{}}}, %D{}", ea->disasm(), of_s, wi_s,
                        dn);
}

string BFINS_D::disasm() {
     std::string of_s =
         Do ? fmt::format("%D{}", offset & 7) : fmt::format("{}", offset);
     std::string wi_s = Do ? fmt::format("%D{}", width & 7)
                           : fmt::format("{}", width ? width : 32);
     return fmt::format("BFINS %D{}, {}{{{}:{}}}", dn, ea->disasm(), of_s,
                        wi_s);
}

string BFINS_M::disasm() {
     std::string of_s =
         Do ? fmt::format("%D{}", offset & 7) : fmt::format("{}", offset);
     std::string wi_s = Do ? fmt::format("%D{}", width & 7)
                           : fmt::format("{}", width ? width : 32);
     return fmt::format("BFINS %D{}, {}{{{}:{}}}", dn, ea->disasm(), of_s,
                        wi_s);
}

string FLINE::disasm() { return "CO-PROCESSOR"; }

string MOVE16_inc_imm::disasm() {
     return fmt::format("MOVE16 (%A{})+, ({:08x})", ax, imm);
}
string MOVE16_imm_inc::disasm() {
     return fmt::format("MOVE16 ({:08x}), (%A{})+", imm, ax);
}

string MOVE16_base_imm::disasm() {
     return fmt::format("MOVE16 (%A{}), ({:08x})", ax, imm);
}
string MOVE16_imm_base::disasm() {
     return fmt::format("MOVE16 ({:08x}), (%A{})", imm, ax);
}
string MOVE16_inc_inc::disasm() {
     return fmt::format("MOVE16 (%A{})+, (%A{})+", ax, ay);
}
} // namespace OP
#endif