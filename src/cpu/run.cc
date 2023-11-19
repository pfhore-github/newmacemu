#include "68040.hpp"
#include "exception.hpp"
#include "memory.hpp"
#include "inline.hpp"
#include <utility>

extern run_t run_table[0x10000];
std::unordered_map<uint32_t, void (*)()> rom_funcs;
void op_prologue(uint32_t pc) {
    // check IRQ
     if(int i = cpu.inturrupt.load(); i == 7 || i > cpu.I) {
        cpu.inturrupt.store(-1);
        IRQ(i);
    }
    cpu.EA = 0;
    cpu.must_trace = cpu.T == 2;
    cpu.PC = cpu.oldpc = pc;
}

void run_op() {
    op_prologue(cpu.PC);
    uint16_t op = FETCH();
    
    if(rom_funcs.contains(cpu.PC & 0xfffff)) {
        rom_funcs[cpu.PC & 0xfffff]();
    } else {
        if(auto p = run_table[op]) {
            (*p)(op);
        } else {
            ILLEGAL_OP();
        }
    }
    if(std::exchange(cpu.must_trace, false)) {
        TRACE();
    }
}
