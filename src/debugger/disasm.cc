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
using fmt::format;
using std::string;
struct restore_pc {
    uint32_t pc;
    ~restore_pc() { cpu.PC = pc; }
};
string Rn(int reg) { return format("%{}{}", reg & 8 ? 'A' : 'D', reg & 7); }
string disasm_parseExt(int reg) {

    uint16_t next = FETCH();
    int ri = next >> 12 & 15;
    bool ri_l = next & 1 << 11;
    int ri_c = next >> 9 & 3;
    string base = reg >= 0 ? format("%A{}", reg) : "%PC";
    string rn_v = format("{}{}*{}", Rn(ri), ri_l ? "" : ".W", 1 << ri_c);
    if(!(next & 1 << 8)) {
        int8_t d = next & 0xff;
        return format("({}, {}, {})", d, base, rn_v);
    }
    std::vector<string> v1, v2;

    switch(next >> 4 & 3) {
    case 1:
        break;
    case 2:
        v1.push_back(format("{}", static_cast<int16_t>(FETCH())));
        break;
    case 3:
        v1.push_back(format("{}", static_cast<int32_t>(FETCH32())));
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
        return format("({})", fmt::join(v1, ", "));
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
            v2.push_back(format("[{}]", fmt::join(v1, ", ")));
        } else {
            v2.push_back(format("[{}]", fmt::join(v1, ", ")));
            v2.push_back(rn_v);
        }
    }
    if(od_v) {
        v2.push_back(format("{}", od_v));
    }
    return format("({})", fmt::join(v2, ", "));
}
void load_fpX(uint64_t frac, uint16_t exp);
void load_fpP(uint32_t addr);
string disasm_ea(int type, int reg, int sz) {
    switch(type) {
    case 0:
        return format("%D{}", reg);
    case 1:
        return format("%A{}", reg);
    case 2:
        return format("(%A{})", reg);
    case 3:
        return format("(%A{})+", reg);
    case 4:
        return format("-(%A{})", reg);
    case 5: {
        int16_t d = FETCH();
        return format("({:d}, %A{})", d, reg);
    }
    case 6:
        return disasm_parseExt(reg);
    case 7:
        switch(reg) {
        case 0:
            return format("(0x{:x})", FETCH());
        case 1:
            return format("(0x{:x})", FETCH32());
        case 2: {
            int16_t d = FETCH();
            return format("({}, %PC)", d);
        }
        case 3:
            return disasm_parseExt(-1);
        case 4:
            switch(sz) {
            case 1:
                return format("#{}", static_cast<int8_t>(FETCH()));
            case 2:
                return format("#{}", static_cast<int16_t>(FETCH()));
            case 4:
                return format("#{}", static_cast<int32_t>(FETCH32()));
            case -4: // float
                return format("#{}", std::bit_cast<float>(FETCH32()));
            case -8: // double
            {
                uint64_t v1 = FETCH32();
                v1 = v1 << 32 | FETCH32();
                return format("#{}", std::bit_cast<double>(v1));
            }
            case -12: // EXT
            {
                uint16_t exp = FETCH32() >> 16;
                uint64_t frac = FETCH32();
                frac = frac << 32 | FETCH32();
                load_fpX(frac, exp);
                char *p;
                mpfr_asprintf(&p, "%RNf", cpu.fp_tmp);
                auto ss = format("#{}", p);
                mpfr_free_str(p);
                return ss;
            }
            case 12: // P
            {
                load_fpP(cpu.PC);
                cpu.PC += 12;
                char *p;
                mpfr_asprintf(&p, "%RNf", cpu.fp_tmp);
                auto ss = format("#{}", p);
                mpfr_free_str(p);
                return ss;
            }
            }
        }
    }
    return "?";
}
namespace DISASM {
static const char *ops[] = {"ORI", "ANDI", "SUBI", "ADDI",
                            "",    "EORI", "CMPI", ""};
const char *sz_array[3] = {
    "B",
    "W",
    "L",
};

string disasm_bitimm_B(int op, int type, int reg) {
    uint8_t imm = FETCH();
    if(type == 7 && reg == 4) {
        return format("{}.B #0x{:02x}, %CCR", ops[op], imm);
    } else {
        return format("{}.B #0x{:02x}, {}", ops[op], imm,
                      disasm_ea(type, reg, 1));
    }
}

string disasm_arithmetic_B(int op, int type, int reg) {
    uint8_t imm = FETCH();
    return format("{}.B #{}, {}", ops[op], imm, disasm_ea(type, reg, 1));
}

string disasm_bitimm_W(int op, int type, int reg) {
    uint16_t imm = FETCH();
    if(type == 7 && reg == 4) {
        return format("{}.W #0x{:04x}, %SR", ops[op], imm);
    } else {
        return format("{}.W #0x{:04x}, {}", ops[op], imm,
                      disasm_ea(type, reg, 2));
    }
}

string disasm_arithmetic_W(int op, int type, int reg) {
    uint16_t imm = FETCH();
    return format("{}.W #{}, {}", ops[op], imm, disasm_ea(type, reg, 2));
}

string disasm_bitimm_L(int op, int type, int reg) {
    uint32_t imm = FETCH32();
    return format("{}.L #0x{:08x}, {}", ops[op], imm, disasm_ea(type, reg, 4));
}

string disasm_arithmetic_L(int op, int type, int reg) {
    uint32_t imm = FETCH32();
    return format("{}.L #{}, {}", ops[op], imm, disasm_ea(type, reg, 4));
}

string disasm_moves(int size, int type, int reg) {
    uint16_t imm = FETCH();
    if(imm & 1 << 11) {
        return format("MOVES.{0} {2}, {1}", sz_array[size],
                      disasm_ea(type, reg, 1 << size), Rn(imm >> 12 & 0x1f));

    } else {
        return format("MOVES.{0} {1}, {2}", sz_array[size],
                      disasm_ea(type, reg, 1 << size), Rn(imm >> 12 & 0x1f));
    }
}
static const char *bitop[] = {"BTST", "BCHG", "BCLR", "BSET"};
string disasm_bitop_imm(int op, int type, int reg) {
    uint8_t imm = FETCH();
    return format("{}.{} #{}, {}", bitop[op], type == 0 ? 'L' : 'B',
                  imm, disasm_ea(type, reg, 1));
}
string disasm_0X0(int dn, int type, int reg) {
    switch(dn) {
    case 0:
        return disasm_bitimm_B(0, type, reg);
    case 1:
        return disasm_bitimm_B(1, type, reg);
    case 2:
        return disasm_arithmetic_B(2, type, reg);
    case 3:
        return disasm_arithmetic_B(3, type, reg);
    case 4:
        return disasm_bitop_imm(0, type, reg);
    case 5:
        return disasm_bitimm_B(5, type, reg);
    case 6:
        return disasm_arithmetic_B(6, type, reg);
    case 7:
        return disasm_moves(0, type, reg);
    }
    __builtin_unreachable();
}

string disasm_0X1(int dn, int type, int reg) {
    switch(dn) {
    case 0:
        return disasm_bitimm_W(0, type, reg);
    case 1:
        return disasm_bitimm_W(1, type, reg);
    case 2:
        return disasm_arithmetic_W(2, type, reg);
    case 3:
        return disasm_arithmetic_W(3, type, reg);
    case 4:
        return disasm_bitop_imm(1, type, reg);

    case 5:
        return disasm_bitimm_W(5, type, reg);
    case 6:
        return disasm_arithmetic_W(6, type, reg);
    case 7:
        return disasm_moves(1, type, reg);
    }
    __builtin_unreachable();
}

string disasm_0X2(int dn, int type, int reg) {
    switch(dn) {
    case 0:
        return disasm_bitimm_L(0, type, reg);
    case 1:
        return disasm_bitimm_L(1, type, reg);
    case 2:
        return disasm_arithmetic_L(2, type, reg);
    case 3:
        return disasm_arithmetic_L(3, type, reg);
    case 4:
        return disasm_bitop_imm(2, type, reg);

    case 5:
        return disasm_bitimm_L(5, type, reg);
    case 6:
        return disasm_arithmetic_L(6, type, reg);
    case 7:
        return disasm_moves(2, type, reg);
    }
    __builtin_unreachable();
}

string disasm_0X3(int dn, int type, int reg) {
    switch(dn) {
    case 0:
    case 1:
    case 2: {
        uint16_t extw = FETCH();
        int rn = extw >> 12 & 0xf;
        string ea = disasm_ea(type, reg, 0);
        if(extw & 1 << 11) {
            return format("CHK2.{} {}, {}", sz_array[dn], ea, Rn(rn));
        } else {
            return format("CMP2.{} {}, {}", sz_array[dn], ea, Rn(rn));
        }
    }
    case 4:
        return disasm_bitop_imm(3, type, reg);

    case 5: {
        uint16_t extw = FETCH();
        int du = extw >> 6 & 7;
        int dc = extw & 7;
        return format("CAS.B %D{}, %D{}, {}", dc, du, disasm_ea(type, reg, 1));
    }
    case 6:
    case 7:
        if(type == 7 && reg == 4) {
            uint16_t extw1 = FETCH();
            uint16_t extw2 = FETCH();
            return format("CAS2.{} %D{}:%D{}, %D{}:%D{}, ({}:{})",
                          dn == 6 ? 'W' : 'L', extw1 & 7, extw2 & 7,
                          extw1 >> 6 & 7, extw2 >> 6 & 7, Rn(extw1 >> 12 & 15),
                          Rn(extw2 >> 12 & 15));

        } else {
            uint16_t extw = FETCH();
            int du = extw >> 6 & 7;
            int dc = extw & 7;
            return format("CAS.{} %D{}, %D{}, {}", dn == 6 ? 'W' : 'L', dc, du,
                          disasm_ea(type, reg, 1));
        }
    default:
        __builtin_unreachable();
    }
}
string disasm_0X4(int dn, int type, int reg) {
    if(type == 1) {
        int16_t imm = FETCH();
        return format("MOVEP.W (#{0}, %A{1}), %D{2}", imm, reg, dn);
    } else {
        return format("BTST.{} %D{}, {}", type == 0 ? 'L' : 'B', dn,
                      disasm_ea(type, reg, 1));
    }
}

string disasm_0X5(int dn, int type, int reg) {
    if(type == 1) {
        int16_t imm = FETCH();
        return format("MOVEP.L (#{0}, %A{1}), %D{2}", imm, reg, dn);
    } else {
        return format("BCHG.{} %D{}, {}", type == 0 ? 'L' : 'B', dn,
                      disasm_ea(type, reg, 1));
    }
}

string disasm_0X6(int dn, int type, int reg) {
    if(type == 1) {
        int16_t imm = FETCH();
        return format("MOVEP.W %D{2}, (#{0}, %A{1})", imm, reg, dn);
    } else {
        return format("BCLR.{} %D{}, {}", type == 0 ? 'L' : 'B', dn,
                      disasm_ea(type, reg, 1));
    }
}

string disasm_0X7(int dn, int type, int reg) {
    if(type == 1) {
        int16_t imm = FETCH();
        return format("MOVEP.L %D{2}, (#{0}, %A{1})", imm, reg, dn);
    } else {
        return format("BSET.{} %D{}, {}", type == 0 ? 'L' : 'B', dn,
                      disasm_ea(type, reg, 1));
    }
}

string disasm_1XX(int dn_type, int dn, int type, int reg) {
    auto src = disasm_ea(type, reg, 1);
    auto dst = disasm_ea(dn_type, dn, 1);
    return format("MOVE.B {}, {}", src, dst);
}

string disasm_2XX(int dn_type, int dn, int type, int reg) {
    auto src = disasm_ea(type, reg, 4);
    auto dst = disasm_ea(dn_type, dn, 4);
    return format("MOVE{}.L {}, {}", dn_type == 1 ? "A" : "", src, dst);
}
string disasm_3XX(int dn_type, int dn, int type, int reg) {
    auto src = disasm_ea(type, reg, 2);
    auto dst = disasm_ea(dn_type, dn, 2);
    return format("MOVE{}.W {}, {}", dn_type == 1 ? "A" : "", src, dst);
}
std::unordered_map<int, const char *> CRMAP{
    {0x000, "SFC"},   {0x001, "DFC"},  {0x002, "CACR"}, {0x003, "TC"},
    {0x004, "ITT0"},  {0x005, "ITT1"}, {0x006, "DTT0"}, {0x007, "DTT1"},
    {0x800, "USP"},   {0x801, "VBR"},  {0x803, "MSP"},  {0x804, "ISP"},
    {0x805, "MMUSR"}, {0x806, "URP"},  {0x807, "SRP"},
};
static const char *op1[] = {"NEGX", "CLR", "NEG", "NOT", "", "TST"};
string disasm_4X0(int dn, int type, int reg) {
    switch(dn) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 5:
        return format("{}.B {}", op1[dn], disasm_ea(type, reg, 1));
    case 4:
        if(type == 1) {
            int32_t imm = FETCH32();
            return format("LINK.L %A{}, #{}", reg, imm);
        } else {
            return format("NBCD {}", disasm_ea(type, reg, 1));
        }
    case 6: {
        uint16_t extw = FETCH();
        int dl = extw >> 12 & 7;
        int dh = extw & 7;
        bool sg = extw & 1 << 11;
        bool lw = extw & 1 << 10;
        if(lw) {
            return format("MUL{}.L {}, %D{}-%D{}", sg ? 'S' : 'U',
                          disasm_ea(type, reg, 4), dh, dl);
        } else {
            return format("MUL{}.L {}, %D{}", sg ? 'S' : 'U',
                          disasm_ea(type, reg, 4), dl);
        }
    }
    case 7:
        return "#UNDEF";
    default:
        __builtin_unreachable();
    }
}
string disasm_4X1(int dn, int type, int reg) {
    switch(dn) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 5:
        return format("{}.W {}", op1[dn], disasm_ea(type, reg, 2));
    case 4:
        if(type == 0) {
            return format("SWAP %D{}", reg);
        } else if(type == 1) {
            return "BKPT";
        } else {
            return format("PEA {}", disasm_ea(type, reg, 0));
        }
    case 6: {
        uint16_t extw = FETCH();
        int dq = extw >> 12 & 7;
        int dr = extw & 7;
        bool sg = extw & 1 << 11;
        bool lw = extw & 1 << 10;
        if(lw) {
            return format("DIV{}.L {}, %D{}:%D{}", sg ? 'S' : 'U',
                          disasm_ea(type, reg, 4), dr, dq);
        } else {
            if(dr != dq) {
                return format("DIV{}L.L {}, %D{}:%D{}", sg ? 'S' : 'U',
                              disasm_ea(type, reg, 4), dr, dq);
            } else {
                return format("DIV{}.L {}, %D{}", sg ? 'S' : 'U',
                              disasm_ea(type, reg, 4), dq);
            }
        }
    }
    case 7:
        switch(type) {
        case 0:
        case 1:
            return format("TRAP #{}", type << 3 | reg);
        case 2: {
            int16_t imm = FETCH();
            return format("LINK.W %A{}, #{}", reg, imm);
        }
        case 3:
            return format("UNLK %A{}", reg);
        case 4:
            return format("MOVE %A{}, %USP", reg);
        case 5:
            return format("MOVE %USP, %A{}", reg);
        case 6:
            switch(reg) {
            case 0:
                return "RESET";
            case 1:
                return "NOP";
            case 2:
                return format("STOP #{:04x}", FETCH());
            case 3:
                return "RTE";
            case 4:
                return format("RTD #{}", static_cast<int16_t>(FETCH()));
            case 5:
                return "RTS";
            case 6:
                return "TRAPV";
            case 7:
                return "RTR";
            }
            __builtin_unreachable();
        case 7: {
            uint16_t extw = FETCH();
            int rn = extw >> 12;
            extw &= 0xfff;
            if(reg & 1) {
                return format("MOVEC {1}, %{0}", CRMAP[extw], Rn(rn));
            } else {
                return format("MOVEC %{0}, {1}", CRMAP[extw], Rn(rn));
            }
        }
        }
        __builtin_unreachable();
    default:
        __builtin_unreachable();
    }
}

