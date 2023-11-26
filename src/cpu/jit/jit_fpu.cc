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

void load_fpS(uint32_t v);
void load_fpD(uint64_t v);
void load_fpX(uint64_t frac, uint16_t exp);
void load_fpP(uint32_t addr);

int32_t fpu_storeL();
int16_t fpu_storeW();
int8_t fpu_storeB();
uint32_t store_fpS();
uint64_t store_fpD();
std::tuple<uint64_t, uint16_t> store_fpX(int i);
void store_fpP(uint32_t addr, int k);

void fpu_testex();
bool test_NAN1();
auto FP_N(int n) {
    return x86::ptr(x86::rbx, offsetof(Cpu, FP) + sizeof(mpfr_t) * n);
}
void Set_FPCR(uint16_t v);
void Set_FPSR(uint32_t v);
uint16_t Get_FPCR();
uint32_t Get_FPSR();
uint32_t fmovem_from_reg(uint32_t base, uint8_t regs);
uint32_t fmovem_from_reg_rev(uint32_t base, uint8_t regs);
uint32_t fmovem_to_reg(uint32_t base, uint8_t regs);
namespace JIT_OP {
void jit_trace_branch();
#ifdef __x86_64__
void jit_fpu_prologue() {
    as->call(mpfr_clear_flags);
    as->mov(x86::eax, x86::dword_ptr(x86::rbx, offsetof(Cpu, oldpc)));
    as->mov(x86::dword_ptr(x86::rbx, offsetof(Cpu, FPIAR)), x86::eax);
}
void update_pc();
constexpr auto FP_TMP = x86::qword_ptr(x86::rbx, offsetof(Cpu, fp_tmp));
constexpr auto FP_RND = x86::dword_ptr(x86::rbx, offsetof(Cpu, FPCR.RND));
static void jit_mpfr_set_ui(unsigned int i) {
    as->lea(ARG1, FP_TMP);
    as->mov(ARG2.r32(), i);
    as->mov(ARG3.r32(), FP_RND);
    as->call(mpfr_set_ui);
}
static void jit_mpfr_run(int (*p)(mpfr_ptr, mpfr_srcptr, mpfr_rnd_t)) {
    as->lea(ARG1, FP_TMP);
    as->lea(ARG2, FP_TMP);
    as->mov(ARG3.r32(), FP_RND);
    as->call(p);
}

void jit_fmovecr(int opc, int fpn) {
    switch(opc) {
    case 0:
        // PI
        as->lea(ARG1, FP_TMP);
        as->mov(ARG2.r32(), FP_RND);
        as->call(mpfr_const_pi);
        break;
    case 0xB:
        // log10_2
        jit_mpfr_set_ui(2);
        jit_mpfr_run(mpfr_log10);
        break;
    case 0xC:
        // e
        jit_mpfr_set_ui(1);
        jit_mpfr_run(mpfr_exp);
        break;
    case 0xD:
        // log2_e
        jit_mpfr_set_ui(1);
        jit_mpfr_run(mpfr_exp);
        jit_mpfr_run(mpfr_log2);
        break;
    case 0xE:
        // log10_e
        jit_mpfr_set_ui(1);
        jit_mpfr_run(mpfr_exp);
        jit_mpfr_run(mpfr_log10);
        break;
    case 0x30:
        // log2_e
        as->lea(ARG1, FP_TMP);
        as->mov(ARG2.r32(), FP_RND);
        as->call(mpfr_const_log2);
        break;
    case 0x31:
        // ln(10)
        jit_mpfr_set_ui(10);
        jit_mpfr_run(mpfr_log);
        break;
    case 0x32:
        // 10^0
        as->lea(ARG1, FP_TMP);
        as->mov(ARG2.r32(), 10);
        as->mov(ARG3.r32(), 0);
        as->mov(ARG4.r32(), FP_RND);
        as->call(mpfr_ui_pow_ui);
        break;
    case 0x33:
        // 10^1
        as->lea(ARG1, FP_TMP);
        as->mov(ARG2.r32(), 10);
        as->mov(ARG3.r32(), 1);
        as->mov(ARG4.r32(), FP_RND);
        as->call(mpfr_ui_pow_ui);
        break;
    case 0x34:
        // 10^2
        as->lea(ARG1, FP_TMP);
        as->mov(ARG2.r32(), 10);
        as->mov(ARG3.r32(), 2);
        as->mov(ARG4.r32(), FP_RND);
        as->call(mpfr_ui_pow_ui);
        break;

    case 0x35:
        // 10^4
        as->lea(ARG1, FP_TMP);
        as->mov(ARG2.r32(), 10);
        as->mov(ARG3.r32(), 4);
        as->mov(ARG4.r32(), FP_RND);
        as->call(mpfr_ui_pow_ui);
        break;
    case 0x36:
        // 10^8
        as->lea(ARG1, FP_TMP);
        as->mov(ARG2.r32(), 10);
        as->mov(ARG3.r32(), 8);
        as->mov(ARG4.r32(), FP_RND);
        as->call(mpfr_ui_pow_ui);
        break;
    case 0x37:
        // 10^16
        as->lea(ARG1, FP_TMP);
        as->mov(ARG2.r32(), 10);
        as->mov(ARG3.r32(), 16);
        as->mov(ARG4.r32(), FP_RND);
        as->call(mpfr_ui_pow_ui);
        break;
    case 0x38:
        // 10^32
        as->lea(ARG1, FP_TMP);
        as->mov(ARG2.r32(), 10);
        as->mov(ARG3.r32(), 32);
        as->mov(ARG4.r32(), FP_RND);
        as->call(mpfr_ui_pow_ui);
        break;
    case 0x39:
        // 10^64
        as->lea(ARG1, FP_TMP);
        as->mov(ARG2.r32(), 10);
        as->mov(ARG3.r32(), 64);
        as->mov(ARG4.r32(), FP_RND);
        as->call(mpfr_ui_pow_ui);
        break;
    case 0x3A:
        // 10^128
        as->lea(ARG1, FP_TMP);
        as->mov(ARG2.r32(), 10);
        as->mov(ARG3.r32(), 128);
        as->mov(ARG4.r32(), FP_RND);
        as->call(mpfr_ui_pow_ui);
        break;
    case 0x3B:
        // 10^256
        as->lea(ARG1, FP_TMP);
        as->mov(ARG2.r32(), 10);
        as->mov(ARG3.r32(), 256);
        as->mov(ARG4.r32(), FP_RND);
        as->call(mpfr_ui_pow_ui);
        break;
    case 0x3C:
        // 10^512
        as->lea(ARG1, FP_TMP);
        as->mov(ARG2.r32(), 10);
        as->mov(ARG3.r32(), 512);
        as->mov(ARG4.r32(), FP_RND);
        as->call(mpfr_ui_pow_ui);
        break;
    case 0x3D:
        // 10^1024
        as->lea(ARG1, FP_TMP);
        as->mov(ARG2.r32(), 10);
        as->mov(ARG3.r32(), 1024);
        as->mov(ARG4.r32(), FP_RND);
        as->call(mpfr_ui_pow_ui);
        break;
    case 0x3E:
        // 10^2048
        as->lea(ARG1, FP_TMP);
        as->mov(ARG2.r32(), 10);
        as->mov(ARG3.r32(), 2048);
        as->mov(ARG4.r32(), FP_RND);
        as->call(mpfr_ui_pow_ui);
        break;
    case 0x3F:
        // 10^4096
        as->lea(ARG1, FP_TMP);
        as->mov(ARG2.r32(), 10);
        as->mov(ARG3.r32(), 4096);
        as->mov(ARG4.r32(), FP_RND);
        as->call(mpfr_ui_pow_ui);
        break;
    case 0x0F:
    default:
        as->lea(ARG1, FP_TMP);
        as->mov(ARG2.r32(), 1);
        as->call(mpfr_set_zero);

        break;
    }
    as->mov(ARG1.r32(), fpn);
    as->mov(ARG2.r32(), (int)FPU_PREC::AUTO);
    as->call(store_fpr);
}
enum class FP_SIZE { LONG, SINGLE, EXT, PACKED, WORD, DOUBLE, BYTE, PACKED2 };

void jit_loadFP(int type, int reg, bool rm, int src) {
    if(rm) {
        switch(FP_SIZE(src)) {
        case FP_SIZE::LONG:
            ea_readL_jit(type, reg);
            as->lea(ARG1, FP_TMP);
            as->movsxd(ARG2, x86::eax);
            as->mov(ARG3.r32(), FP_RND);
            as->call(mpfr_set_si);
            break;
        case FP_SIZE::SINGLE:
            ea_readL_jit(type, reg);
            as->mov(ARG1.r32(), x86::eax);
            as->call(load_fpS);
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

            jit_readW(EA);
            as->mov(x86::r13w, x86::ax);

            as->mov(x86::esi, EA);
            as->add(x86::esi, 4);
            jit_readL(x86::esi);
            as->mov(x86::r14d, x86::eax);
            as->ror(x86::r14, 32);

            as->mov(x86::esi, EA);
            as->add(x86::esi, 8);
            jit_readL(x86::esi);
            as->mov(x86::eax, x86::eax);
            as->or_(x86::r14, x86::rax);

            as->mov(ARG1, x86::r14);
            as->mov(ARG2.r16(), x86::r13w);
            as->call(load_fpX);
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
            as->mov(ARG1.r32(), EA);
            as->call(load_fpP);
            break;
        case FP_SIZE::WORD:
            ea_readW_jit(type, reg);
            as->lea(ARG1, FP_TMP);
            as->movsx(ARG2, x86::ax);
            as->mov(ARG3.r32(), FP_RND);
            as->call(mpfr_set_si);
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
            as->mov(x86::r13w, x86::ax);

            jit_readL(EA);
            as->mov(x86::r14d, x86::eax);
            as->ror(x86::r14, 32);

            as->mov(x86::esi, EA);
            as->add(x86::esi, 4);
            jit_readL(x86::esi);

            as->or_(x86::r14, x86::rax);
            as->mov(ARG1, x86::r14);
            as->call(load_fpD);

            break;
        }
        case FP_SIZE::BYTE:
            ea_readB_jit(type, reg);
            as->lea(ARG1, FP_TMP);
            as->movsx(ARG2, x86::al);
            as->mov(ARG3.r32(), FP_RND);
            as->call(mpfr_set_si);
            break;
        case FP_SIZE::PACKED2:
            __builtin_unreachable();
        }
    } else {
        as->lea(ARG1, FP_N(src));
        as->lea(ARG2, FP_TMP);
        as->call(mpfr_swap);
        as->mov(x86::rax, x86::qword_ptr(x86::rbx, offsetof(Cpu, FP_nan) +
                                                       sizeof(int64_t) * src));
        as->xchg(x86::rax, x86::qword_ptr(x86::rbx, offsetof(Cpu, fp_tmp_nan)));
        as->xchg(x86::rax, x86::qword_ptr(x86::rbx, offsetof(Cpu, FP_nan) +
                                                        sizeof(int64_t) * src));
    }
}
constexpr auto TMP_TV = x86::dword_ptr(x86::rbx, offsetof(Cpu, fp_tmp_tv));

