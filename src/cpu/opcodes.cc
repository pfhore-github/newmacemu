#include "68040.hpp"
#include "bus.hpp"
#include "exception.hpp"
#include "inline.hpp"
#include "memory.hpp"
#include <memory>
#include <utility>
void bus_reset();

static std::pair<std::function<void()>, int> bitimm(int type, int reg,
                                                    uint8_t imm, int o) {
    if(type == 0) {
        imm &= 31;
        switch(o) {
        case 0:
            // BTST.L
            return {[reg, imm, o]() { BSET_L(cpu.D[reg], imm); }, 2};
        case 1:
            // BCHG.L
            return {[reg, imm, o]() { cpu.D[reg] = BCHG_L(cpu.D[reg], imm); },
                    2};
        case 2:
            // BCLR.L
            return {[reg, imm, o]() { cpu.D[reg] = BCLR_L(cpu.D[reg], imm); },
                    2};
        case 3:
            // BSET.L
            return {[reg, imm, o]() { cpu.D[reg] = BSET_L(cpu.D[reg], imm); },
                    2};
        }
    } else {
         imm &= 7;
        if(o == 0) {
            // BTST.B
            auto [r, off] = ea_read8(type, reg, cpu.PC + 4);
            return {[r, imm]() { BTST_B(r(), imm); }, off + 2};
        } else {
            auto [r, w, off] = ea_rw8(type, reg, cpu.PC + 4);
            switch(o) {
            case 1:
                // BCHG.B
                return {[r, w, imm]() { w(BCHG_B(r(), imm)); }, off + 2};
            case 2:
                // BCLR.B
                return {[r, w, imm]() { w(BCLR_B(r(), imm)); }, off + 2};
            case 3:
                // BSET.B
                return {[r, w, imm]() { w(BSET_B(r(), imm)); }, off + 2};
            }
        }
    }
    throw DecodeError{};
}
std::pair<std::function<void()>, int> decode_ea(int type, int rn, uint32_t pc,
                                                SIZ sz, bool w);
static std::pair<std::function<void()>, int> decode00(int dn, int type,
                                                      int reg) {
    uint8_t imm = FETCH(cpu.PC + 2);
    if(dn == 4) {
        return bitimm(type, reg, imm, 0);
    } else if(dn == 7) {
        int rn = imm >> 12 & 7;
        if(imm & 1 << 11) {
            // MOVES.W TO REG
            auto [addr, off] = ea_addr(type, reg, cpu.PC + 2, 1, true);
            return {[rn, addr]() {
                        if(!cpu.S) {
                            PRIV_ERROR();
                        }
                        BusWriteB(cpu.EA = addr(), cpu.R(rn));
                    },
                    off};
        } else {
            auto [addr, off] = ea_addr(type, reg, cpu.PC + 2, 1, false);
            // MOVES.W TO MEM
            return {[rn, addr]() {
                        if(!cpu.S) {
                            PRIV_ERROR();
                        }
                        cpu.R(rn) = BusReadB(cpu.EA = addr());
                    },
                    off};
        }
    }
    if(type == 7 && reg == 4) {
        switch(dn) {
        case 0:
            // ORI TO CCR
            return {[imm]() { SetCCR(GetCCR() | imm); }, 2};
        case 1:
            // ANDI TO CCR
            return {[imm]() { SetCCR(GetCCR() & imm); }, 2};
        case 5:
            // EORI TO CCR
            return {[imm]() { SetCCR(GetCCR() ^ imm); }, 2};
        default:
            throw DecodeError{};
        }
    }
    if(dn == 6) {
        // CMPI.B
        auto [r, off] = ea_read8(type, reg, cpu.PC + 2);
        return {[r, imm]() { CMP_B(r(), imm); }, off};
    }
    auto [r, w, off] = ea_rw8(type, reg, cpu.PC + 2);
    switch(dn) {
    case 0:
        // ORI.B
        return {[r, w, imm]() { w(OR_B(r(), imm)); }, off+2};
    case 1:
        // ANDI.B
        return {[r, w, imm]() { w(AND_B(r(), imm)); }, off+2};
    case 2:
        // SUBI.B
        return {[r, w, imm]() { w(SUB_B(r(), imm)); }, off+2};
    case 3:
        // ADDI.B
        return {[r, w, imm]() { w(ADD_B(r(), imm)); }, off+2};
    case 5:
        // EORI.B
        return {[r, w, imm]() { w(XOR_B(r(), imm)); }, off+2};
    }
    throw DecodeError{};
}
void MOVEM_W_TO_MEM_DECR(uint16_t regs, uint8_t reg);
void MOVEM_W_TO_MEM_ADDR(uint16_t regs);
void MOVEM_W_FROM_MEM_INCR(uint16_t regs, uint8_t reg);
void MOVEM_W_FROM_MEM_ADDR(uint16_t regs);
void MOVEM_L_TO_MEM_DECR(uint16_t regs, uint8_t reg);
void MOVEM_L_TO_MEM_ADDR(uint16_t regs);
void MOVEM_L_FROM_MEM_INCR(uint16_t regs, uint8_t reg);
void MOVEM_L_FROM_MEM_ADDR(uint16_t regs);
static std::pair<std::function<void()>, int> decode01(int dn, int type,
                                                      int reg) {
    uint16_t imm = FETCH(cpu.PC + 2);
    if(dn == 4) {
        return bitimm(type, reg, imm, 1);
    } else if(dn == 7) {
        int rn = imm >> 12 & 7;
        if(imm & 1 << 11) {
            auto [addr, off] = ea_addr(type, reg, cpu.PC + 2, 2, true);
            // MOVES.W TO REG
            return {[rn, addr]() {
                        if(!cpu.S) {
                            PRIV_ERROR();
                        }
                        BusWriteW(cpu.EA = addr(), cpu.R(rn));
                    },
                    off};
        } else {
            auto [addr, off] = ea_addr(type, reg, cpu.PC + 2, 2, false);
            // MOVES.W TO MEM
            return {[rn, addr]() {
                        if(!cpu.S) {
                            PRIV_ERROR();
                        }
                        cpu.R(rn) = BusReadW(cpu.EA = addr());
                    },
                    off};
        }
    }
    if(type == 7 && reg == 4) {
        switch(dn) {
        case 0:
            // ORI TO CCR
            return {[imm]() { SetSR(GetSR() | imm); }, 2};
        case 1:
            // ANDI TO CCR
            return {[imm]() { SetSR(GetSR() & imm); }, 2};
        case 5:
            // EORI TO CCR
            return {[imm]() { SetSR(GetSR() ^ imm); }, 2};
        default:
            throw DecodeError{};
        }
    }
    if(dn == 6) {
        // CMPI.W
        auto [r, off] = ea_read16(type, reg, cpu.PC + 2);
        return {[r, imm]() { CMP_W(r(), imm); }, off};

    } else {
        auto [r, w, off] = ea_rw16(type, reg, cpu.PC + 2);
        switch(dn) {
        case 0:
            // ORI.W
            return {[r, w, imm]() { w(OR_W(r(), imm)); }, off+2};
        case 1:
            // ANDI.W
            return {[r, w, imm]() { w(AND_W(r(), imm)); }, off+2};
        case 2:
            // SUBI.W
            return {[r, w, imm]() { w(SUB_W(r(), imm)); }, off+2};
        case 3:
            // ADDI.W
            return {[r, w, imm]() { w(ADD_W(r(), imm)); }, off+2};
        case 5:
            // EORI.W
            return {[r, w, imm]() { w(XOR_W(r(), imm)); }, off+2};
        }
    }
    throw DecodeError{};
}

static std::pair<std::function<void()>, int> decode02(int dn, int type,
                                                      int reg) {
    if(dn == 4) {
        uint8_t imm = FETCH(cpu.PC + 2);
        return bitimm(type, reg, imm, 2);
    } else if(dn == 7) {
        uint16_t imm = FETCH(cpu.PC + 2);
        int rn = imm >> 12 & 7;
        if(imm & 1 << 11) {
        auto [addr, off] = ea_addr(type, reg, cpu.PC + 2, 4, true);
            // MOVES.L TO REG
            return {[rn, addr]() {
                        if(!cpu.S) {
                            PRIV_ERROR();
                        }
                        BusWriteL(cpu.EA = addr(), cpu.R(rn));
                    },
                    off+2};
        } else {
        auto [addr, off] = ea_addr(type, reg, cpu.PC + 2, 4, false);
            // MOVES.L TO MEM
            return {[rn, addr]() {
                        if(!cpu.S) {
                            PRIV_ERROR();
                        }
                        cpu.R(rn) = BusReadL(cpu.EA = addr());
                    },
                    off+2};
        }
    }
    uint32_t imm = FETCH32(cpu.PC + 2);
    if(dn == 6) {
        // CMPI.L
        auto [r, off] = ea_read32(type, reg, cpu.PC + 2);
        return {[r, imm]() { CMP_L(r(), imm); }, off+4};

    } else {
        auto [r, w, off] = ea_rw32(type, reg, cpu.PC + 2);
        switch(dn) {
        case 0:
            // ORI.L
            return {[r, w, imm]() { w(OR_L(r(), imm)); }, off+4};
        case 1:
            // ANDI.L
            return {[r, w, imm]() { w(AND_L(r(), imm)); }, off+4};
        case 2:
            // SUBI.L
            return {[r, w, imm]() { w(SUB_L(r(), imm)); }, off+4};
        case 3:
            // ADDI.L
            return {[r, w, imm]() { w(ADD_L(r(), imm)); }, off+4};
        case 5:
            // EORI.L
            return {[r, w, imm]() { w(XOR_L(r(), imm)); }, off+4};
        }
    }
    throw DecodeError{};
}

