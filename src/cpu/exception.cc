#include "exception.hpp"
#include "68040.hpp"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_log.h"
#include "inline.hpp"
#include "memory.hpp"
#include "mmu.hpp"
#include <expected>
#include <print>
[[noreturn]] void double_fault() {
#ifdef CI
    struct TestEnd {};
    std::print("double bus fault:{:x}", cpu.PC);
    throw TestEnd{};
#else
    // Double Bus Fault
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "double bus fault:%08x\n", cpu.PC);
    exit(1);
#endif
}
void WriteWImpl(uint32_t addr, uint16_t w);
uint32_t ReadLImpl(uint32_t addr);

jmp_buf ex_buf;
uint32_t ex_addr;
EXCEPTION_NUMBER ex_n;

inline void ex_PUSH16(uint16_t v) {
	try {
		WriteWImpl(cpu.A[7] -= 2, v);
	} catch(PTestError&) {
        double_fault();
    }
}
inline void ex_PUSH32(uint32_t v) {
    ex_PUSH16(v);
    ex_PUSH16(v >> 16);
}

inline uint32_t ex_READL(uint32_t addr) {
    try {
		return ReadLImpl(addr, false);
	} catch(PTestError& e) {
        double_fault();
    }
}

inline void ex_JUMP(uint32_t addr) {
    if(addr & 1) {
        double_fault();
    }
    cpu.PC = addr;
}
void EXCEPTION0(int n, uint32_t nextpc, uint16_t sr) {
    ex_PUSH16(n << 2);
    ex_PUSH32(nextpc);
    ex_PUSH16(sr);
    ex_JUMP(ex_READL(cpu.VBR + (n << 2)));
}

void EXCEPTION2(int n, uint32_t addr, uint32_t nextpc, uint16_t sr) {
    ex_PUSH32(addr);
    ex_PUSH16(0x2 << 12 | n << 2);
    ex_PUSH32(nextpc);
    ex_PUSH16(sr);
    ex_JUMP(ex_READL(cpu.VBR + (n << 2)));
}

void EXCEPTION3(int n, uint16_t sr) {
    ex_PUSH32(cpu.EA);
    ex_PUSH16(0x3 << 12 | n << 2);
    ex_PUSH32(cpu.PC);
    ex_PUSH16(sr);
    ex_JUMP(ex_READL(cpu.VBR + (n << 2)));
}

