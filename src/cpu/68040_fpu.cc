#include "68040.hpp"
#include "68040fpu.hpp"
#include "bus.hpp"
#include "exception.hpp"
#include "gmp.h"
#include "inline.hpp"
#include "memory.hpp"
#include "mpfr.h"
#include <memory>
#include <string.h>
#include <utility>
constexpr uint64_t QNAN_DEFAULT = ~0LLU;
using std::make_unique;
using std::string;
void reset_fpu();

std::unique_ptr<Fpu> createFPU(FPU_TYPE) {
    return std::make_unique<M68040FpuFull>();
}
void M68040FpuFull::init() {
    for(int i = 0; i < 8; ++i) {
        mpfr_init2(FP[i], 64);
    }
    mpfr_init2(fp_tmp, 64);
    mpfr_set_default_prec(64);
    fp_tmp_nan = 0;
    FPCR.RND = MPFR_RNDN;
}

void M68040FpuFull::normalize_fp(FPU_PREC p) {
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
    fp_tmp_tv = mpfr_check_range(fp_tmp, fp_tmp_tv, FPCR.RND);
    fp_tmp_tv = mpfr_subnormalize(fp_tmp, fp_tmp_tv, FPCR.RND);
    switch(p) {
    case FPU_PREC::AUTO:
        __builtin_unreachable();
    case FPU_PREC::X:
        fp_tmp_tv = mpfr_prec_round(fp_tmp, 64, FPCR.RND);
        break;
    case FPU_PREC::D:
        fp_tmp_tv = mpfr_prec_round(fp_tmp, 53, FPCR.RND);
        break;
    case FPU_PREC::S:
        fp_tmp_tv = mpfr_prec_round(fp_tmp, 24, FPCR.RND);
        break;
    }
    mpfr_set_emin(-32768);
    mpfr_set_emax(32767);
}

void M68040FpuFull::testex() {
    FPSR.OPERR = mpfr_nanflag_p() || mpfr_erangeflag_p();
    FPSR.OVFL = mpfr_overflow_p();
    FPSR.UNFL = mpfr_underflow_p();
    FPSR.DZ = mpfr_divby0_p();
    FPSR.INEX2 = mpfr_inexflag_p();

    FPSR.EXC_IOP |= (FPSR.S_NAN || FPSR.OPERR);
    FPSR.EXC_OVFL |= (FPSR.OVFL);
    FPSR.EXC_UNFL |= (FPSR.UNFL || FPSR.INEX2);
    FPSR.EXC_DZ |= (FPSR.DZ);
    FPSR.EXC_INEX |= (FPSR.INEX1 || FPSR.INEX2 || FPSR.OVFL);

    if(FPSR.S_NAN && FPCR.S_NAN) {
        FP_EX_SNAN();
    }
    if(FPSR.OPERR && FPCR.OPERR) {
        FP_EX_OPERR();
    }
    if(FPSR.OVFL && FPCR.OVFL) {
        FP_EX_OVFL();
    }
    if(FPSR.UNFL && FPCR.UNFL) {
        FP_EX_UNFL();
    }
    if(FPSR.DZ && FPCR.DZ) {
        FP_EX_DIV0();
    }
    if(FPSR.INEX1 && FPCR.INEX1) {
        FP_EX_INEX();
    }
    if(FPSR.INEX2 && FPCR.INEX2) {
        FP_EX_INEX();
    }
}

void M68040FpuFull::store_fpr(int fpn, FPU_PREC p) {
    FPSR.CC_NAN = mpfr_nan_p(fp_tmp);
    if(FPSR.CC_NAN) {
        if(!(fp_tmp_nan & 1LL << 62)) {
            FPSR.S_NAN = true;
            fp_tmp_nan |= (1LL << 62);
        }
    }
    FPU_PREC pp = p == FPU_PREC::AUTO ? FPCR.PREC : p;
    normalize_fp(pp);
    if(fpn != -1) {
        mpfr_swap(FP[fpn], fp_tmp);
        std::swap(FP_nan[fpn], fp_tmp_nan);
    }
    FPSR.CC_I = mpfr_inf_p(fp_tmp);
    FPSR.CC_Z = mpfr_zero_p(fp_tmp);
    FPSR.CC_N = mpfr_sgn(fp_tmp) < 0;

    testex();
}

bool M68040FpuFull::test_NAN1() {
    if(mpfr_nan_p(fp_tmp)) {
        FPSR.OPERR = true;
        return false;
    }
    return true;
}

bool M68040FpuFull::test_NAN2(int fpn) {
    if(mpfr_nan_p(FP[fpn]) && !mpfr_nan_p(fp_tmp)) {
        FPSR.OPERR = true;
        mpfr_set_nan(fp_tmp);
        fp_tmp_nan = FP_nan[fpn];
        return false;
    } else if(mpfr_nan_p(fp_tmp)) {
        FPSR.OPERR = true;
        return false;
    }
    return true;
}
void M68040FpuFull::prologue() {
    mpfr_clear_flags();
    FPIAR = cpu.oldpc;
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
void M68040FpuFull::loadS(uint32_t v) {
    uint16_t exp = v >> 23 & 0xff;
    uint64_t frac = v & 0x7FFFFF;
    // test NAN
    if(exp == 0xff && frac) {
        mpfr_set_nan(fp_tmp);
        fp_tmp_nan = frac << 41;
    } else {
        auto f = std::bit_cast<float>(v);
        mpfr_set_flt(fp_tmp, f, FPCR.RND);
    }
}

void M68040FpuFull::loadD(uint64_t v) {
    uint16_t exp = v >> 52 & 0x7ff;
    uint64_t frac = v & 0xFFFFFFFFFFFFFLLU;
    // test NAN
    if(exp == 0x7FF && frac) {
        mpfr_set_nan(fp_tmp);
        fp_tmp_nan = frac << 12;
    } else {
        double d = std::bit_cast<double>(v);
        mpfr_set_d(fp_tmp, d, FPCR.RND);
    }
}

void M68040FpuFull::loadX(uint64_t frac, uint16_t exp) {
    bool sign = exp & 0x8000;
    exp &= 0x7fff;
    // consider special number
    if(exp == 0 && frac == 0) {
        // zero
        mpfr_set_zero(fp_tmp, sign ? -1 : 1);
        return;
    } else if(exp == 0x7fff) {
        if(frac == 0) {
            // infinity
            mpfr_set_inf(fp_tmp, sign ? -1 : 1);
        } else {
            // NAN
            mpfr_set_nan(fp_tmp);
            fp_tmp_nan = frac;
        }
        return;
    }
    mpfr_set_uj_2exp(fp_tmp, frac, exp - 16383 - 63, FPCR.RND);
    mpfr_setsign(fp_tmp, fp_tmp, sign, FPCR.RND);
}

void M68040FpuFull::loadP(uint32_t addr) {
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
            mpfr_set_inf(fp_tmp, sm ? -1 : 1);
        } else {
            // NAN
            mpfr_set_nan(fp_tmp);
            fp_tmp_nan = frac;
        }
        return;
    } else if(exp == 0 && frac == 0) {
        // zero
        mpfr_set_zero(fp_tmp, sm ? -1.0 : 1.0);
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
    fp_tmp_tv = mpfr_strtofr(fp_tmp, s, nullptr, 10, FPCR.RND);
    FPSR.INEX1 = (fp_tmp_tv != 0);
}

