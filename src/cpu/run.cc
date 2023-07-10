#include "68040.hpp"
#include "exception.hpp"
#include "memory.hpp"
#include "proto.hpp"
#include <utility>

std::pair<std::function<void()>, int> decode();
void run_op() {
    cpu.af_value.ea = 0;
    cpu.must_trace = cpu.T == 2;
    auto o = cpu.icache.find(cpu.PC);
    if(o == cpu.icache.end()) {
        auto next = decode();
        std::tie(o, std::ignore) = cpu.icache.emplace(cpu.PC, next);
    }
    cpu.nextpc = cpu.PC + 2 + o->second.second;
    if(setjmp(cpu.ex_buf) == 0) {
        o->second.first();
        if(std::exchange(cpu.must_trace, false)) {
            TRACE();
        }   
    }
    cpu.PC = cpu.nextpc;
}