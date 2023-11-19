#ifndef JIT_HPP_
#define JIT_HPP_ 1
#include <asmjit/asmjit.h>
#include <memory>
#include <vector>
#ifdef WIN32
#define ARG1L asmjit::x86::rcx
#define ARG2L asmjit::x86::rdx
#define ARG2L asmjit::x86::r8
#define ARG1 asmjit::x86::ecx
#define ARG2 asmjit::x86::edx
#define ARG3L asmjit::x86::r8d
#include <windows.h>
#else
#define ARG1L asmjit::x86::rdi
#define ARG2L asmjit::x86::rsi
#define ARG3L asmjit::x86::rdx
#define ARG1 asmjit::x86::edi
#define ARG2 asmjit::x86::esi
#define ARG3 asmjit::x86::edx
#endif

struct JitError {};

void ea_getaddr_jit(asmjit::x86::Assembler &a, int type, int reg, int sz);
void ea_readB_jit(asmjit::x86::Assembler &a, int type, int reg);
void ea_readW_jit(asmjit::x86::Assembler &a, int type, int reg);
void ea_readL_jit(asmjit::x86::Assembler &a, int type, int reg);
void ea_writeB_jit(asmjit::x86::Assembler &a, int type, int reg, bool update);
void ea_writeW_jit(asmjit::x86::Assembler &a, int type, int reg, bool update);
void ea_writeL_jit(asmjit::x86::Assembler &a, int type, int reg, bool update);
extern std::unique_ptr<asmjit::JitRuntime> rt;
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