auto get_rnlist(uint16_t regs, bool rev) {
    int first = -1;
    std::vector<std::pair<int, int>> x;
    for(int i = 0; i < 16; ++i) {
        if(regs & 1 << i) {
            if(first == -1) {
                first = i;
            }
        } else if(first != -1) {
            x.emplace_back(first, i - 1);
            first = -1;
        }
    }
    if(first != -1) {
        x.emplace_back(first, 15);
    }
    std::vector<string> reg_list;
    for(auto [bgn, end] : x) {
        if(bgn == end) {
            reg_list.push_back(Rn(rev ? 15 - bgn : bgn));
        } else {
            if(rev) {
                reg_list.push_back(format("{}-{}", Rn(15 - bgn), Rn(15 - end)));
            } else {
                reg_list.push_back(format("{}-{}", Rn(bgn), Rn(end)));
            }
        }
    }
    return reg_list;
}

string disasm_4X2(int dn, int type, int reg) {
    switch(dn) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 5:
        return format("{}.L {}", op1[dn], disasm_ea(type, reg, 4));
    case 4:
        if(type == 0) {
            return format("EXT.W %D{}", reg);
        } else {
            auto reglist = get_rnlist(FETCH(), type == 4);
            return format("MOVEM.W {}, {}", fmt::join(reglist, "/"),
                          disasm_ea(type, reg, 0));
        }
    case 6: {
        auto reglist = get_rnlist(FETCH(), false);
        return format("MOVEM.W {1}, {0}", fmt::join(reglist, "/"),
                      disasm_ea(type, reg, 0));
    }
    case 7:
        return format("JSR {}", disasm_ea(type, reg, 0));
    default:
        __builtin_unreachable();
    }
}