static void jit_test_nan1(Label &f) {
    auto t = as->newLabel();
    as->lea(x86::rsi, FP_TMP);
    as->mov(x86::rax,
            x86::qword_ptr(x86::rsi, offsetof(__mpfr_struct, _mpfr_exp)));
    as->mov(x86::rdx, __MPFR_EXP_NAN);
    as->cmp(x86::rax, x86::rdx);
    as->jne(t);
    as->mov(x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.OPERR)), 1);
    as->jmp(f);
    as->bind(t);
}
constexpr uint64_t QNAN_DEFAULT = ~0LLU;

static void jit_test_nan2(Label &f, int fpn) {
    auto lb1 = as->newLabel();
    auto lb2 = as->newLabel();
    as->lea(x86::rsi, FP_N(fpn));
    as->mov(x86::rax,
            x86::qword_ptr(x86::rsi, offsetof(__mpfr_struct, _mpfr_exp)));
    as->mov(x86::rdx, __MPFR_EXP_NAN);
    as->cmp(x86::rax, x86::rdx);
    as->sete(x86::cl);

    as->lea(x86::rsi, FP_TMP);
    as->mov(x86::rax,
            x86::qword_ptr(x86::rsi, offsetof(__mpfr_struct, _mpfr_exp)));
    as->mov(x86::rdx, __MPFR_EXP_NAN);
    as->cmp(x86::rax, x86::rdx);
    as->setne(x86::ch);
    as->and_(x86::cl, x86::ch);

    as->je(lb1);

    as->lea(ARG1, FP_TMP);
    as->call(mpfr_set_nan);
    as->call(mpfr_set_nanflag);
    as->mov(x86::rax, x86::qword_ptr(x86::rbx, offsetof(Cpu, FP_nan) +
                                                   fpn * sizeof(uint64_t)));
    as->mov(x86::qword_ptr(x86::rbx, offsetof(Cpu, fp_tmp_nan)), x86::rax);
    as->jmp(f);

    as->bind(lb1);
    as->test(x86::ch, x86::ch);
    as->jne(lb2);

    as->lea(x86::rsi, FP_TMP);
    as->mov(x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.OPERR)), 1);
    as->jmp(f);
    as->bind(lb2);
}

