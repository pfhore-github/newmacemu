#include "68040.hpp"
#include "exception.hpp"
#include "proto.hpp"
#include "memory.hpp"

struct __attribute__((packed)) mmu_result {
    bool R : 1 = false;
    bool T : 1 = false;
    bool W : 1 = false;
    bool : 1;
    bool M : 1 = false;
    unsigned int CM : 2 = 0;
    bool S : 1 = false;
    unsigned int Ux : 2 = 0;
    bool G : 1 = false;
    bool B : 1 = false;
    unsigned int paddr : 20 = 0;
};
static_assert(sizeof(mmu_result) == 4);

struct __attribute__((packed)) addr_e {
    unsigned int pgi : 6;
    unsigned int pi : 7;
    unsigned int ri : 7;
    unsigned int : 12;
};
static_assert(sizeof(addr_e) == 4);
struct __attribute__((packed)) addr_p {
    unsigned int offset_h : 1;
    unsigned int pgi : 5;
    unsigned int pi : 7;
    unsigned int ri : 7;
    unsigned int : 12;
};
static_assert(sizeof(addr_p) == 4);

struct __attribute__((packed)) root_dsc {
    unsigned int UDT : 2;
    bool W : 1;
    bool U : 2;
    unsigned int : 5;
    unsigned int paddr : 22;
};
static_assert(sizeof(root_dsc) == 4);

struct __attribute__((packed)) ptr_dsc {
    unsigned int UDT : 2;
    bool W : 1;
    bool U : 2;
    unsigned int : 4;
    unsigned int paddr : 23;
};
static_assert(sizeof(ptr_dsc) == 4);

struct __attribute__((packed)) ptrP_dsc {
    unsigned int UDT : 2;
    bool W : 1;
    bool U : 2;
    unsigned : 3;
    unsigned int paddr : 24;
};
static_assert(sizeof(ptrP_dsc) == 4);

struct __attribute__((packed)) page_dsc {
    unsigned int PDT : 2;
    bool W : 1;
    bool U : 1;
    bool M : 1;
    unsigned int CM : 2;
    bool S : 1;
    unsigned int Ux : 2;
    bool G : 1;
    unsigned : 1;
    unsigned int paddr : 20;
};
static_assert(sizeof(page_dsc) == 4);

uint16_t Get_TTR_t(const Cpu::TTR_t &x) {
    return x.logic_base << 24 | x.logic_mask << 16 | x.E << 15 | x.S << 13 |
           x.U << 8 | x.CM << 5 | x.W << 2;
}
void Set_TTR_t(Cpu::TTR_t &x, uint16_t v) {
    x.logic_base = v >> 24 & 0xff;
    x.logic_mask = v >> 16 & 0xff;
    x.E = v & 1 << 15;
    x.S = v >> 13 & 3;
    x.U = v >> 8 & 3;
    x.CM = v >> 5 & 3;
    x.W = v & 1 << 2;
}
struct AccessFault {};
mmu_result TEST_TTR(const Cpu::TTR_t &x, uint32_t addr, bool S) {
    if(!x.E) {
        return {};
    }
    if((x.S == 0 && S) || (x.S == 1 && !S)) {
        return {};
    }
    uint8_t base = addr >> 12;
    if((base & ~x.logic_mask) == (x.logic_base & ~x.logic_mask)) {
        return {
            .R = true,
            .T = true,
            .W = x.W,
            .paddr = addr,
        };
    }
    return {};
}
mmu_result ptest_ttr(uint32_t addr, bool sys, bool code) {
    if(code) {
        auto ret = TEST_TTR(cpu.ITTR[0], addr, sys);
        if(ret.R) {
            return ret;
        }
        ret = TEST_TTR(cpu.ITTR[1], addr, sys);
        if(ret.R) {
            return ret;
        }
    } else {
        auto ret = TEST_TTR(cpu.DTTR[0], addr, sys);
        if(ret.R) {
            return ret;
        }
        ret = TEST_TTR(cpu.DTTR[1], addr, sys);
        if(ret.R) {
            return ret;
        }
    }
    return {.R = false};
}
uint32_t BusReadL(uint32_t addr);
void BusWriteL(uint32_t addr, uint32_t v);