static std::pair<std::function<void()>, int> decode03(int dn, int type,
                                                      int reg) {
    switch(dn) {
    case 0: {
        // CMP2/CHK2.B
        uint16_t nw = FETCH(cpu.PC + 2);
        int rn = nw >> 12 & 7;
        bool is_an = nw & 1 << 15;
        bool chk = nw & 1 << 11;
        auto [addr, of] = ea_addr(type, reg, cpu.PC + 4, 0, false);
        if(is_an) {
            return {[addr, rn, chk]() {
                        cpu.EA = addr();
                        int32_t v = cpu.A[rn];
                        int32_t lo = static_cast<int8_t>(MMU_ReadB(cpu.EA));
                        int32_t hi = static_cast<int8_t>(MMU_ReadB(cpu.EA + 1));
                        cpu.Z = (lo == v) || (hi == v);
                        cpu.C = (lo > v) || (hi < v);
                        if(chk && cpu.C) {
                            CHK_ERROR();
                        }
                    },
                    of + 2};
        } else {
            return {[addr, rn, chk]() {
                        cpu.EA = addr();
                        uint8_t v = cpu.A[rn];
                        auto lo = MMU_ReadB(cpu.EA);
                        auto hi = MMU_ReadB(cpu.EA + 1);
                        cpu.Z = (lo == v) || (hi == v);
                        cpu.C = (lo > v) || (hi < v);
                        if(chk && cpu.C) {
                            CHK_ERROR();
                        }
                    },
                    of + 2};
        }
    }
    case 1: {
        // CMP2/CHK2.W
        uint16_t nw = FETCH(cpu.PC + 2);
        int rn = nw >> 12 & 7;
        bool is_an = nw & 1 << 15;
        bool chk = nw & 1 << 11;
        auto [addr, of] = ea_addr(type, reg, cpu.PC + 4, 0, false);
        if(is_an) {
            return {[addr, rn, chk]() {
                        cpu.EA = addr();
                        int32_t v = cpu.A[rn];
                        int32_t lo = static_cast<int16_t>(MMU_ReadW(cpu.EA));
                        int32_t hi =
                            static_cast<int16_t>(MMU_ReadW(cpu.EA + 2));
                        cpu.Z = (lo == v) || (hi == v);
                        cpu.C = (lo > v) || (hi < v);
                        if(chk && cpu.C) {
                            CHK_ERROR();
                        }
                    },
                    of + 2};
        } else {
            return {[addr, rn, chk]() {
                        cpu.EA = addr();
                        uint16_t v = cpu.A[rn];
                        auto lo = MMU_ReadW(cpu.EA);
                        auto hi = MMU_ReadW(cpu.EA + 2);
                        cpu.Z = (lo == v) || (hi == v);
                        cpu.C = (lo > v) || (hi < v);
                        if(chk && cpu.C) {
                            CHK_ERROR();
                        }
                    },
                    of + 2};
        }
    }
    case 2: {
        // CMP2/CHK2.L
        uint16_t nw = FETCH(cpu.PC + 2);
        int rn = nw >> 12 & 7;
        bool is_an = nw & 1 << 15;
        bool chk = nw & 1 << 11;
        auto [addr, of] = ea_addr(type, reg, cpu.PC + 4, 0, false);
        if(is_an) {
            return {[addr, rn, chk]() {
                        cpu.EA = addr();
                        int32_t v = cpu.A[rn];
                        int32_t lo = MMU_ReadL(cpu.EA);
                        int32_t hi = MMU_ReadL(cpu.EA + 4);
                        cpu.Z = (lo == v) || (hi == v);
                        cpu.C = (lo > v) || (hi < v);
                        if(chk && cpu.C) {
                            CHK_ERROR();
                        }
                    },
                    of + 2};
        } else {
            return {[addr, rn, chk]() {
                        cpu.EA = addr();
                        uint32_t v = cpu.A[rn];
                        auto lo = MMU_ReadL(cpu.EA);
                        auto hi = MMU_ReadL(cpu.EA + 4);
                        cpu.Z = (lo == v) || (hi == v);
                        cpu.C = (lo > v) || (hi < v);
                        if(chk && cpu.C) {
                            CHK_ERROR();
                        }
                    },
                    of + 2};
        }
    }
    case 3:
        throw DecodeError{};
    case 4: {
        uint8_t imm = FETCH(cpu.PC + 2);
        return bitimm(type, reg, imm, 3);
    }
    case 5:
        // CAS.B
        {
            uint16_t imm = FETCH(cpu.PC + 2);
            int du = imm >> 6 & 7;
            int dc = imm & 7;
            auto [addr, of] = ea_addr(type, reg, cpu.PC + 4, 1, true);
            return {[addr, du, dc]() {
                        cpu.EA = addr();
                        uint8_t v = MMU_ReadB(cpu.EA);
                        CMP_B(v, cpu.D[dc]);
                        if(cpu.Z) {
                            MMU_WriteB(cpu.EA, cpu.D[du]);
                        } else {
                            cpu.D[dc] = v;
                        }
                    },
                    of + 2};
        }
    case 6:
        // CAS.W/CAS2.W
        {
            uint16_t imm = FETCH(cpu.PC + 2);
            int du = imm >> 6 & 7;
            int dc = imm & 7;
            if(type == 7 && reg == 4) {
                int rn1 = imm >> 12 & 15;
                uint16_t imm2 = FETCH(cpu.PC + 4);
                int rn2 = imm2 >> 12 & 15;
                int du2 = imm2 >> 6 & 7;
                int dc2 = imm2 & 7;
                return {[rn1, rn2, dc, du, dc2, du2]() {
                            uint16_t v1 = MMU_ReadW(cpu.R(rn1));
                            uint16_t v2 = MMU_ReadW(cpu.R(rn2));
                            CMP_W(v1, cpu.D[dc]);
                            if(cpu.Z) {
                                CMP_W(v2, cpu.D[dc2]);
                            }
                            if(cpu.Z) {
                                MMU_WriteW(cpu.R(rn1), cpu.D[du]);
                                MMU_WriteW(cpu.R(rn2), cpu.D[du2]);
                            } else {
                                STORE_W(cpu.D[dc], v1);
                                STORE_W(cpu.D[dc2], v2);
                            }
                        },
                        4};
            } else {
                auto [addr, of] = ea_addr(type, reg, cpu.PC + 4, 2, true);
                return {[addr, du, dc]() {
                            cpu.EA = addr();
                            uint16_t v = MMU_ReadW(cpu.EA);
                            CMP_W(v, cpu.D[dc]);
                            if(cpu.Z) {
                                MMU_WriteW(cpu.EA, cpu.D[du]);
                            } else {
                                cpu.D[dc] = v;
                            }
                        },
                        of + 2};
            }
        }

    case 7:
        // CAS.L/CAS2.L
        {
            uint16_t imm = FETCH(cpu.PC + 2);
            int du = imm >> 6 & 7;
            int dc = imm & 7;
            if(type == 7 && reg == 4) {
                int rn1 = imm >> 12 & 15;
                uint16_t imm2 = FETCH(cpu.PC + 4);
                int rn2 = imm2 >> 12 & 15;
                int du2 = imm2 >> 6 & 7;
                int dc2 = imm2 & 7;
                return {[rn1, rn2, dc, du, dc2, du2]() {
                            uint32_t v1 = MMU_ReadL(cpu.R(rn1));
                            uint32_t v2 = MMU_ReadL(cpu.R(rn2));
                            CMP_L(v1, cpu.D[dc]);
                            if(cpu.Z) {
                                CMP_L(v2, cpu.D[dc2]);
                            }
                            if(cpu.Z) {
                                MMU_WriteL(cpu.R(rn1), cpu.D[du]);
                                MMU_WriteL(cpu.R(rn2), cpu.D[du2]);
                            } else {
                                cpu.D[dc] = v1;
                                cpu.D[dc2] = v2;
                            }
                        },
                        4};
            } else {
                auto [addr, of] = ea_addr(type, reg, cpu.PC + 4, 4, true);
                return {[addr, du, dc]() {
                            cpu.EA = addr();
                            uint32_t v = MMU_ReadL(cpu.EA);
                            CMP_L(v, cpu.D[dc]);
                            if(cpu.Z) {
                                MMU_WriteL(cpu.EA, cpu.D[du]);
                            } else {
                                cpu.D[dc] = v;
                            }
                        },
                        of + 2};
            }
        }
    }
    throw DecodeError{};
}
static std::pair<std::function<void()>, int> decode04(int dn, int type,
                                                      int reg) {
    if(type == 0) {
        // BTST.L
        return {[dn, reg]() { BSET_L(cpu.D[reg], cpu.D[dn] & 31); }, 0};
    } else if(type == 1) {
        // MOVEP.W TO
        int16_t imm = FETCH(cpu.PC + 2);
        return {[dn, reg, imm]() {
                    uint32_t base = cpu.A[reg] + imm;
                    STORE_W(cpu.D[dn],
                            MMU_ReadB(base) << 8 | MMU_ReadB(base + 2));
                },
                2};
    } else {
        auto [r, off] = ea_read8(type, reg, cpu.PC + 2);
        return {[r, dn]() { BTST_B(r(), cpu.D[dn] & 7); }, off};
    }
}

static std::pair<std::function<void()>, int> decode05(int dn, int type,
                                                      int reg) {
    if(type == 0) {
        // BTST.L
        return {
            [dn, reg]() { cpu.D[reg] = BCHG_L(cpu.D[reg], cpu.D[dn] & 31); },
            0};
    } else if(type == 1) {
        // MOVEP.L TO
        int16_t imm = FETCH(cpu.PC + 2);
        return {[dn, reg, imm]() {
                    uint32_t base = cpu.A[reg] + imm;
                    cpu.D[dn] = MMU_ReadB(base) << 24 |
                                MMU_ReadB(base + 2) << 16 |
                                MMU_ReadB(base + 4) << 8 | MMU_ReadB(base + 6);
                },
                2};
    } else {
        auto [r, w, off] = ea_rw8(type, reg, cpu.PC + 2);
        return {[r, w, dn]() { w(BCHG_B(r(), cpu.D[dn] & 7)); }, off};
    }
}

static std::pair<std::function<void()>, int> decode06(int dn, int type,
                                                      int reg) {
    if(type == 0) {
        // BTST.L
        return {
            [dn, reg]() { cpu.D[reg] = BCLR_L(cpu.D[reg], cpu.D[dn] & 31); },
            0};
    } else if(type == 1) {
        // MOVEP.W FROM
        int16_t imm = FETCH(cpu.PC + 2);
        return {[dn, reg, imm]() {
                    uint32_t base = cpu.A[reg] + imm;
                    MMU_WriteB(base, cpu.D[dn] >> 8);
                    MMU_WriteB(base + 2, cpu.D[dn]);
                },
                2};
    } else {
        auto [r, w, off] = ea_rw8(type, reg, cpu.PC + 2);
        return {[r, w, dn]() { w(BCLR_B(r(), cpu.D[dn] & 7)); }, off};
    }
}

static std::pair<std::function<void()>, int> decode07(int dn, int type,
                                                      int reg) {
    if(type == 0) {
        // BTST.L
        return {
            [dn, reg]() { cpu.D[reg] = BSET_L(cpu.D[reg], cpu.D[dn] & 31); },
            0};
    } else if(type == 1) {
        // MOVEP.W FROM
        int16_t imm = FETCH(cpu.PC + 2);
        return {[dn, reg, imm]() {
                    uint32_t base = cpu.A[reg] + imm;
                    MMU_WriteB(base, cpu.D[dn] >> 24);
                    MMU_WriteB(base + 2, cpu.D[dn] >> 16);
                    MMU_WriteB(base + 4, cpu.D[dn] >> 8);
                    MMU_WriteB(base + 6, cpu.D[dn]);
                },
                2};
    } else {
        auto [r, w, off] = ea_rw8(type, reg, cpu.PC + 2);
        return {[r, w, dn]() { w(BSET_B(r(), cpu.D[dn] & 7)); }, off};
    }
}

