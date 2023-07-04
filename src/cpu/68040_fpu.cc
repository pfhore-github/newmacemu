#include "68040.hpp"
#include "bus.hpp"
#include "exception.hpp"
#include "gmp.h"
#include "proto.hpp"
#include "memory.hpp"
#include "mpfr.h"
#include <memory>
#include <string.h>
#include <utility>
constexpr uint64_t QNAN_DEFAULT = ~0LLU;
void reset_fpu();
void init_fpu() {
    for(int i = 0; i < 8; ++i) {
        mpfr_init2(cpu.FP[i], 64);
    }
    mpfr_init2(cpu.fp_tmp, 64);
    mpfr_set_default_prec(64);
    cpu.fp_tmp_nan = 0;
    cpu.FPCR.RND = MPFR_RNDN;
}

inline uint64_t loadLL(uint32_t addr) {
    uint64_t v0 = MMU_ReadL(addr);
    v0 <<= 32;
    v0 |= MMU_ReadL(addr + 4);
    return v0;
}
inline void WriteLL(uint32_t addr, uint64_t v) {
    MMU_WriteL(addr, v >> 32);
    MMU_WriteL(addr + 4, v);
}
void load_fpS(uint32_t v) {
    uint16_t exp = v >> 23 & 0xff;
    uint32_t frac = v & 0x7FFFFF;
    // test NAN
    if(exp == 0xff && frac) {
        mpfr_set_nan(cpu.fp_tmp);
        cpu.fp_tmp_nan = static_cast<uint64_t>(frac) << 41;
    } else {
        auto f = std::bit_cast<float>(v);
        mpfr_set_flt(cpu.fp_tmp, f, cpu.FPCR.RND);
    }
}

void load_fpD(uint64_t v) {
    uint16_t exp = v >> 52 & 0x7ff;
    uint64_t frac = v & 0xFFFFFFFFFFFFFLLU;
    // test NAN
    if(exp == 0x7FF && frac) {
        mpfr_set_nan(cpu.fp_tmp);
        cpu.fp_tmp_nan = frac << 12;
    } else {
        double d = std::bit_cast<double>(v);
        mpfr_set_d(cpu.fp_tmp, d, cpu.FPCR.RND);
    }
}

void load_fpX(uint64_t frac, uint16_t exp) {
    bool sign = exp & 0x8000;
    exp &= 0x7fff;
    // consider special number
    if(exp == 0 && frac == 0) {
        // zero
        mpfr_set_zero(cpu.fp_tmp, sign ? -1 : 1);
        return;
    } else if(exp == 0x7fff) {
        if(frac == 0) {
            // infinity
            mpfr_set_inf(cpu.fp_tmp, sign ? -1 : 1);
        } else {
            // NAN
            mpfr_set_nan(cpu.fp_tmp);
            cpu.fp_tmp_nan = frac;
        }
        return;
    }
    mpfr_set_uj_2exp(cpu.fp_tmp, frac, exp - 16383 - 63, cpu.FPCR.RND);
    mpfr_setsign(cpu.fp_tmp, cpu.fp_tmp, sign, cpu.FPCR.RND);
}

void load_fpP(uint32_t addr) {
    uint32_t exp = MMU_ReadL(addr);
    bool sm = exp & 0x80000000;
    bool se = exp & 0x40000000;
    uint8_t yy = exp >> 28 & 3;
    uint8_t d16 = exp & 0xf;
    exp = (exp >> 16) & 0xfff;
    uint64_t frac =
        static_cast<uint64_t>(MMU_ReadL(addr + 4)) << 32 | MMU_ReadL(addr + 8);
    // consider special number
    if(yy == 3 && exp == 0xfff) {
        if(frac == 0) {
            // infinity
            mpfr_set_inf(cpu.fp_tmp, sm ? -1 : 1);
        } else {
            // NAN
            mpfr_set_nan(cpu.fp_tmp);
            cpu.fp_tmp_nan = frac;
        }
        return;
    } else if(exp == 0 && frac == 0) {
        // zero
        mpfr_set_zero(cpu.fp_tmp, sm ? -1.0 : 1.0);
        return;
    }
    char s[25];
    s[0] = sm ? '-' : '+'; // sign of frac
    s[1] = '0' + d16;      // integer of frac
    s[2] = '.';
    for(int i = 0; i < 16; ++i) {
        s[3 + i] = '0' + (frac >> (4 * (15 - i)) & 0xF);
    }
    s[19] = 'e';
    s[20] = se ? '-' : '+'; // sign of exp
    s[21] = '0' + (exp >> 8 & 0xF);
    s[22] = '0' + (exp >> 4 & 0xF);
    s[23] = '0' + (exp & 0xF);
    s[24] = '\0';
    cpu.fp_tmp_tv = mpfr_strtofr(cpu.fp_tmp, s, nullptr, 10, cpu.FPCR.RND);
    cpu.FPSR.INEX1 = (cpu.fp_tmp_tv != 0);
}

std::pair<std::function<void()>, int> fpu_load(int t, int type, int reg) {
    switch(t) {
    case 0: {
        auto [f, i] = ea_read32(type, reg, cpu.PC + 4);
        return {[f = std::move(f)]() {
                    mpfr_set_si(cpu.fp_tmp, static_cast<int32_t>(f()),
                                cpu.FPCR.RND);
                },
                i + 2};
    }
    case 1: {
        auto [f, i] = ea_read32(type, reg, cpu.PC + 4);
        return {[f = std::move(f)]() { load_fpS(f()); }, i + 2};
    }
    case 2: {
        auto [f, i] = ea_addr(type, reg, cpu.PC + 4, 12, false);
        return {[f = std::move(f)]() {
                    cpu.EA = f();
                    uint16_t exp = MMU_ReadW(cpu.EA);
                    uint64_t frac = loadLL(cpu.EA + 4);
                    load_fpX(frac, exp);
                },
                i + 2};
    }
    case 3: {
        auto [f, i] = ea_addr(type, reg, cpu.PC + 4, 12, false);
        return {[f = std::move(f)]() { load_fpP(cpu.EA = f()); }, i + 2};
    }
    case 4: {
        auto [f, i] = ea_read16(type, reg, cpu.PC + 4);
        return {[f = std::move(f)]() {
                    mpfr_set_si(cpu.fp_tmp, static_cast<int16_t>(f()),
                                cpu.FPCR.RND);
                },
                i + 2};
    }
    case 5: {
        auto [f, i] = ea_addr(type, reg, cpu.PC + 4, 8, false);
        return {[f = std::move(f)]() {
                    uint64_t frac = loadLL(cpu.EA = f());
                    load_fpD(frac);
                },
                i + 2};
    }
    case 6: {
        auto [f, i] = ea_read8(type, reg, cpu.PC + 4);
        return {[f  = std::move(f)]() {
                    mpfr_set_si(cpu.fp_tmp, static_cast<int8_t>(f()),
                                cpu.FPCR.RND);
                },
                i + 2};
    }
    }
    __builtin_unreachable();
}
int32_t store_fpL() {
    if(mpfr_get_exp(cpu.fp_tmp) < -1022) {
        mpfr_set_erangeflag();
    }
    auto v = mpfr_get_sj(cpu.fp_tmp, cpu.FPCR.RND);
    if(v > std::numeric_limits<int32_t>::max()) {
        mpfr_set_erangeflag();
        return std::numeric_limits<int32_t>::max();
    } else if(v < std::numeric_limits<int32_t>::min()) {
        mpfr_set_erangeflag();
        return std::numeric_limits<int32_t>::min();
    } else {
        return v;
    }
}

int16_t fpu_storeW() {
    if(mpfr_get_exp(cpu.fp_tmp) < -1022) {
        mpfr_set_underflow();
    }
    auto v = mpfr_get_sj(cpu.fp_tmp, cpu.FPCR.RND);
    if(v > std::numeric_limits<int16_t>::max()) {
        mpfr_set_overflow();
        return std::numeric_limits<int16_t>::max();
    } else if(v < std::numeric_limits<int16_t>::min()) {
        mpfr_set_overflow();
        return std::numeric_limits<int16_t>::min();
    } else {
        return v;
    }
}

int8_t store_fpB() {
    if(mpfr_get_exp(cpu.fp_tmp) < -1022) {
        mpfr_set_underflow();
    }
    auto v = mpfr_get_sj(cpu.fp_tmp, cpu.FPCR.RND);
    if(v > std::numeric_limits<int8_t>::max()) {
        mpfr_set_overflow();
        return std::numeric_limits<int8_t>::max();
    } else if(v < std::numeric_limits<int8_t>::min()) {
        mpfr_set_overflow();
        return std::numeric_limits<int8_t>::min();
    } else {
        return v;
    }
}