std::unordered_map<uint32_t, Cpu::atc_entry>::iterator
atc_search_not_found(uint32_t addr, bool s) {
    Cpu::atc_entry notfound = {0, 0, false, 0, false, false, false};
    if(s) {
        return cpu.sg_atc.insert_or_assign(addr, notfound).first;
    } else {
        return cpu.ug_atc.insert_or_assign(addr, notfound).first;
    }
}

std::unordered_map<uint32_t, Cpu::atc_entry>::iterator
atc_set(uint32_t addr, uint32_t pg_addr, bool W, bool wp, bool s) {
    uint32_t pg_v = BusReadL(pg_addr);
    page_dsc pg;
    if((pg_v & 3) == 0) {
        return atc_search_not_found(addr, s);
    } else if((pg_v & 3) == 2) {
        pg_addr = pg_v & ~3;
        pg = std::bit_cast<page_dsc>(BusReadL(pg_addr));
    } else {
        pg = std::bit_cast<page_dsc>(pg_v);
    }
    pg.U = true;
    wp = wp || pg.W;
    // only accepatble access
    if(!(wp && W) && !(pg.S && !s)) {
        if(!pg.M && W) {
            pg.M = true;
        }
    }
    BusWriteL(pg_addr, std::bit_cast<uint32_t>(pg));

    Cpu::atc_entry e = {static_cast<uint32_t>(pg.paddr << 12),
                        static_cast<uint8_t>(pg.Ux),
                        pg.S,
                        static_cast<uint8_t>(pg.CM),
                        pg.M,
                        wp,
                        true};
    if(cpu.TCR_P) {
        e.paddr &= ~1;
    }
    if(s) {
        if(pg.G) {
            return cpu.sg_atc.insert_or_assign(addr, e).first;
        } else {
            return cpu.s_atc.insert_or_assign(addr, e).first;
        }
    } else {
        if(pg.G) {
            return cpu.ug_atc.insert_or_assign(addr, e).first;
        } else {
            return cpu.u_atc.insert_or_assign(addr, e).first;
        }
    }
}
template <class T> std::optional<T> pdt_lookup(uint32_t addr) {
    auto urp = std::bit_cast<T>(BusReadL(addr));
    if(urp.UDT < 2) {
        return {};
    }
    urp.U = true;
    BusWriteL(addr, std::bit_cast<uint32_t>(urp));
    return urp;
}

std::unordered_map<uint32_t, Cpu::atc_entry>::iterator
atc_searchP(uint32_t addr, bool s, bool W) {
    auto ap = std::bit_cast<addr_p>(addr);
    uint32_t ur_addr = (s ? cpu.SRP : cpu.URP) | (ap.ri << 2);
    auto urp = pdt_lookup<root_dsc>(ur_addr);
    if(!urp) {
        return atc_search_not_found(addr, s);
    }
    uint32_t pt_addr = urp->paddr << 10 | (ap.pi << 2);
    auto pt = pdt_lookup<ptrP_dsc>(pt_addr);
    if(!pt) {
        return atc_search_not_found(addr, s);
    }

    uint32_t pg_addr = pt->paddr << 8 | (ap.pgi << 2);

    return atc_set(addr, pg_addr, W, urp->W || pt->W, s);
}

