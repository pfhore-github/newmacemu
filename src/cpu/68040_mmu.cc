#include "68040.hpp"
#include "exception.hpp"
#include "inline.hpp"
#include "memory.hpp"
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
std::function<void()> from_cc(int reg, int cc) {
    switch(cc) {
    case 0:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.R(reg) = cpu.SFC;
        };
    case 1:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.R(reg) = cpu.DFC;
        };
    case 2:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.R(reg) = cpu.CACR_DE << 31 | cpu.CACR_IE << 15;
        };
    case 3:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.R(reg) = cpu.TCR_E << 15 | cpu.TCR_P << 14;
        };
    case 4:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.R(reg) = Get_TTR_t(cpu.ITTR[0]);
        };
    case 5:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.R(reg) = Get_TTR_t(cpu.ITTR[1]);
        };
    case 6:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.R(reg) = Get_TTR_t(cpu.DTTR[0]);
        };
    case 7:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.R(reg) = Get_TTR_t(cpu.DTTR[1]);
        };
    case 0x800:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.R(reg) = cpu.USP;
        };
    case 0x801:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.R(reg) = cpu.VBR;
        };
    case 0x803:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.R(reg) = cpu.MSP;
        };
    case 0x804:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.R(reg) = cpu.ISP;
        };
    case 0x805:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.R(reg) = cpu.MMUSR.paddr << 12 | cpu.MMUSR.B << 11 |
                         cpu.MMUSR.G << 10 | cpu.MMUSR.U << 8 |
                         cpu.MMUSR.S << 7 | cpu.MMUSR.CM << 5 |
                         cpu.MMUSR.M << 4 | cpu.MMUSR.W << 2 |
                         cpu.MMUSR.T << 1 | cpu.MMUSR.R;
        };
    case 0x806:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.R(reg) = cpu.URP;
        };
    case 0x807:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.R(reg) = cpu.SRP;
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
        };
    case 1:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.DFC = cpu.R(reg) & 7;
        };
    case 2:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.CACR_DE = cpu.R(reg) >> 31 & 1;
            cpu.CACR_IE = cpu.R(reg) >> 15 & 1;
        };
    case 3:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.TCR_E = cpu.R(reg) >> 15 & 1;
            cpu.TCR_P = cpu.R(reg) >> 14 & 1;
        };
    case 4:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            Set_TTR_t(cpu.ITTR[0], cpu.R(reg));
        };
    case 5:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            Set_TTR_t(cpu.ITTR[1], cpu.R(reg));
        };
    case 6:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            Set_TTR_t(cpu.DTTR[0], cpu.R(reg));
        };
    case 7:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            Set_TTR_t(cpu.DTTR[1], cpu.R(reg));
        };
    case 0x800:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.USP = cpu.R(reg);
        };
    case 0x801:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.VBR = cpu.R(reg);
        };
    case 0x803:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.MSP = cpu.R(reg);
        };
    case 0x804:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.ISP = cpu.R(reg);
        };
    case 0x805:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.MMUSR.paddr = cpu.R(reg) >> 12;
            cpu.MMUSR.B = cpu.R(reg) >> 11 & 1;
            cpu.MMUSR.G = cpu.R(reg) >> 10 & 1;
            cpu.MMUSR.U = cpu.R(reg) >> 8 & 3;
            cpu.MMUSR.S = cpu.R(reg) >> 7 & 1;
            cpu.MMUSR.CM = cpu.R(reg) >> 5 & 3;
            cpu.MMUSR.M = cpu.R(reg) >> 4 & 1;
            cpu.MMUSR.W = cpu.R(reg) >> 2 & 1;
            cpu.MMUSR.T = cpu.R(reg) >> 1 & 1;
            cpu.MMUSR.R = cpu.R(reg) & 1;
        };
    case 0x806:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.URP = cpu.R(reg);
        };
    case 0x807:
        return [reg]() {
            if(!cpu.S) {
                PRIV_ERROR();
            }
            cpu.SRP = cpu.R(reg);
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
void ptestr(uint32_t an);
void ptestw(uint32_t an);
std::function<void()> decode_mmu(int sz, int type, int reg) {
    switch(sz) {
    case 1:
        // CINVL DC, *
        return []() {
                    if(!cpu.S) {
                        PRIV_ERROR();
                    }
                    // date cache is not implemented
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
                    };
        case 3:
        case 7:
            // CINVPA
            return [reg]() {
                        if(!cpu.S) {
                            PRIV_ERROR();
                        }
                        cpu.icache.clear();
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
                    };
        case 1:
            return [reg]() {
                        if(!cpu.S) {
                            PRIV_ERROR();
                        }
                        pflush(cpu.A[reg]);
                    };
        case 2:
            return []() {
                        if(!cpu.S) {
                            PRIV_ERROR();
                        }
                        pflushan();
                    };
        case 3:
            return []() {
                        if(!cpu.S) {
                            PRIV_ERROR();
                        }
                        pflusha();
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
                        ptestr(cpu.A[reg]);
                    };
        case 5:
            return [reg]() {
                        if(!cpu.S) {
                            PRIV_ERROR();
                        }
                        ptestw(cpu.A[reg]);
                    };
        }
    }
    throw DecodeError{};
}

void pflushn(uint32_t an) {}
void pflush(uint32_t an) {}
void pflushan() {}
void pflusha() {}
void ptestr(uint32_t an) {}
void ptestw(uint32_t an) {}