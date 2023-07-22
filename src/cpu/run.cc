#include "68040.hpp"
#include "exception.hpp"
#include "memory.hpp"
#include "proto.hpp"
#include <utility>

std::unique_ptr<Opcode> decode();
extern run_t run_table[0x10000];
void run_op() {
    cpu.af_value.ea = 0;
    cpu.must_trace = cpu.T == 2;
    cpu.oldpc = cpu.PC;
    uint16_t op = FETCH();
    if(setjmp(cpu.ex_buf) == 0) {
        if(auto p = run_table[op]) {
            (*p)(op, op >> 9 & 7, op >> 3 & 7, op & 7);
        } else {
            ILLEGAL_OP();
        }
        if(std::exchange(cpu.must_trace, false)) {
            TRACE();
        }
    }
}