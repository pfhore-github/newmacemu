
#include "68040.hpp"
#include "bus.hpp"
#include "exception.hpp"
#include "memory.hpp"
#include "inline.hpp"
#include <memory>
#include <utility>
void bus_reset();
void do_rte();
uint32_t Get_TTR_t(const Cpu::TTR_t &x);
void Set_TTR_t(Cpu::TTR_t &x, uint32_t v);
uint32_t ea_getaddr(int type, int reg, int sz, bool rw);
#ifdef CI
extern volatile bool testing;
#endif

uint8_t GetCCR(const Cpu& cpu) {
    return cpu.X << 4 | cpu.N << 3 | cpu.Z << 2 | cpu.V << 1 | cpu.C;
}
void SetCCR(Cpu& cpu, uint8_t v) {
    cpu.X = v >> 4 & 1;
    cpu.N = v >> 3 & 1;
    cpu.Z = v >> 2 & 1;
    cpu.V = v >> 1 & 1;
    cpu.C = v & 1;
}
uint16_t GetSR(const Cpu& cpu) {
    return GetCCR(cpu) | cpu.I << 8 | cpu.M << 12 | cpu.S << 13 | cpu.T << 14;
}
void SaveSP() {
    if(cpu.S)
        if(cpu.M)
            cpu.MSP = cpu.A[7];
        else
            cpu.ISP = cpu.A[7];
    else
        cpu.USP = cpu.A[7];
}
void LoadSP() {
    if(cpu.S)
        if(cpu.M)
            cpu.A[7] = cpu.MSP;
        else
            cpu.A[7] = cpu.ISP;
    else
        cpu.A[7] = cpu.USP;
}
void SetSR(Cpu& cpu, uint16_t v) {
    SaveSP();
    SetCCR(cpu, v);
    cpu.I = v >> 8 & 7;
    cpu.M = v >> 12 & 1;
    cpu.S = v >> 13 & 1;
    cpu.T = v >> 14 & 3;
    LoadSP();
}

