#include "68040.hpp"
#include "bus.hpp"
#include "exception.hpp"
#include "inline.hpp"
#include "memory.hpp"
#include "mmu.hpp"
#include <expected>
#include <optional>
struct addr_e {
    addr_e(uint32_t v, bool large)
        : pgi(large ? ((v >> 1) & 0x1f) : (v & 0x3f)), pi(v >> 6 & 0x7f),
          ri(v >> 13 & 0x7f), offset_h(large ? v & 1 : false) {}
    uint8_t pgi;
    uint8_t pi;
    uint8_t ri;
    bool offset_h;
};
struct jit_cache;
// [sys/user][key]
atc_entry d_atc[2][16], i_atc[2][16];

extern std::unordered_map<uint32_t, std::shared_ptr<jit_cache>> jit_tables;

constexpr uint32_t DESP_W = 1 << 2;
constexpr uint32_t DESP_U = 1 << 3;
constexpr uint32_t DESP_M = 1 << 4;
constexpr uint32_t DESP_S = 1 << 7;
constexpr uint32_t DESP_G = 1 << 10;

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
        mmu_result re;
        re.R = true;
        re.T = true;
        re.W = x.W;
        re.paddr = addr;
        return re;
    }
    return {};
}
mmu_result ptest_ttr(uint32_t addr, bool sys, bool code) {
    auto ttrp = code ? cpu.ITTR : cpu.DTTR;
    for(int i = 0; i < 2; ++i) {
        auto ret = TEST_TTR(ttrp[i], addr, sys);
        if(ret.R) {
            return ret;
        }
    }
    return {.R = false};
}

void atc_search_not_found(atc_entry *atcp, uint32_t addr) {
    atcp[addr & 0xf].laddr = addr;
    atcp[addr & 0xf].V = true;
    atcp[addr & 0xf].R = false;
}

void atc_set(atc_entry *atcp, uint32_t addr, uint32_t pg_addr, bool W, bool wp,
             bool s) {
    uint32_t pg = BusReadL(pg_addr);
    if((pg & 3) == 0) {
        return;
    } else if((pg & 3) == 2) {
        pg_addr = pg & ~3;
        pg = BusReadL(pg_addr);
    }
    pg |= DESP_U;
    wp = wp || (pg & DESP_W);
    // only accepatble access
    if(!(wp && W) && !((pg & DESP_S) && !s)) {
        if(!(pg & DESP_M) && W) {
            pg |= DESP_M;
        }
    }
    BusWriteL(pg_addr, pg);
    auto &e = atcp[addr & 0xf];

    e.laddr = addr;
    e.V = true;
    e.G = pg & DESP_G;

    e.paddr = pg >> 12;
    e.pg_addr = pg_addr;

    e.U = pg >> 8 & 3;
    e.S = pg & DESP_S;
    e.CM = pg >> 5 & 3;
    e.M = pg & DESP_M;
    e.W = wp;
    e.R = true;
}
uint32_t pdt_lookup(uint32_t addr) {
    auto urp = BusReadL(addr);
    if((urp & 3) < 2) {
        return 0;
    }
    urp |= DESP_U;
    BusWriteL(addr, urp);
    return urp;
}
void atc_search(atc_entry *atcp, uint32_t addr, bool s, bool W) {
    auto ap = addr_e(addr, cpu.TCR_P);
    uint32_t ur_addr = (s ? cpu.SRP : cpu.URP) | (ap.ri << 2);
    auto urp = pdt_lookup(ur_addr);
    if(!urp) {
        atc_search_not_found(atcp, addr);
        return;
    }
    uint32_t pt_addr = (urp & ~0x1FF) | (ap.pi << 2);
    auto pt = pdt_lookup(pt_addr);
    if(!pt) {
        atc_search_not_found(atcp, addr);
        return;
    }

    uint32_t pg_addr = (pt & ~(cpu.TCR_P ? 0x7f : 0xFF)) | (ap.pgi << 2);
    atc_set(atcp, addr, pg_addr, W, (urp | pt) & DESP_W, s);
}