void normalize_fp(FPU_PREC p);

uint32_t store_fpS() {
    normalize_fp(FPU_PREC::S);
    if(mpfr_nan_p(cpu.fp_tmp)) {
        // NAN
        return 0x7F800000 | (cpu.fp_tmp_nan >> 41);
    }
    return std::bit_cast<uint32_t>(mpfr_get_flt(cpu.fp_tmp, cpu.FPCR.RND));
}

uint64_t store_fpD() {
    normalize_fp(FPU_PREC::D);
    if(mpfr_nan_p(cpu.fp_tmp)) {
        // NAN
        return 0x7FF0000000000000LLU | (cpu.fp_tmp_nan >> 12);
    }
    return std::bit_cast<uint64_t>(mpfr_get_d(cpu.fp_tmp, cpu.FPCR.RND));
}

std::pair<uint64_t, uint16_t> store_fpX() {
    normalize_fp(FPU_PREC::X);
    if(mpfr_nan_p(cpu.fp_tmp)) {
        // NAN
        return {cpu.fp_tmp_nan, static_cast<uint16_t>(0x7FFF)};
    }
    uint16_t e = (!!mpfr_signbit(cpu.fp_tmp)) << 15;
    if(mpfr_inf_p(cpu.fp_tmp)) {
        return {0, e | 0x7FFF};
    }
    if(mpfr_zero_p(cpu.fp_tmp)) {
        return {0, e};
    }
    mpfr_exp_t exp;
    mpfr_frexp(&exp, cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
    mpfr_abs(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
    exp--;
    if(exp > -16383) {
        // normal
        mpfr_mul_2si(cpu.fp_tmp, cpu.fp_tmp, 64, cpu.FPCR.RND);
        uint64_t frac = mpfr_get_uj(cpu.fp_tmp, cpu.FPCR.RND);
        return {frac, e | (exp + 16383)};
    } else {
        int sn = -exp - 16383;
        mpfr_mul_2si(cpu.fp_tmp, cpu.fp_tmp, 64 - sn, cpu.FPCR.RND);
        uint64_t frac = mpfr_get_uj(cpu.fp_tmp, cpu.FPCR.RND);
        return {frac, e};
    }
}
constexpr double LOG10_2 = .30102999566398119521;

void fpu_testex() {
    cpu.FPSR.OPERR = mpfr_nanflag_p() || mpfr_erangeflag_p();
    cpu.FPSR.OVFL = mpfr_overflow_p();
    cpu.FPSR.UNFL = mpfr_underflow_p();
    cpu.FPSR.DZ = mpfr_divby0_p();
    cpu.FPSR.INEX2 = mpfr_inexflag_p();

    cpu.FPSR.EXC_IOP |= (cpu.FPSR.S_NAN || cpu.FPSR.OPERR);
    cpu.FPSR.EXC_OVFL |= (cpu.FPSR.OVFL);
    cpu.FPSR.EXC_UNFL |= (cpu.FPSR.UNFL || cpu.FPSR.INEX2);
    cpu.FPSR.EXC_DZ |= (cpu.FPSR.DZ);
    cpu.FPSR.EXC_INEX |= (cpu.FPSR.INEX1 || cpu.FPSR.INEX2 || cpu.FPSR.OVFL);

    if(cpu.FPSR.S_NAN && cpu.FPCR.S_NAN) {
        CPU_RAISE(54);
    }
    if(cpu.FPSR.OPERR && cpu.FPCR.OPERR) {
        CPU_RAISE(52);
    }
    if(cpu.FPSR.OVFL && cpu.FPCR.OVFL) {
        CPU_RAISE(53);
    }
    if(cpu.FPSR.UNFL && cpu.FPCR.UNFL) {
        CPU_RAISE(51);
    }
    if(cpu.FPSR.DZ && cpu.FPCR.DZ) {
        CPU_RAISE(50);
    }
    if(cpu.FPSR.INEX1 && cpu.FPCR.INEX1) {
        CPU_RAISE(49);
    }
    if(cpu.FPSR.INEX2 && cpu.FPCR.INEX2) {
        CPU_RAISE(49);
    }
}
long mpfr_digit10() {
    mpfr_exp_t e = mpfr_get_exp(cpu.fp_tmp);
    if(e <= 0) {
        return 1;
    }
    return e * LOG10_2;
}
void store_fpP(uint32_t addr, int k) {
    if(k > 18) {
        cpu.FPSR.OPERR = true;
        k = 17;
    }
    normalize_fp(FPU_PREC::X);
    if(mpfr_nan_p(cpu.fp_tmp)) {
        // NAN
        MMU_WriteL(addr, 0x7FFF0000);
        WriteLL(addr + 4, cpu.fp_tmp_nan);
        return;
    }
    uint32_t e = mpfr_signbit(cpu.fp_tmp) << 31;
    if(mpfr_inf_p(cpu.fp_tmp)) {
        MMU_WriteL(addr, e | 0x7FFF0000);
        WriteLL(addr + 4, 0);
        return;
    }
    if(mpfr_zero_p(cpu.fp_tmp)) {
        MMU_WriteL(addr, e | 0);
        WriteLL(addr + 4, 0);
        return;
    }
    int ln = mpfr_digit10();
    int sn = k <= 0 ? -k + ln : k - 1;
    char buf[96] = {0};
    mpfr_snprintf(buf, 95, "%+.*R*e", sn, cpu.FPCR.RND, cpu.fp_tmp);
    uint32_t exp = (buf[1] - '0');
    exp |= (buf[0] == '-') << 31;
    auto ep = strchr(buf, 'e');
    exp |= (ep[1] == '-') << 30;

    uint64_t v = 0;
    for(int k = 0; k < (ep - buf - 3); ++k) {
        v |= static_cast<uint64_t>(buf[3 + k] - '0') << ((15 - k) * 4);
    }
    if(ep[3] == '\0') {
        exp |= (ep[2] - '0') << 16;
    } else if(ep[4] == '\0') {
        exp |= (ep[2] - '0') << 20;
        exp |= (ep[3] - '0') << 16;
    } else if(ep[5] == '\0') {
        exp |= (ep[2] - '0') << 24;
        exp |= (ep[3] - '0') << 20;
        exp |= (ep[4] - '0') << 16;
    } else {
        exp |= (ep[2] - '0') << 12;
        exp |= (ep[3] - '0') << 24;
        exp |= (ep[4] - '0') << 20;
        exp |= (ep[5] - '0') << 16;
    }
    MMU_WriteL(addr, exp);
    WriteLL(addr + 4, v);
}

std::pair<std::function<void()>, int> fpu_store(int t, int type, int reg,
                                                int fpn, int k) {

    switch(t) {
    case 0: {
        auto [f, i] = ea_write32(type, reg, cpu.PC + 4);
        return {[f = std::move(f), fpn]() {
                    mpfr_set(cpu.fp_tmp, cpu.FP[fpn], cpu.FPCR.RND);
                    cpu.fp_tmp_nan = cpu.FP_nan[fpn];
                    f(store_fpL());
                    fpu_testex();
                },
                i + 2};
    }
    case 1: {
        auto [f, i] = ea_write32(type, reg, cpu.PC + 4);
        return {[f = std::move(f), fpn]() {
                    mpfr_set(cpu.fp_tmp, cpu.FP[fpn], cpu.FPCR.RND);
                    cpu.fp_tmp_nan = cpu.FP_nan[fpn];
                    f(store_fpS());
                    fpu_testex();
                },
                i + 2};
    }
    case 2: {
        auto [f, i] = ea_addr(type, reg, cpu.PC + 4, 12, true);
        return {[f = std::move(f), fpn]() {
                    mpfr_set(cpu.fp_tmp, cpu.FP[fpn], cpu.FPCR.RND);
                    cpu.fp_tmp_nan = cpu.FP_nan[fpn];
                    cpu.EA = f();
                    auto [frac, exp] = store_fpX();
                    MMU_WriteW(cpu.EA, exp);
                    WriteLL(cpu.EA + 4, frac);
                    fpu_testex();
                },
                i + 2};
    }
    case 3: {
        auto [f, i] = ea_addr(type, reg, cpu.PC + 4, 12, true);
        return {[f = std::move(f), k, fpn]() {
                    mpfr_set(cpu.fp_tmp, cpu.FP[fpn], cpu.FPCR.RND);
                    cpu.fp_tmp_nan = cpu.FP_nan[fpn];
                    store_fpP(cpu.EA = f(), k);
                    fpu_testex();
                },
                i + 2};
    }
    case 4: {
        auto [f, i] = ea_write16(type, reg, cpu.PC + 4);
        return {[f = std::move(f), fpn]() {
                    mpfr_set(cpu.fp_tmp, cpu.FP[fpn], cpu.FPCR.RND);
                    cpu.fp_tmp_nan = cpu.FP_nan[fpn];
                    f(fpu_storeW());
                    fpu_testex();
                },
                i + 2};
    }
    case 5: {
        auto [f, i] = ea_addr(type, reg, cpu.PC + 4, 8, true);
        return {[f = std::move(f), fpn]() {
                    mpfr_set(cpu.fp_tmp, cpu.FP[fpn], cpu.FPCR.RND);
                    cpu.fp_tmp_nan = cpu.FP_nan[fpn];
                    WriteLL(cpu.EA = f(), store_fpD());
                    fpu_testex();
                },
                i + 2};
    }
    case 6: {
        auto [f, i] = ea_write8(type, reg, cpu.PC + 4);
        return {[f = std::move(f), fpn]() {
                    mpfr_set(cpu.fp_tmp, cpu.FP[fpn], cpu.FPCR.RND);
                    cpu.fp_tmp_nan = cpu.FP_nan[fpn];
                    f(store_fpB());
                    fpu_testex();
                },
                i + 2};
    }
    case 7: {
        auto [f, i] = ea_addr(type, reg, cpu.PC + 4, 12, true);
        return {[f = std::move(f), k = k >> 4, fpn]() {
                    mpfr_set(cpu.fp_tmp, cpu.FP[fpn], cpu.FPCR.RND);
                    cpu.fp_tmp_nan = cpu.FP_nan[fpn];
                    store_fpP(cpu.EA = f(), static_cast<int8_t>(cpu.D[k]));
                    fpu_testex();
                },
                i + 2};
    }
    }
    __builtin_unreachable();
}
void normalize_fp(FPU_PREC p) {
    switch(p) {
    case FPU_PREC::AUTO:
        __builtin_unreachable();
    case FPU_PREC::X:
        mpfr_set_emin(-16445);
        mpfr_set_emax(16384);
        break;
    case FPU_PREC::D:
        mpfr_set_emin(-1073);
        mpfr_set_emax(1024);
        break;
    case FPU_PREC::S:
        mpfr_set_emin(-148);
        mpfr_set_emax(128);
        break;
    }
    cpu.fp_tmp_tv = mpfr_check_range(cpu.fp_tmp, cpu.fp_tmp_tv, cpu.FPCR.RND);
    cpu.fp_tmp_tv = mpfr_subnormalize(cpu.fp_tmp, cpu.fp_tmp_tv, cpu.FPCR.RND);
    switch(p) {
    case FPU_PREC::AUTO:
        __builtin_unreachable();
    case FPU_PREC::X:
        cpu.fp_tmp_tv = mpfr_prec_round(cpu.fp_tmp, 64, cpu.FPCR.RND);
        break;
    case FPU_PREC::D:
        cpu.fp_tmp_tv = mpfr_prec_round(cpu.fp_tmp, 53, cpu.FPCR.RND);
        break;
    case FPU_PREC::S:
        cpu.fp_tmp_tv = mpfr_prec_round(cpu.fp_tmp, 24, cpu.FPCR.RND);
        break;
    }
    mpfr_set_emin(-32768);
    mpfr_set_emax(32767);
}
void store_fpr(int fpn, FPU_PREC p = FPU_PREC::AUTO) {
    cpu.FPSR.CC_NAN = mpfr_nan_p(cpu.fp_tmp);
    if(cpu.FPSR.CC_NAN) {
        if(!(cpu.fp_tmp_nan & 1LL << 62)) {
            cpu.FPSR.S_NAN = true;
            cpu.fp_tmp_nan |= (1LL << 62);
        }
    }
    FPU_PREC pp = p == FPU_PREC::AUTO ? cpu.FPCR.PREC : p;
    normalize_fp(pp);
    if(fpn != -1) {
        mpfr_swap(cpu.FP[fpn], cpu.fp_tmp);
        std::swap(cpu.FP_nan[fpn], cpu.fp_tmp_nan);
    }
    cpu.FPSR.CC_I = mpfr_inf_p(cpu.fp_tmp);
    cpu.FPSR.CC_Z = mpfr_zero_p(cpu.fp_tmp);
    cpu.FPSR.CC_N = mpfr_sgn(cpu.fp_tmp) < 0;

    fpu_testex();
}

bool test_NAN1() {
    if(mpfr_nan_p(cpu.fp_tmp)) {
        cpu.FPSR.OPERR = true;
        return false;
    }
    return true;
}

bool test_NAN2(int fpn) {
    if(mpfr_nan_p(cpu.FP[fpn]) && !mpfr_nan_p(cpu.fp_tmp)) {
        cpu.FPSR.OPERR = true;
        mpfr_set_nan(cpu.fp_tmp);
        cpu.fp_tmp_nan = cpu.FP_nan[fpn];
        return false;
    } else if(mpfr_nan_p(cpu.fp_tmp)) {
        cpu.FPSR.OPERR = true;
        return false;
    }
    return true;
}
void fpu_prologue() {
    mpfr_clear_flags();
    cpu.FPIAR = cpu.PC;
}

std::function<void()> decode_movecr(int opc, int fpn) {
    switch(opc) {
    case 0:
        // PI
        return [fpn]() {
            fpu_prologue();
            mpfr_const_pi(cpu.fp_tmp, cpu.FPCR.RND);
            store_fpr(fpn);
        };
    case 0xB:
        // log10_2
        return [fpn]() {
            fpu_prologue();
            mpfr_set_ui(cpu.fp_tmp, 2.0, cpu.FPCR.RND);
            mpfr_log10(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
            store_fpr(fpn);
        };
    case 0xC:
        // e
        return [fpn]() {
            fpu_prologue();
            mpfr_set_ui(cpu.fp_tmp, 1.0, cpu.FPCR.RND);
            mpfr_exp(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
            store_fpr(fpn);
        };
    case 0xD:
        // log2_e
        return [fpn]() {
            fpu_prologue();
            mpfr_set_ui(cpu.fp_tmp, 1.0, cpu.FPCR.RND);
            mpfr_exp(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
            mpfr_log2(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
            store_fpr(fpn);
        };
    case 0xE:
        // log10_e
        return [fpn]() {
            fpu_prologue();
            mpfr_set_ui(cpu.fp_tmp, 1.0, cpu.FPCR.RND);
            mpfr_exp(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
            mpfr_log10(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
            store_fpr(fpn);
        };
    case 0x30:
        // log2_e
        return [fpn]() {
            fpu_prologue();
            mpfr_const_log2(cpu.fp_tmp, cpu.FPCR.RND);
            store_fpr(fpn);
        };
    case 0x31:
        // ln(10)
        return [fpn]() {
            fpu_prologue();
            mpfr_set_ui(cpu.fp_tmp, 10.0, cpu.FPCR.RND);
            mpfr_log(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
            store_fpr(fpn);
        };
    case 0x32:
        // 10^0
        return [fpn]() {
            fpu_prologue();
            mpfr_ui_pow_ui(cpu.fp_tmp, 10, 0, cpu.FPCR.RND);
            store_fpr(fpn);
        };
    case 0x33:
        // 10^1
        return [fpn]() {
            fpu_prologue();
            mpfr_ui_pow_ui(cpu.fp_tmp, 10, 1, cpu.FPCR.RND);
            store_fpr(fpn);
        };
    case 0x34:
        // 10^2
        return [fpn]() {
            fpu_prologue();
            mpfr_ui_pow_ui(cpu.fp_tmp, 10, 2, cpu.FPCR.RND);
            store_fpr(fpn);
        };

    case 0x35:
        // 10^4
        return [fpn]() {
            fpu_prologue();
            mpfr_ui_pow_ui(cpu.fp_tmp, 10, 4, cpu.FPCR.RND);
            store_fpr(fpn);
        };
    case 0x36:
        // 10^8
        return [fpn]() {
            fpu_prologue();
            mpfr_ui_pow_ui(cpu.fp_tmp, 10, 8, cpu.FPCR.RND);
            store_fpr(fpn);
        };
    case 0x37:
        // 10^16
        return [fpn]() {
            fpu_prologue();
            mpfr_ui_pow_ui(cpu.fp_tmp, 10, 16, cpu.FPCR.RND);
            store_fpr(fpn);
        };
    case 0x38:
        // 10^32
        return [fpn]() {
            fpu_prologue();
            mpfr_ui_pow_ui(cpu.fp_tmp, 10, 32, cpu.FPCR.RND);
            store_fpr(fpn);
        };
    case 0x39:
        // 10^64
        return [fpn]() {
            fpu_prologue();
            mpfr_ui_pow_ui(cpu.fp_tmp, 10, 64, cpu.FPCR.RND);
            store_fpr(fpn);
        };
    case 0x3A:
        // 10^128
        return [fpn]() {
            fpu_prologue();
            mpfr_ui_pow_ui(cpu.fp_tmp, 10, 128, cpu.FPCR.RND);
            store_fpr(fpn);
        };
    case 0x3B:
        // 10^256
        return [fpn]() {
            fpu_prologue();
            mpfr_ui_pow_ui(cpu.fp_tmp, 10, 256, cpu.FPCR.RND);
            store_fpr(fpn);
        };
    case 0x3C:
        // 10^512
        return [fpn]() {
            fpu_prologue();
            mpfr_ui_pow_ui(cpu.fp_tmp, 10, 512, cpu.FPCR.RND);
            store_fpr(fpn);
        };
    case 0x3D:
        // 10^1024
        return [fpn]() {
            fpu_prologue();
            mpfr_ui_pow_ui(cpu.fp_tmp, 10, 1024, cpu.FPCR.RND);
            store_fpr(fpn);
        };
    case 0x3E:
        // 10^2048
        return [fpn]() {
            fpu_prologue();
            mpfr_ui_pow_ui(cpu.fp_tmp, 10, 2048, cpu.FPCR.RND);
            store_fpr(fpn);
        };
    case 0x3F:
        // 10^4096
        return [fpn]() {
            fpu_prologue();
            mpfr_ui_pow_ui(cpu.fp_tmp, 10, 4096, cpu.FPCR.RND);
            store_fpr(fpn);
        };
    case 0x0F:
    default:
        return [fpn]() {
            fpu_prologue();
            mpfr_set_zero(cpu.fp_tmp, 1);
            store_fpr(fpn);
        };
    }
}

std::function<void()> fop_to_reg(std::function<void()> f, int opc, int fpn) {
    switch(opc) {
    case 0:
        // FMOVE
        return [f, fpn]() {
            fpu_prologue();
            f();
            store_fpr(fpn);
        };
    case 1:
        // FINT
        return [f, fpn]() {
            fpu_prologue();
            cpu.FPIAR = cpu.PC;
            f();
            if(test_NAN1()) {
                cpu.fp_tmp_tv = mpfr_rint(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
            }
            store_fpr(fpn);
        };
    case 2:
        // FSINH
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN1()) {
                cpu.fp_tmp_tv = mpfr_sinh(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
            }
            store_fpr(fpn);
        };
    case 3:
        // FINTRZ
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN1()) {
                cpu.fp_tmp_tv = mpfr_trunc(cpu.fp_tmp, cpu.fp_tmp);
            }
            store_fpr(fpn);
        };
    case 4:
        // FSQRT
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN1()) {
                cpu.fp_tmp_tv = mpfr_sqrt(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
            }
            store_fpr(fpn);
        };
    case 6:
        // FLOGNP1
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN1()) {
                cpu.fp_tmp_tv =
                    mpfr_log1p(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
            }
            store_fpr(fpn);
        };
    case 8:
        // FETOXM1
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN1()) {
                cpu.fp_tmp_tv =
                    mpfr_expm1(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
            }
            store_fpr(fpn);
        };
    case 9:
        // FTANH
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN1()) {
                cpu.fp_tmp_tv = mpfr_tanh(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
            }
            store_fpr(fpn);
        };
    case 10:
        // FATAN
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN1()) {
                cpu.fp_tmp_tv = mpfr_atan(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
            }
            store_fpr(fpn);
        };
    case 12:
        // FASIN
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN1()) {
                cpu.fp_tmp_tv = mpfr_asin(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
            }
            store_fpr(fpn);
        };
    case 13:
        // FATANH
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN1()) {
                cpu.fp_tmp_tv =
                    mpfr_atanh(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
            }
            store_fpr(fpn);
        };
    case 14:
        // FSIN
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN1()) {
                cpu.fp_tmp_tv = mpfr_sin(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
            }
            store_fpr(fpn);
        };
    case 15:
        // FTAN
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN1()) {
                cpu.fp_tmp_tv = mpfr_tan(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
            }
            store_fpr(fpn);
        };
    case 16:
        // FETOX
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN1()) {
                cpu.fp_tmp_tv = mpfr_exp(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
            }
            store_fpr(fpn);
        };
    case 17:
        // FTWOTOX
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN1()) {
                cpu.fp_tmp_tv = mpfr_exp2(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
            }
            store_fpr(fpn);
        };
    case 18:
        // FTENTOX
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN1()) {
                cpu.fp_tmp_tv =
                    mpfr_exp10(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
            }
            store_fpr(fpn);
        };
    case 20:
        // FLOGN
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN1()) {
                cpu.fp_tmp_tv = mpfr_log(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
            }
            store_fpr(fpn);
        };
    case 21:
        // FLOG10
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN1()) {
                cpu.fp_tmp_tv =
                    mpfr_log10(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
            }
            store_fpr(fpn);
        };
    case 22:
        // FLOG2
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN1()) {
                cpu.fp_tmp_tv = mpfr_log2(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
            }
            store_fpr(fpn);
        };
    case 24:
        // FABS
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN1()) {
                cpu.fp_tmp_tv = mpfr_abs(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
            }
            store_fpr(fpn);
        };
    case 25:
        // FCOSH
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN1()) {
                cpu.fp_tmp_tv = mpfr_cosh(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
            }
            store_fpr(fpn);
        };
    case 26:
        // FNEG
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN1()) {
                cpu.fp_tmp_tv = mpfr_neg(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
            }
            store_fpr(fpn);
        };
    case 28:
        // FACOS
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN1()) {
                cpu.fp_tmp_tv = mpfr_acos(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
            }
            store_fpr(fpn);
        };
    case 29:
        // FCOS
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN1()) {
                cpu.fp_tmp_tv = mpfr_cos(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
            }
            store_fpr(fpn);
        };
    case 30:
        // FGETEXP
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN1()) {
                if(mpfr_inf_p(cpu.fp_tmp)) {
                    mpfr_set_nan(cpu.fp_tmp);
                    mpfr_set_nanflag();
                    cpu.fp_tmp_nan = QNAN_DEFAULT;
                } else if(!mpfr_zero_p(cpu.fp_tmp)) {
                    long exp;
                    mpfr_get_d_2exp(&exp, cpu.fp_tmp, cpu.FPCR.RND);
                    // the result of mpfr_get_d_2exp  is
                    // [0.5, 1.0)
                    mpfr_set_si(cpu.fp_tmp, exp - 1, cpu.FPCR.RND);
                }
            }
            store_fpr(fpn);
        };
    case 31:
        // FGETMAN
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN1()) {
                if(mpfr_inf_p(cpu.fp_tmp)) {
                    mpfr_set_nan(cpu.fp_tmp);
                    mpfr_set_nanflag();
                    cpu.fp_tmp_nan = QNAN_DEFAULT;
                } else {
                    mpfr_exp_t exp;
                    cpu.fp_tmp_tv =
                        mpfr_frexp(&exp, cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
                    // the result of mpfr_frexp  is
                    // [0.5, 1.0)
                    mpfr_mul_si(cpu.fp_tmp, cpu.fp_tmp, 2.0, cpu.FPCR.RND);
                }
            }
            store_fpr(fpn);
        };
    case 32:
        // FDIV
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN2(fpn)) {

                cpu.fp_tmp_tv =
                    mpfr_div(cpu.fp_tmp, cpu.fp_tmp, cpu.FP[fpn], cpu.FPCR.RND);
            }
            store_fpr(fpn);
        };
    case 33:
        // FMOD
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN2(fpn)) {
                long q;
                cpu.fp_tmp_tv = mpfr_fmodquo(cpu.fp_tmp, &q, cpu.fp_tmp,
                                             cpu.FP[fpn], cpu.FPCR.RND);
                cpu.FPSR.Quat = abs(q) & 0x7f;
                cpu.FPSR.QuatSign = q < 0;
            }
            store_fpr(fpn);
        };
    case 34:
        // FADD
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN2(fpn)) {
                cpu.fp_tmp_tv =
                    mpfr_add(cpu.fp_tmp, cpu.fp_tmp, cpu.FP[fpn], cpu.FPCR.RND);
            }
            store_fpr(fpn);
        };
    case 35:
        // FMUL
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN2(fpn)) {
                cpu.fp_tmp_tv =
                    mpfr_mul(cpu.fp_tmp, cpu.fp_tmp, cpu.FP[fpn], cpu.FPCR.RND);
            }
            store_fpr(fpn);
        };
    case 36:
        // FSGLDIV
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN2(fpn)) {
                mpfr_prec_round(cpu.fp_tmp, 23, cpu.FPCR.RND);
                mpfr_prec_round(cpu.FP[fpn], 23, cpu.FPCR.RND);
                cpu.fp_tmp_tv =
                    mpfr_div(cpu.fp_tmp, cpu.fp_tmp, cpu.FP[fpn], cpu.FPCR.RND);
            }
            store_fpr(fpn, FPU_PREC::S);
        };
    case 37:
        // FREM
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN2(fpn)) {
                long q;

                cpu.fp_tmp_tv = mpfr_remquo(cpu.fp_tmp, &q, cpu.fp_tmp,
                                            cpu.FP[fpn], cpu.FPCR.RND);
                cpu.FPSR.Quat = std::abs(q) & 0x7f;
                cpu.FPSR.QuatSign = q < 0;
            }
            store_fpr(fpn);
        };
    case 38:
        // FSCALE
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN2(fpn)) {
                if(mpfr_inf_p(cpu.fp_tmp)) {
                    mpfr_set_nan(cpu.fp_tmp);
                    mpfr_set_nanflag();
                    cpu.fp_tmp_nan = QNAN_DEFAULT;
                } else {
                    cpu.fp_tmp_tv = mpfr_mul_2si(
                        cpu.fp_tmp, cpu.FP[fpn],
                        mpfr_get_si(cpu.fp_tmp, cpu.FPCR.RND), cpu.FPCR.RND);
                }
            }
            store_fpr(fpn);
        };
    case 39:
        // FSGLMUL
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN2(fpn)) {
                mpfr_prec_round(cpu.fp_tmp, 23, cpu.FPCR.RND);
                mpfr_prec_round(cpu.FP[fpn], 23, cpu.FPCR.RND);
                cpu.fp_tmp_tv =
                    mpfr_mul(cpu.fp_tmp, cpu.fp_tmp, cpu.FP[fpn], cpu.FPCR.RND);
            }
            store_fpr(fpn, FPU_PREC::S);
        };
    case 40:
        // FMUL
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN2(fpn)) {
                cpu.fp_tmp_tv =
                    mpfr_sub(cpu.fp_tmp, cpu.fp_tmp, cpu.FP[fpn], cpu.FPCR.RND);
            }
            store_fpr(fpn);
        };
    case 48:
    case 49:
    case 50:
    case 51:
    case 52:
    case 53:
    case 54:
    case 55: {
        // FSINCOS
        int fpc = opc & 7;
        if(fpc == fpn) {
            return [f, fpn]() {
                fpu_prologue();
                f();
                if(test_NAN1()) {
                    cpu.fp_tmp_tv =
                        mpfr_sin(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
                }
                store_fpr(fpn);
            };
        } else {
            return [f, fpn, fpc]() {
                fpu_prologue();
                f();
                if(test_NAN1()) {
                    cpu.fp_tmp_tv = mpfr_sin_cos(cpu.fp_tmp, cpu.FP[fpc],
                                                 cpu.fp_tmp, cpu.FPCR.RND);
                }
                store_fpr(fpn);
            };
        }
    }
    case 56:
        // FCMP
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN2(fpn)) {
                cpu.FPSR.CC_Z = mpfr_equal_p(cpu.FP[fpn], cpu.fp_tmp);
                if((mpfr_zero_p(cpu.fp_tmp) && mpfr_zero_p(cpu.FP[fpn])) ||
                   (mpfr_inf_p(cpu.fp_tmp) && mpfr_inf_p(cpu.FP[fpn]))) {
                    cpu.FPSR.CC_N = mpfr_signbit(cpu.FP[fpn]);
                } else {
                    cpu.FPSR.CC_N = mpfr_less_p(cpu.FP[fpn], cpu.fp_tmp);
                }
            } else {
                cpu.FPSR.CC_NAN = true;
            }
            fpu_testex();
        };
    case 58:
        // FTST
        return [f]() {
            fpu_prologue();
            f();
            cpu.FPSR.CC_NAN = mpfr_nan_p(cpu.fp_tmp);
            cpu.FPSR.CC_I = mpfr_inf_p(cpu.fp_tmp);
            cpu.FPSR.CC_N = mpfr_signbit(cpu.fp_tmp);
            cpu.FPSR.CC_Z = mpfr_zero_p(cpu.fp_tmp);
            fpu_testex();
        };

    case 64:
        // FSMOVE
        return [f, fpn]() {
            fpu_prologue();
            f();
            store_fpr(fpn, FPU_PREC::S);
        };
    case 65:
        // FSSQRT
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN1()) {
                cpu.fp_tmp_tv = mpfr_sqrt(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
            }
            store_fpr(fpn, FPU_PREC::S);
        };
    case 68:
        // FDMOVE
        return [f, fpn]() {
            fpu_prologue();
            f();
            store_fpr(fpn, FPU_PREC::D);
        };
    case 69:
        // FDSQRT
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN1()) {
                cpu.fp_tmp_tv = mpfr_sqrt(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
            }
            store_fpr(fpn, FPU_PREC::D);
        };
    case 88:
        // FSABS
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN1()) {
                cpu.fp_tmp_tv = mpfr_abs(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
            }
            store_fpr(fpn, FPU_PREC::S);
        };
    case 90:
        // FSNEG
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN1()) {
                cpu.fp_tmp_tv = mpfr_neg(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
            }
            store_fpr(fpn, FPU_PREC::S);
        };

    case 92:
        // FDABS
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN1()) {
                cpu.fp_tmp_tv = mpfr_abs(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
            }
            store_fpr(fpn, FPU_PREC::D);
        };
    case 94:
        // FDNEG
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN1()) {
                cpu.fp_tmp_tv = mpfr_neg(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
            }
            store_fpr(fpn, FPU_PREC::D);
        };
    case 96:
        // FSDIV
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN2(fpn)) {

                cpu.fp_tmp_tv =
                    mpfr_div(cpu.fp_tmp, cpu.fp_tmp, cpu.FP[fpn], cpu.FPCR.RND);
            }
            store_fpr(fpn, FPU_PREC::S);
        };
    case 98:
        // FSADD
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN2(fpn)) {

                cpu.fp_tmp_tv =
                    mpfr_add(cpu.fp_tmp, cpu.fp_tmp, cpu.FP[fpn], cpu.FPCR.RND);
            }
            store_fpr(fpn, FPU_PREC::S);
        };
    case 99:
        // FSMUL
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN2(fpn)) {

                cpu.fp_tmp_tv =
                    mpfr_mul(cpu.fp_tmp, cpu.fp_tmp, cpu.FP[fpn], cpu.FPCR.RND);
            }
            store_fpr(fpn, FPU_PREC::S);
        };

    case 100:
        // FDDIV
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN2(fpn)) {
                cpu.fp_tmp_tv =
                    mpfr_div(cpu.fp_tmp, cpu.fp_tmp, cpu.FP[fpn], cpu.FPCR.RND);
            }
            store_fpr(fpn, FPU_PREC::D);
        };
    case 102:
        // FDADD
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN2(fpn)) {
                cpu.fp_tmp_tv =
                    mpfr_add(cpu.fp_tmp, cpu.fp_tmp, cpu.FP[fpn], cpu.FPCR.RND);
            }
            store_fpr(fpn, FPU_PREC::D);
        };
    case 103:
        // FDMUL
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN2(fpn)) {

                cpu.fp_tmp_tv =
                    mpfr_mul(cpu.fp_tmp, cpu.fp_tmp, cpu.FP[fpn], cpu.FPCR.RND);
            }
            store_fpr(fpn, FPU_PREC::D);
        };

    case 104:
        // FSSUB
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN2(fpn)) {

                cpu.fp_tmp_tv =
                    mpfr_sub(cpu.fp_tmp, cpu.fp_tmp, cpu.FP[fpn], cpu.FPCR.RND);
            }
            store_fpr(fpn, FPU_PREC::S);
        };
    case 108:
        // FDSUB
        return [f, fpn]() {
            fpu_prologue();
            f();
            if(test_NAN2(fpn)) {

                cpu.fp_tmp_tv =
                    mpfr_sub(cpu.fp_tmp, cpu.fp_tmp, cpu.FP[fpn], cpu.FPCR.RND);
            }
            store_fpr(fpn, FPU_PREC::D);
        };
    }
    throw DecodeError{};
}
static uint16_t Get_FPCR() {
    uint16_t v = 0;
    switch(cpu.FPCR.RND) {
    case MPFR_RNDN:
        v = 0 << 4;
        break;
    case MPFR_RNDZ:
        v = 1 << 4;
        break;
    case MPFR_RNDD:
        v = 2 << 4;
        break;
    case MPFR_RNDU:
        v = 3 << 4;
        break;
    default:
        __builtin_unreachable();
    }
    switch(cpu.FPCR.PREC) {
    case FPU_PREC::X:
        v |= 0 << 6;
        break;
    case FPU_PREC::S:
        v |= 1 << 6;
        break;
    case FPU_PREC::D:
        v |= 2 << 6;
        break;
    default:
        __builtin_unreachable();
    }
    return v | cpu.FPCR.INEX1 << 8 | cpu.FPCR.INEX2 << 9 | cpu.FPCR.DZ << 10 |
           cpu.FPCR.UNFL << 11 | cpu.FPCR.OVFL << 12 | cpu.FPCR.OPERR << 13 |
           cpu.FPCR.S_NAN << 14 | cpu.FPCR.BSUN << 15;
}

