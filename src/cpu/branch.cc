#include "68040.hpp"
#include "bus.hpp"
#include "exception.hpp"
#include "memory.hpp"
#include "inline.hpp"
#include <memory>
#include <utility>
void do_bra(int32_t ix) {
    JUMP(cpu.oldpc + 2 + ix);
    TRACE_BRANCH();
}
void do_bsr(int32_t ix) {
    PUSH32(cpu.PC);
    JUMP(cpu.oldpc + 2 + ix);
    TRACE_BRANCH();
}

void do_bcc(bool cond, int ix) {
    if(cond) {
        JUMP(cpu.oldpc + 2 + ix);
        TRACE_BRANCH();
    }
}
void do_dbcc(bool cond, int reg, int16_t d) {
    if(!cond) {
        int16_t dn = cpu.D[reg];
        STORE_W(cpu.D[reg], --dn);
        if(dn != -1) {
            JUMP(cpu.oldpc + 2 + d);
            TRACE_BRANCH();
        }
    }
}

void do_rtr() {
    SetCCR(POP16());
    JUMP(POP32());
    TRACE_BRANCH();
}

void do_rtd(int16_t disp) {
    JUMP(POP32());
    cpu.A[7] += disp;
    TRACE_BRANCH();
}

void do_rts() {
    JUMP(POP32());
    TRACE_BRANCH();
}