mmu_result ptest(uint32_t addr, bool sys, bool code, bool W) {
    mmu_result re;
    auto pret = ptest_ttr(addr, sys, code);
    if(pret.R) {
        if(pret.W && W) {
            re.R = false;
            return re;
        } else {
            return pret;
        }
    }
    if(!cpu.TCR_E) {
        re.R = true;
        re.paddr = addr;
        return re;
    }
    auto atcp = code ? i_atc[sys] : d_atc[sys];
    atc_entry *entry = nullptr;
    // ATC check
    auto p = &atcp[addr & 0xf];
    if(p->V && p->laddr == addr) {
        entry = p;
    } else {
        try {
            atc_search(atcp, addr, sys, W);
            entry = &atcp[addr & 0xf];
        } catch(BusError &) {
            return {.B = true};
        }
    }
    if(entry->R && W && !entry->W && !entry->M && !(entry->S && !sys)) {
        BusWriteL(entry->pg_addr, BusReadL(entry->pg_addr) | DESP_M);
        entry->M = true;
    }
    re.R = entry->R;
    re.W = entry->W;
    re.M = entry->M;
    re.CM = entry->CM;
    re.S = entry->S;
    re.Ux = entry->U;
    re.G = entry->G;
    re.paddr = entry->paddr;
    return re;
}
uint32_t ptest_and_check(uint32_t addr, bool code, bool W) {
    auto ret = ptest(addr >> 12, cpu.S, code, W);
    uint32_t base = ret.paddr << 12;
    if(ret.B) {
		throw PTestError{0};
    }
    if(!ret.R) {
        throw PTestError{SSW_ATC};
    }
    if(ret.W && W) {
        throw PTestError{SSW_ATC};
    }
    if(ret.S && !cpu.S) {
		throw PTestError{SSW_ATC};
    }
    return cpu.TCR_P ? (base & ~1) | (addr & 0x1fff) : base | (addr & 0xfff);
}

static uint32_t page_size() { return cpu.TCR_P ? 0x2000 : 0x1000; }
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
    for(uint32_t b = base; b < base + 0x10; b += 2) {
        jit_tables.erase(b);
    }
}

void cinvp_i(uint32_t base) {
    auto pg_sz = page_size();
    base &= ~(pg_sz - 1);
    for(uint32_t b = base; b < base + pg_sz; b += 2) {
        jit_tables.erase(b);
    }
}

void cinva_i() { jit_tables.clear(); }

void cpushl_i(uint32_t base) {
    base &= ~0xf;
    for(uint32_t b = base; b < base + 0x10; b += 2) {
        jit_tables.erase(b);
    }
}

void cpushp_i(uint32_t base) {
    auto pg_sz = page_size();
    base &= ~(pg_sz - 1);
    for(uint32_t b = base; b < base + pg_sz; b += 2) {
        jit_tables.erase(b);
    }
}

void cpusha_i() { jit_tables.clear(); }

void pflushn_impl(uint32_t addr) {
    if(d_atc[1][addr & 0xf].laddr == addr && !d_atc[1][addr & 0xf].G) {
        d_atc[1][addr & 0xf].V = false;
    }
    if(i_atc[1][addr & 0xf].laddr == addr && !i_atc[1][addr & 0xf].G) {
        i_atc[1][addr & 0xf].V = false;
    }
    if(cpu.DFC == 1 || cpu.DFC == 2) {
        if(d_atc[0][addr & 0xf].laddr == addr && !d_atc[0][addr & 0xf].G) {
            d_atc[0][addr & 0xf].V = false;
        }
        if(i_atc[0][addr & 0xf].laddr == addr && !i_atc[0][addr & 0xf].G) {
            i_atc[0][addr & 0xf].V = false;
        }
    }
}

void pflush_impl(uint32_t addr) {
    if(d_atc[1][addr & 0xf].laddr == addr) {
        d_atc[1][addr & 0xf].V = false;
    }
    if(i_atc[1][addr & 0xf].laddr == addr) {
        i_atc[1][addr & 0xf].V = false;
    }
    if(cpu.DFC == 1 || cpu.DFC == 2) {
        if(d_atc[0][addr & 0xf].laddr == addr) {
            d_atc[0][addr & 0xf].V = false;
        }
        if(i_atc[0][addr & 0xf].laddr == addr) {
            i_atc[0][addr & 0xf].V = false;
        }
    }
}

void pflushan_impl() {
    for(int j = 0; j < 16; ++j) {
        if(!d_atc[1][j].G) {
            d_atc[1][j].V = false;
        }
        if(!i_atc[1][j].G) {
            i_atc[1][j].V = false;
        }
        if(cpu.DFC == 1 || cpu.DFC == 2) {
            if(!d_atc[0][j].G) {
                d_atc[0][j].V = false;
            }
            if(!i_atc[0][j].G) {
                i_atc[0][j].V = false;
            }
        }
    }
}

