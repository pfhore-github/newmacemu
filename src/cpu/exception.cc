#include "exception.hpp"
#include "68040.hpp"
#include "inline.hpp"
#include "memory.hpp"
void do_exception() {
    if(cpu.in_exception) {
        // Double Bus Fault
        exit(1);
    }
    cpu.in_exception = true;
    uint16_t sr = GetSR();
    cpu.S = true;
    cpu.T = 0;
    uint32_t contpc = cpu.PC;
    int type = 0;
    switch(cpu.ex_n) {
    case 2: // Access Fault
        cpu.A[7] += 4 * 9;
        PUSH32(cpu.af_value.addr);
        cpu.A[7] += 2 * 3;
        // SSW
        PUSH16(cpu.af_value.CP << 15 | cpu.af_value.CU << 14 |
               cpu.af_value.CT << 13 | cpu.af_value.CM << 12 |
               cpu.af_value.MA << 11 | cpu.af_value.ATC << 10 |
               cpu.af_value.LK << 9 | cpu.af_value.RW << 8 |
               int(cpu.af_value.size) << 5 | int(cpu.af_value.tt) << 3 |
               int(cpu.af_value.tm));
        PUSH32(cpu.af_value.ea);
        type = 7;
        break;
    case 3:
        // ADDRESS ERROR
        PUSH32(cpu.PC - 1);
        type = 2;
        break;
    case 4:
    // ILLEGAL OP
    case 8:
    // PRIV ERROR
    case 10:
    // ALINE EX
    case 11:
    // FLINE EX
    case 14:
        // FORMAT ERROR
        type = 0;
        break;
    case 5:
    // DIV0
    case 6:
    // CHK/CHK2
    case 7:
    // FTAPcc/TRAPcc/TRAPv
    case 9:
        // TRACE
        PUSH32(cpu.PC);
        contpc = cpu.nextpc;
        type = 2;
        break;
    case 32:
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
    // TRAP #N
    case 15:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
        // IRQ
        contpc = cpu.nextpc;
        type = 0;
        break;
    }
    PUSH16(type << 12 | (cpu.ex_n << 2));
    PUSH32(contpc);
    PUSH16(sr);
    JUMP(MMU_ReadL(cpu.VBR + (cpu.ex_n << 2)));
}
void RTE() {
    uint16_t sr = POP16();
    uint32_t pc = POP32();
    uint16_t vec = POP16();
    switch(vec >> 12) {
    case 0:
    case 1:
        break;
    case 2:
    case 3:
        cpu.A[7] += 4;
        break;
    case 7:
        cpu.A[7] += 52;
        break;
    default:
        CPU_RAISE(14);
    }
    SetSR(sr);
    JUMP(pc);
}