static mpfr_rnd_t MPFR_RNDs[] = {MPFR_RNDN, MPFR_RNDZ, MPFR_RNDD, MPFR_RNDU};
static FPU_PREC MPFR_PRECs[] = {FPU_PREC::X, FPU_PREC::S, FPU_PREC::D,
                                FPU_PREC::X};
static void Set_FPCR(uint16_t v) {
    cpu.FPCR.RND = MPFR_RNDs[v >> 4 & 3];
    cpu.FPCR.PREC = MPFR_PRECs[v >> 6 & 3];
    cpu.FPCR.INEX1 = v & 1 << 8;
    cpu.FPCR.INEX2 = v & 1 << 9;
    cpu.FPCR.DZ = v & 1 << 10;
    cpu.FPCR.UNFL = v & 1 << 11;
    cpu.FPCR.OVFL = v & 1 << 12;
    cpu.FPCR.OPERR = v & 1 << 13;
    cpu.FPCR.S_NAN = v & 1 << 14;
    cpu.FPCR.BSUN = v & 1 << 15;
}
static uint32_t Get_FPSR() {
    return cpu.FPSR.CC_N << 27 | cpu.FPSR.CC_Z << 26 | cpu.FPSR.CC_I << 25 |
           cpu.FPSR.CC_NAN << 24 | cpu.FPSR.QuatSign << 23 |
           cpu.FPSR.Quat << 16 | cpu.FPSR.BSUN << 15 | cpu.FPSR.S_NAN << 14 |
           cpu.FPSR.OPERR << 13 | cpu.FPSR.OVFL << 12 | cpu.FPSR.UNFL << 11 |
           cpu.FPSR.DZ << 10 | cpu.FPSR.INEX2 << 9 | cpu.FPSR.INEX1 << 8 |
           cpu.FPSR.EXC_IOP << 7 | cpu.FPSR.EXC_OVFL << 6 |
           cpu.FPSR.EXC_UNFL << 5 | cpu.FPSR.EXC_DZ << 4 |
           cpu.FPSR.EXC_INEX << 3;
}

