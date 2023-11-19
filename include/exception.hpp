#ifndef CPU_EXCEPTION_
#define CPU_EXCEPTION_
#include <stdint.h>

enum class TT : uint8_t { NORMAL = 0, MOVE16, ALT, ACK };
enum class TM : uint8_t {
    ALT_0 = 0,
    USER_DATA,
    USER_CODE,
    MMU_DATA,
    MMU_CODE,
    SYS_DATA,
    SYS_CODE,
    ALT_7
};
enum class SIZ : uint8_t { L = 0, B, W, LN, NONE };
void handle_exception(int n);

// 2
struct FaultParam {
    bool CP, CU, CT, CM, MA, ATC, LK, RW;
    SIZ size;
    TT tt;
    TM tm;
    FaultParam() { reset(); }
    void reset() {
        CP = CU = CT = CM = MA = ATC = LK = RW = false;
        size = SIZ::NONE;
        tt = TT::NORMAL;
        tm = TM::ALT_0;
    }
};
[[noreturn]] void ACCESS_FAULT(uint32_t a, SIZ sz, bool rw,
                               TM m = TM::USER_DATA);
// 3
[[noreturn]] void ADDRESS_ERROR(uint32_t next);

// 4
[[noreturn]] void ILLEGAL_OP();

// 5
[[noreturn]] void DIV0_ERROR();

// 6
[[noreturn]] void CHK_ERROR();

// 7
[[noreturn]] void TRAPX_ERROR();

// 8
[[noreturn]] void PRIV_ERROR();

// 9
[[noreturn]] void TRACE();

// 10
[[noreturn]] void ALINE();

// 11
[[noreturn]] void FLINE();

// 14
[[noreturn]] void FORMAT_ERROR();

// 32-47
[[noreturn]] void TRAP_ERROR(int n);

// 48
[[noreturn]] void FP_EX_BSUN();
// 49
[[noreturn]] void FP_EX_INEX();
// 50
[[noreturn]] void FP_EX_DIV0();

// 51
[[noreturn]] void FP_EX_UNFL();

// 52
[[noreturn]] void FP_EX_OPERR();

// 53
[[noreturn]] void FP_EX_OVFL();

// 54
[[noreturn]] void FP_EX_SNAN();

// 55(not used)
[[noreturn]] void FP_EX_UNIMPL_TYPE();

[[noreturn]] void IRQ(int n);
#endif
