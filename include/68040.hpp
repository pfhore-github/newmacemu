#ifndef CPU_68040_
#define CPU_68040_ 1
#include "exception.hpp"
#include "mpfr.h"
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <functional>
#include <memory>
#include <setjmp.h>
#include <stdint.h>
#include <unordered_map>
enum class TT { NORMAL = 0, MOVE16, ALT, ACK };
enum class TM {
    ALT_0 = 0,
    USER_DATA,
    USER_CODE,
    MMU_DATA,
    MMU_CODE,
    SYS_DATA,
    SYS_CODE,
    ALT_7
};
enum class SIZ { L = 0, B, W, LN, NONE };
struct MemBus {
    uint32_t A;
    uint32_t D = 0;
    TT tt = TT::NORMAL;
    TM tm = TM::USER_DATA;
    SIZ sz;
    uint8_t UPM = 0;
    bool RW;
};

struct AccessFault_t {
    uint32_t addr;
    bool CP = false, CU = false, CT = false, CM = false, MA = false,
         ATC = false, LK = false, RW;
    SIZ size;
    TT tt = TT::NORMAL;
    TM tm = TM::USER_DATA;
};
enum class FPU_PREC {
    X,
    S,
    D,
    AUTO,
};

struct Cpu {
    uint32_t D[8];
    uint32_t A[8];
    uint32_t &R(int n) { return n < 8 ? D[n & 7] : A[n & 7]; }
    uint32_t PC;
    // CCR
    bool X, N, Z, V, C;

    // FPU
    mpfr_t FP[8];
    uint64_t FP_nan[8];
    struct FPCR_t {
        mpfr_rnd_t RND;
        FPU_PREC PREC;
        bool INEX1, INEX2, DZ, UNFL, OVFL, OPERR, S_NAN, BSUN;
    } FPCR;
    struct FPSR_t {
        bool CC_NAN, CC_I, CC_Z, CC_N;
        uint8_t Quat;
        bool QuatSign;
        bool INEX1, INEX2, DZ, UNFL, OVFL, OPERR, S_NAN, BSUN;
        bool EXC_INEX, EXC_DZ, EXC_UNFL, EXC_OVFL, EXC_IOP;
    } FPSR;
    uint32_t FPIAR;
    mpfr_t fp_tmp;
    uint64_t fp_tmp_nan;
    int fp_tmp_tv;

    // hyperviser
    uint32_t MSP, ISP, USP;
    uint8_t I, T;
    bool M, S;
    uint32_t VBR;
    uint8_t SFC, DFC;
    uint32_t URP, SRP;
    bool TCR_E, TCR_P;
    struct TTR_t {
        uint8_t logic_base;
        uint8_t logic_mask;
        bool E;
        uint8_t S;
        uint8_t U;
        uint8_t CM;
        bool W;
    } ITTR[2], DTTR[2];
    uint32_t MMUSR;
    bool CACR_DE, CACR_IE;

    // ATC
    struct atc_entry {
        uint32_t paddr;
        uint8_t U;
        bool S;
        uint8_t CM;
        bool M, W, R;
    };

    std::unordered_map<uint32_t, atc_entry> l_atc[2], g_atc[2];

    // internal
    jmp_buf ex_buf;
    uint32_t oldpc;

    AccessFault_t af_value;

    bool in_exception;
    bool must_trace;
    // JIT cache
    //    std::unordered_map<uint32_t, int> icache;
    uint32_t EA;
    int n;
    std::mutex mtx_;
    std::condition_variable cond_;
    bool movem_run;
    bool sleeping;

    std::atomic<int> inturrupt;
    std::atomic<bool> run;
};

struct mmu_result {
    uint32_t paddr = 0;
    uint8_t Ux = 0;
    uint8_t CM = 0;
    bool R = false;
    bool T = false;
    bool W = false;
    bool M = false;
    bool S = false;
    bool G = false;
    bool B = false;
    uint32_t value() const {
        return R | T << 1 | W << 2 | M << 4 | CM << 5 | S << 7 | Ux << 8 |
               G << 10 | B << 11 | paddr << 12;
    }
};

struct DecodeError {};
// Mac 68K has no multi CPU, so doesn't support multi CPU!
extern Cpu cpu;
uint32_t ea_getaddr(int type, int reg, int sz);
uint8_t ea_readB(int type, int reg);
uint32_t ea_readW(int type, int reg);
uint32_t ea_readL(int type, int reg);
void ea_writeB(int type, int reg, uint8_t v, bool update);
void ea_writeW(int type, int reg, uint16_t v, bool update);
void ea_writeL(int type, int reg, uint32_t v, bool update);
using run_t = void (*)(uint16_t op, int dn, int type, int reg);

inline void PRIV_CHECK() {
    if(!cpu.S) {
        PRIV_ERROR();
    }
}
#endif