string disasm_4X3(int dn, int type, int reg) {
    switch(dn) {
    case 0:
        return format("MOVE.W %SR, {}", disasm_ea(type, reg, 2));
    case 1:
        return format("MOVE.W %CCR, {}", disasm_ea(type, reg, 2));
    case 2:
        return format("MOVE.W {}, %CCR", disasm_ea(type, reg, 2));
    case 3:
        return format("MOVE.W {}, %SR", disasm_ea(type, reg, 2));
    case 4:
        if(type == 0) {
            return format("EXT.L %D{}", reg);
        } else {
            auto reglist = get_rnlist(FETCH(), type == 4);
            return format("MOVEM.L {}, {}", fmt::join(reglist, "/"),
                          disasm_ea(type, reg, 0));
        }
    case 5:
        if(type == 7 && reg == 4) {
            return "ILLEGAL";
        } else {
            return format("TAS {}", disasm_ea(type, reg, 1));
        }
    case 6: {
        auto reglist = get_rnlist(FETCH(), false);
        return format("MOVEM.L {1}, {0}", fmt::join(reglist, "/"),
                      disasm_ea(type, reg, 0));
    }
    case 7:
        return format("JMP {}", disasm_ea(type, reg, 0));
    default:
        __builtin_unreachable();
    }
}
string disasm_4X4(int dn, int type, int reg) {
    return format("CHK.L {}, %D{}", disasm_ea(type, reg, 4), dn);
}
string disasm_4X6(int dn, int type, int reg) {
    return format("CHK.W {}, %D{}", disasm_ea(type, reg, 4), dn);
}
string disasm_4X7(int dn, int type, int reg) {
    if(type == 0 && dn == 4) {
        return format("EXTB.L %D{}", reg);

    } else {
        return format("LEA {}, %A{}", disasm_ea(type, reg, 0), dn);
    }
}
string disasm_5X0(int dn, int type, int reg) {
    return format("ADDQ.B #{}, {}", dn ? dn : 8, disasm_ea(type, reg, 1));
}
string disasm_5X1(int dn, int type, int reg) {
    return format("ADDQ.W #{}, {}", dn ? dn : 8, disasm_ea(type, reg, 2));
}
string disasm_5X2(int dn, int type, int reg) {
    return format("ADDQ.L #{}, {}", dn ? dn : 8, disasm_ea(type, reg, 4));
}
string disasm_5X4(int dn, int type, int reg) {
    return format("SUBQ.B #{}, {}", dn ? dn : 8, disasm_ea(type, reg, 1));
}
string disasm_5X5(int dn, int type, int reg) {
    return format("SUBQ.W #{}, {}", dn ? dn : 8, disasm_ea(type, reg, 2));
}
string disasm_5X6(int dn, int type, int reg) {
    return format("SUBQ.L #{}, {}", dn ? dn : 8, disasm_ea(type, reg, 4));
}
const char *cc_n[] = {"T",  "F",  "HI", "LS", "CC", "CS", "NE", "EQ",
                      "VC", "VS", "PL", "MI", "GE", "LT", "GT", "LE"};