void test_bsun() {
    if(cpu.FPSR.CC_NAN) {
        cpu.FPSR.BSUN = true;
        if(cpu.FPCR.BSUN) {
            CPU_RAISE(48);
        }
    }
}

bool test_Fcc(uint8_t v) {
    switch(v) {
    // (S)F
    case 0B0000:
        return false;
    // (S)T
    case 0B1111:
        return true;
    case 0B0001:
        // (S)EQ
        return cpu.FPSR.CC_Z;
    case 0B1110:
        // (S)NE
        return !cpu.FPSR.CC_Z;
    case 0B0010:
        // (O)GT
        return !(cpu.FPSR.CC_NAN || cpu.FPSR.CC_Z || cpu.FPSR.CC_N);
    case 0B1101:
        // NGT/ULE
        return (cpu.FPSR.CC_NAN || cpu.FPSR.CC_Z || cpu.FPSR.CC_N);
    case 0B0011:
        // (O)GE
        return (cpu.FPSR.CC_Z || !(cpu.FPSR.CC_NAN || cpu.FPSR.CC_N));
    case 0B1100:
        // NGE/ULT
        return (cpu.FPSR.CC_NAN || (cpu.FPSR.CC_N && !cpu.FPSR.CC_Z));
    case 0B0100:
        // (O)LT
        return (cpu.FPSR.CC_N && !(cpu.FPSR.CC_NAN || cpu.FPSR.CC_Z));
    case 0B1011:
        // NLT/UGE
        return (cpu.FPSR.CC_NAN || (cpu.FPSR.CC_Z && !cpu.FPSR.CC_N));
    case 0B0101:
        // (O)LE
        return (cpu.FPSR.CC_Z || (cpu.FPSR.CC_N && !cpu.FPSR.CC_NAN));
    case 0B1010:
        // NLE/UGT
        return (cpu.FPSR.CC_NAN || (cpu.FPSR.CC_Z && !cpu.FPSR.CC_N));
    case 0B0110:
        // O(GL)
        return !(cpu.FPSR.CC_NAN || cpu.FPSR.CC_Z);
    case 0B1001:
        // MGL/UEQ
        return (cpu.FPSR.CC_NAN || cpu.FPSR.CC_Z);
    case 0B0111:
        // GLE/OR
        return !cpu.FPSR.CC_NAN;
    case 0B1000:
        // NGL/UN
        return cpu.FPSR.CC_NAN;
    default:
        throw DecodeError{};
    }
}
static void Set_FPSR(uint32_t v) {
    cpu.FPSR.CC_N = v & 1 << 27;
    cpu.FPSR.CC_Z = v & 1 << 26;
    cpu.FPSR.CC_I = v & 1 << 25;
    cpu.FPSR.CC_NAN = v & 1 << 24;
    cpu.FPSR.QuatSign = v & 1 << 23;
    cpu.FPSR.Quat = v >> 16 & 0x7f;
    cpu.FPSR.BSUN = v & 1 << 15;
    cpu.FPSR.S_NAN = v & 1 << 14;
    cpu.FPSR.OPERR = v & 1 << 13;
    cpu.FPSR.OVFL = v & 1 << 12;
    cpu.FPSR.UNFL = v & 1 << 11;
    cpu.FPSR.DZ = v & 1 << 10;
    cpu.FPSR.INEX2 = v & 1 << 9;
    cpu.FPSR.INEX1 = v & 1 << 8;
    cpu.FPSR.EXC_IOP = v & 1 << 7;
    cpu.FPSR.EXC_OVFL = v & 1 << 6;
    cpu.FPSR.EXC_UNFL = v & 1 << 5;
    cpu.FPSR.EXC_DZ = v & 1 << 4;
    cpu.FPSR.EXC_INEX = v & 1 << 3;
}