uint16_t FPCR_t::value() const noexcept {
    uint16_t v = 0;
    switch(RND) {
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
    switch(PREC) {
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
    return v | INEX1 << 8 | INEX2 << 9 | DZ << 10 |
           UNFL << 11 | OVFL << 12 | OPERR << 13 |
           S_NAN << 14 | BSUN << 15;
}

static mpfr_rnd_t MPFR_RNDs[] = {MPFR_RNDN, MPFR_RNDZ, MPFR_RNDD, MPFR_RNDU};
static FPU_PREC MPFR_PRECs[] = {FPU_PREC::X, FPU_PREC::S, FPU_PREC::D,
                                FPU_PREC::X};
void FPCR_t::set(uint16_t v) noexcept {
    RND = MPFR_RNDs[v >> 4 & 3];
    PREC = MPFR_PRECs[v >> 6 & 3];
    INEX1 = v & 1 << 8;
    INEX2 = v & 1 << 9;
    DZ = v & 1 << 10;
    UNFL = v & 1 << 11;
    OVFL = v & 1 << 12;
    OPERR = v & 1 << 13;
    S_NAN = v & 1 << 14;
    BSUN = v & 1 << 15;
}
uint32_t FPSR_t::value() const noexcept {
    return CC_N << 27 | CC_Z << 26 | CC_I << 25 |
           CC_NAN << 24 | QuatSign << 23 | Quat << 16 |
           BSUN << 15 | S_NAN << 14 | OPERR << 13 |
           OVFL << 12 | UNFL << 11 | DZ << 10 | INEX2 << 9 |
           INEX1 << 8 | EXC_IOP << 7 | EXC_OVFL << 6 |
           EXC_UNFL << 5 | EXC_DZ << 4 | EXC_INEX << 3;
}

void FPSR_t::set(uint32_t v) noexcept {
    CC_N = v & 1 << 27;
    CC_Z = v & 1 << 26;
    CC_I = v & 1 << 25;
    CC_NAN = v & 1 << 24;
    QuatSign = v & 1 << 23;
    Quat = v >> 16 & 0x7f;
    BSUN = v & 1 << 15;
    S_NAN = v & 1 << 14;
    OPERR = v & 1 << 13;
    OVFL = v & 1 << 12;
    UNFL = v & 1 << 11;
    DZ = v & 1 << 10;
    INEX2 = v & 1 << 9;
    INEX1 = v & 1 << 8;
    EXC_IOP = v & 1 << 7;
    EXC_OVFL = v & 1 << 6;
    EXC_UNFL = v & 1 << 5;
    EXC_DZ = v & 1 << 4;
    EXC_INEX = v & 1 << 3;
}

void M68040FpuFull::test_bsun() {
    if(FPSR.CC_NAN) {
        FPSR.BSUN = true;
        if(FPCR.BSUN) {
            FP_EX_BSUN();
        }
    }
}

bool M68040FpuFull::test_Fcc(uint8_t v) {
    switch(v) {
    // (S)F
    case 0B0000:
        return false;
    // (S)T
    case 0B1111:
        return true;
    case 0B0001:
        // (S)EQ
        return FPSR.CC_Z;
    case 0B1110:
        // (S)NE
        return !FPSR.CC_Z;
    case 0B0010:
        // (O)GT
        return !(FPSR.CC_NAN || FPSR.CC_Z || FPSR.CC_N);
    case 0B1101:
        // NGT/ULE
        return (FPSR.CC_NAN || FPSR.CC_Z || FPSR.CC_N);
    case 0B0011:
        // (O)GE
        return (FPSR.CC_Z || !(FPSR.CC_NAN || FPSR.CC_N));
    case 0B1100:
        // NGE/ULT
        return (FPSR.CC_NAN || (FPSR.CC_N && !FPSR.CC_Z));
    case 0B0100:
        // (O)LT
        return (FPSR.CC_N && !(FPSR.CC_NAN || FPSR.CC_Z));
    case 0B1011:
        // NLT/UGE
        return (FPSR.CC_NAN || (FPSR.CC_Z && !FPSR.CC_N));
    case 0B0101:
        // (O)LE
        return (FPSR.CC_Z || (FPSR.CC_N && !FPSR.CC_NAN));
    case 0B1010:
        // NLE/UGT
        return (FPSR.CC_NAN || (FPSR.CC_Z && !FPSR.CC_N));
    case 0B0110:
        // O(GL)
        return !(FPSR.CC_NAN || FPSR.CC_Z);
    case 0B1001:
        // NGL/UEQ
        return (FPSR.CC_NAN || FPSR.CC_Z);
    case 0B0111:
        // GLE/OR
        return !FPSR.CC_NAN;
    case 0B1000:
        // NGL/UN
        return FPSR.CC_NAN;
    }
    return false;
}

std::pair<uint64_t, uint16_t> M68040FpuFull::storeX() {
    normalize_fp(FPU_PREC::X);
    if(mpfr_nan_p(fp_tmp)) {
        // NAN
        return {fp_tmp_nan, static_cast<uint16_t>(0x7FFF)};
    }
    uint16_t e = (!!mpfr_signbit(fp_tmp)) << 15;
    if(mpfr_inf_p(fp_tmp)) {
        return {0, e | 0x7FFF};
    }
    if(mpfr_zero_p(fp_tmp)) {
        return {0, e};
    }
    mpfr_exp_t exp;
    mpfr_frexp(&exp, fp_tmp, fp_tmp, FPCR.RND);
    mpfr_abs(fp_tmp, fp_tmp, FPCR.RND);
    exp--;
    if(exp > -16383) {
        // normal
        mpfr_mul_2si(fp_tmp, fp_tmp, 64, FPCR.RND);
        uint64_t frac = mpfr_get_uj(fp_tmp, FPCR.RND);
        return {frac, e | (exp + 16383)};
    } else {
        int sn = -exp - 16383;
        mpfr_mul_2si(fp_tmp, fp_tmp, 64 - sn, FPCR.RND);
        uint64_t frac = mpfr_get_uj(fp_tmp, FPCR.RND);
        return {frac, e};
    }
}

uint32_t M68040FpuFull::fmovem_from_reg_rev(uint32_t base, uint8_t regs) {
    for(int i = 7; i >= 0; --i) {
        if(regs & 1 << i) {
            base -= 12;
            mpfr_set(fp_tmp, FP[i], MPFR_RNDN);
            fp_tmp_nan = FP_nan[i];
            auto [frac, exp] = storeX();
            WriteW(base, exp);
            WriteLL(base + 4, frac);
        }
    }
    return base;
}

uint32_t M68040FpuFull::fmovem_from_reg(uint32_t base, uint8_t regs) {
    for(int i = 0; i < 8; ++i) {
        if(regs & 1 << (7 - i)) {
            mpfr_set(fp_tmp, FP[i], MPFR_RNDN);
            fp_tmp_nan = FP_nan[i];
            auto [frac, exp] = storeX();
            WriteW(base, exp);
            WriteLL(base + 4, frac);
            base += 12;
        }
    }
    return base;
}

uint32_t M68040FpuFull::fmovem_to_reg(uint32_t base, uint8_t regs) {
    for(int i = 0; i < 8; ++i) {
        if(regs & 1 << (7 - i)) {
            uint16_t exp = ReadW(base);
            uint64_t frac = loadLL(base + 4);
            loadX(frac, exp);
            mpfr_swap(fp_tmp, FP[i]);
            std::swap(fp_tmp_nan, FP_nan[i]);
            base += 12;
        }
    }
    return base;
}

int32_t M68040FpuFull::storeL() {
    if(mpfr_get_exp(fp_tmp) < -1022) {
        mpfr_set_erangeflag();
    }
    auto v = mpfr_get_sj(fp_tmp, FPCR.RND);
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

int16_t M68040FpuFull::storeW() {
    if(mpfr_get_exp(fp_tmp) < -1022) {
        mpfr_set_underflow();
    }
    auto v = mpfr_get_sj(fp_tmp, FPCR.RND);
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

int8_t M68040FpuFull::storeB() {
    if(mpfr_get_exp(fp_tmp) < -1022) {
        mpfr_set_underflow();
    }
    auto v = mpfr_get_sj(fp_tmp, FPCR.RND);
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

uint32_t M68040FpuFull::storeS() {
    normalize_fp(FPU_PREC::S);
    if(mpfr_nan_p(fp_tmp)) {
        // NAN
        return 0x7F800000 | (fp_tmp_nan >> 41);
    }
    return std::bit_cast<uint32_t>(mpfr_get_flt(fp_tmp, FPCR.RND));
}

uint64_t M68040FpuFull::storeD() {
    normalize_fp(FPU_PREC::D);
    if(mpfr_nan_p(fp_tmp)) {
        // NAN
        return 0x7FF0000000000000LLU | (fp_tmp_nan >> 12);
    }
    return std::bit_cast<uint64_t>(mpfr_get_d(fp_tmp, FPCR.RND));
}
static uint32_t do_frestore_common(M68040FpuFull *fpu) {
    uint32_t first = ReadL(cpu.EA);
    if(first >> 24 == 00) {
        fpu->reset();
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

long M68040FpuFull::mpfr_digit10() {
    mpfr_exp_t e = mpfr_get_exp(fp_tmp);
    if(e <= 0) {
        return 1;
    }
    return e * LOG10_2;
}
void M68040FpuFull::store_fpP(uint32_t addr, int k) {
    if(k > 18) {
        mpfr_set_erangeflag();
        k = 17;
    }
    normalize_fp(FPU_PREC::X);
    if(mpfr_nan_p(fp_tmp)) {
        // NAN
        WriteL(addr, 0x7FFF0000);
        WriteLL(addr + 4, fp_tmp_nan);
        return;
    }
    uint32_t e = mpfr_signbit(fp_tmp) << 31;
    if(mpfr_inf_p(fp_tmp)) {
        WriteL(addr, e | 0x7FFF0000);
        WriteLL(addr + 4, 0);
        return;
    }
    if(mpfr_zero_p(fp_tmp)) {
        WriteL(addr, e | 0);
        WriteLL(addr + 4, 0);
        return;
    }
    int ln = mpfr_digit10();
    int sn = k <= 0 ? -k + ln : k - 1;
    char buf[96] = {0};
    mpfr_snprintf(buf, 95, "%+.*R*e", sn, FPCR.RND, fp_tmp);
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

void M68040FpuFull::reset() {
    mpfr_set_default_prec(64);
    for(int i = 0; i < 8; ++i) {
        mpfr_set_nan(FP[i]);
        FP_nan[i] = QNAN_DEFAULT;
    }
    mpfr_set_nan(fp_tmp);
    fp_tmp_nan = QNAN_DEFAULT;

    FPIAR = 0;
	FPCR.set(0);
    FPSR.set(0);
}
namespace M68040_Impl {
void fpu_prologue(M68040FpuFull *fpu) { fpu->prologue(); }
void fmovecr_0(M68040FpuFull *fpu, int fpn) {
    mpfr_const_pi(fpu->fp_tmp, fpu->FPCR.RND);
    fpu->store_fpr(fpn);
}
void fmovecr_B(M68040FpuFull *fpu, int fpn) {
    mpfr_set_ui(fpu->fp_tmp, 2, fpu->FPCR.RND);
    mpfr_log10(fpu->fp_tmp, fpu->fp_tmp, fpu->FPCR.RND);
    fpu->store_fpr(fpn);
}
void fmovecr_C(M68040FpuFull *fpu, int fpn) {
    mpfr_set_ui(fpu->fp_tmp, 1, fpu->FPCR.RND);
    mpfr_exp(fpu->fp_tmp, fpu->fp_tmp, fpu->FPCR.RND);
    fpu->store_fpr(fpn);
}
void fmovecr_D(M68040FpuFull *fpu, int fpn) {
    mpfr_set_ui(fpu->fp_tmp, 1, fpu->FPCR.RND);
    mpfr_exp(fpu->fp_tmp, fpu->fp_tmp, fpu->FPCR.RND);
    mpfr_log2(fpu->fp_tmp, fpu->fp_tmp, fpu->FPCR.RND);
    fpu->store_fpr(fpn);
}
void fmovecr_E(M68040FpuFull *fpu, int fpn) {
    mpfr_set_ui(fpu->fp_tmp, 1, fpu->FPCR.RND);
    mpfr_exp(fpu->fp_tmp, fpu->fp_tmp, fpu->FPCR.RND);
    mpfr_log10(fpu->fp_tmp, fpu->fp_tmp, fpu->FPCR.RND);
    fpu->store_fpr(fpn);
}
void fmovecr_F(M68040FpuFull *fpu, int fpn) {
    mpfr_set_zero(fpu->fp_tmp, 1);
    fpu->store_fpr(fpn);
}
void fmovecr_30(M68040FpuFull *fpu, int fpn) {
    mpfr_const_log2(fpu->fp_tmp, fpu->FPCR.RND);
    fpu->store_fpr(fpn);
}
void fmovecr_31(M68040FpuFull *fpu, int fpn) {
    mpfr_set_ui(fpu->fp_tmp, 10, fpu->FPCR.RND);
    mpfr_log(fpu->fp_tmp, fpu->fp_tmp, fpu->FPCR.RND);
    fpu->store_fpr(fpn);
}
void fmovecr_32(M68040FpuFull *fpu, int fpn) {
    mpfr_ui_pow_ui(fpu->fp_tmp, 10, 0, fpu->FPCR.RND);
    fpu->store_fpr(fpn);
}
void fmovecr_33(M68040FpuFull *fpu, int fpn) {
    mpfr_ui_pow_ui(fpu->fp_tmp, 10, 1, fpu->FPCR.RND);
    fpu->store_fpr(fpn);
}
void fmovecr_34(M68040FpuFull *fpu, int fpn) {
    mpfr_ui_pow_ui(fpu->fp_tmp, 10, 2, fpu->FPCR.RND);
    fpu->store_fpr(fpn);
}
void fmovecr_35(M68040FpuFull *fpu, int fpn) {
    mpfr_ui_pow_ui(fpu->fp_tmp, 10, 4, fpu->FPCR.RND);
    fpu->store_fpr(fpn);
}
void fmovecr_36(M68040FpuFull *fpu, int fpn) {
    mpfr_ui_pow_ui(fpu->fp_tmp, 10, 8, fpu->FPCR.RND);
    fpu->store_fpr(fpn);
}
void fmovecr_37(M68040FpuFull *fpu, int fpn) {
    mpfr_ui_pow_ui(fpu->fp_tmp, 10, 16, fpu->FPCR.RND);
    fpu->store_fpr(fpn);
}
void fmovecr_38(M68040FpuFull *fpu, int fpn) {
    mpfr_ui_pow_ui(fpu->fp_tmp, 10, 32, fpu->FPCR.RND);
    fpu->store_fpr(fpn);
}
void fmovecr_39(M68040FpuFull *fpu, int fpn) {
    mpfr_ui_pow_ui(fpu->fp_tmp, 10, 64, fpu->FPCR.RND);
    fpu->store_fpr(fpn);
}
void fmovecr_3A(M68040FpuFull *fpu, int fpn) {
    mpfr_ui_pow_ui(fpu->fp_tmp, 10, 128, fpu->FPCR.RND);
    fpu->store_fpr(fpn);
}
void fmovecr_3B(M68040FpuFull *fpu, int fpn) {
    mpfr_ui_pow_ui(fpu->fp_tmp, 10, 256, fpu->FPCR.RND);
    fpu->store_fpr(fpn);
}
void fmovecr_3C(M68040FpuFull *fpu, int fpn) {
    mpfr_ui_pow_ui(fpu->fp_tmp, 10, 512, fpu->FPCR.RND);
    fpu->store_fpr(fpn);
}
void fmovecr_3D(M68040FpuFull *fpu, int fpn) {
    mpfr_ui_pow_ui(fpu->fp_tmp, 10, 1024, fpu->FPCR.RND);
    fpu->store_fpr(fpn);
}
void fmovecr_3E(M68040FpuFull *fpu, int fpn) {
    mpfr_ui_pow_ui(fpu->fp_tmp, 10, 2048, fpu->FPCR.RND);
    fpu->store_fpr(fpn);
}
void fmovecr_3F(M68040FpuFull *fpu, int fpn) {
    mpfr_ui_pow_ui(fpu->fp_tmp, 10, 4096, fpu->FPCR.RND);
    fpu->store_fpr(fpn);
}
void fload_l(M68040FpuFull *fpu, int32_t value) {
    mpfr_set_si(fpu->fp_tmp, value, fpu->FPCR.RND);
}
void fload_s(M68040FpuFull *fpu, uint32_t value) { fpu->loadS(value); }
void fload_x(M68040FpuFull *fpu) {
    uint16_t exp = ReadW(cpu.EA);
    uint64_t frac = loadLL(cpu.EA + 4);
    fpu->loadX(frac, exp);
}
void fload_p(M68040FpuFull *fpu) { fpu->loadP(cpu.EA); }
void fload_w(M68040FpuFull *fpu, int16_t value) {
    mpfr_set_si(fpu->fp_tmp, value, fpu->FPCR.RND);
}
void fload_d(M68040FpuFull *fpu) {
    uint64_t frac = loadLL(cpu.EA);
    fpu->loadD(frac);
}
void fload_b(M68040FpuFull *fpu, int8_t value) {
    mpfr_set_si(fpu->fp_tmp, value, fpu->FPCR.RND);
}
void fload_r(M68040FpuFull *fpu, int src) {
    mpfr_swap(fpu->FP[src], fpu->fp_tmp);
    std::swap(fpu->FP_nan[src], fpu->fp_tmp_nan);
}
void fmove(M68040FpuFull *fpu, int dst) { fpu->store_fpr(dst, FPU_PREC::X); }
void fint(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN1()) {
        fpu->fp_tmp_tv = mpfr_rint(fpu->fp_tmp, fpu->fp_tmp, fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::X);
}
void fsinh(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN1()) {
        fpu->fp_tmp_tv = mpfr_sinh(fpu->fp_tmp, fpu->fp_tmp, fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::X);
}
void fintrz(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN1()) {
        fpu->fp_tmp_tv = mpfr_trunc(fpu->fp_tmp, fpu->fp_tmp);
    }
    fpu->store_fpr(dst, FPU_PREC::X);
}
void fsqrt(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN1()) {
        fpu->fp_tmp_tv = mpfr_sqrt(fpu->fp_tmp, fpu->fp_tmp, fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::X);
}
void flognp1(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN1()) {
        fpu->fp_tmp_tv = mpfr_log1p(fpu->fp_tmp, fpu->fp_tmp, fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::X);
}
void fetoxm1(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN1()) {
        fpu->fp_tmp_tv = mpfr_expm1(fpu->fp_tmp, fpu->fp_tmp, fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::X);
}
void ftanh(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN1()) {
        fpu->fp_tmp_tv = mpfr_tanh(fpu->fp_tmp, fpu->fp_tmp, fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::X);
}
void fatan(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN1()) {
        fpu->fp_tmp_tv = mpfr_atan(fpu->fp_tmp, fpu->fp_tmp, fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::X);
}
void fasin(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN1()) {
        fpu->fp_tmp_tv = mpfr_asin(fpu->fp_tmp, fpu->fp_tmp, fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::X);
}
void fatanh(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN1()) {
        fpu->fp_tmp_tv = mpfr_atanh(fpu->fp_tmp, fpu->fp_tmp, fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::X);
}
void fsin(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN1()) {
        fpu->fp_tmp_tv = mpfr_sin(fpu->fp_tmp, fpu->fp_tmp, fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::X);
}
void ftan(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN1()) {
        fpu->fp_tmp_tv = mpfr_tan(fpu->fp_tmp, fpu->fp_tmp, fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::X);
}
void fetox(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN1()) {
        fpu->fp_tmp_tv = mpfr_exp(fpu->fp_tmp, fpu->fp_tmp, fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::X);
}
void ftwotox(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN1()) {
        fpu->fp_tmp_tv = mpfr_exp2(fpu->fp_tmp, fpu->fp_tmp, fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::X);
}
void ftentox(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN1()) {
        fpu->fp_tmp_tv = mpfr_exp10(fpu->fp_tmp, fpu->fp_tmp, fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::X);
}
void flogn(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN1()) {
        fpu->fp_tmp_tv = mpfr_log(fpu->fp_tmp, fpu->fp_tmp, fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::X);
}
void flog2(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN1()) {
        fpu->fp_tmp_tv = mpfr_log2(fpu->fp_tmp, fpu->fp_tmp, fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::X);
}
void flog10(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN1()) {
        fpu->fp_tmp_tv = mpfr_log10(fpu->fp_tmp, fpu->fp_tmp, fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::X);
}
void fabs(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN1()) {
        fpu->fp_tmp_tv = mpfr_abs(fpu->fp_tmp, fpu->fp_tmp, fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::X);
}
void fcosh(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN1()) {
        fpu->fp_tmp_tv = mpfr_cosh(fpu->fp_tmp, fpu->fp_tmp, fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::X);
}
void fneg(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN1()) {
        fpu->fp_tmp_tv = mpfr_neg(fpu->fp_tmp, fpu->fp_tmp, fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::X);
}
void facos(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN1()) {
        fpu->fp_tmp_tv = mpfr_acos(fpu->fp_tmp, fpu->fp_tmp, fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::X);
}
void fcos(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN1()) {
        fpu->fp_tmp_tv = mpfr_cos(fpu->fp_tmp, fpu->fp_tmp, fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::X);
}
void fgetexp(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN1()) {
        if(mpfr_inf_p(fpu->fp_tmp)) {
            mpfr_set_nan(fpu->fp_tmp);
            mpfr_set_nanflag();
            fpu->fp_tmp_nan = QNAN_DEFAULT;
        } else if(!mpfr_zero_p(fpu->fp_tmp)) {
            long exp;
            mpfr_get_d_2exp(&exp, fpu->fp_tmp, fpu->FPCR.RND);
            // the result of mpfr_get_d_2exp  is
            // [0.5, 1.0)
            mpfr_set_si(fpu->fp_tmp, exp - 1, fpu->FPCR.RND);
        }
    }
    fpu->store_fpr(dst, FPU_PREC::X);
}
void fgetman(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN1()) {
        if(mpfr_inf_p(fpu->fp_tmp)) {
            mpfr_set_nan(fpu->fp_tmp);
            mpfr_set_nanflag();
            fpu->fp_tmp_nan = QNAN_DEFAULT;
        } else {
            mpfr_exp_t exp;
            fpu->fp_tmp_tv =
                mpfr_frexp(&exp, fpu->fp_tmp, fpu->fp_tmp, fpu->FPCR.RND);
            // the result of mpfr_frexp  is
            // [0.5, 1.0)
            mpfr_mul_si(fpu->fp_tmp, fpu->fp_tmp, 2.0, fpu->FPCR.RND);
        }
    }
    fpu->store_fpr(dst, FPU_PREC::X);
}
void fdiv(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN2(dst)) {
        fpu->fp_tmp_tv =
            mpfr_div(fpu->fp_tmp, fpu->fp_tmp, fpu->FP[dst], fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::X);
}
void fmod(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN2(dst)) {
        long q;
        fpu->fp_tmp_tv = mpfr_fmodquo(fpu->fp_tmp, &q, fpu->fp_tmp,
                                      fpu->FP[dst], fpu->FPCR.RND);
        fpu->FPSR.Quat = std::abs(q) & 0x7f;
        fpu->FPSR.QuatSign = q < 0;
    }
    fpu->store_fpr(dst, FPU_PREC::X);
}

void fadd(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN2(dst)) {
        fpu->fp_tmp_tv =
            mpfr_add(fpu->fp_tmp, fpu->fp_tmp, fpu->FP[dst], fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::X);
}

void fmul(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN2(dst)) {
        fpu->fp_tmp_tv =
            mpfr_mul(fpu->fp_tmp, fpu->fp_tmp, fpu->FP[dst], fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::X);
}
void fsgldiv(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN2(dst)) {
        mpfr_prec_round(fpu->fp_tmp, 23, fpu->FPCR.RND);
        mpfr_prec_round(fpu->FP[dst], 23, fpu->FPCR.RND);
        fpu->fp_tmp_tv =
            mpfr_div(fpu->fp_tmp, fpu->fp_tmp, fpu->FP[dst], fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::S);
}

void frem(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN2(dst)) {
        long q;
        fpu->fp_tmp_tv = mpfr_remquo(fpu->fp_tmp, &q, fpu->fp_tmp, fpu->FP[dst],
                                     fpu->FPCR.RND);
        fpu->FPSR.Quat = std::abs(q) & 0x7f;
        fpu->FPSR.QuatSign = q < 0;
    }
    fpu->store_fpr(dst, FPU_PREC::X);
}

void fscale(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN2(dst)) {
        if(mpfr_inf_p(fpu->fp_tmp)) {
            mpfr_set_nan(fpu->fp_tmp);
            fpu->fp_tmp_nan = QNAN_DEFAULT;
        } else {
            fpu->fp_tmp_tv = mpfr_mul_2si(
                fpu->fp_tmp, fpu->FP[dst],
                mpfr_get_si(fpu->fp_tmp, fpu->FPCR.RND), fpu->FPCR.RND);
        }
    }
    fpu->store_fpr(dst, FPU_PREC::X);
}
void fsglmul(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN2(dst)) {
        mpfr_prec_round(fpu->fp_tmp, 23, fpu->FPCR.RND);
        mpfr_prec_round(fpu->FP[dst], 23, fpu->FPCR.RND);
        fpu->fp_tmp_tv =
            mpfr_mul(fpu->fp_tmp, fpu->fp_tmp, fpu->FP[dst], fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::S);
}
void fsub(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN2(dst)) {
        fpu->fp_tmp_tv =
            mpfr_sub(fpu->fp_tmp, fpu->fp_tmp, fpu->FP[dst], fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::X);
}
void fsincos(M68040FpuFull *fpu, int dst, int fp_c) {
    if(fpu->test_NAN1()) {
        if(fp_c == dst) {
            fpu->fp_tmp_tv = mpfr_sin(fpu->fp_tmp, fpu->fp_tmp, fpu->FPCR.RND);
        } else {
            fpu->fp_tmp_tv = mpfr_sin_cos(fpu->fp_tmp, fpu->FP[fp_c],
                                          fpu->fp_tmp, fpu->FPCR.RND);
        }
    }
    fpu->store_fpr(dst, FPU_PREC::X);
}

void fcmp(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN2(dst)) {
        fpu->FPSR.CC_Z = mpfr_equal_p(fpu->FP[dst], fpu->fp_tmp);
        if((mpfr_zero_p(fpu->fp_tmp) && mpfr_zero_p(fpu->FP[dst])) ||
           (mpfr_inf_p(fpu->fp_tmp) && mpfr_inf_p(fpu->FP[dst]))) {
            fpu->FPSR.CC_N = mpfr_signbit(fpu->FP[dst]);
        } else {
            fpu->FPSR.CC_N = mpfr_less_p(fpu->FP[dst], fpu->fp_tmp);
        }
    } else {
        fpu->FPSR.CC_NAN = true;
    }
    fpu->testex();
}

void ftst(M68040FpuFull *fpu, int) {
    fpu->FPSR.CC_NAN = mpfr_nan_p(fpu->fp_tmp);
    fpu->FPSR.CC_I = mpfr_inf_p(fpu->fp_tmp);
    fpu->FPSR.CC_N = mpfr_signbit(fpu->fp_tmp);
    fpu->FPSR.CC_Z = mpfr_zero_p(fpu->fp_tmp);
    fpu->testex();
}

void fsmove(M68040FpuFull *fpu, int dst) { fpu->store_fpr(dst, FPU_PREC::S); }
void fssqrt(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN1()) {
        fpu->fp_tmp_tv = mpfr_sqrt(fpu->fp_tmp, fpu->fp_tmp, fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::S);
}

void fdmove(M68040FpuFull *fpu, int dst) { fpu->store_fpr(dst, FPU_PREC::D); }
void fdsqrt(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN1()) {
        fpu->fp_tmp_tv = mpfr_sqrt(fpu->fp_tmp, fpu->fp_tmp, fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::D);
}

void fsabs(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN1()) {
        fpu->fp_tmp_tv = mpfr_abs(fpu->fp_tmp, fpu->fp_tmp, fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::S);
}

void fsneg(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN1()) {
        fpu->fp_tmp_tv = mpfr_neg(fpu->fp_tmp, fpu->fp_tmp, fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::S);
}

void fdabs(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN1()) {
        fpu->fp_tmp_tv = mpfr_abs(fpu->fp_tmp, fpu->fp_tmp, fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::D);
}

void fdneg(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN1()) {
        fpu->fp_tmp_tv = mpfr_neg(fpu->fp_tmp, fpu->fp_tmp, fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::D);
}

void fsdiv(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN2(dst)) {
        fpu->fp_tmp_tv =
            mpfr_div(fpu->fp_tmp, fpu->fp_tmp, fpu->FP[dst], fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::S);
}

void fsadd(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN2(dst)) {
        fpu->fp_tmp_tv =
            mpfr_add(fpu->fp_tmp, fpu->fp_tmp, fpu->FP[dst], fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::S);
}

void fsmul(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN2(dst)) {
        fpu->fp_tmp_tv =
            mpfr_mul(fpu->fp_tmp, fpu->fp_tmp, fpu->FP[dst], fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::S);
}

void fddiv(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN2(dst)) {
        fpu->fp_tmp_tv =
            mpfr_div(fpu->fp_tmp, fpu->fp_tmp, fpu->FP[dst], fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::D);
}

void fdadd(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN2(dst)) {
        fpu->fp_tmp_tv =
            mpfr_add(fpu->fp_tmp, fpu->fp_tmp, fpu->FP[dst], fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::D);
}

void fdmul(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN2(dst)) {
        fpu->fp_tmp_tv =
            mpfr_mul(fpu->fp_tmp, fpu->fp_tmp, fpu->FP[dst], fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::D);
}

void fssub(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN2(dst)) {
        fpu->fp_tmp_tv =
            mpfr_sub(fpu->fp_tmp, fpu->fp_tmp, fpu->FP[dst], fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::S);
}

void fdsub(M68040FpuFull *fpu, int dst) {
    if(fpu->test_NAN2(dst)) {
        fpu->fp_tmp_tv =
            mpfr_sub(fpu->fp_tmp, fpu->fp_tmp, fpu->FP[dst], fpu->FPCR.RND);
    }
    fpu->store_fpr(dst, FPU_PREC::D);
}
void fstore_common(M68040FpuFull *fpu, int fpn) {
    mpfr_set(fpu->fp_tmp, fpu->FP[fpn], fpu->FPCR.RND);
    fpu->fp_tmp_nan = fpu->FP_nan[fpn];
}
void fstore_l(M68040FpuFull *fpu, int type, int reg) {
    ea_writeL(type, reg, fpu->storeL());
    fpu->testex();
}
void fstore_s(M68040FpuFull *fpu, int type, int reg) {
    ea_writeL(type, reg, fpu->storeS());
    fpu->testex();
}
void fstore_x(M68040FpuFull *fpu, int type, int reg) {
    cpu.EA = ea_getaddr(type, reg, 12);
    auto [frac, exp] = fpu->storeX();
    WriteW(cpu.EA, exp);
    WriteLL(cpu.EA + 4, frac);
    fpu->testex();
}
void fstore_p(M68040FpuFull *fpu, int type, int reg, int k) {
    cpu.EA = ea_getaddr(type, reg, 12);
    fpu->store_fpP(cpu.EA, k);
    fpu->testex();
}
void fstore_w(M68040FpuFull *fpu, int type, int reg) {
    ea_writeW(type, reg, fpu->storeW());
    fpu->testex();
}
void fstore_d(M68040FpuFull *fpu, int type, int reg) {
    cpu.EA = ea_getaddr(type, reg, 8);
    WriteLL(cpu.EA, fpu->storeD());
    fpu->testex();
}
void fstore_b(M68040FpuFull *fpu, int type, int reg) {
    ea_writeB(type, reg, fpu->storeB());
    fpu->testex();
}
void fstore_p2(M68040FpuFull *fpu, int type, int reg, int k) {
    cpu.EA = ea_getaddr(type, reg, 12);
    fpu->store_fpP(cpu.EA, static_cast<int8_t>(cpu.D[k]));
    fpu->testex();
}

uint32_t get_fpiar(M68040FpuFull *fpu) { return fpu->FPIAR; }
uint32_t get_fpsr(M68040FpuFull *fpu) { return fpu->FPSR.value(); }
uint32_t get_fpcr(M68040FpuFull *fpu) { return fpu->FPCR.value(); }
void set_fpiar(M68040FpuFull *fpu, uint32_t value) { fpu->FPIAR = value; }
void set_fpsr(M68040FpuFull *fpu, uint32_t value) { fpu->FPSR.set(value); }
void set_fpcr(M68040FpuFull *fpu, uint32_t value) { fpu->FPCR.set(value); }
void fsave(int type, int reg) {
    PRIV_CHECK();
    // always save idle
    ea_writeL(type, reg, 0x41000000);
    TRACE_BRANCH();
}
void frestore3(M68040FpuFull *fpu, int reg) {
    PRIV_CHECK();
    cpu.EA = cpu.A[reg];
    auto first = do_frestore_common(fpu);
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
}

void frestore(M68040FpuFull *fpu, int type, int reg) {
    PRIV_CHECK();
    cpu.EA = ea_getaddr(type, reg, 0);
    auto first = do_frestore_common(fpu);
    if(!first) {
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


} // namespace M68040_Impl
void M68040FpuFull::fmovecr(int opc, int fpn) {
    switch(opc) {
    case 0:
        // PI
        M68040_Impl::fmovecr_0(this, fpn);
        return;
    case 0xB:
        // log10_2
        M68040_Impl::fmovecr_B(this, fpn);
        break;
    case 0xC:
        // e
        M68040_Impl::fmovecr_C(this, fpn);
        break;
    case 0xD:
        // log2_e
        M68040_Impl::fmovecr_D(this, fpn);
        break;
    case 0xE:
        // log10_e
        M68040_Impl::fmovecr_E(this, fpn);
        break;
    case 0x30:
        // log2_e
        M68040_Impl::fmovecr_30(this, fpn);
        break;
    case 0x31:
        // ln(10)
        M68040_Impl::fmovecr_31(this, fpn);
        break;
    case 0x32:
        // 10^0
        M68040_Impl::fmovecr_32(this, fpn);
        break;
    case 0x33:
        // 10^1
        M68040_Impl::fmovecr_33(this, fpn);
        break;
    case 0x34:
        // 10^2
        M68040_Impl::fmovecr_34(this, fpn);
        break;
    case 0x35:
        // 10^4
        M68040_Impl::fmovecr_35(this, fpn);
        break;
    case 0x36:
        // 10^8
        M68040_Impl::fmovecr_36(this, fpn);
        break;
    case 0x37:
        // 10^16
        M68040_Impl::fmovecr_37(this, fpn);
        break;
    case 0x38:
        // 10^32
        M68040_Impl::fmovecr_38(this, fpn);
        break;
    case 0x39:
        // 10^64
        M68040_Impl::fmovecr_39(this, fpn);
        break;
    case 0x3A:
        // 10^128
        M68040_Impl::fmovecr_3A(this, fpn);
        break;
    case 0x3B:
        // 10^256
        M68040_Impl::fmovecr_3B(this, fpn);
        break;
    case 0x3C:
        // 10^512
        M68040_Impl::fmovecr_3C(this, fpn);
        break;
    case 0x3D:
        // 10^1024
        M68040_Impl::fmovecr_3D(this, fpn);
        break;
    case 0x3E:
        // 10^2048
        M68040_Impl::fmovecr_3E(this, fpn);
        break;
    case 0x3F:
        // 10^4096
        M68040_Impl::fmovecr_3F(this, fpn);
        break;
    case 0x0F:
    default:
        M68040_Impl::fmovecr_F(this, fpn);
        break;
    }
}

void M68040FpuFull::loadFP(int type, int reg, bool rm, int src) {
    if(rm) {
        switch(FP_SIZE(src)) {
        case FP_SIZE::LONG:
            M68040_Impl::fload_l(this, ea_readL(type, reg));
            break;
        case FP_SIZE::SINGLE:
            M68040_Impl::fload_s(this, ea_readL(type, reg));
            break;
        case FP_SIZE::EXT: {
            if(type == 7 && reg == 4) {
                // EXT imm
                cpu.EA = cpu.PC;
                cpu.PC += 12;
            } else {
                cpu.EA = ea_getaddr(type, reg, 12);
            }
            M68040_Impl::fload_x(this);
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
            M68040_Impl::fload_p(this);
            break;
        case FP_SIZE::WORD:
            M68040_Impl::fload_w(this, ea_readW(type, reg));
            break;
        case FP_SIZE::DOUBLE: {
            if(type == 7 && reg == 4) {
                // EXT imm
                cpu.EA = cpu.PC;
                cpu.PC += 8;
            } else {
                cpu.EA = ea_getaddr(type, reg, 8);
            }
            M68040_Impl::fload_d(this);
            break;
        }
        case FP_SIZE::BYTE:
            M68040_Impl::fload_b(this, ea_readB(type, reg));
            break;
        case FP_SIZE::PACKED2:
            __builtin_unreachable();
        }
    } else {
        M68040_Impl::fload_r(this, src);
    }
}
void M68040FpuFull::fop_do(int opc, int dst) {
    auto fpu = FPU_P();
    switch(opc) {
    case 0: // FMOVE
        M68040_Impl::fmove(fpu, dst);
        break;
    case 1: // FINT
        M68040_Impl::fint(fpu, dst);
        break;
    case 2: // FSINH
        M68040_Impl::fsinh(fpu, dst);
        break;
    case 3: // FINTRZ
        M68040_Impl::fintrz(fpu, dst);
        break;
    case 4: // FSQRT
        M68040_Impl::fsqrt(fpu, dst);
        break;
    case 6: // FLOGNP1
        M68040_Impl::flognp1(fpu, dst);
        break;
    case 8: // FETOXM1
        M68040_Impl::fetoxm1(fpu, dst);
        break;
    case 9: // FTANH
        M68040_Impl::ftanh(fpu, dst);
        break;
    case 10: // FATAN
        M68040_Impl::fatan(fpu, dst);
        break;
    case 12: // FASIN
        M68040_Impl::fasin(fpu, dst);
        break;
    case 13: // FATANH
        M68040_Impl::fatanh(fpu, dst);
        break;
    case 14: // FSIN
        M68040_Impl::fsin(fpu, dst);
        break;
    case 15: // FTAN
        M68040_Impl::ftan(fpu, dst);
        break;
    case 16: // FETOX
        M68040_Impl::fetox(fpu, dst);
        break;
    case 17: // FTWOTOX
        M68040_Impl::ftwotox(fpu, dst);
        break;
    case 18: // FTENTOX
        M68040_Impl::ftentox(fpu, dst);
        break;
    case 20: // FLOGN
        M68040_Impl::flogn(fpu, dst);
        break;
    case 21: // FLOG10
        M68040_Impl::flog10(fpu, dst);
        break;
    case 22: // FLOG2
        M68040_Impl::flog2(fpu, dst);
        break;
    case 24: // FABS
        M68040_Impl::fabs(fpu, dst);
        break;
    case 25: // FCOSH
        M68040_Impl::fcosh(fpu, dst);
        break;
    case 26: // FNEG
        M68040_Impl::fneg(fpu, dst);
        break;
    case 28: // FACOS
        M68040_Impl::facos(fpu, dst);
        break;
    case 29: // FCOS
        M68040_Impl::fcos(fpu, dst);
        break;
    case 30: // FGETEXP
        M68040_Impl::fgetexp(fpu, dst);
        break;
    case 31: // FGETMAN
        M68040_Impl::fgetman(fpu, dst);
        break;
    case 32: // FDIV
        M68040_Impl::fdiv(fpu, dst);
        break;
    case 33: // FMOD
        M68040_Impl::fmod(fpu, dst);
        break;
    case 34: // FADD
        M68040_Impl::fadd(fpu, dst);
        break;
    case 35: // FMUL
        M68040_Impl::fmul(fpu, dst);
        break;
    case 36: // FSGLDIV
        M68040_Impl::fsgldiv(fpu, dst);
        break;
    case 37: // FREM
        M68040_Impl::frem(fpu, dst);
        break;
    case 38: // FSCALE
        M68040_Impl::fscale(fpu, dst);
        break;
    case 39: // FSGLMUL
        M68040_Impl::fsglmul(fpu, dst);
        break;
    case 40: // FSUB
        M68040_Impl::fsub(fpu, dst);
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
        M68040_Impl::fsincos(fpu, dst, fp_c);
        break;
    }
    case 56: // FCMP
        M68040_Impl::fcmp(fpu, dst);
        break;
    case 58: // FTST
        M68040_Impl::ftst(fpu, dst);
        break;
    case 64: // FSMOVE
        M68040_Impl::fsmove(fpu, dst);
        break;
    case 65: // FSSQRT
        M68040_Impl::fssqrt(fpu, dst);
        break;
    case 68: // FDMOVE
        M68040_Impl::fdmove(fpu, dst);
        break;
    case 69: // FDSQRT
        M68040_Impl::fdsqrt(fpu, dst);
        break;
    case 88: // FSABS
        M68040_Impl::fsabs(fpu, dst);
        break;
    case 90: // FSNEG
        M68040_Impl::fsneg(fpu, dst);
        break;
    case 92: // FDABS
        M68040_Impl::fdabs(fpu, dst);
        break;
    case 94: // FDNEG
        M68040_Impl::fdneg(fpu, dst);
        break;
    case 96: // FSDIV
        M68040_Impl::fsdiv(fpu, dst);
        break;
    case 98: // FSADD
        M68040_Impl::fsadd(fpu, dst);
        break;
    case 99: // FSMUL
        M68040_Impl::fsmul(fpu, dst);
        break;
    case 100: // FDDIV
        M68040_Impl::fddiv(fpu, dst);
        break;
    case 102: // FDADD
        M68040_Impl::fdadd(fpu, dst);
        break;
    case 103: // FDMUL
        M68040_Impl::fdmul(fpu, dst);
        break;
    case 104: // FSSUB
        M68040_Impl::fssub(fpu, dst);
        break;
    case 108: // FDSUB
        M68040_Impl::fdsub(fpu, dst);
        break;
    default:
        FLINE();
    }
}
void M68040FpuFull::fpu_store(int t, int type, int reg, int fpn, int k) {
    auto fpu = FPU_P();
    M68040_Impl::fstore_common(fpu, fpn);
    switch(FP_SIZE{t}) {
    case FP_SIZE::LONG:
        M68040_Impl::fstore_l(fpu, type, reg);
        break;
    case FP_SIZE::SINGLE:
        M68040_Impl::fstore_s(fpu, type, reg);
        break;
    case FP_SIZE::EXT:
        M68040_Impl::fstore_x(fpu, type, reg);
        break;
    case FP_SIZE::PACKED:
        M68040_Impl::fstore_p(fpu, type, reg, static_cast<int8_t>(k << 1) >> 1);
        break;
    case FP_SIZE::WORD:
        M68040_Impl::fstore_w(fpu, type, reg);
        break;
    case FP_SIZE::DOUBLE:
        M68040_Impl::fstore_d(fpu, type, reg);
        break;
    case FP_SIZE::BYTE:
        M68040_Impl::fstore_b(fpu, type, reg);
        break;
    case FP_SIZE::PACKED2:
        M68040_Impl::fstore_p2(fpu, type, reg, k >> 4);
        break;
    default:
        __builtin_unreachable();
    }
}

void M68040FpuFull::fmove_to_fpcc(int type, int reg, unsigned int regs) {
    switch(regs) {
    case 0: // NONE
        break;
    case 1: // IR
        M68040_Impl::set_fpiar(this, ea_readL(type, reg));
        break;
    case 2: // FPSR
        M68040_Impl::set_fpsr(this, ea_readL(type, reg));
        break;
    case 4: // FPCR
        M68040_Impl::set_fpcr(this, ea_readL(type, reg));
        break;
    default: {
        uint32_t v = cpu.EA = ea_getaddr(type, reg, 4 * std::popcount(regs));
        if(regs & 4) {
            M68040_Impl::set_fpcr(this, ReadL(v));
            v += 4;
        }
        if(regs & 2) {
            M68040_Impl::set_fpsr(this, ReadL(v));
            v += 4;
        }
        if(regs & 1) {
            M68040_Impl::set_fpiar(this, ReadL(v));
        }
        break;
    }
    }
    TRACE_BRANCH();
}

void M68040FpuFull::fmove_from_fpcc(int type, int reg, unsigned int regs) {
    switch(regs) {
    case 0: // NONE
        break;
    case 1: // IR
        ea_writeL(type, reg, M68040_Impl::get_fpiar(this));
        break;
    case 2: // FPSR
        ea_writeL(type, reg, M68040_Impl::get_fpsr(this));
        break;
    case 4: // FPCR
        ea_writeL(type, reg, M68040_Impl::get_fpcr(this));
        break;
    default: {
        uint32_t v = cpu.EA = ea_getaddr(type, reg, 4 * std::popcount(regs));
        if(regs & 4) {
            WriteL(v, M68040_Impl::get_fpcr(this));
            v += 4;
        }
        if(regs & 2) {
            WriteL(v, M68040_Impl::get_fpsr(this));
            v += 4;
        }
        if(regs & 1) {
            WriteL(v, M68040_Impl::get_fpiar(this));
        }
        break;
    }
    }
    TRACE_BRANCH();
}
namespace OP {

void fop(uint16_t op) {
    auto fpu = static_cast<M68040FpuFull &>(*cpu.fpu);
    fpu.prologue();
    uint16_t extw = FETCH();
    bool rm = extw & 1 << 14;
    unsigned int src = extw >> 10 & 7;
    unsigned int dst = extw >> 7 & 7;
    unsigned int opc = extw & 0x7f;
    if(!(extw & 1 << 15)) {
        if(!(extw & 1 << 13)) {
            if(rm && src == 7) {
                // FMOVECR
                fpu.fmovecr(opc, dst);
                return;
            }
            fpu.loadFP(TYPE(op), REG(op), rm, src);
            fpu.fop_do(opc, dst);
        } else {
            // FMOVE to ea
            fpu.fpu_store(src, TYPE(op), REG(op), dst, opc);
        }
    } else {
        switch(extw >> 13 & 3) {
        case 0:
            // FMOVE(M) to FPcc
            fpu.fmove_to_fpcc(TYPE(op), REG(op), src);
            break;
        case 1:
            fpu.fmove_from_fpcc(TYPE(op), REG(op), src);
            break;
        case 2: {
            uint8_t reglist =
                extw & 1 << 11 ? cpu.D[extw >> 4 & 7] : extw & 0xff;
            if(TYPE(op) == 3) {
                cpu.A[REG(op)] = fpu.fmovem_to_reg(cpu.A[REG(op)], reglist);
            } else {
                fpu.fmovem_to_reg(ea_getaddr(TYPE(op), REG(op), 0), reglist);
            }
            TRACE_BRANCH();
            break;
        }
        case 3: {
            uint8_t reglist =
                extw & 1 << 11 ? cpu.D[extw >> 4 & 7] : extw & 0xff;
            if(!(extw & 1 << 12) && TYPE(op) == 4) {
                cpu.A[REG(op)] =
                    fpu.fmovem_from_reg_rev(cpu.A[REG(op)], reglist);
            } else {
                fpu.fmovem_from_reg(ea_getaddr(TYPE(op), REG(op), 0), reglist);
            }
            TRACE_BRANCH();
        }
        }
    }
}
void fscc(uint16_t op) {
    uint16_t extw = FETCH();
    if(extw & 1 << 4) {
        FPU_P()->test_bsun();
    }
    ea_writeB(TYPE(op), REG(op), FPU_P()->test_Fcc(extw & 0xf) ? 0xff : 0);
}
void fdbcc(uint16_t op) {
    uint16_t extw = FETCH();
    int16_t offset = FETCH();
    if(extw & 1 << 4) {
        FPU_P()->test_bsun();
    }
    if(!FPU_P()->test_Fcc(extw & 0xf)) {
        int16_t v2 = cpu.D[REG(op)];
        STORE_W(cpu.D[REG(op)], --v2);
        if(v2 != -1) {
            JUMP(cpu.oldpc + 4 + offset);
        }
    }
    TRACE_BRANCH();
}

void ftrapcc(uint16_t op) {
    uint16_t extw = FETCH();
    if(REG(op) == 2) {
        FETCH();
    } else if(REG(op) == 3) {
        FETCH32();
    }
    if(extw & 1 << 4) {
        FPU_P()->test_bsun();
    }
    if(FPU_P()->test_Fcc(extw & 0xf)) {
        TRAPX_ERROR();
    }
}

void fbcc_w(uint16_t op) {
    int c = op & 077;
    int16_t offset = FETCH();
    if(c & 1 << 4) {
        FPU_P()->test_bsun();
    }
    if(FPU_P()->test_Fcc(c & 0xf)) {
        JUMP(cpu.oldpc + 2 + offset);
        TRACE_BRANCH();
    }
}

void fbcc_l(uint16_t op) {
    int c = op & 077;
    int32_t offset = FETCH32();
    if(c & 1 << 4) {
        FPU_P()->test_bsun();
    }
    if(FPU_P()->test_Fcc(c & 0xf)) {
        JUMP(cpu.oldpc + 2 + offset);
        TRACE_BRANCH();
    }
}

void fsave(uint16_t op) { M68040_Impl::fsave(TYPE(op), REG(op)); }

void frestore(uint16_t op) {
    if(TYPE(op) == 3) {
        M68040_Impl::frestore3(FPU_P(), REG(op));
    } else {
        M68040_Impl::frestore(FPU_P(), TYPE(op), REG(op));
    }
}
} // namespace OP
extern run_t run_table[0x10000];
std::string M68040FpuFull::dumpReg() {
    std::string s;
    for(int i = 0; i < 8; ++i) {
        mpfr_set(fp_tmp, FP[i], MPFR_RNDN);
        fp_tmp_nan = FP_nan[i];
        auto [f, e] = storeX();
        s += std::format("{:04x}0000{:016x}", e, f);
    }
    s += std::format("{:08x}", FPCR.value());
    s += std::format("{:08x}", FPSR.value());
    s += std::format("{:08x}", FPIAR);
    return s;
}
void M68040FpuFull::init_table() {
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
