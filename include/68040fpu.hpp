#ifndef M68040_HPP_
#define M68040_HPP_
#include "68040.hpp"
enum class FPU_PREC {
    X,
    S,
    D,
    AUTO,
};
struct FPCR_t {
	mpfr_rnd_t RND;
	FPU_PREC PREC;
	bool INEX1, INEX2, DZ, UNFL, OVFL, OPERR, S_NAN, BSUN;

	uint16_t value() const noexcept;
	void set(uint16_t v) noexcept;
};
struct FPSR_t {
	bool CC_NAN, CC_I, CC_Z, CC_N;
	uint8_t Quat;
	bool QuatSign;
	bool INEX1, INEX2, DZ, UNFL, OVFL, OPERR, S_NAN, BSUN;
	bool EXC_INEX, EXC_DZ, EXC_UNFL, EXC_OVFL, EXC_IOP;
	uint32_t value() const noexcept;
	void set(uint32_t v) noexcept;
};
struct M68040FpuFull : Fpu {
    mpfr_t FP[8];
    uint64_t FP_nan[8];

    mpfr_t fp_tmp;
    uint64_t fp_tmp_nan;
    int fp_tmp_tv;

	FPCR_t FPCR;
    FPSR_t FPSR;
    uint32_t FPIAR;

    void init_table() override;
    void init_jit() override;
    void init() override;
    void reset() override;
    std::string dumpReg() override;

    void normalize_fp(FPU_PREC p);
    void testex();
    void fmovecr(int opc, int fpn);
    void fop_do(int opc, int dst);
    void fpu_store(int t, int type, int reg, int fpn, int k);
    uint32_t fmovem_from_reg_rev(uint32_t base, uint8_t regs);
    uint32_t fmovem_from_reg(uint32_t base, uint8_t regs);
    uint32_t fmovem_to_reg(uint32_t base, uint8_t regs);
    void fmove_from_fpcc(int type, int reg, unsigned int regs);
    void fmove_to_fpcc(int type, int reg, unsigned int regs);
    void store_fpr(int fpn, FPU_PREC p = FPU_PREC::AUTO);
    std::pair<uint64_t, uint16_t> storeX();
    uint32_t storeS();
    uint64_t storeD();

    void loadS(uint32_t v);
    void loadD(uint64_t v);
    void loadX(uint64_t frac, uint16_t exp);
    void loadP(uint32_t addr);

    int32_t storeL();
    int16_t storeW();
    int8_t storeB();
    void test_bsun();
    bool test_Fcc(uint8_t v);
    void store_fpP(uint32_t addr, int k);
    void loadFP(int type, int reg, bool rm, int src);
    void prologue();
    bool test_NAN1();
    bool test_NAN2(int fpn);

  private:
    long mpfr_digit10();
};
inline M68040FpuFull *FPU_P() {
    return static_cast<M68040FpuFull *>(cpu.fpu.get());
}
enum class FP_SIZE { LONG, SINGLE, EXT, PACKED, WORD, DOUBLE, BYTE, PACKED2 };

