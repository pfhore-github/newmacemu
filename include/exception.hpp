#ifndef CPU_EXCEPTION_
#define CPU_EXCEPTION_
#include <stdint.h>
[[noreturn]] void ACCESS_FAULT() ; // 2
[[noreturn]] void ADDRESS_ERROR(uint32_t next); // 3
[[noreturn]] void ILLEGAL_OP();        // 4
[[noreturn]] void DIV0_ERROR();        // 5
[[noreturn]] void CHK_ERROR();         // 6
[[noreturn]] void TRAPX_ERROR();       // 7
[[noreturn]] void PRIV_ERROR();        // 8
[[noreturn]] void TRACE();             // 9
[[noreturn]] void ALINE();             // 10
[[noreturn]] void FLINE();             // 11
[[noreturn]] void FORMAT_ERROR();      // 14
[[noreturn]] void TRAP_ERROR(int n);         // 32-47
[[noreturn]] void FP_EX_BSUN();        // 48
[[noreturn]] void FP_EX_INEX();        // 49
[[noreturn]] void FP_EX_DIV0();        // 50
[[noreturn]] void FP_EX_UNFL();        // 51
[[noreturn]] void FP_EX_OPERR();       // 52
[[noreturn]] void FP_EX_OVFL();        // 53
[[noreturn]] void FP_EX_SNAN();        // 54
// not implemented
// void FP_EX_UNIMPL_TYPE(); // 55


#endif