void jit_set_nan() {
    as->lea(ARG1, FP_TMP);
    as->call(mpfr_set_nan);
    as->call(mpfr_set_nanflag);
    as->mov(x86::rax, QNAN_DEFAULT);
    as->mov(FP_TMP, x86::rax);
}

static void jit_fmod(int dst, int (*f)(mpfr_t, long *, mpfr_srcptr, mpfr_srcptr,
                                       mpfr_rnd_t)) {
    long q;
    cpu.fp_tmp_tv = f(cpu.fp_tmp, &q, cpu.fp_tmp, cpu.FP[dst], cpu.FPCR.RND);
    cpu.FPSR.Quat = std::abs(q) & 0x7f;
    cpu.FPSR.QuatSign = q < 0;
}

static int jit_fcmp_n(mpfr_ptr dst, mpfr_ptr tmp) {
    if((mpfr_zero_p(dst) && mpfr_zero_p(tmp)) ||
       (mpfr_inf_p(dst) && mpfr_inf_p(tmp))) {
        return mpfr_signbit(dst);
    } else {
        return mpfr_less_p(dst, tmp);
    }
}
void jit_fop_do(int opc, int dst) {
    FPU_PREC prec = FPU_PREC::X;
    auto isNan = as->newLabel();
    switch(opc) {
    case 0: // FMOVE
        break;
    case 1: // FINT
        jit_test_nan1(isNan);
        jit_mpfr_run(mpfr_rint);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        break;
    case 2: // FSINH
        jit_test_nan1(isNan);
        jit_mpfr_run(mpfr_sinh);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        break;
    case 3: // FINTRZ
        jit_test_nan1(isNan);
        as->lea(ARG1, FP_TMP);
        as->lea(ARG2, FP_TMP);
        as->call(mpfr_trunc);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        break;
    case 4:
        // FSQRT
        jit_test_nan1(isNan);
        jit_mpfr_run(mpfr_sqrt);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        break;
    case 6:
        // FLOGNP1
        jit_test_nan1(isNan);
        jit_mpfr_run(mpfr_log1p);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        break;
    case 8:
        // FETOXM1
        jit_test_nan1(isNan);
        jit_mpfr_run(mpfr_expm1);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        break;
    case 9:
        // FTANH
        jit_test_nan1(isNan);
        jit_mpfr_run(mpfr_tanh);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        break;
    case 10:
        // FATAN
        jit_test_nan1(isNan);
        jit_mpfr_run(mpfr_atan);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        break;
    case 12:
        // FASIN
        jit_test_nan1(isNan);
        jit_mpfr_run(mpfr_asin);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        break;
    case 13:
        // FATANH
        jit_test_nan1(isNan);
        jit_mpfr_run(mpfr_atanh);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        break;
    case 14:
        // FSIN
        jit_test_nan1(isNan);
        jit_mpfr_run(mpfr_sin);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        break;
    case 15:
        // FTAN
        jit_test_nan1(isNan);
        jit_mpfr_run(mpfr_tan);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        break;
    case 16:
        // FETOX
        jit_test_nan1(isNan);
        jit_mpfr_run(mpfr_exp);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        break;
    case 17:
        // FTWOTOX
        jit_test_nan1(isNan);
        jit_mpfr_run(mpfr_exp2);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        break;
    case 18:
        // FTENTOX
        jit_test_nan1(isNan);
        jit_mpfr_run(mpfr_exp10);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        break;
    case 20:
        // FLOGN
        jit_test_nan1(isNan);
        jit_mpfr_run(mpfr_log);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        break;
    case 21:
        // FLOG10
        jit_test_nan1(isNan);
        jit_mpfr_run(mpfr_log10);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        break;
    case 22:
        // FLOG2
        jit_test_nan1(isNan);
        jit_mpfr_run(mpfr_log2);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        break;
    case 24:
        // FABS
        jit_test_nan1(isNan);
        jit_mpfr_run(mpfr_abs);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        break;
    case 25:
        // FCOSH
        jit_test_nan1(isNan);
        jit_mpfr_run(mpfr_cosh);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        break;
    case 26:
        // FNEG
        jit_test_nan1(isNan);
        jit_mpfr_run(mpfr_neg);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        break;
    case 28:
        // FACOS
        jit_test_nan1(isNan);
        jit_mpfr_run(mpfr_acos);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        break;
    case 29:
        // FCOS
        jit_test_nan1(isNan);
        jit_mpfr_run(mpfr_cos);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        break;
    case 30:
        // FGETEXP
        {
            auto lb2 = as->newLabel();
            jit_test_nan1(isNan);
            as->lea(x86::rsi, FP_TMP);
            as->mov(x86::rax, x86::qword_ptr(x86::rsi, offsetof(__mpfr_struct,
                                                                _mpfr_exp)));
            as->mov(x86::rdx, __MPFR_EXP_INF);
            as->cmp(x86::rax, x86::rdx);
            as->jne(lb2);
            jit_set_nan();
            as->jmp(isNan);
            as->bind(lb2);
            as->mov(x86::rdx, __MPFR_EXP_ZERO);
            as->cmp(x86::rax, x86::rdx);
            as->je(isNan);
            as->lea(ARG1, x86::qword_ptr(x86::rsp, 16));
            as->lea(ARG2, FP_TMP);
            as->mov(ARG3.r32(), FP_RND);
            as->call(mpfr_get_d_2exp);
            as->dec(x86::qword_ptr(x86::rsp, 16));
            // the result of mpfr_get_d_2exp  is
            // [0.5, 1.0)
            as->lea(ARG1, FP_TMP);
            as->mov(ARG2, x86::qword_ptr(x86::rsp, 16));
            as->mov(ARG3.r32(), FP_RND);
            as->call(mpfr_set_si);
            as->bind(isNan);
            break;
        }
    case 31: {
        // FGETMAN
        auto lb2 = as->newLabel();
        jit_test_nan1(isNan);
        as->lea(x86::rsi, FP_TMP);
        as->mov(x86::rax,
                x86::qword_ptr(x86::rsi, offsetof(__mpfr_struct, _mpfr_exp)));
        as->mov(x86::rdx, __MPFR_EXP_INF);
        as->cmp(x86::rax, x86::rdx);
        as->jne(lb2);
        jit_set_nan();
        as->jmp(isNan);
        as->bind(lb2);
        as->lea(ARG1, x86::qword_ptr(x86::rsp, 16));
        as->lea(ARG2, FP_TMP);
        as->lea(ARG3, FP_TMP);
        as->mov(ARG4.r32(), FP_RND);
        as->call(mpfr_frexp);
        as->mov(TMP_TV, x86::eax);
        // the result of mpfr_frexp  is
        // [0.5, 1.0)
        as->lea(ARG1, FP_TMP);
        as->lea(ARG2, FP_TMP);
        as->mov(ARG3.r32(), 2);
        as->mov(ARG4.r32(), FP_RND);
        as->call(mpfr_mul_si);
        as->bind(isNan);
        break;
    }
    case 32:
        // FDIV
        jit_test_nan2(isNan, dst);
        as->lea(ARG1, FP_TMP);
        as->lea(ARG2, FP_TMP);
        as->lea(ARG3, FP_N(dst));
        as->mov(ARG4.r32(), FP_RND);
        as->call(mpfr_div);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        break;
    case 33:
        // FMOD
        jit_test_nan2(isNan, dst);
        as->mov(ARG1.r32(), dst);
        as->mov(ARG2, (intptr_t)mpfr_fmodquo);
        as->call(jit_fmod);
        break;
    case 34:
        // FADD
        jit_test_nan2(isNan, dst);
        as->lea(ARG1, FP_TMP);
        as->lea(ARG2, FP_TMP);
        as->lea(ARG3, FP_N(dst));
        as->mov(ARG4.r32(), FP_RND);
        as->call(mpfr_add);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        break;
    case 35:
        // FMUL
        jit_test_nan2(isNan, dst);
        as->lea(ARG1, FP_TMP);
        as->lea(ARG2, FP_TMP);
        as->lea(ARG3, FP_N(dst));
        as->mov(ARG4.r32(), FP_RND);
        as->call(mpfr_mul);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        break;
    case 36:
        // FSGLDIV
        jit_test_nan2(isNan, dst);
        as->lea(ARG1, FP_TMP);
        as->mov(ARG2.r32(), 23);
        as->mov(ARG3.r32(), FP_RND);
        as->call(mpfr_prec_round);
        as->lea(ARG1, FP_N(dst));
        as->mov(ARG2.r32(), 23);
        as->mov(ARG3.r32(), FP_RND);
        as->call(mpfr_prec_round);

        as->lea(ARG1, FP_TMP);
        as->lea(ARG2, FP_TMP);
        as->lea(ARG3, FP_N(dst));
        as->mov(ARG4.r32(), FP_RND);
        as->call(mpfr_div);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);

        prec = FPU_PREC::S;
        break;
    case 37:
        // FREM
        jit_test_nan2(isNan, dst);
        as->mov(ARG1.r32(), dst);
        as->mov(ARG2, (intptr_t)mpfr_remquo);
        as->call(jit_fmod);
        break;
    case 38: {
        // FSCALE
        auto lb = as->newLabel();
        jit_test_nan2(isNan, dst);
        as->lea(x86::rsi, FP_TMP);
        as->mov(x86::rax,
                x86::qword_ptr(x86::rsi, offsetof(__mpfr_struct, _mpfr_exp)));
        as->mov(x86::rdx, __MPFR_EXP_INF);
        as->cmp(x86::rax, x86::rdx);
        as->jne(lb);
        jit_set_nan();
        as->jmp(isNan);
        as->bind(lb);
        as->lea(ARG1, FP_TMP);
        as->mov(ARG2.r32(), FP_RND);
        as->call(mpfr_get_si);
        as->lea(ARG1, FP_TMP);
        as->lea(ARG2, FP_N(dst));
        as->mov(ARG3, x86::rax);
        as->mov(ARG4.r32(), FP_RND);
        as->call(mpfr_mul_2si);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        break;
    }
    case 39:
        // FSGLMUL
        jit_test_nan2(isNan, dst);
        as->lea(ARG1, FP_TMP);
        as->mov(ARG2.r32(), 23);
        as->mov(ARG3.r32(), FP_RND);
        as->call(mpfr_prec_round);
        as->lea(ARG1, FP_N(dst));
        as->mov(ARG2.r32(), 23);
        as->mov(ARG3.r32(), FP_RND);
        as->call(mpfr_prec_round);

        as->lea(ARG1, FP_TMP);
        as->lea(ARG2, FP_TMP);
        as->lea(ARG3, FP_N(dst));
        as->mov(ARG4.r32(), FP_RND);
        as->call(mpfr_mul);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);

        prec = FPU_PREC::S;
        break;
    case 40:
        // FSUB
        jit_test_nan2(isNan, dst);
        as->lea(ARG1, FP_TMP);
        as->lea(ARG2, FP_TMP);
        as->lea(ARG3, FP_N(dst));
        as->mov(ARG4.r32(), FP_RND);
        as->call(mpfr_sub);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
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
        jit_test_nan1(isNan);
        if(fp_c == dst) {
            as->lea(ARG1, FP_TMP);
            as->lea(ARG2, FP_TMP);
            as->mov(ARG3.r32(), FP_RND);
            as->call(mpfr_sin);
        } else {
            as->lea(ARG1, FP_TMP);
            as->lea(ARG2, FP_N(fp_c));
            as->lea(ARG3, FP_TMP);
            as->mov(ARG4.r32(), FP_RND);
            as->call(mpfr_sin_cos);
        }
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        break;
    }
    case 56: {
        // FCMP
        auto lb = as->newLabel();
        as->lea(x86::r14, FP_N(dst));
        as->lea(x86::r15, FP_TMP);
        as->mov(x86::rsi,
                x86::qword_ptr(x86::r14, offsetof(__mpfr_struct, _mpfr_exp)));
        as->mov(x86::rdi,
                x86::qword_ptr(x86::r15, offsetof(__mpfr_struct, _mpfr_exp)));
        as->mov(x86::rcx, __MPFR_EXP_NAN);
        as->cmp(x86::rsi, x86::rcx);
        as->sete(x86::al);
        as->cmp(x86::rdi, x86::rcx);
        as->sete(x86::ah);
        as->or_(x86::al, x86::ah);
        as->mov(x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_NAN)), x86::al);
        as->jz(lb);
        as->jmp(isNan);

        as->bind(lb);

        as->mov(ARG1, x86::r14);
        as->mov(ARG2, x86::r15);
        as->call(mpfr_equal_p);
        as->mov(x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_Z)), x86::al);

        as->mov(ARG1, x86::r14);
        as->mov(ARG2, x86::r15);
        as->call(jit_fcmp_n);
        as->mov(x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_N)), x86::al);

        as->bind(isNan);
        as->call(fpu_testex);
        return;
    }
    case 58:
        // FTST
        as->lea(x86::r15, FP_TMP);
        as->mov(x86::rsi,
                x86::qword_ptr(x86::r15, offsetof(__mpfr_struct, _mpfr_exp)));
        as->mov(x86::rcx, __MPFR_EXP_NAN);
        as->cmp(x86::rsi, x86::rcx);
        as->sete(x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_NAN)));
        as->mov(x86::rcx, __MPFR_EXP_INF);
        as->cmp(x86::rsi, x86::rcx);
        as->sete(x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_I)));
        as->mov(x86::rcx, __MPFR_EXP_ZERO);
        as->cmp(x86::rsi, x86::rcx);
        as->sete(x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_Z)));
        as->mov(x86::esi,
                x86::dword_ptr(x86::r15, offsetof(__mpfr_struct, _mpfr_sign)));
        as->cmp(x86::esi, 0);
        as->setl(x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_N)));
        as->call(fpu_testex);
        return;
    case 64:
        // FSMOVE
        prec = FPU_PREC::S;
        break;
    case 65:
        // FSSQRT
        jit_test_nan1(isNan);
        jit_mpfr_run(mpfr_sqrt);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        prec = FPU_PREC::S;
        break;
    case 68:
        // FDMOVE
        prec = FPU_PREC::D;
        break;
    case 69:
        // FDSQRT
        jit_test_nan1(isNan);
        jit_mpfr_run(mpfr_sqrt);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        prec = FPU_PREC::D;
        break;
    case 88:
        // FSABS
        jit_test_nan1(isNan);
        jit_mpfr_run(mpfr_abs);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        prec = FPU_PREC::S;
        break;
    case 90:
        // FSNEG
        jit_test_nan1(isNan);
        jit_mpfr_run(mpfr_neg);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        prec = FPU_PREC::S;
        break;
    case 92:
        // FDABS
        jit_test_nan1(isNan);
        jit_mpfr_run(mpfr_abs);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        prec = FPU_PREC::D;
        break;
    case 94:
        // FDNEG
        jit_test_nan1(isNan);
        jit_mpfr_run(mpfr_neg);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        prec = FPU_PREC::D;
        break;
    case 96:
        // FSDIV
        jit_test_nan2(isNan, dst);
        as->lea(ARG1, FP_TMP);
        as->lea(ARG2, FP_TMP);
        as->lea(ARG3, FP_N(dst));
        as->mov(ARG4.r32(), FP_RND);
        as->call(mpfr_div);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        prec = FPU_PREC::S;
        break;
    case 98:
        // FSADD
        jit_test_nan2(isNan, dst);
        as->lea(ARG1, FP_TMP);
        as->lea(ARG2, FP_TMP);
        as->lea(ARG3, FP_N(dst));
        as->mov(ARG4.r32(), FP_RND);
        as->call(mpfr_add);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        prec = FPU_PREC::S;
        break;
    case 99:
        // FSMUL
        jit_test_nan2(isNan, dst);
        as->lea(ARG1, FP_TMP);
        as->lea(ARG2, FP_TMP);
        as->lea(ARG3, FP_N(dst));
        as->mov(ARG4.r32(), FP_RND);
        as->call(mpfr_mul);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        prec = FPU_PREC::S;
        break;
    case 100:
        // FDDIV
        jit_test_nan2(isNan, dst);
        as->lea(ARG1, FP_TMP);
        as->lea(ARG2, FP_TMP);
        as->lea(ARG3, FP_N(dst));
        as->mov(ARG4.r32(), FP_RND);
        as->call(mpfr_div);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        prec = FPU_PREC::D;
        break;
    case 102:
        // FDADD
        jit_test_nan2(isNan, dst);
        as->lea(ARG1, FP_TMP);
        as->lea(ARG2, FP_TMP);
        as->lea(ARG3, FP_N(dst));
        as->mov(ARG4.r32(), FP_RND);
        as->call(mpfr_add);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        prec = FPU_PREC::D;
        break;
    case 103:
        // FDMUL
        jit_test_nan2(isNan, dst);
        as->lea(ARG1, FP_TMP);
        as->lea(ARG2, FP_TMP);
        as->lea(ARG3, FP_N(dst));
        as->mov(ARG4.r32(), FP_RND);
        as->call(mpfr_mul);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        prec = FPU_PREC::D;
        break;

    case 104:
        // FSSUB
        jit_test_nan2(isNan, dst);
        as->lea(ARG1, FP_TMP);
        as->lea(ARG2, FP_TMP);
        as->lea(ARG3, FP_N(dst));
        as->mov(ARG4.r32(), FP_RND);
        as->call(mpfr_sub);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        prec = FPU_PREC::S;
        break;
    case 108:
        // FDSUB
        jit_test_nan2(isNan, dst);
        as->lea(ARG1, FP_TMP);
        as->lea(ARG2, FP_TMP);
        as->lea(ARG3, FP_N(dst));
        as->mov(ARG4.r32(), FP_RND);
        as->call(mpfr_sub);
        as->mov(TMP_TV, x86::eax);
        as->bind(isNan);
        prec = FPU_PREC::D;
        break;
    default:
        as->call(FLINE);
    }
    as->mov(ARG1.r32(), dst);
    as->mov(ARG2.r32(), int(prec));
    as->call(store_fpr);
}

