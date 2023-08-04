#include "68040.hpp"
#include "bus.hpp"
#include "exception.hpp"
#include "gmp.h"
#include "memory.hpp"
#include "mpfr.h"
#include "proto.hpp"
#include <memory>
#include <string.h>
#include <utility>
constexpr uint64_t QNAN_DEFAULT = ~0LLU;
using std::make_unique;
using std::string;
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
        FP_EX_SNAN();
    }
    if(cpu.FPSR.OPERR && cpu.FPCR.OPERR) {
        FP_EX_OPERR();
    }
    if(cpu.FPSR.OVFL && cpu.FPCR.OVFL) {
        FP_EX_OVFL();
    }
    if(cpu.FPSR.UNFL && cpu.FPCR.UNFL) {
        FP_EX_UNFL();
    }
    if(cpu.FPSR.DZ && cpu.FPCR.DZ) {
        FP_EX_DIV0();
    }
    if(cpu.FPSR.INEX1 && cpu.FPCR.INEX1) {
        FP_EX_INEX();
    }
    if(cpu.FPSR.INEX2 && cpu.FPCR.INEX2) {
        FP_EX_INEX();
    }
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
    cpu.FPIAR = cpu.oldpc;
}

inline uint64_t loadLL(uint32_t addr) {
    uint64_t v0 = ReadL(addr);
    v0 <<= 32;
    v0 |= ReadL(addr + 4);
    return v0;
}
inline void WriteLL(uint32_t addr, uint64_t v) {
    WriteL(addr, v >> 32);
    WriteL(addr + 4, v);
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
    uint32_t exp = ReadL(addr);
    bool sm = exp & 0x80000000;
    bool se = exp & 0x40000000;
    uint8_t yy = exp >> 28 & 3;
    uint8_t d16 = exp & 0xf;
    exp = (exp >> 16) & 0xfff;
    uint64_t frac =
        static_cast<uint64_t>(ReadL(addr + 4)) << 32 | ReadL(addr + 8);
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

#if 0

#endif

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
            FP_EX_BSUN();
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
    }
    return false;
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

std::tuple<uint64_t, uint16_t> store_fpX() {
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

uint32_t fmovem_from_reg_rev(uint32_t base, uint8_t regs) {
    for(int i = 7; i >= 0; --i) {
        if(regs & 1 << i) {
            base -= 12;
            mpfr_set(cpu.fp_tmp, cpu.FP[i], cpu.FPCR.RND);
            cpu.fp_tmp_nan = cpu.FP_nan[i];
            auto [frac, exp] = store_fpX();
            WriteW(base, exp);
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
            WriteW(base, exp);
            WriteLL(base + 4, frac);
            base += 12;
        }
    }
    return base;
}

uint32_t fmovem_to_reg(uint32_t base, uint8_t regs) {
    for(int i = 0; i < 8; ++i) {
        if(regs & 1 << (7 - i)) {
            uint16_t exp = ReadW(base);
            uint64_t frac = loadLL(base + 4);
            load_fpX(frac, exp);
            mpfr_swap(cpu.fp_tmp, cpu.FP[i]);
            std::swap(cpu.fp_tmp_nan, cpu.FP_nan[i]);
            base += 12;
        }
    }
    return base;
}

int32_t fpu_storeL() {
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
        mpfr_set_erangeflag();
        return std::numeric_limits<int16_t>::max();
    } else if(v < std::numeric_limits<int16_t>::min()) {
        mpfr_set_erangeflag();
        return std::numeric_limits<int16_t>::min();
    } else {
        return v;
    }
}

