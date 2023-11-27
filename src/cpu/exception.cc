#include "exception.hpp"
#include "68040.hpp"
#include "SDL_events.h"
#include "SDL_log.h"
#include "memory.hpp"
#include "inline.hpp"
#include <fmt/core.h>
void double_fault() {
#ifdef CI
    fmt::print("double buf fault:{:x}", cpu.PC);
#else
    // Double Bus Fault
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "double bus fault:%08x", cpu.PC);
#endif
    exit(1);
}

void EXCEPTION0(int n, uint32_t nextpc, uint16_t sr) {
    PUSH16(n << 2);
    PUSH32(nextpc);
    PUSH16(sr);
    JUMP(ReadL(cpu.VBR + (n << 2)));
}

void EXCEPTION2(int n, uint32_t addr, uint32_t nextpc, uint16_t sr) {
    PUSH32(addr);
    PUSH16(0x2 << 12 | n << 2);
    PUSH32(nextpc);
    PUSH16(sr);
    JUMP(ReadL(cpu.VBR + (n << 2)));
}

void EXCEPTION3(int n, uint16_t sr) {
    PUSH32(cpu.EA);
    PUSH16(0x3 << 12 | n << 2);
    PUSH32(cpu.PC);
    PUSH16(sr);
    JUMP(ReadL(cpu.VBR + (n << 2)));
}

void handle_exception(EXCAPTION_NUMBER n) {
    if(cpu.in_exception) {
        double_fault();
    }
    cpu.in_exception = true;
    uint16_t sr = GetSR();
    cpu.S = true;
    LoadSP();
    cpu.T = 0;
    switch(n) {
    case EXCAPTION_NUMBER::NO_ERR:
    case EXCAPTION_NUMBER::RESET:
        break;
    case EXCAPTION_NUMBER::AFAULT:
        cpu.A[7] -= 4 * 9;
        PUSH32(cpu.ex_addr);
        cpu.A[7] -= 2 * 3;
        if(cpu.must_trace) {
            cpu.fault_SSW |= SSW_CT;
        }
        // SSW
        PUSH16(cpu.fault_SSW);
        if(cpu.fault_SSW & (SSW_CP | SSW_CU | SSW_CT | SSW_CM)) {
            PUSH32(cpu.EA);
        } else {
            PUSH32(0);
        }
        PUSH16(0x7 << 12 | 2 << 2);
        PUSH32(cpu.oldpc);
        PUSH16(sr);
        JUMP(ReadL(cpu.VBR + (2 << 2)));

        // reset mid flag
        cpu.fault_SSW = 0;

        break;
    case EXCAPTION_NUMBER::ADDR_ERR:
        EXCEPTION2(3, cpu.ex_addr& ~1, cpu.oldpc, sr);
        break;
    case EXCAPTION_NUMBER::ILLEGAL_OP:  
    case EXCAPTION_NUMBER::PRIV_ERR:  
    case EXCAPTION_NUMBER::ALINE:
    case EXCAPTION_NUMBER::FLINE: 
    case EXCAPTION_NUMBER::FORMAT_ERR:
        EXCEPTION0(int(n), cpu.oldpc, sr);
        break;
    case EXCAPTION_NUMBER::TRAP0: // TRAP#x
    case EXCAPTION_NUMBER::TRAP1:
    case EXCAPTION_NUMBER::TRAP2:
    case EXCAPTION_NUMBER::TRAP3:
    case EXCAPTION_NUMBER::TRAP4:
    case EXCAPTION_NUMBER::TRAP5:
    case EXCAPTION_NUMBER::TRAP6:
    case EXCAPTION_NUMBER::TRAP7:
    case EXCAPTION_NUMBER::TRAP8:
    case EXCAPTION_NUMBER::TRAP9:
    case EXCAPTION_NUMBER::TRAP10:
    case EXCAPTION_NUMBER::TRAP11:
    case EXCAPTION_NUMBER::TRAP12:
    case EXCAPTION_NUMBER::TRAP13:
    case EXCAPTION_NUMBER::TRAP14:
    case EXCAPTION_NUMBER::TRAP15:
        EXCEPTION0(int(n), cpu.PC, sr);
        break;
    case EXCAPTION_NUMBER::DIV0: 
    case EXCAPTION_NUMBER::CHK_FAIL: 
    case EXCAPTION_NUMBER::TRAPx: 
    case EXCAPTION_NUMBER::TRACE:
        EXCEPTION2(int(n), cpu.oldpc, cpu.PC, sr);
        break;
    case EXCAPTION_NUMBER::FP_UNORDER:
    case EXCAPTION_NUMBER::FP_INEX: 
    case EXCAPTION_NUMBER::FP_DIV0:
    case EXCAPTION_NUMBER::FP_UNFL:
    case EXCAPTION_NUMBER::FP_OPERR:
    case EXCAPTION_NUMBER::FP_OVFL: 
    case EXCAPTION_NUMBER::FP_SNAN:
    case EXCAPTION_NUMBER::FP_UNIMPL_TYPE: 
        EXCEPTION3(int(n), cpu.oldpc);
        break;
    case EXCAPTION_NUMBER::UNDEF_IRQ: 
    case EXCAPTION_NUMBER::UNKNOWN_IRQ: 
    case EXCAPTION_NUMBER::IRQ_LV1:
    case EXCAPTION_NUMBER::IRQ_LV2:
    case EXCAPTION_NUMBER::IRQ_LV3:
    case EXCAPTION_NUMBER::IRQ_LV4:
    case EXCAPTION_NUMBER::IRQ_LV5:
    case EXCAPTION_NUMBER::IRQ_LV6:
    case EXCAPTION_NUMBER::IRQ_LV7:
        cpu.I = int(n) - 24;
        PUSH16(int(n) << 2);
        PUSH32(cpu.PC);
        PUSH16(sr);
        if(cpu.M) {
            cpu.M = false;
            LoadSP();
            PUSH32(0x01 << 12 | (int(n) << 2));
            PUSH32(cpu.PC);
            PUSH16(sr);
        }
        JUMP(ReadL(cpu.VBR + (int(n) << 2)));
        break;
    }
}
[[noreturn]] static inline void RAISE(EXCAPTION_NUMBER n) {
    cpu.ex_n = n;
    longjmp(cpu.ex, 1);
}
void ACCESS_FAULT(uint32_t a, SIZ sz, bool rw,  TM m) {
    cpu.ex_addr = a;
    if( cpu.movem_run) {
        cpu.fault_SSW |= SSW_CM;
    }
    cpu.fault_SSW |= cpu.bus_lock << 9;
    cpu.fault_SSW |= rw << 8;
    cpu.fault_SSW = (cpu.fault_SSW &~ (3 << 5)) | int(sz) << 5;
    if( (cpu.fault_SSW & TT_MASK) != TT_ALT) {
        cpu.fault_SSW = (cpu.fault_SSW &~ 7) | int(m);
    }
    RAISE(EXCAPTION_NUMBER::AFAULT);
}
void ADDRESS_ERROR(uint32_t addr) {
    cpu.ex_addr = addr ;
    RAISE(EXCAPTION_NUMBER::ADDR_ERR);
}