void handle_exception(EXCEPTION_NUMBER n) {
    uint16_t sr = GetSR();
    cpu.S = true;
    LoadSP();
    cpu.T = 0;
    switch(n) {
    case EXCEPTION_NUMBER::NO_ERR:
    case EXCEPTION_NUMBER::RESET:
        break;
    case EXCEPTION_NUMBER::AFAULT:
        cpu.A[7] -= 4 * 9;
        ex_PUSH32(ex_addr);
        cpu.A[7] -= 2 * 3;
        if(cpu.must_trace) {
            cpu.fault_SSW |= SSW_CT;
        }
        // SSW
        ex_PUSH16(cpu.fault_SSW);
        if(cpu.fault_SSW & (SSW_CP | SSW_CU | SSW_CT | SSW_CM)) {
            ex_PUSH32(cpu.EA);
        } else {
            ex_PUSH32(0);
        }
        ex_PUSH16(0x7 << 12 | 2 << 2);
        ex_PUSH32(cpu.oldpc);
        ex_PUSH16(sr);
        JUMP(ex_READL(cpu.VBR + (2 << 2)));
        break;
    case EXCEPTION_NUMBER::ADDR_ERR:
        EXCEPTION2(3, ex_addr & ~1, cpu.oldpc, sr);
        break;
    case EXCEPTION_NUMBER::ILLEGAL_OP:
    case EXCEPTION_NUMBER::PRIV_ERR:
    case EXCEPTION_NUMBER::ALINE:
    case EXCEPTION_NUMBER::FLINE:
    case EXCEPTION_NUMBER::FORMAT_ERR:
        EXCEPTION0(int(n), cpu.oldpc, sr);
        break;
    case EXCEPTION_NUMBER::TRAP0: // TRAP#x
    case EXCEPTION_NUMBER::TRAP1:
    case EXCEPTION_NUMBER::TRAP2:
    case EXCEPTION_NUMBER::TRAP3:
    case EXCEPTION_NUMBER::TRAP4:
    case EXCEPTION_NUMBER::TRAP5:
    case EXCEPTION_NUMBER::TRAP6:
    case EXCEPTION_NUMBER::TRAP7:
    case EXCEPTION_NUMBER::TRAP8:
    case EXCEPTION_NUMBER::TRAP9:
    case EXCEPTION_NUMBER::TRAP10:
    case EXCEPTION_NUMBER::TRAP11:
    case EXCEPTION_NUMBER::TRAP12:
    case EXCEPTION_NUMBER::TRAP13:
    case EXCEPTION_NUMBER::TRAP14:
    case EXCEPTION_NUMBER::TRAP15:
        EXCEPTION0(int(n), cpu.PC, sr);
        break;
    case EXCEPTION_NUMBER::DIV0:
    case EXCEPTION_NUMBER::CHK_FAIL:
    case EXCEPTION_NUMBER::TRAPx:
    case EXCEPTION_NUMBER::TRACE:
        EXCEPTION2(int(n), cpu.oldpc, cpu.PC, sr);
        break;
    case EXCEPTION_NUMBER::FP_UNORDER:
    case EXCEPTION_NUMBER::FP_INEX:
    case EXCEPTION_NUMBER::FP_DIV0:
    case EXCEPTION_NUMBER::FP_UNFL:
    case EXCEPTION_NUMBER::FP_OPERR:
    case EXCEPTION_NUMBER::FP_OVFL:
    case EXCEPTION_NUMBER::FP_SNAN:
    case EXCEPTION_NUMBER::FP_UNIMPL_TYPE:
        EXCEPTION3(int(n), cpu.oldpc);
        break;
    case EXCEPTION_NUMBER::UNDEF_IRQ:
    case EXCEPTION_NUMBER::UNKNOWN_IRQ:
    case EXCEPTION_NUMBER::IRQ_LV1:
    case EXCEPTION_NUMBER::IRQ_LV2:
    case EXCEPTION_NUMBER::IRQ_LV3:
    case EXCEPTION_NUMBER::IRQ_LV4:
    case EXCEPTION_NUMBER::IRQ_LV5:
    case EXCEPTION_NUMBER::IRQ_LV6:
    case EXCEPTION_NUMBER::IRQ_LV7:
        cpu.I = int(n) - 24;
        ex_PUSH16(int(n) << 2);
        ex_PUSH32(cpu.PC);
        ex_PUSH16(sr);
        if(cpu.M) {
            cpu.M = false;
            LoadSP();
            ex_PUSH32(0x01 << 12 | (int(n) << 2));
            ex_PUSH32(cpu.PC);
            ex_PUSH16(sr);
        }
        ex_JUMP(ex_READL(cpu.VBR + (int(n) << 2)));
        break;
    }
}
[[noreturn]] static inline void RAISE(EXCEPTION_NUMBER n) {
    ex_n = n;
    longjmp(ex_buf, 1);
}
void ACCESS_FAULT(uint32_t a, SIZ sz, bool rw, TM m, uint16_t tt) {
    ex_addr = a;
    if(cpu.movem_run) {
        cpu.fault_SSW |= SSW_CM;
    }
    cpu.fault_SSW |= tt;
    cpu.fault_SSW |= cpu.bus_lock << 9;
    cpu.fault_SSW |= rw << 8;
    cpu.fault_SSW |= int(sz) << 5;
    cpu.fault_SSW |= int(m);
    RAISE(EXCEPTION_NUMBER::AFAULT);
}
void ADDRESS_ERROR(uint32_t addr) {
    ex_addr = addr;
    RAISE(EXCEPTION_NUMBER::ADDR_ERR);
}