std::unordered_map<uint32_t, Cpu::atc_entry>::iterator
atc_search(uint32_t addr, bool s, bool W) {
    auto ap = std::bit_cast<addr_e>(addr);
    uint32_t ur_addr = (s ? cpu.SRP : cpu.URP) | (ap.ri << 2);
    auto urp = pdt_lookup<root_dsc>(ur_addr);
    if(!urp) {
        return atc_search_not_found(addr, s);
    }
    uint32_t pt_addr = urp->paddr << 10 | (ap.pi << 2);
    auto pt = pdt_lookup<ptr_dsc>(pt_addr);
    if(!pt) {
        return atc_search_not_found(addr, s);
    }

    uint32_t pg_addr = pt->paddr << 9 | (ap.pgi << 2);
    return atc_set(addr, pg_addr, W, urp->W || pt->W, s);
}
mmu_result ptest(uint32_t addr, bool sys, bool code, bool W) {
    auto ret = ptest_ttr(addr, sys, code);
    if(ret.R) {
        if(ret.W && W) {
            return {.R = false};
        } else {
            return ret;
        }
    }
    if(!cpu.TCR_E) {
        return {.R = true, .paddr = addr};
    }
    std::unordered_map<uint32_t, Cpu::atc_entry>::iterator atc_found;
    bool G = false;
    // ATC check
    if(sys) {
        atc_found = cpu.s_atc.find(addr);
        if(atc_found != cpu.s_atc.end()) {
            goto FOUND;
        }
        atc_found = cpu.sg_atc.find(addr);
        if(atc_found != cpu.sg_atc.end()) {
            G = true;
            goto FOUND;
        }
    } else {
        atc_found = cpu.u_atc.find(addr);
        if(atc_found != cpu.u_atc.end()) {
            goto FOUND;
        }
        atc_found = cpu.ug_atc.find(addr);
        if(atc_found != cpu.ug_atc.end()) {
            G = true;
            goto FOUND;
        }
    }
ATC_SEARCH:
    try {
        atc_found =
            cpu.TCR_P ? atc_searchP(addr, sys, W) : atc_search(addr, sys, W);
    } catch(AccessFault &) {
        return {.B = true};
    }
FOUND : {
    auto &entry = atc_found->second;
    if(W && !entry.W && ! entry.M && !(entry.S && !sys))  {
        if(sys) {
            cpu.s_atc.erase(addr);
            cpu.sg_atc.erase(addr);
        } else {
            cpu.u_atc.erase(addr);
            cpu.ug_atc.erase(addr);
        }
        goto ATC_SEARCH;
    }
    mmu_result ret = {.R = entry.R,
                      .W = entry.W,
                      .M = entry.M,
                      .CM = entry.CM,
                      .S = entry.S,
                      .Ux = entry.U,
                      .G = G,
                      .paddr = entry.paddr};
    if(cpu.TCR_P && entry.R) {
        ret.paddr |= (addr & 0x1000);
    }
    return ret;
}
}
uint32_t ptest_and_raise(uint32_t addr, bool sys, bool code, bool W) {
    auto ret = ptest(addr, sys, code, W);
    if(!ret.R) {
        cpu.af_value.MA = true;
        throw AccessFault{};
    }
    if(ret.W && W) {
        cpu.af_value.MA = true;
        throw AccessFault{};
    }
    if(ret.B) {
        cpu.af_value.MA = true;
        throw AccessFault{};
    }
    if(ret.S && !sys) {
        cpu.af_value.MA = true;
        throw AccessFault{};
    }
    return ret.paddr << 12;
}