std::pair<std::function<void()>, int> decode_00(int dn, int sz, int type,
                                                int reg) {
    switch(sz) {
    case 0:
        return decode00(dn, type, reg);
    case 1:
        return decode01(dn, type, reg);
    case 2:
        return decode02(dn, type, reg);
    case 3:
        return decode03(dn, type, reg);
    case 4:
        return decode04(dn, type, reg);
    case 5:
        return decode05(dn, type, reg);
    case 6:
        return decode06(dn, type, reg);
    case 7:
        return decode07(dn, type, reg);
    }
    throw DecodeError{};
}
void RTE();
std::function<void()> from_cc(int reg, int cc);
std::function<void()> to_cc(int reg, int cc);
std::pair<std::function<void()>, int> decode_op4(int dn, int sz, int type,
                                                 int reg) {
    switch(sz) {
    case 0:
        switch(dn) {
        case 0: {
            // NEGX.B
            auto [r, w, off] = ea_rw8(type, reg, cpu.PC + 2);
            return {[r, w]() { w(SUB_B(0, r() + cpu.X)); }, off};
        }
        case 1: {
            // CLR.B
            auto [w, off] = ea_write8(type, reg, cpu.PC + 2);
            return {[w]() {
                        cpu.N = cpu.Z = cpu.V = cpu.C = false;
                        w(0);
                    },
                    off};
        }
        case 2: {
            // NEG.B
            auto [r, w, off] = ea_rw8(type, reg, cpu.PC + 2);
            return {[r, w]() { w(SUB_B(0, r())); }, off};
        }
        case 3: {
            // NOT.B
            auto [r, w, off] = ea_rw8(type, reg, cpu.PC + 2);
            return {[r, w]() {
                        uint8_t v = ~r();
                        TEST_B(v);
                        w(v);
                    },
                    off};
        }
        case 4:
            if(type == 1) {
                // LINK.L
                int32_t disp = FETCH32(cpu.PC + 2);
                return {[disp, reg]() {
                            PUSH32(cpu.A[reg]);
                            cpu.A[reg] = cpu.A[7];
                            cpu.A[7] += disp;
                        },
                        4};
            } else {
                // NBCD
                auto [r, w, off] = ea_rw8(type, reg, cpu.PC + 2);
                return {[r, w]() {
                            uint8_t v = BCD2BIN(r());
                            v = 100 - v - cpu.X;
                            cpu.X = cpu.C = (v != 0);
                            if(v) {
                                cpu.Z = false;
                            }
                            w(BIN2BCD(v));
                        },
                        off};
            }
        case 5:
            // TST.B
            {
                auto [r, off] = ea_read8(type, reg, cpu.PC + 2);
                return {[r]() { TEST_B(r()); }, off};
            }
        case 6: {
            uint16_t next = FETCH(cpu.PC + 2);
            auto [r, off] = ea_read32(type, reg, cpu.PC + 4);
            int dl = next >> 12 & 7;
            int dh = next & 7;
            bool sg = next & 1 << 11;
            bool lg = next & 1 << 10;
            if(sg) {
                if(lg) {
                    // MULS.L <EA>, Dh-Dl
                    return {[dh, dl, r]() {
                                int64_t v = MULS_LL(cpu.D[dl], r());
                                cpu.D[dh] = v >> 32;
                                cpu.D[dl] = v & 0xffffffff;
                            },
                            off + 2};
                } else {
                    // MULS.L <EA>, Dl
                    return {
                        [dh, dl, r]() { cpu.D[dl] = MULS_L(cpu.D[dl], r()); },
                        off + 2};
                }
            } else {
                if(lg) {
                    // MULU.L <EA>, Dh-Dl
                    return {[dh, dl, r]() {
                                uint64_t v = MULU_LL(cpu.D[dl], r());
                                cpu.D[dh] = v >> 32;
                                cpu.D[dl] = v & 0xffffffff;
                            },
                            off + 2};
                } else {
                    // MULU.L <EA>, Dl
                    return {
                        [dh, dl, r]() { cpu.D[dl] = MULU_L(cpu.D[dl], r()); },
                        off + 2};
                }
            }
        }
        case 7:
            throw DecodeError{};
        }
        break;
    case 1:
        switch(dn) {
        case 0: {
            // NEGX.W
            auto [r, w, off] = ea_rw16(type, reg, cpu.PC + 2);
            return {[r, w]() { w(SUB_W(0, r() + cpu.X)); }, off};
        }
        case 1: {
            // CLR.W
            auto [w, off] = ea_write16(type, reg, cpu.PC + 2);
            return {[w]() {
                        cpu.N = cpu.Z = cpu.V = cpu.C = false;
                        w(0);
                    },
                    off};
        }
        case 2: {
            // NEG.W
            auto [r, w, off] = ea_rw16(type, reg, cpu.PC + 2);
            return {[r, w]() { w(SUB_W(0, r())); }, off};
        }
        case 3: {
            // NOT.W
            auto [r, w, off] = ea_rw16(type, reg, cpu.PC + 2);
            return {[r, w]() {
                        uint16_t v = ~r();
                        TEST_W(v);
                        w(v);
                    },
                    off};
        }
        case 4:
            if(type == 0) {
                // SWAP
                return {[reg]() {
                            uint32_t v = cpu.D[reg] >> 16 | cpu.D[reg] << 16;
                            cpu.V = cpu.C = false;
                            TEST_L(v);
                            cpu.D[reg] = v;
                        },
                        0};
            } else if(type == 1) {
                // BKPT
                return {[]() { ILLEGAL_OP(); }, 0};
            } else {
                // PEA
                auto [a, off] = ea_addr(type, reg, cpu.PC + 2, 0, false);
                return {[a]() { PUSH32(a()); }, off};
            }
        case 5:
            // TST.W
            {
                auto [r, off] = ea_read16(type, reg, cpu.PC + 2);
                return {[r]() { TEST_W(r()); }, off};
            }
        case 6: {
            uint16_t next = FETCH(cpu.PC + 2);
            auto [r, off] = ea_read32(type, reg, cpu.PC + 4);
            int dq = next >> 12 & 7;
            int dr = next & 7;
            bool sg = next & 1 << 11;
            bool lg = next & 1 << 10;
            if(sg) {
                if(lg) {
                    // DIVS.L <EA>, Dr:Dq
                    return {[dq, dr, r]() {
                                int64_t x = static_cast<uint64_t>(cpu.D[dr])
                                                << 32 |
                                            cpu.D[dq];
                                auto [quot, rem] = DIVS_LL(x, r());
                                cpu.D[dq] = quot;
                                cpu.D[dr] = rem;
                            },
                            off + 2};
                } else {
                    // DIVS.L <EA>, Dr:Dq
                    if(dq != dr) {
                        return {[dq, dr, r]() {
                                    std::tie(cpu.D[dq], cpu.D[dr]) =
                                        DIVS_L(cpu.D[dq], r());
                                },
                                off + 2};
                    } else {
                        return {[dq, r]() {
                                    std::tie(cpu.D[dq], std::ignore) =
                                        DIVS_L(cpu.D[dq], r());
                                },
                                off + 2};
                    }
                }
            } else {
                if(lg) {
                    // DIVS.L <EA>, Dr:Dq
                    return {[dq, dr, r]() {
                                int64_t x = static_cast<uint64_t>(cpu.D[dr])
                                                << 32 |
                                            cpu.D[dq];
                                auto [quot, rem] = DIVS_LL(x, r());
                                cpu.D[dq] = quot;
                                cpu.D[dr] = rem;
                            },
                            off + 2};
                } else {
                    // DIVU.L <EA>, Dr:Dq
                    if(dq != dr) {
                        return {[dq, dr, r]() {
                                    std::tie(cpu.D[dq], cpu.D[dr]) =
                                        DIVU_L(cpu.D[dq], r());
                                },
                                off + 2};
                    } else {
                        return {[dq, r]() {
                                    std::tie(cpu.D[dq], std::ignore) =
                                        DIVU_L(cpu.D[dq], r());
                                },
                                off + 2};
                    }
                }
            }
        }
        case 7:
            switch(type) {
            case 0:
            case 1:
                // TRAP
                return {[i = (type << 3 | reg)]() { TRAP(i); }, 0};
            case 2: {
                // LINK.W
                int16_t disp = FETCH(cpu.PC + 2);
                return {[disp, reg]() {
                            PUSH32(cpu.A[reg]);
                            cpu.A[reg] = cpu.A[7];
                            cpu.A[7] += disp;
                        },
                        2};
            }
            case 3: {
                // UNLK
                return {[reg]() {
                            cpu.A[7] = cpu.A[reg];
                            cpu.A[reg] = POP32();
                        },
                        0};
            }
            case 4:
                // MOVE TO USP
                return {[reg]() {
                            if(!cpu.S) {
                                PRIV_ERROR();
                            }
                            cpu.USP = cpu.A[reg];
                        },
                        0};
            case 5:
                // MOVE FROM USP
                return {[reg]() {
                            if(!cpu.S) {
                                PRIV_ERROR();
                            }
                            cpu.A[reg] = cpu.USP;
                        },
                        0};
            case 6:
                switch(reg) {
                case 0: // RESET
                    return {[]() {
                                if(!cpu.S) {
                                    PRIV_ERROR();
                                }
                                bus_reset();
                            },
                            0};
                case 1: // NOP
                    return {[]() {}, 0};
                case 2: // STOP
                {
                    uint16_t next = FETCH(cpu.PC + 2);
                    return {[next]() {
                                if(!cpu.S) {
                                    PRIV_ERROR();
                                }
                                SetSR(next);
                                cpu.sleep = true;
                            },
                            2};
                }
                case 3: // RTE
                    return {[]() {
                                if(!cpu.S) {
                                    PRIV_ERROR();
                                }
                                RTE();
                            },
                            0};
                case 4: // RTD
                {
                    int16_t disp = FETCH(cpu.PC + 2);
                    return {[disp]() {
                                JUMP(POP32());
                                cpu.A[7] += disp;
                            },
                            2};
                }
                case 5: // RTS
                    return {[]() { JUMP(POP32()); }, 0};
                case 6: // TRAPV
                    return {[]() {
                                if(cpu.V) {
                                    TRAPX();
                                }
                            },
                            0};
                case 7: // RTR
                    return {[]() {
                                SetCCR(POP16());
                                JUMP(POP32());
                            },
                            0};
                }
                break;
            case 7:
                if(reg == 2) {
                    uint16_t next = FETCH(cpu.PC + 2);
                    int reg = next >> 12 & 15;
                    int cr = next & 0x8ff;
                    return {from_cc(reg, cr), 2};
                } else if(reg == 3) {
                    uint16_t next = FETCH(cpu.PC + 2);
                    int reg = next >> 12 & 15;
                    int cr = next & 0x8ff;
                    return {to_cc(reg, cr), 2};
                } else {
                    throw DecodeError{};
                }
                break;
            }
        }
        break;

    case 2:
        switch(dn) {
        case 0: {
            // NEGX.L
            auto [r, w, off] = ea_rw32(type, reg, cpu.PC + 2);
            return {[r, w]() { w(SUB_L(0, r() + cpu.X)); }, off};
        }
        case 1: {
            // CLR.L
            auto [w, off] = ea_write32(type, reg, cpu.PC + 2);
            return {[w]() {
                        cpu.N = cpu.Z = cpu.V = cpu.C = false;
                        w(0);
                    },
                    off};
        }
        case 2: {
            // NEG.L
            auto [r, w, off] = ea_rw32(type, reg, cpu.PC + 2);
            return {[r, w]() { w(SUB_L(0, r())); }, off};
        }
        case 3: {
            // NOT.L
            auto [r, w, off] = ea_rw32(type, reg, cpu.PC + 2);
            return {[r, w]() {
                        uint32_t v = ~r();
                        TEST_L(v);
                        w(v);
                    },
                    off};
        }
        case 4:
            if(type == 0) {
                // EXT.W
                return {[reg]() {
                            int16_t v = static_cast<int8_t>(cpu.D[reg] & 0xff);
                            cpu.V = cpu.C = false;
                            TEST_W(v);
                            STORE_W(cpu.D[reg], v);
                        },
                        0};
            } else if(type == 3) {
                // MOVEM.W TO MEM(Decr)
                uint16_t regs = FETCH(cpu.PC + 2);
                return {[reg, regs]() {
                            cpu.af_value.CM = true;
                            MOVEM_W_TO_MEM_DECR(regs, reg);
                            // DONE
                            cpu.movem_cnt = 0;
                            cpu.af_value.CM = false;
                        },
                        2};
            } else {
                // MOVEM.W TO MEM(Addr)
                uint16_t regs = FETCH(cpu.PC + 2);
                auto [addr, of] = ea_addr(type, reg, cpu.PC + 4, 0, true);
                return {[addr, regs]() {
                            cpu.af_value.CM = true;
                            if(cpu.movem_cnt == 0) {
                                cpu.movem_addr = cpu.EA = addr();
                            }
                            MOVEM_W_TO_MEM_ADDR(regs);
                            // DONE
                            cpu.movem_cnt = 0;
                            cpu.af_value.CM = false;
                        },
                        of + 2};
            }
        case 5:
            // TST.L
            {
                auto [r, off] = ea_read32(type, reg, cpu.PC + 2);
                return {[r]() { TEST_L(r()); }, off};
            }
        case 6:
            if(type == 3) {
                // MOVEM.W FROM MEM(Incr)
                uint16_t regs = FETCH(cpu.PC + 2);
                return {[reg, regs]() {
                            cpu.af_value.CM = true;
                            MOVEM_W_FROM_MEM_INCR(regs, reg);
                            // DONE
                            cpu.movem_cnt = 0;
                            cpu.af_value.CM = false;
                        },
                        2};
            } else {
                // MOVEM.W FROM MEM(Addr)
                uint16_t regs = FETCH(cpu.PC + 2);
                auto [addr, of] = ea_addr(type, reg, cpu.PC + 4, 0, false);
                return {[addr, regs]() {
                            cpu.af_value.CM = true;
                            if(cpu.movem_cnt == 0) {
                                cpu.movem_addr = cpu.EA = addr();
                            }
                            MOVEM_W_FROM_MEM_ADDR(regs);
                            // DONE
                            cpu.movem_cnt = 0;
                            cpu.af_value.CM = false;
                        },
                        of + 2};
            }
        case 7: {
            // JSR
            auto [addr, off] = ea_addr(type, reg, cpu.PC + 2, 0, false);
            return {[addr, off]() {
                        PUSH32(cpu.PC + off);
                        JUMP(addr());
                    },
                    off};
        }
        default:
            throw DecodeError{};
        }
    case 3:
        switch(dn) {
        case 0: {
            // MOVE From SR
            auto [w, f] = ea_write16(type, reg, cpu.PC);
            return {[w]() {
                        if(!cpu.S) {
                            PRIV_ERROR();
                        }
                        w(GetSR());
                    },
                    f};
        }
        case 1: {
            // MOVE From CCR
            auto [w, f] = ea_write16(type, reg, cpu.PC);
            return {[w]() { w(GetCCR()); }, f};
        }
        case 2: {
            // MOVE To CCR
            auto [r, f] = ea_read16(type, reg, cpu.PC);
            return {[r]() { SetCCR(r()); }, f};
        }
        case 3: {
            // MOVE To SR
            auto [r, f] = ea_read16(type, reg, cpu.PC);
            return {[r]() {
                        if(!cpu.S) {
                            PRIV_ERROR();
                        }
                        SetSR(r());
                    },
                    f};
        }
        case 4:
            if(type == 0) {
                // EXT.L
                return {[reg]() {
                            int32_t v =
                                static_cast<int16_t>(cpu.D[reg] & 0xffff);
                            cpu.V = cpu.C = false;
                            TEST_L(v);
                            cpu.D[reg] = v;
                        },
                        0};
            } else if(type == 3) {
                // MOVEM.L TO MEM(Decr)
                uint16_t regs = FETCH(cpu.PC + 2);
                return {[reg, regs]() {
                            cpu.af_value.CM = true;
                            MOVEM_L_TO_MEM_DECR(regs, reg);
                            // DONE
                            cpu.movem_cnt = 0;
                            cpu.af_value.CM = false;
                        },
                        2};
            } else {
                // MOVEM.W TO MEM(Addr)
                uint16_t regs = FETCH(cpu.PC + 2);
                auto [addr, of] = ea_addr(type, reg, cpu.PC + 4, 0, true);
                return {[addr, regs]() {
                            cpu.af_value.CM = true;
                            if(cpu.movem_cnt == 0) {
                                cpu.movem_addr = cpu.EA = addr();
                            }
                            MOVEM_L_TO_MEM_ADDR(regs);
                            // DONE
                            cpu.movem_cnt = 0;
                            cpu.af_value.CM = false;
                        },
                        of + 2};
            }
        case 5:
            // TAS
            {
                auto [r, w, off] = ea_rw8(type, reg, cpu.PC + 2);
                return {[r, w]() {
                            uint8_t v = r();
                            TEST_L(v);
                            w(v | 0x80);
                        },
                        off};
            }
        case 6:
            if(type == 3) {
                // MOVEM.L FROM MEM(Incr)
                uint16_t regs = FETCH(cpu.PC + 2);
                return {[reg, regs]() {
                            cpu.af_value.CM = true;
                            MOVEM_L_FROM_MEM_INCR(regs, reg);
                            // DONE
                            cpu.movem_cnt = 0;
                            cpu.af_value.CM = false;
                        },
                        2};
            } else {
                // MOVEM.W FROM MEM(Addr)
                uint16_t regs = FETCH(cpu.PC + 2);
                auto [addr, of] = ea_addr(type, reg, cpu.PC + 4, 0, false);
                return {[addr, regs]() {
                            cpu.af_value.CM = true;
                            if(cpu.movem_cnt == 0) {
                                cpu.movem_addr = cpu.EA = addr();
                            }
                            MOVEM_L_FROM_MEM_ADDR(regs);
                            // DONE
                            cpu.movem_cnt = 0;
                            cpu.af_value.CM = false;
                        },
                        of + 2};
            }
        case 7: {
            // JMP
            auto [addr, off] = ea_addr(type, reg, cpu.PC + 2, 0, false);
            return {[addr, off]() { JUMP(addr()); }, off};
        }
        default:
            throw DecodeError{};
        }
    case 4:
        // CHK.L
        {
            auto [r, f] = ea_read32(type, reg, cpu.PC + 2);
            return {[dn, r]() {
                        int32_t v = r();
                        int32_t x = cpu.D[dn];
                        if(x >= 0 && x <= v) {
                            return;
                        }
                        cpu.N = x < 0;
                        CHK_ERROR();
                    },
                    f};
        }
    case 5:
        throw DecodeError{};
    case 6:
        // CHK.W
        {
            auto [r, f] = ea_read16(type, reg, cpu.PC + 2);
            return {[dn, r]() {
                        int16_t v = r();
                        int16_t x = cpu.D[dn];
                        if(x >= 0 && x <= v) {
                            return;
                        }
                        cpu.N = x < 0;
                        CHK_ERROR();
                    },
                    f};
        }
    case 7:
        if(type == 0) {
            if(dn == 4) {
                // EXTB.L
                return {[reg]() {
                            int32_t v = static_cast<int8_t>(cpu.D[reg] & 0xff);
                            cpu.V = cpu.C = false;
                            TEST_L(v);
                            cpu.D[reg] = v;
                        },
                        0};
            } else {
                throw DecodeError{};
            }
        } else {
            // LEA
            auto [addr, off] = ea_addr(type, reg, cpu.PC + 2, 0, false);
            return {[reg, addr]() { cpu.A[reg] = addr(); }, off};
        }
    default:
        throw DecodeError{};
    }
    throw DecodeError{};
}
std::function<bool()> testCC(int cc) {
    switch(cc) {
    case 0: // T
        return []() { return true; };
    case 1: // F
        return []() { return false; };
    case 2: // HI/LS
        return []() { return !cpu.C && !cpu.Z; };
    case 3: // LS
        return []() { return cpu.C || cpu.Z; };
    case 4: // CC
        return []() { return !cpu.C; };
    case 5: // CS
        return []() { return cpu.C; };
    case 6: // NE
        return []() { return !cpu.Z; };
    case 7: // EQ
        return []() { return cpu.Z; };
    case 8: // VC
        return []() { return !cpu.V; };
    case 9: // VS
        return []() { return cpu.V; };
    case 10: // PL
        return []() { return !cpu.N; };
    case 11: // MI
        return []() { return cpu.N; };
    case 12: // GE
        return []() { return cpu.N == cpu.V; };
    case 13: // LT
        return []() { return cpu.N != cpu.V; };
    case 14: // GT
        return []() { return !cpu.Z && (cpu.N == cpu.V); };
    case 15: // LE
        return []() { return cpu.Z || (cpu.N != cpu.V); };
    default:
        throw DecodeError{};
    }
}
std::pair<std::function<void()>, int> decode_op5(int dn, int sz, int type,
                                                 int reg) {
    switch(sz) {
    case 0: // ADDQ.B
        if(type == 1) {
            throw DecodeError{};
        } else {
            auto [r, w, off] = ea_rw8(type, reg, cpu.PC + 2);
            return {[r, w, dn = dn ? dn : 8]() { w(ADD_B(r(), dn)); }, off};
        }
    case 1: // ADDQ.W
        if(type == 1) {
            return {[reg, dn = dn ? dn : 8]() { cpu.A[reg] += dn; }, 0};
        } else {
            auto [r, w, off] = ea_rw16(type, reg, cpu.PC + 2);
            return {[r, w, dn = dn ? dn : 8]() { w(ADD_W(r(), dn)); }, off};
        }
    case 2: // ADDQ.L
        if(type == 1) {
            return {[reg, dn = dn ? dn : 8]() { cpu.A[reg] += dn; }, 0};
        } else {
            auto [r, w, off] = ea_rw32(type, reg, cpu.PC + 2);
            return {[r, w, dn = dn ? dn : 8]() { w(ADD_L(r(), dn)); }, off};
        }
    case 4: // SUBQ.B
        if(type == 1) {
            throw DecodeError{};
        } else {
            auto [r, w, off] = ea_rw8(type, reg, cpu.PC + 2);
            return {[r, w, dn = dn ? dn : 8]() { w(SUB_B(r(), dn)); }, off};
        }
    case 5: // SUBQ.W
        if(type == 1) {
            return {[reg, dn = dn ? dn : 8]() { cpu.A[reg] -= dn; }, 0};
        } else {
            auto [r, w, off] = ea_rw16(type, reg, cpu.PC + 2);
            return {[r, w, dn = dn ? dn : 8]() { w(SUB_W(r(), dn)); }, off};
        }
    case 6: // SUBQ.L
        if(type == 1) {
            return {[reg, dn = dn ? dn : 8]() { cpu.A[reg] -= dn; }, 0};
        } else {
            auto [r, w, off] = ea_rw32(type, reg, cpu.PC + 2);
            return {[r, w, dn = dn ? dn : 8]() { w(SUB_L(r(), dn)); }, off};
        }
    default: {
        int cc = dn << 1 | sz >> 2;
        auto cc_v = testCC(cc);
        if(type == 1) {
            // DBcc
            int16_t d = FETCH(cpu.PC + 2);
            return {[d, cc_v, reg]() {
                        if(!cc_v()) {
                            int16_t dn = cpu.D[reg];
                            STORE_W(cpu.D[reg], --dn);
                            if(dn == -1) {
                                JUMP(cpu.PC + 2 + d);
                            }
                        }
                    },
                    0};
        } else if(type == 7 && reg == 4) {
            // Trapcc
            return {[cc_v]() {
                        if(cc_v()) {
                            TRAPX();
                        }
                    },
                    0};
        } else if(type == 7 && reg == 2) {
            // Trapcc
            return {[cc_v]() {
                        if(cc_v()) {
                            TRAPX();
                        }
                    },
                    2};
        } else if(type == 7 && reg == 3) {
            // Trapcc
            return {[cc_v]() {
                        if(cc_v()) {
                            TRAPX();
                        }
                    },
                    4};
        } else {
            // Scc
            auto [w, off] = ea_write8(type, reg, cpu.PC + 2);
            return {[w, cc_v]() { w(cc_v() ? 0xff : 0); }, off};
        }
    }
    }
}