namespace OP {
#ifdef CI
void test_exit(uint16_t) { testing = false; }
#endif
void ori_b(uint16_t op) {
    uint8_t v = FETCH();
    ea_writeB(TYPE(op), REG(op), OR_B(ea_readB(TYPE(op), REG(op), true), v));
}

void ori_w(uint16_t op) {
    uint16_t v = FETCH();
    ea_writeW(TYPE(op), REG(op), OR_W(ea_readW(TYPE(op), REG(op), true), v));
}

void ori_l(uint16_t op) {
    uint32_t v = FETCH32();
    ea_writeL(TYPE(op), REG(op), OR_L(ea_readL(TYPE(op), REG(op), true), v));
}

void ori_b_ccr(uint16_t) {
    uint8_t v = FETCH();
    SetCCR(cpu, GetCCR(cpu) | v);
}

void ori_w_sr(uint16_t) {
    PRIV_CHECK();
    uint16_t v = FETCH();
    SetSR(cpu, GetSR(cpu) | v);
    TRACE_BRANCH();
}

void andi_b(uint16_t op) {
    uint8_t v = FETCH();
    ea_writeB(TYPE(op), REG(op), AND_B(ea_readB(TYPE(op), REG(op), true), v));
}

void andi_w(uint16_t op) {
    uint16_t v = FETCH();
    ea_writeW(TYPE(op), REG(op), AND_W(ea_readW(TYPE(op), REG(op), true), v));
}

void andi_l(uint16_t op) {
    uint32_t v = FETCH32();
    ea_writeL(TYPE(op), REG(op), AND_L(ea_readL(TYPE(op), REG(op), true), v));
}

void andi_b_ccr(uint16_t) {
    uint8_t v = FETCH();
    SetCCR(cpu, GetCCR(cpu) & v);
}

void andi_w_sr(uint16_t) {
    PRIV_CHECK();
    uint16_t v = FETCH();
    SetSR(cpu, GetSR(cpu) & v);
    TRACE_BRANCH();
}

void eori_b(uint16_t op) {
    uint8_t v = FETCH();
    ea_writeB(TYPE(op), REG(op), XOR_B(ea_readB(TYPE(op), REG(op),true), v));
}

void eori_w(uint16_t op) {
    uint16_t v = FETCH();
    ea_writeW(TYPE(op), REG(op), XOR_W(ea_readW(TYPE(op), REG(op),true), v));
}

void eori_l(uint16_t op) {
    uint32_t v = FETCH32();
    ea_writeL(TYPE(op), REG(op), XOR_L(ea_readL(TYPE(op), REG(op),true), v));
}

void eori_b_ccr(uint16_t) {
    uint8_t v = FETCH();
    SetCCR(cpu, GetCCR(cpu) ^ v);
}

void eori_w_sr(uint16_t) {
    PRIV_CHECK();
    uint16_t v = FETCH();
    SetSR(cpu, GetSR(cpu) ^ v);
    TRACE_BRANCH();
}

void subi_b(uint16_t op) {
    uint8_t v = FETCH();
    ea_writeB(TYPE(op), REG(op), SUB_B(ea_readB(TYPE(op), REG(op),true), v));
}

void subi_w(uint16_t op) {
    uint16_t v = FETCH();
    ea_writeW(TYPE(op), REG(op), SUB_W(ea_readW(TYPE(op), REG(op),true), v));
}

void subi_l(uint16_t op) {
    uint32_t v = FETCH32();
    ea_writeL(TYPE(op), REG(op), SUB_L(ea_readL(TYPE(op), REG(op),true), v));
}

void addi_b(uint16_t op) {
    uint8_t v = FETCH();
    ea_writeB(TYPE(op), REG(op), ADD_B(ea_readB(TYPE(op), REG(op),true), v));
}

void addi_w(uint16_t op) {
    uint16_t v = FETCH();
    ea_writeW(TYPE(op), REG(op), ADD_W(ea_readW(TYPE(op), REG(op),true), v));
}

void addi_l(uint16_t op) {
    uint32_t v = FETCH32();
    ea_writeL(TYPE(op), REG(op), ADD_L(ea_readL(TYPE(op), REG(op),true), v));
}

void cmpi_b(uint16_t op) {
    uint8_t v = FETCH();
    CMP_B(ea_readB(TYPE(op), REG(op)), v);
}

void cmpi_w(uint16_t op) {
    uint16_t v = FETCH();
    CMP_W(ea_readW(TYPE(op), REG(op)), v);
}

void cmpi_l(uint16_t op) {
    uint32_t v = FETCH32();
    CMP_L(ea_readL(TYPE(op), REG(op)), v);
}

void cmp2_chk2_b(uint16_t op) {
    uint16_t nextop = FETCH();
    bool s = nextop & 1 << 15;
    int rn = nextop >> 12 & 7;
    bool chk2 = nextop & 1 << 11;
    cpu.EA = ea_getaddr(TYPE(op), REG(op), 1);
    if(s) {
        CMP2_SB(cpu.A[rn], cpu.EA);
    } else {
        CMP2_B(cpu.D[rn], cpu.EA);
    }
    if(chk2 && cpu.C) {
        CHK_ERROR();
    }
}

void cmp2_chk2_w(uint16_t op) {
    uint16_t nextop = FETCH();
    bool s = nextop & 1 << 15;
    int rn = nextop >> 12 & 7;
    bool chk2 = nextop & 1 << 11;
    cpu.EA = ea_getaddr(TYPE(op), REG(op), 2);
    if(s) {
        CMP2_SW(cpu.A[rn], cpu.EA);
    } else {
        CMP2_W(cpu.D[rn], cpu.EA);
    }
    if(chk2 && cpu.C) {
        CHK_ERROR();
    }
}

void cmp2_chk2_l(uint16_t op) {
    uint16_t nextop = FETCH();
    bool s = nextop & 1 << 15;
    int rn = nextop >> 12 & 7;
    bool chk2 = nextop & 1 << 11;
    cpu.EA = ea_getaddr(TYPE(op), REG(op), 4);
    if(s) {
        CMP2_SL(cpu.A[rn], cpu.EA);
    } else {
        CMP2_L(cpu.D[rn], cpu.EA);
    }
    if(chk2 && cpu.C) {
        CHK_ERROR();
    }
}

void btst_l_i(uint16_t op) {
    int pos = FETCH() & 31;
    BTST_L(cpu.D[REG(op)], pos);
}

void btst_b_i(uint16_t op) {
    int pos = FETCH() & 7;
    cpu.EA = ea_getaddr(TYPE(op), REG(op), 1);
    BTST_B(ReadB(cpu.EA), pos);
}
void btst_b_i_imm(uint16_t) {
    int pos = FETCH() & 7;
    uint8_t imm = FETCH();
    BTST_B(imm, pos);
}

void btst_l_r(uint16_t op) {
    BTST_L(cpu.D[REG(op)], cpu.D[DN(op)] & 31);
}

void btst_b_r(uint16_t op) {
    cpu.EA = ea_getaddr(TYPE(op), REG(op), 1);
    BTST_B(ReadB(cpu.EA), cpu.D[DN(op)] & 7);
}
void btst_b_r_imm(uint16_t op) {
    uint8_t imm = FETCH();
    BTST_B(imm, cpu.D[DN(op)] & 7);
}

void bchg_l_i(uint16_t op) {
    int pos = FETCH() & 31;
    cpu.D[REG(op)] = BCHG_L(cpu.D[REG(op)], pos);
}

void bchg_b_i(uint16_t op) {
    int pos = FETCH() & 7;
    cpu.EA = ea_getaddr(TYPE(op), REG(op), 1);
    WriteB(cpu.EA, BCHG_B(ReadB(cpu.EA), pos));
}

void bchg_l_r(uint16_t op) {
    cpu.D[REG(op)] = BCHG_L(cpu.D[REG(op)], cpu.D[DN(op)] & 31);
}

void bchg_b_r(uint16_t op) {
    cpu.EA = ea_getaddr(TYPE(op), REG(op), 1);
    WriteB(cpu.EA, BCHG_B(ReadB(cpu.EA), cpu.D[DN(op)] & 7));
}

void bclr_l_i(uint16_t op) {
    int pos = FETCH() & 31;
    cpu.D[REG(op)] = BCLR_L(cpu.D[REG(op)], pos);
}

void bclr_b_i(uint16_t op) {
    int pos = FETCH() & 7;
    cpu.EA = ea_getaddr(TYPE(op), REG(op), 1);
    WriteB(cpu.EA, BCLR_B(ReadB(cpu.EA), pos));
}

void bclr_l_r(uint16_t op) {
    cpu.D[REG(op)] = BCLR_L(cpu.D[REG(op)], cpu.D[DN(op)] & 31);
}

void bclr_b_r(uint16_t op) {
    cpu.EA = ea_getaddr(TYPE(op), REG(op), 1);
    WriteB(cpu.EA, BCLR_B(ReadB(cpu.EA), cpu.D[DN(op)] & 7));
}

void bset_l_i(uint16_t op) {
    int pos = FETCH() & 31;
    cpu.D[REG(op)] = BSET_L(cpu.D[REG(op)], pos);
}

void bset_b_i(uint16_t op) {
    int pos = FETCH() & 7;
    cpu.EA = ea_getaddr(TYPE(op), REG(op), 1);
    WriteB(cpu.EA, BSET_B(ReadB(cpu.EA), pos));
}

void bset_l_r(uint16_t op) {
    cpu.D[REG(op)] = BSET_L(cpu.D[REG(op)], cpu.D[DN(op)] & 31);
}

void bset_b_r(uint16_t op) {
    cpu.EA = ea_getaddr(TYPE(op), REG(op), 1);
    WriteB(cpu.EA, BSET_B(ReadB(cpu.EA), cpu.D[DN(op)] & 7));
}
void movep_w_load(uint16_t op) {
    int16_t disp = FETCH();
    uint32_t base = cpu.A[REG(op)] + disp;
    STORE_W(cpu.D[DN(op)], ReadB(base) << 8 | ReadB(base + 2));
}

void movep_l_load(uint16_t op) {
    int16_t disp = FETCH();
    uint32_t base = cpu.A[REG(op)] + disp;
    cpu.D[DN(op)] = ReadB(base) << 24 | ReadB(base + 2) << 16 |
                ReadB(base + 4) << 8 | ReadB(base + 6);
}

void movep_w_store(uint16_t op) {
    int16_t disp = FETCH();
    uint32_t base = cpu.A[REG(op)] + disp;
    WriteB(base, cpu.D[DN(op)] >> 8);
    WriteB(base + 2, cpu.D[DN(op)]);
}

void movep_l_store(uint16_t op) {
    int16_t disp = FETCH();
    uint32_t base = cpu.A[REG(op)] + disp;
    WriteB(base, cpu.D[DN(op)] >> 24);
    WriteB(base + 2, cpu.D[DN(op)] >> 16);
    WriteB(base + 4, cpu.D[DN(op)] >> 8);
    WriteB(base + 6, cpu.D[DN(op)]);
}

void cas_b(uint16_t op) {
    uint16_t extw = FETCH();
    int du = extw >> 6 & 7;
    int dc = extw & 7;
    cpu.EA = ea_getaddr(TYPE(op), REG(op), 1);
    CAS_B(cpu.EA, dc, cpu.D[du]);
}

void cas_w(uint16_t op) {
    uint16_t extw = FETCH();
    int du = extw >> 6 & 7;
    int dc = extw & 7;
    cpu.EA = ea_getaddr(TYPE(op), REG(op), 2);
    CAS_W(cpu.EA, dc, cpu.D[du]);
}

void cas_l(uint16_t op) {
    uint16_t extw = FETCH();
    int du = extw >> 6 & 7;
    int dc = extw & 7;
    cpu.EA = ea_getaddr(TYPE(op), REG(op), 4);
    CAS_L(cpu.EA, dc, cpu.D[du]);
}

void cas2_w(uint16_t) {
    uint16_t extw1 = FETCH();
    uint16_t extw2 = FETCH();
    int rn1 = extw1 >> 12 & 15;
    int du1 = extw1 >> 6 & 7;
    int dc1 = extw1 & 7;
    int rn2 = extw2 >> 12 & 15;
    int du2 = extw2 >> 6 & 7;
    int dc2 = extw2 & 7;
    CAS2_W(cpu.R(rn1), dc1, cpu.D[du1], cpu.R(rn2), dc2, cpu.D[du2]);
}

void cas2_l(uint16_t) {
    uint16_t extw1 = FETCH();
    uint16_t extw2 = FETCH();
    int rn1 = extw1 >> 12 & 15;
    int du1 = extw1 >> 6 & 7;
    int dc1 = extw1 & 7;
    int rn2 = extw2 >> 12 & 15;
    int du2 = extw2 >> 6 & 7;
    int dc2 = extw2 & 7;
    CAS2_L(cpu.R(rn1), dc1, cpu.D[du1], cpu.R(rn2), dc2, cpu.D[du2]);
}

void moves_b(uint16_t op) {
    PRIV_CHECK();
    uint16_t extw = FETCH();
    int rn = extw >> 12 & 15;
    cpu.EA = ea_getaddr(TYPE(op), REG(op), 1);
    if(extw & 1 << 11) {
        WriteSB(cpu.EA, cpu.R(rn));
    } else {
        STORE_B(cpu.R(rn), ReadSB(cpu.EA));
    }
    TRACE_BRANCH();
}

void moves_w(uint16_t op) {
    PRIV_CHECK();
    uint16_t extw = FETCH();
    int rn = extw >> 12 & 15;
    cpu.EA = ea_getaddr(TYPE(op), REG(op), 2);
    if(extw & 1 << 11) {
        WriteSW(cpu.EA, cpu.R(rn));
    } else {
        STORE_W(cpu.R(rn), ReadSW(cpu.EA));
    }
    TRACE_BRANCH();
}

void moves_l(uint16_t op) {
    PRIV_CHECK();
    uint16_t extw = FETCH();
    int rn = extw >> 12 & 15;
    cpu.EA = ea_getaddr(TYPE(op), REG(op), 4);
    if(extw & 1 << 11) {
        WriteSL(cpu.EA, cpu.R(rn));
    } else {
        STORE_L(cpu.R(rn), ReadSL(cpu.EA));
    }
    TRACE_BRANCH();
}

void move_b(uint16_t op) {
    int dst_type = op >> 6 & 7;
    uint8_t v = ea_readB(TYPE(op), REG(op));
    TEST_B(v);
    cpu.V = cpu.C = false;
    ea_writeB(dst_type, DN(op), v);
}

void move_w(uint16_t op) {
    int dst_type = op >> 6 & 7;
    uint16_t v = ea_readW(TYPE(op), REG(op));
    TEST_W(v);
    cpu.V = cpu.C = false;
    ea_writeW(dst_type, DN(op), v);
}

void move_l(uint16_t op) {
    int dst_type = op >> 6 & 7;
    uint32_t v = ea_readL(TYPE(op), REG(op));
    TEST_L(v);
    cpu.V = cpu.C = false;
    ea_writeL(dst_type, DN(op), v);
}

void movea_w(uint16_t op) {
    uint16_t v = ea_readW(TYPE(op), REG(op));
    cpu.A[DN(op)] = static_cast<int16_t>(v);
}

void movea_l(uint16_t op) {
    uint32_t v = ea_readL(TYPE(op), REG(op));
    cpu.A[DN(op)] = v;
}

void move_from_sr(uint16_t op) {
    PRIV_CHECK();
    ea_writeW(TYPE(op), REG(op), GetSR(cpu));
}

void move_from_ccr(uint16_t op) {
    ea_writeW(TYPE(op), REG(op), GetCCR(cpu));
}

void move_to_ccr(uint16_t op) {
    SetCCR(cpu, ea_readW(TYPE(op), REG(op)));
}

void move_to_sr(uint16_t op) {
    PRIV_CHECK();
    SetSR(cpu, ea_readW(TYPE(op), REG(op)));
    TRACE_BRANCH();
}

void negx_b(uint16_t op) {
    uint8_t v = ea_readB(TYPE(op), REG(op), true);
    ea_writeB(TYPE(op), REG(op), NEGX_B(v, cpu.X));
}

void negx_w(uint16_t op) {
    uint16_t v = ea_readW(TYPE(op), REG(op), true);
    ea_writeW(TYPE(op), REG(op), NEGX_W(v, cpu.X));
}

void negx_l(uint16_t op) {
    uint32_t v = ea_readL(TYPE(op), REG(op), true);
    ea_writeL(TYPE(op), REG(op), NEGX_L(v, cpu.X));
}

void clr_b(uint16_t op) {
    cpu.N = cpu.V = cpu.C = false;
    cpu.Z = true;
    ea_writeB(TYPE(op), REG(op), 0);
}

void clr_w(uint16_t op) {
    cpu.N = cpu.V = cpu.C = false;
    cpu.Z = true;
    ea_writeW(TYPE(op), REG(op), 0);
}

void clr_l(uint16_t op) {
    cpu.N = cpu.V = cpu.C = false;
    cpu.Z = true;
    ea_writeL(TYPE(op), REG(op), 0);
}

void neg_b(uint16_t op) {
    uint8_t v = ea_readB(TYPE(op), REG(op), true);
    ea_writeB(TYPE(op), REG(op), NEG_B(v));
}

void neg_w(uint16_t op) {
    uint16_t v = ea_readW(TYPE(op), REG(op), true);
    ea_writeW(TYPE(op), REG(op), NEG_W(v));
}

void neg_l(uint16_t op) {
    uint32_t v = ea_readL(TYPE(op), REG(op), true);
    ea_writeL(TYPE(op), REG(op), NEG_L(v));
}

void not_b(uint16_t op) {
    uint8_t v = ea_readB(TYPE(op), REG(op), true);
    ea_writeB(TYPE(op), REG(op), NOT_B(v));
}

void not_w(uint16_t op) {
    uint16_t v = ea_readW(TYPE(op), REG(op), true);
    ea_writeW(TYPE(op), REG(op), NOT_W(v));
}

void not_l(uint16_t op) {
    uint32_t v = ea_readL(TYPE(op), REG(op), true);
    ea_writeL(TYPE(op), REG(op), NOT_L(v));
}

void nbcd(uint16_t op) {
    uint8_t b = ea_readB(TYPE(op), REG(op), true);
    ea_writeB(TYPE(op), REG(op), do_nbcd(b, cpu.X));
}

void link_l(uint16_t op) {
    int32_t disp = FETCH32();
    PUSH32(cpu.A[REG(op)]);
    cpu.A[REG(op)] = cpu.A[7];
    cpu.A[7] += disp;
}

void swap(uint16_t op) {
    uint32_t v = cpu.D[REG(op)] >> 16 | cpu.D[REG(op)] << 16;
    cpu.V = cpu.C = false;
    TEST_L(v);
    cpu.D[REG(op)] = v;
}

void bkpt(uint16_t) { ILLEGAL_OP(); }

void pea(uint16_t op) { PUSH32(ea_getaddr(TYPE(op), REG(op), 0)); }

void ext_w(uint16_t op) {
    int16_t v = static_cast<int8_t>(cpu.D[REG(op)] & 0xff);
    cpu.V = cpu.C = false;
    TEST_W(v);
    STORE_W(cpu.D[REG(op)], v);
}

void ext_l(uint16_t op) {
    int32_t v = static_cast<int16_t>(cpu.D[REG(op)] & 0xffff);
    cpu.V = cpu.C = false;
    TEST_L(v);
    STORE_L(cpu.D[REG(op)], v);
}

void movem_w_store_decr(uint16_t op);
void movem_w_store_base(uint16_t op);
void movem_l_store_decr(uint16_t op);
void movem_l_store_base(uint16_t op);

void movem_w_load_incr(uint16_t op);
void movem_w_load_base(uint16_t op);
void movem_l_load_incr(uint16_t op);
void movem_l_load_base(uint16_t op);

void tst_b(uint16_t op) {
    uint8_t b = ea_readB(TYPE(op), REG(op));
    cpu.V = cpu.C = false;
    TEST_B(b);
}

void tst_w(uint16_t op) {
    uint16_t b = ea_readW(TYPE(op), REG(op));
    cpu.V = cpu.C = false;
    TEST_W(b);
}

void tst_l(uint16_t op) {
    uint32_t b = ea_readL(TYPE(op), REG(op));
    cpu.V = cpu.C = false;
    TEST_L(b);
}

void tas(uint16_t op) {
    uint8_t v = ea_readB(TYPE(op), REG(op), true);
    TEST_B(v);
    ea_writeB(TYPE(op), REG(op), v | 0x80);
}

void illegal(uint16_t) { ILLEGAL_OP(); }

void mul_l(uint16_t op) {
    uint16_t ext = FETCH();
    uint32_t v = ea_readL(TYPE(op), REG(op));
    int low = ext >> 12 & 7;
    bool sig = ext & 1 << 11;
    bool dbl = ext & 1 << 10;
    int high = ext & 7;
    if(!sig) {
        // MULU.L
        if(!dbl) {
            cpu.D[low] = ::MULU_L(cpu.D[low], v);
        } else {
            uint64_t v2 = ::MULU_LL(cpu.D[low], v);
            cpu.D[high] = v2 >> 32;
            cpu.D[low] = v2 & 0xffffffff;
        }
    } else {
        // MULS.L
        if(!dbl) {
            cpu.D[low] = ::MULS_L(cpu.D[low], v);
        } else {
            int64_t v2 = ::MULS_LL(cpu.D[low], v);
            cpu.D[high] = v2 >> 32;
            cpu.D[low] = v2 & 0xffffffff;
        }
    }
}

void div_l(uint16_t op) {
    uint16_t ext = FETCH();
    uint32_t v = ea_readL(TYPE(op), REG(op));
    int q = ext >> 12 & 7;
    bool sig = ext & 1 << 11;
    bool dbl = ext & 1 << 10;
    int r = ext & 7;
    if(!sig) {
        // DIVU.L
        if(!dbl) {
            auto [quo, rem] = ::DIVU_L(cpu.D[q], v);
            cpu.D[q] = quo;
            if(q != r) {
                cpu.D[r] = rem;
            }
        } else {
            uint64_t x = static_cast<uint64_t>(cpu.D[r]) << 32 | cpu.D[q];
            auto [quot, rem] = ::DIVU_LL(x, v);
            cpu.D[q] = quot;
            if(q != r) {
                cpu.D[r] = rem;
            }
        }
    } else {
        // DIVS.L
        if(!dbl) {
            auto [quo, rem] = ::DIVS_L(cpu.D[q], v);
            cpu.D[q] = quo;
            if(q != r) {
                cpu.D[r] = rem;
            }
        } else {
            int64_t x = static_cast<int64_t>(cpu.D[r]) << 32 | cpu.D[q];
            auto [quot, rem] = ::DIVS_LL(x, static_cast<int32_t>(v));
            cpu.D[q] = quot;
            if(q != r) {
                cpu.D[r] = rem;
            }
        }
    }
}

void trap(uint16_t op) { TRAP_ERROR(TYPE(op) << 3 | REG(op)); }

void link_w(uint16_t op) {
    int16_t disp = FETCH();
    PUSH32(cpu.A[REG(op)]);
    cpu.A[REG(op)] = cpu.A[7];
    cpu.A[7] += disp;
}

void unlk(uint16_t op) {
    cpu.A[7] = cpu.A[REG(op)];
    cpu.A[REG(op)] = POP32();
}

void move_to_usp(uint16_t op) {
    PRIV_CHECK();
    cpu.USP = cpu.A[REG(op)];
    TRACE_BRANCH();
}

void move_from_usp(uint16_t op) {
    PRIV_CHECK();
    cpu.A[REG(op)] = cpu.USP;
    TRACE_BRANCH();
}

void reset(uint16_t) {
    PRIV_CHECK();
    bus_reset();
}

void nop(uint16_t) { TRACE_BRANCH(); }

void stop_impl(uint16_t nw) {
    PRIV_CHECK();
    SetSR(cpu, nw);
    cpu.run.stop();
    cpu.run.wait();
    TRACE_BRANCH();
}
void stop(uint16_t) {
    uint16_t nw = FETCH();
    stop_impl(nw);
}

void rte(uint16_t) {
    PRIV_CHECK();
    do_rte();
    TRACE_BRANCH();
}

void rtd(uint16_t) {
    int16_t disp = FETCH();
    do_rtd(disp);
    TRACE_BRANCH();
}

void rts(uint16_t) {
    do_rts();
    TRACE_BRANCH();
}

void trapv(uint16_t) {
    if(cpu.V) {
        TRAPX_ERROR();
    }
}

void rtr(uint16_t) {
    do_rtr();
    TRACE_BRANCH();
}

void movec_from_cr(uint16_t);
void movec_to_cr(uint16_t);

void jsr(uint16_t op) {
    uint32_t addr = ea_getaddr(TYPE(op), REG(op), 0);
    PUSH32(cpu.PC);
    JUMP(addr);
    TRACE_BRANCH();
}

void jmp(uint16_t op) {
    uint32_t addr = ea_getaddr(TYPE(op), REG(op), 0);
    JUMP(addr);
    TRACE_BRANCH();
}

void chk_l(uint16_t op) {
    uint32_t v = ea_readL(TYPE(op), REG(op));
    CHK_L(cpu.D[DN(op)], v);
}

void chk_w(uint16_t op) {
    uint16_t v = ea_readW(TYPE(op), REG(op));
    CHK_W(cpu.D[DN(op)], v);
}

void extb_l(uint16_t op) {
    int32_t v = static_cast<int8_t>(cpu.D[REG(op)] & 0xff);
    cpu.V = cpu.C = false;
    TEST_L(v);
    cpu.D[REG(op)] = v;
}

void lea(uint16_t op) {
    cpu.A[DN(op)] = ea_getaddr(TYPE(op), REG(op), 0);
}

void addq_b(uint16_t op) {
    uint8_t v = ea_readB(TYPE(op), REG(op), true);
    ea_writeB(TYPE(op), REG(op), ADD_B(v, DN(op) ? DN(op) : 8));
}

void addq_w(uint16_t op) {
    uint16_t v = ea_readW(TYPE(op), REG(op), true);
    ea_writeW(TYPE(op), REG(op), ADD_W(v, DN(op) ? DN(op) : 8));
}

void addq_l(uint16_t op) {
    uint32_t v = ea_readL(TYPE(op), REG(op), true);
    ea_writeL(TYPE(op), REG(op), ADD_L(v, DN(op) ? DN(op) : 8));
}

void subq_b(uint16_t op) {
    uint8_t v = ea_readB(TYPE(op), REG(op), true);
    ea_writeB(TYPE(op), REG(op), SUB_B(v, DN(op) ? DN(op) : 8));
}

void subq_w(uint16_t op) {
    uint16_t v = ea_readW(TYPE(op), REG(op), true);
    ea_writeW(TYPE(op), REG(op), SUB_W(v, DN(op) ? DN(op) : 8));
}

void subq_l(uint16_t op) {
    uint32_t v = ea_readL(TYPE(op), REG(op), true);
    ea_writeL(TYPE(op), REG(op), SUB_L(v, DN(op) ? DN(op) : 8));
}

void addq_an(uint16_t op) { cpu.A[REG(op)] += DN(op) ? DN(op) : 8; }

void subq_an(uint16_t op) { cpu.A[REG(op)] -= DN(op) ? DN(op) : 8; }

bool testCC(int cc) {
    switch(cc) {
    case 0: // T
        return true;
    case 1: // F
        return false;
    case 2: // HI
        return !cpu.C && !cpu.Z;
    case 3: // LS
        return cpu.C || cpu.Z;
    case 4: // CC
        return !cpu.C;
    case 5: // CS
        return cpu.C;
    case 6: // NE
        return !cpu.Z;
    case 7: // EQ
        return cpu.Z;
    case 8: // VC
        return !cpu.V;
    case 9: // VS
        return cpu.V;
    case 10: // PL
        return !cpu.N;
    case 11: // MI
        return cpu.N;
    case 12: // GE
        return cpu.N == cpu.V;
    case 13: // LT
        return cpu.N != cpu.V;
    case 14: // GT
        return !cpu.Z && (cpu.N == cpu.V);
    case 15: // LE
        return cpu.Z || (cpu.N != cpu.V);
    default:
        __builtin_unreachable();
    }
}


void scc_ea(uint16_t op) {
    int cc = op >> 8 & 0xf;
    ea_writeB(TYPE(op), REG(op), testCC(cc) ? 0xff : 0);
}

void dbcc(uint16_t op) {
    int cc = op >> 8 & 0xf;
    int16_t disp = FETCH();
    do_dbcc(testCC(cc), REG(op), disp);
}

void trapcc(uint16_t op) {
    int cc = op >> 8 & 0xf;
    switch(REG(op)) {
    case 2:
        cpu.PC += 2;
        break;
    case 3:
        cpu.PC += 4;
        break;
    case 4:
        break;
    default:
        __builtin_unreachable();
    }
    if(testCC(cc)) {
        TRAPX_ERROR();
    }
}

void bxx(uint16_t op) {
    int cc = op >> 8 & 0xf;
    int8_t imm8 = op & 0xff;
    int32_t imm;
    if(imm8 == 0) {
        imm = static_cast<int16_t>(FETCH());
    } else if(imm8 == -1) {
        imm = FETCH32();
    } else {
        imm = imm8;
    }
    if(cc == 0) {
        do_bra(imm);
    } else if(cc == 1) {
        do_bsr(imm);
    } else {
        do_bcc(testCC(cc), imm);
    }
}

void moveq(uint16_t op) {
    int8_t data = op & 0xff;
    cpu.D[DN(op)] = data;
    TEST_B(data);
    cpu.V = false;
    cpu.C = false;
}

void or_b_to_dn(uint16_t op) {
    STORE_B(cpu.D[DN(op)], OR_B(ea_readB(TYPE(op), REG(op)), cpu.D[DN(op)]));
}

void or_w_to_dn(uint16_t op) {
    STORE_W(cpu.D[DN(op)], OR_W(ea_readW(TYPE(op), REG(op)), cpu.D[DN(op)]));
}

void or_l_to_dn(uint16_t op) {
    STORE_L(cpu.D[DN(op)], OR_L(ea_readL(TYPE(op), REG(op)), cpu.D[DN(op)]));
}

void divu_w(uint16_t op) {
    auto [div_q, div_r] = ::DIVU_W(cpu.D[DN(op)], ea_readW(TYPE(op), REG(op)));
    cpu.D[DN(op)] = (div_r << 16 | div_q);
}

void sbcd_d(uint16_t op) {
    STORE_B(cpu.D[DN(op)], do_sbcd(cpu.D[REG(op)], cpu.D[DN(op)], cpu.X));
}

void sbcd_m(uint16_t op) {
    auto v1 = ReadB(--cpu.A[REG(op)]);
    auto v2 = ReadB(--cpu.A[DN(op)]);
    WriteB(cpu.A[DN(op)], do_sbcd(v1, v2, cpu.X));
}

void or_b_to_ea(uint16_t op) {
    uint8_t v = ea_readB(TYPE(op), REG(op), true);
    ea_writeB(TYPE(op), REG(op), OR_B(cpu.D[DN(op)], v));
}

void pack_d(uint16_t op) {
    uint16_t adj = FETCH();
    STORE_B(cpu.D[DN(op)], do_pack(cpu.D[REG(op)], adj));
}

void pack_m(uint16_t op) {
    uint16_t adj = FETCH();
    uint16_t d = ReadB(--cpu.A[REG(op)]);
    d |= ReadB(--cpu.A[REG(op)]) << 8;
    WriteB(--cpu.A[DN(op)], do_pack(d, adj));
}

void or_w_to_ea(uint16_t op) {
    uint16_t v = ea_readW(TYPE(op), REG(op), true);
    ea_writeW(TYPE(op), REG(op), OR_W(cpu.D[DN(op)], v));
}

void unpk_d(uint16_t op) {
    uint16_t adj = FETCH();
    STORE_W(cpu.D[DN(op)], do_unpk(cpu.D[REG(op)], adj));
}

void unpk_m(uint16_t op) {
    uint16_t adj = FETCH();
    uint16_t d = do_unpk(ReadB(--cpu.A[REG(op)]), adj);
    WriteB(--cpu.A[DN(op)] , d);
    WriteB(--cpu.A[DN(op)] , d >> 8);
}

void or_l_to_ea(uint16_t op) {
    uint32_t v = ea_readL(TYPE(op), REG(op), true);
    ea_writeL(TYPE(op), REG(op), OR_L(cpu.D[DN(op)], v));
}

void divs_w(uint16_t op) {
    auto [div_q, div_r] = ::DIVS_W(cpu.D[DN(op)], ea_readW(TYPE(op), REG(op)));
    cpu.D[DN(op)] =
        static_cast<uint16_t>(div_r) << 16 | static_cast<uint16_t>(div_q);
}

void sub_b_to_dn(uint16_t op) {
    STORE_B(cpu.D[DN(op)], SUB_B(cpu.D[DN(op)], ea_readB(TYPE(op), REG(op))));
}

void sub_w_to_dn(uint16_t op) {
    STORE_W(cpu.D[DN(op)], SUB_W(cpu.D[DN(op)], ea_readW(TYPE(op), REG(op))));
}

void sub_l_to_dn(uint16_t op) {
    STORE_L(cpu.D[DN(op)], SUB_L(cpu.D[DN(op)], ea_readL(TYPE(op), REG(op))));
}

void suba_w(uint16_t op) {
    cpu.A[DN(op)] -= static_cast<int16_t>(ea_readW(TYPE(op), REG(op)));
}

void suba_l(uint16_t op) {
    cpu.A[DN(op)] -= ea_readL(TYPE(op), REG(op));
}

void subx_b_d(uint16_t op) {
    STORE_B(cpu.D[REG(op)], SUBX_B(cpu.D[REG(op)], cpu.D[DN(op)], cpu.X));
}

void subx_b_m(uint16_t op) {
    uint32_t dst_adr = --cpu.A[REG(op)];
    uint32_t src_adr = --cpu.A[DN(op)];
    auto v1 = ReadB(dst_adr);
    auto v2 = ReadB(src_adr);
    WriteB(dst_adr, SUBX_B(v1, v2, cpu.X));
}

void subx_w_d(uint16_t op) {
    STORE_W(cpu.D[REG(op)], SUBX_W(cpu.D[REG(op)], cpu.D[DN(op)], cpu.X));
}

void subx_w_m(uint16_t op) {
    uint32_t dst_adr = cpu.A[REG(op)] -= 2;
    uint32_t src_adr = cpu.A[DN(op)] -= 2;
    auto v1 = ReadW(dst_adr);
    auto v2 = ReadW(src_adr);
    WriteW(dst_adr, SUBX_W(v1, v2, cpu.X));
}
void subx_l_d(uint16_t op) {
    STORE_L(cpu.D[REG(op)], SUBX_L(cpu.D[REG(op)], cpu.D[DN(op)], cpu.X));
}

void subx_l_m(uint16_t op) {
    uint32_t dst_adr = cpu.A[REG(op)] -= 4;
    uint32_t src_adr = cpu.A[DN(op)] -= 4;
    auto v1 = ReadL(dst_adr);
    auto v2 = ReadL(src_adr);
    WriteL(dst_adr, SUBX_L(v1, v2, cpu.X));
}

void sub_b_to_ea(uint16_t op) {
    uint8_t v = ea_readB(TYPE(op), REG(op), true);
    ea_writeB(TYPE(op), REG(op), SUB_B(v, cpu.D[DN(op)]));
}

void sub_w_to_ea(uint16_t op) {
    uint16_t v = ea_readW(TYPE(op), REG(op), true);
    ea_writeW(TYPE(op), REG(op), SUB_W(v, cpu.D[DN(op)]));
}

void sub_l_to_ea(uint16_t op) {
    uint32_t v = ea_readL(TYPE(op), REG(op), true);
    ea_writeL(TYPE(op), REG(op), SUB_L(v, cpu.D[DN(op)]));
}

void aline_ex(uint16_t /* op */) {
    // some optimize for specific op?
    ALINE();
}

void cmp_b(uint16_t op) {
    CMP_B(cpu.D[DN(op)], ea_readB(TYPE(op), REG(op)));
}

void cmp_w(uint16_t op) {
    CMP_W(cpu.D[DN(op)], ea_readW(TYPE(op), REG(op)));
}

void cmp_l(uint16_t op) {
    CMP_L(cpu.D[DN(op)], ea_readL(TYPE(op), REG(op)));
}

void cmpa_w(uint16_t op) {
    CMP_L(cpu.A[DN(op)],
          static_cast<int16_t>(ea_readW(TYPE(op), REG(op))));
}

void cmpa_l(uint16_t op) {
    CMP_L(cpu.A[DN(op)], ea_readL(TYPE(op), REG(op)));
}

void eor_b(uint16_t op) {
    uint8_t v = ea_readB(TYPE(op), REG(op), true);
    ea_writeB(TYPE(op), REG(op), XOR_B(cpu.D[DN(op)], v));
}

void eor_w(uint16_t op) {
    uint16_t v = ea_readW(TYPE(op), REG(op), true);
    ea_writeW(TYPE(op), REG(op), XOR_W(cpu.D[DN(op)], v));
}

void eor_l(uint16_t op) {
    uint32_t v = ea_readL(TYPE(op), REG(op), true);
    ea_writeL(TYPE(op), REG(op), XOR_L(cpu.D[DN(op)], v));
}

void cmpm_b(uint16_t op) {
    uint32_t src_adr = std::exchange(cpu.A[REG(op)], cpu.A[REG(op)] + 1);
    uint32_t dst_adr = std::exchange(cpu.A[DN(op)], cpu.A[DN(op)] + 1);
    CMP_B(ReadB(dst_adr), ReadB(src_adr));
}

void cmpm_w(uint16_t op) {
    uint32_t src_adr = std::exchange(cpu.A[REG(op)], cpu.A[REG(op)] + 2);
    uint32_t dst_adr = std::exchange(cpu.A[DN(op)], cpu.A[DN(op)] + 2);
    CMP_W(ReadW(dst_adr), ReadW(src_adr));
}

void cmpm_l(uint16_t op) {
    uint32_t src_adr = std::exchange(cpu.A[REG(op)], cpu.A[REG(op)] + 4);
    uint32_t dst_adr = std::exchange(cpu.A[DN(op)], cpu.A[DN(op)] + 4);
    CMP_L(ReadL(dst_adr), ReadL(src_adr));
}

void and_b_to_dn(uint16_t op) {
    STORE_B(cpu.D[DN(op)], AND_B(ea_readB(TYPE(op), REG(op)), cpu.D[DN(op)]));
}

void and_w_to_dn(uint16_t op) {
    STORE_W(cpu.D[DN(op)], AND_W(ea_readW(TYPE(op), REG(op)), cpu.D[DN(op)]));
}

void and_l_to_dn(uint16_t op) {
    STORE_L(cpu.D[DN(op)], AND_L(ea_readL(TYPE(op), REG(op)), cpu.D[DN(op)]));
}

void mulu_w(uint16_t op) {
    cpu.D[DN(op)] = ::MULU_W(cpu.D[DN(op)], ea_readW(TYPE(op), REG(op)));
}

void abcd_d(uint16_t op) {
    STORE_B(cpu.D[DN(op)], do_abcd(cpu.D[REG(op)], cpu.D[DN(op)], cpu.X));
}

void abcd_m(uint16_t op) {
    auto v1 = ReadB(--cpu.A[REG(op)]);
    auto v2 = ReadB(--cpu.A[DN(op)]);
    WriteB(cpu.A[DN(op)], do_abcd(v1, v2, cpu.X));
}

void and_b_to_ea(uint16_t op) {
    uint8_t v = ea_readB(TYPE(op), REG(op), true);
    ea_writeB(TYPE(op), REG(op), AND_B(cpu.D[DN(op)], v));
}

void and_w_to_ea(uint16_t op) {
    uint16_t v = ea_readW(TYPE(op), REG(op), true);
    ea_writeW(TYPE(op), REG(op), AND_W(cpu.D[DN(op)], v));
}

void and_l_to_ea(uint16_t op) {
    uint32_t v = ea_readL(TYPE(op), REG(op), true);
    ea_writeL(TYPE(op), REG(op), AND_L(cpu.D[DN(op)], v));
}

void muls_w(uint16_t op) {
    cpu.D[DN(op)] = ::MULS_W(cpu.D[DN(op)], ea_readW(TYPE(op), REG(op)));
}

void exg_dn(uint16_t op) {
    std::swap(cpu.D[DN(op)], cpu.D[REG(op)]);
}

void exg_an(uint16_t op) {
    std::swap(cpu.A[DN(op)], cpu.A[REG(op)]);
}

void exg_da(uint16_t op) {
    std::swap(cpu.D[DN(op)], cpu.A[REG(op)]);
}

void add_b_to_dn(uint16_t op) {
    STORE_B(cpu.D[DN(op)], ADD_B(cpu.D[DN(op)], ea_readB(TYPE(op), REG(op))));
}

void add_w_to_dn(uint16_t op) {
    STORE_W(cpu.D[DN(op)], ADD_W(cpu.D[DN(op)], ea_readW(TYPE(op), REG(op))));
}

void add_l_to_dn(uint16_t op) {
    STORE_L(cpu.D[DN(op)], ADD_L(cpu.D[DN(op)], ea_readL(TYPE(op), REG(op))));
}

void adda_w(uint16_t op) {
    cpu.A[DN(op)] += static_cast<int16_t>(ea_readW(TYPE(op), REG(op)));
}

void adda_l(uint16_t op) {
    cpu.A[DN(op)] += ea_readL(TYPE(op), REG(op));
}

void addx_b_d(uint16_t op) {
    STORE_B(cpu.D[DN(op)], ADDX_B(cpu.D[DN(op)], cpu.D[REG(op)], cpu.X));
}

void addx_b_m(uint16_t op) {
    uint32_t dst_adr = --cpu.A[DN(op)];
    uint32_t src_adr = --cpu.A[REG(op)];
    auto v1 = ReadB(dst_adr);
    auto v2 = ReadB(src_adr);
    WriteB(dst_adr, ADDX_B(v1, v2, cpu.X));
}

void addx_w_d(uint16_t op) {
    STORE_W(cpu.D[DN(op)], ADDX_W(cpu.D[DN(op)], cpu.D[REG(op)], cpu.X));
}

void addx_w_m(uint16_t op) {
    uint32_t dst_adr = cpu.A[DN(op)] -= 2;
    uint32_t src_adr = cpu.A[REG(op)] -= 2;
    auto v1 = ReadW(dst_adr);
    auto v2 = ReadW(src_adr);
    WriteW(dst_adr, ADDX_W(v1, v2, cpu.X));
}
void addx_l_d(uint16_t op) {
    STORE_L(cpu.D[DN(op)], ADDX_L(cpu.D[DN(op)], cpu.D[REG(op)], cpu.X));
}

void addx_l_m(uint16_t op) {
    uint32_t dst_adr = cpu.A[DN(op)] -= 4;
    uint32_t src_adr = cpu.A[REG(op)] -= 4;
    auto v1 = ReadL(dst_adr);
    auto v2 = ReadL(src_adr);
    WriteL(dst_adr, ADDX_L(v1, v2, cpu.X));
}

void add_b_to_ea(uint16_t op) {
    uint8_t v = ea_readB(TYPE(op), REG(op), true);
    ea_writeB(TYPE(op), REG(op), ADD_B(v, cpu.D[DN(op)]));
}

void add_w_to_ea(uint16_t op) {
    uint16_t v = ea_readW(TYPE(op), REG(op), true);
    ea_writeW(TYPE(op), REG(op), ADD_W(v, cpu.D[DN(op)]));
}

void add_l_to_ea(uint16_t op) {
    uint32_t v = ea_readL(TYPE(op), REG(op), true);
    ea_writeL(TYPE(op), REG(op), ADD_L(v, cpu.D[DN(op)]));
}

void asr_b_i(uint16_t op) {
    STORE_B(cpu.D[REG(op)], ASR_B(cpu.D[REG(op)], DN(op) ? DN(op) : 8));
}

void lsr_b_i(uint16_t op) {
    STORE_B(cpu.D[REG(op)], LSR_B(cpu.D[REG(op)], DN(op) ? DN(op) : 8));
}

void roxr_b_i(uint16_t op) {
    STORE_B(cpu.D[REG(op)], ROXR_B(cpu.D[REG(op)], DN(op) ? DN(op) : 8, cpu.X));
}

void ror_b_i(uint16_t op) {
    STORE_B(cpu.D[REG(op)], ROR_B(cpu.D[REG(op)], DN(op) ? DN(op) : 8));
}

void asr_b_r(uint16_t op) {
    STORE_B(cpu.D[REG(op)], ASR_B(cpu.D[REG(op)], cpu.D[DN(op)] & 63));
}

void lsr_b_r(uint16_t op) {
    STORE_B(cpu.D[REG(op)], LSR_B(cpu.D[REG(op)], cpu.D[DN(op)] & 63));
}

void roxr_b_r(uint16_t op) {
    STORE_B(cpu.D[REG(op)], ROXR_B(cpu.D[REG(op)], cpu.D[DN(op)] & 63, cpu.X));
}

void ror_b_r(uint16_t op) {
    STORE_B(cpu.D[REG(op)], ROR_B(cpu.D[REG(op)], cpu.D[DN(op)] & 63));
}

void asr_w_i(uint16_t op) {
    STORE_W(cpu.D[REG(op)], ASR_W(cpu.D[REG(op)], DN(op) ? DN(op) : 8));
}

void lsr_w_i(uint16_t op) {
    STORE_W(cpu.D[REG(op)], LSR_W(cpu.D[REG(op)], DN(op) ? DN(op) : 8));
}

void roxr_w_i(uint16_t op) {
    STORE_W(cpu.D[REG(op)], ROXR_W(cpu.D[REG(op)], DN(op) ? DN(op) : 8, cpu.X));
}

void ror_w_i(uint16_t op) {
    STORE_W(cpu.D[REG(op)], ROR_W(cpu.D[REG(op)], DN(op) ? DN(op) : 8));
}

void asr_w_r(uint16_t op) {
    STORE_W(cpu.D[REG(op)], ASR_W(cpu.D[REG(op)], cpu.D[DN(op)] & 63));
}

void lsr_w_r(uint16_t op) {
    STORE_W(cpu.D[REG(op)], LSR_W(cpu.D[REG(op)], cpu.D[DN(op)] & 63));
}

void roxr_w_r(uint16_t op) {
    STORE_W(cpu.D[REG(op)], ROXR_W(cpu.D[REG(op)], cpu.D[DN(op)] & 63, cpu.X));
}

void ror_w_r(uint16_t op) {
    STORE_W(cpu.D[REG(op)], ROR_W(cpu.D[REG(op)], cpu.D[DN(op)] & 63));
}

void asr_l_i(uint16_t op) {
    STORE_L(cpu.D[REG(op)], ASR_L(cpu.D[REG(op)], DN(op) ? DN(op) : 8));
}

void lsr_l_i(uint16_t op) {
    STORE_L(cpu.D[REG(op)], LSR_L(cpu.D[REG(op)], DN(op) ? DN(op) : 8));
}

void roxr_l_i(uint16_t op) {
    STORE_L(cpu.D[REG(op)], ROXR_L(cpu.D[REG(op)], DN(op) ? DN(op) : 8, cpu.X));
}

void ror_l_i(uint16_t op) {
    STORE_L(cpu.D[REG(op)], ROR_L(cpu.D[REG(op)], DN(op) ? DN(op) : 8));
}

void asr_l_r(uint16_t op) {
    STORE_L(cpu.D[REG(op)], ASR_L(cpu.D[REG(op)], cpu.D[DN(op)] & 63));
}

void lsr_l_r(uint16_t op) {
    STORE_L(cpu.D[REG(op)], LSR_L(cpu.D[REG(op)], cpu.D[DN(op)] & 63));
}

void roxr_l_r(uint16_t op) {
    STORE_L(cpu.D[REG(op)], ROXR_L(cpu.D[REG(op)], cpu.D[DN(op)] & 63, cpu.X));
}

void ror_l_r(uint16_t op) {
    STORE_L(cpu.D[REG(op)], ROR_L(cpu.D[REG(op)], cpu.D[DN(op)] & 63));
}

void asr_ea(uint16_t op) {
    uint16_t v = ea_readW(TYPE(op), REG(op), true);
    ea_writeW(TYPE(op), REG(op), ASR_W(v, 1));
}

void lsr_ea(uint16_t op) {
    uint16_t v = ea_readW(TYPE(op), REG(op), true);
    ea_writeW(TYPE(op), REG(op), LSR_W(v, 1));
}

void roxr_ea(uint16_t op) {
    uint16_t v = ea_readW(TYPE(op), REG(op), true);
    ea_writeW(TYPE(op), REG(op), ROXR_W(v, 1, cpu.X));
}

void ror_ea(uint16_t op) {
    uint16_t v = ea_readW(TYPE(op), REG(op), true);
    ea_writeW(TYPE(op), REG(op), ROR_W(v, 1));
}



void asl_b_i(uint16_t op) {
    STORE_B(cpu.D[REG(op)], ASL_B(cpu.D[REG(op)], DN(op) ? DN(op) : 8));
}

void lsl_b_i(uint16_t op) {
    STORE_B(cpu.D[REG(op)], LSL_B(cpu.D[REG(op)], DN(op) ? DN(op) : 8));
}

void roxl_b_i(uint16_t op) {
    STORE_B(cpu.D[REG(op)], ROXL_B(cpu.D[REG(op)], DN(op) ? DN(op) : 8, cpu.X));
}

void rol_b_i(uint16_t op) {
    STORE_B(cpu.D[REG(op)], ROL_B(cpu.D[REG(op)], DN(op) ? DN(op) : 8));
}

void asl_b_r(uint16_t op) {
    STORE_B(cpu.D[REG(op)], ASL_B(cpu.D[REG(op)], cpu.D[DN(op)] & 63));
}

void lsl_b_r(uint16_t op) {
    STORE_B(cpu.D[REG(op)], LSL_B(cpu.D[REG(op)], cpu.D[DN(op)] & 63));
}

void roxl_b_r(uint16_t op) {
    STORE_B(cpu.D[REG(op)], ROXL_B(cpu.D[REG(op)], cpu.D[DN(op)] & 63, cpu.X));
}

void rol_b_r(uint16_t op) {
    STORE_B(cpu.D[REG(op)], ROL_B(cpu.D[REG(op)], cpu.D[DN(op)] & 63));
}

void asl_w_i(uint16_t op) {
    STORE_W(cpu.D[REG(op)], ASL_W(cpu.D[REG(op)], DN(op) ? DN(op) : 8));
}

void lsl_w_i(uint16_t op) {
    STORE_W(cpu.D[REG(op)], LSL_W(cpu.D[REG(op)], DN(op) ? DN(op) : 8));
}

void roxl_w_i(uint16_t op) {
    STORE_W(cpu.D[REG(op)], ROXL_W(cpu.D[REG(op)], DN(op) ? DN(op) : 8, cpu.X));
}

void rol_w_i(uint16_t op) {
    STORE_W(cpu.D[REG(op)], ROL_W(cpu.D[REG(op)], DN(op) ? DN(op) : 8));
}

void asl_w_r(uint16_t op) {
    STORE_W(cpu.D[REG(op)], ASL_W(cpu.D[REG(op)], cpu.D[DN(op)] & 63));
}

void lsl_w_r(uint16_t op) {
    STORE_W(cpu.D[REG(op)], LSL_W(cpu.D[REG(op)], cpu.D[DN(op)] & 63));
}

void roxl_w_r(uint16_t op) {
    STORE_W(cpu.D[REG(op)], ROXL_W(cpu.D[REG(op)], cpu.D[DN(op)] & 63, cpu.X));
}

void rol_w_r(uint16_t op) {
    STORE_W(cpu.D[REG(op)], ROL_W(cpu.D[REG(op)], cpu.D[DN(op)] & 63));
}

void asl_l_i(uint16_t op) {
    STORE_L(cpu.D[REG(op)], ASL_L(cpu.D[REG(op)], DN(op) ? DN(op) : 8));
}

void lsl_l_i(uint16_t op) {
    STORE_L(cpu.D[REG(op)], LSL_L(cpu.D[REG(op)], DN(op) ? DN(op) : 8));
}

void roxl_l_i(uint16_t op) {
    STORE_L(cpu.D[REG(op)], ROXL_L(cpu.D[REG(op)], DN(op) ? DN(op) : 8, cpu.X));
}

void rol_l_i(uint16_t op) {
    STORE_L(cpu.D[REG(op)], ROL_L(cpu.D[REG(op)], DN(op) ? DN(op) : 8));
}

void asl_l_r(uint16_t op) {
    STORE_L(cpu.D[REG(op)], ASL_L(cpu.D[REG(op)], cpu.D[DN(op)] & 63));
}

void lsl_l_r(uint16_t op) {
    STORE_L(cpu.D[REG(op)], LSL_L(cpu.D[REG(op)], cpu.D[DN(op)] & 63));
}

void roxl_l_r(uint16_t op) {
    STORE_L(cpu.D[REG(op)], ROXL_L(cpu.D[REG(op)], cpu.D[DN(op)] & 63, cpu.X));
}

void rol_l_r(uint16_t op) {
    STORE_L(cpu.D[REG(op)], ROL_L(cpu.D[REG(op)], cpu.D[DN(op)] & 63));
}

void asl_ea(uint16_t op) {
    uint16_t v = ea_readW(TYPE(op), REG(op), true);
    ea_writeW(TYPE(op), REG(op), ASL_W(v, 1));
}

void lsl_ea(uint16_t op) {
    uint16_t v = ea_readW(TYPE(op), REG(op), true);
    ea_writeW(TYPE(op), REG(op), LSL_W(v, 1));
}

void roxl_ea(uint16_t op) {
    uint16_t v = ea_readW(TYPE(op), REG(op), true);
    ea_writeW(TYPE(op), REG(op), ROXL_W(v, 1, cpu.X));
}

void rol_ea(uint16_t op) {
    uint16_t v = ea_readW(TYPE(op), REG(op), true);
    ea_writeW(TYPE(op), REG(op), ROL_W(v, 1));
}

std::pair<int, int> get_bf_offset_width(uint16_t extw) {
    int offset = extw >> 6 & 0x1f;
    int width = extw & 0x1f;
    int offset_v = (extw & 1 << 11) ? cpu.D[offset & 7] : offset;
    int width_v = (extw & 1 << 5) ? (cpu.D[width & 7] & 31) : width;
    if(width_v == 0) {
        width_v = 32;
    }
    return std::make_pair(offset_v, width_v);
}

void bftst_dn(uint16_t op) {
    uint16_t extw = FETCH();
    auto [offset_v, width_v] = get_bf_offset_width(extw);
    BFTST_D(cpu.D[REG(op)], offset_v, width_v);
}

void bftst_mem(uint16_t op) {
    uint16_t extw = FETCH();
    auto [offset_v, width_v] = get_bf_offset_width(extw);
    ::BFTST_M(ea_getaddr(TYPE(op), REG(op), 0), offset_v, width_v);
}

void bfchg_dn(uint16_t op) {
    uint16_t extw = FETCH();
    auto [offset_v, width_v] = get_bf_offset_width(extw);
    cpu.D[REG(op)] = BFCHG_D(cpu.D[REG(op)], offset_v, width_v);
}

void bfchg_mem(uint16_t op) {
    uint16_t extw = FETCH();
    auto [offset_v, width_v] = get_bf_offset_width(extw);
    ::BFCHG_M(ea_getaddr(TYPE(op), REG(op), 0), offset_v, width_v);
}

void bfclr_dn(uint16_t op) {
    uint16_t extw = FETCH();
    auto [offset_v, width_v] = get_bf_offset_width(extw);
    cpu.D[REG(op)] = BFCLR_D(cpu.D[REG(op)], offset_v, width_v);
}

void bfclr_mem(uint16_t op) {
    uint16_t extw = FETCH();
    auto [offset_v, width_v] = get_bf_offset_width(extw);
    ::BFCLR_M(ea_getaddr(TYPE(op), REG(op), 0), offset_v, width_v);
}

void bfset_dn(uint16_t op) {
    uint16_t extw = FETCH();
    auto [offset_v, width_v] = get_bf_offset_width(extw);
    cpu.D[REG(op)] = BFSET_D(cpu.D[REG(op)], offset_v, width_v);
}

void bfset_mem(uint16_t op) {
    uint16_t extw = FETCH();
    auto [offset_v, width_v] = get_bf_offset_width(extw);
    ::BFSET_M(ea_getaddr(TYPE(op), REG(op), 0), offset_v, width_v);
}

void bfextu_dn(uint16_t op) {
    uint16_t extw = FETCH();
    auto [offset_v, width_v] = get_bf_offset_width(extw);
    int dn = extw >> 12 & 7;
    cpu.D[dn] = BFEXTU_D(cpu.D[REG(op)], offset_v, width_v);
}

void bfextu_mem(uint16_t op) {
    uint16_t extw = FETCH();
    auto [offset_v, width_v] = get_bf_offset_width(extw);
    int dn = extw >> 12 & 7;
    cpu.D[dn] = BFEXTU_M(ea_getaddr(TYPE(op), REG(op), 0), offset_v, width_v);
}

void bfexts_dn(uint16_t op) {
    uint16_t extw = FETCH();
    auto [offset_v, width_v] = get_bf_offset_width(extw);
    int dn = extw >> 12 & 7;
    cpu.D[dn] = BFEXTS_D(cpu.D[REG(op)], offset_v, width_v);
}

void bfexts_mem(uint16_t op) {
    uint16_t extw = FETCH();
    auto [offset_v, width_v] = get_bf_offset_width(extw);
    int dn = extw >> 12 & 7;
    cpu.D[dn] = BFEXTS_M(ea_getaddr(TYPE(op), REG(op), 0), offset_v, width_v);
}

void bfffo_dn(uint16_t op) {
    uint16_t extw = FETCH();
    auto [offset_v, width_v] = get_bf_offset_width(extw);
    int dn = extw >> 12 & 7;
    cpu.D[dn] = BFFFO_D(cpu.D[REG(op)], offset_v, width_v);
}

void bfffo_mem(uint16_t op) {
    uint16_t extw = FETCH();
    auto [offset_v, width_v] = get_bf_offset_width(extw);
    int dn = extw >> 12 & 7;
    cpu.D[dn] = BFFFO_M(ea_getaddr(TYPE(op), REG(op), 0), offset_v, width_v);
}

void bfins_dn(uint16_t op) {
    uint16_t extw = FETCH();
    auto [offset_v, width_v] = get_bf_offset_width(extw);
    int dn = extw >> 12 & 7;
    cpu.D[REG(op)] = BFINS_D(cpu.D[REG(op)], offset_v, width_v, cpu.D[dn]);
}

void bfins_mem(uint16_t op) {
    uint16_t extw = FETCH();
    auto [offset_v, width_v] = get_bf_offset_width(extw);
    int dn = extw >> 12 & 7;
    BFINS_M(ea_getaddr(TYPE(op), REG(op), 0), offset_v, width_v, cpu.D[dn]);
}

void fline_default(uint16_t) { FLINE(); }

void move16_inc_imm(uint16_t op) {
    uint32_t imm = FETCH32();
    Transfer16(cpu.A[REG(op)], imm);
    cpu.A[REG(op)] += 16;
}

void move16_imm_inc(uint16_t op) {
    uint32_t imm = FETCH32();
    Transfer16(imm, cpu.A[REG(op)]);
    cpu.A[REG(op)] += 16;
}

void move16_base_imm(uint16_t op) {
    uint32_t imm = FETCH32();
    Transfer16(cpu.A[REG(op)], imm);
}

void move16_imm_base(uint16_t op) {
    uint32_t imm = FETCH32();
    Transfer16(imm, cpu.A[REG(op)]);
}

void move16_inc_inc(uint16_t op) {
    uint16_t extw = FETCH();
    int ay = extw >> 12 & 7;
    Transfer16(cpu.A[REG(op)], cpu.A[ay]);
    cpu.A[ay] += 16;
    cpu.A[REG(op)] += 16;
}

} // namespace OP
run_t run_table[0x10000];

