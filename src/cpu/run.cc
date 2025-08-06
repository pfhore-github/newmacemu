#include "68040.hpp"
#include "exception.hpp"
#include "inline.hpp"
#include "memory.hpp"
#include <format>
#include <unordered_set>
#include <utility>
#include <thread>
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
std::unordered_set<uint32_t> bks;
std::unordered_set<uint32_t> rom_stops;
void stop_cpu(const std::string &stop);
void run_op();
int get_signum(EXCEPTION_NUMBER con);
bool DEBUG;
int run_cpu() {
    for(;;) {
        if(setjmp(ex_buf) == 0) {
            for(;;) {
                while(cpu.sleeping.load()) {
                    cpu.sleeping.wait(true);
                }
                if(rom_stops.contains(cpu.PC)) {
                    cpu.sleeping.store(true);
                    cpu.sleeping.notify_one();
                    continue;
                }
                if(DEBUG && bks.contains(cpu.PC)) {
                    stop_cpu("T05hwbreak:;");
                    continue;
                }
                run_op();
            }
        } else {
            handle_exception(ex_n);
            cpu.bus_lock = false;
            if(DEBUG) {
                cpu.PC = cpu.oldpc;
                stop_cpu(std::format("S{:02x}", get_signum(ex_n)));
            }
        }
    }
}
void run_op() {

    if(rom_funcs.contains(cpu.PC & 0xfffff)) {
        std::thread rom_th{ []{ rom_funcs[cpu.PC & 0xfffff](); } };
        rom_th.detach();
        cpu.sleeping.store(true);
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
