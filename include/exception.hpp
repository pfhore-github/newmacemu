#ifndef CPU_EXCEPTION_
#define CPU_EXCEPTION_
#include "68040.hpp"
#include <stdint.h>
void do_exception();

inline void CPU_RAISE(int n) {
    cpu.ex_n = n;
    longjmp(cpu.ex_buf, 2);
}
inline void ACCESS_FAULT() {
    CPU_RAISE(2);
}
inline void ADDRESS_ERROR() {
    CPU_RAISE(3);
}
inline void ILLEGAL_OP() {
    CPU_RAISE(4);
}
inline void DIV0_ERROR() {
    CPU_RAISE(5);
}
inline void CHK_ERROR() {
    CPU_RAISE(6);
}
inline void PRIV_ERROR() {
    CPU_RAISE(8);
}
inline void ILLEGAL_FP() {
    CPU_RAISE(11);
}
inline void TRAP(int i) {
    CPU_RAISE(32+i);
}
inline void TRAPX() {
    CPU_RAISE(7);
}
inline void ALINE() {
    CPU_RAISE(10);
}

#endif