std::pair<std::function<void()>, int> decode_op6(int cc, int8_t imm8) {
    int32_t ix = imm8;
    int off = 0;
    if(imm8 == 0) {
        ix = FETCH(cpu.PC + 2);
        off = 2;
    } else if(imm8 == -1) {
        ix = FETCH32(cpu.PC + 2);
        off = 4;
    }
    if(cc == 0) {
        return {[ix]() { JUMP(cpu.PC + 2 + ix); }, off};
    } else if(cc == 1) {
        return {[ix]() {
                    PUSH32(cpu.PC + 2);
                    JUMP(cpu.PC + 2 + ix);
                },
                off};
    } else {
        auto cc_v = testCC(cc);
        return {[ix, cc_v]() {
                    if(cc_v()) {
                        JUMP(cpu.PC + 2 + ix);
                    }
                },
                off};
    }
}

std::pair<std::function<void()>, int> decode_op8(int dn, int sz, int type,
                                                 int reg) {
    switch(sz) {
    case 0: {
        // OR.B to REG
        auto [r, off] = ea_read8(type, reg, cpu.PC + 2);
        return {[r, dn]() { STORE_B(cpu.D[dn], OR_B(cpu.D[dn], r())); }, off};
    }
    case 1: {
        // OR.W
        auto [r, off] = ea_read16(type, reg, cpu.PC + 2);
        return {[r, dn]() { STORE_W(cpu.D[dn], OR_W(cpu.D[dn], r())); }, off};
    }
    case 2: {
        // OR.L
        auto [r, off] = ea_read32(type, reg, cpu.PC + 2);
        return {[r, dn]() { cpu.D[dn] = OR_L(cpu.D[dn], r()); }, off};
    }
    case 3: {
        // DIVU.W
        auto [r, off] = ea_read16(type, reg, cpu.PC + 2);
        return {[r, dn]() {
                    auto [div_q, div_r] = DIVU_W(cpu.D[dn], r());
                    cpu.D[dn] = div_r << 16 | div_q;
                },
                off};
    }
    case 4:
        if(type == 0) {
            // SBCD.D
            return {[dn, reg]() {
                        int d =
                            BCD2BIN(cpu.D[reg]) - BCD2BIN(cpu.D[dn]) - cpu.X;
                        if((cpu.C = d < 0)) {
                            d += 100;
                        }
                        if(d == 0) {
                            cpu.Z = false;
                        }
                        STORE_B(cpu.D[reg], BIN2BCD(d));
                    },
                    0};
        } else if(type == 1) {
            // SBCD.M
            return {[dn, reg]() {
                        int d = BCD2BIN(MMU_ReadB(--cpu.A[reg]));
                        d -= BCD2BIN(MMU_ReadB(--cpu.D[dn])) - cpu.X;
                        if((cpu.C = d < 0)) {
                            d += 100;
                        }
                        if(d == 0) {
                            cpu.Z = false;
                        }
                        MMU_WriteB(cpu.D[reg], BIN2BCD(d));
                    },
                    0};
        } else {
            // OR.B to MEM
            auto [r, w, off] = ea_rw8(type, reg, cpu.PC + 2);
            return {[r, w, dn]() { w(OR_B(cpu.D[dn], r())); }, off};
        }
    case 5:
        if(type == 0) {
            // PACK.D
            int16_t adj = FETCH(cpu.PC + 2);
            return {[dn, reg, adj]() {
                        int d = cpu.D[reg] + adj;
                        STORE_B(cpu.D[reg], (d & 0xf) | (d & 0xf00) >> 8);
                    },
                    2};
        } else if(type == 1) {
            // PACK.M
            int16_t adj = FETCH(cpu.PC + 2);
            return {[dn, reg, adj]() {
                        uint16_t d = MMU_ReadB(--cpu.A[reg]);
                        d |= MMU_ReadB(--cpu.A[reg]) << 8;
                        d += adj;
                        MMU_WriteB(--cpu.A[dn], (d & 0xf) | (d & 0xf00) >> 8);
                    },
                    2};
        } else {
            // OR.W to MEM
            auto [r, w, off] = ea_rw16(type, reg, cpu.PC + 2);
            return {[r, w, dn]() { w(OR_W(cpu.D[dn], r())); }, off};
        }
    case 6:
        if(type == 0) {
            // UNPK.D
            int16_t adj = FETCH(cpu.PC + 2);
            return {[dn, reg, adj]() {
                        uint16_t d = cpu.D[reg];
                        d = ((d & 0xf0) << 8 | (d & 0xf));
                        STORE_W(cpu.D[reg], static_cast<int16_t>(d) + adj);
                    },
                    2};
        } else if(type == 1) {
            // PACK.M
            int16_t adj = FETCH(cpu.PC + 2);
            return {[dn, reg, adj]() {
                        uint16_t d = MMU_ReadB(--cpu.A[reg]);
                        d = ((d & 0xf0) << 8 | (d & 0xf));
                        MMU_WriteW(cpu.A[dn] -= 2,
                                   static_cast<int16_t>(d) + adj);
                    },
                    2};
        } else {
            // OR.L to MEM
            auto [r, w, off] = ea_rw32(type, reg, cpu.PC + 2);
            return {[r, w, dn]() { w(OR_L(cpu.D[dn], r())); }, off};
        }
    case 7: {
        // DIVS.W
        auto [r, off] = ea_read16(type, reg, cpu.PC + 2);
        return {[r, dn]() {
                    auto [div_q, div_r] = DIVS_W(cpu.D[dn], r());
                    cpu.D[dn] = div_r << 16 | static_cast<uint16_t>(div_q);
                },
                off};
    }
    }
    throw DecodeError{};
}

