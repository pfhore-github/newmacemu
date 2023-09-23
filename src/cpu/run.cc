#include "68040.hpp"
#include "exception.hpp"
#include "memory.hpp"
#include "proto.hpp"
#include <utility>

extern run_t run_table[0x10000];
std::unordered_map<uint32_t, void (*)()> rom_funcs;
void IRQ(int i);


void run_op() {
    // check IRQ
    if(int i = cpu.inturrupt.load(); i == 7 || i > cpu.I) {
        IRQ(i);
        cpu.inturrupt.store(-1);
        return;
    }
    cpu.EA = 0;
    cpu.must_trace = cpu.T == 2;
    cpu.oldpc = cpu.PC;
    uint16_t op = FETCH();
    
    if(rom_funcs.contains(cpu.PC & 0xfffff)) {
        rom_funcs[cpu.PC & 0xfffff]();
    } else {
        if(auto p = run_table[op]) {
            (*p)(op, op >> 9 & 7, op >> 3 & 7, op & 7);
        } else {
            ILLEGAL_OP();
        }
    }
    if(std::exchange(cpu.must_trace, false)) {
        TRACE();
    }
}