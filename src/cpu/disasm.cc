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

struct restore_pc {
    uint32_t pc;
    ~restore_pc() { cpu.PC = pc; }
};
std::string Rn(int reg) {
    return fmt::format("%{}{}", reg & 8 ? 'A' : 'D', reg & 7);
}
std::string disasm_parseExt(int reg) {

    uint16_t next = FETCH();
    int ri = next >> 12 & 15;
    bool ri_w = next & 1 << 11;
    int ri_c = next >> 9 & 3;
    std::string base = reg >= 0 ? fmt::format("%A{}", reg) : "%PC";
    std::string rn_v =
        fmt::format("{}{}*{}", Rn(ri), ri_w ? ".W" : "", 1 << ri_c);
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
                uint16_t exp = FETCH32() >> 16;
                uint64_t frac = FETCH32();
                frac = frac << 32 | FETCH32();
                load_fpX(frac, exp);
                char *p;
                mpfr_asprintf(&p, "%RNf", cpu.fp_tmp);
                auto ss = fmt::format("#{}", p);
                mpfr_free_str(p);
                return ss;
            }
            case 12: // P
            {
                load_fpP(cpu.PC);
                cpu.PC += 12;
                char *p;
                mpfr_asprintf(&p, "%RNf", cpu.fp_tmp);
                auto ss = fmt::format("#{}", p);
                mpfr_free_str(p);
                return ss;
            }
            }
        }
    }
    return "?";
}
namespace DISASM {
std::string disasm_00X0(int dn, int type, int reg) {
    uint16_t imm = FETCH();
    switch(dn) {
    case 0:
        if(type == 7 && reg == 4) {
            return fmt::format("ORI.B #0x{:02x}, %CCR", imm & 0xff);
        } else {
            return fmt::format("ORI.B #0x{:02x}, {}", imm & 0xff,
                               disasm_ea(type, reg, 1));
        }
    case 1:
        if(type == 7 && reg == 4) {
            return fmt::format("ANDI.B #0x{:02x}, %CCR", imm & 0xff);
        } else {
            return fmt::format("ANDI.B #0x{:02x}, {}", imm & 0xff,
                               disasm_ea(type, reg, 1));
        }
    case 2:
        return fmt::format("SUBI.B #{}, {}", imm & 0xff,
                           disasm_ea(type, reg, 1));
    case 3:
        return fmt::format("ADDI.B #{}, {}", imm & 0xff,
                           disasm_ea(type, reg, 1));
    case 4:
        imm &= 0xff;
        return fmt::format("BTST.{} #{}, {}", type == 0 ? 'L' : 'B',
                           imm ? imm : 8, disasm_ea(type, reg, 1));
    case 5:
        if(type == 7 && reg == 4) {
            return fmt::format("EORI.B #0x{:02x}, %CCR", imm & 0xff);
        } else {
            return fmt::format("EORI.B #0x{:02x}, {}", imm & 0xff,
                               disasm_ea(type, reg, 1));
        }
    case 6:
        return fmt::format("CMPI.B #{}, {}", imm & 0xff,
                           disasm_ea(type, reg, 1));
    case 7:
        if(imm & 1 << 11) {
            return fmt::format("MOVES.B {1}, {0}", disasm_ea(type, reg, 1),
                               Rn(imm >> 12 & 0x1f));

        } else {
            return fmt::format("MOVES.B {0}, {1}", disasm_ea(type, reg, 1),
                               Rn(imm >> 12 & 0x1f));
        }
    default:
        __builtin_unreachable();
    }
}

std::string disasm_00X1(int dn, int type, int reg) {
    uint16_t imm = FETCH();
    switch(dn) {
    case 0:
        if(type == 7 && reg == 4) {
            return fmt::format("ORI.W #0x{:04x}, %SR", imm);
        } else {
            return fmt::format("ORI.W #0x{:04x}, {}", imm,
                               disasm_ea(type, reg, 2));
        }
    case 1:
        if(type == 7 && reg == 4) {
            return fmt::format("ANDI.W #0x{:04x}, %SR", imm);
        } else {
            return fmt::format("ANDI.W #0x{:04x}, {}", imm,
                               disasm_ea(type, reg, 2));
        }
    case 2:
        return fmt::format("SUBI.W #{}, {}", imm, disasm_ea(type, reg, 2));
    case 3:
        return fmt::format("ADDI.W #{}, {}", imm, disasm_ea(type, reg, 2));
    case 4:
        imm &= 0xff;
        return fmt::format("BCHG.{} #{}, {}", type == 0 ? 'L' : 'B',
                           imm ? imm : 8, disasm_ea(type, reg, 1));
    case 5:
        if(type == 7 && reg == 4) {
            return fmt::format("EORI.W #0x{:04x}, %SR", imm);
        } else {
            return fmt::format("EORI.W #0x{:04x}, {}", imm,
                               disasm_ea(type, reg, 2));
        }
    case 6:
        return fmt::format("CMPI.W #{}, {}", imm, disasm_ea(type, reg, 2));
    case 7:
        if(imm & 1 << 11) {
            return fmt::format("MOVES.W {1}, {0}", disasm_ea(type, reg, 1),
                               Rn(imm >> 12 & 0x1f));

        } else {
            return fmt::format("MOVES.W {0}, {1}", disasm_ea(type, reg, 1),
                               Rn(imm >> 12 & 0x1f));
        }
    default:
        __builtin_unreachable();
    }
}

std::string disasm_00X2(int dn, int type, int reg) {
    switch(dn) {
    case 0: {
        uint32_t imm = FETCH32();
        return fmt::format("ORI.L #0x{:08x}, {}", imm, disasm_ea(type, reg, 4));
    }
    case 1: {
        uint32_t imm = FETCH32();
        return fmt::format("ANDI.L #0x{:08x}, {}", imm,
                           disasm_ea(type, reg, 4));
    }
    case 2: {
        uint32_t imm = FETCH32();
        return fmt::format("SUBI.L #{}, {}", imm, disasm_ea(type, reg, 4));
    }
    case 3: {
        uint32_t imm = FETCH32();
        return fmt::format("ADDI.L #{}, {}", imm, disasm_ea(type, reg, 4));
    }
    case 4: {
        uint16_t imm = FETCH();
        return fmt::format("BCLR.{} #{}, {}", type == 0 ? 'L' : 'B',
                           imm ? imm : 8, disasm_ea(type, reg, 1));
    }
    case 5: {
        uint32_t imm = FETCH32();
        return fmt::format("EORI.L #0x{:08x}, {}", imm,
                           disasm_ea(type, reg, 4));
    }
    case 6: {
        uint32_t imm = FETCH32();
        return fmt::format("CMPI.L #{}, {}", imm, disasm_ea(type, reg, 4));
    }
    case 7: {
        uint16_t imm = FETCH();
        if(imm & 1 << 11) {
            return fmt::format("MOVES.L {1}, {0}", disasm_ea(type, reg, 1),
                               Rn(imm >> 12 & 0x1f));

        } else {
            return fmt::format("MOVES.L {0}, {1}", disasm_ea(type, reg, 1),
                               Rn(imm >> 12 & 0x1f));
        }
    }
    default:
        __builtin_unreachable();
    }
}
const char *sz_array[3] = {
    "B",
    "W",
    "L",
};

std::string disasm_00X3(int dn, int type, int reg) {
    switch(dn) {
    case 0:
    case 1:
    case 2: {
        uint16_t extw = FETCH();
        int rn = extw >> 12 & 0xf;
        std::string ea = disasm_ea(type, reg, 0);
        if(extw & 1 << 11) {
            return fmt::format("CHK2.{} {}, {}", sz_array[dn], ea, Rn(rn));
        } else {
            return fmt::format("CMP2.{} {}, {}", sz_array[dn], ea, Rn(rn));
        }
    }
    case 4: {
        uint16_t imm = FETCH();
        return fmt::format("BSET.{} #{}, {}", type == 0 ? 'L' : 'B',
                           imm ? imm : 8, disasm_ea(type, reg, 1));
    }
    case 5: {
        uint16_t extw = FETCH();
        int du = extw >> 6 & 7;
        int dc = extw & 7;
        return fmt::format("CAS.B %D{}, %D{}, {}", dc, du,
                           disasm_ea(type, reg, 1));
    }
    case 6:
    case 7:
        if(type == 7 && reg == 4) {
            uint16_t extw1 = FETCH();
            uint16_t extw2 = FETCH();
            return fmt::format("CAS2.{} %D{}:%D{}, %D{}:%D{}, ({}:{})",
                               dn == 6 ? 'W' : 'L', extw1 & 7, extw2 & 7,
                               extw1 >> 6 & 7, extw2 >> 6 & 7,
                               Rn(extw1 >> 12 & 15), Rn(extw2 >> 12 & 15));

        } else {
            uint16_t extw = FETCH();
            int du = extw >> 6 & 7;
            int dc = extw & 7;
            return fmt::format("CAS.{} %D{}, %D{}, {}", dn == 6 ? 'W' : 'L', dc,
                               du, disasm_ea(type, reg, 1));
        }
    default:
        __builtin_unreachable();
    }
}
std::string disasm_00X4(int dn, int type, int reg) {
    if(type == 1) {
        int16_t imm = FETCH();
        return fmt::format("MOVEP.W (#{0}, %A{1}), %D{2}", imm, reg, dn);
    } else {
        return fmt::format("BTST.{} %D{}, {}", type == 0 ? 'L' : 'B', dn,
                           disasm_ea(type, reg, 1));
    }
}

std::string disasm_00X5(int dn, int type, int reg) {
    if(type == 1) {
        int16_t imm = FETCH();
        return fmt::format("MOVEP.L (#{0}, %A{1}), %D{2}", imm, reg, dn);
    } else {
        return fmt::format("BCHG.{} %D{}, {}", type == 0 ? 'L' : 'B', dn,
                           disasm_ea(type, reg, 1));
    }
}

std::string disasm_00X6(int dn, int type, int reg) {
    if(type == 1) {
        int16_t imm = FETCH();
        return fmt::format("MOVEP.W %D{2}, (#{0}, %A{1})", imm, reg, dn);
    } else {
        return fmt::format("BCLR.{} %D{}, {}", type == 0 ? 'L' : 'B', dn,
                           disasm_ea(type, reg, 1));
    }
}

std::string disasm_00X7(int dn, int type, int reg) {
    if(type == 1) {
        int16_t imm = FETCH();
        return fmt::format("MOVEP.L %D{2}, (#{0}, %A{1})", imm, reg, dn);
    } else {
        return fmt::format("BSET.{} %D{}, {}", type == 0 ? 'L' : 'B', dn,
                           disasm_ea(type, reg, 1));
    }
}

std::string disasm_01XX(int dn_type, int dn, int type, int reg) {
    auto src = disasm_ea(type, reg, 1);
    auto dst = disasm_ea(dn_type, dn, 1);
    return fmt::format("MOVE.B {}, {}", src, dst);
}

std::string disasm_02XX(int dn_type, int dn, int type, int reg) {
    auto src = disasm_ea(type, reg, 4);
    auto dst = disasm_ea(dn_type, dn, 4);
    return fmt::format("MOVE{}.L {}, {}", dn_type == 1 ? "A" : "", src, dst);
}
std::string disasm_03XX(int dn_type, int dn, int type, int reg) {
    auto src = disasm_ea(type, reg, 2);
    auto dst = disasm_ea(dn_type, dn, 2);
    return fmt::format("MOVE{}.W {}, {}", dn_type == 1 ? "A" : "", src, dst);
}
static std::unordered_map<int, const char *> CRMAP{
    {0x000, "SFC"},   {0x001, "DFC"},  {0x002, "CACR"}, {0x003, "TC"},
    {0x004, "ITT0"},  {0x005, "ITT1"}, {0x006, "DTT0"}, {0x007, "DTT1"},
    {0x800, "USP"},   {0x801, "VBR"},  {0x803, "MSP"},  {0x804, "ISP"},
    {0x805, "MMUSR"}, {0x806, "URP"},  {0x807, "SRP"},
};
std::string disasm_04X0(int dn, int type, int reg) {
    switch(dn) {
    case 0:
        return fmt::format("NEGX.B {}", disasm_ea(type, reg, 1));
    case 1:
        return fmt::format("CLR.B {}", disasm_ea(type, reg, 1));
    case 2:
        return fmt::format("NEG.B {}", disasm_ea(type, reg, 1));
    case 3:
        return fmt::format("NOT.B {}", disasm_ea(type, reg, 1));
    case 4:
        if(type == 1) {
            int32_t imm = FETCH32();
            return fmt::format("LINK.L %A{}, #{}", reg, imm);
        } else {
            return fmt::format("NBCD {}", disasm_ea(type, reg, 1));
        }
    case 5:
        return fmt::format("TST.B {}", disasm_ea(type, reg, 1));
    case 6: {
        uint16_t extw = FETCH();
        int dl = extw >> 12 & 7;
        int dh = extw & 7;
        bool sg = extw & 1 << 11;
        bool lw = extw & 1 << 10;
        if(lw) {
            return fmt::format("MUL{}.L {}, %D{}-%D{}", sg ? 'S' : 'U',
                               disasm_ea(type, reg, 4), dh, dl);
        } else {
            return fmt::format("MUL{}.L {}, %D{}", sg ? 'S' : 'U',
                               disasm_ea(type, reg, 4), dl);
        }
    }
    case 7:
        return "#UNDEF";
    default:
        __builtin_unreachable();
    }
}
std::string disasm_04X1(int dn, int type, int reg) {
    switch(dn) {
    case 0:
        return fmt::format("NEGX.W {}", disasm_ea(type, reg, 2));
    case 1:
        return fmt::format("CLR.W {}", disasm_ea(type, reg, 2));
    case 2:
        return fmt::format("NEG.W {}", disasm_ea(type, reg, 2));
    case 3:
        return fmt::format("NOT.W {}", disasm_ea(type, reg, 2));
    case 4:
        if(type == 0) {
            return fmt::format("SWAP %D{}", reg);
        } else if(type == 1) {
            return "BKPT";
        } else {
            return fmt::format("PEA {}", disasm_ea(type, reg, 0));
        }
    case 5:
        return fmt::format("TST.W {}", disasm_ea(type, reg, 2));
    case 6: {
        uint16_t extw = FETCH();
        int dq = extw >> 12 & 7;
        int dr = extw & 7;
        bool sg = extw & 1 << 11;
        bool lw = extw & 1 << 10;
        if(lw) {
            return fmt::format("DIV{}.L {}, %D{}:%D{}", sg ? 'S' : 'U',
                               disasm_ea(type, reg, 4), dr, dq);
        } else {
            if(dr != dq) {
                return fmt::format("DIV{}L.L {}, %D{}:%D{}", sg ? 'S' : 'U',
                                   disasm_ea(type, reg, 4), dr, dq);
            } else {
                return fmt::format("DIV{}.L {}, %D{}", sg ? 'S' : 'U',
                                   disasm_ea(type, reg, 4), dq);
            }
        }
    }
    case 7:
        switch(type) {
        case 0:
        case 1:
            return fmt::format("TRAP #{}", type << 3 | reg);
        case 2: {
            int16_t imm = FETCH();
            return fmt::format("LINK.W %A{}, #{}", reg, imm);
        }
        case 3:
            return fmt::format("UNLK %A{}", reg);
        case 4:
            return fmt::format("MOVE %A{}, %USP", reg);
        case 5:
            return fmt::format("MOVE %USP, %A{}", reg);
        case 6:
            switch(reg) {
            case 0:
                return "RESET";
            case 1:
                return "NOP";
            case 2:
                return fmt::format("STOP #{:04x}", FETCH());
            case 3:
                return "RTE";
            case 4:
                return fmt::format("RTD #{}", static_cast<int16_t>(FETCH()));
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
                return fmt::format("MOVEC %{0}, {1}", CRMAP[extw], Rn(rn));
            } else {
                return fmt::format("MOVEC {1}, %{0}", CRMAP[extw], Rn(rn));
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
    std::vector<std::string> reg_list;
    for(auto [bgn, end] : x) {
        if(bgn == end) {
            reg_list.push_back(Rn(rev ? 15 - bgn : bgn));
        } else {
            if(rev) {
                reg_list.push_back(
                    fmt::format("{}-{}", Rn(15 - bgn), Rn(15 - end)));
            } else {
                reg_list.push_back(fmt::format("{}-{}", Rn(bgn), Rn(end)));
            }
        }
    }
    return reg_list;
}

std::string disasm_04X2(int dn, int type, int reg) {
    switch(dn) {
    case 0:
        return fmt::format("NEGX.L {}", disasm_ea(type, reg, 4));
    case 1:
        return fmt::format("CLR.L {}", disasm_ea(type, reg, 4));
    case 2:
        return fmt::format("NEG.L {}", disasm_ea(type, reg, 4));
    case 3:
        return fmt::format("NOT.L {}", disasm_ea(type, reg, 4));
    case 4:
        if(type == 0) {
            return fmt::format("EXT.W %D{}", reg);
        } else {
            auto reglist = get_rnlist(FETCH(), type == 4);
            return fmt::format("MOVEM.W {}, {}", fmt::join(reglist, "/"),
                               disasm_ea(type, reg, 0));
        }
    case 5:
        return fmt::format("TST.L {}", disasm_ea(type, reg, 4));
    case 6: {
        auto reglist = get_rnlist(FETCH(), false);
        return fmt::format("MOVEM.W {1}, {0}", fmt::join(reglist, "/"),
                           disasm_ea(type, reg, 0));
    }
    case 7:
        return fmt::format("JSR {}", disasm_ea(type, reg, 0));
    default:
        __builtin_unreachable();
    }
}

std::string disasm_04X3(int dn, int type, int reg) {
    switch(dn) {
    case 0:
        return fmt::format("MOVE.W %SR, {}", disasm_ea(type, reg, 2));
    case 1:
        return fmt::format("MOVE.W %CCR, {}", disasm_ea(type, reg, 2));
    case 2:
        return fmt::format("MOVE.W {}, %CCR", disasm_ea(type, reg, 2));
    case 3:
        return fmt::format("MOVE.W {}, %SR", disasm_ea(type, reg, 2));
    case 4:
        if(type == 0) {
            return fmt::format("EXT.L %D{}", reg);
        } else {
            auto reglist = get_rnlist(FETCH(), type == 4);
            return fmt::format("MOVEM.L {}, {}", fmt::join(reglist, "/"),
                               disasm_ea(type, reg, 0));
        }
    case 5:
        return fmt::format("TAS {}", disasm_ea(type, reg, 1));
    case 6: {
        auto reglist = get_rnlist(FETCH(), false);
        return fmt::format("MOVEM.L {1}, {0}", fmt::join(reglist, "/"),
                           disasm_ea(type, reg, 0));
    }
    case 7:
        return fmt::format("JSR {}", disasm_ea(type, reg, 0));
    default:
        __builtin_unreachable();
    }
}
} // namespace DISASM
std::string disasm() {
    uint32_t pc = cpu.PC;
    restore_pc p(pc);
    uint16_t wd = FETCH();
    unsigned int dn = wd >> 9 & 7;
    unsigned int type = wd >> 3 & 7;
    unsigned int reg = wd & 7;
    switch((wd >> 9 & 01770) | ((wd >> 6) & 7)) {
    case 0000:
        return DISASM::disasm_00X0(dn, type, reg);
    case 0001:
        return DISASM::disasm_00X1(dn, type, reg);
    case 0002:
        return DISASM::disasm_00X2(dn, type, reg);
    case 0003:
        return DISASM::disasm_00X3(dn, type, reg);
    case 0004:
        return DISASM::disasm_00X4(dn, type, reg);
    case 0005:
        return DISASM::disasm_00X5(dn, type, reg);
    case 0006:
        return DISASM::disasm_00X6(dn, type, reg);
    case 0007:
        return DISASM::disasm_00X7(dn, type, reg);
    case 0010:
    case 0012:
    case 0013:
    case 0014:
    case 0015:
    case 0016:
    case 0017:
        return DISASM::disasm_01XX((wd >> 6) & 7, dn, type, reg);
    case 0020:
    case 0021:
    case 0022:
    case 0023:
    case 0024:
    case 0025:
    case 0026:
    case 0027:
        return DISASM::disasm_02XX((wd >> 6) & 7, dn, type, reg);
    case 0030:
    case 0031:
    case 0032:
    case 0033:
    case 0034:
    case 0035:
    case 0036:
    case 0037:
        return DISASM::disasm_03XX((wd >> 6) & 7, dn, type, reg);
    case 0040:
        return DISASM::disasm_04X0(dn, type, reg);
    case 0041:
        return DISASM::disasm_04X1(dn, type, reg);
    case 0042:
        return DISASM::disasm_04X2(dn, type, reg);
    case 0043:
        return DISASM::disasm_04X3(dn, type, reg);
    }
    return "";
}
#if 0

namespace OP {


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