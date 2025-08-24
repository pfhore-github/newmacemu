#include "68040.hpp"
#include "SDL3/SDL_mutex.h"
#include "exception.hpp"
#include "inline.hpp"
#include "memory.hpp"
#include <format>
#include <thread>
#include <unordered_set>
#include <utility>

extern run_t run_table[0x10000];
std::unordered_map<uint32_t, void (*)()> rom_funcs;
void op_prologue(uint32_t pc) {
    // check IRQ
    if(int i = cpu.inturrupt.load(); i == 7 || i > cpu.I) {
        cpu.inturrupt.store(-1);
        IRQ(i);
    }
    if(!cpu.movem_run) [[likely]] {
        cpu.EA = 0;
    }
    cpu.must_trace = cpu.T == 2;
    cpu.PC = cpu.oldpc = pc;
}
std::unordered_set<uint32_t> rom_stops;
void run_op();
int get_signum(EXCEPTION_NUMBER con);
void run_cpu() {
    for(;;) {
        try {
            for(;;) {
                cpu.run.wait();
                if(rom_stops.contains(cpu.PC)) {
                    cpu.run.stop();
                    continue;
                }
                run_op();
            }
        } catch(M68kException& e) {
            handle_exception(e.ex_n);
            cpu.bus_lock = false;
        }
    }
}
void run_op() {

    if(rom_funcs.contains(cpu.PC & 0xfffff)) {
        cpu.run.stop();
        std::thread rom_th{rom_funcs[cpu.PC & 0xfffff]};
        rom_th.detach();
        return;
    } else {
        op_prologue(cpu.PC);
        uint16_t op = FETCH();
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
