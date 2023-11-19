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

void handle_exception(int n) {
    if(cpu.in_exception) {
        double_fault();
    }
    cpu.in_exception = true;
    uint16_t sr = GetSR();
    cpu.S = true;
    LoadSP();
    cpu.T = 0;
    switch(n) {
    case 0:
    case 1:
        break;
    case 2:
        // ACCESS FAULT
        cpu.A[7] -= 4 * 9;
        PUSH32(cpu.ex_addr);
        cpu.A[7] -= 2 * 3;
        if(cpu.must_trace) {
            cpu.faultParam->CT = true;
        }
        // SSW
        PUSH16(cpu.faultParam->CP << 15 | cpu.faultParam->CU << 14 |
               cpu.faultParam->CT << 13 | cpu.faultParam->CM << 12 |
               cpu.faultParam->MA << 11 | cpu.faultParam->ATC << 10 |
               cpu.faultParam->LK << 9 | cpu.faultParam->RW << 8 |
               int(cpu.faultParam->size) << 5 | int(cpu.faultParam->tt) << 3 |
               int(cpu.faultParam->tm));
        if(cpu.faultParam->CP || cpu.faultParam->CU || cpu.faultParam->CT ||
           cpu.faultParam->CM) {
            PUSH32(cpu.EA);
        } else {
            PUSH32(0);
        }
        PUSH16(0x7 << 12 | 2 << 2);
        PUSH32(cpu.oldpc);
        PUSH16(sr);
        JUMP(ReadL(cpu.VBR + (2 << 2)));

        // reset mid flag
		cpu.faultParam.reset();

        break;
    case 3:
        // ADDRESS ERROR
        EXCEPTION2(3, cpu.oldpc, cpu.ex_addr, sr);
        break;
    case 4:  // ILLEGAL OP
    case 8:  // PRIV ERROR
    case 10: // ALINE
    case 11: // FLINE
    case 14: // FORMAT_ERROR
        EXCEPTION0(n, cpu.oldpc, sr);
        break;
    case 32: // TRAP#x
    case 33:
    case 34:
    case 35:
    case 36:
    case 37:
    case 38:
    case 39:
    case 40:
    case 41:
    case 42:
    case 43:
    case 44:
    case 45:
    case 46:
    case 47:
        EXCEPTION0(n, cpu.PC, sr);
        break;
    case 5: // DIV0
    case 6: // CHK
    case 7: // TRAPX
    case 9: // TRACE
        EXCEPTION2(n, cpu.PC, cpu.oldpc, sr);
        break;
    case 48: // FP_BSUN
    case 49: // FP_EX_INEX
    case 50: // FP_DIV0
    case 51: // FP_UNFL
    case 52: // FP_OPERR
    case 53: // FP_OVFL
    case 54: // FP_SNAN
    case 55: // FP_EX_UNIMPL_TYPE
        EXCEPTION3(n, cpu.oldpc);
        break;
    case 24: // IRQ 0
    case 25: // IRQ 1
    case 26: // IRQ 2
    case 27: // IRQ 3
    case 28: // IRQ 4
    case 29: // IRQ 5
    case 30: // IRQ 6
    case 31: // IRQ 7
        cpu.I = n - 24;
        PUSH16(n << 2);
        PUSH32(cpu.PC);
        PUSH16(sr);
        if(cpu.M) {
            cpu.M = false;
            LoadSP();
            PUSH32(0x01 << 12 | (n << 2));
            PUSH32(cpu.PC);
            PUSH16(sr);
        }
        JUMP(ReadL(cpu.VBR + (n << 2)));
        break;
    }
}
[[noreturn]] static inline void RAISE(int n) {
    cpu.ex_n = n;
    longjmp(cpu.ex, 1);
}
void ACCESS_FAULT(uint32_t a, SIZ sz, bool rw,  TM m) {
    cpu.ex_addr = a;
    cpu.faultParam->CM = cpu.movem_run;
    cpu.faultParam->RW = rw;
    cpu.faultParam->size = sz;
    cpu.faultParam->tm = m;
    RAISE(2);
}
void ADDRESS_ERROR(uint32_t addr) {
    cpu.ex_addr = addr & ~1;
    RAISE(3);
}

void ILLEGAL_OP() { RAISE(4); }

void DIV0_ERROR() { RAISE(5); }
void CHK_ERROR() { RAISE(6); }

void TRAPX_ERROR() { RAISE(7); }

void PRIV_ERROR() { RAISE(8); }
void TRACE() { RAISE(9); }

void ALINE() { RAISE(10); }
void FLINE() { RAISE(11); }
void FORMAT_ERROR() { RAISE(14); }
void TRAP_ERROR(int n) { RAISE(32 + n); }
void FP_EX_BSUN() { RAISE(48); }
void FP_EX_INEX() { RAISE(49); }
void FP_EX_DIV0() { RAISE(50); }
void FP_EX_UNFL() { RAISE(51); }
void FP_EX_OPERR() { RAISE(52); }
void FP_EX_OVFL() { RAISE(53); }
void FP_EX_SNAN() { RAISE(54); }
void FP_EX_UNIMPL_TYPE() { RAISE(55); }
void IRQ(int n) { RAISE(24 + n); }

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