namespace M68040_Impl {
void fpu_prologue(M68040FpuFull *fpu); 
void fmovecr_0(M68040FpuFull *fpu, int fpn);
void fmovecr_B(M68040FpuFull *fpu, int fpn);
void fmovecr_C(M68040FpuFull *fpu, int fpn);
void fmovecr_D(M68040FpuFull *fpu, int fpn);
void fmovecr_E(M68040FpuFull *fpu, int fpn);
void fmovecr_F(M68040FpuFull *fpu, int fpn);
void fmovecr_30(M68040FpuFull *fpu, int fpn);
void fmovecr_31(M68040FpuFull *fpu, int fpn);
void fmovecr_32(M68040FpuFull *fpu, int fpn);
void fmovecr_33(M68040FpuFull *fpu, int fpn);
void fmovecr_34(M68040FpuFull *fpu, int fpn);
void fmovecr_35(M68040FpuFull *fpu, int fpn);
void fmovecr_36(M68040FpuFull *fpu, int fpn);
void fmovecr_37(M68040FpuFull *fpu, int fpn);
void fmovecr_38(M68040FpuFull *fpu, int fpn);
void fmovecr_39(M68040FpuFull *fpu, int fpn);
void fmovecr_3A(M68040FpuFull *fpu, int fpn);
void fmovecr_3B(M68040FpuFull *fpu, int fpn);
void fmovecr_3C(M68040FpuFull *fpu, int fpn);
void fmovecr_3D(M68040FpuFull *fpu, int fpn);
void fmovecr_3E(M68040FpuFull *fpu, int fpn);
void fmovecr_3F(M68040FpuFull *fpu, int fpn);
void fload_l(M68040FpuFull *fpu, int32_t value);
void fload_s(M68040FpuFull *fpu, uint32_t value);
void fload_x(M68040FpuFull *fpu);
void fload_p(M68040FpuFull *fpu);
void fload_w(M68040FpuFull *fpu, int16_t value);
void fload_d(M68040FpuFull *fpu);
void fload_b(M68040FpuFull *fpu, int8_t value);
void fload_r(M68040FpuFull *fpu, int src);
void fmove(M68040FpuFull *fpu, int dst);
void fint(M68040FpuFull *fpu, int dst);
void fsinh(M68040FpuFull *fpu, int dst);
void fintrz(M68040FpuFull *fpu, int dst);
void fsqrt(M68040FpuFull *fpu, int dst);
void flognp1(M68040FpuFull *fpu, int dst);
void fetoxm1(M68040FpuFull *fpu, int dst);
void ftanh(M68040FpuFull *fpu, int dst);
void fatan(M68040FpuFull *fpu, int dst);
void fasin(M68040FpuFull *fpu, int dst);
void fatanh(M68040FpuFull *fpu, int dst);
void fsin(M68040FpuFull *fpu, int dst);
void ftan(M68040FpuFull *fpu, int dst);
void fetox(M68040FpuFull *fpu, int dst);
void ftwotox(M68040FpuFull *fpu, int dst);
void ftentox(M68040FpuFull *fpu, int dst);
void flogn(M68040FpuFull *fpu, int dst);
void flog10(M68040FpuFull *fpu, int dst);
void flog2(M68040FpuFull *fpu, int dst);
void fabs(M68040FpuFull *fpu, int dst);
void fcosh(M68040FpuFull *fpu, int dst);
void fneg(M68040FpuFull *fpu, int dst);
void facos(M68040FpuFull *fpu, int dst);
void fcos(M68040FpuFull *fpu, int dst);
void fgetexp(M68040FpuFull *fpu, int dst);
void fgetman(M68040FpuFull *fpu, int dst);
void fdiv(M68040FpuFull *fpu, int dst);
void fmod(M68040FpuFull *fpu, int dst);
void fadd(M68040FpuFull *fpu, int dst);
void fmul(M68040FpuFull *fpu, int dst);
void fsgldiv(M68040FpuFull *fpu, int dst);
void frem(M68040FpuFull *fpu, int dst);
void fscale(M68040FpuFull *fpu, int dst);
void fsglmul(M68040FpuFull *fpu, int dst);
void fsub(M68040FpuFull *fpu, int dst);
void fsincos(M68040FpuFull *fpu, int dst, int fp_c);
void fcmp(M68040FpuFull *fpu, int dst);
void ftst(M68040FpuFull *fpu, int dst);
void fsmove(M68040FpuFull *fpu, int dst);
void fssqrt(M68040FpuFull *fpu, int dst);
void fdmove(M68040FpuFull *fpu, int dst);
void fdsqrt(M68040FpuFull *fpu, int dst);
void fsabs(M68040FpuFull *fpu, int dst);
void fsneg(M68040FpuFull *fpu, int dst);
void fdabs(M68040FpuFull *fpu, int dst);
void fdneg(M68040FpuFull *fpu, int dst);
void fsdiv(M68040FpuFull *fpu, int dst);
void fsadd(M68040FpuFull *fpu, int dst);
void fsmul(M68040FpuFull *fpu, int dst);
void fddiv(M68040FpuFull *fpu, int dst);
void fdadd(M68040FpuFull *fpu, int dst);
void fdmul(M68040FpuFull *fpu, int dst);
void fssub(M68040FpuFull *fpu, int dst);
void fdsub(M68040FpuFull *fpu, int dst);
void fstore_common(M68040FpuFull *fpu, int fpn);
void fstore_l(M68040FpuFull *fpu, int type, int reg);
void fstore_s(M68040FpuFull *fpu, int type, int reg);
void fstore_x(M68040FpuFull *fpu, int type, int reg);
void fstore_p(M68040FpuFull *fpu, int type, int reg, int k);
void fstore_w(M68040FpuFull *fpu, int type, int reg);
void fstore_d(M68040FpuFull *fpu, int type, int reg);
void fstore_b(M68040FpuFull *fpu, int type, int reg);
void fstore_p2(M68040FpuFull *fpu, int type, int reg, int k);

uint32_t get_fpiar(M68040FpuFull* fpu);
uint32_t get_fpsr(M68040FpuFull* fpu);
uint32_t get_fpcr(M68040FpuFull* fpu);
void set_fpiar(M68040FpuFull* fpu, uint32_t);
void set_fpsr(M68040FpuFull* fpu, uint32_t);
void set_fpcr(M68040FpuFull* fpu, uint32_t);
void fsave(int type, int reg);
void frestore3(M68040FpuFull* fpu, int reg);
void frestore(M68040FpuFull* fpu, int type, int reg);
} // namespace M68040_Impl
#endif
