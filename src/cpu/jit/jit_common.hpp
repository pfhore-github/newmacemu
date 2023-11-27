#ifndef JIT_COMMON_HPP_
#define JIT_COMMON_HPP_ 1
#include "68040.hpp"
#include "asmjit/asmjit.h"
#include "inline.hpp"
#include "jit.hpp"
#ifdef __x86_64__
using namespace asmjit::x86;
constexpr auto EA = dword_ptr(rbx, offsetof(Cpu, EA));
constexpr auto CC_C = byte_ptr(rbx, offsetof(Cpu, C));
constexpr auto CC_V = byte_ptr(rbx, offsetof(Cpu, V));
constexpr auto CC_N = byte_ptr(rbx, offsetof(Cpu, N));
constexpr auto CC_Z = byte_ptr(rbx, offsetof(Cpu, Z));
constexpr auto CC_X = byte_ptr(rbx, offsetof(Cpu, X));
constexpr auto CC_T = byte_ptr(rbx, offsetof(Cpu, T));
constexpr auto CC_S = byte_ptr(rbx, offsetof(Cpu, S));
constexpr auto DR_B(int n) { return byte_ptr(rbx, n * sizeof(uint32_t)); }
constexpr auto DR_W(int n) { return word_ptr(rbx, n * sizeof(uint32_t)); }
constexpr auto DR_L(int n) { return dword_ptr(rbx, n * sizeof(uint32_t)); }
constexpr auto AR_B(int n) { return byte_ptr(rbx, (8 + n) * sizeof(uint32_t)); }
constexpr auto AR_W(int n) { return word_ptr(rbx, (8 + n) * sizeof(uint32_t)); }
constexpr auto AR_L(int n) { return dword_ptr(rbx, (8 + n) * sizeof(uint32_t)); }
constexpr auto SP = AR_L(7);
constexpr auto c_pc = dword_ptr(rbx, offsetof(Cpu, PC));
#define CPU_BYTE(name_) x86::byte_ptr(x86::rbx, offsetof(Cpu, name_))
#define CPU_WORD(name_) x86::word_ptr(x86::rbx, offsetof(Cpu, name_))
#define CPU_LONG(name_) x86::dword_ptr(x86::rbx, offsetof(Cpu, name_))
#define CPU_DLONG(name_) x86::qword_ptr(x86::rbx, offsetof(Cpu, name_))
void call_prolog();
void call_epilog(bool restoreRax);
#ifdef WIN32
constexpr auto ARG1 = rcx;
constexpr auto ARG2 = rdx;
constexpr auto ARG3 = r8;
constexpr auto ARG4 = r9;
#include <windows.h>
#else
constexpr auto ARG1 = rdi;
constexpr auto ARG2 = rsi;
constexpr auto ARG3 = rdx;
constexpr auto ARG4 = rcx;
#endif

template <class Adr> void jit_readB(const Adr &adr) {
    if(adr != ARG1.r32()) {
        as->mov(ARG1.r32(), adr);
    }
    as->call(ReadB);
}

template <class Adr> void jit_readW(const Adr &adr) {
    if(adr != ARG1.r32()) {
        as->mov(ARG1.r32(), adr);
    }
    as->call(ReadW);
}

template <class Adr> void jit_readL(const Adr &adr) {
    if(adr != ARG1.r32()) {
        as->mov(ARG1.r32(), adr);
    }
    as->call(ReadL);
}

template <class Adr, class Val> void jit_writeB(const Adr &adr, const Val &v) {
    if(adr != ARG1.r32()) {
        as->mov(ARG1.r32(), adr);
    }
    if(v != ARG2.r8()) {
        as->mov(ARG2.r8(), v);
    }
    as->call(WriteB);
}

template <class Adr, class Val> void jit_writeW(const Adr &adr, const Val &v) {
    if(adr != ARG1.r32()) {
        as->mov(ARG1.r32(), adr);
    }
    if(v != ARG2.r16()) {
        as->mov(ARG2.r16(), v);
    }
    as->call(WriteW);
}

template <class Adr, class Val> void jit_writeL(const Adr &adr, const Val &v) {
    if(adr != ARG1.r32()) {
        as->mov(ARG1.r32(), adr);
    }
    if(v != ARG2.r32()) {
        as->mov(ARG2.r32(), v);
    }
    as->call(WriteL);
}
enum class COND {
    TRUE, FALSE, EQ, NE, LE, LT, GE, GT
};
template<class T> void jit_if(COND cc, T t) {
    auto ENDIF = as->newLabel();
    switch(cc) {
        case COND::TRUE: as->jz(ENDIF); break;
        case COND::FALSE: as->jnz(ENDIF); break;
        case COND::EQ: as->jne(ENDIF); break;
        case COND::NE: as->je(ENDIF); break;
        case COND::LE: as->jg(ENDIF); break;
        case COND::LT: as->jge(ENDIF); break;
        case COND::GE: as->jl(ENDIF); break;
        case COND::GT: as->jle(ENDIF); break;
    }
    t();
    as->bind(ENDIF);
}

template<class T, class F> void jit_if(COND cc, T t, F f) {
    auto ELSE = as->newLabel();
    auto ENDIF = as->newLabel();
     switch(cc) {
        case COND::TRUE: as->jz(ELSE); break;
        case COND::FALSE: as->jnz(ELSE); break;
        case COND::EQ: as->jne(ELSE); break;
        case COND::NE: as->je(ELSE); break;
        case COND::LE: as->jg(ELSE); break;
        case COND::LT: as->jge(ELSE); break;
        case COND::GE: as->jl(ELSE); break;
        case COND::GT: as->jle(ELSE); break;
    }
    t();
    as->jmp(ENDIF);
    as->bind(ELSE);
    f();
    as->bind(ENDIF);
}
#endif
#endif