string disasm_05C3(int cc, int type, int reg) {
    if(type == 1) {
        int16_t imm = FETCH();
        return format("DB{} %D{}, #{}", cc_n[cc], reg, imm);
    } else if(type == 7 && reg == 2) {
        uint16_t imm = FETCH();
        return format("TRAP{} #0x{:04x}", cc_n[cc], imm);
    } else if(type == 7 && reg == 3) {
        uint32_t imm = FETCH32();
        return format("TRAP{} #0x{:08x}", cc_n[cc], imm);
    } else if(type == 7 && reg == 4) {
        return format("TRAP{}", cc_n[cc]);
    } else {
        return format("S{} {}", cc_n[cc], disasm_ea(type, reg, 1));
    }
}

string disasm_6C(int cc, int8_t imm) {
    int32_t v = imm;
    uint32_t base = cpu.PC;
    if(imm == 0) {
        v = static_cast<int16_t>(FETCH());
    } else if(imm == -1) {
        v = FETCH32();
    }
    if(cc == 0) {
        return format("BRA #{:06x}", base + v);
    } else if(cc == 1) {
        return format("BSR #{:06x}", base + v);
    } else {
        return format("B{} #{:06x}", cc_n[cc], base + v);
    }
}
string disasm_7XX(int dn, int8_t imm) {
    return format("MOVEQ #{}, %D{}", imm, dn);
}

string disasm_8X0(int dn, int type, int reg) {
    return format("OR.B {}, %D{}", disasm_ea(type, reg, 1), dn);
}
string disasm_8X1(int dn, int type, int reg) {
    return format("OR.W {}, %D{}", disasm_ea(type, reg, 2), dn);
}
string disasm_8X2(int dn, int type, int reg) {
    return format("OR.L {}, %D{}", disasm_ea(type, reg, 4), dn);
}
string disasm_8X3(int dn, int type, int reg) {
    return format("DIVU.W {}, %D{}", disasm_ea(type, reg, 2), dn);
}
string disasm_8X4(int dn, int type, int reg) {
    if(type == 0) {
        return format("SBCD %D{}, %D{}", dn, reg);
    } else if(type == 1) {
        return format("SBCD -(%A{}), -(%A{})", dn, reg);
    } else {
        return format("OR.B %D{}, {}", dn, disasm_ea(type, reg, 1));
    }
}

string disasm_8X5(int dn, int type, int reg) {
    if(type == 0) {
        int16_t adj = FETCH();
        return format("PACK %D{}, %D{}, #{}", reg, dn, adj);
    } else if(type == 1) {
        int16_t adj = FETCH();
        return format("PACK -(%A{}), -(%A{}), #{}", reg, dn, adj);
    } else {
        return format("OR.W %D{}, {}", dn, disasm_ea(type, reg, 2));
    }
}

string disasm_8X6(int dn, int type, int reg) {
    if(type == 0) {
        int16_t adj = FETCH();
        return format("UNPK %D{}, %D{}, #{}", reg, dn, adj);
    } else if(type == 1) {
        int16_t adj = FETCH();
        return format("UNPK -(%A{}), -(%A{}), #{}", reg, dn, adj);
    } else {
        return format("OR.L %D{}, {}", dn, disasm_ea(type, reg, 4));
    }
}
string disasm_8X7(int dn, int type, int reg) {
    return format("DIVS.W {}, %D{}", disasm_ea(type, reg, 2), dn);
}

string disasm_9X0(int dn, int type, int reg) {
    return format("SUB.B {}, %D{}", disasm_ea(type, reg, 1), dn);
}
string disasm_9X1(int dn, int type, int reg) {
    return format("SUB.W {}, %D{}", disasm_ea(type, reg, 2), dn);
}
string disasm_9X2(int dn, int type, int reg) {
    return format("SUB.L {}, %D{}", disasm_ea(type, reg, 4), dn);
}
string disasm_9X3(int dn, int type, int reg) {
    return format("SUBA.W {}, %A{}", disasm_ea(type, reg, 2), dn);
}
string disasm_9X4(int dn, int type, int reg) {
    if(type == 0) {
        return format("SUBX.B %D{}, %D{}", dn, reg);
    } else if(type == 1) {
        return format("SUBX.B -(%A{}), -(%A{})", dn, reg);
    } else {
        return format("SUB.B %D{}, {}", dn, disasm_ea(type, reg, 1));
    }
}

