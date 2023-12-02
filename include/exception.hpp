#ifndef CPU_EXCEPTION_
#define CPU_EXCEPTION_
#include <stdint.h>
#include <setjmp.h>
constexpr uint16_t TT_MASK = 0x18;
constexpr uint16_t TT_NORMAL = 0;
constexpr uint16_t TT_MOVE16 = 1 << 3;
constexpr uint16_t TT_ALT = 2 << 3;
constexpr uint16_t TT_ACK = 3 << 3;
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
enum class EXCEPTION_NUMBER : uint8_t {
    NO_ERR = 0,
    RESET = 1,
    AFAULT,
    ADDR_ERR,
    ILLEGAL_OP,
    DIV0,
    CHK_FAIL,
    TRAPx,
    PRIV_ERR,
    TRACE,
    ALINE,
    FLINE,
    FORMAT_ERR = 14,
    UNDEF_IRQ,
    UNKNOWN_IRQ = 24,
    IRQ_LV1,
    IRQ_LV2,
    IRQ_LV3,
    IRQ_LV4,
    IRQ_LV5,
    IRQ_LV6,
    IRQ_LV7,
    TRAP0,
    TRAP1,
    TRAP2,
    TRAP3,
    TRAP4,
    TRAP5,
    TRAP6,
    TRAP7,
    TRAP8,
    TRAP9,
    TRAP10,
    TRAP11,
    TRAP12,
    TRAP13,
    TRAP14,
    TRAP15,
    FP_UNORDER = 48,
    FP_INEX,
    FP_DIV0,
    FP_UNFL,
    FP_OPERR,
    FP_OVFL,
    FP_SNAN,
    FP_UNIMPL_TYPE
};
void handle_exception(EXCEPTION_NUMBER n);
constexpr uint16_t SSW_CP = 1 << 15;
constexpr uint16_t SSW_CU = 1 << 14;
constexpr uint16_t SSW_CT = 1 << 13;
constexpr uint16_t SSW_CM = 1 << 12;
constexpr uint16_t SSW_MA = 1 << 11;
constexpr uint16_t SSW_ATC = 1 << 10;
constexpr uint16_t SSW_LK = 1 << 9;
constexpr uint16_t SSW_RW = 1 << 8;
[[noreturn]] void ACCESS_FAULT(uint32_t a, SIZ sz, bool rw,
                               TM m, uint16_t tt = TT_NORMAL);
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

extern jmp_buf ex_buf;
extern EXCEPTION_NUMBER ex_n;
#endif