std::pair<std::function<void()>, int> decode_op9(int dn, int sz, int type,
                                                 int reg) {
    switch(sz) {
    case 0: {
        // SUB.B
        auto [r, off] = ea_read8(type, reg, cpu.PC + 2);
        return {[r, dn]() { STORE_B(cpu.D[dn], SUB_B(cpu.D[dn], r())); }, off};
    }
    case 1: {
        // SUB.W
        auto [r, off] = ea_read16(type, reg, cpu.PC + 2);
        return {[r, dn]() { STORE_W(cpu.D[dn], SUB_W(cpu.D[dn], r())); }, off};
    }
    case 2: {
        // SUB.L
        auto [r, off] = ea_read32(type, reg, cpu.PC + 2);
        return {[r, dn]() { cpu.D[dn] = SUB_L(cpu.D[dn], r()); }, off};
    }
    case 3: {
        // SUBA.W
        auto [r, off] = ea_read16(type, reg, cpu.PC + 2);
        return {[r, dn]() { cpu.A[dn] -= r(); }, off};
    }
    case 4:
        if(type == 0) {
            // SUBX.B/D
            return {[dn, reg]() {
                        bool zz = cpu.Z;
                        uint8_t v = SUB_B(cpu.D[reg], cpu.D[dn] + cpu.X);
                        STORE_B(cpu.D[reg], v);
                        cpu.Z = v ? false : zz;
                    },
                    0};
        } else if(type == 1) {
            // SUBX.B/M
            return {[dn, reg]() {
                        uint32_t src_adr = --cpu.A[reg];
                        uint32_t dst_adr = --cpu.A[dn];
                        bool zz = cpu.Z;
                        uint8_t v = SUB_B(MMU_ReadB(dst_adr),
                                          MMU_ReadB(src_adr) + cpu.X);
                        MMU_WriteB(dst_adr, v);
                        cpu.Z = v ? false : zz;
                    },
                    0};
        } else {
            // SUB.B to MEM
            auto [r, w, off] = ea_rw8(type, reg, cpu.PC + 2);
            return {[r, w, dn]() { w(SUB_B(r(), cpu.D[dn])); }, off};
        }
    case 5:
        if(type == 0) {
            // SUBX.W/D
            return {[dn, reg]() {
                        bool zz = cpu.Z;
                        uint8_t v = SUB_W(cpu.D[reg], cpu.D[dn] + cpu.X);
                        STORE_W(cpu.D[reg], v);
                        cpu.Z = v ? false : zz;
                    },
                    0};
        } else if(type == 1) {
            // SUBX.W/M
            return {[dn, reg]() {
                        uint32_t src_adr = (cpu.A[reg] -= 2);
                        uint32_t dst_adr = (cpu.A[dn] -= 2);
                        bool zz = cpu.Z;
                        uint8_t v = SUB_W(MMU_ReadW(dst_adr),
                                          MMU_ReadW(src_adr) + cpu.X);
                        MMU_WriteW(dst_adr, v);
                        cpu.Z = v ? false : zz;
                    },
                    0};
        } else {
            // SUB.W to MEM
            auto [r, w, off] = ea_rw16(type, reg, cpu.PC + 2);
            return {[r, w, dn]() { w(SUB_W(r(), cpu.D[dn])); }, off};
        }
    case 6:
        if(type == 0) {
            // SUBX.L/D
            return {[dn, reg]() {
                        bool zz = cpu.Z;
                        uint8_t v = SUB_L(cpu.D[reg], cpu.D[dn] + cpu.X);
                        cpu.D[reg] = v;
                        cpu.Z = v ? false : zz;
                    },
                    0};
        } else if(type == 1) {
            // SUBX.L/M
            return {[dn, reg]() {
                        uint32_t src_adr = (cpu.A[reg] -= 4);
                        uint32_t dst_adr = (cpu.A[dn] -= 4);
                        bool zz = cpu.Z;
                        uint8_t v = SUB_L(MMU_ReadL(dst_adr),
                                          MMU_ReadL(src_adr) + cpu.X);
                        MMU_WriteL(dst_adr, v);
                        cpu.Z = v ? false : zz;
                    },
                    0};
        } else {
            // SUB.L to MEM
            auto [r, w, off] = ea_rw32(type, reg, cpu.PC + 2);
            return {[r, w, dn]() { w(SUB_L(r(), cpu.D[dn])); }, off};
        }
    case 7: {
        // SUBA.L
        auto [r, off] = ea_read32(type, reg, cpu.PC + 2);
        return {[r, dn]() { cpu.A[dn] -= r(); }, off};
    }
    }
    throw DecodeError{};
}

std::pair<std::function<void()>, int> decode_op11(int dn, int sz, int type,
                                                  int reg) {
    switch(sz) {
    case 0: {
        // CMP.B
        auto [r, off] = ea_read8(type, reg, cpu.PC + 2);
        return {[r, dn]() { CMP_B(cpu.D[dn], r()); }, off};
    }
    case 1: {
        // CMP.W
        auto [r, off] = ea_read16(type, reg, cpu.PC + 2);
        return {[r, dn]() { CMP_W(cpu.D[dn], r()); }, off};
    }
    case 2: {
        // SUB.L
        auto [r, off] = ea_read32(type, reg, cpu.PC + 2);
        return {[r, dn]() { CMP_L(cpu.D[dn], r()); }, off};
    }
    case 3: {
        // CMPA.W
        auto [r, off] = ea_read16(type, reg, cpu.PC + 2);
        return {[r, dn]() { CMP_L(cpu.A[dn], static_cast<int16_t>(r())); },
                off};
    }
    case 4:
        if(type == 1) {
            // CMPM.B
            return {[dn, reg]() {
                        uint32_t src_adr =
                            std::exchange(cpu.A[reg], cpu.A[reg] + 1);
                        uint32_t dst_adr =
                            std::exchange(cpu.A[dn], cpu.A[dn] + 1);
                        CMP_B(MMU_ReadB(dst_adr), MMU_ReadB(src_adr));
                    },
                    0};
        } else {
            // EOR.B
            auto [r, w, off] = ea_rw8(type, reg, cpu.PC + 2);
            return {[r, w, dn]() { w(XOR_B(r(), cpu.D[dn])); }, off};
        }
    case 5:
        if(type == 1) {
            // CMPM.W
            return {[dn, reg]() {
                        uint32_t src_adr =
                            std::exchange(cpu.A[reg], cpu.A[reg] + 2);
                        uint32_t dst_adr =
                            std::exchange(cpu.A[dn], cpu.A[dn] + 2);
                        CMP_W(MMU_ReadW(dst_adr), MMU_ReadW(src_adr));
                    },
                    0};
        } else {
            // EOR.W
            auto [r, w, off] = ea_rw16(type, reg, cpu.PC + 2);
            return {[r, w, dn]() { w(XOR_W(r(), cpu.D[dn])); }, off};
        }
    case 6:
        if(type == 1) {
            // CMPM.L
            return {[dn, reg]() {
                        uint32_t src_adr =
                            std::exchange(cpu.A[reg], cpu.A[reg] + 4);
                        uint32_t dst_adr =
                            std::exchange(cpu.A[dn], cpu.A[dn] + 4);
                        CMP_L(MMU_ReadL(dst_adr), MMU_ReadL(src_adr));
                    },
                    0};
        } else {
            // EOR.L
            auto [r, w, off] = ea_rw32(type, reg, cpu.PC + 2);
            return {[r, w, dn]() { w(XOR_L(r(), cpu.D[dn])); }, off};
        }
    case 7: {
        // CMPA.L
        auto [r, off] = ea_read32(type, reg, cpu.PC + 2);
        return {[r, dn]() { CMP_L(cpu.A[dn], r()); }, off};
    }
    }
    throw DecodeError{};
}

