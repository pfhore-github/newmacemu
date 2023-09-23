#include "68040.hpp"
#include "exception.hpp"
#include "memory.hpp"
#include "proto.hpp"

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

uint32_t Get_TTR_t(const Cpu::TTR_t &x) {
    return x.logic_base << 24 | x.logic_mask << 16 | x.E << 15 | x.S << 13 |
           x.U << 8 | x.CM << 5 | x.W << 2;
}
void Set_TTR_t(Cpu::TTR_t &x, uint32_t v) {
    x.logic_base = v >> 24 & 0xff;
    x.logic_mask = v >> 16 & 0xff;
    x.E = v & 1 << 15;
    x.S = v >> 13 & 3;
    x.U = v >> 8 & 3;
    x.CM = v >> 5 & 3;
    x.W = v & 1 << 2;
}
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

    Cpu::atc_entry e = {static_cast<uint32_t>(pg.paddr),
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
    if(W && !entry.W && !entry.M && !(entry.S && !sys)) {
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
__attribute__((noreturn)) void ATC_Fault() {
    cpu.af_value.ATC = true;
    throw AccessFault{};
}
uint32_t ptest_and_raise(uint32_t addr, bool sys, bool code, bool W) {
    auto ret = ptest(addr >> 12, sys, code, W);
    if(ret.B) {
        ATC_Fault();
    }
    if(!ret.R) {
        ATC_Fault();
    }
    if(ret.W && W) {
        ATC_Fault();
    }
    if(ret.S && !sys) {
        ATC_Fault();
    }
    return (ret.paddr << 12) | (addr & 0xfff);
}

static uint32_t page_size() { return cpu.TCR_P ? 0x2000 : 0x100; }
void op_ptest(uint32_t addr, bool w);
extern run_t run_table[0x10000];

void cinvl_d(uint32_t base) {
    base &= ~0xf;
    (void)base;
    // date cache is not implemented
}

void cinvp_d(uint32_t base) {
    base &= ~(page_size() - 1);
    (void)base;
    // date cache is not implemented
}

void cinva_d() {
    // date cache is not implemented
}

void cpushl_d(uint32_t base) {
    base &= ~0xf;
    (void)base;
    // date cache is not implemented
}

void cpushp_d(uint32_t base) {
    base &= ~(page_size() - 1);
    (void)base;
    // date cache is not implemented
}

void cpusha_d() {
    // date cache is not implemented
}

void cinvl_i(uint32_t base) {
    base &= ~0xf;
    (void)base;
    // TODO: JIT cache must be refreshed
}

void cinvp_i(uint32_t base) {
    base &= ~(page_size() - 1);
    (void)base;
    // TODO: JIT cache must be refreshed
}

void cinva_i() {
    // TODO: JIT cache must be refreshed
}

void cpushl_i(uint32_t base) {
    base &= ~0xf;
    (void)base;
    // TODO: JIT cache must be refreshed
}

void cpushp_i(uint32_t base) {
    base &= ~(page_size() - 1);
    (void)base;
    // TODO: JIT cache must be refreshed
}

void cpusha_i() {
    // TODO: JIT cache must be refreshed
}

namespace OP {
void cinvl_dc(uint16_t, int, int, int reg) {
    PRIV_CHECK();
    cinvl_d(cpu.A[reg]);
    TRACE_BRANCH();
}

void cinvp_dc(uint16_t, int, int, int reg) {
    PRIV_CHECK();
    cinvp_d(cpu.A[reg]);
    TRACE_BRANCH();
}

void cinva_dc(uint16_t, int, int, int) {
    PRIV_CHECK();
    cinva_d();
    TRACE_BRANCH();
}

void cpushl_dc(uint16_t, int, int, int reg) {
    PRIV_CHECK();
    cpushl_d(cpu.A[reg]);
    TRACE_BRANCH();
}

void cpushp_dc(uint16_t, int, int, int reg) {
    PRIV_CHECK();
    cpushp_d(cpu.A[reg]);
    TRACE_BRANCH();
}

void cpusha_dc(uint16_t, int, int, int) {
    PRIV_CHECK();
    cpusha_d();
    TRACE_BRANCH();
}

void cinvl_ic(uint16_t, int, int, int reg) {
    PRIV_CHECK();
    cinvl_i(cpu.A[reg]);
    TRACE_BRANCH();
}

void cinvp_ic(uint16_t, int, int, int reg) {
    PRIV_CHECK();
    cinvp_i(cpu.A[reg]);
    TRACE_BRANCH();
}

void cinva_ic(uint16_t, int, int, int) {
    PRIV_CHECK();
    cinva_i();
    TRACE_BRANCH();
}

void cpushl_ic(uint16_t, int, int, int reg) {
    PRIV_CHECK();
    cpushl_i(cpu.A[reg]);
    TRACE_BRANCH();
}

void cpushp_ic(uint16_t, int, int, int reg) {
    PRIV_CHECK();
    cpushp_i(cpu.A[reg]);
    TRACE_BRANCH();
}

void cpusha_ic(uint16_t, int, int, int) {
    PRIV_CHECK();
    cpusha_i();
    TRACE_BRANCH();
}

void cinvl_bc(uint16_t, int, int, int reg) {
    PRIV_CHECK();
    cinvl_d(cpu.A[reg]);
    cinvl_i(cpu.A[reg]);
    TRACE_BRANCH();
}

void cinvp_bc(uint16_t, int, int, int reg) {
    PRIV_CHECK();
    cinvp_d(cpu.A[reg]);
    cinvp_i(cpu.A[reg]);
    TRACE_BRANCH();
}

void cinva_bc(uint16_t, int, int, int) {
    PRIV_CHECK();
    cinva_d();
    cinva_i();
    TRACE_BRANCH();
}

void cpushl_bc(uint16_t, int, int, int reg) {
    PRIV_CHECK();
    cpushl_d(cpu.A[reg]);
    cpushl_i(cpu.A[reg]);
    TRACE_BRANCH();
}

void cpushp_bc(uint16_t, int, int, int reg) {
    PRIV_CHECK();
    cpushp_d(cpu.A[reg]);
    cpushp_i(cpu.A[reg]);
    TRACE_BRANCH();
}

void cpusha_bc(uint16_t, int, int, int) {
    PRIV_CHECK();
    cpusha_d();
    cpusha_i();
    TRACE_BRANCH();
}

void pflushn(uint16_t, int, int, int reg) {
    PRIV_CHECK();
    uint32_t addr = cpu.A[reg] >> 12;
    if(cpu.DFC == 1 || cpu.DFC == 2) {
        cpu.u_atc.erase(addr);
    } else if(cpu.DFC == 5 || cpu.DFC == 6) {
        cpu.s_atc.erase(addr);
    }
    TRACE_BRANCH();
}

void pflush(uint16_t, int, int, int reg) {
    PRIV_CHECK();
    uint32_t addr = cpu.A[reg] >> 12;
    if(cpu.DFC == 1 || cpu.DFC == 2) {
        cpu.u_atc.erase(addr);
        cpu.ug_atc.erase(addr);
    } else if(cpu.DFC == 5 || cpu.DFC == 6) {
        cpu.s_atc.erase(addr);
        cpu.sg_atc.erase(addr);
    }
    TRACE_BRANCH();
}

void pflushan(uint16_t, int, int, int) {
    PRIV_CHECK();
    if(cpu.DFC == 1 || cpu.DFC == 2) {
        cpu.u_atc.clear();
    } else if(cpu.DFC == 5 || cpu.DFC == 6) {
        cpu.s_atc.clear();
    }
    TRACE_BRANCH();
}

void pflusha(uint16_t, int, int, int) {
    PRIV_CHECK();
    if(cpu.DFC == 1 || cpu.DFC == 2) {
        cpu.u_atc.clear();
        cpu.ug_atc.clear();
    } else if(cpu.DFC == 5 || cpu.DFC == 6) {
        cpu.s_atc.clear();
        cpu.sg_atc.clear();
    }
    TRACE_BRANCH();
}

void ptestr(uint16_t, int, int, int reg) {
    PRIV_CHECK();
    op_ptest(cpu.A[reg], false);
    TRACE_BRANCH();
}

void ptestw(uint16_t, int, int, int reg) {
    PRIV_CHECK();
    op_ptest(cpu.A[reg], true);
    TRACE_BRANCH();
}

} // namespace OP
void init_run_table_mmu() {
    for(int r = 0; r < 8; ++r) {
        run_table[0172110 | r] = OP::cinvl_dc;
        run_table[0172120 | r] = OP::cinvp_dc;
        run_table[0172130 | r] = OP::cinva_dc;
        run_table[0172150 | r] = OP::cpushl_dc;
        run_table[0172160 | r] = OP::cpushp_dc;
        run_table[0172170 | r] = OP::cpusha_dc;

        run_table[0172210 | r] = OP::cinvl_ic;
        run_table[0172220 | r] = OP::cinvp_ic;
        run_table[0172230 | r] = OP::cinva_ic;
        run_table[0172250 | r] = OP::cpushl_ic;
        run_table[0172260 | r] = OP::cpushp_ic;
        run_table[0172270 | r] = OP::cpusha_ic;

        run_table[0172310 | r] = OP::cinvl_bc;
        run_table[0172320 | r] = OP::cinvp_bc;
        run_table[0172330 | r] = OP::cinva_bc;
        run_table[0172350 | r] = OP::cpushl_bc;
        run_table[0172360 | r] = OP::cpushp_bc;
        run_table[0172370 | r] = OP::cpusha_bc;

        run_table[0172400 | r] = OP::pflushn;
        run_table[0172410 | r] = OP::pflush;
        run_table[0172420 | r] = OP::pflushan;
        run_table[0172430 | r] = OP::pflusha;

        run_table[0172510 | r] = OP::ptestw;
        run_table[0172550 | r] = OP::ptestr;
    }
}

void op_ptest(uint32_t addr, bool w) {
    addr >>= 12;
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
bool movec_from_cr_impl(uint16_t extw, uint32_t* rn) {
    switch(extw) {
    case 0:
        *rn = cpu.SFC;
        break;
    case 1:
        *rn = cpu.DFC;
        break;
    case 2:
        *rn = cpu.CACR_DE << 31 | cpu.CACR_IE << 15;
        break;
    case 3:
        *rn = cpu.TCR_E << 15 | cpu.TCR_P << 14;
        break;
    case 4:
        *rn = Get_TTR_t(cpu.ITTR[0]);
        break;
    case 5:
        *rn = Get_TTR_t(cpu.ITTR[1]);
        break;
    case 6:
        *rn = Get_TTR_t(cpu.DTTR[0]);
        break;
    case 7:
        *rn = Get_TTR_t(cpu.DTTR[1]);
        break;
    case 0x800:
        *rn = cpu.USP;
        break;
    case 0x801:
        *rn = cpu.VBR;
        break;
    case 0x803:
        if(cpu.M) {
            cpu.MSP = cpu.A[7];
        }
        *rn = cpu.MSP;
        break;
    case 0x804:
        if(!cpu.M) {
            cpu.ISP = cpu.A[7];
        }
        *rn = cpu.ISP;
        break;
    case 0x805:
        *rn = cpu.MMUSR;
        break;
    case 0x806:
        *rn = cpu.URP;
        break;
    case 0x807:
        *rn = cpu.SRP;
        break;
    default:
        return false;
    }
    return true;
}

bool movec_to_cr_impl(uint16_t extw, uint32_t v) {
     switch(extw & 0xfff) {
    case 0:
        cpu.SFC = v & 7;
        break;
    case 1:
        cpu.DFC = v & 7;
        break;
    case 2:
        cpu.CACR_DE = v >> 31 & 1;
        cpu.CACR_IE = v >> 15 & 1;
        break;
    case 3:
        cpu.TCR_E = v >> 15 & 1;
        cpu.TCR_P = v >> 14 & 1;
        break;
    case 4:
        Set_TTR_t(cpu.ITTR[0], v);
        break;
    case 5:
        Set_TTR_t(cpu.ITTR[1], v);
        break;
    case 6:
        Set_TTR_t(cpu.DTTR[0], v);
        break;
    case 7:
        Set_TTR_t(cpu.DTTR[1], v);
        break;
    case 0x800:
        cpu.USP = v;
        break;
    case 0x801:
        cpu.VBR = v;
        break;
    case 0x803:
        cpu.MSP = v;
        if(cpu.M) {
            cpu.A[7] = cpu.MSP;
        }
        break;
    case 0x804:
        cpu.ISP = v;
        if(!cpu.M) {
            cpu.A[7] = cpu.ISP;
        }
        break;
    case 0x805:
        cpu.MMUSR = v;
        break;
    case 0x806:
        cpu.URP = v & ~0x1ff;
        break;
    case 0x807:
        cpu.SRP = v & ~0x1ff;
        break;
    default:
        return false;
    }
    return true;
}
namespace OP {
void movec_from_cr(uint16_t, int, int, int) {
    PRIV_CHECK();
    uint16_t extw = FETCH();
    int rn = extw >> 12 & 15;
    if( ! movec_from_cr_impl(extw & 0xfff, &cpu.R(rn)) ) {
        ILLEGAL_OP();
    }
    TRACE_BRANCH();
}
void movec_to_cr(uint16_t, int, int, int) {
    PRIV_CHECK();
    uint16_t extw = FETCH();
    int rn = extw >> 12 & 15;
      if( ! movec_to_cr_impl(extw & 0xfff, cpu.R(rn)) ) {
        ILLEGAL_OP();
    }

    TRACE_BRANCH();
}
} // namespace OP