uint32_t fmovem_from_reg_rev(uint32_t base, uint8_t regs) {
    for(int i = 7; i >= 0; --i) {
        if(regs & 1 << i) {
            base -= 12;
            mpfr_set(cpu.fp_tmp, cpu.FP[i], cpu.FPCR.RND);
            cpu.fp_tmp_nan = cpu.FP_nan[i];
            auto [frac, exp] = store_fpX();
            MMU_WriteW(base, exp);
            WriteLL(base + 4, frac);
        }
    }
    return base;
}

uint32_t fmovem_from_reg(uint32_t base, uint8_t regs) {
    for(int i = 0; i < 8; ++i) {
        if(regs & 1 << (7 - i)) {
            mpfr_set(cpu.fp_tmp, cpu.FP[i], cpu.FPCR.RND);
            cpu.fp_tmp_nan = cpu.FP_nan[i];
            auto [frac, exp] = store_fpX();
            MMU_WriteW(base, exp);
            WriteLL(base + 4, frac);
            base += 12;
        }
    }
    return base;
}

uint32_t fmovem_to_reg(uint32_t base, uint8_t regs) {
    for(int i = 0; i < 8; ++i) {
        if(regs & 1 << (7 - i)) {
            uint16_t exp = MMU_ReadW(base);
            uint64_t frac = loadLL(base + 4);
            load_fpX(frac, exp);
            mpfr_swap(cpu.fp_tmp, cpu.FP[i]);
            std::swap(cpu.fp_tmp_nan, cpu.FP_nan[i]);
            base += 12;
        }
    }
    return base;
}