void jit_store_fpX(uint32_t addr, int fpn) {
    auto [frac, exp] = store_fpX(fpn);
    WriteW(addr, exp);
    WriteL(addr + 4, frac >> 32);
    WriteL(addr + 8, frac);
}
void jit_fpu_store(int t, int type, int reg, int fpn, int k) {
    as->lea(ARG1, FP_TMP);
    as->lea(ARG2, FP_N(fpn));
    as->mov(ARG3.r32(), FP_RND);
    as->call(mpfr_set);

    as->mov(x86::rax, x86::qword_ptr(x86::rbx, offsetof(Cpu, FP_nan) +
                                                   sizeof(uint64_t) * fpn));
    as->mov(x86::qword_ptr(x86::rbx, offsetof(Cpu, fp_tmp_nan)), x86::rax);

    switch(FP_SIZE{t}) {
    case FP_SIZE::LONG:
        as->call(fpu_storeL);
        ea_writeL_jit(type, reg, false);
        break;
    case FP_SIZE::SINGLE:
        as->call(store_fpS);
        ea_writeL_jit(type, reg, false);
        break;
    case FP_SIZE::EXT: {
        ea_getaddr_jit(type, reg, 12);
        as->mov(ARG2.r32(), fpn);
        as->call(jit_store_fpX);
        break;
    }
    case FP_SIZE::PACKED: {
        ea_getaddr_jit(type, reg, 12);
        as->mov(ARG2.r32(), static_cast<int8_t>(k << 1) >> 1);
        as->call(store_fpP);
        break;
    }
    case FP_SIZE::WORD:
        as->call(fpu_storeW);
        ea_writeW_jit(type, reg, false);
        break;
    case FP_SIZE::DOUBLE:
        ea_getaddr_jit(type, reg, 8);
        as->call(store_fpD);
        as->mov(x86::r13, x86::rax);
        as->mov(ARG2, x86::r13);
        as->ror(ARG2, 32);
        jit_writeL(EA, ARG2.r32());
        as->mov(x86::esi, EA);
        as->add(x86::esi, 4);
        jit_writeL(x86::esi, x86::r13d);
        break;
    case FP_SIZE::BYTE:
        as->call(fpu_storeB);
        ea_writeB_jit(type, reg, false);
        break;
    case FP_SIZE::PACKED2:
        ea_getaddr_jit(type, reg, 12);
        as->movsx(ARG2.r32(), DR_B(k >> 4));
        as->call(store_fpP);
        break;
    default:
        __builtin_unreachable();
    }
    as->call(fpu_testex);
}