std::pair<std::function<void()>, int> decode_op12(int dn, int sz, int type,
                                                  int reg) {
    switch(sz) {
    case 0: {
        // AND.B to REG
        auto [r, off] = ea_read8(type, reg, cpu.PC + 2);
        return {[r, dn]() { STORE_B(cpu.D[dn], AND_B(cpu.D[dn], r())); }, off};
    }
    case 1: {
        // AND.W to REG
        auto [r, off] = ea_read16(type, reg, cpu.PC + 2);
        return {[r, dn]() { STORE_W(cpu.D[dn], AND_W(cpu.D[dn], r())); }, off};
    }
    case 2: {
        // AND.L to REG
        auto [r, off] = ea_read32(type, reg, cpu.PC + 2);
        return {[r, dn]() { cpu.D[dn] = AND_L(cpu.D[dn], r()); }, off};
    }
    case 3: {
        // MULU.W
        auto [r, off] = ea_read16(type, reg, cpu.PC + 2);
        return {[r, dn]() { cpu.D[dn] = MULU_W(cpu.D[dn], r()); }, off};
    }
    case 4:
        if(type == 0) {
            // ABCD.D
            return {[dn, reg]() {
                        int d = BCD2BIN(cpu.D[reg]);
                        d += BCD2BIN(cpu.D[dn]) + cpu.X;
                        if((cpu.X = cpu.C = d >= 100)) {
                            d -= 100;
                        }
                        if(d != 0) {
                            cpu.Z = false;
                        }
                        STORE_B(cpu.D[dn], BIN2BCD(d));
                    },
                    0};
        } else if(type == 1) {
            // ABCD.M
            return {[dn, reg]() {
                        int d = BCD2BIN(MMU_ReadB(--cpu.A[reg]));
                        d += BCD2BIN(MMU_ReadB(--cpu.A[dn])) + cpu.X;
                        if((cpu.X = cpu.C = d >= 100)) {
                            d -= 100;
                        }
                        if(d != 0) {
                            cpu.Z = false;
                        }
                        MMU_WriteB(cpu.A[dn], BIN2BCD(d));
                    },
                    0};
        } else {
            // AND.B to MEM
            auto [r, w, off] = ea_rw8(type, reg, cpu.PC + 2);
            return {[r, w, dn]() { w(AND_B(cpu.D[dn], r())); }, off};
        }
    case 5:
        if(type == 0) {
            // EXG Dn
            return {[dn, reg]() { std::swap(cpu.D[reg], cpu.D[dn]); }, 0};
        } else if(type == 1) {
            // EXG An
            return {[dn, reg]() { std::swap(cpu.A[reg], cpu.A[dn]); }, 0};
        } else {
            // AND.W to MEM
            auto [r, w, off] = ea_rw16(type, reg, cpu.PC + 2);
            return {[r, w, dn]() { w(AND_W(cpu.D[dn], r())); }, off};
        }
    case 6:
        if(type == 0) {
            throw DecodeError{};
        } else if(type == 1) {
            // EXG Dn, Am
            return {[dn, reg]() { std::swap(cpu.A[reg], cpu.D[dn]); }, 0};
        } else {
            // AND.L to MEM
            auto [r, w, off] = ea_rw32(type, reg, cpu.PC + 2);
            return {[r, w, dn]() { w(AND_L(cpu.D[dn], r())); }, off};
        }
    case 7: {
        // MULS.W
        auto [r, off] = ea_read16(type, reg, cpu.PC + 2);
        return {[r, dn]() { cpu.D[dn] = MULS_W(cpu.D[dn], r()); }, off};
    }
    }
    throw DecodeError{};
}

std::pair<std::function<void()>, int> decode_op13(int dn, int sz, int type,
                                                  int reg) {
    switch(sz) {
    case 0: {
        // ADD.B
        auto [r, off] = ea_read8(type, reg, cpu.PC + 2);
        return {[r, dn]() { STORE_B(cpu.D[dn], ADD_B(cpu.D[dn], r())); }, off};
    }
    case 1: {
        // ADD.W
        auto [r, off] = ea_read16(type, reg, cpu.PC + 2);
        return {[r, dn]() { STORE_W(cpu.D[dn], ADD_W(cpu.D[dn], r())); }, off};
    }
    case 2: {
        // ADD.L
        auto [r, off] = ea_read32(type, reg, cpu.PC + 2);
        return {[r, dn]() { cpu.D[dn] = ADD_L(cpu.D[dn], r()); }, off};
    }
    case 3: {
        // ADDA.W
        auto [r, off] = ea_read16(type, reg, cpu.PC + 2);
        return {[r, dn]() { cpu.A[dn] += static_cast<int16_t>(r()); }, off};
    }
    case 4:
        if(type == 0) {
            // ADDX.B/D
            return {[dn, reg]() {
                        bool zz = cpu.Z;
                        uint8_t v = ADD_B(cpu.D[reg], cpu.D[dn] + cpu.X);
                        STORE_B(cpu.D[dn], v);
                        cpu.Z = v ? false : zz;
                    },
                    0};
        } else if(type == 1) {
            // ADDX.B/M
            return {[dn, reg]() {
                        uint32_t src_adr = --cpu.A[reg];
                        uint32_t dst_adr = --cpu.A[dn];
                        bool zz = cpu.Z;
                        uint8_t v = ADD_B(MMU_ReadB(dst_adr),
                                          MMU_ReadB(src_adr) + cpu.X);
                        MMU_WriteB(dst_adr, v);
                        cpu.Z = v ? false : zz;
                    },
                    0};
        } else {
            // ADD.B to MEM
            auto [r, w, off] = ea_rw8(type, reg, cpu.PC + 2);
            return {[r, w, dn]() { w(ADD_B(r(), cpu.D[dn])); }, off};
        }
    case 5:
        if(type == 0) {
            // ADDX.W/D
            return {[dn, reg]() {
                        bool zz = cpu.Z;
                        uint16_t v = ADD_W(cpu.D[reg], cpu.D[dn] + cpu.X);
                        STORE_W(cpu.D[dn], v);
                        cpu.Z = v ? false : zz;
                    },
                    0};
        } else if(type == 1) {
            // ADDX.W/M
            return {[dn, reg]() {
                        uint32_t src_adr = (cpu.A[reg] -= 2);
                        uint32_t dst_adr = (cpu.A[dn] -= 2);
                        bool zz = cpu.Z;
                        uint16_t v = ADD_W(MMU_ReadW(dst_adr),
                                          MMU_ReadW(src_adr) + cpu.X);
                        MMU_WriteW(dst_adr, v);
                        cpu.Z = v ? false : zz;
                    },
                    0};
        } else {
            // ADD.W to MEM
            auto [r, w, off] = ea_rw16(type, reg, cpu.PC + 2);
            return {[r, w, dn]() { w(ADD_W(r(), cpu.D[dn])); }, off};
        }
    case 6:
        if(type == 0) {
            // ADDX.L/D
            return {[dn, reg]() {
                        bool zz = cpu.Z;
                        uint32_t v = ADD_L(cpu.D[reg], cpu.D[dn] + cpu.X);
                        cpu.D[dn] = v;
                        cpu.Z = v ? false : zz;
                    },
                    0};
        } else if(type == 1) {
            // ADDX.L/M
            return {[dn, reg]() {
                        uint32_t src_adr = (cpu.A[reg] -= 4);
                        uint32_t dst_adr = (cpu.A[dn] -= 4);
                        bool zz = cpu.Z;
                        uint32_t v = ADD_L(MMU_ReadL(dst_adr),
                                          MMU_ReadL(src_adr) + cpu.X);
                        MMU_WriteL(dst_adr, v);
                        cpu.Z = v ? false : zz;
                    },
                    0};
        } else {
            // ADD.L to MEM
            auto [r, w, off] = ea_rw32(type, reg, cpu.PC + 2);
            return {[r, w, dn]() { w(ADD_L(r(), cpu.D[dn])); }, off};
        }
    case 7: {
        // ADDA.L
        auto [r, off] = ea_read32(type, reg, cpu.PC + 2);
        return {[r, dn]() { cpu.A[dn] += r(); }, off};
    }
    }
    throw DecodeError{};
}
std::function<void()> decode_op14_0(int dn, int type, int reg) {
    switch(type) {
    case 0:
        return [dn = dn ? dn : 8, reg] {
            STORE_B(cpu.D[reg], ASR_B(cpu.D[reg], dn));
        };
    case 1:
        return [dn = dn ? dn : 8, reg] {
            STORE_B(cpu.D[reg], LSR_B(cpu.D[reg], dn));
        };
    case 2:
        return [dn = dn ? dn : 8, reg] {
            STORE_B(cpu.D[reg], ROXR_B(cpu.D[reg], dn));
        };
    case 3:
        return [dn = dn ? dn : 8, reg] {
            STORE_B(cpu.D[reg], ROR_B(cpu.D[reg], dn));
        };
    case 4:
        return [dn, reg] {
            STORE_B(cpu.D[reg], ASR_B(cpu.D[reg], cpu.D[dn] & 63));
        };
    case 5:
        return [dn, reg] {
            STORE_B(cpu.D[reg], LSR_B(cpu.D[reg], cpu.D[dn] & 63));
        };
    case 6:
        return [dn, reg] {
            STORE_B(cpu.D[reg], ROXR_B(cpu.D[reg], cpu.D[dn] & 63));
        };
    case 7:
        return [dn, reg] {
            STORE_B(cpu.D[reg], ROR_B(cpu.D[reg], cpu.D[dn] & 63));
        };
    }
    throw DecodeError{};
}

