#ifndef JIT_HPP_
#define JIT_HPP_ 1
#include <asmjit/asmjit.h>
#include <memory>
#include <vector>
using namespace asmjit;


struct JitError {};
extern std::unique_ptr<x86::Assembler> as; 

void ea_getaddr_jit(int type, int reg, int sz);
void ea_readB_jit(int type, int reg);
void ea_readW_jit(int type, int reg);
void ea_readL_jit(int type, int reg);
void ea_writeB_jit(int type, int reg, bool update);
void ea_writeW_jit(int type, int reg, bool update);
void ea_writeL_jit(int type, int reg, bool update);
extern std::unique_ptr<JitRuntime> rt;
void deregister_frame(struct jit_cache* p);
struct jit_cache {
    void (*exec)(uint32_t addr);
    uint32_t begin;
    std::vector<uint16_t> offset;
    std::vector<uint8_t> unwindInfo;
#if WIN32
    RUNTIME_FUNCTION table;
#endif

    ~jit_cache() {
        rt->release(exec);
        deregister_frame(this);
    }
};
void jit_compile(uint32_t base, uint32_t len);
void jit_run(uint32_t pc);
#endif