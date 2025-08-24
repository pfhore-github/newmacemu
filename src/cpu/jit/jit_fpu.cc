#include "68040fpu.hpp"
#include "exception.hpp"
#include "jit_common.hpp"
#include "memory.hpp"
#include "mpfr.h"
#include <memory>
#include <unordered_map>
#include <utility>
// FPU call isn't inlined as it calls mpfr library
using namespace asmjit;
extern CpuFeatures feature;
void jit_postop(x86::Assembler &a);
extern volatile bool testing;
extern std::unordered_map<uint32_t, Label> jumpMap;
using jit_op = void (*)(uint16_t op);
extern jit_op jit_compile_op[0x10000];

namespace JIT_OP {
void jumpC(uint32_t t);
void jit_trace_branch();
void jit_priv_check();
#ifdef __x86_64__

void jit_fmovecr(int opc, int fpn) {
    as->mov(ARG1, FPU_P());
    as->mov(ARG2, fpn);
    switch(opc) {
    case 0:
        // PI
        as->call(M68040_Impl::fmovecr_0);
        break;
    case 0xB:
        // log10_2
        as->call(M68040_Impl::fmovecr_B);
        break;
    case 0xC:
        // e
        as->call(M68040_Impl::fmovecr_C);
        break;
    case 0xD:
        // log2_e
        as->call(M68040_Impl::fmovecr_D);
        break;
    case 0xE:
        // log10_e
        as->call(M68040_Impl::fmovecr_E);
        break;
    case 0x30:
        // log2_e
        as->call(M68040_Impl::fmovecr_30);
        break;
    case 0x31:
        // ln(10)
        as->call(M68040_Impl::fmovecr_31);
        break;
    case 0x32:
        // 10^0
        as->call(M68040_Impl::fmovecr_32);
        break;
    case 0x33:
        // 10^1
        as->call(M68040_Impl::fmovecr_33);
        break;
    case 0x34:
        // 10^2
        as->call(M68040_Impl::fmovecr_34);
        break;

    case 0x35:
        // 10^4
        as->call(M68040_Impl::fmovecr_35);
        break;
    case 0x36:
        // 10^8
        as->call(M68040_Impl::fmovecr_36);
        break;
    case 0x37:
        // 10^16
        as->call(M68040_Impl::fmovecr_37);
        break;
    case 0x38:
        // 10^32
        as->call(M68040_Impl::fmovecr_38);
        break;
    case 0x39:
        // 10^64
        as->call(M68040_Impl::fmovecr_39);
        break;
    case 0x3A:
        // 10^128
        as->call(M68040_Impl::fmovecr_3A);
        break;
    case 0x3B:
        // 10^256
        as->call(M68040_Impl::fmovecr_3B);
        break;
    case 0x3C:
        // 10^512
        as->call(M68040_Impl::fmovecr_3C);
        break;
    case 0x3D:
        // 10^1024
        as->call(M68040_Impl::fmovecr_3D);
        break;
    case 0x3E:
        // 10^2048
        as->call(M68040_Impl::fmovecr_3E);
        break;
    case 0x3F:
        // 10^4096
        as->call(M68040_Impl::fmovecr_3F);
        break;
    case 0x0F:
    default:
        as->call(M68040_Impl::fmovecr_F);

        break;
    }
}

void jit_loadFP(int type, int reg, bool rm, int src) {
    if(rm) {
        switch(FP_SIZE(src)) {
        case FP_SIZE::LONG:
            ea_readL_jit(type, reg);
            as->movsxd(ARG2, x86::eax);
            as->mov(ARG1, FPU_P());
            as->call(&M68040_Impl::fload_l);
            break;
        case FP_SIZE::SINGLE:
            ea_readL_jit(type, reg);
            as->mov(ARG2, x86::eax);
            as->mov(ARG1, FPU_P());
            as->call(&M68040_Impl::fload_s);
            break;
        case FP_SIZE::EXT: {
            if(type == 7 && reg == 4) {
                // EXT imm
                as->mov(x86::esi, c_pc);
                as->mov(EA, x86::esi);
                as->add(c_pc, 12);
            } else {
                ea_getaddr_jit(type, reg, 12);
            }
            as->mov(ARG1, FPU_P());
            as->call(&M68040_Impl::fload_x);
            break;
        }
        case FP_SIZE::PACKED:
            if(type == 7 && reg == 4) {
                // EXT imm
                as->mov(x86::esi, c_pc);
                as->mov(EA, x86::esi);
                as->add(c_pc, 12);
            } else {
                ea_getaddr_jit(type, reg, 12);
            }
            as->mov(ARG1, FPU_P());
            as->call(&M68040_Impl::fload_p);
            break;
        case FP_SIZE::WORD:
            ea_readW_jit(type, reg);
            as->movsx(ARG2, x86::ax);
            as->mov(ARG1, FPU_P());
            as->call(&M68040_Impl::fload_w);
            break;
        case FP_SIZE::DOUBLE: {
            if(type == 7 && reg == 4) {
                // EXT imm
                as->mov(x86::esi, c_pc);
                as->mov(EA, x86::esi);
                as->add(c_pc, 8);
            } else {
                ea_getaddr_jit(type, reg, 8);
            }
            as->mov(ARG1, FPU_P());
            as->call(&M68040_Impl::fload_d);
            break;
        }
        case FP_SIZE::BYTE:
            ea_readB_jit(type, reg);
            as->movsx(ARG2, x86::al);
            as->mov(ARG1, FPU_P());
            as->call(&M68040_Impl::fload_b);
            break;
        case FP_SIZE::PACKED2:
            __builtin_unreachable();
        }
    } else {
        as->mov(ARG1, FPU_P());
        as->mov(ARG2.r32(), src);
        as->call(&M68040_Impl::fload_r);
    }
}

void jit_fop_do(int opc, int dst) {
    as->mov(ARG1, FPU_P());
    as->mov(ARG2.r32(), dst);
    switch(opc) {
    case 0: // FMOVE
        as->call(&M68040_Impl::fmove);
        break;
    case 1: // FINT
        as->call(&M68040_Impl::fint);
        break;
    case 2: // FSINH
        as->call(&M68040_Impl::fsinh);
        break;
    case 3: // FINTRZ
        as->call(&M68040_Impl::fintrz);
        break;
    case 4: // FSQRT
        as->call(&M68040_Impl::fsqrt);
        break;
    case 6: // FLOGNP1
        as->call(&M68040_Impl::flognp1);
        break;
    case 8: // FETOXM1
        as->call(&M68040_Impl::fetoxm1);
        break;
    case 9: // FTANH
        as->call(&M68040_Impl::ftanh);
        break;
    case 10: // FATAN
        as->call(&M68040_Impl::fatan);
        break;
    case 12: // FASIN
        as->call(&M68040_Impl::fasin);
        break;
    case 13: // FATANH
        as->call(&M68040_Impl::fatanh);
        break;
    case 14: // FSIN
        as->call(&M68040_Impl::fsin);
        break;
    case 15: // FTAN
        as->call(&M68040_Impl::ftan);
        break;
    case 16: // FETOX
        as->call(&M68040_Impl::fetox);
        break;
    case 17: // FTWOTOX
        as->call(&M68040_Impl::ftwotox);
        break;
    case 18: // FTENTOX
        as->call(&M68040_Impl::ftentox);
        break;
    case 20: // FLOGN
        as->call(&M68040_Impl::flogn);
        break;
    case 21: // FLOG10
        as->call(&M68040_Impl::flog10);
        break;
    case 22: // FLOG2
        as->call(&M68040_Impl::flog2);
        break;
    case 24: // FABS
        as->call(&M68040_Impl::fabs);
        break;
    case 25: // FCOSH
        as->call(&M68040_Impl::fcosh);
        break;
    case 26: // FNEG
        as->call(&M68040_Impl::fneg);
        break;
    case 28: // FACOS
        as->call(&M68040_Impl::facos);
        break;
    case 29: // FCOS
        as->call(&M68040_Impl::fcos);
        break;
    case 30: // FGETEXP
        as->call(&M68040_Impl::fgetexp);
        break;
    case 31: // FGETMAN
        as->call(&M68040_Impl::fgetman);
        break;
    case 32: // FDIV
        as->call(&M68040_Impl::fdiv);
        break;
    case 33: // FMOD
        as->call(&M68040_Impl::fmod);
        break;
    case 34: // FADD
        as->call(&M68040_Impl::fadd);
        break;
    case 35: // FMUL
        as->call(&M68040_Impl::fmul);
        break;
    case 36: // FSGLDIV
        as->call(&M68040_Impl::fsgldiv);
        break;
    case 37: // FREM
        as->call(&M68040_Impl::frem);
        break;
    case 38: // FSCALE
        as->call(&M68040_Impl::fscale);
        break;
    case 39: // FSGLMUL
        as->call(&M68040_Impl::fsglmul);
        break;
    case 40: // FSUB
        as->call(&M68040_Impl::fsub);
        break;
    case 48:
    case 49:
    case 50:
    case 51:
    case 52:
    case 53:
    case 54:
    case 55: {
        // FSINCOS
        int fp_c = opc & 7;
        as->mov(ARG3.r32(), fp_c);
        as->call(&M68040_Impl::fsincos);
        break;
    }
    case 56: // FCMP
        as->call(&M68040_Impl::fcmp);
        break;
    case 58: // FTST
        as->call(&M68040_Impl::ftst);
        break;
    case 64: // FSMOVE
        as->call(&M68040_Impl::fsmove);
        break;
    case 65: // FSSQRT
        as->call(&M68040_Impl::fssqrt);
        break;
    case 68: // FDMOVE
        as->call(&M68040_Impl::fdmove);
        break;
    case 69: // FDSQRT
        as->call(&M68040_Impl::fdsqrt);
        break;
    case 88: // FSABS
        as->call(&M68040_Impl::fsabs);
        break;
    case 90: // FSNEG
        as->call(&M68040_Impl::fsneg);
        break;
    case 92: // FDABS
        as->call(&M68040_Impl::fdabs);
        break;
    case 94: // FDNEG
        as->call(&M68040_Impl::fdneg);
        break;
    case 96: // FSDIV
        as->call(&M68040_Impl::fsdiv);
        break;
    case 98: // FSADD
        as->call(&M68040_Impl::fsadd);
        break;
    case 99: // FSMUL
        as->call(&M68040_Impl::fsmul);
        break;
    case 100: // FDDIV
        as->call(&M68040_Impl::fddiv);
        break;
    case 102: // FDADD
        as->call(&M68040_Impl::fdadd);
        break;
    case 103: // FDMUL
        as->call(&M68040_Impl::fdmul);
        break;
    case 104: // FSSUB
        as->call(&M68040_Impl::fssub);
        break;
    case 108: // FDSUB
        as->call(&M68040_Impl::fdsub);
        break;
    default:
        as->call(FLINE);
    }
}

void jit_fpu_store(int t, int type, int reg, int fpn, int k) {
    as->mov(ARG1, FPU_P());
    as->mov(ARG2.r32(), fpn);
    as->call(&M68040_Impl::fstore_common);
    as->mov(ARG1, FPU_P());
    as->mov(ARG2.r32(), type);
    as->mov(ARG3.r32(), reg);
    switch(FP_SIZE{t}) {
    case FP_SIZE::LONG:
        as->call(&M68040_Impl::fstore_l);
        break;
    case FP_SIZE::SINGLE:
        as->call(&M68040_Impl::fstore_s);
        break;
    case FP_SIZE::EXT:
        as->call(&M68040_Impl::fstore_x);
        break;
    case FP_SIZE::PACKED:
        as->mov(ARG4.r32(), static_cast<int8_t>(k << 1) >> 1);
        as->call(&M68040_Impl::fstore_p);
        break;
    case FP_SIZE::WORD:
        as->call(&M68040_Impl::fstore_w);
        break;
    case FP_SIZE::DOUBLE:
        as->call(&M68040_Impl::fstore_d);
        break;
    case FP_SIZE::BYTE:
        as->call(&M68040_Impl::fstore_b);
        break;
    case FP_SIZE::PACKED2:
        as->mov(ARG4.r32(), k >> 4);
        as->call(&M68040_Impl::fstore_p2);
        break;
    default:
        __builtin_unreachable();
    }
}

void jit_fmove_to_fpcc(int type, int reg, unsigned int regs) {
    switch(regs) {
    case 0: // NONE
        break;
    case 1: // IR
        ea_readL_jit(type, reg);
        as->mov(ARG2.r32(), x86::eax);
        as->mov(ARG1, FPU_P());
        as->call(&M68040_Impl::set_fpiar);
        break;
    case 2: // FPSR
        ea_readL_jit(type, reg);
        as->mov(ARG2.r32(), x86::eax);
        as->mov(ARG1, FPU_P());
        as->call(&M68040_Impl::set_fpsr);
        break;
    case 4: // FPCR
        ea_readL_jit(type, reg);
        as->mov(ARG2.r32(), x86::eax);
        as->mov(ARG1, FPU_P());
        as->call(&M68040_Impl::set_fpcr);
        break;
    default:
        ea_getaddr_jit(type, reg, 4 * std::popcount(regs));
        as->mov(x86::r12d, EA);
        if(regs & 4) {
            jit_readL(x86::r12d);
            as->mov(ARG2.r32(), x86::eax);
            as->mov(ARG1, FPU_P());
            as->call(&M68040_Impl::set_fpcr);
            as->lea(x86::r12d, x86::dword_ptr(x86::r12d, 4));
        }
        if(regs & 2) {
            jit_readL(x86::r12d);
            as->mov(ARG2.r32(), x86::eax);
            as->mov(ARG1, FPU_P());
            as->call(&M68040_Impl::set_fpsr);
            as->lea(x86::r12d, x86::dword_ptr(x86::r12d, 4));
        }
        if(regs & 1) {
            jit_readL(x86::r12d);
            as->mov(ARG2.r32(), x86::eax);
            as->mov(ARG1, FPU_P());
            as->call(&M68040_Impl::set_fpiar);
        }
        break;
    }
    jit_trace_branch();
}

void jit_fmove_from_fpcc(int type, int reg, unsigned int regs) {
    switch(regs) {
    case 0: // NONE
        break;
    case 1: // IR
        as->mov(ARG1, FPU_P());
        as->call(&M68040_Impl::get_fpiar);
        ea_writeL_jit(type, reg, false);
        break;
    case 2: // FPSR
        as->mov(ARG1, FPU_P());
        as->call(&M68040_Impl::get_fpsr);
        ea_writeL_jit(type, reg, false);
        break;
    case 4: // FPCR
        as->mov(ARG1, FPU_P());
        as->call(&M68040_Impl::get_fpcr);
        ea_writeL_jit(type, reg, false);
        break;
    default:
        ea_getaddr_jit(type, reg, 4 * std::popcount(regs));
        as->mov(x86::r12d, EA);
        if(regs & 4) {
            as->mov(ARG1, FPU_P());
            as->call(&M68040_Impl::get_fpcr);
            as->movzx(x86::eax, x86::ax);
            jit_writeL(x86::r12d, x86::eax);
            as->lea(x86::r12d, x86::dword_ptr(x86::r12d, 4));
        }
        if(regs & 2) {
            as->mov(ARG1, FPU_P());
            as->call(&M68040_Impl::get_fpsr);
            jit_writeL(x86::r12d, x86::eax);
            as->lea(x86::r12d, x86::dword_ptr(x86::r12d, 4));
        }
        if(regs & 1) {
            as->mov(ARG1, FPU_P());
            as->call(&M68040_Impl::get_fpiar);
            jit_writeL(x86::r12d, x86::eax);
        }
        break;
    }
    jit_trace_branch();
}

void update_pc();
static void fmovem_to_reg_(M68040FpuFull *fpu, uint32_t addr,
                           uint32_t reglist) {
    fpu->fmovem_to_reg(addr, reglist);
}
static void fmovem_to_reg_rev_(M68040FpuFull *fpu, uint32_t addr,
                               uint32_t reglist) {
    fpu->fmovem_from_reg_rev(addr, reglist);
}
static int testfcc_(M68040FpuFull *fpu, uint8_t v) { return fpu->test_Fcc(v); }
void fop(uint16_t op) {
    uint16_t extw = FETCH();
    bool rm = extw & 1 << 14;
    unsigned int src = extw >> 10 & 7;
    unsigned int dst = extw >> 7 & 7;
    unsigned int opc = extw & 0x7f;
    update_pc();
    as->mov(ARG1, FPU_P());
    as->call(&M68040_Impl::fpu_prologue);
    if(!(extw & 1 << 15)) {
        if(!(extw & 1 << 13)) {
            if(rm && src == 7) {
                // FMOVECR
                jit_fmovecr(opc, dst);
                return;
            }
            jit_loadFP(TYPE(op), REG(op), rm, src);
            jit_fop_do(opc, dst);
        } else {
            // FMOVE to ea
            jit_fpu_store(src, TYPE(op), REG(op), dst, opc);
        }
    } else {
        switch(extw >> 13 & 3) {
        case 0:
            // FMOVE(M) to FPcc
            jit_fmove_to_fpcc(TYPE(op), REG(op), src);
            break;
        case 1:
            jit_fmove_from_fpcc(TYPE(op), REG(op), src);
            break;
        case 2:
            if(extw & 1 << 11) {
                as->mov(x86::r13b, DR_B(extw >> 4 & 7));
            } else {
                as->mov(x86::r13b, extw & 0xff);
            }
            if(TYPE(op) == 3) {
                as->mov(ARG1, FPU_P());
                as->mov(ARG2.r32(), REG(op));
                as->mov(ARG3.r8(), x86::r13b);
                as->call(fmovem_to_reg_);
                as->mov(AR_L(REG(op)), x86::eax);
            } else {
                ea_getaddr_jit(TYPE(op), REG(op), 0);
                as->mov(ARG1, FPU_P());
                as->mov(ARG2.r32(), x86::eax);
                as->mov(ARG3.r8(), x86::r13b);
                as->call(fmovem_to_reg_);
            }
            jit_trace_branch();
            break;
        case 3:
            if(extw & 1 << 11) {
                as->mov(x86::r13b, DR_B(extw >> 4 & 7));
            } else {
                as->mov(x86::r13b, extw & 0xff);
            }
            if(!(extw & 1 << 12) && TYPE(op) == 4) {
                as->mov(ARG1, FPU_P());
                as->mov(ARG2.r32(), AR_L(REG(op)));
                as->mov(ARG3.r8(), x86::r13b);
                as->call(fmovem_to_reg_rev_);
                as->mov(AR_L(REG(op)), x86::eax);
            } else {
                ea_getaddr_jit(TYPE(op), REG(op), 0);
                as->mov(ARG1, FPU_P());
                as->mov(ARG2.r32(), x86::eax);
                as->mov(ARG3.r8(), x86::r13b);
                as->call(fmovem_to_reg_);
            }
            jit_trace_branch();
            break;
        }
    }
}
void jit_test_Fcc(uint8_t v) {
    as->mov(ARG1, FPU_P());
    as->mov(ARG2.r8(), v);
    as->call(testfcc_);
}

static void jit_test_bsun(M68040FpuFull *fpu) { fpu->test_bsun(); }
void fscc(uint16_t op) {
    uint16_t extw = FETCH();
    if(extw & 1 << 4) {
        as->mov(ARG1, FPU_P());
        as->call(jit_test_bsun);
    }
    jit_test_Fcc(extw);
    as->neg(x86::al);
    as->sbb(x86::dl, x86::dl);
    as->mov(x86::al, x86::dl);
    ea_writeB_jit(TYPE(op), REG(op), false);
}
void fsave(uint16_t op) {
    as->mov(ARG1.r8(), TYPE(op));
    as->mov(ARG2.r8(), REG(op));
    as->call(&M68040_Impl::fsave);
}

void frestore(uint16_t op) {
    as->mov(ARG1, FPU_P());
    if(TYPE(op) == 3) {
        as->mov(ARG2.r8(), REG(op));
        as->call(&M68040_Impl::frestore3);
    } else {
        as->mov(ARG2.r8(), TYPE(op));
        as->mov(ARG3.r8(), REG(op));
        as->call(&M68040_Impl::frestore);
    }
}

void fdbcc(uint16_t op) {
    auto fpu = FPU_P();
    uint16_t extw = FETCH();
    int16_t offset = FETCH();
    update_pc();
    if(extw & 1 << 4) {
        as->mov(ARG1, fpu);
        as->call(jit_test_bsun);
    }
    jit_test_Fcc(extw & 0xf);
    as->test(x86::al, x86::al);
    jit_if(COND::FALSE, [op, offset] {
        as->mov(x86::dx, DR_W(REG(op)));
        as->dec(x86::dx);
        as->mov(DR_W(REG(op)), x86::dx);
        as->cmp(x86::dx, -1);
        jit_if(COND::NE, [offset] { jumpC(cpu.PC + offset - 2); });
    });
    jit_trace_branch();
}


void ftrapcc(uint16_t op) {
    auto fpu = FPU_P();
    uint16_t extw = FETCH();
    if(REG(op) == 2) {
        FETCH();
    } else if(REG(op) == 3) {
        FETCH32();
    }
    update_pc();
    if(extw & 1 << 4) {
        as->mov(ARG1, fpu);
        as->call(jit_test_bsun);
    }
    jit_test_Fcc(extw & 0xf);
    as->test(x86::al, x86::al);
    jit_if(COND::TRUE, [] { as->call(TRAPX_ERROR); });
}

void fbcc_w(uint16_t op) {
    auto fpu = FPU_P();
    int c = op & 077;
    int16_t offset = FETCH();
    update_pc();
    if(c & 1 << 4) {
        as->mov(ARG1, fpu);
        as->call(jit_test_bsun);
    }
    jit_test_Fcc(c & 0xf);
    as->test(x86::al, x86::al);
    jit_if(COND::TRUE, [offset] { jumpC(cpu.PC + offset - 2); });
    jit_trace_branch();
}

void fbcc_l(uint16_t op) {
    auto fpu = FPU_P();
    int c = op & 077;
    int32_t offset = FETCH32();
    update_pc();
    if(c & 1 << 4) {
        as->mov(ARG1, fpu);
        as->call(jit_test_bsun);
    }
    jit_test_Fcc(c & 0xf);
    as->test(x86::al, x86::al);
    jit_if(COND::TRUE, [offset] { jumpC(cpu.PC + offset - 2); });
    jit_trace_branch();
}
#endif
} // namespace JIT_OP

void M68040FpuFull::init_jit() {
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
}