string disasm_9X5(int dn, int type, int reg) {
    if(type == 0) {
        return format("SUBX.W %D{}, %D{}", dn, reg);
    } else if(type == 1) {
        return format("SUBX.W -(%A{}), -(%A{})", dn, reg);
    } else {
        return format("SUB.W %D{}, {}", dn, disasm_ea(type, reg, 2));
    }
}

string disasm_9X6(int dn, int type, int reg) {
    if(type == 0) {
        return format("SUBX.L %D{}, %D{}", dn, reg);
    } else if(type == 1) {
        return format("SUBX.L -(%A{}), -(%A{})", dn, reg);
    } else {
        return format("SUB.L %D{}, {}", dn, disasm_ea(type, reg, 4));
    }
}
string disasm_9X7(int dn, int type, int reg) {
    return format("SUBA.L {}, %A{}", disasm_ea(type, reg, 2), dn);
}

string disasm_BX0(int dn, int type, int reg) {
    return format("CMP.B {}, %D{}", disasm_ea(type, reg, 1), dn);
}
string disasm_BX1(int dn, int type, int reg) {
    return format("CMP.W {}, %D{}", disasm_ea(type, reg, 2), dn);
}
string disasm_BX2(int dn, int type, int reg) {
    return format("CMP.L {}, %D{}", disasm_ea(type, reg, 4), dn);
}
string disasm_BX3(int dn, int type, int reg) {
    return format("CMPA.W {}, %A{}", disasm_ea(type, reg, 2), dn);
}
string disasm_BX4(int dn, int type, int reg) {
    if(type == 1) {
        return format("CMPM.B (%A{})+, (%A{})+", reg, dn);
    } else {
        return format("EOR.B %D{}, {}", dn, disasm_ea(type, reg, 1));
    }
}

string disasm_BX5(int dn, int type, int reg) {
    if(type == 1) {
        return format("CMPM.W (%A{})+, (%A{})+", reg, dn);
    } else {
        return format("EOR.W %D{}, {}", dn, disasm_ea(type, reg, 2));
    }
}

string disasm_BX6(int dn, int type, int reg) {
    if(type == 1) {
        return format("CMPM.L (%A{})+, (%A{})+", reg, dn);
    } else {
        return format("EOR.L %D{}, {}", dn, disasm_ea(type, reg, 4));
    }
}
string disasm_BX7(int dn, int type, int reg) {
    return format("CMPA.L {}, %A{}", disasm_ea(type, reg, 4), dn);
}

string disasm_CX0(int dn, int type, int reg) {
    return format("AND.B {}, %D{}", disasm_ea(type, reg, 1), dn);
}
string disasm_CX1(int dn, int type, int reg) {
    return format("AND.W {}, %D{}", disasm_ea(type, reg, 2), dn);
}
string disasm_CX2(int dn, int type, int reg) {
    return format("AND.L {}, %D{}", disasm_ea(type, reg, 4), dn);
}
string disasm_CX3(int dn, int type, int reg) {
    return format("MULU.W {}, %D{}", disasm_ea(type, reg, 2), dn);
}
string disasm_CX4(int dn, int type, int reg) {
    if(type == 0) {
        return format("ABCD %D{}, %D{}", reg, dn);
    } else if(type == 1) {
        return format("ABCD -(%A{}), -(%A{})", reg, dn);
    } else {
        return format("AND.B %D{}, {}", dn, disasm_ea(type, reg, 1));
    }
}

string disasm_CX5(int dn, int type, int reg) {
    if(type == 0) {
        return format("EXG %D{}, %D{}", dn, reg);
    } else if(type == 1) {
        return format("EXG %A{}, %A{}", dn, reg);
    } else {
        return format("AND.W %D{}, {}", dn, disasm_ea(type, reg, 2));
    }
}

string disasm_CX6(int dn, int type, int reg) {
    if(type == 1) {
        return format("EXG %D{}, %A{}", dn, reg);
    } else {
        return format("AND.L %D{}, {}", dn, disasm_ea(type, reg, 4));
    }
}
string disasm_CX7(int dn, int type, int reg) {
    return format("MULS.W {}, %D{}", disasm_ea(type, reg, 2), dn);
}

string disasm_DX0(int dn, int type, int reg) {
    return format("ADD.B {}, %D{}", disasm_ea(type, reg, 1), dn);
}
string disasm_DX1(int dn, int type, int reg) {
    return format("ADD.W {}, %D{}", disasm_ea(type, reg, 2), dn);
}
string disasm_DX2(int dn, int type, int reg) {
    return format("ADD.L {}, %D{}", disasm_ea(type, reg, 4), dn);
}
string disasm_DX3(int dn, int type, int reg) {
    return format("ADDA.W {}, %A{}", disasm_ea(type, reg, 2), dn);
}
string disasm_DX4(int dn, int type, int reg) {
    if(type == 0) {
        return format("ADDX.B %D{}, %D{}", dn, reg);
    } else if(type == 1) {
        return format("ADDX.B -(%A{}), -(%A{})", dn, reg);
    } else {
        return format("ADD.B %D{}, {}", dn, disasm_ea(type, reg, 1));
    }
}

string disasm_DX5(int dn, int type, int reg) {
    if(type == 0) {
        return format("ADDX.W %D{}, %D{}", dn, reg);
    } else if(type == 1) {
        return format("ADDX.W -(%A{}), -(%A{})", dn, reg);
    } else {
        return format("ADD.W %D{}, {}", dn, disasm_ea(type, reg, 2));
    }
}