int8_t fpu_storeB() {
    if(mpfr_get_exp(cpu.fp_tmp) < -1022) {
        mpfr_set_underflow();
    }
    auto v = mpfr_get_sj(cpu.fp_tmp, cpu.FPCR.RND);
    if(v > std::numeric_limits<int8_t>::max()) {
        mpfr_set_erangeflag();
        return std::numeric_limits<int8_t>::max();
    } else if(v < std::numeric_limits<int8_t>::min()) {
        mpfr_set_erangeflag();
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
uint32_t do_frestore_common() {
    uint32_t first = ReadL(cpu.EA);
    if(first >> 24 == 00) {
        reset_fpu();
        return 0;
    } else if(first >> 24 != 0x41) {
        FORMAT_ERROR();
    }
    if(cpu.T == 1) {
        cpu.must_trace = true;
    }
    return first;
}
constexpr double LOG10_2 = .30102999566398119521;

long mpfr_digit10() {
    mpfr_exp_t e = mpfr_get_exp(cpu.fp_tmp);
    if(e <= 0) {
        return 1;
    }
    return e * LOG10_2;
}
void store_fpP(uint32_t addr, int k) {
    if(k > 18) {
        mpfr_set_erangeflag();
        k = 17;
    }
    normalize_fp(FPU_PREC::X);
    if(mpfr_nan_p(cpu.fp_tmp)) {
        // NAN
        WriteL(addr, 0x7FFF0000);
        WriteLL(addr + 4, cpu.fp_tmp_nan);
        return;
    }
    uint32_t e = mpfr_signbit(cpu.fp_tmp) << 31;
    if(mpfr_inf_p(cpu.fp_tmp)) {
        WriteL(addr, e | 0x7FFF0000);
        WriteLL(addr + 4, 0);
        return;
    }
    if(mpfr_zero_p(cpu.fp_tmp)) {
        WriteL(addr, e | 0);
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
    WriteL(addr, exp);
    WriteLL(addr + 4, v);
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

void fmovecr(int opc, int fpn) {
    fpu_prologue();
    switch(opc) {
    case 0:
        // PI
        mpfr_const_pi(cpu.fp_tmp, cpu.FPCR.RND);
        break;
    case 0xB:
        // log10_2
        mpfr_set_ui(cpu.fp_tmp, 2.0, cpu.FPCR.RND);
        mpfr_log10(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
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
enum class FP_SIZE { LONG, SINGLE, EXT, PACKED, WORD, DOUBLE, BYTE, PACKED2 };

void loadFP(int type, int reg, bool rm, int src) {
    if(rm) {
        switch(FP_SIZE(src)) {
        case FP_SIZE::LONG:
            mpfr_set_si(cpu.fp_tmp, static_cast<int32_t>(ea_readL(type, reg)),
                        cpu.FPCR.RND);
            break;
        case FP_SIZE::SINGLE:
            load_fpS(ea_readL(type, reg));
            break;
        case FP_SIZE::EXT: {
            if(type == 7 && reg == 4) {
                // EXT imm
                cpu.EA = cpu.PC;
                cpu.PC += 12;
            } else {
                cpu.EA = ea_getaddr(type, reg, 12);
            }
            uint16_t exp = ReadW(cpu.EA);
            uint64_t frac = loadLL(cpu.EA + 4);
            load_fpX(frac, exp);
            break;
        }
        case FP_SIZE::PACKED:
            if(type == 7 && reg == 4) {
                // EXT imm
                cpu.EA = cpu.PC;
                cpu.PC += 12;
            } else {
                cpu.EA = ea_getaddr(type, reg, 12);
            }
            load_fpP(cpu.EA);
            break;
        case FP_SIZE::WORD:
            mpfr_set_si(cpu.fp_tmp, static_cast<int16_t>(ea_readW(type, reg)),
                        cpu.FPCR.RND);
            break;
        case FP_SIZE::DOUBLE: {
            if(type == 7 && reg == 4) {
                // EXT imm
                cpu.EA = cpu.PC;
                cpu.PC += 12;
            } else {
                cpu.EA = ea_getaddr(type, reg, 8);
            }
            uint64_t frac = loadLL(cpu.EA);
            load_fpD(frac);
            break;
        }
        case FP_SIZE::BYTE:
            mpfr_set_si(cpu.fp_tmp, static_cast<int8_t>(ea_readB(type, reg)),
                        cpu.FPCR.RND);
            break;
        case FP_SIZE::PACKED2:
            __builtin_unreachable();
        }
    } else {
        mpfr_swap(cpu.FP[src], cpu.fp_tmp);
        std::swap(cpu.FP_nan[src], cpu.fp_tmp_nan);
    }
}
namespace OP {
void fop_do(int opc, int dst) {
    FPU_PREC prec = FPU_PREC::X;
    switch(opc) {
    case 0: // FMOVE
        break;
    case 1: // FINT
        if(test_NAN1()) {
            cpu.fp_tmp_tv = mpfr_rint(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
        }
        break;
    case 2: // FSINH
        if(test_NAN1()) {
            cpu.fp_tmp_tv = mpfr_sinh(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
        }
        break;
    case 3: // FINTRZ
        if(test_NAN1()) {
            cpu.fp_tmp_tv = mpfr_trunc(cpu.fp_tmp, cpu.fp_tmp);
        }
        break;
    case 4:
        // FSQRT
        if(test_NAN1()) {
            cpu.fp_tmp_tv = mpfr_sqrt(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
        }
        break;
    case 6:
        // FLOGNP1
        if(test_NAN1()) {
            cpu.fp_tmp_tv = mpfr_log1p(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
        }
        break;
    case 8:
        // FETOXM1
        if(test_NAN1()) {
            cpu.fp_tmp_tv = mpfr_expm1(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
        }
        break;
    case 9:
        // FTANH
        if(test_NAN1()) {
            cpu.fp_tmp_tv = mpfr_tanh(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
        }
        break;
    case 10:
        // FATAN
        if(test_NAN1()) {
            cpu.fp_tmp_tv = mpfr_atan(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
        }
        break;
    case 12:
        // FASIN
        if(test_NAN1()) {
            cpu.fp_tmp_tv = mpfr_asin(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
        }
        break;
    case 13:
        // FATANH
        if(test_NAN1()) {
            cpu.fp_tmp_tv = mpfr_atanh(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
        }
        break;
    case 14:
        // FSIN
        if(test_NAN1()) {
            cpu.fp_tmp_tv = mpfr_sin(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
        }
        break;
    case 15:
        // FTAN
        if(test_NAN1()) {
            cpu.fp_tmp_tv = mpfr_tan(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
        }
        break;
    case 16:
        // FETOX
        if(test_NAN1()) {
            cpu.fp_tmp_tv = mpfr_exp(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
        }
        break;
    case 17:
        // FTWOTOX
        if(test_NAN1()) {
            cpu.fp_tmp_tv = mpfr_exp2(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
        }
        break;
    case 18:
        // FTENTOX
        if(test_NAN1()) {
            cpu.fp_tmp_tv = mpfr_exp10(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
        }
        break;
    case 20:
        // FLOGN
        if(test_NAN1()) {
            cpu.fp_tmp_tv = mpfr_log(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
        }
        break;
    case 21:
        // FLOG10
        if(test_NAN1()) {
            cpu.fp_tmp_tv = mpfr_log10(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
        }
        break;
    case 22:
        // FLOG2
        if(test_NAN1()) {
            cpu.fp_tmp_tv = mpfr_log2(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
        }
        break;
    case 24:
        // FABS
        if(test_NAN1()) {
            cpu.fp_tmp_tv = mpfr_abs(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
        }
        break;
    case 25:
        // FCOSH
        if(test_NAN1()) {
            cpu.fp_tmp_tv = mpfr_cosh(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
        }
        break;
    case 26:
        // FNEG
        if(test_NAN1()) {
            cpu.fp_tmp_tv = mpfr_neg(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
        }
        break;
    case 28:
        // FACOS
        if(test_NAN1()) {
            cpu.fp_tmp_tv = mpfr_acos(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
        }
        break;
    case 29:
        // FCOS
        if(test_NAN1()) {
            cpu.fp_tmp_tv = mpfr_cos(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
        }
        break;
    case 30:
        // FGETEXP
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
        break;
    case 31:
        // FGETMAN
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
        break;
    case 32:
        // FDIV
        if(test_NAN2(dst)) {
            cpu.fp_tmp_tv =
                mpfr_div(cpu.fp_tmp, cpu.fp_tmp, cpu.FP[dst], cpu.FPCR.RND);
        }
        break;
    case 33:
        // FMOD
        if(test_NAN2(dst)) {
            long q;
            cpu.fp_tmp_tv = mpfr_fmodquo(cpu.fp_tmp, &q, cpu.fp_tmp,
                                         cpu.FP[dst], cpu.FPCR.RND);
            cpu.FPSR.Quat = std::abs(q) & 0x7f;
            cpu.FPSR.QuatSign = q < 0;
        }
        break;
    case 34:
        // FADD
        if(test_NAN2(dst)) {
            cpu.fp_tmp_tv =
                mpfr_add(cpu.fp_tmp, cpu.fp_tmp, cpu.FP[dst], cpu.FPCR.RND);
        }
        break;
    case 35:
        // FMUL
        if(test_NAN2(dst)) {
            cpu.fp_tmp_tv =
                mpfr_mul(cpu.fp_tmp, cpu.fp_tmp, cpu.FP[dst], cpu.FPCR.RND);
        }
        break;
    case 36:
        // FSGLDIV
        if(test_NAN2(dst)) {
            mpfr_prec_round(cpu.fp_tmp, 23, cpu.FPCR.RND);
            mpfr_prec_round(cpu.FP[dst], 23, cpu.FPCR.RND);
            cpu.fp_tmp_tv =
                mpfr_div(cpu.fp_tmp, cpu.fp_tmp, cpu.FP[dst], cpu.FPCR.RND);
        }
        prec = FPU_PREC::S;
        break;
    case 37:
        // FREM
        if(test_NAN2(dst)) {
            long q;

            cpu.fp_tmp_tv = mpfr_remquo(cpu.fp_tmp, &q, cpu.fp_tmp, cpu.FP[dst],
                                        cpu.FPCR.RND);
            cpu.FPSR.Quat = std::abs(q) & 0x7f;
            cpu.FPSR.QuatSign = q < 0;
        }
        break;
    case 38:
        // FSCALE
        if(test_NAN2(dst)) {
            if(mpfr_inf_p(cpu.fp_tmp)) {
                mpfr_set_nan(cpu.fp_tmp);
                cpu.fp_tmp_nan = QNAN_DEFAULT;
            } else {
                cpu.fp_tmp_tv = mpfr_mul_2si(
                    cpu.fp_tmp, cpu.FP[dst],
                    mpfr_get_si(cpu.fp_tmp, cpu.FPCR.RND), cpu.FPCR.RND);
            }
        }
        break;
    case 39:
        // FSGLMUL
        if(test_NAN2(dst)) {
            mpfr_prec_round(cpu.fp_tmp, 23, cpu.FPCR.RND);
            mpfr_prec_round(cpu.FP[dst], 23, cpu.FPCR.RND);
            cpu.fp_tmp_tv =
                mpfr_mul(cpu.fp_tmp, cpu.fp_tmp, cpu.FP[dst], cpu.FPCR.RND);
        }
        prec = FPU_PREC::S;
        break;
    case 40:
        // FSUB
        if(test_NAN2(dst)) {
            cpu.fp_tmp_tv =
                mpfr_sub(cpu.fp_tmp, cpu.fp_tmp, cpu.FP[dst], cpu.FPCR.RND);
        }
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
        if(test_NAN1()) {
            if(fp_c == dst) {
                cpu.fp_tmp_tv = mpfr_sin(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
            } else {
                cpu.fp_tmp_tv = mpfr_sin_cos(cpu.fp_tmp, cpu.FP[fp_c],
                                             cpu.fp_tmp, cpu.FPCR.RND);
            }
        }
        break;
    }
    case 56:
        // FCMP
        if(test_NAN2(dst)) {
            cpu.FPSR.CC_Z = mpfr_equal_p(cpu.FP[dst], cpu.fp_tmp);
            if((mpfr_zero_p(cpu.fp_tmp) && mpfr_zero_p(cpu.FP[dst])) ||
               (mpfr_inf_p(cpu.fp_tmp) && mpfr_inf_p(cpu.FP[dst]))) {
                cpu.FPSR.CC_N = mpfr_signbit(cpu.FP[dst]);
            } else {
                cpu.FPSR.CC_N = mpfr_less_p(cpu.FP[dst], cpu.fp_tmp);
            }
        } else {
            cpu.FPSR.CC_NAN = true;
        }
        fpu_testex();
        return;
    case 58:
        // FTST
        cpu.FPSR.CC_NAN = mpfr_nan_p(cpu.fp_tmp);
        cpu.FPSR.CC_I = mpfr_inf_p(cpu.fp_tmp);
        cpu.FPSR.CC_N = mpfr_signbit(cpu.fp_tmp);
        cpu.FPSR.CC_Z = mpfr_zero_p(cpu.fp_tmp);
        fpu_testex();
        return;
    case 64:
        // FSMOVE
        prec = FPU_PREC::S;
        break;
    case 65:
        // FSSQRT
        if(test_NAN1()) {
            cpu.fp_tmp_tv = mpfr_sqrt(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
        }
        prec = FPU_PREC::S;
        break;
    case 68:
        // FDMOVE
        prec = FPU_PREC::D;
        break;
    case 69:
        // FDSQRT
        if(test_NAN1()) {
            cpu.fp_tmp_tv = mpfr_sqrt(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
        }
        prec = FPU_PREC::D;
        break;
    case 88:
        // FSABS
        if(test_NAN1()) {
            cpu.fp_tmp_tv = mpfr_abs(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
        }
        prec = FPU_PREC::S;
        break;
    case 90:
        // FSNEG
        if(test_NAN1()) {
            cpu.fp_tmp_tv = mpfr_neg(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
        }
        prec = FPU_PREC::S;
        break;
    case 92:
        // FDABS
        if(test_NAN1()) {
            cpu.fp_tmp_tv = mpfr_abs(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
        }
        prec = FPU_PREC::D;
        break;
    case 94:
        // FDNEG
        if(test_NAN1()) {
            cpu.fp_tmp_tv = mpfr_neg(cpu.fp_tmp, cpu.fp_tmp, cpu.FPCR.RND);
        }
        prec = FPU_PREC::D;
        break;
    case 96:
        // FSDIV
        if(test_NAN2(dst)) {
            cpu.fp_tmp_tv =
                mpfr_div(cpu.fp_tmp, cpu.fp_tmp, cpu.FP[dst], cpu.FPCR.RND);
        }
        prec = FPU_PREC::S;
        break;
    case 98:
        // FSADD
        if(test_NAN2(dst)) {
            cpu.fp_tmp_tv =
                mpfr_add(cpu.fp_tmp, cpu.fp_tmp, cpu.FP[dst], cpu.FPCR.RND);
        }
        prec = FPU_PREC::S;
        break;
    case 99:
        // FSMUL
        if(test_NAN2(dst)) {
            cpu.fp_tmp_tv =
                mpfr_mul(cpu.fp_tmp, cpu.fp_tmp, cpu.FP[dst], cpu.FPCR.RND);
        }
        prec = FPU_PREC::S;
        break;
    case 100:
        // FDDIV
        if(test_NAN2(dst)) {
            cpu.fp_tmp_tv =
                mpfr_div(cpu.fp_tmp, cpu.fp_tmp, cpu.FP[dst], cpu.FPCR.RND);
        }
        prec = FPU_PREC::D;
        break;
    case 102:
        // FDADD
        if(test_NAN2(dst)) {
            cpu.fp_tmp_tv =
                mpfr_add(cpu.fp_tmp, cpu.fp_tmp, cpu.FP[dst], cpu.FPCR.RND);
        }
        prec = FPU_PREC::D;
        break;
    case 103:
        // FDMUL
        if(test_NAN2(dst)) {
            cpu.fp_tmp_tv =
                mpfr_mul(cpu.fp_tmp, cpu.fp_tmp, cpu.FP[dst], cpu.FPCR.RND);
        }
        prec = FPU_PREC::D;
        break;

    case 104:
        // FSSUB
        if(test_NAN2(dst)) {
            cpu.fp_tmp_tv =
                mpfr_sub(cpu.fp_tmp, cpu.fp_tmp, cpu.FP[dst], cpu.FPCR.RND);
        }
        prec = FPU_PREC::S;
        break;
    case 108:
        // FDSUB
        if(test_NAN2(dst)) {

            cpu.fp_tmp_tv =
                mpfr_sub(cpu.fp_tmp, cpu.fp_tmp, cpu.FP[dst], cpu.FPCR.RND);
        }
        prec = FPU_PREC::D;
        break;
    default:
        FLINE();
    }
    store_fpr(dst, prec);
}
void fpu_store(int t, int type, int reg, int fpn, int k) {
    mpfr_set(cpu.fp_tmp, cpu.FP[fpn], cpu.FPCR.RND);
    cpu.fp_tmp_nan = cpu.FP_nan[fpn];

    switch(FP_SIZE{t}) {
    case FP_SIZE::LONG:
        ea_writeL(type, reg, fpu_storeL(), false);
        break;
    case FP_SIZE::SINGLE:
        ea_writeL(type, reg, store_fpS(), false);
        break;
    case FP_SIZE::EXT: {
        cpu.EA = ea_getaddr(type, reg, 12);
        auto [frac, exp] = store_fpX();
        WriteW(cpu.EA, exp);
        WriteLL(cpu.EA + 4, frac);
        break;
    }
    case FP_SIZE::PACKED: {
        cpu.EA = ea_getaddr(type, reg, 12);
        store_fpP(cpu.EA, static_cast<int8_t>(k << 1) >> 1);
        break;
    }
    case FP_SIZE::WORD:
        ea_writeW(type, reg, fpu_storeW(), false);
        break;
    case FP_SIZE::DOUBLE:
        cpu.EA = ea_getaddr(type, reg, 8);
        WriteLL(cpu.EA, store_fpD());
        break;
    case FP_SIZE::BYTE:
        ea_writeB(type, reg, fpu_storeB(), false);
        break;
    case FP_SIZE::PACKED2:
        cpu.EA = ea_getaddr(type, reg, 12);
        store_fpP(cpu.EA, static_cast<int8_t>(cpu.D[k >> 4]));
        break;
    default:
        __builtin_unreachable();
    }
    fpu_testex();
}

void fmove_to_fpcc(int type, int reg, unsigned int regs) {
    switch(regs) {
    case 0: // NONE
        break;
    case 1: // IR
        cpu.FPIAR = ea_readL(type, reg);
        break;
    case 2: // FPSR
        Set_FPSR(ea_readL(type, reg));
        break;
    case 4: // FPCR
        Set_FPCR(ea_readL(type, reg));
        break;
    default: {
        uint32_t v = cpu.EA = ea_getaddr(type, reg, 4 * std::popcount(regs));
        if(regs & 4) {
            Set_FPCR(ReadL(v));
            v += 4;
        }
        if(regs & 2) {
            Set_FPSR(ReadL(v));
            v += 4;
        }
        if(regs & 1) {
            cpu.FPIAR = ReadL(v);
        }
        break;
    }
    }
    TRACE_BRANCH();
}

void fmove_from_fpcc(int type, int reg, unsigned int regs) {
    switch(regs) {
    case 0: // NONE
        break;
    case 1: // IR
        ea_writeL(type, reg, cpu.FPIAR, false);
        break;
    case 2: // FPSR
        ea_writeL(type, reg, Get_FPSR(), false);
        break;
    case 4: // FPCR
        ea_writeL(type, reg, Get_FPCR(), false);
        break;
    default: {
        uint32_t v = cpu.EA = ea_getaddr(type, reg, 4 * std::popcount(regs));
        if(regs & 4) {
            WriteL(v, Get_FPCR());
            v += 4;
        }
        if(regs & 2) {
            WriteL(v, Get_FPSR());
            v += 4;
        }
        if(regs & 1) {
            WriteL(v, cpu.FPIAR);
        }
        break;
    }
    }
    TRACE_BRANCH();
}

void fop(uint16_t, int, int type, int reg) {
    fpu_prologue();
    uint16_t extw = FETCH();
    bool rm = extw & 1 << 14;
    unsigned int src = extw >> 10 & 7;
    unsigned int dst = extw >> 7 & 7;
    unsigned int opc = extw & 0x7f;
    if(!(extw & 1 << 15)) {
        if(!(extw & 1 << 13)) {
            if(rm && src == 7) {
                // FMOVECR
                fmovecr(opc, dst);
                return;
            }
            loadFP(type, reg, rm, src);
            fop_do(opc, dst);
        } else {
            // FMOVE to ea
            fpu_store(src, type, reg, dst, opc);
        }
    } else {
        switch(extw >> 13 & 3) {
        case 0:
            // FMOVE(M) to FPcc
            fmove_to_fpcc(type, reg, src);
            break;
        case 1:
            fmove_from_fpcc(type, reg, src);
            break;
        case 2: {
            uint8_t reglist =
                extw & 1 << 11 ? cpu.D[extw >> 4 & 7] : extw & 0xff;
            if(type == 3) {
                cpu.A[reg] = fmovem_to_reg(cpu.A[reg], reglist);
            } else {
                fmovem_to_reg(ea_getaddr(type, reg, 0), reglist);
            }
            TRACE_BRANCH();
            break;
        }
        case 3: {
            uint8_t reglist =
                extw & 1 << 11 ? cpu.D[extw >> 4 & 7] : extw & 0xff;
            if(!(extw & 1 << 12) && type == 4) {
                cpu.A[reg] = fmovem_from_reg_rev(cpu.A[reg], reglist);
            } else {
                fmovem_from_reg(ea_getaddr(type, reg, 0), reglist);
            }
            TRACE_BRANCH();
        }
        }
    }
}
void fscc(uint16_t, int, int type, int reg) {
    uint16_t extw = FETCH();
    if(extw & 1 << 4) {
        test_bsun();
    }
    ea_writeB(type, reg, test_Fcc(extw & 0xf) ? 0xff : 0, false);
}
void fdbcc(uint16_t, int, int, int reg) {
    uint16_t extw = FETCH();
    int16_t offset = FETCH();
    if(extw & 1 << 4) {
        test_bsun();
    }
    if(!test_Fcc(extw & 0xf)) {
        int16_t v2 = cpu.D[reg];
        STORE_W(cpu.D[reg], --v2);
        if(v2 != -1) {
            JUMP(cpu.oldpc + 4 + offset);
        }
    }
    TRACE_BRANCH();
}

void ftrapcc(uint16_t, int, int, int reg) {
    uint16_t extw = FETCH();
    if(reg == 2) {
        FETCH();
    } else if(reg == 3) {
        FETCH32();
    }
    if(extw & 1 << 4) {
        test_bsun();
    }
    if(test_Fcc(extw & 0xf)) {
        TRAPX_ERROR();
    }
}

void fbcc_w(uint16_t op, int, int, int) {
    int c = op & 077;
    int16_t offset = FETCH();
    if(c & 1 << 4) {
        test_bsun();
    }
    if(test_Fcc(c & 0xf)) {
        JUMP(cpu.oldpc + 2 + offset);
        TRACE_BRANCH();
    }
}

void fbcc_l(uint16_t op, int, int, int) {
    int c = op & 077;
    int32_t offset = FETCH32();
    if(c & 1 << 4) {
        test_bsun();
    }
    if(test_Fcc(c & 0xf)) {
        JUMP(cpu.oldpc + 2 + offset);
        TRACE_BRANCH();
    }
}

void fsave(uint16_t, int, int type, int reg) {
    PRIV_CHECK();
    // always save idle
    ea_writeL(type, reg, 0x41000000, false);
    TRACE_BRANCH();
}

void frestore(uint16_t, int, int type, int reg) {
    PRIV_CHECK();
    if(type == 3) {
        cpu.EA = cpu.A[reg];
        auto first = do_frestore_common();
        if(!first) {
            cpu.A[reg] += 4;
            return;
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
            FORMAT_ERROR();
        }
    } else {
        cpu.EA = ea_getaddr(type, reg, 0);
        auto first = do_frestore_common();
        if(!first) {
            reset_fpu();
            return;
        }
        // only check format
        switch(first >> 16 & 0xff) {
        case 0x60: // BUSY
        case 0x30: // UNIMPLEMNET
        case 0x00: // IDLE
            break;
        default:
            // unknown frame
            FORMAT_ERROR();
        }
    }
}
} // namespace OP
extern run_t run_table[0x10000];

void init_run_table_fpu() {
    for(int i = 0; i < 074; ++i) {
        run_table[0171000 | i] = OP::fop;
        run_table[0171100 | i] = OP::fscc;
        run_table[0171400 | i] = OP::fsave;
        run_table[0171500 | i] = OP::frestore;
    }
    for(int k = 0; k < 8; ++k) {
        run_table[0171110 | k] = OP::fdbcc;
    }
    run_table[0171172] = OP::ftrapcc;
    run_table[0171173] = OP::ftrapcc;
    run_table[0171174] = OP::ftrapcc;
    for(int i = 0; i < 0100; ++i) {
        run_table[0171200 | i] = OP::fbcc_w;
        run_table[0171300 | i] = OP::fbcc_l;
    }
}