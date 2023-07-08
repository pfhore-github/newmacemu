#ifndef CPU_68040_
#define CPU_68040_ 1
#include <stdint.h>
#include <functional>
#include <unordered_map>
#include <memory>
#include <atomic>
#include <setjmp.h>
#include "mpfr.h"
#include "exception.hpp"
enum class TT {
    NORMAL = 0, MOVE16, ALT, ACK
};
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
enum class SIZ {
    L = 0, B, W, LN, NONE
};
struct MemBus {
    uint32_t A;
    uint32_t D = 0;
    TT tt = TT::NORMAL;
    TM tm = TM::USER_DATA;
    SIZ sz;
    uint8_t UPM = 0;
    bool RW;
};
struct Opcode;
struct AccessFault_t {
    uint32_t addr;
    uint32_t ea = 0;
    bool CP = false, CU = false, CT = false, CM = false, MA = false,
         ATC = false, LK = false, RW;
    SIZ size;
    TT tt = TT::NORMAL;
    TM tm = TM::USER_DATA;
};
enum class FPU_PREC {
    X, S, D, AUTO,
};

struct Cpu {
    uint32_t D[8];
    uint32_t A[8];
    uint32_t& R(int n) { return n < 8 ? D[n&7] : A[n&7]; }
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

    std::unordered_map<uint32_t, atc_entry> s_atc, sg_atc, u_atc, ug_atc;

    // internal
    jmp_buf ex_buf;
    uint32_t nextpc;

    AccessFault_t af_value;

    int n_mask = 0;
    bool in_exception;
    bool must_trace;
    std::unordered_map<uint32_t, std::pair<std::function<void()>, int> > icache;
    uint32_t EA;
    int n;
    std::atomic<bool> sleep;
    bool movem_run;
};
struct DecodeError {};
// Mac 68K has no multi CPU, so doesn't support multi CPU!
extern Cpu cpu;
std::pair<std::function<uint32_t()>, int> ea_addr(int type, int reg, uint32_t pc, int sz, bool rw);
std::pair<std::function<uint8_t()>, int> ea_read8(int type, int reg, uint32_t pc);
std::pair<std::function<uint16_t()>, int> ea_read16(int type, int reg, uint32_t pc) ;
std::pair<std::function<uint32_t()>, int> ea_read32(int type, int reg, uint32_t pc) ;
std::pair<std::function<void(uint8_t)>, int> ea_write8(int type, int reg, uint32_t pc) ;
std::pair<std::function<void(uint16_t)>, int> ea_write16(int type, int reg, uint32_t pc) ;
std::pair<std::function<void(uint32_t)>, int> ea_write32(int type, int reg, uint32_t pc);
std::tuple<std::function<uint8_t()>, std::function<void(uint8_t)>, int> ea_rw8(int type, int reg, uint32_t pc);
std::tuple<std::function<uint16_t()>, std::function<void(uint16_t)>, int> ea_rw16(int type, int reg, uint32_t pc);
std::tuple<std::function<uint32_t()>, std::function<void(uint32_t)>, int> ea_rw32(int type, int reg, uint32_t pc);

#endif