string disasm_DX6(int dn, int type, int reg) {
    if(type == 0) {
        return format("ADDX.L %D{}, %D{}", dn, reg);
    } else if(type == 1) {
        return format("ADDX.L -(%A{}), -(%A{})", dn, reg);
    } else {
        return format("ADD.L %D{}, {}", dn, disasm_ea(type, reg, 4));
    }
}
string disasm_DX7(int dn, int type, int reg) {
    return format("ADDA.L {}, %A{}", disasm_ea(type, reg, 2), dn);
}
static const char *sc_n[] = {"AS", "LS", "ROX", "RO"};
string disasm_EX_SHIFT(bool isL, int N, int dn, int type, int reg) {
    if(type & 4) {
        return format("{}{}.{} %D{}, %D{}", sc_n[type & 3], isL ? 'L' : 'R',
                      sz_array[N], dn, reg);
    } else {
        return format("{}{}.{} #{}, %D{}", sc_n[type & 3], isL ? 'L' : 'R',
                      sz_array[N], dn ? dn : 8, reg);
    }
}
string disasm_EX3(int dn, int type, int reg) {
    static const char *bf1[] = {"BFTST", "BFCHG", "BFCLR", "BFSET"};
    if(!(dn & 4)) {
        return format("{}R.W #1, {}", sc_n[dn], disasm_ea(type, reg, 2));
    }
    uint16_t nw = FETCH();
    bool offset_r = nw & 1 << 11;
    int offset = nw >> 6 & 0x1f;
    bool width_r = nw & 1 << 5;
    int width = nw & 0x1f;
    return format("{} {}{{{}{}:{}{}}}", bf1[dn & 3], disasm_ea(type, reg, 0),
                  offset_r ? "%D" : "", offset, width_r ? "%D" : "", width);
}
string disasm_EX7(int dn, int type, int reg) {
    static const char *bf2[] = {"BFEXTU", "BFEXTS", "BFFFO", "BFINS"};
    if(!(dn & 4)) {
        return format("{}L.W #1, {}", sc_n[dn], disasm_ea(type, reg, 2));
    }
    uint16_t nw = FETCH();
    bool offset_r = nw & 1 << 11;
    int offset = nw >> 6 & 0x1f;
    bool width_r = nw & 1 << 5;
    int width = nw & 0x1f;
    int dn2 = nw >> 12 & 7;
    if(dn != 7) {
        return format("{} {}{{{}{}:{}{}}}, %D{}", bf2[dn & 3],
                      disasm_ea(type, reg, 0), offset_r ? "%D" : "", offset,
                      width_r ? "%D" : "", width, dn2);

    } else {
        return format("BFINS %D{}, {}{{{}{}:{}{}}}", dn2,
                      disasm_ea(type, reg, 0), offset_r ? "%D" : "", offset,
                      width_r ? "%D" : "", width);
    }
}
static int fp_sz[] = {4, -4, -12, 12, 2, -8, 1, 12};
static const char fp_c[] = {'L', 'S', 'X', 'P', 'W', 'D', 'B', 'P'};
std::unordered_map<int, const char *> fop{
    {0x0, "FMOVE"},    {0x1, "FINT"},     {0x2, "FSINH"},    {0x3, "FINTRZ"},
    {0x4, "FSQRT"},    {0x6, "FLOGNP1"},  {0x8, "FETOXM1"},  {0x9, "FTANH"},
    {0xA, "FATAN"},    {0xC, "FASIN"},    {0xD, "FATANH"},   {0xE, "FSIN"},
    {0xF, "FTAN"},     {0x10, "FETOX"},   {0x11, "FTWOTOX"}, {0x12, "FTENTOX"},
    {0x14, "FLOGN"},   {0x15, "FLOG10"},  {0x16, "FLOG2"},   {0x18, "FABS"},
    {0x19, "FCOSH"},   {0x1A, "FNEG"},    {0x1C, "FACOS"},   {0x1D, "FCOS"},
    {0x1E, "FGETEXP"}, {0x1F, "FGETMAN"}, {0x20, "FDIV"},    {0x21, "FMOD"},
    {0x22, "FADD"},    {0x23, "FMUL"},    {0x24, "FSGLDIV"}, {0x25, "FREM"},
    {0x26, "FSCALE"},  {0x27, "FSGLMUL"}, {0x28, "FSUB"},    {0x38, "FCMP"},
    {0x40, "FSMOVE"},  {0x41, "FSSQRT"},  {0x44, "FDMOVE"},  {0x45, "FDSQRT"},
    {0x58, "FSABS"},   {0x5A, "FSNEG"},   {0x5C, "FDABS"},   {0x5E, "FDNEG"},
    {0x60, "FSDIV"},   {0x62, "FSADD"},   {0x63, "FSMUL"},   {0x64, "FDDIV"},
    {0x66, "FDADD"},   {0x67, "FDMUL"},   {0x68, "FSSUB"},   {0x6C, "FDSUB"}};