void pflusha_impl() {
    for(int j = 0; j < 16; ++j) {
        d_atc[1][j].V = false;
        i_atc[1][j].V = false;
        if(cpu.DFC == 1 || cpu.DFC == 2) {
            d_atc[0][j].V = false;
            i_atc[0][j].V = false;
        }
    }
}
namespace OP {
void cinvl_dc(uint16_t op) {
    PRIV_CHECK();
    cinvl_d(cpu.A[REG(op)]);
    TRACE_BRANCH();
}

void cinvp_dc(uint16_t op) {
    PRIV_CHECK();
    cinvp_d(cpu.A[REG(op)]);
    TRACE_BRANCH();
}

void cinva_dc(uint16_t) {
    PRIV_CHECK();
    cinva_d();
    TRACE_BRANCH();
}

void cpushl_dc(uint16_t op) {
    PRIV_CHECK();
    cpushl_d(cpu.A[REG(op)]);
    TRACE_BRANCH();
}

void cpushp_dc(uint16_t op) {
    PRIV_CHECK();
    cpushp_d(cpu.A[REG(op)]);
    TRACE_BRANCH();
}

void cpusha_dc(uint16_t) {
    PRIV_CHECK();
    cpusha_d();
    TRACE_BRANCH();
}

void cinvl_ic(uint16_t op) {
    PRIV_CHECK();
    cinvl_i(cpu.A[REG(op)]);
    TRACE_BRANCH();
}

void cinvp_ic(uint16_t op) {
    PRIV_CHECK();
    cinvp_i(cpu.A[REG(op)]);
    TRACE_BRANCH();
}

void cinva_ic(uint16_t) {
    PRIV_CHECK();
    cinva_i();
    TRACE_BRANCH();
}

void cpushl_ic(uint16_t op) {
    PRIV_CHECK();
    cpushl_i(cpu.A[REG(op)]);
    TRACE_BRANCH();
}

void cpushp_ic(uint16_t op) {
    PRIV_CHECK();
    cpushp_i(cpu.A[REG(op)]);
    TRACE_BRANCH();
}

void cpusha_ic(uint16_t) {
    PRIV_CHECK();
    cpusha_i();
    TRACE_BRANCH();
}

void cinvl_bc(uint16_t op) {
    PRIV_CHECK();
    cinvl_d(cpu.A[REG(op)]);
    cinvl_i(cpu.A[REG(op)]);
    TRACE_BRANCH();
}

void cinvp_bc(uint16_t op) {
    PRIV_CHECK();
    cinvp_d(cpu.A[REG(op)]);
    cinvp_i(cpu.A[REG(op)]);
    TRACE_BRANCH();
}

void cinva_bc(uint16_t) {
    PRIV_CHECK();
    cinva_d();
    cinva_i();
    TRACE_BRANCH();
}

void cpushl_bc(uint16_t op) {
    PRIV_CHECK();
    cpushl_d(cpu.A[REG(op)]);
    cpushl_i(cpu.A[REG(op)]);
    TRACE_BRANCH();
}

void cpushp_bc(uint16_t op) {
    PRIV_CHECK();
    cpushp_d(cpu.A[REG(op)]);
    cpushp_i(cpu.A[REG(op)]);
    TRACE_BRANCH();
}

void cpusha_bc(uint16_t) {
    PRIV_CHECK();
    cpusha_d();
    cpusha_i();
    TRACE_BRANCH();
}

void pflushn(uint16_t op) {
    PRIV_CHECK();
    uint32_t addr = cpu.A[REG(op)] >> 12;
    pflushn_impl(addr);
    TRACE_BRANCH();
}

void pflush(uint16_t op) {
    PRIV_CHECK();
    uint32_t addr = cpu.A[REG(op)] >> 12;
    pflush_impl(addr);
    TRACE_BRANCH();
}

void pflushan(uint16_t) {
    PRIV_CHECK();
    pflushan_impl();
    TRACE_BRANCH();
}

void pflusha(uint16_t) {
    PRIV_CHECK();
    pflusha_impl();
    TRACE_BRANCH();
}

void ptestr(uint16_t op) {
    PRIV_CHECK();
    op_ptest(cpu.A[REG(op)], false);
    TRACE_BRANCH();
}

void ptestw(uint16_t op) {
    PRIV_CHECK();
    op_ptest(cpu.A[REG(op)], true);
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
        d_atc[0][addr & 0xf].V = false;
        cpu.MMUSR = ptest(addr, false, false, w).value();
        break;
    case 2:
        // USER_CODE
        i_atc[0][addr & 0xf].V = false;
        cpu.MMUSR = ptest(addr, false, true, w).value();
        break;
    case 5:
        // SYS_DATA
        d_atc[1][addr & 0xf].V = false;
        cpu.MMUSR = ptest(addr, true, false, w).value();
        break;
    case 6:
        // SYS_CODE
        i_atc[1][addr & 0xf].V = false;
        cpu.MMUSR = ptest(addr, true, true, w).value();
        break;
    }
}
bool movec_from_cr_impl(uint16_t extw, uint32_t *rn) {
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
void movec_from_cr(uint16_t) {
    PRIV_CHECK();
    uint16_t extw = FETCH();
    int rn = extw >> 12 & 15;
    if(!movec_from_cr_impl(extw & 0xfff, &cpu.R(rn))) {
        ILLEGAL_OP();
    }
    TRACE_BRANCH();
}
void movec_to_cr(uint16_t) {
    PRIV_CHECK();
    uint16_t extw = FETCH();
    int rn = extw >> 12 & 15;
    if(!movec_to_cr_impl(extw & 0xfff, cpu.R(rn))) {
        ILLEGAL_OP();
    }

    TRACE_BRANCH();
}
} // namespace OP
