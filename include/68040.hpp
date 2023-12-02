#ifndef CPU_68040_
#define CPU_68040_ 1
#include "exception.hpp"
#include "mpfr.h"
#include <atomic>
#include <functional>
#include <memory>

#include <stdint.h>
#include <unordered_map>

enum class FPU_PREC {
    X,
    S,
    D,
    AUTO,
};

// ATC

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

    mpfr_t fp_tmp;
    uint64_t fp_tmp_nan;
    int fp_tmp_tv;

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


    // internal
 
    uint16_t fault_SSW;
    uint32_t oldpc;
    bool bus_lock;

    bool must_trace;

    bool movem_run;
    std::atomic<bool> sleeping;

    std::atomic<int> inturrupt;
    uint32_t &R(int n) { return n < 8 ? D[n & 7] : A[n & 7]; }
};


// Mac 68K has no multi CPU, so doesn't support multi CPU!
extern Cpu cpu;
uint32_t ea_getaddr(int type, int reg, int sz);
uint8_t ea_readB(int type, int reg, bool lk = false);
uint32_t ea_readW(int type, int reg, bool lk = false);
uint32_t ea_readL(int type, int reg, bool lk = false);
void ea_writeB(int type, int reg, uint8_t v);
void ea_writeW(int type, int reg, uint16_t v);
void ea_writeL(int type, int reg, uint32_t v);
using run_t = void (*)(uint16_t op);

inline void PRIV_CHECK() {
    if(!cpu.S) {
        PRIV_ERROR();
    }
}
#endif