string disasm_F10(int type, int reg) {
    uint16_t extw = FETCH();
    if(!(extw & 1 << 15)) {
        if(!(extw & 1 << 13)) {
            // Fop From EA
            bool rm = extw & 1 << 14;
            int src = extw >> 10 & 7;
            int dst = extw >> 7 & 7;
            int opc = extw & 0x7f;
            std::string src_v;
            if(rm && src == 7) {
                // FMOVECR.X
                return format("FMOVECR.X #0x{:02x}, %FP{}", opc, dst);
            } else if(!rm) {
                src_v = format("%FP{}", src);
            } else {
                src_v = disasm_ea(type, reg, fp_sz[src]);
            }
            char sz = rm ? fp_c[src] : 'X';
            if(opc >= 0x30 && opc < 0x38) {
                return format("FSINCOS.{} {}, %FP{}, %FP{}", sz, src_v, opc & 7,
                              dst);
            } else if(opc == 0x3A) {
                return format("FTST.{} {}", sz, src_v);
            } else {
                return format("{}.{} {}, %FP{}", fop[opc], sz, src_v, dst);
            }
        } else {
            int dst = extw >> 10 & 7;
            int src = extw >> 7 & 7;
            int k = extw & 0x7f;
            if(dst == 3) {
                return format("FMOVE.P %FP{}, {}{{{}}}", src,
                              disasm_ea(type, reg, 12), k);
            } else if(dst == 7) {
                return format("FMOVE.P %FP{}, {}{{%D{}}}", src,
                              disasm_ea(type, reg, 12), k);
            } else {
                return format("FMOVE.{} %FP{}, {}", fp_c[dst], src,
                              disasm_ea(type, reg, fp_sz[dst]));
            }
        }
    } else {
        // FMOVEM
        if(extw & 1 << 14) {
            std::vector<std::string> ccs;
            if(extw & 1 << 12) {
                ccs.push_back("%FPCR");
            }
            if(extw & 1 << 11) {
                ccs.push_back("%FPSR");
            }
            if(extw & 1 << 12) {
                ccs.push_back("%FPIR");
            }
            if(extw & 1 << 13) {
                if(ccs.size() == 1) {
                    return fmt::format("FMOVE {0}, {1}", ccs[0],
                                       disasm_ea(type, reg, 0));
                } else {
                    return fmt::format("FMOVEM {0}, {1}", fmt::join(ccs, "/"),
                                       disasm_ea(type, reg, 0));
                }
            } else {
                if(ccs.size() == 1) {
                    return fmt::format("FMOVE {1}, {0}", ccs[0],
                                       disasm_ea(type, reg, 0));
                } else {
                    return fmt::format("FMOVEM {1}, {0}", fmt::join(ccs, "/"),
                                       disasm_ea(type, reg, 0));
                }
            }
        } else {
            if(extw & 1 << 11) {
                if(extw & 1 << 13) {
                    return format("FMOVEM.X %D{0}, {1}", extw >> 4 & 7,
                                  disasm_ea(type, reg, -12));
                } else {
                    return format("FMOVEM.X %D{1}, {0}", extw >> 4 & 7,
                                  disasm_ea(type, reg, -12));
                }
            } else {
                std::vector<std::pair<int, int>> ranges;
                bool rev = (extw >> 12 & 3) == 2;
                int first = -1;
                for(int i = 0; i < 8; ++i) {
                    if(extw & 1 << (7 - i)) {
                        if(first == -1) {
                            first = rev ? 7 - i : i;
                        }
                    } else if(first != -1) {
                        ranges.emplace_back(first, rev ? 8 - i : i - 1);
                        first = -1;
                    }
                }
                if(first != -1) {
                    ranges.emplace_back(first, rev ? 0 : 7);
                }
                std::vector<string> reg_list;
                for(auto [bgn, end] : ranges) {
                    if(bgn == end) {
                        reg_list.push_back(format("%FP{}", bgn));
                    } else {
                        reg_list.push_back(format("%FP{}-%FP{}", bgn, end));
                    }
                }
                if(extw & 1 << 13) {
                    return format("FMOVEM.X {0}, {1}", fmt::join(reg_list, "/"),
                                  disasm_ea(type, reg, -12));
                } else {
                    return format("FMOVEM.X {1}, {0}", fmt::join(reg_list, "/"),
                                  disasm_ea(type, reg, -12));
                }
            }
        }
    }
}
static const char *fp_cc[] = {
    "F",    "EQ",  "OGT", "OGE", "OLT", "OLE", "OGL", "OR",
    "UN",   "UEQ", "UGT", "UGE", "ULT", "ULE", "NE",  "T",

    "SF",   "SEQ", "GT",  "GE",  "LT",  "LE",  "GL",  "GLE",
    "NGLE", "NGL", "NLE", "NLT", "NGE", "NGT", "SNE", "ST",
};
string disasm_F11(int type, int reg) {
    uint8_t cc = FETCH() & 0x3f;
    if(type == 7) {
        if(reg == 2) {
            return format("FTRAP{} #{:04x}", fp_cc[cc], FETCH());
        } else if(reg == 3) {
            return format("FTRAP{} #{:08x}", fp_cc[cc], FETCH32());
        } else {
            return format("FTRAP{}", fp_cc[cc]);
        }
    } else if(type == 1) {
        int16_t disp = FETCH();
        return format("FDB{} %D{}, #{}", fp_cc[cc], reg, disp);
    } else {
        return format("FS{}, {}", fp_cc[cc], disasm_ea(type, reg, 2));
    }
}
string disasm_F12(uint8_t cc) {
    int16_t imm = FETCH();
    if(cc == 0 && imm == 0) {
        return "FNOP";
    } else {
        return format("FB{} #{}", fp_cc[cc], imm);
    }
}

string disasm_F13(uint8_t cc) {
    int32_t imm = FETCH32();
    return format("FB{} #{}", fp_cc[cc], imm);
}
static const char *push_target[] = {"", "DC", "IC", "BC"};
string disasm_F2X(int opx, int type, int reg) {
    switch(opx) {
    case 1:
    case 2:
    case 3:
        switch(type) {
        case 1:
            return format("CINVL {}, (%A{})", push_target[opx], reg);
        case 2:
            return format("CINVP {}, (%A{})", push_target[opx], reg);
        case 3:
            return format("CINVA {}", push_target[opx]);
        case 5:
            return format("CPUSHL {}, (%A{})", push_target[opx], reg);
        case 6:
            return format("CPUSHP {}, (%A{})", push_target[opx], reg);
        case 7:
            return format("CPUSHA {}", push_target[opx]);
        }
        break;
    case 4:
        switch(type) {
        case 0:
            return format("PFULSHN (%A{})", reg);
        case 1:
            return format("PFULSH (%A{})", reg);
        case 2:
            return "PFLUSHAN";
        case 3:
            return "PFLUSHA";
        }
        break;
    case 5:
        switch(type) {
        case 1:
            return format("PTESTW (%A{})", reg);
        case 5:
            return format("PTESTR (%A{})", reg);
        }
    }
    return "#F-ILLEGAL";
}

