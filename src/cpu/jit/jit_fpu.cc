#include "exception.hpp"
#include "jit_common.hpp"
#include "memory.hpp"
#include "mpfr.h"
#include <memory>
#include <unordered_map>
#include <utility>
using namespace asmjit;
extern CpuFeatures feature;
void jit_postop(x86::Assembler &a);
extern volatile bool testing;
extern std::unordered_map<uint32_t, Label> jumpMap;
using jit_op = void (*)(uint16_t op);
extern jit_op jit_compile_op[0x10000];
void store_fpr(int fpn, FPU_PREC p = FPU_PREC::AUTO);
namespace JIT_OP {
#ifdef __x86_64__
void jit_fpu_prologue() {
    as->call(mpfr_clear_flags);
    as->mov(x86::eax, x86::dword_ptr(x86::rbx, offsetof(Cpu, oldpc)));
    as->mov(x86::dword_ptr(x86::rbx, offsetof(Cpu, FPIAR)), x86::eax);
}
void update_pc();
constexpr auto FP_TMP() {
    return x86::dword_ptr(x86::rbx, offsetof(Cpu, fp_tmp));
}
constexpr auto FP_RND() {
    return x86::dword_ptr(x86::rbx, offsetof(Cpu, FPCR.RND));
}
void jit_fmovecr(int opc, int fpn) {
    switch(opc) {
    case 0:
        // PI
        as->mov(ARG1.r32(), FP_TMP());
        as->mov(ARG2.r32(), FP_RND());
        as->call(mpfr_const_pi);
        break;
    case 0xB:
        // log10_2
        as->mov(ARG1.r32(), FP_TMP());
        as->mov(ARG2.r32(), FP_RND());
        as->call(mpfr_const_pi);
        as->mov(ARG1.r32(), FP_TMP());
        as->mov(ARG2.r32(), 2);
        as->mov(ARG3.r32(), FP_RND());
        as->call(mpfr_log10);
        break;
    case 0xC:
        // e
        mpfr_set_ui(cpu.fp_tmp, 1.0, cpu.FPCR.RND);
        mpfr_exp(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
        break;
    case 0xD:
        // log2_e
        mpfr_set_ui(cpu.fp_tmp, 1.0, cpu.FPCR.RND);
        mpfr_exp(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
        mpfr_log2(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
        break;
    case 0xE:
        // log10_e
        mpfr_set_ui(cpu.fp_tmp, 1.0, cpu.FPCR.RND);
        mpfr_exp(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
        mpfr_log10(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
        break;
    case 0x30:
        // log2_e
        mpfr_const_log2(cpu.fp_tmp, cpu.FPCR.RND);
        break;
    case 0x31:
        // ln(10)
        mpfr_set_ui(cpu.fp_tmp, 10.0, cpu.FPCR.RND);
        mpfr_log(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
        break;
    case 0x32:
        // 10^0
        mpfr_ui_pow_ui(cpu.fp_tmp, 10, 0, cpu.FPCR.RND);
        break;
    case 0x33:
        // 10^1
        mpfr_ui_pow_ui(cpu.fp_tmp, 10, 1, cpu.FPCR.RND);
        break;
    case 0x34:
        // 10^2
        mpfr_ui_pow_ui(cpu.fp_tmp, 10, 2, cpu.FPCR.RND);
        break;

    case 0x35:
        // 10^4
        mpfr_ui_pow_ui(cpu.fp_tmp, 10, 4, cpu.FPCR.RND);
        break;
    case 0x36:
        // 10^8
        mpfr_ui_pow_ui(cpu.fp_tmp, 10, 8, cpu.FPCR.RND);
        break;
    case 0x37:
        // 10^16
        mpfr_ui_pow_ui(cpu.fp_tmp, 10, 16, cpu.FPCR.RND);
        break;
    case 0x38:
        // 10^32
        mpfr_ui_pow_ui(cpu.fp_tmp, 10, 32, cpu.FPCR.RND);
        break;
    case 0x39:
        // 10^64
        mpfr_ui_pow_ui(cpu.fp_tmp, 10, 64, cpu.FPCR.RND);
        break;
    case 0x3A:
        // 10^128
        mpfr_ui_pow_ui(cpu.fp_tmp, 10, 128, cpu.FPCR.RND);
        break;
    case 0x3B:
        // 10^256
        mpfr_ui_pow_ui(cpu.fp_tmp, 10, 256, cpu.FPCR.RND);
        break;
    case 0x3C:
        // 10^512
        mpfr_ui_pow_ui(cpu.fp_tmp, 10, 512, cpu.FPCR.RND);
        break;
    case 0x3D:
        // 10^1024
        mpfr_ui_pow_ui(cpu.fp_tmp, 10, 1024, cpu.FPCR.RND);
        break;
    case 0x3E:
        // 10^2048
        mpfr_ui_pow_ui(cpu.fp_tmp, 10, 2048, cpu.FPCR.RND);
        break;
    case 0x3F:
        // 10^4096
        mpfr_ui_pow_ui(cpu.fp_tmp, 10, 4096, cpu.FPCR.RND);
        break;
    case 0x0F:
    default:
        mpfr_set_zero(cpu.fp_tmp, 1);
        break;
    }
    store_fpr(fpn);
}
void fop(uint16_t op) {
    uint16_t extw = FETCH();
    bool rm = extw & 1 << 14;
    unsigned int src = extw >> 10 & 7;
    unsigned int dst = extw >> 7 & 7;
    unsigned int opc = extw & 0x7f;
    update_pc();
    jit_fpu_prologue();
    if(!(extw & 1 << 15)) {
        if(!(extw & 1 << 13)) {
            if(rm && src == 7) {
                // FMOVECR
                jit_fmovecr( opc, dst);
                return;
            }
#if 0            
            loadFP(TYPE(op), REG(op), rm, src);
            fop_do(opc, dst);
        } else {
            // FMOVE to ea
            fpu_store(src, TYPE(op), REG(op), dst, opc);
#endif
        }
    } else {
#if 0
        switch(extw >> 13 & 3) {
        case 0:
            // FMOVE(M) to FPcc
            fmove_to_fpcc(TYPE(op), REG(op), src);
            break;
        case 1:
            fmove_from_fpcc(TYPE(op), REG(op), src);
            break;
        case 2: {
            uint8_t reglist =
                extw & 1 << 11 ? cpu.D[extw >> 4 & 7] : extw & 0xff;
            if(TYPE(op) == 3) {
                cpu.A[REG(op)] = fmovem_to_reg(cpu.A[REG(op)], reglist);
            } else {
                fmovem_to_reg(ea_getaddr(TYPE(op), REG(op), 0), reglist);
            }
            TRACE_BRANCH();
            break;
        }
        case 3: {
            uint8_t reglist =
                extw & 1 << 11 ? cpu.D[extw >> 4 & 7] : extw & 0xff;
            if(!(extw & 1 << 12) && TYPE(op) == 4) {
                cpu.A[REG(op)] = fmovem_from_reg_rev(cpu.A[REG(op)], reglist);
            } else {
                fmovem_from_reg(ea_getaddr(TYPE(op), REG(op), 0), reglist);
            }
            TRACE_BRANCH();
        }
        }
#endif
    }
}
#endif
} // namespace JIT_OP

void init_jit_table_fpu() {
#if 0
    for(int i = 0; i < 074; ++i) {
        jit_compile_op[0171000 | i] = JIT_OP::fop;
        jit_compile_op[0171100 | i] = JIT_OP::fscc;
        jit_compile_op[0171400 | i] = JIT_OP::fsave;
        jit_compile_op[0171500 | i] = JIT_OP::frestore;
    }
    for(int k = 0; k < 8; ++k) {
        jit_compile_op[0171110 | k] = JIT_OP::fdbcc;
    }
    jit_compile_op[0171172] = JIT_OP::ftrapcc;
    jit_compile_op[0171173] = JIT_OP::ftrapcc;
    jit_compile_op[0171174] = JIT_OP::ftrapcc;
    for(int i = 0; i < 0100; ++i) {
        jit_compile_op[0171200 | i] = JIT_OP::fbcc_w;
        jit_compile_op[0171300 | i] = JIT_OP::fbcc_l;
    }
#endif
}