std::function<void()> decode_op14_1(int dn, int type, int reg) {
    switch(type) {
    case 0:
        return [dn = dn ? dn : 8, reg] {
            STORE_W(cpu.D[reg], ASR_W(cpu.D[reg], dn));
        };
    case 1:
        return [dn = dn ? dn : 8, reg] {
            STORE_W(cpu.D[reg], LSR_W(cpu.D[reg], dn));
        };
    case 2:
        return [dn = dn ? dn : 8, reg] {
            STORE_W(cpu.D[reg], ROXR_W(cpu.D[reg], dn));
        };
    case 3:
        return [dn = dn ? dn : 8, reg] {
            STORE_W(cpu.D[reg], ROR_W(cpu.D[reg], dn));
        };
    case 4:
        return [dn, reg] {
            STORE_W(cpu.D[reg], ASR_W(cpu.D[reg], cpu.D[dn] & 63));
        };
    case 5:
        return [dn, reg] {
            STORE_W(cpu.D[reg], LSR_W(cpu.D[reg], cpu.D[dn] & 63));
        };
    case 6:
        return [dn, reg] {
            STORE_W(cpu.D[reg], ROXR_W(cpu.D[reg], cpu.D[dn] & 63));
        };
    case 7:
        return [dn, reg] {
            STORE_W(cpu.D[reg], ROR_W(cpu.D[reg], cpu.D[dn] & 63));
        };
    }
    throw DecodeError{};
}
std::function<void()> decode_op14_2(int dn, int type, int reg) {
    switch(type) {
    case 0:
        return [dn = dn ? dn : 8, reg] { cpu.D[reg] = ASR_L(cpu.D[reg], dn); };
    case 1:
        return [dn = dn ? dn : 8, reg] { cpu.D[reg] = LSR_L(cpu.D[reg], dn); };
    case 2:
        return [dn = dn ? dn : 8, reg] { cpu.D[reg] = ROXR_L(cpu.D[reg], dn); };
    case 3:
        return [dn = dn ? dn : 8, reg] { cpu.D[reg] = ROR_L(cpu.D[reg], dn); };
    case 4:
        return [dn, reg] { cpu.D[reg] = ASR_L(cpu.D[reg], cpu.D[dn] & 63); };
    case 5:
        return [dn, reg] { cpu.D[reg] = LSR_L(cpu.D[reg], cpu.D[dn] & 63); };
    case 6:
        return [dn, reg] { cpu.D[reg] = ROXR_L(cpu.D[reg], cpu.D[dn] & 63); };
    case 7:
        return [dn, reg] { cpu.D[reg] = ROR_L(cpu.D[reg], cpu.D[dn] & 63); };
    }
    throw DecodeError{};
}
std::pair<std::function<void()>, int> decode_op14_3(int dn, int type, int reg) {
    switch(dn) {
    case 0: {
        auto [r, w, off] = ea_rw16(type, reg, cpu.PC + 2);
        return {[r, w]() { w(ASR_W(r(), 1)); }, off};
    }
    case 1: {
        auto [r, w, off] = ea_rw16(type, reg, cpu.PC + 2);
        return {[r, w]() { w(LSR_W(r(), 1)); }, off};
    }
    case 2: {
        auto [r, w, off] = ea_rw16(type, reg, cpu.PC + 2);
        return {[r, w]() { w(ROXR_W(r(), 1)); }, off};
    }
    case 3: {
        auto [r, w, off] = ea_rw16(type, reg, cpu.PC + 2);
        return {[r, w]() { w(ROR_W(r(), 1)); }, off};
    }
    }
    uint16_t next = FETCH(cpu.PC + 2);
    bool offset_r = next & 1 << 11;
    int offset = next >> 6 & 0x1f;
    bool width_r = next & 1 << 5;
    int width = next & 0x1f;
    if(type == 0) {
        if(offset_r) {
            offset &= 7;
            if(width_r) {
                width &= 7;
                switch(dn) {
                case 4:
                    return {[offset, width, reg]() {
                                BFTST_D(cpu.D[reg], cpu.D[offset],
                                        cpu.D[width]);
                            },
                            2};
                case 5:
                    return {[offset, width, reg]() {
                                BFCHG_D(cpu.D[reg], cpu.D[offset],
                                        cpu.D[width]);
                            },
                            2};
                case 6:
                    return {[offset, width, reg]() {
                                BFCLR_D(cpu.D[reg], cpu.D[offset],
                                        cpu.D[width]);
                            },
                            2};
                case 7:
                    return {[offset, width, reg]() {
                                BFSET_D(cpu.D[reg], cpu.D[offset],
                                        cpu.D[width]);
                            },
                            2};
                }
            } else {
                width = width ? width : 32;
                switch(dn) {
                case 4:
                    return {[offset, width, reg]() {
                                BFTST_D(cpu.D[reg], cpu.D[offset], width);
                            },
                            2};
                case 5:
                    return {[offset, width, reg]() {
                                BFCHG_D(cpu.D[reg], cpu.D[offset], width);
                            },
                            2};
                case 6:
                    return {[offset, width, reg]() {
                                BFCLR_D(cpu.D[reg], cpu.D[offset], width);
                            },
                            2};
                case 7:
                    return {[offset, width, reg]() {
                                BFSET_D(cpu.D[reg], cpu.D[offset], width);
                            },
                            2};
                }
            }
        } else {
            if(width_r) {
                width &= 7;
                switch(dn) {
                case 4:
                    return {[offset, width, reg]() {
                                BFTST_D(cpu.D[reg], offset, cpu.D[width]);
                            },
                            2};
                case 5:
                    return {[offset, width, reg]() {
                                BFCHG_D(cpu.D[reg], offset, cpu.D[width]);
                            },
                            2};
                case 6:
                    return {[offset, width, reg]() {
                                BFCLR_D(cpu.D[reg], offset, cpu.D[width]);
                            },
                            2};
                case 7:
                    return {[offset, width, reg]() {
                                BFSET_D(cpu.D[reg], offset, cpu.D[width]);
                            },
                            2};
                }
            } else {
                width = width ? width : 32;
                switch(dn) {
                case 4:
                    return {[offset, width, reg]() {
                                BFTST_D(cpu.D[reg], offset, width);
                            },
                            2};
                case 5:
                    return {[offset, width, reg]() {
                                BFCHG_D(cpu.D[reg], offset, width);
                            },
                            2};
                case 6:
                    return {[offset, width, reg]() {
                                BFCLR_D(cpu.D[reg], offset, width);
                            },
                            2};
                case 7:
                    return {[offset, width, reg]() {
                                BFSET_D(cpu.D[reg], offset, width);
                            },
                            2};
                }
            }
        }
    } else {
        auto [a, f] = ea_addr(type, reg, cpu.PC + 2, 0, dn != 4);
        if(offset_r) {
            offset &= 7;
            if(width_r) {
                width &= 7;
                switch(dn) {
                case 4:
                    return {[offset, width, a]() {
                                BFTST_M(a(), cpu.D[offset], cpu.D[width]);
                            },
                            f + 2};
                case 5:
                    return {[offset, width, a]() {
                                BFCHG_M(a(), cpu.D[offset], cpu.D[width]);
                            },
                            f + 2};
                case 6:
                    return {[offset, width, a]() {
                                BFCLR_M(a(), cpu.D[offset], cpu.D[width]);
                            },
                            f + 2};
                case 7:
                    return {[offset, width, a]() {
                                BFSET_M(a(), cpu.D[offset], cpu.D[width]);
                            },
                            f + 2};
                }
            } else {
                width = width ? width : 32;
                switch(dn) {
                case 4:
                    return {[offset, width, a]() {
                                BFTST_M(a(), cpu.D[offset], width);
                            },
                            f + 2};
                case 5:
                    return {[offset, width, a]() {
                                BFCHG_M(a(), cpu.D[offset], width);
                            },
                            f + 2};
                case 6:
                    return {[offset, width, a]() {
                                BFCLR_M(a(), cpu.D[offset], width);
                            },
                            f + 2};
                case 7:
                    return {[offset, width, a]() {
                                BFSET_M(a(), cpu.D[offset], width);
                            },
                            f + 2};
                }
            }
        } else {
            if(width_r) {
                width &= 7;
                switch(dn) {
                case 4:
                    return {[offset, width, a]() {
                                BFTST_M(a(), offset, cpu.D[width]);
                            },
                            f + 2};
                case 5:
                    return {[offset, width, a]() {
                                BFCHG_M(a(), offset, cpu.D[width]);
                            },
                            f + 2};
                case 6:
                    return {[offset, width, a]() {
                                BFCLR_M(a(), offset, cpu.D[width]);
                            },
                            f + 2};
                case 7:
                    return {[offset, width, a]() {
                                BFSET_M(a(), offset, cpu.D[width]);
                            },
                            f + 2};
                }
            } else {
                width = width ? width : 32;
                switch(dn) {
                case 4:
                    return {
                        [offset, width, a]() { BFTST_M(a(), offset, width); },
                        f + 2};
                case 5:
                    return {
                        [offset, width, a]() { BFCHG_M(a(), offset, width); },
                        f + 2};
                case 6:
                    return {
                        [offset, width, a]() { BFCLR_M(a(), offset, width); },
                        f + 2};
                case 7:
                    return {
                        [offset, width, a]() { BFSET_M(a(), offset, width); },
                        f + 2};
                }
            }
        }
    }
    throw DecodeError{};
}

std::function<void()> decode_op14_4(int dn, int type, int reg) {
    switch(type) {
    case 0:
        return [dn = dn ? dn : 8, reg] {
            STORE_B(cpu.D[reg], ASL_B(cpu.D[reg], dn));
        };
    case 1:
        return [dn = dn ? dn : 8, reg] {
            STORE_B(cpu.D[reg], LSL_B(cpu.D[reg], dn));
        };
    case 2:
        return [dn = dn ? dn : 8, reg] {
            STORE_B(cpu.D[reg], ROXL_B(cpu.D[reg], dn));
        };
    case 3:
        return [dn = dn ? dn : 8, reg] {
            STORE_B(cpu.D[reg], ROL_B(cpu.D[reg], dn));
        };
    case 4:
        return [dn, reg] {
            STORE_B(cpu.D[reg], ASL_B(cpu.D[reg], cpu.D[dn] & 63));
        };
    case 5:
        return [dn, reg] {
            STORE_B(cpu.D[reg], LSL_B(cpu.D[reg], cpu.D[dn] & 63));
        };
    case 6:
        return [dn, reg] {
            STORE_B(cpu.D[reg], ROXL_B(cpu.D[reg], cpu.D[dn] & 63));
        };
    case 7:
        return [dn, reg] {
            STORE_B(cpu.D[reg], ROL_B(cpu.D[reg], cpu.D[dn] & 63));
        };
    }
    throw DecodeError{};
}

std::function<void()> decode_op14_5(int dn, int type, int reg) {
    switch(type) {
    case 0:
        return [dn = dn ? dn : 8, reg] {
            STORE_W(cpu.D[reg], ASL_W(cpu.D[reg], dn));
        };
    case 1:
        return [dn = dn ? dn : 8, reg] {
            STORE_W(cpu.D[reg], LSL_W(cpu.D[reg], dn));
        };
    case 2:
        return [dn = dn ? dn : 8, reg] {
            STORE_W(cpu.D[reg], ROXL_W(cpu.D[reg], dn));
        };
    case 3:
        return [dn = dn ? dn : 8, reg] {
            STORE_W(cpu.D[reg], ROL_W(cpu.D[reg], dn));
        };
    case 4:
        return [dn, reg] {
            STORE_W(cpu.D[reg], ASL_W(cpu.D[reg], cpu.D[dn] & 63));
        };
    case 5:
        return [dn, reg] {
            STORE_W(cpu.D[reg], LSL_W(cpu.D[reg], cpu.D[dn] & 63));
        };
    case 6:
        return [dn, reg] {
            STORE_W(cpu.D[reg], ROXL_W(cpu.D[reg], cpu.D[dn] & 63));
        };
    case 7:
        return [dn, reg] {
            STORE_W(cpu.D[reg], ROL_W(cpu.D[reg], cpu.D[dn] & 63));
        };
    }
    throw DecodeError{};
}
std::function<void()> decode_op14_6(int dn, int type, int reg) {
    switch(type) {
    case 0:
        return [dn = dn ? dn : 8, reg] { cpu.D[reg] = ASL_L(cpu.D[reg], dn); };
    case 1:
        return [dn = dn ? dn : 8, reg] { cpu.D[reg] = LSL_L(cpu.D[reg], dn); };
    case 2:
        return [dn = dn ? dn : 8, reg] { cpu.D[reg] = ROXL_L(cpu.D[reg], dn); };
    case 3:
        return [dn = dn ? dn : 8, reg] { cpu.D[reg] = ROL_L(cpu.D[reg], dn); };
    case 4:
        return [dn, reg] { cpu.D[reg] = ASL_L(cpu.D[reg], cpu.D[dn] & 63); };
    case 5:
        return [dn, reg] { cpu.D[reg] = LSL_L(cpu.D[reg], cpu.D[dn] & 63); };
    case 6:
        return [dn, reg] { cpu.D[reg] = ROXL_L(cpu.D[reg], cpu.D[dn] & 63); };
    case 7:
        return [dn, reg] { cpu.D[reg] = ROL_L(cpu.D[reg], cpu.D[dn] & 63); };
    }
    throw DecodeError{};
}

