#include "exception.hpp"
#include "jit_common.hpp"
#include "memory.hpp"
#include <memory>
#include <unordered_map>
#include <utility>
#include <setjmp.h>
using namespace asmjit;
extern CpuFeatures feature;
void jit_postop();
extern volatile bool testing;
extern std::unordered_map<uint32_t, Label> jumpMap;
using jit_op = void (*)(uint16_t op);
extern jmp_buf ex_buf;
extern jit_op jit_compile_op[0x10000];
void cinvl_d(uint32_t base);
void cinvp_d(uint32_t base);
void cinva_d() ;
void cpushl_d(uint32_t base) ;
void cpushp_d(uint32_t base) ;
void cpusha_d() ;

void cinvl_i(uint32_t base);
void cinvp_i(uint32_t base) ;
void cinva_i();
void cpushl_i(uint32_t base) ;
void cpushp_i(uint32_t base);
void cpusha_i();
void pflushn_impl(uint32_t addr);
void pflush_impl(uint32_t addr);
void pflushan_impl();
void pflusha_impl();
void op_ptest(uint32_t addr, bool write);
namespace JIT_OP {
extern Label end_lbl;
void jumpC(uint32_t t);
void jit_trace_branch();
void jit_priv_check();

#ifdef __x86_64__
void stop_jit() {
    cpu.inJit = false;
    longjmp(ex_buf, 2);
}
void cinvl_dc(uint16_t op) {
    jit_priv_check();
    as->mov(ARG1.r32(), AR_L(REG(op)));
    as->call(cinvl_d);
    jit_trace_branch();
}
void cinvp_dc(uint16_t op) {
    jit_priv_check();
    as->mov(ARG1.r32(), AR_L(REG(op)));
    as->call(cinvp_d);
    jit_trace_branch();
}
void cinva_dc(uint16_t) {
    jit_priv_check();
    as->call(cinva_d);
    jit_trace_branch();
}
void cpushl_dc(uint16_t op) {
    jit_priv_check();
    as->mov(ARG1.r32(), AR_L(REG(op)));
    as->call(cpushl_d);
    jit_trace_branch();
}
void cpushp_dc(uint16_t op) {
    jit_priv_check();
    as->mov(ARG1.r32(), AR_L(REG(op)));
    as->call(cpushp_d);
    jit_trace_branch();
}
void cpusha_dc(uint16_t) {
    jit_priv_check();
    as->call(cpusha_d);
    jit_trace_branch();
}

void cinvl_ic(uint16_t op) {
    jit_priv_check();
    as->mov(ARG1.r32(), AR_L(REG(op)));
    as->call(cinvl_i);
    jit_trace_branch();
    jit_postop();
    as->mov(c_pc, cpu.PC);
    as->call(stop_jit);	
}
void cinvp_ic(uint16_t op) {
    jit_priv_check();
    as->mov(ARG1.r32(), AR_L(REG(op)));
    as->call(cinvp_i);
    jit_trace_branch();
    jit_postop();
    as->mov(c_pc, cpu.PC);
    as->call(stop_jit);	
}
void cinva_ic(uint16_t) {
    jit_priv_check();
    as->call(cinva_i);
    jit_trace_branch();
    jit_postop();
    as->mov(c_pc, cpu.PC);
    as->call(stop_jit);	
}
void cpushl_ic(uint16_t op) {
    jit_priv_check();
    as->mov(ARG1.r32(), AR_L(REG(op)));
    as->call(cpushl_i);
    jit_trace_branch();
    jit_postop();
    as->mov(c_pc, cpu.PC);
    as->call(stop_jit);	
}
void cpushp_ic(uint16_t op) {
    jit_priv_check();
    as->mov(ARG1.r32(), AR_L(REG(op)));
    as->call(cpushp_i);
    jit_trace_branch();
    jit_postop();
    as->mov(c_pc, cpu.PC);
    as->call(stop_jit);	
}
void cpusha_ic(uint16_t) {
    jit_priv_check();
    as->call(cpusha_i);
    jit_trace_branch();
    jit_postop();
    as->mov(c_pc, cpu.PC);
    as->call(stop_jit);	
}

void cinvl_bc(uint16_t op) {
    jit_priv_check();
	as->mov(x86::r13d, AR_L(REG(op)));
    as->mov(ARG1.r32(), x86::r13d);
    as->call(cinvl_d);
    as->mov(ARG1.r32(), x86::r13d);
    as->call(cinvl_i);
    jit_trace_branch();
    jit_postop();
    as->mov(c_pc, cpu.PC);
    as->call(stop_jit);		
}
void cinvp_bc(uint16_t op) {
    jit_priv_check();
	as->mov(x86::r13d, AR_L(REG(op)));
    as->mov(ARG1.r32(), x86::r13d);
    as->call(cinvl_d);
    as->mov(ARG1.r32(), x86::r13d);
    as->call(cinvp_i);
    jit_trace_branch();
    jit_postop();
    as->mov(c_pc, cpu.PC);
    as->call(stop_jit);	
}
void cinva_bc(uint16_t) {
    jit_priv_check();
    as->call(cinva_d);
    as->call(cinva_i);
    jit_trace_branch();
    jit_postop();
    as->mov(c_pc, cpu.PC);
    as->call(stop_jit);	
}
void cpushl_bc(uint16_t op) {
    jit_priv_check();
	as->mov(x86::r13d, AR_L(REG(op)));
    as->mov(ARG1.r32(), x86::r13d);
    as->call(cinvl_d);
    as->mov(ARG1.r32(), x86::r13d);
    as->call(cpushl_i);
    jit_trace_branch();
    jit_postop();
    as->mov(c_pc, cpu.PC);
    as->call(stop_jit);	
}
void cpushp_bc(uint16_t op) {
    jit_priv_check();
	as->mov(x86::r13d, AR_L(REG(op)));
    as->mov(ARG1.r32(), x86::r13d);
    as->call(cinvl_d);
    as->mov(ARG1.r32(), x86::r13d);
    as->call(cpushp_i);
    jit_trace_branch();
    jit_postop();
    as->mov(c_pc, cpu.PC);
    as->call(stop_jit);	
}
void cpusha_bc(uint16_t) {
    jit_priv_check();
    as->call(cpusha_d);
    as->call(cpusha_i);
    jit_trace_branch();
    jit_postop();
    as->mov(c_pc, cpu.PC);
    as->call(stop_jit);	
}

void pflushn(uint16_t op) {
    jit_priv_check();
	as->mov(ARG1.r32(), AR_L(REG(op)));
	as->shr(ARG1.r32(), 12);
    as->call(pflushn_impl);
    jit_trace_branch();
}

void pflush(uint16_t op) {
    jit_priv_check();
	as->mov(ARG1.r32(), AR_L(REG(op)));
	as->shr(ARG1.r32(), 12);
    as->call(pflush_impl);
    jit_trace_branch();
}

void pflushan(uint16_t ) {
    jit_priv_check();
    as->call(pflushan_impl);
    jit_trace_branch();
}

void pflusha(uint16_t) {
    jit_priv_check();
    as->call(pflusha_impl);
    jit_trace_branch();
}

void ptestr(uint16_t op) {
    jit_priv_check();
	as->mov(ARG1.r32(), AR_L(REG(op)));
	as->mov(ARG2.r8(), 0);
	as->call(op_ptest);
    jit_trace_branch();	
}

void ptestw(uint16_t op) {
    jit_priv_check();
	as->mov(ARG1.r32(), AR_L(REG(op)));
	as->mov(ARG2.r8(), 1);
	as->call(op_ptest);
    jit_trace_branch();	
}

#endif
} // namespace JIT_OP
void init_jit_table_mmu() {
    for(int r = 0; r < 8; ++r) {
        jit_compile_op[0172110 | r] = JIT_OP::cinvl_dc;
        jit_compile_op[0172120 | r] = JIT_OP::cinvp_dc;
        jit_compile_op[0172130 | r] = JIT_OP::cinva_dc;
        jit_compile_op[0172150 | r] = JIT_OP::cpushl_dc;
        jit_compile_op[0172160 | r] = JIT_OP::cpushp_dc;
        jit_compile_op[0172170 | r] = JIT_OP::cpusha_dc;

        // Cache reset is broken in JIT
        jit_compile_op[0172210 | r] = JIT_OP::cinvl_ic;
        jit_compile_op[0172220 | r] = JIT_OP::cinvp_ic;
        jit_compile_op[0172230 | r] = JIT_OP::cinva_ic;
        jit_compile_op[0172250 | r] = JIT_OP::cpushl_ic;
        jit_compile_op[0172260 | r] = JIT_OP::cpushp_ic;
        jit_compile_op[0172270 | r] = JIT_OP::cpusha_ic;

        jit_compile_op[0172310 | r] = JIT_OP::cinvl_bc;
        jit_compile_op[0172320 | r] = JIT_OP::cinvp_bc;
        jit_compile_op[0172330 | r] = JIT_OP::cinva_bc;
        jit_compile_op[0172350 | r] = JIT_OP::cpushl_bc;
        jit_compile_op[0172360 | r] = JIT_OP::cpushp_bc;
        jit_compile_op[0172370 | r] = JIT_OP::cpusha_bc;

        jit_compile_op[0172400 | r] = JIT_OP::pflushn;
        jit_compile_op[0172410 | r] = JIT_OP::pflush;
        jit_compile_op[0172420 | r] = JIT_OP::pflushan;
        jit_compile_op[0172430 | r] = JIT_OP::pflusha;

        jit_compile_op[0172510 | r] = JIT_OP::ptestw;
        jit_compile_op[0172550 | r] = JIT_OP::ptestr;
    }
}