void jit_fmove_to_fpcc(int type, int reg, unsigned int regs) {
    switch(regs) {
    case 0: // NONE
        break;
    case 1: // IR
        ea_readL_jit(type, reg);
        as->mov(x86::dword_ptr(x86::rbx, offsetof(Cpu, FPIAR)), x86::eax);
        break;
    case 2: // FPSR
        ea_readL_jit(type, reg);
        as->mov(ARG1.r32(), x86::eax);
        as->call(Set_FPSR);
        break;
    case 4: // FPCR
        ea_readL_jit(type, reg);
        as->mov(ARG1.r16(), x86::ax);
        as->call(Set_FPCR);
        break;
    default:
        ea_getaddr_jit(type, reg, 4 * std::popcount(regs));
        as->mov(x86::r12d, EA);
        if(regs & 4) {
            jit_readL(x86::r12d);
            as->mov(ARG1.r16(), x86::ax);
            as->call(Set_FPCR);
            as->add(x86::r12d, 4);
        }
        if(regs & 2) {
            jit_readL(x86::r12d);
            as->mov(ARG1.r32(), x86::eax);
            as->call(Set_FPSR);
            as->add(x86::r12d, 4);
        }
        if(regs & 1) {
            jit_readL(x86::r12d);
            as->mov(x86::dword_ptr(x86::rbx, offsetof(Cpu, FPIAR)), x86::eax);
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
        as->mov(x86::eax, x86::dword_ptr(x86::rbx, offsetof(Cpu, FPIAR)));
        ea_writeL_jit(type, reg, false);
        break;
    case 2: // FPSR
        as->call(Get_FPSR);
        ea_writeL_jit(type, reg, false);
        break;
    case 4: // FPCR
        as->call(Get_FPCR);
        as->movzx(x86::eax, x86::ax);
        ea_writeL_jit(type, reg, false);
        break;
    default:
        ea_getaddr_jit(type, reg, 4 * std::popcount(regs));
        as->mov(x86::r12d, EA);
        if(regs & 4) {
            as->call(Get_FPCR);
            as->movzx(x86::eax, x86::ax);
            jit_writeL(x86::r12d, x86::eax);
            as->add(x86::r12d, 4);
        }
        if(regs & 2) {
            as->call(Get_FPSR);
            jit_writeL(x86::r12d, x86::eax);
            as->add(x86::r12d, 4);
        }
        if(regs & 1) {
            jit_writeL(x86::r12d,
                       x86::dword_ptr(x86::rbx, offsetof(Cpu, FPIAR)));
        }
        break;
    }
    jit_trace_branch();
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
                as->mov(ARG1.r32(), AR_L(REG(op)));
                as->mov(ARG2.r8(), x86::r13b);
                as->call(fmovem_to_reg);
                as->mov(AR_L(REG(op)), x86::eax);
            } else {
                ea_getaddr_jit(TYPE(op), REG(op), 0);
                as->mov(ARG2.r8(), x86::r13b);
                as->call(fmovem_to_reg);
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
                as->mov(ARG1.r32(), AR_L(REG(op)));
                as->mov(ARG2.r8(), x86::r13b);
                as->call(fmovem_from_reg_rev);
                as->mov(AR_L(REG(op)), x86::eax);
            } else {
                ea_getaddr_jit(TYPE(op), REG(op), 0);
                as->mov(ARG2.r8(), x86::r13b);
                as->call(fmovem_from_reg);
            }
            jit_trace_branch();
            break;
        }
    }
}