void ILLEGAL_OP() { RAISE(EXCAPTION_NUMBER::ILLEGAL_OP); }

void DIV0_ERROR() { RAISE(EXCAPTION_NUMBER::DIV0); }
void CHK_ERROR() { RAISE(EXCAPTION_NUMBER::CHK_FAIL); }

void TRAPX_ERROR() { RAISE(EXCAPTION_NUMBER::TRAPx); }

void PRIV_ERROR() { RAISE(EXCAPTION_NUMBER::PRIV_ERR); }
void TRACE() { RAISE(EXCAPTION_NUMBER::TRACE); }

void ALINE() { RAISE(EXCAPTION_NUMBER::ALINE); }
void FLINE() { RAISE(EXCAPTION_NUMBER::FLINE); }
void FORMAT_ERROR() { RAISE(EXCAPTION_NUMBER::FORMAT_ERR); }
void TRAP_ERROR(int n) { RAISE(EXCAPTION_NUMBER(int(EXCAPTION_NUMBER::TRAP0) + n)); }
void FP_EX_BSUN() { RAISE(EXCAPTION_NUMBER::FP_UNORDER); }
void FP_EX_INEX() { RAISE(EXCAPTION_NUMBER::FP_INEX); }
void FP_EX_DIV0() { RAISE(EXCAPTION_NUMBER::FP_DIV0); }
void FP_EX_UNFL() { RAISE(EXCAPTION_NUMBER::FP_UNFL); }
void FP_EX_OPERR() { RAISE(EXCAPTION_NUMBER::FP_OPERR); }
void FP_EX_OVFL() { RAISE(EXCAPTION_NUMBER::FP_OVFL); }
void FP_EX_SNAN() { RAISE(EXCAPTION_NUMBER::FP_SNAN); }
void FP_EX_UNIMPL_TYPE() { RAISE(EXCAPTION_NUMBER::FP_UNIMPL_TYPE); }
void IRQ(int n) { RAISE(EXCAPTION_NUMBER(int(EXCAPTION_NUMBER::UNKNOWN_IRQ) + n)); }

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
            cpu.EA = ReadL(cpu.A[7] + 0x00);
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
    cpu.in_exception = false;
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
    LoadSP();
    cpu.A[7] = ReadL(0);
    JUMP(ReadL(4));
}