std::function<void()> from_cc(int reg, int cc) {
    switch(cc) {
    case 0:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.R(reg) = cpu.SFC;
            if(cpu.T == 1) {
                cpu.must_trace = true;
            }
        };
    case 1:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.R(reg) = cpu.DFC;
            if(cpu.T == 1) {
                cpu.must_trace = true;
            }
        };
    case 2:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.R(reg) = cpu.CACR_DE << 31 | cpu.CACR_IE << 15;
            if(cpu.T == 1) {
                cpu.must_trace = true;
            }
        };
    case 3:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.R(reg) = cpu.TCR_E << 15 | cpu.TCR_P << 14;
            if(cpu.T == 1) {
                cpu.must_trace = true;
            }
        };
    case 4:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.R(reg) = Get_TTR_t(cpu.ITTR[0]);
            if(cpu.T == 1) {
                cpu.must_trace = true;
            }
        };
    case 5:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.R(reg) = Get_TTR_t(cpu.ITTR[1]);
            if(cpu.T == 1) {
                cpu.must_trace = true;
            }
        };
    case 6:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.R(reg) = Get_TTR_t(cpu.DTTR[0]);
            if(cpu.T == 1) {
                cpu.must_trace = true;
            }
        };
    case 7:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.R(reg) = Get_TTR_t(cpu.DTTR[1]);
            if(cpu.T == 1) {
                cpu.must_trace = true;
            }
        };
    case 0x800:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.R(reg) = cpu.USP;
            if(cpu.T == 1) {
                cpu.must_trace = true;
            }
        };
    case 0x801:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.R(reg) = cpu.VBR;
            if(cpu.T == 1) {
                cpu.must_trace = true;
            }
        };
    case 0x803:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.R(reg) = cpu.MSP;
            if(cpu.T == 1) {
                cpu.must_trace = true;
            }
        };
    case 0x804:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.R(reg) = cpu.ISP;
            if(cpu.T == 1) {
                cpu.must_trace = true;
            }
        };
    case 0x805:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.R(reg) = cpu.MMUSR;
            if(cpu.T == 1) {
                cpu.must_trace = true;
            }
        };
    case 0x806:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.R(reg) = cpu.URP;
            if(cpu.T == 1) {
                cpu.must_trace = true;
            }
        };
    case 0x807:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.R(reg) = cpu.SRP;
            if(cpu.T == 1) {
                cpu.must_trace = true;
            }
        };
    default:
        throw DecodeError{};
    }
}
std::function<void()> to_cc(int reg, int cc) {
    switch(cc) {
    case 0:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.SFC = cpu.R(reg) & 7;
            if(cpu.T == 1) {
                cpu.must_trace = true;
            }
        };
    case 1:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.DFC = cpu.R(reg) & 7;
            if(cpu.T == 1) {
                cpu.must_trace = true;
            }
        };
    case 2:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.CACR_DE = cpu.R(reg) >> 31 & 1;
            cpu.CACR_IE = cpu.R(reg) >> 15 & 1;
            if(cpu.T == 1) {
                cpu.must_trace = true;
            }
        };
    case 3:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.TCR_E = cpu.R(reg) >> 15 & 1;
            cpu.TCR_P = cpu.R(reg) >> 14 & 1;
            if(cpu.T == 1) {
                cpu.must_trace = true;
            }
        };
    case 4:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            Set_TTR_t(cpu.ITTR[0], cpu.R(reg));
            if(cpu.T == 1) {
                cpu.must_trace = true;
            }
        };
    case 5:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            Set_TTR_t(cpu.ITTR[1], cpu.R(reg));
            if(cpu.T == 1) {
                cpu.must_trace = true;
            }
        };
    case 6:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            Set_TTR_t(cpu.DTTR[0], cpu.R(reg));
            if(cpu.T == 1) {
                cpu.must_trace = true;
            }
        };
    case 7:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            Set_TTR_t(cpu.DTTR[1], cpu.R(reg));
            if(cpu.T == 1) {
                cpu.must_trace = true;
            }
        };
    case 0x800:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.USP = cpu.R(reg);
            if(cpu.T == 1) {
                cpu.must_trace = true;
            }
        };
    case 0x801:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.VBR = cpu.R(reg);
            if(cpu.T == 1) {
                cpu.must_trace = true;
            }
        };
    case 0x803:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.MSP = cpu.R(reg);
            if(cpu.T == 1) {
                cpu.must_trace = true;
            }
        };
    case 0x804:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.ISP = cpu.R(reg);
            if(cpu.T == 1) {
                cpu.must_trace = true;
            }
        };
    case 0x805:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.MMUSR = cpu.R(reg);
            if(cpu.T == 1) {
                cpu.must_trace = true;
            }
        };
    case 0x806:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.URP = cpu.R(reg) & ~0x1ff;
            if(cpu.T == 1) {
                cpu.must_trace = true;
            }
        };
    case 0x807:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.SRP = cpu.R(reg) & ~0x1ff;

            if(cpu.T == 1) {
                cpu.must_trace = true;
            }
        };
    default:
        throw DecodeError{};
    }
}
static uint32_t page_mask() { return cpu.TCR_P ? 0x1FFF : 0xFFF; }
void pflushn(uint32_t an);
void pflush(uint32_t an);
void pflushan();
void pflusha();
void op_ptest(uint32_t addr, bool w);
std::function<void()> decode_mmu(int sz, int type, int reg) {
    switch(sz) {
    case 1:
        // CINVL DC, *
        return []() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            // date cache is not implemented
            if(cpu.T == 1) {
                cpu.must_trace = true;
            }
        };
    case 2:
    case 3:
        // CINVL IC/BC
        switch(type) {
        case 1:
        case 5:
            // CINVL/CPUSHL IC, (An)
            return [reg]() {
                if(!cpu.S) {
                    PRIV_ERROR();
                }
                uint32_t addr = cpu.A[reg] & ~0xf;
                for(uint32_t i = 0; i < 16; i += 2) {
                    cpu.icache.erase(addr + i);
                }
                if(cpu.T == 1) {
                    cpu.must_trace = true;
                }
            };
        case 2:
        case 6:
            // CINVP IC, (An)
            return [reg]() {
                if(!cpu.S) {
                    PRIV_ERROR();
                }
                uint32_t next = page_mask();
                uint32_t addr = cpu.A[reg] & ~next;
                for(uint32_t a = 0; a < next; a += 2) {
                    cpu.icache.erase(addr + a);
                }
                if(cpu.T == 1) {
                    cpu.must_trace = true;
                }
            };
        case 3:
        case 7:
            // CINVPA
            return [reg]() {
                if(!cpu.S) {
                    PRIV_ERROR();
                }
                cpu.icache.clear();
                if(cpu.T == 1) {
                    cpu.must_trace = true;
                }
            };
        }
        break;
    case 4:
        switch(type) {
        case 0:
            return [reg]() {
                if(!cpu.S) {
                    PRIV_ERROR();
                }
                pflushn(cpu.A[reg]);
                if(cpu.T == 1) {
                    cpu.must_trace = true;
                }
            };
        case 1:
            return [reg]() {
                if(!cpu.S) {
                    PRIV_ERROR();
                }
                pflush(cpu.A[reg]);
                if(cpu.T == 1) {
                    cpu.must_trace = true;
                }
            };
        case 2:
            return []() {
                if(!cpu.S) {
                    PRIV_ERROR();
                }
                pflushan();
                if(cpu.T == 1) {
                    cpu.must_trace = true;
                }
            };
        case 3:
            return []() {
                if(!cpu.S) {
                    PRIV_ERROR();
                }
                pflusha();
                if(cpu.T == 1) {
                    cpu.must_trace = true;
                }
            };
        }
        break;
    case 5:
        switch(type) {
        case 1:
            return [reg]() {
                if(!cpu.S) {
                    PRIV_ERROR();
                }
                op_ptest(cpu.A[reg], false);
                if(cpu.T == 1) {
                    cpu.must_trace = true;
                }
            };
        case 5:
            return [reg]() {
                if(!cpu.S) {
                    PRIV_ERROR();
                }
                op_ptest(cpu.A[reg], true);
                if(cpu.T == 1) {
                    cpu.must_trace = true;
                }
            };
        }
    }
    throw DecodeError{};
}