void jit_test_Fcc(uint8_t v) {
    switch(v) {
    // (S)F
    case 0B0000:
        as->xor_(x86::al, x86::al);
        break;
    // (S)T
    case 0B1111:
        as->mov(x86::al, 1);
        break;
    case 0B0001:
        // (S)EQ
        as->mov(x86::al, x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_Z)));
        as->test(x86::al, x86::al);
        break;
    case 0B1110:
        // (S)NE
        as->mov(x86::al, x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_Z)));
        as->test(x86::al, x86::al);
        as->sete(x86::al);
        break;
    case 0B0010:
        // (O)GT
        as->mov(x86::al, x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_NAN)));
        as->or_(x86::al, x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_Z)));
        as->or_(x86::al, x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_N)));
        as->sete(x86::al);
        break;
    case 0B1101:
        // NGT/ULE
        as->mov(x86::al, x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_NAN)));
        as->or_(x86::al, x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_Z)));
        as->or_(x86::al, x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_N)));
        break;
    case 0B0011:
        // (O)GE
        as->mov(x86::al, x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_Z)));
        as->mov(x86::dl, x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_NAN)));
        as->or_(x86::dl, x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_N)));
        as->sete(x86::dl);
        as->or_(x86::al, x86::dl);
        break;
    case 0B1100:
        // NGE/ULT
        as->mov(x86::al, x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_NAN)));
        as->mov(x86::dl, x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_N)));
        as->mov(x86::cl, x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_Z)));
        as->test(x86::cl, x86::cl);
        as->sete(x86::cl);
        as->and_(x86::cl, x86::dl);
        as->or_(x86::al, x86::cl);
        break;
    case 0B0100:
        // (O)LT
        as->mov(x86::al, x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_N)));
        as->mov(x86::dl, x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_NAN)));
        as->or_(x86::dl, x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_Z)));
        as->sete(x86::dl);
        as->and_(x86::al, x86::dl);
        break;
    case 0B1011:
        // NLT/UGE
        as->mov(x86::al, x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_NAN)));
        as->mov(x86::dl, x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_Z)));
        as->mov(x86::cl, x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_N)));
        as->test(x86::cl, x86::cl);
        as->sete(x86::cl);
        as->and_(x86::dl, x86::cl);
        as->or_(x86::al, x86::dl);
        break;
    case 0B0101:
        // (O)LE
        as->mov(x86::al, x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_Z)));
        as->mov(x86::dl, x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_N)));
        as->mov(x86::cl, x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_NAN)));
        as->test(x86::cl, x86::cl);
        as->sete(x86::cl);
        as->and_(x86::dl, x86::cl);
        as->or_(x86::al, x86::dl);
        break;
    case 0B1010:
        // NLE/UGT
        as->mov(x86::al, x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_NAN)));
        as->mov(x86::dl, x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_Z)));
        as->mov(x86::cl, x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_N)));
        as->test(x86::cl, x86::cl);
        as->sete(x86::cl);
        as->and_(x86::dl, x86::cl);
        as->or_(x86::al, x86::dl);
        break;
    case 0B0110:
        // O(GL)
        as->mov(x86::al, x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_NAN)));
        as->mov(x86::dl, x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_Z)));
        as->or_(x86::al, x86::dl);
        as->sete(x86::al);
        break;
    case 0B1001:
        // NGL/UEQ
        as->mov(x86::al, x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_NAN)));
        as->mov(x86::dl, x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_Z)));
        as->or_(x86::al, x86::dl);
        break;
    case 0B0111:
        // GLE/OR
        as->mov(x86::al, x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_NAN)));
        as->test(x86::al, x86::al);
        as->sete(x86::al);
        break;
    case 0B1000:
        // NGL/UN
        as->mov(x86::al, x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_NAN)));
        break;
    }
    return;
}

