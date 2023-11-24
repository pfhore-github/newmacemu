#ifndef CPU_68040_
#define CPU_68040_ 1
#include "exception.hpp"
#include "mpfr.h"
#include <atomic>
#include <functional>
#include <memory>
#include <setjmp.h>
#include <stdint.h>
#include <unordered_map>

enum class FPU_PREC {
    X,
    S,
    D,
    AUTO,
};
struct FaultParam;
struct Cpu {
    uint32_t D[8];
    uint32_t A[8];
    uint32_t EA;
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
    jmp_buf ex;
    std::unique_ptr<FaultParam> faultParam;
    uint32_t oldpc;
    uint32_t ex_addr;
    volatile uint8_t ex_n;

    bool in_exception;
    bool must_trace;

    bool movem_run;
    std::atomic<bool> sleeping;

    std::atomic<int> inturrupt;
    uint32_t &R(int n) { return n < 8 ? D[n & 7] : A[n & 7]; }
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

// Mac 68K has no multi CPU, so doesn't support multi CPU!
extern Cpu cpu;
uint32_t ea_getaddr(int type, int reg, int sz);
uint8_t ea_readB(int type, int reg);
uint32_t ea_readW(int type, int reg);
uint32_t ea_readL(int type, int reg);
void ea_writeB(int type, int reg, uint8_t v, bool update);
void ea_writeW(int type, int reg, uint16_t v, bool update);
void ea_writeL(int type, int reg, uint32_t v, bool update);
using run_t = void (*)(uint16_t op);

inline void PRIV_CHECK() {
    if(!cpu.S) {
        PRIV_ERROR();
    }
}
#endif