std::pair<std::function<void()>, int> decode_op14_7(int dn, int type, int reg) {
    switch(dn) {
    case 0: {
        auto [r, w, off] = ea_rw16(type, reg, cpu.PC + 2);
        return {[r, w]() { w(ASL_W(r(), 1)); }, off};
    }
    case 1: {
        auto [r, w, off] = ea_rw16(type, reg, cpu.PC + 2);
        return {[r, w]() { w(LSL_W(r(), 1)); }, off};
    }
    case 2: {
        auto [r, w, off] = ea_rw16(type, reg, cpu.PC + 2);
        return {[r, w]() { w(ROXL_W(r(), 1)); }, off};
    }
    case 3: {
        auto [r, w, off] = ea_rw16(type, reg, cpu.PC + 2);
        return {[r, w]() { w(ROL_W(r(), 1)); }, off};
    }
    }
    uint16_t next = FETCH(cpu.PC + 2);
    bool offset_r = next & 1 << 11;
    int offset = next >> 6 & 0x1f;
    bool width_r = next & 1 << 5;
    int width = next & 0x1f;
    int reg2 = next >> 12 & 7;
    if(type == 0) {
        if(offset_r) {
            offset &= 7;
            if(width_r) {
                width &= 7;
                switch(dn) {
                case 4:
                    return {[offset, width, reg, reg2]() {
                                cpu.D[reg2] = BFEXTU_D(
                                    cpu.D[reg], cpu.D[offset], cpu.D[width]);
                            },
                            2};
                case 5:
                    return {[offset, width, reg, reg2]() {
                                cpu.D[reg2] = BFEXTS_D(
                                    cpu.D[reg], cpu.D[offset], cpu.D[width]);
                            },
                            2};
                case 6:
                    return {[offset, width, reg, reg2]() {
                                cpu.D[reg2] = BFFFO_D(cpu.D[reg], cpu.D[offset],
                                                      cpu.D[width]);
                            },
                            2};
                case 7:
                    return {[offset, width, reg, reg2]() {
                                cpu.D[reg] = BFINS_D(cpu.D[reg], cpu.D[offset], cpu.D[width],
                                        cpu.D[reg2]);
                            },
                            2};
                }
            } else {
                width = width ? width : 32;
                switch(dn) {
                case 4:
                    return {[offset, width, reg, reg2]() {
                                cpu.D[reg2] =
                                    BFEXTU_D(cpu.D[reg], cpu.D[offset], width);
                            },
                            2};
                case 5:
                    return {[offset, width, reg, reg2]() {
                                cpu.D[reg2] =
                                    BFEXTS_D(cpu.D[reg], cpu.D[offset], width);
                            },
                            2};
                case 6:
                    return {[offset, width, reg, reg2]() {
                                cpu.D[reg2] =
                                    BFFFO_D(cpu.D[reg], cpu.D[offset], width);
                            },
                            2};
                case 7:
                    return {[offset, width, reg, reg2]() {
                                cpu.D[reg] = BFINS_D(cpu.D[reg], cpu.D[offset], width,
                                        cpu.D[reg2]);
                            },
                            2};
                }
            }
        } else {
            if(width_r) {
                width &= 7;
                switch(dn) {
                case 4:
                    return {[offset, width, reg, reg2]() {
                                cpu.D[reg2] =
                                    BFEXTU_D(cpu.D[reg], offset, cpu.D[width]);
                            },
                            2};
                case 5:
                    return {[offset, width, reg, reg2]() {
                                cpu.D[reg2] =
                                    BFEXTS_D(cpu.D[reg], offset, cpu.D[width]);
                            },
                            2};
                case 6:
                    return {[offset, width, reg, reg2]() {
                                cpu.D[reg2] =
                                    BFFFO_D(cpu.D[reg], offset, cpu.D[width]);
                            },
                            2};
                case 7:
                    return {[offset, width, reg, reg2]() {
                                cpu.D[reg] = BFINS_D(cpu.D[reg], offset, cpu.D[width],
                                        cpu.D[reg2]);
                            },
                            2};
                }
            } else {
                width = width ? width : 32;
                switch(dn) {
                case 4:
                    return {[offset, width, reg, reg2]() {
                                cpu.D[reg2] =
                                    BFEXTU_D(cpu.D[reg], offset, width);
                            },
                            2};
                case 5:
                    return {[offset, width, reg, reg2]() {
                                cpu.D[reg2] =
                                    BFEXTS_D(cpu.D[reg], offset, width);
                            },
                            2};
                case 6:
                    return {[offset, width, reg, reg2]() {
                                cpu.D[reg2] =
                                    BFFFO_D(cpu.D[reg], offset, width);
                            },
                            2};
                case 7:
                    return {[offset, width, reg, reg2]() {
                                cpu.D[reg] = BFINS_D(cpu.D[reg], offset, width, cpu.D[reg2]);
                            },
                            2};
                }
            }
        }
    } else {
        auto [a, f] = ea_addr(type, reg, cpu.PC + 2, 0, dn == 7);
        if(offset_r) {
            offset &= 7;
            if(width_r) {
                width &= 7;
                switch(dn) {
                case 4:
                    return {[offset, width, a, reg2]() {
                                cpu.D[reg2] =
                                    BFEXTU_M(a(), cpu.D[offset], cpu.D[width]);
                            },
                            f + 2};
                case 5:
                    return {[offset, width, a, reg2]() {
                                cpu.D[reg2] =
                                    BFEXTS_M(a(), cpu.D[offset], cpu.D[width]);
                            },
                            f + 2};
                case 6:
                    return {[offset, width, a, reg2]() {
                                cpu.D[reg2] =
                                    BFFFO_M(a(), cpu.D[offset], cpu.D[width]);
                            },
                            f + 2};
                case 7:
                    return {[offset, width, a, reg2]() {
                                BFINS_M(a(), cpu.D[offset], cpu.D[width],
                                        cpu.D[reg2]);
                            },
                            f + 2};
                }
            } else {
                width = width ? width : 32;
                switch(dn) {
                case 4:
                    return {[offset, width, a, reg2]() {
                                cpu.D[reg2] =
                                    BFEXTU_M(a(), cpu.D[offset], width);
                            },
                            f + 2};
                case 5:
                    return {[offset, width, a, reg2]() {
                                cpu.D[reg2] =
                                    BFEXTS_M(a(), cpu.D[offset], width);
                            },
                            f + 2};
                case 6:
                    return {[offset, width, a, reg2]() {
                                cpu.D[reg2] =
                                    BFFFO_M(a(), cpu.D[offset], width);
                            },
                            f + 2};
                case 7:
                    return {[offset, width, a, reg2]() {
                                BFINS_M(a(), cpu.D[offset], width, cpu.D[reg2]);
                            },
                            f + 2};
                }
            }
        } else {
            if(width_r) {
                width &= 7;
                switch(dn) {
                case 4:
                    return {[offset, width, a, reg2]() {
                                cpu.D[reg2] =
                                    BFEXTU_M(a(), offset, cpu.D[width]);
                            },
                            f + 2};
                case 5:
                    return {[offset, width, a, reg2]() {
                                cpu.D[reg2] =
                                    BFEXTS_M(a(), offset, cpu.D[width]);
                            },
                            f + 2};
                case 6:
                    return {[offset, width, a, reg2]() {
                                cpu.D[reg2] =
                                    BFFFO_M(a(), offset, cpu.D[width]);
                            },
                            f + 2};
                case 7:
                    return {[offset, width, a, reg2]() {
                                BFINS_M(a(), offset, cpu.D[width], cpu.D[reg2]);
                            },
                            f + 2};
                }
            } else {
                width = width ? width : 32;
                switch(dn) {
                case 4:
                    return {[offset, width, a, reg2]() {
                                cpu.D[reg2] = BFEXTU_M(a(), offset, width);
                            },
                            f + 2};
                case 5:
                    return {[offset, width, a, reg2]() {
                                cpu.D[reg2] = BFEXTS_M(a(), offset, width);
                            },
                            f + 2};
                case 6:
                    return {[offset, width, a, reg2]() {
                                cpu.D[reg2] = BFFFO_M(a(), offset, width);
                            },
                            f + 2};
                case 7:
                    return {[offset, width, a, reg2]() {
                                BFINS_M(a(), offset, width, cpu.D[reg2]);
                            },
                            f + 2};
                }
            }
        }
    }
    throw DecodeError{};
}

std::pair<std::function<void()>, int> decode_op14(int dn, int sz, int type,
                                                  int reg) {
    switch(sz) {
    case 0:
        return {decode_op14_0(dn, type, reg), 0};
    case 1:
        return {decode_op14_1(dn, type, reg), 0};
    case 2:
        return {decode_op14_2(dn, type, reg), 0};
    case 3:
        return decode_op14_3(dn, type, reg);
    case 4:
        return {decode_op14_4(dn, type, reg), 0};
    case 5:
        return {decode_op14_5(dn, type, reg), 0};
    case 6:
        return {decode_op14_6(dn, type, reg), 0};
    case 7:
        return decode_op14_7(dn, type, reg);
    }
    __builtin_unreachable();
}
std::pair<std::function<void()>, int> decode_fpu(int sz, int type, int reg);
std::function<void()> decode_mmu(int sz, int type, int reg);
std::pair<std::function<void()>, int> decode_op15(int dn, int sz, int type,
                                                  int reg) {
    switch(dn) {
    case 1:
        return decode_fpu(sz, type, reg);
    case 2:
        return {decode_mmu(sz, type, reg), 0};
    case 3:
        if(sz == 0) {
            switch(type) {
            case 0:
                // MOVE16 (Ay)+, (#imm32).L
                {
                    uint32_t imm32 = FETCH32(cpu.PC + 2) & 0xf;
                    return {[reg, imm32]() {
                                uint8_t buf[16];
                                BusRead16(cpu.A[reg], buf);
                                BusWrite16(imm32, buf);
                                cpu.A[reg] += 16;
                            },
                            4};
                }
            case 1:
                // MOVE16 (#imm32).L, (Ay)+
                {
                    uint32_t imm32 = FETCH32(cpu.PC + 2) & 0xf;
                    return {[reg, imm32]() {
                                uint8_t buf[16];
                                BusRead16(imm32, buf);
                                BusWrite16(cpu.A[reg], buf);
                                cpu.A[reg] += 16;
                            },
                            4};
                }
            case 2:
                // MOVE16 (Ay), (#imm32).L
                {
                    uint32_t imm32 = FETCH32(cpu.PC + 2) & 0xf;
                    return {[reg, imm32]() {
                                uint8_t buf[16];
                                BusRead16(cpu.A[reg], buf);
                                BusWrite16(imm32, buf);
                                cpu.A[reg] += 16;
                            },
                            4};
                }
            case 3:
                // MOVE16 (#imm32).L, (Ay)
                {
                    uint32_t imm32 = FETCH32(cpu.PC + 2) & 0xf;
                    return {[reg, imm32]() {
                                uint8_t buf[16];
                                BusRead16(imm32, buf);
                                BusWrite16(cpu.A[reg], buf);
                            },
                            4};
                }
            case 4:
                // MOVE16 (Ax)+., (Ay)+
                {
                    uint32_t v2 = FETCH(cpu.PC + 2) >> 12 & 7;
                    return {[reg, v2]() {
                                uint8_t buf[16];
                                BusRead16(cpu.A[reg], buf);
                                BusWrite16(cpu.A[v2], buf);
                                cpu.A[reg] += 16;
                                cpu.A[v2] += 16;
                            },
                            2};
                }
            }
        }
    }
    return {[]() { ILLEGAL_FP(); }, 0};
}
std::pair<std::function<void()>, int> decode() {
    uint16_t d = FETCH(cpu.PC);
    int tp = d >> 3 & 7;
    int rn = d & 7;
    int dn = d >> 9 & 7;
    int u = d >> 12 & 0xf;
    int ex = d >> 6 & 7;
    switch(u) {
    case 0:
        return decode_00(dn, ex, tp, rn);
    case 1: {
        // MOVE.B
        if(ex == 1) {
            throw DecodeError{};
        }
        auto [r, r_off] = ea_read8(tp, rn, cpu.PC + 2);
        auto [w, w_off] = ea_write8(ex, dn, cpu.PC + 2 + r_off);
        return {[r, w]() {
                    uint8_t v = r();
                    TEST_B(v);
                    cpu.V = cpu.C = false;
                    w(v);
                },
                r_off + w_off};
    }
    case 2: {
        // MOVE.L
        auto [r, r_off] = ea_read32(tp, rn, cpu.PC + 2);
        if(ex == 1) {
            // MOVEA.L
            return {[r, dn]() { cpu.A[dn] = r(); }, r_off};
        }
        auto [w, w_off] = ea_write32(ex, dn, cpu.PC + 2 + r_off);
        return {[r, w]() {
                    uint32_t v = r();
                    TEST_L(v);
                    cpu.V = cpu.C = false;
                    w(v);
                },
                r_off + w_off};
    }
    case 3: {
        // MOVE.W
        auto [r, r_off] = ea_read16(tp, rn, cpu.PC + 2);
        if(ex == 1) {
            // MOVEA.W
            return {[r, dn]() { cpu.A[dn] = static_cast<int16_t>(r()); },
                    r_off};
        }
        auto [w, w_off] = ea_write16(ex, dn, cpu.PC + 2 + r_off);
        return {[r, w]() {
                    uint16_t v = r();
                    TEST_W(v);
                    cpu.V = cpu.C = false;
                    w(v);
                },
                r_off + w_off};
    }
    case 4:
        return decode_op4(dn, ex, tp, rn);
    case 5:
        return decode_op5(dn, ex, tp, rn);
    case 6: {
        int cc = d >> 8 & 15;
        int8_t im = d & 0xff;
        return decode_op6(cc, im);
    }
    case 7: {
        int8_t imm = d & 0xff;
        if(d & 1 << 8) {
            // TODO: BasisiliskII emu op
            throw DecodeError{};
        }
        // MOVEQ
        return {[dn, imm]() { cpu.D[dn] = imm; }, 0};
    }
    case 8:
        return decode_op8(dn, ex, tp, rn);
    case 9:
        return decode_op9(dn, ex, tp, rn);
    case 10:
        return {[]() { ALINE(); }, 0};
    case 11:
        return decode_op11(dn, ex, tp, rn);
    case 12:
        return decode_op12(dn, ex, tp, rn);
    case 13:
        return decode_op13(dn, ex, tp, rn);
    case 14:
        return decode_op14(dn, ex, tp, rn);
    case 15:
        return decode_op15(dn, ex, tp, rn);
    }
    throw DecodeError{};
}