void ILLEGAL_OP() { RAISE(EXCEPTION_NUMBER::ILLEGAL_OP); }

void DIV0_ERROR() { RAISE(EXCEPTION_NUMBER::DIV0); }
void CHK_ERROR() { RAISE(EXCEPTION_NUMBER::CHK_FAIL); }

void TRAPX_ERROR() { RAISE(EXCEPTION_NUMBER::TRAPx); }

void PRIV_ERROR() { RAISE(EXCEPTION_NUMBER::PRIV_ERR); }
void TRACE() { RAISE(EXCEPTION_NUMBER::TRACE); }

void ALINE() { RAISE(EXCEPTION_NUMBER::ALINE); }
void FLINE() { RAISE(EXCEPTION_NUMBER::FLINE); }
void FORMAT_ERROR() { RAISE(EXCEPTION_NUMBER::FORMAT_ERR); }
void TRAP_ERROR(int n) {
    RAISE(EXCEPTION_NUMBER(int(EXCEPTION_NUMBER::TRAP0) + n));
}
void FP_EX_BSUN() { RAISE(EXCEPTION_NUMBER::FP_UNORDER); }
void FP_EX_INEX() { RAISE(EXCEPTION_NUMBER::FP_INEX); }
void FP_EX_DIV0() { RAISE(EXCEPTION_NUMBER::FP_DIV0); }
void FP_EX_UNFL() { RAISE(EXCEPTION_NUMBER::FP_UNFL); }
void FP_EX_OPERR() { RAISE(EXCEPTION_NUMBER::FP_OPERR); }
void FP_EX_OVFL() { RAISE(EXCEPTION_NUMBER::FP_OVFL); }
void FP_EX_SNAN() { RAISE(EXCEPTION_NUMBER::FP_SNAN); }
void FP_EX_UNIMPL_TYPE() { RAISE(EXCEPTION_NUMBER::FP_UNIMPL_TYPE); }
void IRQ(int n) {
    RAISE(EXCEPTION_NUMBER(int(EXCEPTION_NUMBER::UNKNOWN_IRQ) + n));
}

void do_rte() {
    uint16_t sr = POP16();
    uint32_t pc = POP32();
    uint16_t vec = POP16();
    switch(vec >> 12) {
    case 0:
        break;
    case 1:
        SetSR(sr);
        return do_rte();
    case 2:
    case 3:
        cpu.A[7] += 4;
        break;
    case 7: {
        // AF
        uint16_t ssw = ReadW(cpu.A[7] + 0x04);
        if(ssw & 1 << 15) {
            // CP restart
        }
        if(ssw & 1 << 14) {
            // CU restart
        }
        if(ssw & 1 << 13) {
            // CT restart
            cpu.A[7] += 52;
            // restart trace
            SetSR(sr);
            cpu.must_trace = false;
            cpu.PC = pc;
            TRACE();
        }
        if(ssw & 1 << 12) {
            // CM restart
            cpu.EA = ex_READL(cpu.A[7] + 0x00);
            // restart MOVEM
            cpu.movem_run = true;
        }
        cpu.A[7] += 52;
        break;
    }
    default:
        FORMAT_ERROR();
    }
    SetSR(sr);
    if(cpu.T == 1) {
        cpu.must_trace = true;
    }
    JUMP(pc);
}

void RESET() {
    cpu.S = true;
    cpu.M = false;
    cpu.T = 0;
    cpu.I = 7;
    cpu.VBR = 0;
    cpu.CACR_DE = cpu.CACR_IE = false;
    cpu.DTTR[0].E = 0;
    cpu.DTTR[1].E = 0;
    cpu.ITTR[0].E = 0;
    cpu.ITTR[1].E = 0;
    for(int i = 0; i < 2; ++i) {
        for(int k = 0; k < 16; ++k) {
            i_atc[i][k].V = false;
            d_atc[i][k].V = false;
        }
    }
    LoadSP();
    cpu.A[7] = ex_READL(0);
    JUMP(ex_READL(4));
}