string disasm_F30(int type, int reg) {
    switch(type) {
    case 0:
        return format("MOVE16 (%A{0})+, (0x{1:08x})", reg, FETCH32());
    case 1:
        return format("MOVE16 (0x{1:08x}), (%A{0})+", reg, FETCH32());
    case 2:
        return format("MOVE16 (%A{0}), (0x{1:08x})", reg, FETCH32());
    case 3:
        return format("MOVE16 (0x{1:08x}), (%A{0})", reg, FETCH32());
    case 4:
        return format("MOVE16 (%A{})+, (%A{})+", reg, FETCH() >> 12 & 0xf);
    }
    return "#F-ILLEGAL";
}
} // namespace DISASM
string disasm() {
    uint16_t wd = FETCH();
    unsigned int dn = wd >> 9 & 7;
    unsigned int type = wd >> 3 & 7;
    unsigned int reg = wd & 7;
    unsigned int opx = wd >> 6 & 7;
    switch(wd >> 12) {
    case 0x0:
        switch(opx) {
        case 0:
            return DISASM::disasm_0X0(dn, type, reg);
        case 1:
            return DISASM::disasm_0X1(dn, type, reg);
        case 2:
            return DISASM::disasm_0X2(dn, type, reg);
        case 3:
            return DISASM::disasm_0X3(dn, type, reg);
        case 4:
            return DISASM::disasm_0X4(dn, type, reg);
        case 5:
            return DISASM::disasm_0X5(dn, type, reg);
        case 6:
            return DISASM::disasm_0X6(dn, type, reg);
        case 7:
            return DISASM::disasm_0X7(dn, type, reg);
        }
        __builtin_unreachable();
    case 0x1:
        return DISASM::disasm_1XX((wd >> 6) & 7, dn, type, reg);
    case 0x2:
        return DISASM::disasm_2XX((wd >> 6) & 7, dn, type, reg);
    case 0x3:
        return DISASM::disasm_3XX((wd >> 6) & 7, dn, type, reg);
    case 0x4:
        switch(opx) {
        case 0:
            return DISASM::disasm_4X0(dn, type, reg);
        case 1:
            return DISASM::disasm_4X1(dn, type, reg);
        case 2:
            return DISASM::disasm_4X2(dn, type, reg);
        case 3:
            return DISASM::disasm_4X3(dn, type, reg);
        case 4:
            return DISASM::disasm_4X4(dn, type, reg);
        case 5:
            return "#UNDEF";
        case 6:
            return DISASM::disasm_4X6(dn, type, reg);
        case 7:
            return DISASM::disasm_4X7(dn, type, reg);
        }
        __builtin_unreachable();
    case 0x5:
        switch(opx) {
        case 0:
            return DISASM::disasm_5X0(dn, type, reg);
        case 1:
            return DISASM::disasm_5X1(dn, type, reg);
        case 2:
            return DISASM::disasm_5X2(dn, type, reg);
        case 4:
            return DISASM::disasm_5X4(dn, type, reg);
        case 5:
            return DISASM::disasm_5X5(dn, type, reg);
        case 6:
            return DISASM::disasm_5X6(dn, type, reg);
        case 3:
        case 7:
            return DISASM::disasm_05C3(wd >> 8 & 0xf, type, reg);
        }
        __builtin_unreachable();
    case 0x6:
        return DISASM::disasm_6C(wd >> 8 & 0xf, wd & 0xff);
    case 0x7:
        return DISASM::disasm_7XX(dn, wd & 0xff);
    case 0x8:
        switch(opx) {
        case 0:
            return DISASM::disasm_8X0(dn, type, reg);
        case 1:
            return DISASM::disasm_8X1(dn, type, reg);
        case 2:
            return DISASM::disasm_8X2(dn, type, reg);
        case 3:
            return DISASM::disasm_8X3(dn, type, reg);
        case 4:
            return DISASM::disasm_8X4(dn, type, reg);
        case 5:
            return DISASM::disasm_8X5(dn, type, reg);
        case 6:
            return DISASM::disasm_8X6(dn, type, reg);
        case 7:
            return DISASM::disasm_8X7(dn, type, reg);
        }
        __builtin_unreachable();
    case 0x9:
        switch(opx) {
        case 0:
            return DISASM::disasm_9X0(dn, type, reg);
        case 1:
            return DISASM::disasm_9X1(dn, type, reg);
        case 2:
            return DISASM::disasm_9X2(dn, type, reg);
        case 3:
            return DISASM::disasm_9X3(dn, type, reg);
        case 4:
            return DISASM::disasm_9X4(dn, type, reg);
        case 5:
            return DISASM::disasm_9X5(dn, type, reg);
        case 6:
            return DISASM::disasm_9X6(dn, type, reg);
        case 7:
            return DISASM::disasm_9X7(dn, type, reg);
        }
        __builtin_unreachable();
    case 0xA:
        return format("SYSCALL #0x{:x}", wd);
    case 0xB:
        switch(opx) {
        case 0:
            return DISASM::disasm_BX0(dn, type, reg);
        case 1:
            return DISASM::disasm_BX1(dn, type, reg);
        case 2:
            return DISASM::disasm_BX2(dn, type, reg);
        case 3:
            return DISASM::disasm_BX3(dn, type, reg);
        case 4:
            return DISASM::disasm_BX4(dn, type, reg);
        case 5:
            return DISASM::disasm_BX5(dn, type, reg);
        case 6:
            return DISASM::disasm_BX6(dn, type, reg);
        case 7:
            return DISASM::disasm_BX7(dn, type, reg);
        }
        __builtin_unreachable();
    case 0xC:
        switch(opx) {
        case 0:
            return DISASM::disasm_CX0(dn, type, reg);
        case 1:
            return DISASM::disasm_CX1(dn, type, reg);
        case 2:
            return DISASM::disasm_CX2(dn, type, reg);
        case 3:
            return DISASM::disasm_CX3(dn, type, reg);
        case 4:
            return DISASM::disasm_CX4(dn, type, reg);
        case 5:
            return DISASM::disasm_CX5(dn, type, reg);
        case 6:
            return DISASM::disasm_CX6(dn, type, reg);
        case 7:
            return DISASM::disasm_CX7(dn, type, reg);
        }
        __builtin_unreachable();
    case 0xD:
        switch(opx) {
        case 0:
            return DISASM::disasm_DX0(dn, type, reg);
        case 1:
            return DISASM::disasm_DX1(dn, type, reg);
        case 2:
            return DISASM::disasm_DX2(dn, type, reg);
        case 3:
            return DISASM::disasm_DX3(dn, type, reg);
        case 4:
            return DISASM::disasm_DX4(dn, type, reg);
        case 5:
            return DISASM::disasm_DX5(dn, type, reg);
        case 6:
            return DISASM::disasm_DX6(dn, type, reg);
        case 7:
            return DISASM::disasm_DX7(dn, type, reg);
        }
        __builtin_unreachable();
    case 0xE:
        switch(opx) {
        case 0:
        case 1:
        case 2:
            return DISASM::disasm_EX_SHIFT(false, opx, dn, type, reg);
        case 3:
            return DISASM::disasm_EX3(dn, type, reg);
        case 4:
        case 5:
        case 6:
            return DISASM::disasm_EX_SHIFT(true, opx & 3, dn, type, reg);
        case 7:
            return DISASM::disasm_EX7(dn, type, reg);
        }
        __builtin_unreachable();
    case 0xF:
        switch(dn) {
        case 1:
            switch(opx) {
            case 0:
                return DISASM::disasm_F10(type, reg);
            case 1:
                return DISASM::disasm_F11(type, reg);
            case 2:
                return DISASM::disasm_F12(type << 3 | reg);
            case 3:
                return DISASM::disasm_F13(type << 3 | reg);
            case 4:
                return format("FSAVE {}", disasm_ea(type, reg, 0));
            case 5:
                return format("FRESTORE {}", disasm_ea(type, reg, 0));
            default:
                return "#F-INVALID";
            }
        case 2:
            return DISASM::disasm_F2X(opx, type, reg);
        case 3:
            if(opx == 0) {
                return DISASM::disasm_F30(type, reg);
            }
            break;
        }
        return "#F-INVALID";
    }
    return "#INVALID";
}

