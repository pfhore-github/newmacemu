#ifndef CPU_68040_
#define CPU_68040_ 1
#include "exception.hpp"
#include "mpfr.h"
#include <atomic>
#include <functional>
#include <memory>

#include <stdint.h>
#include <unordered_map>
#include "util.hpp"

class Fpu {
    public:
    virtual void init_table() = 0;
    virtual void init_jit() = 0;

    virtual void init() = 0;
    virtual void reset() = 0;

    virtual std::string dumpReg() = 0;
	virtual ~Fpu() {}
};
enum class FPU_TYPE {
    M68040_FULL, // 68040 + native special function
};
std::unique_ptr<Fpu> createFPU(FPU_TYPE fpuType);
struct Cpu {
    uint32_t D[8];
    uint32_t A[8];
    uint32_t EA;
    uint32_t PC;
    // CCR
    bool X, N, Z, V, C;

    // FPU
    std::unique_ptr<Fpu> fpu;
  
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
    uint32_t ex_addr;
    uint32_t oldpc;
 
    uint16_t fault_SSW;
    bool bus_lock;
    bool inJit = false;
    bool must_trace;

    bool movem_run;
    ThreadController run;

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