void pflushn(uint32_t an) {
    an &= ~0xFFF;
    if(cpu.DFC == 1 || cpu.DFC == 2) {
        cpu.u_atc.erase(an);
    } else if(cpu.DFC == 5 || cpu.DFC == 6) {
        cpu.s_atc.erase(an);
    }
}
void pflush(uint32_t an) {
    an &= ~0xFFF;
    if(cpu.DFC == 1 || cpu.DFC == 2) {
        cpu.u_atc.erase(an);
        cpu.ug_atc.erase(an);
    } else if(cpu.DFC == 5 || cpu.DFC == 6) {
        cpu.s_atc.erase(an);
        cpu.sg_atc.erase(an);
    }
}
void pflushan() {
    if(cpu.DFC == 1 || cpu.DFC == 2) {
        cpu.u_atc.clear();
    } else if(cpu.DFC == 5 || cpu.DFC == 6) {
        cpu.s_atc.clear();
    }
}
void pflusha() {
    if(cpu.DFC == 1 || cpu.DFC == 2) {
        cpu.u_atc.clear();
        cpu.ug_atc.clear();
    } else if(cpu.DFC == 5 || cpu.DFC == 6) {
        cpu.s_atc.clear();
        cpu.sg_atc.clear();
    }
}
void op_ptest(uint32_t addr, bool w) {
    addr &= ~0xfff;
    switch(cpu.DFC) {
    case 1:
        // USER_DATA
        cpu.u_atc.erase(addr);
        cpu.ug_atc.erase(addr);
        cpu.MMUSR = std::bit_cast<uint32_t>(ptest(addr, false, false, w));
        break;
    case 2:
        // USER_CODE
        cpu.u_atc.erase(addr);
        cpu.ug_atc.erase(addr);
        cpu.MMUSR = std::bit_cast<uint32_t>(ptest(addr, false, true, w));
        break;
    case 5:
        // SYS_DATA
        cpu.s_atc.erase(addr);
        cpu.sg_atc.erase(addr);
        cpu.MMUSR = std::bit_cast<uint32_t>(ptest(addr, true, false, w));
        break;
    case 6:
        // SYS_CODE
        cpu.s_atc.erase(addr);
        cpu.sg_atc.erase(addr);
        cpu.MMUSR = std::bit_cast<uint32_t>(ptest(addr, true, true, w));
        break;
    }
}