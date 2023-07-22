#include "exception.hpp"
#include "68040.hpp"
#include "memory.hpp"
#include "proto.hpp"
static uint16_t exception_entry() {
    if(cpu.in_exception) {
        // Double Bus Fault
        exit(1);
    }
    cpu.in_exception = true;
    uint16_t sr = GetSR();
    cpu.S = true;
    LoadSP();
    cpu.T = 0;
    return sr;
}

[[noreturn]] static void exception0(int n, uint32_t contpc) {
    uint16_t sr = exception_entry();
    PUSH16(n << 2);
    PUSH32(contpc);
    PUSH16(sr);
    JUMP(ReadL(cpu.VBR + (n << 2)));
    longjmp(cpu.ex_buf, 1);
}

[[noreturn]] static void exception2(int n, uint32_t contpc, uint32_t addr) {
    uint16_t sr = exception_entry();
    PUSH32(addr);
    PUSH16(0x2 << 12 | n << 2);
    PUSH32(contpc);
    PUSH16(sr);
    JUMP(ReadL(cpu.VBR + (n << 2)));
    longjmp(cpu.ex_buf, 1);
}

[[noreturn]] static void exception3(int n) {
    uint16_t sr = exception_entry();
    PUSH32(cpu.EA);
    PUSH16(0x3 << 12 | n << 2);
    PUSH32(cpu.PC);
    PUSH16(sr);
    JUMP(ReadL(cpu.VBR + (n << 2)));
    longjmp(cpu.ex_buf, 1);
}

void ACCESS_FAULT() {
    uint16_t sr = exception_entry();
    cpu.A[7] += 4 * 9;
    PUSH32(cpu.af_value.addr);
    cpu.A[7] += 2 * 3;
    if(cpu.must_trace) {
        cpu.af_value.CT = true;
    }
    // SSW
    PUSH16(cpu.af_value.CP << 15 | cpu.af_value.CU << 14 |
           cpu.af_value.CT << 13 | cpu.af_value.CM << 12 |
           cpu.af_value.MA << 11 | cpu.af_value.ATC << 10 |
           cpu.af_value.LK << 9 | cpu.af_value.RW << 8 |
           int(cpu.af_value.size) << 5 | int(cpu.af_value.tt) << 3 |
           int(cpu.af_value.tm));
    PUSH32(cpu.af_value.ea);
    PUSH16(0x7 << 12 | 2 << 2);
    PUSH32(cpu.PC);
    PUSH16(sr);
    JUMP(ReadL(cpu.VBR + (2 << 2)));
    longjmp(cpu.ex_buf, 1);
}

void ADDRESS_ERROR(uint32_t next) { exception2(3, cpu.oldpc, next & ~1); }
void ILLEGAL_OP() { exception0(4, cpu.oldpc); }

void DIV0_ERROR() { exception2(5, cpu.PC, cpu.oldpc); }
void CHK_ERROR() { exception2(6, cpu.PC, cpu.oldpc); }

void TRAPX_ERROR() { exception2(7, cpu.PC, cpu.oldpc); }

void PRIV_ERROR() { exception0(8, cpu.oldpc); }

void TRACE() { exception2(9, cpu.PC, cpu.oldpc); }

void ALINE() { exception0(10, cpu.oldpc); }

void FLINE() { exception0(11, cpu.oldpc); }

void FORMAT_ERROR() { exception0(14, cpu.oldpc); }

void TRAP_ERROR(int n) { exception0(32 + n, cpu.PC); }

void FP_EX_BSUN() { exception3(48); }

void FP_EX_INEX() { exception3(49); }

void FP_EX_DIV0() { exception3(50); }

void FP_EX_UNFL() { exception3(51); }
void FP_EX_OPERR() { exception3(52); }
void FP_EX_OVFL() { exception3(53); }
void FP_EX_SNAN() { exception3(54); }

void IRQ(int n) {
    if(n != 7 && n <= cpu.I) {
        return;
    }
    cpu.I = n;
    n += 24;
    if(cpu.in_exception) {
        // Double Bus Fault
        exit(1);
    }
    cpu.in_exception = true;
    uint16_t sr = GetSR();
    cpu.S = true;
    LoadSP();
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