std::pair<std::function<void()>, int> decode_fpu(int sz, int type, int reg) {
    uint16_t nextop = FETCH(cpu.PC + 2);
    bool is_fmovem = nextop & 1 << 15;
    bool rm = nextop & 1 << 14;
    bool fp_k = nextop & 1 << 13;
    int fpm = nextop >> 10 & 7;
    int fpn = nextop >> 7 & 7;
    uint8_t opc = nextop & 0x7f;
    try {
        if(sz == 0) {
            if(!is_fmovem) {
                if(!fp_k) {
                    if(fpm != 7) {
                        std::function<void()> f;
                        int i = 0;
                        if(rm) {
                            std::tie(f, i) = fpu_load(fpm, type, reg);
                        } else {
                            f = [fpm]() {
                                mpfr_swap(cpu.FP[fpm], cpu.fp_tmp);
                                std::swap(cpu.FP_nan[fpm], cpu.fp_tmp_nan);
                            };
                        }
                        return {fop_to_reg(f, opc, fpn), i + 2};
                    } else {
                        // FMOVE CR
                        return {decode_movecr(opc, fpn), 2};
                    }
                } else {
                    // FMOVE to EA
                    return fpu_store(fpm, type, reg, fpn,
                                     static_cast<int8_t>(opc << 1) >> 1);
                }
            } else {
                if(!rm) {
                    uint8_t regs = nextop >> 10 & 7;
                    if(!fp_k) {
                        // Fmove(M) to FPcc
                        switch(regs) {
                        case 0:
                            throw DecodeError{};
                        case 1: // IR only
                        {
                            auto [r, o] = ea_read32(type, reg, 4);
                            return {[r = std::move(r)]() {
                                        cpu.FPIAR = r();
                                        if(cpu.T == 1) {
                                            cpu.must_trace = true;
                                        }
                                    },
                                    o + 2};
                        }
                        case 2: // FPSR only
                        {
                            auto [r, o] = ea_read32(type, reg, 4);
                            return {[r = std::move(r)]() {
                                        Set_FPSR(r());
                                        if(cpu.T == 1) {
                                            cpu.must_trace = true;
                                        }
                                    },
                                    o + 2};
                        }
                        case 3: // FPSR & IR
                        {
                            auto [a, o] =
                                ea_addr(type, reg, cpu.PC + 2, 8, false);
                            return {[a = std::move(a)]() {
                                        cpu.EA = a();
                                        Set_FPSR(MMU_ReadL(cpu.EA));
                                        cpu.FPIAR = MMU_ReadL(cpu.EA + 4);
                                        if(cpu.T == 1) {
                                            cpu.must_trace = true;
                                        }
                                    },
                                    o + 2};
                        }
                        case 4: // FPCR only
                        {
                            auto [r, o] = ea_read32(type, reg, 4);
                            return {[r = std::move(r)]() {
                                        Set_FPCR(r());
                                        if(cpu.T == 1) {
                                            cpu.must_trace = true;
                                        }
                                    },
                                    o + 2};
                        }
                        case 5: // FPCR & IR
                        {
                            auto [a, o] =
                                ea_addr(type, reg, cpu.PC + 2, 8, false);
                            return {[a = std::move(a)]() {
                                        cpu.EA = a();
                                        Set_FPCR(MMU_ReadL(cpu.EA));
                                        cpu.FPIAR = MMU_ReadL(cpu.EA + 4);
                                        if(cpu.T == 1) {
                                            cpu.must_trace = true;
                                        }
                                    },
                                    o + 2};
                        }
                        case 6: // FPCR & FPSR
                        {
                            auto [a, o] =
                                ea_addr(type, reg, cpu.PC + 2, 8, false);
                            return {[a = std::move(a)]() {
                                        cpu.EA = a();
                                        Set_FPCR(MMU_ReadL(cpu.EA));
                                        Set_FPSR(MMU_ReadL(cpu.EA + 4));
                                        if(cpu.T == 1) {
                                            cpu.must_trace = true;
                                        }
                                    },
                                    o + 2};
                        }
                        case 7: // FPCR & FPSR & UR
                        {
                            auto [a, o] =
                                ea_addr(type, reg, cpu.PC + 2, 12, false);
                            return {[a = std::move(a)]() {
                                        cpu.EA = a();
                                        Set_FPCR(MMU_ReadL(cpu.EA));
                                        Set_FPSR(MMU_ReadL(cpu.EA + 4));
                                        cpu.FPIAR = MMU_ReadL(cpu.EA + 8);
                                        if(cpu.T == 1) {
                                            cpu.must_trace = true;
                                        }
                                    },
                                    o + 2};
                        }
                        }
                    } else {
                        // fmove(M) from FPcc
                        switch(regs) {
                        case 0:
                            throw DecodeError{};
                        case 1: // IR only
                        {
                            auto [w, o] = ea_write32(type, reg, 4);
                            return {[w = std::move(w)]() {
                                        w(cpu.FPIAR);
                                        if(cpu.T == 1) {
                                            cpu.must_trace = true;
                                        }
                                    },
                                    o + 2};
                        }
                        case 2: // FPSR only
                        {
                            auto [w, o] = ea_write32(type, reg, 4);
                            return {[w = std::move(w)]() {
                                        w(Get_FPSR());
                                        if(cpu.T == 1) {
                                            cpu.must_trace = true;
                                        }
                                    },
                                    o + 2};
                        }
                        case 3: // FPSR & IR
                        {
                            auto [a, o] =
                                ea_addr(type, reg, cpu.PC + 2, 8, true);
                            return {[a = std::move(a)]() {
                                        cpu.EA = a();
                                        MMU_WriteL(cpu.EA, Get_FPSR());
                                        MMU_WriteL(cpu.EA + 4, cpu.FPIAR);
                                        if(cpu.T == 1) {
                                            cpu.must_trace = true;
                                        }
                                    },
                                    o + 2};
                        }
                        case 4: // FPCR only
                        {
                            auto [w, o] = ea_write32(type, reg, 4);
                            return {[w = std::move(w)]() {
                                        w(Get_FPCR());
                                        if(cpu.T == 1) {
                                            cpu.must_trace = true;
                                        }
                                    },
                                    o + 2};
                        }
                        case 5: // FPCR & IR
                        {
                            auto [a, o] =
                                ea_addr(type, reg, cpu.PC + 2, 8, true);
                            return {[a = std::move(a)]() {
                                        cpu.EA = a();
                                        MMU_WriteL(cpu.EA, Get_FPCR());
                                        MMU_WriteL(cpu.EA + 4, cpu.FPIAR);
                                        if(cpu.T == 1) {
                                            cpu.must_trace = true;
                                        }
                                    },
                                    o + 2};
                        }
                        case 6: // FPCR & FPSR
                        {
                            auto [a, o] =
                                ea_addr(type, reg, cpu.PC + 2, 8, true);
                            return {[a = std::move(a)]() {
                                        cpu.EA = a();
                                        MMU_WriteL(cpu.EA, Get_FPCR());
                                        MMU_WriteL(cpu.EA + 4, Get_FPSR());
                                        if(cpu.T == 1) {
                                            cpu.must_trace = true;
                                        }
                                    },
                                    o + 2};
                        }
                        case 7: // FPCR & FPSR & UR
                        {
                            auto [a, o] =
                                ea_addr(type, reg, cpu.PC + 2, 12, true);
                            return {[a = std::move(a)]() {
                                        cpu.EA = a();
                                        MMU_WriteL(cpu.EA, Get_FPCR());
                                        MMU_WriteL(cpu.EA + 4, Get_FPSR());
                                        MMU_WriteL(cpu.EA + 8, cpu.FPIAR);
                                        if(cpu.T == 1) {
                                            cpu.must_trace = true;
                                        }
                                    },
                                    o + 2};
                        }
                        }
                    }
                } else {
                    if(!fp_k) {
                        // FMOVEM to REGLIST
                        if(!(nextop & 1 << 11)) {
                            // FMOVEM to static regs
                            uint8_t reglist = nextop & 0xff;
                            if(type == 3) {
                                return {[reg, reglist]() {
                                            cpu.A[reg] = fmovem_to_reg(
                                                cpu.A[reg], reglist);
                                            if(cpu.T == 1) {
                                                cpu.must_trace = true;
                                            }
                                        },
                                        2};
                            } else {
                                auto [a, i] =
                                    ea_addr(type, reg, cpu.PC + 2, 0, false);
                                return {[a = std::move(a), reglist]() {
                                            fmovem_to_reg(cpu.EA = a(),
                                                          reglist);
                                            if(cpu.T == 1) {
                                                cpu.must_trace = true;
                                            }
                                        },
                                        2};
                            }
                        } else {
                            // FMOVE to reglist(Dn)
                            int nd = nextop >> 4 & 7;
                            if(type == 3) {
                                return {[reg, nd]() {
                                            uint8_t reglist = cpu.D[nd];
                                            cpu.A[reg] = fmovem_to_reg(
                                                cpu.A[reg], reglist);
                                            if(cpu.T == 1) {
                                                cpu.must_trace = true;
                                            }
                                        },
                                        2};
                            } else {
                                auto [a, i] =
                                    ea_addr(type, reg, cpu.PC + 2, 0, false);
                                return {[a = std::move(a), nd]() {
                                            uint8_t reglist = cpu.D[nd];
                                            fmovem_to_reg(cpu.EA = a(),
                                                          reglist);
                                            if(cpu.T == 1) {
                                                cpu.must_trace = true;
                                            }
                                        },
                                        2};
                            }
                        }
                    } else {
                        if(!(nextop & 1 << 11)) {
                            // FMOVEM from static regs
                            uint8_t reglist = nextop & 0xff;
                            if(type == 4) {
                                return {[reg, reglist]() {
                                            cpu.A[reg] = fmovem_from_reg_rev(
                                                cpu.A[reg], reglist);
                                            if(cpu.T == 1) {
                                                cpu.must_trace = true;
                                            }
                                        },
                                        2};
                            } else {
                                auto [a, i] =
                                    ea_addr(type, reg, cpu.PC + 2, 0, true);
                                return {[a = std::move(a), reglist]() {
                                            fmovem_from_reg(cpu.EA = a(),
                                                            reglist);
                                            if(cpu.T == 1) {
                                                cpu.must_trace = true;
                                            }
                                        },
                                        2};
                            }
                        } else {
                            // FMOVE from reglist(Dn)
                            int nd = nextop >> 4 & 7;
                            if(type == 4) {
                                return {[reg, nd]() {
                                            uint8_t reglist = cpu.D[nd];
                                            cpu.A[reg] = fmovem_from_reg_rev(
                                                cpu.A[reg], reglist);
                                            if(cpu.T == 1) {
                                                cpu.must_trace = true;
                                            }
                                        },
                                        2};
                            } else {
                                auto [a, i] =
                                    ea_addr(type, reg, cpu.PC + 2, 0, true);
                                return {[a = std::move(a), nd]() {
                                            uint8_t reglist = cpu.D[nd];
                                            fmovem_from_reg(cpu.EA = a(),
                                                            reglist);
                                            if(cpu.T == 1) {
                                                cpu.must_trace = true;
                                            }
                                        },
                                        2};
                            }
                        }
                    }
                }
            }
        } else if(sz == 1) {
            if(type == 1) {
                // FDBcc
                uint16_t nextop = FETCH(cpu.PC + 2);
                int16_t offset = FETCH(cpu.PC + 4);
                return {[reg, offset, c = nextop & 0x3f]() {
                            if(c & 1 << 4) {
                                test_bsun();
                            }
                            if(!test_Fcc(c & 0xf)) {
                                int16_t v2 = cpu.D[reg];
                                STORE_W(cpu.D[reg], --v2);
                                if(v2 != -1) {
                                    JUMP(cpu.PC + 4 + offset);
                                }
                            }
                            if(cpu.T == 1) {
                                cpu.must_trace = true;
                            }
                        },
                        4};

            } else if(type == 7 && reg == 2) {
                // FTRAPcc. W
                uint16_t nextop = FETCH(cpu.PC + 2);
                return {[c = nextop & 0x3f]() {
                            if(c & 1 << 4) {
                                test_bsun();
                            }
                            if(test_Fcc(c & 0xf)) {
                                TRAPX();
                            }
                        },
                        4};
            } else if(type == 7 && reg == 3) {
                // FTRAPcc. L
                uint16_t nextop = FETCH(cpu.PC + 2);
                return {[c = nextop & 0x3f]() {
                            if(c & 1 << 4) {
                                test_bsun();
                            }
                            if(test_Fcc(c & 0xf)) {
                                TRAPX();
                            }
                        },
                        6};
            } else if(type == 7 && reg == 4) {
                // FTRAPcc.
                uint16_t nextop = FETCH(cpu.PC + 2);
                return {[c = nextop & 0x3f]() {
                            if(c & 1 << 4) {
                                test_bsun();
                            }
                            if(test_Fcc(c & 0xf)) {
                                TRAPX();
                            }
                        },
                        2};
            } else {
                // FScc
                uint16_t nextop = FETCH(cpu.PC + 2);
                auto [w, i] = ea_write8(type, reg, cpu.PC + 4);
                return {[w = std::move(w), c = nextop & 0x3f]() {
                            if(c & 1 << 4) {
                                test_bsun();
                            }
                            if(test_Fcc(c & 0xf)) {
                                w(0xff);
                            } else {
                                w(0);
                            }
                        },
                        2 + i};
            }
        } else if(sz == 2) {
            // FBcc.W
            int16_t offset = FETCH(cpu.PC + 2);
            return {[offset, c = type << 3 | reg]() {
                        if(c & 1 << 4) {
                            test_bsun();
                        }
                        if(test_Fcc(c & 0xf)) {
                            JUMP(cpu.PC + 2 + offset);
                            if(cpu.T == 1) {
                                cpu.must_trace = true;
                            }
                        }
                    },
                    2};
        } else if(sz == 3) {
            // FBcc.L
            int32_t offset = FETCH32(cpu.PC + 2);
            return {[offset, c = type << 3 | reg]() {
                        if(c & 1 << 4) {
                            test_bsun();
                        }
                        if(test_Fcc(c & 0xf)) {
                            JUMP(cpu.PC + 2 + offset);
                            if(cpu.T == 1) {
                                cpu.must_trace = true;
                            }
                        }
                    },
                    4};
        } else if(sz == 4) {
            // FSAVE
            // always save idle
            auto [w, i] = ea_write32(type, reg, cpu.PC + 2);
            return {[w = std::move(w)]() {
                        if(!cpu.S) {
                            PRIV_ERROR();
                        }
                        if(cpu.T == 1) {
                            cpu.must_trace = true;
                        }
                        w(0x41000000);
                    },
                    i};
        } else if(sz == 5) {
            // FRESTORE
            if(type == 3) {
                return {[reg]() {
                            if(!cpu.S) {
                                PRIV_ERROR();
                            }
                            cpu.EA = cpu.A[reg];
                            uint32_t first = MMU_ReadL(cpu.EA);
                            if(first >> 24 == 00) {
                                reset_fpu();
                                return;
                            } else if(first >> 24 != 0x41) {
                                CPU_RAISE(14);
                            }
                            if(cpu.T == 1) {
                                cpu.must_trace = true;
                            }
                            // only increment reg
                            switch(first >> 16 & 0xff) {
                            case 0x60: // BUSY
                                cpu.A[reg] += 96;
                                break;
                            case 0x30: // UNIMPLEMNET
                                cpu.A[reg] += 48;
                                break;
                            case 0x00: // IDLE
                                cpu.A[reg] += 4;
                                break;
                            default:
                                // unknown frame
                                CPU_RAISE(14);
                            }
                        },
                        2};
            } else {
                auto [a, i] = ea_addr(type, reg, cpu.PC + 2, 0, false);
                return {[a = std::move(a)]() {
                            if(!cpu.S) {
                                PRIV_ERROR();
                            }
                            cpu.EA = a();
                            uint32_t first = MMU_ReadL(cpu.EA);
                            if(first >> 24 == 00) {
                                reset_fpu();
                                return;
                            } else if(first >> 24 != 0x41) {
                                CPU_RAISE(14);
                            }
                            // actually nop
                        },
                        i};
            }
        }
        return {[]() { ILLEGAL_FP(); }, 2};
    } catch(DecodeError &) {
        return {[]() { ILLEGAL_FP(); }, 2};
    }
}

void reset_fpu() {
    mpfr_set_default_prec(64);
    for(int i = 0; i < 8; ++i) {
        mpfr_set_nan(cpu.FP[i]);
        cpu.FP_nan[i] = QNAN_DEFAULT;
    }
    mpfr_set_nan(cpu.fp_tmp);
    cpu.fp_tmp_nan = QNAN_DEFAULT;

    cpu.FPIAR = 0;
    Set_FPCR(0);
    Set_FPSR(0);
}