void init_run_table_mmu();
void init_run_table() {

    for(int i = 0; i < 0x10000; ++i) {
        run_table[i] = nullptr;
    }
    // Comon
    for(int ea = 0; ea < 075; ++ea) {
        run_table[0000000 | ea] = OP::ori_b;
        run_table[0000100 | ea] = OP::ori_w;
        run_table[0000200 | ea] = OP::ori_l;
        run_table[0001000 | ea] = OP::andi_b;
        run_table[0001100 | ea] = OP::andi_w;
        run_table[0001200 | ea] = OP::andi_l;
        run_table[0002000 | ea] = OP::subi_b;
        run_table[0002100 | ea] = OP::subi_w;
        run_table[0002200 | ea] = OP::subi_l;
        run_table[0003000 | ea] = OP::addi_b;
        run_table[0003100 | ea] = OP::addi_w;
        run_table[0003200 | ea] = OP::addi_l;
        run_table[0005000 | ea] = OP::eori_b;
        run_table[0005100 | ea] = OP::eori_w;
        run_table[0005200 | ea] = OP::eori_l;
        run_table[0006000 | ea] = OP::cmpi_b;
        run_table[0006100 | ea] = OP::cmpi_w;
        run_table[0006200 | ea] = OP::cmpi_l;

        run_table[0040000 | ea] = OP::negx_b;
        run_table[0040100 | ea] = OP::negx_w;
        run_table[0040200 | ea] = OP::negx_l;
        run_table[0040300 | ea] = OP::move_from_sr;

        run_table[0041000 | ea] = OP::clr_b;
        run_table[0041100 | ea] = OP::clr_w;
        run_table[0041200 | ea] = OP::clr_l;
        run_table[0041300 | ea] = OP::move_from_ccr;

        run_table[0042000 | ea] = OP::neg_b;
        run_table[0042100 | ea] = OP::neg_w;
        run_table[0042200 | ea] = OP::neg_l;
        run_table[0042300 | ea] = OP::move_to_ccr;

        run_table[0043000 | ea] = OP::not_b;
        run_table[0043100 | ea] = OP::not_w;
        run_table[0043200 | ea] = OP::not_l;
        run_table[0043300 | ea] = OP::move_to_sr;

        run_table[0044000 | ea] = OP::nbcd;
        run_table[0045000 | ea] = OP::tst_b;
        run_table[0045100 | ea] = OP::tst_w;
        run_table[0045200 | ea] = OP::tst_l;
        run_table[0045300 | ea] = OP::tas;

        run_table[0046000 | ea] = OP::mul_l;
        run_table[0046100 | ea] = OP::div_l;

        for(int d = 0; d < 8; ++d) {
            run_table[0010000 | d << 9 | ea] = OP::move_b;
            run_table[0020000 | d << 9 | ea] = OP::move_l;
            run_table[0030000 | d << 9 | ea] = OP::move_w;
            run_table[0020100 | d << 9 | ea] = OP::movea_l;
            run_table[0030100 | d << 9 | ea] = OP::movea_w;
            run_table[0040400 | d << 9 | ea] = OP::chk_l;
            run_table[0040600 | d << 9 | ea] = OP::chk_w;
            run_table[0050000 | d << 9 | ea] = OP::addq_b;
            run_table[0050100 | d << 9 | ea] = OP::addq_w;
            run_table[0050200 | d << 9 | ea] = OP::addq_l;
            run_table[0050400 | d << 9 | ea] = OP::subq_b;
            run_table[0050500 | d << 9 | ea] = OP::subq_w;
            run_table[0050600 | d << 9 | ea] = OP::subq_l;
            run_table[0100000 | d << 9 | ea] = OP::or_b_to_dn;
            run_table[0100100 | d << 9 | ea] = OP::or_w_to_dn;
            run_table[0100200 | d << 9 | ea] = OP::or_l_to_dn;
            run_table[0100300 | d << 9 | ea] = OP::divu_w;
            run_table[0100700 | d << 9 | ea] = OP::divs_w;

            run_table[0110000 | d << 9 | ea] = OP::sub_b_to_dn;
            run_table[0110100 | d << 9 | ea] = OP::sub_w_to_dn;
            run_table[0110200 | d << 9 | ea] = OP::sub_l_to_dn;
            run_table[0110300 | d << 9 | ea] = OP::suba_w;
            run_table[0110700 | d << 9 | ea] = OP::suba_l;

            run_table[0130000 | d << 9 | ea] = OP::cmp_b;
            run_table[0130100 | d << 9 | ea] = OP::cmp_w;
            run_table[0130200 | d << 9 | ea] = OP::cmp_l;
            run_table[0130300 | d << 9 | ea] = OP::cmpa_w;

            run_table[0130400 | d << 9 | ea] = OP::eor_b;
            run_table[0130500 | d << 9 | ea] = OP::eor_w;
            run_table[0130600 | d << 9 | ea] = OP::eor_l;
            run_table[0130700 | d << 9 | ea] = OP::cmpa_l;

            run_table[0140000 | d << 9 | ea] = OP::and_b_to_dn;
            run_table[0140100 | d << 9 | ea] = OP::and_w_to_dn;
            run_table[0140200 | d << 9 | ea] = OP::and_l_to_dn;
            run_table[0140300 | d << 9 | ea] = OP::mulu_w;
            run_table[0140700 | d << 9 | ea] = OP::muls_w;

            run_table[0150000 | d << 9 | ea] = OP::add_b_to_dn;
            run_table[0150100 | d << 9 | ea] = OP::add_w_to_dn;
            run_table[0150200 | d << 9 | ea] = OP::add_l_to_dn;
            run_table[0150300 | d << 9 | ea] = OP::adda_w;
            run_table[0150700 | d << 9 | ea] = OP::adda_l;
            for(int k = 2; k < 8; ++k) {
                run_table[0010000 | d << 9 | k << 6 | ea] = OP::move_b;
                run_table[0020000 | d << 9 | k << 6 | ea] = OP::move_l;
                run_table[0030000 | d << 9 | k << 6 | ea] = OP::move_w;
            }
        }
    }
    // EA = Dn
    for(int i = 0; i < 8; ++i) {
        run_table[0004000 | i] = OP::btst_l_i;
        run_table[0004100 | i] = OP::bchg_l_i;
        run_table[0004200 | i] = OP::bclr_l_i;
        run_table[0004300 | i] = OP::bset_l_i;

        run_table[0044100 | i] = OP::swap;
        run_table[0044200 | i] = OP::ext_w;
        run_table[0044300 | i] = OP::ext_l;
        run_table[0044700 | i] = OP::extb_l;

        run_table[0047100 | i] = OP::trap;

        run_table[0164300 | i] = OP::bftst_dn;
        run_table[0165300 | i] = OP::bfchg_dn;
        run_table[0166300 | i] = OP::bfclr_dn;
        run_table[0167300 | i] = OP::bfset_dn;

        run_table[0164700 | i] = OP::bfextu_dn;
        run_table[0165700 | i] = OP::bfexts_dn;
        run_table[0166700 | i] = OP::bfffo_dn;
        run_table[0167700 | i] = OP::bfins_dn;

        for(int d = 0; d < 8; ++d) {
            run_table[0000400 | d << 9 | i] = OP::btst_l_r;
            run_table[0000500 | d << 9 | i] = OP::bchg_l_r;
            run_table[0000600 | d << 9 | i] = OP::bclr_l_r;
            run_table[0000700 | d << 9 | i] = OP::bset_l_r;

            run_table[0100400 | d << 9 | i] = OP::sbcd_d;
            run_table[0100500 | d << 9 | i] = OP::pack_d;
            run_table[0100600 | d << 9 | i] = OP::unpk_d;
            run_table[0110400 | d << 9 | i] = OP::subx_b_d;
            run_table[0110500 | d << 9 | i] = OP::subx_w_d;
            run_table[0110600 | d << 9 | i] = OP::subx_l_d;

            run_table[0140400 | d << 9 | i] = OP::abcd_d;
            run_table[0140500 | d << 9 | i] = OP::exg_dn;
            run_table[0150400 | d << 9 | i] = OP::addx_b_d;
            run_table[0150500 | d << 9 | i] = OP::addx_w_d;
            run_table[0150600 | d << 9 | i] = OP::addx_l_d;
        }
        for(int c = 0; c < 16; ++c) {
            run_table[0050300 | c << 8 | i] = OP::scc_ea;
        }
    }

    // An
    for(int i = 0; i < 8; ++i) {
        for(int d = 0; d < 8; ++d) {
            run_table[0000410 | d << 9 | i] = OP::movep_w_load;
            run_table[0000510 | d << 9 | i] = OP::movep_l_load;
            run_table[0000610 | d << 9 | i] = OP::movep_w_store;
            run_table[0000710 | d << 9 | i] = OP::movep_l_store;

            run_table[0050110 | d << 9 | i] = OP::addq_an;
            run_table[0050210 | d << 9 | i] = OP::addq_an;
            run_table[0050510 | d << 9 | i] = OP::subq_an;
            run_table[0050610 | d << 9 | i] = OP::subq_an;

            run_table[0100410 | d << 9 | i] = OP::sbcd_m;
            run_table[0100510 | d << 9 | i] = OP::pack_m;
            run_table[0100610 | d << 9 | i] = OP::unpk_m;
            run_table[0110410 | d << 9 | i] = OP::subx_b_m;
            run_table[0110510 | d << 9 | i] = OP::subx_w_m;
            run_table[0110610 | d << 9 | i] = OP::subx_l_m;

            run_table[0130410 | d << 9 | i] = OP::cmpm_b;
            run_table[0130510 | d << 9 | i] = OP::cmpm_w;
            run_table[0130610 | d << 9 | i] = OP::cmpm_l;

            run_table[0140410 | d << 9 | i] = OP::abcd_m;
            run_table[0140510 | d << 9 | i] = OP::exg_an;
            run_table[0140610 | d << 9 | i] = OP::exg_da;
            run_table[0150410 | d << 9 | i] = OP::addx_b_m;
            run_table[0150510 | d << 9 | i] = OP::addx_w_m;
            run_table[0150610 | d << 9 | i] = OP::addx_l_m;
        }
        run_table[0044010 | i] = OP::link_l;
        run_table[0044110 | i] = OP::bkpt;
        run_table[0047110 | i] = OP::trap;
        for(int c = 0; c < 16; ++c) {
            run_table[0050310 | c << 8 | i] = OP::dbcc;
        }
    }
    for(int ea = 020; ea < 074; ++ea) {
        if(ea >= 030 && ea < 040) {
            // Increment only
            run_table[0046200 | ea] = OP::movem_w_load_incr;
            run_table[0046300 | ea] = OP::movem_l_load_incr;
        }
        if(ea >= 040 && ea < 050) {
            //  Decrement only
            run_table[0044200 | ea] = OP::movem_w_store_decr;
            run_table[0044300 | ea] = OP::movem_l_store_decr;
        }
        if(ea < 030 || ea >= 050) {
            // Ctl Only
            run_table[0000300 | ea] = OP::cmp2_chk2_b;
            run_table[0001300 | ea] = OP::cmp2_chk2_w;
            run_table[0002300 | ea] = OP::cmp2_chk2_l;
            run_table[0044100 | ea] = OP::pea;
            run_table[0044200 | ea] = OP::movem_w_store_base;
            run_table[0044300 | ea] = OP::movem_l_store_base;
            run_table[0046200 | ea] = OP::movem_w_load_base;
            run_table[0046300 | ea] = OP::movem_l_load_base;

            run_table[0047200 | ea] = OP::jsr;
            run_table[0047300 | ea] = OP::jmp;

            run_table[0164300 | ea] = OP::bftst_mem;
            run_table[0165300 | ea] = OP::bfchg_mem;
            run_table[0166300 | ea] = OP::bfclr_mem;
            run_table[0167300 | ea] = OP::bfset_mem;

            run_table[0164700 | ea] = OP::bfextu_mem;
            run_table[0165700 | ea] = OP::bfexts_mem;
            run_table[0166700 | ea] = OP::bfffo_mem;
            run_table[0167700 | ea] = OP::bfins_mem;
            for(int d = 0; d < 8; ++d) {
                run_table[0040700 | d << 9 | ea] = OP::lea;
            }
        }
        run_table[0004000 | ea] = OP::btst_b_i;
        run_table[0004100 | ea] = OP::bchg_b_i;
        run_table[0004200 | ea] = OP::bclr_b_i;
        run_table[0004300 | ea] = OP::bset_b_i;
        run_table[0005300 | ea] = OP::cas_b;
        run_table[0006300 | ea] = OP::cas_w;
        run_table[0007000 | ea] = OP::moves_b;
        run_table[0007100 | ea] = OP::moves_w;
        run_table[0007200 | ea] = OP::moves_l;
        run_table[0007300 | ea] = OP::cas_l;
        run_table[0160300 | ea] = OP::asr_ea;
        run_table[0161300 | ea] = OP::lsr_ea;
        run_table[0162300 | ea] = OP::roxr_ea;
        run_table[0163300 | ea] = OP::ror_ea;

        run_table[0160700 | ea] = OP::asl_ea;
        run_table[0161700 | ea] = OP::lsl_ea;
        run_table[0162700 | ea] = OP::roxl_ea;
        run_table[0163700 | ea] = OP::rol_ea;

        for(int d = 0; d < 8; ++d) {
            run_table[0000400 | d << 9 | ea] = OP::btst_b_r;
            run_table[0000500 | d << 9 | ea] = OP::bchg_b_r;
            run_table[0000600 | d << 9 | ea] = OP::bclr_b_r;
            run_table[0000700 | d << 9 | ea] = OP::bset_b_r;

            run_table[0100400 | d << 9 | ea] = OP::or_b_to_ea;
            run_table[0100500 | d << 9 | ea] = OP::or_w_to_ea;
            run_table[0100600 | d << 9 | ea] = OP::or_l_to_ea;

            run_table[0110400 | d << 9 | ea] = OP::sub_b_to_ea;
            run_table[0110500 | d << 9 | ea] = OP::sub_w_to_ea;
            run_table[0110600 | d << 9 | ea] = OP::sub_l_to_ea;

            run_table[0140400 | d << 9 | ea] = OP::and_b_to_ea;
            run_table[0140500 | d << 9 | ea] = OP::and_w_to_ea;
            run_table[0140600 | d << 9 | ea] = OP::and_l_to_ea;

            run_table[0150400 | d << 9 | ea] = OP::add_b_to_ea;
            run_table[0150500 | d << 9 | ea] = OP::add_w_to_ea;
            run_table[0150600 | d << 9 | ea] = OP::add_l_to_ea;
        }
        for(int c = 0; c < 16; ++c) {
            if(ea < 072) {
                run_table[0050300 | c << 8 | ea] = OP::scc_ea;
            }
        }
    }
    run_table[0000074] = OP::ori_b_ccr;
    run_table[0000174] = OP::ori_w_sr;
    run_table[0001074] = OP::andi_b_ccr;
    run_table[0001174] = OP::andi_w_sr;
    run_table[0004074] = OP::btst_b_i_imm;
    run_table[0005074] = OP::eori_b_ccr;
    run_table[0005174] = OP::eori_w_sr;
    run_table[0006374] = OP::cas2_w;
    run_table[0007374] = OP::cas2_l;
    run_table[0042374] = OP::move_to_ccr;
    run_table[0043374] = OP::move_to_sr;
    run_table[0045374] = OP::illegal;

    for(int d = 0; d < 8; ++d) {
        run_table[0000474 | d << 9] = OP::btst_b_r_imm;
        run_table[0020074 | d << 9] = OP::move_b;
        run_table[0020074 | d << 9] = OP::move_l;
        run_table[0030074 | d << 9] = OP::move_w;
        run_table[0020174 | d << 9] = OP::movea_l;
        run_table[0030174 | d << 9] = OP::movea_w;
        for(int k = 2; k < 8; ++k) {
            run_table[0020074 | d << 9 | k << 6] = OP::move_b;
            run_table[0020074 | d << 9 | k << 6] = OP::move_l;
            run_table[0030074 | d << 9 | k << 6] = OP::move_w;
        }
    }
    for(int d = 0; d < 8; ++d) {
        run_table[0047120 | d] = OP::link_w;
        run_table[0047130 | d] = OP::unlk;
        run_table[0047140 | d] = OP::move_to_usp;
        run_table[0047150 | d] = OP::move_from_usp;

        for(int i = 0; i < 0x100; ++i) {
            run_table[0070000 | d << 9 | i] = OP::moveq;
        }
    }

    run_table[0047160] = OP::reset;
    run_table[0047161] = OP::nop;
    run_table[0047162] = OP::stop;
    run_table[0047163] = OP::rte;
    run_table[0047164] = OP::rtd;
    run_table[0047165] = OP::rts;
    run_table[0047166] = OP::trapv;
    run_table[0047167] = OP::rtr;

    run_table[0047172] = OP::movec_from_cr;
    run_table[0047173] = OP::movec_to_cr;

    for(int c = 0; c < 16; ++c) {
        run_table[0050372 | c << 8] = OP::trapcc;
        run_table[0050373 | c << 8] = OP::trapcc;
        run_table[0050374 | c << 8] = OP::trapcc;
    }
    for(int v = 0; v < 0x1000; ++v) {
        run_table[0060000 | v] = OP::bxx;
        run_table[0120000 | v] = OP::aline_ex;
        run_table[0170000 | v] = OP::fline_default;
    }
    for(int im = 0; im < 8; ++im) {
        for(int reg = 0; reg < 8; ++reg) {
            run_table[0160000 | im << 9 | reg] = OP::asr_b_i;
            run_table[0160010 | im << 9 | reg] = OP::lsr_b_i;
            run_table[0160020 | im << 9 | reg] = OP::roxr_b_i;
            run_table[0160030 | im << 9 | reg] = OP::ror_b_i;
            run_table[0160040 | im << 9 | reg] = OP::asr_b_r;
            run_table[0160050 | im << 9 | reg] = OP::lsr_b_r;
            run_table[0160060 | im << 9 | reg] = OP::roxr_b_r;
            run_table[0160070 | im << 9 | reg] = OP::ror_b_r;

            run_table[0160100 | im << 9 | reg] = OP::asr_w_i;
            run_table[0160110 | im << 9 | reg] = OP::lsr_w_i;
            run_table[0160120 | im << 9 | reg] = OP::roxr_w_i;
            run_table[0160130 | im << 9 | reg] = OP::ror_w_i;
            run_table[0160140 | im << 9 | reg] = OP::asr_w_r;
            run_table[0160150 | im << 9 | reg] = OP::lsr_w_r;
            run_table[0160160 | im << 9 | reg] = OP::roxr_w_r;
            run_table[0160170 | im << 9 | reg] = OP::ror_w_r;

            run_table[0160200 | im << 9 | reg] = OP::asr_l_i;
            run_table[0160210 | im << 9 | reg] = OP::lsr_l_i;
            run_table[0160220 | im << 9 | reg] = OP::roxr_l_i;
            run_table[0160230 | im << 9 | reg] = OP::ror_l_i;
            run_table[0160240 | im << 9 | reg] = OP::asr_l_r;
            run_table[0160250 | im << 9 | reg] = OP::lsr_l_r;
            run_table[0160260 | im << 9 | reg] = OP::roxr_l_r;
            run_table[0160270 | im << 9 | reg] = OP::ror_l_r;

            run_table[0160400 | im << 9 | reg] = OP::asl_b_i;
            run_table[0160410 | im << 9 | reg] = OP::lsl_b_i;
            run_table[0160420 | im << 9 | reg] = OP::roxl_b_i;
            run_table[0160430 | im << 9 | reg] = OP::rol_b_i;
            run_table[0160440 | im << 9 | reg] = OP::asl_b_r;
            run_table[0160450 | im << 9 | reg] = OP::lsl_b_r;
            run_table[0160460 | im << 9 | reg] = OP::roxl_b_r;
            run_table[0160470 | im << 9 | reg] = OP::rol_b_r;

            run_table[0160500 | im << 9 | reg] = OP::asl_w_i;
            run_table[0160510 | im << 9 | reg] = OP::lsl_w_i;
            run_table[0160520 | im << 9 | reg] = OP::roxl_w_i;
            run_table[0160530 | im << 9 | reg] = OP::rol_w_i;
            run_table[0160540 | im << 9 | reg] = OP::asl_w_r;
            run_table[0160550 | im << 9 | reg] = OP::lsl_w_r;
            run_table[0160560 | im << 9 | reg] = OP::roxl_w_r;
            run_table[0160570 | im << 9 | reg] = OP::rol_w_r;

            run_table[0160600 | im << 9 | reg] = OP::asl_l_i;
            run_table[0160610 | im << 9 | reg] = OP::lsl_l_i;
            run_table[0160620 | im << 9 | reg] = OP::roxl_l_i;
            run_table[0160630 | im << 9 | reg] = OP::rol_l_i;
            run_table[0160640 | im << 9 | reg] = OP::asl_l_r;
            run_table[0160650 | im << 9 | reg] = OP::lsl_l_r;
            run_table[0160660 | im << 9 | reg] = OP::roxl_l_r;
            run_table[0160670 | im << 9 | reg] = OP::rol_l_r;
        }
    }

    for(int i = 0; i < 8; ++i) {
        run_table[0173000 | i] = OP::move16_inc_imm;
        run_table[0173010 | i] = OP::move16_imm_inc;
        run_table[0173020 | i] = OP::move16_base_imm;
        run_table[0173030 | i] = OP::move16_imm_base;
        run_table[0173040 | i] = OP::move16_inc_inc;
    }
    if( cpu.fpu ) {
        cpu.fpu->init_table();
    }
    init_run_table_mmu();
#ifdef CI
    run_table[0044117] = OP::test_exit;
#endif
}