void jit_test_bsun() {
    auto lb = as->newLabel();
    as->mov(x86::al, x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.CC_NAN)));
    as->test(x86::al, x86::al);
    as->je(lb);
    as->mov(x86::byte_ptr(x86::rbx, offsetof(Cpu, FPSR.BSUN)), 1);
    as->mov(x86::al, x86::byte_ptr(x86::rbx, offsetof(Cpu, FPCR.BSUN)));
    as->test(x86::al, x86::al);
    as->je(lb);
    as->call(FP_EX_BSUN);
    as->bind(lb);
}

void fscc(uint16_t op) {
    uint16_t extw = FETCH();
    if(extw & 1 << 4) {
        jit_test_bsun();
    }
    jit_test_Fcc(extw & 0xf);
    as->neg(x86::al);
    as->sbb(x86::dl, x86::dl);
    as->mov(x86::al, x86::dl);
    ea_writeB_jit(TYPE(op), REG(op), false);
}
#endif
} // namespace JIT_OP

void init_jit_table_fpu() {
    for(int i = 0; i < 074; ++i) {
        jit_compile_op[0171000 | i] = JIT_OP::fop;
        jit_compile_op[0171100 | i] = JIT_OP::fscc;
        //        jit_compile_op[0171400 | i] = JIT_OP::fsave;
        //        jit_compile_op[0171500 | i] = JIT_OP::frestore;
    }
#if 0
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
