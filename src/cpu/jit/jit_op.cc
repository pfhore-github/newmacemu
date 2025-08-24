#include "jit_common.hpp"
#include "memory.hpp"
using namespace asmjit;
extern CpuFeatures feature;
void bus_reset();
void do_rte();
void jit_postop();
extern volatile bool testing;
extern std::unordered_map<uint32_t, Label> jumpMap;
void Transfer16(uint32_t from, uint32_t to);
namespace OP {
void stop_impl(uint16_t nw);
}
bool movec_from_cr_impl(uint16_t extw, uint32_t *rn);
bool movec_to_cr_impl(uint16_t extw, uint32_t rn);
namespace JIT_OP {
#ifdef __x86_64__
Label end_lbl;
void update_pc() {
    as->mov(x86::qword_ptr(x86::rsp, 8), x86::rsi);
    as->mov(x86::esi, cpu.PC);
    as->mov(c_pc, x86::esi);
    as->mov(x86::rsi, x86::qword_ptr(x86::rsp, 8));
}
void update_nz() {
    as->setz(CC_Z);
    as->sets(CC_N);
}
void update_vc() {
    as->seto(CC_V);
    as->setc(CC_C);
}
void clear_vc() {
    as->mov(CC_V, 0);
    as->mov(CC_C, 0);
}
void update_vcx() {
    as->seto(CC_V);
    as->setc(x86::dl);
    as->mov(CC_C, x86::dl);
    as->mov(CC_X, x86::dl);
}
void jit_push16() {
    as->sub(SP, 2);
    jit_writeW(SP, x86::ax);
}

void jit_push32() {
    as->push(x86::rax);
    as->sub(SP, 2);
    jit_writeW(SP, x86::ax);

    as->sub(SP, 2);
    as->pop(x86::rax);
    as->ror(x86::eax, 16);
    jit_writeW(SP, x86::ax);
}

void jit_pop16() {
    jit_readW(SP);
    as->add(SP, 2);
}

void jit_pop32() {
    jit_readW(SP);
    as->mov(x86::r14d, x86::eax);
    as->sal(x86::r14d, 16);
    as->add(SP, 2);
    jit_readW(SP);
    as->or_(x86::eax, x86::r14d);
    as->add(SP, 2);
}

void jit_trace_branch() {
    as->test(CC_T, 1);
    jit_if(COND::TRUE, [] { as->mov(CPU_BYTE(must_trace), 1); });
}
void jit_priv_check() {
    as->test(CC_S, 1);
    jit_if(COND::FALSE, [] { as->call(PRIV_ERROR); });
}

void jump() {
    as->test(x86::eax, 1);
    jit_if(COND::TRUE, [] {
        as->mov(ARG1.r32(), x86::eax);
        as->call(ADDRESS_ERROR);
    });
    as->mov(c_pc, x86::eax);
    jit_trace_branch();
    jit_postop();
    as->jmp(end_lbl);
}

void jumpC(uint32_t t) {
    if(t & 1) {
        as->call(ADDRESS_ERROR);
        return;
    }
    as->mov(x86::eax, t);
    as->mov(c_pc, x86::eax);
    jit_trace_branch();
    jit_postop();
    as->jmp(jumpMap[t]);
}

void jit_getCCR() {
    as->mov(x86::al, CC_X);
    as->shl(x86::al, 4);
    as->mov(x86::dl, CC_N);
    as->shl(x86::dl, 3);
    as->mov(x86::cl, CC_Z);
    as->shl(x86::cl, 2);
    as->mov(x86::sil, CC_V);
    as->shl(x86::sil, 1);
    as->mov(x86::dil, CC_C);
    as->or_(x86::al, x86::dl);
    as->or_(x86::cl, x86::sil);
    as->or_(x86::al, x86::cl);
    as->or_(x86::al, x86::dil);
}

void jit_setCCR() {
    as->bt(x86::al, 4);
    as->setc(CC_X);
    as->bt(x86::al, 3);
    as->setc(CC_N);
    as->bt(x86::al, 2);
    as->setc(CC_Z);
    as->bt(x86::al, 1);
    as->setc(CC_V);
    as->bt(x86::al, 0);
    as->setc(CC_C);
}
#ifdef CI
void jit_exit(uint16_t) {
    update_pc();
    as->mov(x86::rdi, reinterpret_cast<intptr_t>(&testing));
    as->mov(x86::al, x86::byte_ptr(x86::rdi));
    as->xor_(x86::al, x86::al);
    as->mov(x86::byte_ptr(x86::rdi), x86::al);
    as->jmp(end_lbl);
}
#endif
void ori_b(uint16_t op) {
    uint8_t v = FETCH();
    ea_readB_jit(TYPE(op), REG(op), true);
    update_pc();
    as->or_(x86::al, v);
    update_nz();
    clear_vc();
    ea_writeB_jit(TYPE(op), REG(op), true);
}

void ori_w(uint16_t op) {
    uint16_t v = FETCH();
    ea_readW_jit(TYPE(op), REG(op), true);
    update_pc();
    as->or_(x86::ax, v);
    update_nz();
    clear_vc();
    ea_writeW_jit(TYPE(op), REG(op), true);
}
void ori_l(uint16_t op) {
    uint32_t v = FETCH32();
    ea_readL_jit(TYPE(op), REG(op), true);
    update_pc();
    as->or_(x86::eax, v);
    update_nz();
    clear_vc();
    ea_writeL_jit(TYPE(op), REG(op), true);
}

void ori_b_ccr(uint16_t) {
    uint16_t v = FETCH();
    update_pc();
    jit_getCCR();
    as->or_(x86::al, v);
    jit_setCCR();
}

void ori_w_sr(uint16_t) {
    uint16_t v = FETCH();
    as->call(PRIV_CHECK);
    update_pc();
    as->call(GetSR);
    as->or_(x86::ax, v);
    as->mov(ARG1.r32(), x86::eax);
    as->call(SetSR);
    jit_trace_branch();
}
void andi_b(uint16_t op) {
    uint8_t v = FETCH();
    ea_readB_jit(TYPE(op), REG(op), true);
    update_pc();
    as->and_(x86::al, v);
    update_nz();
    clear_vc();
    ea_writeB_jit(TYPE(op), REG(op), true);
}

void andi_w(uint16_t op) {
    uint16_t v = FETCH();
    ea_readW_jit(TYPE(op), REG(op), true);
    update_pc();
    as->and_(x86::ax, v);
    update_nz();
    clear_vc();
    ea_writeW_jit(TYPE(op), REG(op), true);
}
void andi_l(uint16_t op) {
    uint32_t v = FETCH32();
    ea_readL_jit(TYPE(op), REG(op), true);
    update_pc();
    as->and_(x86::eax, v);
    update_nz();
    clear_vc();
    ea_writeL_jit(TYPE(op), REG(op), true);
}

void andi_b_ccr(uint16_t) {
    uint16_t v = FETCH();
    update_pc();
    jit_getCCR();
    as->and_(x86::al, v);
    jit_setCCR();
}

void andi_w_sr(uint16_t) {
    uint16_t v = FETCH();
    as->call(PRIV_CHECK);
    update_pc();
    as->call(GetSR);
    as->and_(x86::ax, v);
    as->mov(ARG1.r32(), x86::eax);
    as->call(SetSR);
    jit_trace_branch();
}

void eori_b(uint16_t op) {
    uint8_t v = FETCH();
    ea_readB_jit(TYPE(op), REG(op), true);
    update_pc();
    as->xor_(x86::al, v);
    update_nz();
    clear_vc();
    ea_writeB_jit(TYPE(op), REG(op), true);
}

void eori_w(uint16_t op) {
    uint16_t v = FETCH();
    ea_readW_jit(TYPE(op), REG(op), true);
    update_pc();
    as->xor_(x86::ax, v);
    update_nz();
    clear_vc();
    ea_writeW_jit(TYPE(op), REG(op), true);
}
void eori_l(uint16_t op) {
    uint32_t v = FETCH32();
    ea_readL_jit(TYPE(op), REG(op), true);
    update_pc();
    as->xor_(x86::eax, v);
    update_nz();
    clear_vc();
    ea_writeL_jit(TYPE(op), REG(op), true);
}

void eori_b_ccr(uint16_t) {
    uint16_t v = FETCH();
    update_pc();
    jit_getCCR();
    as->xor_(x86::al, v);
    jit_setCCR();
}

void eori_w_sr(uint16_t) {
    uint16_t v = FETCH();
    as->call(PRIV_CHECK);
    update_pc();
    as->call(GetSR);
    as->xor_(x86::ax, v);
    as->mov(ARG1.r32(), x86::eax);
    as->call(SetSR);
    jit_trace_branch();
    jit_postop();
}

void subi_b(uint16_t op) {
    uint8_t v = FETCH();
    ea_readB_jit(TYPE(op), REG(op), true);
    update_pc();
    as->sub(x86::al, v);
    update_nz();
    update_vcx();
    ea_writeB_jit(TYPE(op), REG(op), true);
}

void subi_w(uint16_t op) {
    uint16_t v = FETCH();
    ea_readW_jit(TYPE(op), REG(op), true);
    update_pc();
    as->sub(x86::ax, v);
    update_nz();
    update_vcx();
    ea_writeW_jit(TYPE(op), REG(op), true);
}
void subi_l(uint16_t op) {
    uint32_t v = FETCH32();
    ea_readL_jit(TYPE(op), REG(op), true);
    update_pc();
    as->sub(x86::eax, v);
    update_nz();
    update_vcx();
    ea_writeL_jit(TYPE(op), REG(op), true);
}

void addi_b(uint16_t op) {
    uint8_t v = FETCH();
    ea_readB_jit(TYPE(op), REG(op), true);
    update_pc();
    as->add(x86::al, v);
    update_nz();
    update_vcx();
    ea_writeB_jit(TYPE(op), REG(op), true);
}

void addi_w(uint16_t op) {
    uint16_t v = FETCH();
    ea_readW_jit(TYPE(op), REG(op), true);
    update_pc();
    as->add(x86::ax, v);
    update_nz();
    update_vcx();
    ea_writeW_jit(TYPE(op), REG(op), true);
}
void addi_l(uint16_t op) {
    uint32_t v = FETCH32();
    ea_readL_jit(TYPE(op), REG(op), true);
    update_pc();
    as->add(x86::eax, v);
    update_nz();
    update_vcx();
    ea_writeL_jit(TYPE(op), REG(op), true);
}

void cmpi_b(uint16_t op) {
    uint8_t v = FETCH();
    ea_readB_jit(TYPE(op), REG(op));
    update_pc();
    as->sub(x86::al, v);
    update_nz();
    update_vc();
}

void cmpi_w(uint16_t op) {
    uint16_t v = FETCH();
    ea_readW_jit(TYPE(op), REG(op));
    update_pc();
    as->sub(x86::ax, v);
    update_nz();
    update_vc();
}
void cmpi_l(uint16_t op) {
    uint32_t v = FETCH32();
    ea_readL_jit(TYPE(op), REG(op));
    update_pc();
    as->sub(x86::eax, v);
    update_nz();
    update_vc();
}

void cmp2_chk2_b(uint16_t op) {
    uint16_t nextop = FETCH();
    bool s = nextop & 1 << 15;
    int rn = nextop >> 12 & 7;
    bool chk2 = nextop & 1 << 11;
    ea_getaddr_jit(TYPE(op), REG(op), 1);
    as->mov(x86::r12d, ARG1.r32());
    update_pc();
    // LOW
    jit_readB(x86::r12d);
    as->mov(x86::r13b, x86::al);
    // HIGH
    as->lea(x86::r12d, x86::ptr(x86::r12d, 1));
    jit_readB(x86::r12d);
    as->mov(x86::sil, x86::al);
    as->mov(x86::dil, DR_B(rn | s << 3));

    as->cmp(x86::dil, x86::r13b);
    as->sete(x86::cl);
    if(s) {
        as->setl(x86::ch);
    } else {
        as->setb(x86::ch);
    }
    as->cmp(x86::sil, x86::dil);
    as->sete(x86::al);
    if(s) {
        as->setl(x86::ah);
    } else {
        as->setb(x86::ah);
    }
    as->or_(x86::ax, x86::cx);
    as->mov(x86::cl, x86::ah);
    as->mov(CC_Z, x86::al);
    as->mov(CC_C, x86::cl);
    if(chk2) {
        as->test(x86::cl, 1);
        jit_if(COND::TRUE, [] { as->call(CHK_ERROR); });
    }
}

void cmp2_chk2_w(uint16_t op) {
    uint16_t nextop = FETCH();
    bool s = nextop & 1 << 15;
    int rn = nextop >> 12 & 7;
    bool chk2 = nextop & 1 << 11;
    ea_getaddr_jit(TYPE(op), REG(op), 2);
    as->mov(x86::r12d, ARG1.r32());
    update_pc();
    jit_readW(x86::r12d);
    as->mov(x86::r13w, x86::ax); // LOW
    as->lea(x86::r12d, x86::dword_ptr(x86::r12d, 2));
    jit_readW(x86::r12d);
    as->mov(x86::si, x86::ax); // HIGH
    as->mov(x86::di, DR_W(rn | s << 3));

    as->cmp(x86::di, x86::r13w);
    as->sete(x86::cl);
    if(s) {
        as->setl(x86::ch);
    } else {
        as->setb(x86::ch);
    }
    as->cmp(x86::si, x86::di);
    as->sete(x86::al);
    if(s) {
        as->setl(x86::ah);
    } else {
        as->setb(x86::ah);
    }
    as->or_(x86::ax, x86::cx);
    as->mov(x86::cl, x86::ah);
    as->mov(CC_Z, x86::al);
    as->mov(CC_C, x86::cl);
    if(chk2) {
        as->test(x86::cl, 1);
        jit_if(COND::TRUE, [] { as->call(CHK_ERROR); });
    }
}

void cmp2_chk2_l(uint16_t op) {
    uint16_t nextop = FETCH();
    bool s = nextop & 1 << 15;
    int rn = nextop >> 12 & 7;
    bool chk2 = nextop & 1 << 11;
    ea_getaddr_jit(TYPE(op), REG(op), 4);
    as->mov(x86::r12d, ARG1.r32());
    update_pc();
    jit_readL(x86::r12d);
    as->mov(x86::r13d, x86::eax); // LOW

    as->lea(x86::r12d, x86::dword_ptr(x86::r12d, 4));
    jit_readL(x86::r12d);
    as->mov(x86::esi, x86::eax); // HIGH
    as->mov(x86::edi, DR_L(rn | s << 3));

    as->cmp(x86::edi, x86::r13d);
    as->sete(x86::cl);
    if(s) {
        as->setl(x86::ch);
    } else {
        as->setb(x86::ch);
    }
    as->cmp(x86::esi, x86::edi);
    as->sete(x86::al);
    if(s) {
        as->setl(x86::ah);
    } else {
        as->setb(x86::ah);
    }
    as->or_(x86::ax, x86::cx);
    as->mov(x86::cl, x86::ah);
    as->mov(CC_Z, x86::al);
    as->mov(CC_C, x86::cl);
    if(chk2) {
        as->test(x86::cl, 1);
        jit_if(COND::TRUE, [] { as->call(CHK_ERROR); });
    }
}
void btst_l_i(uint16_t op) {
    int pos = FETCH() & 31;
    update_pc();
    as->bt(DR_L(REG(op)), pos);
    as->setnc(CC_Z);
}

void btst_b_i(uint16_t op) {
    int pos = FETCH() & 7;
    ea_readB_jit(TYPE(op), REG(op));
    update_pc();
    as->bt(x86::ax, pos);
    as->setnc(CC_Z);
}

void btst_b_i_imm(uint16_t) {
    int pos = FETCH() & 7;
    uint8_t imm = FETCH();
    update_pc();
    as->mov(x86::ax, imm);
    as->bt(x86::ax, pos);
    as->setnc(CC_Z);
}

void btst_l_r(uint16_t op) {
    update_pc();
    as->movzx(x86::ecx, DR_B(DN(op)));
    as->bt(DR_L(REG(op)), x86::ecx);
    as->setnc(CC_Z);
}

void btst_b_r(uint16_t op) {
    ea_readB_jit(TYPE(op), REG(op));
    update_pc();
    as->mov(x86::cl, DR_L(DN(op)));
    as->and_(x86::cx, 7);
    as->bt(x86::ax, x86::cx);
    as->setnc(CC_Z);
}

void btst_b_r_imm(uint16_t op) {
    uint8_t imm = FETCH();
    update_pc();
    as->mov(x86::al, imm);
    as->mov(x86::cl, DR_B(DN(op)));
    as->and_(x86::cx, 7);
    as->bt(x86::ax, x86::cx);
    as->setnc(CC_Z);
}

void bchg_l_i(uint16_t op) {
    int pos = FETCH() & 31;
    update_pc();
    as->btc(DR_L(REG(op)), pos);
    as->setnc(CC_Z);
}

void bchg_b_i(uint16_t op) {
    int pos = FETCH() & 7;
    ea_readB_jit(TYPE(op), REG(op), true);
    update_pc();
    as->btc(x86::ax, pos);
    as->setnc(CC_Z);
    ea_writeB_jit(TYPE(op), REG(op), true);
}

void bchg_l_r(uint16_t op) {
    update_pc();
    as->movzx(x86::ecx, DR_B(DN(op)));
    as->btc(DR_L(REG(op)), x86::ecx);
    as->setnc(CC_Z);
}

void bchg_b_r(uint16_t op) {
    ea_readB_jit(TYPE(op), REG(op), true);
    update_pc();
    as->mov(x86::cl, DR_B(DN(op)));
    as->and_(x86::cx, 7);
    as->btc(x86::ax, x86::cx);
    as->setnc(CC_Z);
    ea_writeB_jit(TYPE(op), REG(op), true);
}

void bclr_l_i(uint16_t op) {
    int pos = FETCH() & 31;
    update_pc();
    as->btr(DR_L(REG(op)), pos);
    as->setnc(CC_Z);
}

void bclr_b_i(uint16_t op) {
    int pos = FETCH() & 7;
    ea_readB_jit(TYPE(op), REG(op), true);
    update_pc();
    as->btr(x86::ax, pos);
    as->setnc(CC_Z);
    ea_writeB_jit(TYPE(op), REG(op), true);
}

void bclr_l_r(uint16_t op) {
    update_pc();
    as->movzx(x86::ecx, DR_B(DN(op)));
    as->btr(DR_L(REG(op)), x86::ecx);
    as->setnc(CC_Z);
}

void bclr_b_r(uint16_t op) {
    ea_readB_jit(TYPE(op), REG(op), true);
    update_pc();
    as->mov(x86::cl, DR_B(DN(op)));
    as->and_(x86::cl, 7);
    as->btr(x86::ax, x86::cx);
    as->setnc(CC_Z);
    ea_writeB_jit(TYPE(op), REG(op), true);
}

void bset_l_i(uint16_t op) {
    int pos = FETCH() & 31;
    update_pc();
    as->bts(DR_L(REG(op)), pos);
    as->setnc(CC_Z);
}

void bset_b_i(uint16_t op) {
    int pos = FETCH() & 7;
    ea_readB_jit(TYPE(op), REG(op), true);
    update_pc();
    as->bts(x86::ax, pos);
    as->setnc(CC_Z);
    ea_writeB_jit(TYPE(op), REG(op), true);
}

void bset_l_r(uint16_t op) {
    update_pc();
    as->movzx(x86::ecx, DR_B(DN(op)));
    as->bts(DR_L(REG(op)), x86::ecx);
    as->setnc(CC_Z);
}

void bset_b_r(uint16_t op) {
    ea_readB_jit(TYPE(op), REG(op), true);
    update_pc();
    as->mov(x86::cl, DR_B(DN(op)));
    as->and_(x86::cl, 7);
    as->bts(x86::al, x86::cl);
    as->setnc(CC_Z);
    ea_writeB_jit(TYPE(op), REG(op), true);
}
void movep_w_load(uint16_t op) {
    int16_t disp = FETCH();
    update_pc();
    as->mov(x86::r13d, AR_L(REG(op)));
    as->lea(x86::r13d, x86::dword_ptr(x86::r13d, disp));

    jit_readB(x86::r13d);
    as->mov(x86::r12b, x86::al);
    as->shl(x86::r12w, 8);
    as->lea(x86::r13d, x86::dword_ptr(x86::r13d, 2));
    jit_readB(x86::r13d);
    as->mov(x86::r12b, x86::al);
    as->mov(DR_W(DN(op)), x86::r12w);
}

void movep_l_load(uint16_t op) {
    int16_t disp = FETCH();
    update_pc();
    as->mov(x86::r13d, AR_L(REG(op)));
    as->lea(x86::r13d, x86::dword_ptr(x86::r13d, disp));
    jit_readB(x86::r13d);
    as->shl(x86::eax, 24);
    as->mov(x86::r11d, x86::eax);

    as->lea(x86::r13d, x86::dword_ptr(x86::r13d, 2));
    jit_readB(x86::r13d);

    as->shl(x86::eax, 16);
    as->or_(x86::r11d, x86::eax);

    as->lea(x86::r13d, x86::dword_ptr(x86::r13d, 2));
    jit_readB(x86::r13d);

    as->shl(x86::ax, 8);
    as->or_(x86::r11w, x86::ax);

    as->lea(x86::r13d, x86::dword_ptr(x86::r13d, 2));
    jit_readB(x86::r13d);

    as->mov(x86::r11b, x86::al);
    as->mov(DR_L(DN(op)), x86::r11d);
}

void movep_w_store(uint16_t op) {
    int16_t disp = FETCH();
    update_pc();
    as->mov(x86::r10d, AR_L(REG(op)));
    as->lea(x86::r10d, x86::dword_ptr(x86::r10d, disp));
    as->mov(x86::r11w, DR_W(DN(op)));

    as->mov(ARG1.r32(), x86::r10d);
    as->mov(x86::dx, x86::r11w);
    as->shr(x86::dx, 8);
    jit_writeB(x86::r10d, x86::dl);

    as->lea(x86::r10d, x86::ptr(x86::r10d, 2));
    jit_writeB(x86::r10d, x86::r11b);
}

void movep_l_store(uint16_t op) {
    int16_t disp = FETCH();
    update_pc();
    as->mov(x86::r12d, AR_L(REG(op)));
    as->lea(x86::r12d, x86::dword_ptr(x86::r12d, disp));

    as->mov(x86::r11d, DR_L(DN(op)));

    as->mov(x86::edx, x86::r11d);
    as->shr(x86::edx, 24);
    jit_writeB(x86::r12d, x86::dl);

    as->lea(x86::r12d, x86::ptr(x86::r12d, 2));
    as->mov(x86::edx, x86::r11d);
    as->shr(x86::edx, 16);
    jit_writeB(x86::r12d, x86::dl);

    as->lea(x86::r12d, x86::ptr(x86::r12d, 2));
    as->mov(x86::edx, x86::r11d);
    as->shr(x86::edx, 8);
    jit_writeB(x86::r12d, x86::dl);

    as->lea(x86::r12d, x86::ptr(x86::r12d, 2));
    as->mov(x86::edx, x86::r11d);
    jit_writeB(x86::r12d, x86::dl);
}

void cas_b(uint16_t op) {
    uint16_t extw = FETCH();
    int du = extw >> 6 & 7;
    int dc = extw & 7;
    ea_readB_jit(TYPE(op), REG(op), true);
    update_pc();
    as->cmp(DR_B(dc), x86::al);
    update_nz();
    update_vc();
    jit_if(
        COND::EQ,
        [du, op]() {
            as->mov(x86::al, DR_B(du));
            ea_writeB_jit(TYPE(op), REG(op), true);
        },
        [dc]() { as->mov(DR_B(dc), x86::al); });
    jit_trace_branch();
}

void cas_w(uint16_t op) {
    uint16_t extw = FETCH();
    int du = extw >> 6 & 7;
    int dc = extw & 7;
    ea_readW_jit(TYPE(op), REG(op), true);
    update_pc();
    as->cmp(DR_W(dc), x86::ax);
    update_nz();
    update_vc();
    jit_if(
        COND::EQ,
        [du, op]() {
            as->mov(x86::ax, DR_W(du));
            ea_writeW_jit(TYPE(op), REG(op), true);
        },
        [dc]() { as->mov(DR_W(dc), x86::ax); });

    jit_trace_branch();
}

void cas_l(uint16_t op) {
    uint16_t extw = FETCH();
    int du = extw >> 6 & 7;
    int dc = extw & 7;
    ea_readL_jit(TYPE(op), REG(op), true);
    update_pc();
    as->cmp(DR_L(dc), x86::eax);
    update_nz();
    update_vc();
    jit_if(
        COND::EQ,
        [du, op]() {
            as->mov(x86::eax, DR_L(du));
            ea_writeL_jit(TYPE(op), REG(op), true);
        },
        [dc]() { as->mov(DR_L(dc), x86::eax); });

    jit_trace_branch();
}

void cas2_w(uint16_t) {
    uint16_t extw1 = FETCH();
    uint16_t extw2 = FETCH();
    int rn1 = extw1 >> 12 & 15;
    int du1 = extw1 >> 6 & 7;
    int dc1 = extw1 & 7;
    int rn2 = extw2 >> 12 & 15;
    int du2 = extw2 >> 6 & 7;
    int dc2 = extw2 & 7;
    update_pc();
    jit_readW(DR_L(rn1));
    as->mov(x86::r12w, x86::ax);
    jit_readW(DR_L(rn2));
    as->mov(x86::r13w, x86::ax);

    as->cmp(x86::r12w, DR_W(dc1));
    update_nz();
    update_vc();
    as->cmp(CC_Z, 1);
    jit_if(COND::EQ, [dc2]() {
        as->cmp(x86::r13w, DR_W(dc2));
        update_nz();
        update_vc();
    });
    as->cmp(CC_Z, 1);
    jit_if(
        COND::EQ,
        [rn1, du1, rn2, du2]() {
            jit_writeW(DR_L(rn1), DR_W(du1));
            jit_writeW(DR_L(rn2), DR_W(du2));
        },
        [dc1, dc2]() {
            as->mov(DR_W(dc1), x86::r12w);
            as->mov(DR_W(dc2), x86::r13w);
        });
    jit_trace_branch();
}

void cas2_l(uint16_t) {
    uint16_t extw1 = FETCH();
    uint16_t extw2 = FETCH();
    int rn1 = extw1 >> 12 & 15;
    int du1 = extw1 >> 6 & 7;
    int dc1 = extw1 & 7;
    int rn2 = extw2 >> 12 & 15;
    int du2 = extw2 >> 6 & 7;
    int dc2 = extw2 & 7;
    update_pc();
    jit_readL(DR_L(rn1));
    as->mov(x86::r12d, x86::eax);
    jit_readL(DR_L(rn2));
    as->mov(x86::r13d, x86::eax);

    as->cmp(x86::r12d, DR_L(dc1));
    update_nz();
    update_vc();
    as->cmp(CC_Z, 1);
    jit_if(COND::EQ, [dc2]() {
        as->cmp(x86::r13d, DR_L(dc2));
        update_nz();
        update_vc();
    });
    as->cmp(CC_Z, 1);
    jit_if(
        COND::EQ,
        [rn1, du1, rn2, du2]() {
            jit_writeL(DR_L(rn1), DR_L(du1));
            jit_writeL(DR_L(rn2), DR_L(du2));
        },
        [dc1, dc2]() {
            as->mov(DR_L(dc1), x86::r12d);
            as->mov(DR_L(dc2), x86::r13d);
        });
    jit_trace_branch();
}

void moves_b(uint16_t op) {
    uint16_t extw = FETCH();
    int rn = extw >> 12 & 15;
    jit_priv_check();
    ea_getaddr_jit(TYPE(op), REG(op), 1);
    update_pc();
    if(extw & 1 << 11) {
        as->mov(ARG2.r8(), DR_B(rn));
        as->call(WriteSB);
    } else {
        as->call(ReadSB);
        as->mov(DR_B(rn), x86::al);
    }
    jit_trace_branch();
}

void moves_w(uint16_t op) {
    uint16_t extw = FETCH();
    int rn = extw >> 12 & 15;
    jit_priv_check();
    ea_getaddr_jit(TYPE(op), REG(op), 2);
    update_pc();
    if(extw & 1 << 11) {
        as->mov(ARG2.r16(), DR_W(rn));
        as->call(WriteSW);
    } else {
        as->call(ReadSW);
        as->mov(DR_W(rn), x86::ax);
    }
    jit_trace_branch();
}

void moves_l(uint16_t op) {
    uint16_t extw = FETCH();
    int rn = extw >> 12 & 15;
    jit_priv_check();
    ea_getaddr_jit(TYPE(op), REG(op), 4);
    update_pc();
    if(extw & 1 << 11) {
        as->mov(ARG2.r32(), DR_L(rn));
        as->call(WriteSL);
    } else {
        as->call(ReadSL);
        as->mov(DR_L(rn), x86::eax);
    }
    jit_trace_branch();
}

void move_b(uint16_t op) {
    int dst_type = op >> 6 & 7;
    ea_readB_jit(TYPE(op), REG(op));
    update_pc();
    as->or_(x86::al, x86::al);
    update_nz();
    clear_vc();
    ea_writeB_jit(dst_type, DN(op), false);
    update_pc();
}

void move_w(uint16_t op) {
    int dst_type = op >> 6 & 7;
    ea_readW_jit(TYPE(op), REG(op));
    update_pc();
    as->or_(x86::ax, x86::ax);
    update_nz();
    clear_vc();
    ea_writeW_jit(dst_type, DN(op), false);
}

void move_l(uint16_t op) {
    int dst_type = op >> 6 & 7;
    ea_readL_jit(TYPE(op), REG(op));
    update_pc();
    as->or_(x86::eax, x86::eax);
    update_nz();
    clear_vc();
    ea_writeL_jit(dst_type, DN(op), false);
}

void movea_w(uint16_t op) {
    ea_readW_jit(TYPE(op), REG(op));
    update_pc();
    as->cwde();
    as->mov(AR_L(DN(op)), x86::eax);
}

void movea_l(uint16_t op) {
    ea_readL_jit(TYPE(op), REG(op));
    update_pc();
    as->mov(AR_L(DN(op)), x86::eax);
}

void move_from_sr(uint16_t op) {
    jit_priv_check();
    update_pc();
    as->call(GetSR);
    ea_writeW_jit(TYPE(op), REG(op), false);
}

void move_from_ccr(uint16_t op) {
    update_pc();
    jit_getCCR();
    as->movzx(x86::ax, x86::al);
    ea_writeW_jit(TYPE(op), REG(op), false);
}

void move_to_sr(uint16_t op) {
    jit_priv_check();
    ea_readW_jit(TYPE(op), REG(op));
    update_pc();
    as->movzx(ARG1.r32(), x86::ax);
    as->call(SetSR);
    jit_trace_branch();
}

void move_to_ccr(uint16_t op) {
    ea_readW_jit(TYPE(op), REG(op));
    update_pc();
    jit_setCCR();
}

void negx_b(uint16_t op) {
    ea_readB_jit(TYPE(op), REG(op), true);
    update_pc();
    as->add(x86::al, CC_X);
    as->neg(x86::al);
    update_nz();
    update_vcx();
    ea_writeB_jit(TYPE(op), REG(op), true);
}

void negx_w(uint16_t op) {
    ea_readW_jit(TYPE(op), REG(op), true);
    update_pc();
    as->add(x86::ax, CC_X);
    as->neg(x86::ax);
    update_nz();
    update_vcx();
    ea_writeW_jit(TYPE(op), REG(op), true);
}

void negx_l(uint16_t op) {
    ea_readL_jit(TYPE(op), REG(op), true);
    update_pc();
    as->add(x86::eax, CC_X);
    as->neg(x86::eax);
    update_nz();
    update_vcx();
    ea_writeL_jit(TYPE(op), REG(op), true);
}

void clr_b(uint16_t op) {
    as->xor_(x86::al, x86::al);
    update_nz();
    clear_vc();
    ea_writeB_jit(TYPE(op), REG(op), false);
    update_pc();
}

void clr_w(uint16_t op) {
    as->xor_(x86::ax, x86::ax);
    update_nz();
    clear_vc();
    ea_writeW_jit(TYPE(op), REG(op), false);
    update_pc();
}

void clr_l(uint16_t op) {
    as->xor_(x86::eax, x86::eax);
    update_nz();
    clear_vc();
    ea_writeL_jit(TYPE(op), REG(op), false);
    update_pc();
}

void neg_b(uint16_t op) {
    ea_readB_jit(TYPE(op), REG(op), true);
    update_pc();
    as->neg(x86::al);
    update_nz();
    update_vcx();
    ea_writeB_jit(TYPE(op), REG(op), true);
}

void neg_w(uint16_t op) {
    ea_readW_jit(TYPE(op), REG(op), true);
    update_pc();
    as->neg(x86::ax);
    update_nz();
    update_vcx();
    ea_writeW_jit(TYPE(op), REG(op), true);
}

void neg_l(uint16_t op) {
    ea_readL_jit(TYPE(op), REG(op), true);
    update_pc();
    as->neg(x86::eax);
    update_nz();
    update_vcx();
    ea_writeL_jit(TYPE(op), REG(op), true);
}

void not_b(uint16_t op) {
    ea_readB_jit(TYPE(op), REG(op), true);
    update_pc();
    as->not_(x86::al);
    as->test(x86::al, x86::al);
    update_nz();
    clear_vc();
    ea_writeB_jit(TYPE(op), REG(op), true);
}

void not_w(uint16_t op) {
    ea_readW_jit(TYPE(op), REG(op), true);
    update_pc();
    as->not_(x86::ax);
    as->test(x86::ax, x86::ax);
    update_nz();
    clear_vc();
    ea_writeW_jit(TYPE(op), REG(op), true);
}

void not_l(uint16_t op) {
    ea_readL_jit(TYPE(op), REG(op), true);
    update_pc();
    as->not_(x86::eax);
    as->test(x86::eax, x86::eax);
    update_nz();
    clear_vc();
    ea_writeL_jit(TYPE(op), REG(op), true);
}
void nbcd(uint16_t op) {
    ea_readB_jit(TYPE(op), REG(op), true);
    update_pc();
    as->add(x86::al, CC_X);
    as->xor_(x86::cx, x86::cx);
    as->cmp(x86::al, 0x9A);
    as->cmove(x86::ax, x86::cx);
    as->neg(x86::al);
    as->lahf();
    as->and_(x86::ah, 0x10);
    as->setnz(x86::dh);
    as->mov(x86::dl, x86::al);
    as->and_(x86::dl, 0xf);
    as->cmp(x86::dl, 9);
    as->seta(x86::dl);
    as->mov(x86::cx, 6);
    as->xor_(x86::si, x86::si);
    as->or_(x86::dl, x86::dh);
    as->cmovnz(x86::si, x86::cx);
    as->sub(x86::al, x86::sil);
    as->mov(x86::cx, 0);
    as->mov(x86::dx, 0x60);
    as->cmp(x86::al, 0x99);
    as->seta(x86::sil);
    as->and_(x86::ah, 1);
    as->setnz(x86::dil);
    as->or_(x86::sil, x86::dil);
    as->setnz(CC_C);
    as->setnz(CC_X);
    as->cmovnz(x86::cx, x86::dx);
    as->sub(x86::al, x86::cl);
    jit_if(COND::TRUE, [] { as->mov(CC_Z, 0); });

    ea_writeB_jit(TYPE(op), REG(op), true);
}

void link_l(uint16_t op) {
    int32_t disp = FETCH32();
    int reg = REG(op);
    update_pc();
    as->mov(x86::eax, AR_L(reg));
    jit_push32();

    as->mov(x86::eax, SP);
    as->mov(AR_L(reg), x86::eax);
    as->add(SP, disp);
}

void swap(uint16_t op) {
    const auto reg = DR_L(REG(op));
    as->mov(x86::eax, reg);
    clear_vc();
    if(feature.x86().hasBMI2()) {
        as->rorx(x86::eax, x86::eax, 16);
    } else {
        as->ror(x86::eax, 16);
    }
    as->test(x86::eax, x86::eax);
    update_nz();
    as->mov(reg, x86::eax);
}

void bkpt(uint16_t) { as->call(ILLEGAL_OP); }

void pea(uint16_t op) {
    ea_getaddr_jit(TYPE(op), REG(op), 0);
    update_pc();
    as->mov(x86::eax, ARG1.r32());
    jit_push32();
}

void ext_w(uint16_t op) {
    int reg = REG(op);
    as->movsx(x86::ax, DR_B(reg));
    as->test(x86::ax, x86::ax);
    update_nz();
    clear_vc();
    as->mov(DR_W(reg), x86::ax);
}

void ext_l(uint16_t op) {
    int reg = REG(op);
    as->movsx(x86::eax, DR_W(reg));
    as->test(x86::eax, x86::eax);
    update_nz();
    clear_vc();
    as->mov(DR_L(reg), x86::eax);
}
void movem_w_store_decr(uint16_t op) {
    uint16_t regs = FETCH();
    const auto reg = AR_L(REG(op));
    as->mov(x86::al, CPU_BYTE(movem_run));
    as->test(x86::al, x86::al);
    jit_if(COND::FALSE, [reg]() {
        as->mov(x86::edx, reg);
        as->lea(x86::rdx, x86::ptr(x86::rdx, -2));
        as->mov(reg, x86::edx);
        as->mov(EA, x86::edx);
        as->inc(CPU_BYTE(movem_run));
    });
    as->mov(x86::r12d, EA);
    for(int i = 0; i < 16; ++i) {
        if(regs & 1 << i) {
            jit_writeW(x86::r12d, DR_L(15 - i));
            as->lea(x86::r12d, x86::ptr(x86::r12d, -2));
        }
    }
    as->lea(x86::r12d, x86::ptr(x86::r12d, 2));
    as->mov(reg, x86::r12d);
    as->dec(CPU_BYTE(movem_run));
}
void movem_w_store_base(uint16_t op) {
    uint16_t regs = FETCH();
    as->mov(x86::al, CPU_BYTE(movem_run));
    as->test(x86::al, x86::al);
    jit_if(COND::FALSE, [op]() {
        ea_getaddr_jit(TYPE(op), REG(op), 2);
        as->mov(x86::r12d, ARG1.r32());
        as->inc(CPU_BYTE(movem_run));
    });
    as->mov(x86::r12d, EA);
    for(int i = 0; i < 16; ++i) {
        if(regs & 1 << i) {
            jit_writeW(x86::r12d, DR_L(i));
            as->lea(x86::r12d, x86::ptr(x86::r12d, 2));
        }
    }
    as->dec(CPU_BYTE(movem_run));
}
void movem_l_store_decr(uint16_t op) {
    uint16_t regs = FETCH();
    const auto reg = AR_L(REG(op));
    as->mov(x86::al, CPU_BYTE(movem_run));
    as->test(x86::al, x86::al);
    jit_if(COND::FALSE, [reg]() {
        as->mov(x86::edx, reg);
        as->lea(x86::rdx, x86::ptr(x86::rdx, -4));
        as->mov(reg, x86::edx);
        as->mov(EA, x86::edx);
        as->inc(CPU_BYTE(movem_run));
    });

    as->mov(x86::r12d, EA);
    for(int i = 0; i < 16; ++i) {
        if(regs & 1 << i) {
            jit_writeL(x86::r12d, DR_L(15 - i));
            as->lea(x86::r12d, x86::ptr(x86::r12d, -4));
        }
    }
    as->lea(x86::r12d, x86::ptr(x86::r12d, 4));
    as->mov(reg, x86::r12d);
    as->dec(CPU_BYTE(movem_run));
}
void movem_l_store_base(uint16_t op) {
    uint16_t regs = FETCH();
    as->mov(x86::al, CPU_BYTE(movem_run));
    as->test(x86::al, x86::al);
    jit_if(COND::FALSE, [op]() {
        ea_getaddr_jit(TYPE(op), REG(op), 4);
        as->mov(x86::r12d, ARG1.r32());
        as->inc(CPU_BYTE(movem_run));
    });

    as->mov(x86::r12d, EA);
    for(int i = 0; i < 16; ++i) {
        if(regs & 1 << i) {
            jit_writeL(x86::r12d, DR_L(i));
            as->lea(x86::r12d, x86::ptr(x86::r12d, 4));
        }
    }
    as->dec(CPU_BYTE(movem_run));
}

void movem_w_load_incr(uint16_t op) {
    uint16_t regs = FETCH();
    const auto reg = AR_L(REG(op));
    as->mov(x86::al, CPU_BYTE(movem_run));
    as->test(x86::al, x86::al);
    jit_if(COND::FALSE, [reg]() {
        as->mov(x86::edx, reg);
        as->mov(EA, x86::edx);
        as->inc(CPU_BYTE(movem_run));
    });

    as->mov(x86::r12d, EA);
    as->mov(reg, x86::r12d);
    for(int i = 0; i < 16; ++i) {
        if(regs & 1 << i) {
            jit_readW(x86::r12d);
            if(i > 7) {
                as->cwde();
                as->mov(AR_L(i & 7), x86::eax);
            } else {
                as->mov(DR_W(i), x86::ax);
            }
            as->lea(x86::r12d, x86::ptr(x86::r12d, 2));
            as->mov(reg, x86::r12d);
        }
    }
    as->dec(CPU_BYTE(movem_run));
}
void movem_w_load_base(uint16_t op) {
    uint16_t regs = FETCH();
    as->mov(x86::al, CPU_BYTE(movem_run));
    as->test(x86::al, x86::al);
    jit_if(COND::FALSE, [op]() {
        ea_getaddr_jit(TYPE(op), REG(op), 2);
        as->mov(x86::r12d, ARG1.r32());
        as->inc(CPU_BYTE(movem_run));
    });

    as->mov(x86::r12d, EA);
    for(int i = 0; i < 16; ++i) {
        if(regs & 1 << i) {
            jit_readW(x86::r12d);
            if(i > 7) {
                as->cwde();
                as->mov(AR_L(i & 7), x86::eax);
            } else {
                as->mov(DR_W(i), x86::ax);
            }
            as->lea(x86::r12d, x86::ptr(x86::r12d, 2));
        }
    }
    as->dec(CPU_BYTE(movem_run));
}
void movem_l_load_incr(uint16_t op) {
    uint16_t regs = FETCH();
    const auto reg = AR_L(REG(op));
    as->mov(x86::al, CPU_BYTE(movem_run));
    as->test(x86::al, x86::al);
    jit_if(COND::FALSE, [reg]() {
        as->mov(x86::edx, reg);
        as->mov(EA, x86::edx);
        as->inc(CPU_BYTE(movem_run));
    });

    as->mov(x86::r12d, EA);
    as->mov(reg, x86::r12d);
    for(int i = 0; i < 16; ++i) {
        if(regs & 1 << i) {
            jit_readL(x86::r12d);
            as->mov(DR_W(i), x86::eax);
            as->lea(x86::r12d, x86::ptr(x86::r12d, 4));
            as->mov(reg, x86::r12d);
        }
    }
    as->dec(CPU_BYTE(movem_run));
}
void movem_l_load_base(uint16_t op) {
    uint16_t regs = FETCH();
    as->mov(x86::al, CPU_BYTE(movem_run));
    as->test(x86::al, x86::al);
    jit_if(COND::FALSE, [op]() {
        ea_getaddr_jit(TYPE(op), REG(op), 4);
        as->mov(x86::r12d, ARG1.r32());
        as->inc(CPU_BYTE(movem_run));
    });

    as->mov(x86::r12d, EA);
    for(int i = 0; i < 16; ++i) {
        if(regs & 1 << i) {
            jit_readL(x86::r12d);
            as->mov(DR_L(i), x86::eax);
            as->lea(x86::r12d, x86::ptr(x86::r12d, 4));
        }
    }
    as->dec(CPU_BYTE(movem_run));
}

void tst_b(uint16_t op) {
    ea_readB_jit(TYPE(op), REG(op));
    update_pc();
    as->test(x86::al, x86::al);
    update_nz();
    clear_vc();
}
void tst_w(uint16_t op) {
    ea_readW_jit(TYPE(op), REG(op));
    update_pc();
    as->test(x86::ax, x86::ax);
    update_nz();
    clear_vc();
}
void tst_l(uint16_t op) {
    ea_readL_jit(TYPE(op), REG(op));
    update_pc();
    as->test(x86::eax, x86::eax);
    update_nz();
    clear_vc();
}

void tas(uint16_t op) {
    ea_readB_jit(TYPE(op), REG(op), true);
    update_pc();
    as->test(x86::al, x86::al);
    update_nz();
    clear_vc();
    as->bts(x86::ax, 7);
    ea_writeB_jit(TYPE(op), REG(op), true);
}

void illegal(uint16_t) { as->call(ILLEGAL_OP); }

void mul_l(uint16_t op) {
    uint16_t ext = FETCH();
    int low = ext >> 12 & 7;
    bool sig = ext & 1 << 11;
    bool dbl = ext & 1 << 10;
    int high = ext & 7;
    ea_readL_jit(TYPE(op), REG(op));
    update_pc();
    as->mov(CC_C, 0);
    if(!sig) {
        // MULU.L
        as->mul(DR_L(low));
        as->mov(DR_L(low), x86::eax);
        if(!dbl) {
            as->test(x86::edx, x86::edx);
            as->setne(CC_V);
        }
    } else {
        // MULS.L
        if(dbl) {
            as->imul(DR_L(low));
        } else {
            as->imul(x86::eax, DR_L(low));
            as->seto(CC_V);
        }
    }
    as->mov(DR_L(low), x86::eax);
    if(dbl) {
        as->mov(DR_L(high), x86::edx);
        as->mov(CC_V, 0);
        as->ror(x86::rdx, 32);
        as->or_(x86::rdx, x86::rax);
        as->test(x86::rdx, x86::rdx);
    } else {
        as->test(x86::eax, x86::eax);
    }
    update_nz();
}

void div_l(uint16_t op) {
    uint16_t ext = FETCH();
    int q = ext >> 12 & 7;
    bool sig = ext & 1 << 11;
    bool dbl = ext & 1 << 10;
    int r = ext & 7;
    ea_readL_jit(TYPE(op), REG(op));
    update_pc();
    as->mov(x86::ecx, x86::eax);
    as->test(x86::ecx, x86::ecx);
    jit_if(COND::FALSE, [] { as->call(DIV0_ERROR); });
    as->mov(CC_C, 0);
    as->mov(x86::eax, DR_L(q));
    if(!sig) {
        // DIVU.L
        if(!dbl) {
            // unsigned 32bit div doesn't overflow
            as->mov(CC_V, 0);
            as->xor_(x86::edx, x86::edx);
            as->div(x86::ecx);
            as->mov(DR_L(q), x86::eax);
            as->test(x86::eax, x86::eax);
            update_nz();
            if(q != r) {
                as->mov(DR_L(r), x86::edx);
            }
        } else {
            // 64bit/32bit div may overflow, so calc 64bit
            as->mov(x86::edx, DR_L(r));
            as->ror(x86::rdx, 32);
            as->or_(x86::rax, x86::rdx);
            as->xor_(x86::rdx, x86::rdx);
            as->div(x86::rcx);
            if(q != r) {
                as->mov(DR_L(r), x86::edx);
            }
            as->mov(DR_L(q), x86::eax);
            as->test(x86::eax, x86::eax);
            update_nz();
            as->ror(x86::rax, 32);
            as->test(x86::eax, x86::eax);
            as->setne(CC_V);
        }
    } else {
        // DIVS.L
        if(!dbl) {
            // signed 32bit div overflow only if INT_MIN / -1
            as->cmp(x86::eax, 0x80000000);
            as->sete(x86::dl);
            as->cmp(x86::ecx, -1);
            as->sete(x86::dh);
            as->and_(x86::dl, x86::dh);
            jit_if(
                COND::TRUE, [] { as->mov(CC_V, 1); },
                [q, r] {
                    as->cdq();
                    as->idiv(x86::ecx);
                    as->mov(DR_L(q), x86::eax);
                    as->test(x86::eax, x86::eax);
                    update_nz();
                    if(q != r) {
                        as->mov(DR_L(r), x86::edx);
                    }
                });
        } else {
            // 64bit/32bit div may overflow, so calc 64bit
            as->mov(x86::edx, DR_L(r));
            as->ror(x86::rdx, 32);
            as->or_(x86::rax, x86::rdx);
            as->movsx(x86::rcx, x86::ecx);
            as->cqo();
            as->idiv(x86::rcx);
            if(q != r) {
                as->mov(DR_L(r), x86::edx);
            }
            as->mov(DR_L(q), x86::eax);
            as->test(x86::eax, x86::eax);
            update_nz();
            as->movsxd(x86::rcx, x86::eax);
            as->cmp(x86::rcx, x86::rax);
            as->setne(CC_V);
        }
    }
}
void trap(uint16_t op) {
    as->mov(ARG1.r32(), op & 0xf);
    as->call(TRAP_ERROR);
}

void link_w(uint16_t op) {
    int16_t disp = FETCH();
    int reg = REG(op);
    update_pc();
    as->mov(x86::eax, AR_L(reg));
    jit_push32();

    as->mov(x86::eax, SP);
    as->mov(AR_L(reg), x86::eax);
    as->add(SP, disp);
}

void unlk(uint16_t op) {
    int reg = REG(op);
    as->mov(x86::eax, AR_L(reg));
    as->mov(SP, x86::eax);
    jit_pop32();
    as->mov(AR_L(reg), x86::eax);
}

void move_to_usp(uint16_t op) {
    jit_priv_check();
    as->mov(x86::eax, AR_L(REG(op)));
    as->mov(CPU_LONG(USP), x86::eax);
    jit_trace_branch();
}

void move_from_usp(uint16_t op) {
    jit_priv_check();
    as->mov(x86::eax, CPU_LONG(USP));
    as->mov(AR_L(REG(op)), x86::eax);
    jit_trace_branch();
}

void reset(uint16_t) {
    jit_priv_check();
    as->call(bus_reset);
}

void nop(uint16_t) { jit_trace_branch(); }

void stop(uint16_t) {
    uint16_t nw = FETCH();
    as->mov(ARG1.r32(), nw);
    as->call(OP::stop_impl);
}

void rte(uint16_t) {
    jit_priv_check();
    as->call(do_rte);
    jit_trace_branch();
    jit_postop();
    as->jmp(end_lbl);
}

void rtd(uint16_t) {
    int16_t disp = FETCH();
    update_pc();
    as->mov(x86::eax, AR_L(7));
    as->lea(x86::edi, x86::ptr(x86::eax, 4));
    as->mov(AR_L(7), x86::edi);
    jit_readL(x86::eax);
    as->mov(x86::edx, AR_L(7));
    as->lea(x86::edx, x86::ptr(x86::edx, disp));
    as->mov(AR_L(7), x86::edx);
    jump();
}

void rts(uint16_t) {
    as->mov(x86::esi, AR_L(7));
    as->lea(x86::rdi, x86::ptr(x86::rsi, 4));
    as->mov(AR_L(7), x86::edi);
    jit_readL(x86::esi);
    jump();
}

void trapv(uint16_t) {
    as->test(CC_V, 1);
    jit_if(COND::TRUE, [] { as->call(TRAPX_ERROR); });
}

void rtr(uint16_t) {
    as->mov(x86::r12d, AR_L(7));
    jit_readW(x86::r12d);
    jit_setCCR();
    as->lea(x86::r12d, x86::ptr(x86::r12d, 2));
    jit_readL(x86::r12d);
    as->lea(x86::r12d, x86::ptr(x86::r12d, 4));
    as->mov(AR_L(7), x86::r12d);
    jump();
}

void movec_from_cr(uint16_t) {
    uint16_t extw = FETCH();
    int rn = extw >> 12 & 15;
    jit_priv_check();
    update_pc();
    as->mov(ARG1.r32(), extw & 0xfff);
    as->lea(ARG2, DR_L(rn));
    as->call(movec_from_cr_impl);
    as->test(x86::al, x86::al);
    jit_if(COND::FALSE, [] { as->call(ILLEGAL_OP); });
    jit_trace_branch();
}

void movec_to_cr(uint16_t) {
    uint16_t extw = FETCH();
    int rn = extw >> 12 & 15;
    jit_priv_check();
    update_pc();
    as->mov(ARG1.r32(), extw & 0xfff);
    as->mov(ARG2.r32(), DR_L(rn));
    as->call(movec_to_cr_impl);
    as->test(x86::al, x86::al);
    jit_if(COND::FALSE, [] { as->call(ILLEGAL_OP); });
    jit_trace_branch();
}
void jsr(uint16_t op) {
    ea_getaddr_jit(TYPE(op), REG(op), 0);
    as->mov(x86::r12d, ARG1.r32());
    update_pc();
    as->mov(x86::eax, c_pc);
    jit_push32();
    as->mov(x86::eax, x86::r12d);
    jump();
}

void jmp(uint16_t op) {
    ea_getaddr_jit(TYPE(op), REG(op), 0);
    as->mov(x86::r12d, ARG1.r32());
    update_pc();
    as->mov(x86::eax, x86::r12d);
    jump();
}

void chk_l(uint16_t op) {
    ea_readL_jit(TYPE(op), REG(op));
    as->mov(x86::edx, DR_L(DN(op)));
    as->cmp(x86::edx, 0);
    as->setge(x86::cl);
    as->cmp(x86::edx, x86::eax);
    as->setle(x86::ch);
    as->and_(x86::cl, x86::ch);
    jit_if(COND::FALSE, [] {
        as->cmp(x86::edx, 0);
        as->setl(CC_N);
        as->call(CHK_ERROR);
    });
}

void chk_w(uint16_t op) {
    ea_readW_jit(TYPE(op), REG(op));
    as->mov(x86::dx, DR_W(DN(op)));
    as->cmp(x86::dx, 0);
    as->setge(x86::cl);
    as->cmp(x86::dx, x86::ax);
    as->setle(x86::ch);
    as->and_(x86::cl, x86::ch);
    jit_if(COND::FALSE, [] {
        as->cmp(x86::dx, 0);
        as->setl(CC_N);
        as->call(CHK_ERROR);
    });
}

void extb_l(uint16_t op) {
    int reg = REG(op);
    as->movsx(x86::eax, DR_B(reg));
    as->test(x86::eax, x86::eax);
    update_nz();
    clear_vc();
    as->mov(DR_L(reg), x86::eax);
}

void lea(uint16_t op) {
    ea_getaddr_jit(TYPE(op), REG(op), 0);
    as->mov(AR_L(DN(op)), ARG1);
}

void addq_b(uint16_t op) {
    int d = DN(op);
    if(d == 0) {
        d = 8;
    }
    ea_readB_jit(TYPE(op), REG(op), true);
    update_pc();
    as->add(x86::al, d);
    update_nz();
    update_vcx();
    ea_writeB_jit(TYPE(op), REG(op), true);
}

void addq_w(uint16_t op) {
    int d = DN(op);
    if(d == 0) {
        d = 8;
    }
    ea_readW_jit(TYPE(op), REG(op), true);
    update_pc();
    as->add(x86::ax, d);
    update_nz();
    update_vcx();
    ea_writeW_jit(TYPE(op), REG(op), true);
}

void addq_l(uint16_t op) {
    int d = DN(op);
    if(d == 0) {
        d = 8;
    }
    ea_readL_jit(TYPE(op), REG(op), true);
    update_pc();
    as->add(x86::eax, d);
    update_nz();
    update_vcx();
    ea_writeL_jit(TYPE(op), REG(op), true);
}

void subq_b(uint16_t op) {
    int d = DN(op);
    if(d == 0) {
        d = 8;
    }
    ea_readB_jit(TYPE(op), REG(op), true);
    update_pc();
    as->sub(x86::al, d);
    update_nz();
    update_vcx();
    ea_writeB_jit(TYPE(op), REG(op), true);
}

void subq_w(uint16_t op) {
    int d = DN(op);
    if(d == 0) {
        d = 8;
    }
    ea_readW_jit(TYPE(op), REG(op), true);
    update_pc();
    as->sub(x86::ax, d);
    update_nz();
    update_vcx();
    ea_writeW_jit(TYPE(op), REG(op), true);
}

void subq_l(uint16_t op) {
    int d = DN(op);
    if(d == 0) {
        d = 8;
    }
    ea_readL_jit(TYPE(op), REG(op), true);
    update_pc();
    as->sub(x86::eax, d);
    update_nz();
    update_vcx();
    ea_writeL_jit(TYPE(op), REG(op), true);
}

void addq_an(uint16_t op) {
    int d = DN(op);
    if(d == 0) {
        d = 8;
    }
    update_pc();
    as->add(AR_L(REG(op)), d);
}

void subq_an(uint16_t op) {
    int d = DN(op);
    if(d == 0) {
        d = 8;
    }
    update_pc();
    as->sub(AR_L(REG(op)), d);
}

void jit_testcc(int cc) {
    switch(cc) {
    case 0: // T
        as->mov(x86::al, 1);
        return;
    case 1: // F
        as->xor_(x86::al, x86::al);
        return;
    case 2: // HI
        as->mov(x86::al, CC_C);
        as->or_(x86::al, CC_Z);
        as->setz(x86::al);
        return;
    case 3: // LS
        as->mov(x86::al, CC_C);
        as->or_(x86::al, CC_Z);
        return;
    case 4: // CC
        as->mov(x86::al, CC_C);
        as->test(x86::al, x86::al);
        as->setz(x86::al);
        return;
    case 5: // CS
        as->mov(x86::al, CC_C);
        return;
    case 6: // NE
        as->mov(x86::al, CC_Z);
        as->test(x86::al, x86::al);
        as->sete(x86::al);
        return;
    case 7: // EQ
        as->mov(x86::al, CC_Z);
        return;
    case 8: // VC
        as->mov(x86::al, CC_V);
        as->test(x86::al, x86::al);
        as->sete(x86::al);
        return;
    case 9: // VS
        as->mov(x86::al, CC_V);
        return;
    case 10: // PL
        as->mov(x86::al, CC_N);
        as->test(x86::al, x86::al);
        as->sete(x86::al);
        return;
    case 11: // MI
        as->mov(x86::al, CC_N);
        return;
    case 12: // GE
        as->mov(x86::al, CC_N);
        as->xor_(x86::al, CC_V);
        as->sete(x86::al);
        return;
    case 13: // LT
        as->mov(x86::al, CC_N);
        as->xor_(x86::al, CC_V);
        return;
    case 14: // GT
        as->mov(x86::al, CC_N);
        as->xor_(x86::al, CC_V);
        as->or_(x86::al, CC_Z);
        as->sete(x86::al);
        return;
    case 15: // LE
        as->mov(x86::al, CC_N);
        as->xor_(x86::al, CC_V);
        as->or_(x86::al, CC_Z);
        return;
    default:
        __builtin_unreachable();
    }
}

void scc_ea(uint16_t op) {
    int cc = op >> 8 & 0xf;
    jit_testcc(cc);
    as->neg(x86::al);
    as->sbb(x86::dl, x86::dl);
    as->mov(x86::al, x86::dl);
    ea_writeB_jit(TYPE(op), REG(op), false);
}

void dbcc(uint16_t op) {
    int cc = op >> 8 & 0xf;
    int16_t disp = FETCH();
    int reg = REG(op);
    jit_testcc(cc);
    as->test(x86::al, x86::al);
    jit_if(COND::FALSE, [reg, disp] {
        as->mov(x86::cx, DR_W(reg));
        as->dec(x86::cx);
        as->mov(DR_W(reg), x86::cx);
        as->cmp(x86::cx, -1);
        jit_if(COND::TRUE, [disp] { jumpC(cpu.PC + disp - 2); });
    });
}

void trapcc(uint16_t op) {
    int cc = op >> 8 & 0xf;
    switch(REG(op)) {
    case 2:
        FETCH();
        break;
    case 3:
        FETCH32();
        break;
    case 4:
        break;
    default:
        __builtin_unreachable();
    }
    update_pc();
    jit_testcc(cc);
    as->test(x86::al, x86::al);
    jit_if(COND::TRUE, [] { as->call(TRAPX_ERROR); });
}

void bxx(uint16_t op) {
    int cc = op >> 8 & 0xf;
    int8_t imm8 = op & 0xff;
    int32_t imm;
    uint32_t p = cpu.PC;
    if(imm8 == 0) {
        imm = static_cast<int16_t>(FETCH());
    } else if(imm8 == -1) {
        imm = FETCH32();
    } else {
        imm = imm8;
    }
    int target = p + imm;
    auto lb = as->newLabel();
    if(cc == 0) {
        // BRA
    } else if(cc == 1) {
        // BSR
        update_pc();
        as->mov(x86::eax, c_pc);
        jit_push32();
    } else {
        // Bcc
        jit_testcc(cc);
        as->test(x86::al, x86::al);
        as->je(lb);
    }
    if(jumpMap.contains(target)) {
        jumpC(target);
    } else {
        as->mov(x86::eax, target);
        jump();
    }
    as->bind(lb);
}

void moveq(uint16_t op) {
    clear_vc();
    as->mov(x86::al, op & 0xff);
    as->test(x86::al, x86::al);
    update_nz();
    as->movsx(x86::eax, x86::al);
    as->mov(DR_L(DN(op)), x86::eax);
}

void or_b_to_dn(uint16_t op) {
    clear_vc();
    ea_readB_jit(TYPE(op), REG(op));
    update_pc();
    as->or_(x86::al, DR_B(DN(op)));
    update_nz();
    as->mov(DR_B(DN(op)), x86::al);
}

void or_w_to_dn(uint16_t op) {
    clear_vc();
    ea_readW_jit(TYPE(op), REG(op));
    update_pc();
    as->or_(x86::ax, DR_W(DN(op)));
    update_nz();
    as->mov(DR_W(DN(op)), x86::ax);
}

void or_l_to_dn(uint16_t op) {
    clear_vc();
    ea_readL_jit(TYPE(op), REG(op));
    update_pc();
    as->or_(x86::eax, DR_L(DN(op)));
    update_nz();
    as->mov(DR_L(DN(op)), x86::eax);
}

void divu_w(uint16_t op) {
    ea_readW_jit(TYPE(op), REG(op));
    update_pc();
    as->test(x86::ax, x86::ax);
    jit_if(COND::FALSE, [] { as->call(DIV0_ERROR); });
    as->cwde();
    as->mov(CC_C, 0);
    as->mov(x86::ecx, DR_L(DN(op)));
    as->xchg(x86::ecx, x86::eax);
    as->xor_(x86::edx, x86::edx);
    as->div(x86::ecx);
    as->mov(x86::ecx, x86::eax);
    as->shr(x86::ecx, 16);
    as->test(x86::cx, x86::cx);
    as->setne(CC_V);
    as->test(x86::ax, x86::ax);
    update_nz();
    as->movzx(x86::eax, x86::ax);
    as->ror(x86::edx, 16);
    as->or_(x86::eax, x86::edx);
    as->mov(DR_L(DN(op)), x86::eax);
}

void sbcd_d(uint16_t op) {
    int reg = REG(op);
    int dn = DN(op);
    as->mov(x86::al, DR_B(reg));
    as->mov(x86::dl, DR_B(dn));
    as->add(x86::dl, CC_X);
    as->xor_(x86::cx, x86::cx);
    as->cmp(x86::dl, 0x9A);
    as->cmove(x86::dx, x86::cx);
    as->sub(x86::al, x86::dl);
    as->lahf();

    as->and_(x86::ah, 0x10);
    as->setnz(x86::dh);
    as->mov(x86::dl, x86::al);
    as->and_(x86::dl, 0xf);
    as->cmp(x86::dl, 9);
    as->seta(x86::dl);
    as->or_(x86::dl, x86::dh);
    jit_if(COND::TRUE, [] { as->sub(x86::al, 6); });
    as->mov(x86::cx, 0);
    as->mov(x86::dx, 0x60);
    as->cmp(x86::al, 0x99);
    as->seta(x86::sil);
    as->and_(x86::ah, 1);
    as->setnz(x86::dil);
    as->or_(x86::sil, x86::dil);
    as->setnz(CC_C);
    as->setnz(CC_X);
    as->cmovnz(x86::cx, x86::dx);
    as->sub(x86::al, x86::cl);
    jit_if(COND::TRUE, [] { as->mov(CC_Z, 0); });
    as->mov(DR_B(dn), x86::al);
}

void sbcd_m(uint16_t op) {
    int reg = REG(op);
    int dn = DN(op);
    as->mov(x86::r12d, AR_L(reg));
    as->lea(x86::r12d, x86::ptr(x86::r12d, -1));
    as->mov(AR_L(reg), x86::r12d);
    jit_readB(x86::r12d);
    as->mov(x86::r11b, x86::al);

    as->mov(x86::esi, AR_L(dn));
    as->lea(x86::esi, x86::ptr(x86::esi, -1));

    as->mov(AR_L(dn), x86::esi);
    jit_readB(x86::esi);

    as->mov(x86::dl, x86::al);
    as->mov(x86::al, x86::r11b);
    as->add(x86::dl, CC_X);
    as->sub(x86::al, x86::dl);
    as->lahf();
    as->and_(x86::ah, 0x10);
    as->setnz(x86::dh);
    as->mov(x86::dl, x86::al);
    as->and_(x86::dl, 0xf);
    as->cmp(x86::dl, 9);
    as->seta(x86::dl);
    as->or_(x86::dl, x86::dh);
    jit_if(COND::TRUE, [] { as->sub(x86::al, 6); });
    as->mov(x86::cx, 0);
    as->mov(x86::dx, 0x60);
    as->cmp(x86::al, 0x99);
    as->seta(CC_C);
    as->seta(CC_X);
    as->cmova(x86::cx, x86::dx);
    as->sub(x86::al, x86::cl);
    jit_if(COND::TRUE, [] { as->mov(CC_Z, 0); });
    jit_writeB(x86::esi, x86::al);
}

void or_b_to_ea(uint16_t op) {
    clear_vc();
    ea_readB_jit(TYPE(op), REG(op), true);
    update_pc();
    as->or_(x86::al, DR_B(DN(op)));
    update_nz();
    ea_writeB_jit(TYPE(op), REG(op), true);
}

void pack_d(uint16_t op) {
    int reg = REG(op);
    uint16_t adj = FETCH();
    update_pc();

    as->movzx(ARG1.r32(), DR_W(reg));
    as->mov(ARG2.r32(), adj);
    as->call(do_pack);
    as->mov(DR_B(DN(op)), x86::al);
}

void pack_m(uint16_t op) {
    int reg = REG(op);
    uint16_t adj = FETCH();
    update_pc();

    as->mov(x86::r12d, AR_L(reg));
    as->lea(x86::r12d, x86::ptr(x86::r12d, -1));

    as->mov(AR_L(reg), x86::r12d);
    jit_readB(x86::r12d);
    as->movzx(x86::r11d, x86::al);

    as->lea(x86::r12d, x86::ptr(x86::r12d, -1));
    as->mov(AR_L(reg), x86::r12d);
    jit_readB(x86::r12d);
    as->movzx(x86::eax, x86::al);
    as->xchg(x86::al, x86::ah);
    as->or_(x86::ax, x86::r11w);
    as->movzx(ARG1.r32(), x86::ax);
    as->mov(ARG2.r32(), adj);
    as->call(do_pack);

    as->mov(x86::edi, AR_L(DN(op)));
    as->lea(x86::edi, x86::ptr(x86::edi, -1));

    as->mov(AR_L(DN(op)), x86::edi);
    jit_writeB(x86::edi, x86::al);
}

void or_w_to_ea(uint16_t op) {
    clear_vc();
    ea_readW_jit(TYPE(op), REG(op), true);
    update_pc();
    as->or_(x86::ax, DR_W(DN(op)));
    update_nz();
    ea_writeW_jit(TYPE(op), REG(op), true);
}

void unpk_d(uint16_t op) {
    int reg = REG(op);
    uint16_t adj = FETCH();
    update_pc();

    as->movzx(ARG1.r32(), DR_L(reg));
    as->mov(ARG2.r32(), adj);
    as->call(do_unpk);
    as->mov(DR_W(DN(op)), x86::ax);
}

void unpk_m(uint16_t op) {
    int reg = REG(op);
    int dn = DN(op);
    uint16_t adj = FETCH();
    update_pc();

    as->mov(x86::esi, AR_L(reg));
    as->lea(x86::esi, x86::ptr(x86::esi, -1));
    as->mov(AR_L(reg), x86::esi);
    jit_readB(x86::esi);

    as->movzx(ARG1.r32(), x86::al);
    as->mov(ARG2.r32(), adj);
    as->call(do_unpk);
    as->movzx(x86::r13d, x86::ax);

    as->mov(x86::r12d, AR_L(dn));
    as->lea(x86::r12d, x86::ptr(x86::r12d, -1));
    as->mov(AR_L(dn), x86::r12d);
    jit_writeB(x86::r12d, x86::r13b);

    as->lea(x86::r12d, x86::ptr(x86::r12d, -1));
    as->mov(AR_L(dn), x86::r12d);
    as->shr(x86::r13w, 8);
    jit_writeB(x86::r12d, x86::r13b);
}

void or_l_to_ea(uint16_t op) {
    clear_vc();
    ea_readL_jit(TYPE(op), REG(op), true);
    update_pc();
    as->or_(x86::eax, DR_L(DN(op)));
    update_nz();
    ea_writeL_jit(TYPE(op), REG(op), true);
}
void divs_w(uint16_t op) {
    ea_readW_jit(TYPE(op), REG(op));
    update_pc();
    as->test(x86::ax, x86::ax);
    jit_if(COND::FALSE, [] { as->call(DIV0_ERROR); });
    as->cwde();
    as->mov(CC_C, 0);
    as->mov(x86::ecx, DR_L(DN(op)));
    as->xchg(x86::ecx, x86::eax);
    as->cmp(x86::eax, 0x80000000);
    as->sete(x86::dl);
    as->cmp(x86::ecx, -1);
    as->sete(x86::dh);
    as->and_(x86::dl, x86::dh);
    jit_if(
        COND::TRUE, [] { as->mov(CC_V, 1); },
        [op] {
            as->cdq();
            as->idiv(x86::ecx);
            as->mov(x86::ecx, x86::eax);
            as->shr(x86::ecx, 16);
            as->test(x86::cx, x86::cx);
            as->setne(CC_V);
            as->test(x86::ax, x86::ax);
            update_nz();
            as->movzx(x86::eax, x86::ax);
            as->shl(x86::edx, 16);
            as->or_(x86::eax, x86::edx);
            as->mov(DR_L(DN(op)), x86::eax);
        });
}

void sub_b_to_dn(uint16_t op) {
    ea_readB_jit(TYPE(op), REG(op));
    update_pc();
    as->sub(DR_B(DN(op)), x86::al);
    update_nz();
    update_vcx();
}

void sub_w_to_dn(uint16_t op) {
    ea_readW_jit(TYPE(op), REG(op));
    update_pc();
    as->sub(DR_W(DN(op)), x86::ax);
    update_nz();
    update_vcx();
}

void sub_l_to_dn(uint16_t op) {
    ea_readL_jit(TYPE(op), REG(op));
    update_pc();
    as->sub(DR_L(DN(op)), x86::eax);
    update_nz();
    update_vcx();
}

void suba_w(uint16_t op) {
    ea_readW_jit(TYPE(op), REG(op));
    update_pc();
    as->cwde();
    as->sub(AR_L(DN(op)), x86::eax);
}

void suba_l(uint16_t op) {
    ea_readL_jit(TYPE(op), REG(op));
    update_pc();
    as->sub(AR_L(DN(op)), x86::eax);
}

void subx_b_d(uint16_t op) {
    as->mov(x86::al, DR_B(DN(op)));
    as->mov(x86::cl, CC_X);
    as->shr(x86::cl, 1);
    as->sbb(DR_B(REG(op)), x86::al);
    update_vcx();
    as->sets(CC_N);
    as->setz(x86::sil);
    as->and_(CC_Z, x86::sil);
}

void subx_b_m(uint16_t op) {
    as->mov(x86::r12d, AR_L(REG(op)));
    as->lea(x86::r12d, x86::ptr(x86::r12d, -1));
    as->mov(AR_L(REG(op)), x86::r12d);
    jit_readB(x86::r12d);
    as->mov(x86::r13b, x86::al);
    as->mov(x86::esi, AR_L(DN(op)));
    as->lea(x86::esi, x86::ptr(x86::esi, -1));
    as->mov(AR_L(DN(op)), x86::esi);
    jit_readB(x86::esi);
    as->mov(x86::cl, CC_X);
    as->shr(x86::cl, 1);
    as->sbb(x86::r13b, x86::al);
    update_vcx();
    as->sets(CC_N);
    as->setz(x86::sil);
    as->and_(CC_Z, x86::sil);
    as->mov(ARG1.r32(), x86::r12d);
    as->movzx(ARG2.r32(), x86::r13b);
    jit_writeB(x86::r12d, x86::r13b);
}

void subx_w_d(uint16_t op) {
    as->mov(x86::ax, DR_W(DN(op)));
    as->mov(x86::cl, CC_X);
    as->shr(x86::cl, 1);
    as->sbb(DR_W(REG(op)), x86::ax);
    update_vcx();
    as->sets(CC_N);
    as->setz(x86::sil);
    as->and_(CC_Z, x86::sil);
}

void subx_w_m(uint16_t op) {
    as->mov(x86::r12d, AR_L(REG(op)));
    as->lea(x86::r12d, x86::ptr(x86::r12d, -2));
    as->mov(AR_L(REG(op)), x86::r12d);
    jit_readW(x86::r12d);
    as->mov(x86::r13w, x86::ax);
    as->mov(x86::esi, AR_L(DN(op)));
    as->lea(x86::esi, x86::ptr(x86::esi, -2));
    as->mov(AR_L(DN(op)), x86::esi);
    jit_readW(x86::esi);
    as->mov(x86::cl, CC_X);
    as->shr(x86::cl, 1);
    as->sbb(x86::r13w, x86::ax);
    update_vcx();
    as->sets(CC_N);
    as->setz(x86::sil);
    as->and_(CC_Z, x86::sil);
    jit_writeW(x86::r12d, x86::r13w);
}

void subx_l_d(uint16_t op) {
    as->mov(x86::eax, DR_L(DN(op)));
    as->mov(x86::cl, CC_X);
    as->shr(x86::cl, 1);
    as->sbb(DR_L(REG(op)), x86::eax);
    update_vcx();
    as->sets(CC_N);
    as->setz(x86::sil);
    as->and_(CC_Z, x86::sil);
}

void subx_l_m(uint16_t op) {
    as->mov(x86::r12d, AR_L(REG(op)));
    as->lea(x86::r12d, x86::ptr(x86::r12d, -4));
    as->mov(AR_L(REG(op)), x86::r12d);
    jit_readL(x86::r12d);
    as->mov(x86::r13d, x86::eax);
    as->mov(x86::esi, AR_L(DN(op)));
    as->lea(x86::esi, x86::ptr(x86::esi, -4));
    as->mov(AR_L(DN(op)), x86::esi);
    jit_readL(x86::esi);
    as->mov(x86::cl, CC_X);
    as->shr(x86::cl, 1);
    as->sbb(x86::r13d, x86::eax);
    update_vcx();
    as->sets(CC_N);
    as->setz(x86::sil);
    as->and_(CC_Z, x86::sil);
    jit_writeL(x86::r12d, x86::r13d);
}

void sub_b_to_ea(uint16_t op) {
    ea_readB_jit(TYPE(op), REG(op), true);
    update_pc();
    as->sub(x86::al, DR_B(DN(op)));
    update_nz();
    update_vcx();
    ea_writeB_jit(TYPE(op), REG(op), true);
}

void sub_w_to_ea(uint16_t op) {
    ea_readW_jit(TYPE(op), REG(op), true);
    update_pc();
    as->sub(x86::ax, DR_W(DN(op)));
    update_nz();
    update_vcx();
    ea_writeW_jit(TYPE(op), REG(op), true);
}

void sub_l_to_ea(uint16_t op) {
    ea_readL_jit(TYPE(op), REG(op), true);
    update_pc();
    as->sub(x86::eax, DR_W(DN(op)));
    update_nz();
    update_vcx();
    ea_writeL_jit(TYPE(op), REG(op), true);
}

void aline_ex(uint16_t /* op */) {
    // some optimize for specific op?
    as->call(ALINE);
}

void cmp_b(uint16_t op) {
    ea_readB_jit(TYPE(op), REG(op));
    update_pc();
    as->cmp(DR_B(DN(op)), x86::al);
    update_vc();
    update_nz();
}

void cmp_w(uint16_t op) {
    ea_readW_jit(TYPE(op), REG(op));
    update_pc();
    as->cmp(DR_W(DN(op)), x86::ax);
    update_vc();
    update_nz();
}

void cmp_l(uint16_t op) {
    ea_readL_jit(TYPE(op), REG(op));
    update_pc();
    as->cmp(DR_L(DN(op)), x86::eax);
    update_vc();
    update_nz();
}

void cmpa_w(uint16_t op) {
    ea_readW_jit(TYPE(op), REG(op));
    update_pc();
    as->cwde();
    as->cmp(AR_L(DN(op)), x86::eax);
    update_vc();
    update_nz();
}

void cmpa_l(uint16_t op) {
    ea_readL_jit(TYPE(op), REG(op));
    update_pc();
    as->cmp(AR_L(DN(op)), x86::eax);
    update_vc();
    update_nz();
}

void eor_b(uint16_t op) {
    ea_readB_jit(TYPE(op), REG(op), true);
    update_pc();
    clear_vc();
    as->xor_(x86::al, DR_B(DN(op)));
    update_nz();
    ea_writeB_jit(TYPE(op), REG(op), true);
}

void eor_w(uint16_t op) {
    ea_readW_jit(TYPE(op), REG(op), true);
    update_pc();
    clear_vc();
    as->xor_(x86::ax, DR_W(DN(op)));
    update_nz();
    ea_writeW_jit(TYPE(op), REG(op), true);
}

void eor_l(uint16_t op) {
    ea_readL_jit(TYPE(op), REG(op), true);
    update_pc();
    clear_vc();
    as->xor_(x86::eax, DR_L(DN(op)));
    update_nz();
    ea_writeL_jit(TYPE(op), REG(op), true);
}

void cmpm_b(uint16_t op) {
    as->mov(x86::esi, AR_L(REG(op)));
    as->inc(AR_L(REG(op)));
    jit_readB(x86::esi);
    as->mov(x86::r12b, x86::al);

    as->mov(x86::esi, AR_L(DN(op)));
    as->inc(AR_L(DN(op)));
    jit_readB(x86::esi);
    as->cmp(x86::al, x86::r12b);

    update_nz();
    update_vc();
}

void cmpm_w(uint16_t op) {
    as->mov(x86::esi, AR_L(REG(op)));
    as->add(AR_L(REG(op)), 2);
    jit_readW(x86::esi);
    as->mov(x86::r12w, x86::ax);

    as->mov(x86::esi, AR_L(DN(op)));
    as->add(AR_L(DN(op)), 2);
    jit_readW(x86::esi);
    as->cmp(x86::ax, x86::r12w);

    update_nz();
    update_vc();
}

void cmpm_l(uint16_t op) {
    as->mov(x86::esi, AR_L(REG(op)));
    as->add(AR_L(REG(op)), 4);
    jit_readL(x86::esi);
    as->mov(x86::r12d, x86::eax);

    as->mov(x86::esi, AR_L(DN(op)));
    as->add(AR_L(DN(op)), 4);
    jit_readL(x86::esi);
    as->cmp(x86::eax, x86::r12d);

    update_nz();
    update_vc();
}

void and_b_to_dn(uint16_t op) {
    ea_readB_jit(TYPE(op), REG(op));
    update_pc();
    as->and_(DR_B(DN(op)), x86::al);
    update_nz();
    clear_vc();
}

void and_w_to_dn(uint16_t op) {
    ea_readW_jit(TYPE(op), REG(op));
    update_pc();
    as->and_(DR_W(DN(op)), x86::ax);
    update_nz();
    clear_vc();
}

void and_l_to_dn(uint16_t op) {
    ea_readL_jit(TYPE(op), REG(op));
    update_pc();
    as->and_(DR_L(DN(op)), x86::eax);
    update_nz();
    clear_vc();
}

void mulu_w(uint16_t op) {
    ea_readW_jit(TYPE(op), REG(op));
    update_pc();
    as->mul(DR_W(DN(op)));
    as->ror(x86::edx, 16);
    as->or_(x86::eax, x86::edx);
    as->test(x86::eax, x86::eax);
    update_nz();
    clear_vc();
    as->mov(DR_L(DN(op)), x86::eax);
}

void abcd_d(uint16_t op) {
    int dn = DN(op);
    as->mov(x86::al, DR_B(dn));
    as->mov(x86::dl, DR_B(REG(op)));
    as->add(x86::dl, CC_X);
    as->add(x86::al, x86::dl);
    as->lahf();
    as->and_(x86::ah, 0x10);
    as->setnz(x86::dh);
    as->mov(x86::dl, x86::al);
    as->and_(x86::dl, 0xf);
    as->cmp(x86::dl, 9);
    as->seta(x86::dl);
    as->or_(x86::dl, x86::dh);
    jit_if(COND::TRUE, [] { as->add(x86::al, 6); });
    as->mov(x86::cx, 0);
    as->mov(x86::dx, 0x60);
    as->cmp(x86::al, 0x99);
    as->seta(x86::sil);
    as->and_(x86::ah, 1);
    as->setnz(x86::dil);
    as->or_(x86::sil, x86::dil);
    as->setnz(CC_C);
    as->setnz(CC_X);
    as->cmovnz(x86::cx, x86::dx);
    as->add(x86::al, x86::cl);
    jit_if(COND::TRUE, [] { as->mov(CC_Z, 0); });
    as->mov(DR_B(dn), x86::al);
}

void abcd_m(uint16_t op) {
    int dn = DN(op);
    as->mov(x86::r12d, AR_L(dn));
    as->lea(x86::r12d, x86::ptr(x86::r12d, -1));
    as->mov(AR_L(dn), x86::r12d);
    jit_readB(x86::r12d);
    as->mov(x86::r11b, x86::al);

    as->mov(x86::edi, AR_L(REG(op)));
    as->lea(x86::edi, x86::ptr(x86::edi, -1));
    as->mov(AR_L(REG(op)), x86::edi);
    jit_readB(x86::edi);

    as->mov(x86::dl, x86::al);
    as->mov(x86::al, x86::r11b);
    as->add(x86::dl, CC_X);
    as->add(x86::al, x86::dl);
    as->lahf();
    as->and_(x86::ah, 0x10);
    as->setnz(x86::dh);
    as->mov(x86::dl, x86::al);
    as->and_(x86::dl, 0xf);
    as->cmp(x86::dl, 9);
    as->seta(x86::dl);
    as->or_(x86::dl, x86::dh);
    jit_if(COND::TRUE, [] { as->add(x86::al, 6); });
    as->mov(x86::cx, 0);
    as->mov(x86::dx, 0x60);
    as->cmp(x86::al, 0x99);
    as->seta(CC_C);
    as->seta(CC_X);
    as->cmova(x86::cx, x86::dx);
    as->add(x86::al, x86::cl);
    jit_if(COND::TRUE, [] { as->mov(CC_Z, 0); });

    jit_writeB(x86::r12d, x86::al);
}

void and_b_to_ea(uint16_t op) {
    clear_vc();
    ea_readB_jit(TYPE(op), REG(op), true);
    update_pc();
    as->and_(x86::al, DR_B(DN(op)));
    update_nz();
    ea_writeB_jit(TYPE(op), REG(op), true);
}

void and_w_to_ea(uint16_t op) {
    clear_vc();
    ea_readW_jit(TYPE(op), REG(op), true);
    update_pc();
    as->and_(x86::ax, DR_W(DN(op)));
    update_nz();
    ea_writeW_jit(TYPE(op), REG(op), true);
}

void and_l_to_ea(uint16_t op) {
    clear_vc();
    ea_readL_jit(TYPE(op), REG(op), true);
    update_pc();
    as->and_(x86::eax, DR_L(DN(op)));
    update_nz();
    ea_writeL_jit(TYPE(op), REG(op), true);
}

void muls_w(uint16_t op) {
    ea_readW_jit(TYPE(op), REG(op));
    update_pc();
    as->imul(DR_W(DN(op)));
    as->ror(x86::edx, 16);
    as->or_(x86::eax, x86::edx);
    as->test(x86::eax, x86::eax);
    update_nz();
    clear_vc();
    as->mov(DR_L(DN(op)), x86::eax);
}

void exg_dn(uint16_t op) {
    as->mov(x86::eax, DR_L(REG(op)));
    as->xchg(DR_L(DN(op)), x86::eax);
    as->mov(DR_L(REG(op)), x86::eax);
}

void exg_an(uint16_t op) {
    as->mov(x86::eax, AR_L(REG(op)));
    as->xchg(AR_L(DN(op)), x86::eax);
    as->mov(AR_L(REG(op)), x86::eax);
}

void exg_da(uint16_t op) {
    as->mov(x86::eax, AR_L(REG(op)));
    as->xchg(DR_L(DN(op)), x86::eax);
    as->mov(AR_L(REG(op)), x86::eax);
}

void add_b_to_dn(uint16_t op) {
    ea_readB_jit(TYPE(op), REG(op));
    update_pc();
    as->add(DR_B(DN(op)), x86::al);
    update_nz();
    update_vcx();
}

void add_w_to_dn(uint16_t op) {
    ea_readW_jit(TYPE(op), REG(op));
    update_pc();
    as->add(DR_W(DN(op)), x86::ax);
    update_nz();
    update_vcx();
}

void add_l_to_dn(uint16_t op) {
    ea_readL_jit(TYPE(op), REG(op));
    update_pc();
    as->add(DR_L(DN(op)), x86::eax);
    update_nz();
    update_vcx();
}

void adda_w(uint16_t op) {
    ea_readW_jit(TYPE(op), REG(op));
    update_pc();
    as->cwde();
    as->add(AR_L(DN(op)), x86::eax);
}

void adda_l(uint16_t op) {
    ea_readL_jit(TYPE(op), REG(op));
    update_pc();
    as->add(AR_L(DN(op)), x86::eax);
}

void addx_b_d(uint16_t op) {
    as->mov(x86::al, DR_B(REG(op)));
    as->mov(x86::cl, CC_X);
    as->shr(x86::cl, 1);
    as->adc(DR_B(DN(op)), x86::al);
    update_vcx();
    as->sets(CC_N);
    as->setz(x86::sil);
    as->and_(CC_Z, x86::sil);
}

void addx_b_m(uint16_t op) {
    as->mov(x86::r12d, AR_L(DN(op)));
    as->lea(x86::r12d, x86::ptr(x86::r12d, -1));
    as->mov(AR_L(DN(op)), x86::r12d);
    jit_readB(x86::r12d);
    as->mov(x86::r13b, x86::al);
    as->mov(x86::esi, AR_L(REG(op)));
    as->lea(x86::esi, x86::ptr(x86::esi, -1));
    as->mov(AR_L(REG(op)), x86::esi);
    jit_readB(x86::esi);

    as->mov(x86::cl, CC_X);
    as->shr(x86::cl, 1);
    as->adc(x86::r13b, x86::al);
    update_vcx();
    as->sets(CC_N);
    as->setz(x86::sil);
    as->and_(CC_Z, x86::sil);
    as->mov(ARG1.r32(), x86::r12d);
    as->movzx(ARG2.r32(), x86::r13b);
    jit_writeB(x86::r12d, x86::r13b);
}

void addx_w_d(uint16_t op) {
    as->mov(x86::ax, DR_W(REG(op)));
    as->mov(x86::cl, CC_X);
    as->shr(x86::cl, 1);
    as->adc(DR_W(DN(op)), x86::ax);
    update_vcx();
    as->sets(CC_N);
    as->setz(x86::sil);
    as->and_(CC_Z, x86::sil);
}

void addx_w_m(uint16_t op) {
    as->mov(x86::r12d, AR_L(DN(op)));
    as->lea(x86::r12d, x86::ptr(x86::r12d, -2));
    as->mov(AR_L(DN(op)), x86::r12d);
    jit_readW(x86::r12d);
    as->mov(x86::r13w, x86::ax);
    as->mov(x86::esi, AR_L(REG(op)));
    as->lea(x86::esi, x86::ptr(x86::esi, -2));
    as->mov(AR_L(REG(op)), x86::esi);
    jit_readW(x86::esi);
    as->mov(x86::cl, CC_X);
    as->shr(x86::cl, 1);
    as->adc(x86::r13w, x86::ax);
    update_vcx();
    as->sets(CC_N);
    as->setz(x86::sil);
    as->and_(CC_Z, x86::sil);
    jit_writeW(x86::r12d, x86::r13w);
}

void addx_l_d(uint16_t op) {
    as->mov(x86::eax, DR_L(REG(op)));
    as->mov(x86::cl, CC_X);
    as->shr(x86::cl, 1);
    as->adc(DR_L(DN(op)), x86::eax);
    update_vcx();
    as->sets(CC_N);
    as->setz(x86::sil);
    as->and_(CC_Z, x86::sil);
}

void addx_l_m(uint16_t op) {
    as->mov(x86::r12d, AR_L(DN(op)));
    as->lea(x86::r12d, x86::ptr(x86::r12d, -4));
    as->mov(AR_L(DN(op)), x86::r12d);
    jit_readL(x86::r12d);
    as->mov(x86::r13d, x86::eax);
    as->mov(x86::esi, AR_L(REG(op)));
    as->lea(x86::esi, x86::ptr(x86::esi, -4));
    as->mov(AR_L(REG(op)), x86::esi);
    jit_readL(x86::esi);
    as->mov(x86::cl, CC_X);
    as->shr(x86::cl, 1);
    as->adc(x86::r13d, x86::eax);
    update_vcx();
    as->sets(CC_N);
    as->setz(x86::sil);
    as->and_(CC_Z, x86::sil);
    jit_writeL(x86::r12d, x86::r13d);
}

void add_b_to_ea(uint16_t op) {
    ea_readB_jit(TYPE(op), REG(op), true);
    update_pc();
    as->add(x86::al, DR_B(DN(op)));
    update_nz();
    update_vcx();
    ea_writeB_jit(TYPE(op), REG(op), true);
}

void add_w_to_ea(uint16_t op) {
    ea_readW_jit(TYPE(op), REG(op), true);
    update_pc();
    as->add(x86::ax, DR_W(DN(op)));
    update_nz();
    update_vcx();
    ea_writeW_jit(TYPE(op), REG(op), true);
}

void add_l_to_ea(uint16_t op) {
    ea_readL_jit(TYPE(op), REG(op), true);
    update_pc();
    as->add(x86::eax, DR_W(DN(op)));
    update_nz();
    update_vcx();
    ea_writeL_jit(TYPE(op), REG(op), true);
}

void asr_b_i(uint16_t op) {
    int cnt = DN(op) ? DN(op) : 8;
    as->mov(CC_V, 0);
    as->sar(DR_B(REG(op)), cnt);
    as->setc(CC_C);
    as->setc(CC_X);
    update_nz();
}

void lsr_b_i(uint16_t op) {
    int cnt = DN(op) ? DN(op) : 8;
    as->mov(CC_V, 0);
    as->shr(DR_B(REG(op)), cnt);
    as->setc(CC_C);
    as->setc(CC_X);
    update_nz();
}

void roxr_b_i(uint16_t op) {
    int cnt = DN(op) ? DN(op) : 8;
    as->mov(CC_V, 0);
    as->mov(x86::al, DR_B(REG(op)));
    as->mov(x86::dl, CC_X);
    as->shr(x86::dl, 1);
    as->rcr(x86::al, cnt);
    as->setc(CC_C);
    as->setc(CC_X);
    as->test(x86::al, x86::al);
    update_nz();
    as->mov(DR_B(REG(op)), x86::al);
}

void ror_b_i(uint16_t op) {
    int cnt = DN(op) ? DN(op) : 8;
    as->mov(CC_V, 0);
    as->mov(x86::al, DR_B(REG(op)));
    as->ror(x86::al, cnt);
    as->setc(CC_C);
    as->test(x86::al, x86::al);
    update_nz();
    as->mov(DR_B(REG(op)), x86::al);
}

void asr_b_r(uint16_t op) {
    as->movsx(x86::rax, DR_B(REG(op)));
    as->mov(x86::cl, DR_B(DN(op)));
    as->and_(x86::cl, 63);
    jit_if(
        COND::FALSE,
        [] {
            as->mov(CC_C, 0);
            as->test(x86::al, x86::al);
            update_nz();
        },
        [op] {
            as->mov(CC_V, 0);
            as->sar(x86::rax, x86::cl);
            as->setc(CC_C);
            as->setc(CC_X);
            update_nz();
            as->mov(DR_B(REG(op)), x86::al);
        });
}

void lsr_b_r(uint16_t op) {
    as->movzx(x86::rax, DR_B(REG(op)));
    as->mov(x86::cl, DR_B(DN(op)));
    as->and_(x86::cl, 63);
    jit_if(
        COND::FALSE,
        [] {
            as->mov(CC_C, 0);
            as->test(x86::al, x86::al);
            update_nz();
        },
        [op] {
            as->mov(CC_V, 0);
            as->shr(x86::rax, x86::cl);
            as->setc(CC_C);
            as->setc(CC_X);
            update_nz();
            as->mov(DR_B(REG(op)), x86::al);
        });
}

void roxr_b_r(uint16_t op) {
    as->mov(CC_V, 0);
    as->mov(x86::al, DR_B(REG(op)));
    as->mov(x86::cl, DR_B(DN(op)));
    as->and_(x86::cl, 63);
    jit_if(
        COND::FALSE,
        [] {
            as->mov(x86::dl, CC_X);
            as->mov(CC_C, x86::dl);
            as->test(x86::al, x86::al);
            update_nz();
        },
        [op] {
            as->cmp(x86::cl, 31);
            jit_if(
                COND::LE,
                [] {
                    as->mov(x86::dl, CC_X);
                    as->shr(x86::dl, 1);
                },
                [] {
                    as->sub(x86::cl, 32);
                    as->mov(x86::dl, CC_X);
                    as->shr(x86::dl, 1);
                    as->rcr(x86::al, 31);
                    as->rcr(x86::al, 1);
                });
            as->rcr(x86::al, x86::cl);
            as->setc(CC_C);
            as->setc(CC_X);
            as->test(x86::al, x86::al);
            update_nz();
            as->mov(DR_B(REG(op)), x86::al);
        });
}

void ror_b_r(uint16_t op) {
    as->mov(CC_V, 0);
    as->mov(x86::al, DR_B(REG(op)));
    as->mov(x86::cl, DR_B(DN(op)));
    as->and_(x86::cl, 63);
    jit_if(
        COND::FALSE,
        [] {
            as->mov(x86::dl, CC_X);
            as->mov(CC_C, x86::dl);
            as->test(x86::al, x86::al);
            update_nz();
        },
        [op] {
            as->ror(x86::al, x86::cl);
            as->setc(CC_C);
            as->test(x86::al, x86::al);
            update_nz();
            as->mov(DR_B(REG(op)), x86::al);
        });
}

void asr_w_i(uint16_t op) {
    int cnt = DN(op) ? DN(op) : 8;
    as->mov(CC_V, 0);
    as->sar(DR_W(REG(op)), cnt);
    as->setc(CC_C);
    as->setc(CC_X);
    update_nz();
}

void lsr_w_i(uint16_t op) {
    int cnt = DN(op) ? DN(op) : 8;
    as->mov(CC_V, 0);
    as->shr(DR_W(REG(op)), cnt);
    as->setc(CC_C);
    as->setc(CC_X);
    update_nz();
}

void roxr_w_i(uint16_t op) {
    int cnt = DN(op) ? DN(op) : 8;
    as->mov(CC_V, 0);
    as->mov(x86::ax, DR_B(REG(op)));
    as->mov(x86::dl, CC_X);
    as->shr(x86::dl, 1);
    as->rcr(x86::ax, cnt);
    as->setc(CC_C);
    as->setc(CC_X);
    as->test(x86::ax, x86::ax);
    update_nz();
    as->mov(DR_W(REG(op)), x86::ax);
}

void ror_w_i(uint16_t op) {
    int cnt = DN(op) ? DN(op) : 8;
    as->mov(CC_V, 0);
    as->mov(x86::ax, DR_W(REG(op)));
    as->ror(x86::ax, cnt);
    as->setc(CC_C);
    as->test(x86::ax, x86::ax);
    update_nz();
    as->mov(DR_W(REG(op)), x86::ax);
}

void asr_w_r(uint16_t op) {
    as->movsx(x86::rax, DR_W(REG(op)));
    as->mov(x86::cl, DR_B(DN(op)));
    as->and_(x86::cl, 63);
    jit_if(
        COND::FALSE,
        [] {
            as->mov(CC_C, 0);
            as->test(x86::ax, x86::ax);
            update_nz();
        },
        [op] {
            as->mov(CC_V, 0);
            as->sar(x86::rax, x86::cl);
            as->setc(CC_C);
            as->setc(CC_X);
            update_nz();
            as->mov(DR_W(REG(op)), x86::ax);
        });
}

void lsr_w_r(uint16_t op) {
    as->movzx(x86::rax, DR_W(REG(op)));
    as->mov(x86::cl, DR_B(DN(op)));
    as->and_(x86::cl, 63);
    jit_if(
        COND::FALSE,
        [] {
            as->mov(CC_C, 0);
            as->test(x86::ax, x86::ax);
            update_nz();
        },
        [op] {
            as->mov(CC_V, 0);
            as->shr(x86::rax, x86::cl);
            as->setc(CC_C);
            as->setc(CC_X);
            update_nz();
            as->mov(DR_W(REG(op)), x86::ax);
        });
}

void roxr_w_r(uint16_t op) {
    as->mov(CC_V, 0);
    as->mov(x86::ax, DR_W(REG(op)));
    as->mov(x86::cl, DR_B(DN(op)));
    as->and_(x86::cl, 63);
    jit_if(
        COND::FALSE,
        [] {
            as->mov(x86::dl, CC_X);
            as->mov(CC_C, x86::dl);
            as->test(x86::ax, x86::ax);
            update_nz();
        },
        [op] {
            as->cmp(x86::cl, 31);
            jit_if(
                COND::LE,
                [] {
                    as->mov(x86::dl, CC_X);
                    as->shr(x86::dl, 1);
                },
                [] {
                    as->sub(x86::cl, 32);
                    as->mov(x86::dl, CC_X);
                    as->shr(x86::dl, 1);
                    as->rcr(x86::ax, 31);
                    as->rcr(x86::ax, 1);
                });
            as->rcr(x86::ax, x86::cl);
            as->setc(CC_C);
            as->setc(CC_X);
            as->test(x86::ax, x86::ax);
            update_nz();
            as->mov(DR_W(REG(op)), x86::ax);
        });
}

void ror_w_r(uint16_t op) {
    as->mov(CC_V, 0);
    as->mov(x86::ax, DR_W(REG(op)));
    as->mov(x86::cl, DR_B(DN(op)));
    as->and_(x86::cl, 63);
    jit_if(
        COND::FALSE,
        [] {
            as->mov(x86::dl, CC_X);
            as->mov(CC_C, x86::dl);
            as->test(x86::ax, x86::ax);
            update_nz();
        },
        [op] {
            as->ror(x86::ax, x86::cl);
            as->setc(CC_C);
            as->test(x86::ax, x86::ax);
            update_nz();
            as->mov(DR_W(REG(op)), x86::ax);
        });
}

void asr_l_i(uint16_t op) {
    int cnt = DN(op) ? DN(op) : 8;
    as->mov(CC_V, 0);
    as->sar(DR_L(REG(op)), cnt);
    as->setc(CC_C);
    as->setc(CC_X);
    update_nz();
}

void lsr_l_i(uint16_t op) {
    int cnt = DN(op) ? DN(op) : 8;
    as->mov(CC_V, 0);
    as->shr(DR_L(REG(op)), cnt);
    as->setc(CC_C);
    as->setc(CC_X);
    update_nz();
}

void roxr_l_i(uint16_t op) {
    int cnt = DN(op) ? DN(op) : 8;
    as->mov(CC_V, 0);
    as->mov(x86::eax, DR_L(REG(op)));
    as->mov(x86::dl, CC_X);
    as->shr(x86::dl, 1);
    as->rcr(x86::eax, cnt);
    as->setc(CC_C);
    as->setc(CC_X);
    as->test(x86::eax, x86::eax);
    update_nz();
    as->mov(DR_L(REG(op)), x86::eax);
}

void ror_l_i(uint16_t op) {
    int cnt = DN(op) ? DN(op) : 8;
    as->mov(CC_V, 0);
    as->mov(x86::eax, DR_L(REG(op)));
    as->ror(x86::eax, cnt);
    as->setc(CC_C);
    as->test(x86::eax, x86::eax);
    update_nz();
    as->mov(DR_L(REG(op)), x86::eax);
}

void asr_l_r(uint16_t op) {
    as->movsxd(x86::rax, DR_L(REG(op)));
    as->mov(x86::cl, DR_B(DN(op)));
    as->and_(x86::cl, 63);
    jit_if(
        COND::FALSE,
        [] {
            as->mov(CC_C, 0);
            as->test(x86::eax, x86::eax);
            update_nz();
        },
        [op] {
            as->mov(CC_V, 0);
            as->sar(x86::rax, x86::cl);
            as->setc(CC_C);
            as->setc(CC_X);
            update_nz();
            as->mov(DR_L(REG(op)), x86::eax);
        });
}

void lsr_l_r(uint16_t op) {
    as->mov(x86::eax, DR_L(REG(op)));
    as->mov(x86::cl, DR_B(DN(op)));
    as->and_(x86::cl, 63);
    jit_if(
        COND::FALSE,
        [] {
            as->mov(CC_C, 0);
            as->test(x86::eax, x86::eax);
            update_nz();
        },
        [op] {
            as->mov(CC_V, 0);
            as->shr(x86::rax, x86::cl);
            as->setc(CC_C);
            as->setc(CC_X);
            update_nz();
            as->mov(DR_L(REG(op)), x86::eax);
        });
}

void roxr_l_r(uint16_t op) {
    as->mov(CC_V, 0);
    as->mov(x86::eax, DR_L(REG(op)));
    as->mov(x86::cl, DR_B(DN(op)));
    as->and_(x86::cl, 63);
    jit_if(
        COND::FALSE,
        [] {
            as->mov(x86::dl, CC_X);
            as->mov(CC_C, x86::dl);
            as->test(x86::eax, x86::eax);
            update_nz();
        },
        [op] {
            as->cmp(x86::cl, 31);
            jit_if(
                COND::LE,
                [] {
                    as->mov(x86::dl, CC_X);
                    as->shr(x86::dl, 1);
                },
                [] {
                    as->sub(x86::cl, 32);
                    as->mov(x86::dl, CC_X);
                    as->shr(x86::dl, 1);
                    as->rcr(x86::eax, 31);
                    as->rcr(x86::eax, 1);
                });
            as->rcr(x86::eax, x86::cl);
            as->setc(CC_C);
            as->setc(CC_X);
            as->test(x86::eax, x86::eax);
            update_nz();
            as->mov(DR_L(REG(op)), x86::eax);
        });
}

void ror_l_r(uint16_t op) {
    as->mov(CC_V, 0);
    as->mov(x86::eax, DR_L(REG(op)));
    as->mov(x86::cl, DR_B(DN(op)));
    as->and_(x86::cl, 63);
    jit_if(
        COND::FALSE,
        [] {
            as->mov(x86::dl, CC_X);
            as->mov(CC_C, x86::dl);
            as->test(x86::eax, x86::eax);
            update_nz();
        },
        [op] {
            as->cmp(x86::cl, 32);
            jit_if(
                COND::EQ,
                [] {
                    as->mov(x86::edx, x86::eax);
                    as->shl(x86::edx, 1);
                },
                [] { as->ror(x86::eax, x86::cl); });
            as->setc(CC_C);
            as->test(x86::eax, x86::eax);
            update_nz();
            as->mov(DR_L(REG(op)), x86::eax);
        });
}

void asr_ea(uint16_t op) {
    as->mov(CC_V, 0);
    ea_readW_jit(TYPE(op), REG(op), true);
    as->sar(x86::ax, 1);
    as->setc(CC_C);
    as->setc(CC_X);
    update_nz();
    ea_writeW_jit(TYPE(op), REG(op), true);
}

void lsr_ea(uint16_t op) {
    as->mov(CC_V, 0);
    ea_readW_jit(TYPE(op), REG(op), true);
    as->shr(x86::ax, 1);
    as->setc(CC_C);
    as->setc(CC_X);
    update_nz();
    ea_writeW_jit(TYPE(op), REG(op), true);
}

void roxr_ea(uint16_t op) {
    as->mov(CC_V, 0);
    ea_readW_jit(TYPE(op), REG(op), true);
    as->mov(x86::dl, CC_X);
    as->shr(x86::dl, 1);
    as->rcr(x86::ax, 1);
    as->setc(CC_C);
    as->setc(CC_X);
    as->test(x86::ax, x86::ax);
    update_nz();
    ea_writeW_jit(TYPE(op), REG(op), true);
}

void ror_ea(uint16_t op) {
    as->mov(CC_V, 0);
    ea_readW_jit(TYPE(op), REG(op), true);
    as->ror(x86::ax, 1);
    as->setc(CC_C);
    as->test(x86::ax, x86::ax);
    update_nz();
    ea_writeW_jit(TYPE(op), REG(op), true);
}

void asl_b_i(uint16_t op) {
    int cnt = DN(op) ? DN(op) : 8;
    as->mov(x86::al, DR_B(REG(op)));
    as->sal(DR_B(REG(op)), cnt);
    as->setc(CC_C);
    as->setc(CC_X);
    update_nz();
    as->mov(x86::dl, x86::al);
    as->sar(x86::dl, 7);
    if(cnt != 8) {
        as->sar(x86::al, 7 - cnt);
    }
    as->cmp(x86::al, x86::dl);
    as->setne(CC_V);
}

void lsl_b_i(uint16_t op) {
    int cnt = DN(op) ? DN(op) : 8;
    as->mov(CC_V, 0);
    as->shl(DR_B(REG(op)), cnt);
    as->setc(CC_C);
    as->setc(CC_X);
    update_nz();
}

void roxl_b_i(uint16_t op) {
    int cnt = DN(op) ? DN(op) : 8;
    as->mov(CC_V, 0);
    as->mov(x86::al, DR_B(REG(op)));
    as->mov(x86::dl, CC_X);
    as->shr(x86::dl, 1);
    as->rcl(x86::al, cnt);
    as->setc(CC_C);
    as->setc(CC_X);
    as->test(x86::al, x86::al);
    update_nz();
    as->mov(DR_B(REG(op)), x86::al);
}

void rol_b_i(uint16_t op) {
    int cnt = DN(op) ? DN(op) : 8;
    as->mov(CC_V, 0);
    as->mov(x86::al, DR_B(REG(op)));
    as->rol(x86::al, cnt);
    as->setc(CC_C);
    as->test(x86::al, x86::al);
    update_nz();
    as->mov(DR_B(REG(op)), x86::al);
}

void asl_b_r(uint16_t op) {
    as->movsx(x86::rax, DR_B(REG(op)));
    as->mov(x86::cl, DR_B(DN(op)));
    as->and_(x86::cl, 63);
    jit_if(
        COND::FALSE,
        [] {
            as->mov(CC_C, 0);
            as->test(x86::al, x86::al);
            update_nz();
        },
        [op] {
            as->mov(x86::dx, 31);
            as->cmp(x86::cl, x86::dl);
            as->cmovg(x86::cx, x86::dx);

            as->mov(x86::dl, x86::al);
            as->sal(x86::al, x86::cl);
            as->setc(CC_C);
            as->setc(CC_X);
            update_nz();
            as->mov(DR_B(REG(op)), x86::al);

            // overflow check
            as->mov(x86::al, x86::dl);
            as->sar(x86::dl, 7);
            as->cmp(x86::cl, 7);
            jit_if(COND::LE, [] {
                as->mov(x86::sil, 7);
                as->sub(x86::sil, x86::cl);
                as->mov(x86::cl, x86::sil);
                as->sar(x86::al, x86::cl);
            });
            as->cmp(x86::al, x86::dl);
            as->setne(CC_V);
        });
}

void lsl_b_r(uint16_t op) {
    as->mov(x86::al, DR_B(REG(op)));
    as->mov(x86::cl, DR_B(DN(op)));
    as->and_(x86::cl, 63);
    jit_if(
        COND::FALSE, [] { as->mov(CC_C, 0); },
        [op] {
            as->mov(CC_V, 0);

            as->mov(x86::dx, 31);
            as->cmp(x86::cl, x86::dl);
            as->cmovg(x86::cx, x86::dx);

            as->shl(x86::al, x86::cl);
            as->mov(DR_B(REG(op)), x86::al);
            as->setc(CC_C);
            as->setc(CC_X);
        });
    as->test(x86::al, x86::al);
    update_nz();
}

void roxl_b_r(uint16_t op) {
    as->mov(CC_V, 0);
    as->mov(x86::al, DR_B(REG(op)));
    as->mov(x86::cl, DR_B(DN(op)));
    as->and_(x86::cl, 63);
    jit_if(
        COND::FALSE,
        [] {
            as->mov(x86::dl, CC_X);
            as->mov(CC_C, x86::dl);
            as->test(x86::al, x86::al);
            update_nz();
        },
        [op] {
            as->cmp(x86::cl, 31);
            jit_if(
                COND::LE,
                [] {
                    as->mov(x86::dl, CC_X);
                    as->shr(x86::dl, 1);
                },
                [] {
                    as->sub(x86::cl, 32);
                    as->mov(x86::dl, CC_X);
                    as->shr(x86::dl, 1);
                    as->rcl(x86::al, 31);
                    as->rcl(x86::al, 1);
                });
            as->rcl(x86::al, x86::cl);
            as->setc(CC_C);
            as->setc(CC_X);
            as->test(x86::al, x86::al);
            update_nz();
            as->mov(DR_B(REG(op)), x86::al);
        });
}

void rol_b_r(uint16_t op) {
    as->mov(CC_V, 0);
    as->mov(x86::al, DR_B(REG(op)));
    as->mov(x86::cl, DR_B(DN(op)));
    as->and_(x86::cl, 63);
    jit_if(
        COND::FALSE,
        [] {
            as->mov(x86::dl, CC_X);
            as->mov(CC_C, x86::dl);
            as->test(x86::al, x86::al);
            update_nz();
        },
        [op] {
            as->rol(x86::al, x86::cl);
            as->setc(CC_C);
            as->test(x86::al, x86::al);
            update_nz();
            as->mov(DR_B(REG(op)), x86::al);
        });
}
void asl_w_i(uint16_t op) {
    int cnt = DN(op) ? DN(op) : 8;
    as->mov(x86::ax, DR_W(REG(op)));
    as->sal(DR_W(REG(op)), cnt);
    as->setc(CC_C);
    as->setc(CC_X);
    update_nz();
    as->mov(x86::dx, x86::ax);
    as->sar(x86::dx, 15);
    as->sar(x86::ax, 15 - cnt);
    as->cmp(x86::ax, x86::dx);
    as->setne(CC_V);
}

void lsl_w_i(uint16_t op) {
    int cnt = DN(op) ? DN(op) : 8;
    as->mov(CC_V, 0);
    as->shl(DR_W(REG(op)), cnt);
    as->setc(CC_C);
    as->setc(CC_X);
    update_nz();
}

void roxl_w_i(uint16_t op) {
    int cnt = DN(op) ? DN(op) : 8;
    as->mov(CC_V, 0);
    as->mov(x86::ax, DR_W(REG(op)));
    as->mov(x86::dl, CC_X);
    as->shr(x86::dl, 1);
    as->rcl(x86::ax, cnt);
    as->setc(CC_C);
    as->setc(CC_X);
    as->test(x86::ax, x86::ax);
    update_nz();
    as->mov(DR_W(REG(op)), x86::ax);
}

void rol_w_i(uint16_t op) {
    int cnt = DN(op) ? DN(op) : 8;
    as->mov(CC_V, 0);
    as->mov(x86::ax, DR_W(REG(op)));
    as->rol(x86::ax, cnt);
    as->setc(CC_C);
    as->test(x86::ax, x86::ax);
    update_nz();
    as->mov(DR_W(REG(op)), x86::ax);
}

void asl_w_r(uint16_t op) {
    as->mov(x86::ax, DR_W(REG(op)));
    as->mov(x86::cl, DR_B(DN(op)));
    as->and_(x86::cl, 63);
    jit_if(
        COND::FALSE,
        [] {
            as->mov(CC_C, 0);
            as->test(x86::ax, x86::ax);
            update_nz();
        },
        [op] {
            as->mov(x86::dx, x86::ax);
            as->mov(x86::dil, x86::cl);
            as->mov(x86::r8d, 31);
            as->cmp(x86::cl, x86::r8b);
            as->cmovg(x86::cx, x86::r8w);

            as->sal(x86::ax, x86::cl);
            as->setc(CC_C);
            as->setc(CC_X);
            update_nz();
            as->mov(DR_W(REG(op)), x86::ax);

            // overflow check
            as->mov(x86::ax, x86::dx);
            as->mov(x86::cl, x86::dil);
            as->sar(x86::dx, 15);
            as->cmp(x86::cl, 15);
            jit_if(COND::LE, [] {
                as->mov(x86::si, 15);
                as->sub(x86::si, x86::cx);
                as->mov(x86::cx, x86::si);
                as->sar(x86::ax, x86::cl);
            });
            as->cmp(x86::ax, x86::dx);
            as->setne(CC_V);
        });
}

void lsl_w_r(uint16_t op) {
    as->mov(x86::ax, DR_W(REG(op)));
    as->mov(x86::cl, DR_B(DN(op)));
    as->and_(x86::cl, 63);
    jit_if(
        COND::FALSE, [] { as->mov(CC_C, 0); },
        [op] {
            as->mov(CC_V, 0);

            as->mov(x86::dx, 31);
            as->cmp(x86::cl, x86::dl);
            as->cmovg(x86::cx, x86::dx);

            as->shl(x86::ax, x86::cl);
            as->mov(DR_W(REG(op)), x86::ax);
            as->setc(CC_C);
            as->setc(CC_X);
        });
    as->test(x86::ax, x86::ax);
    update_nz();
}

void roxl_w_r(uint16_t op) {
    as->mov(CC_V, 0);
    as->mov(x86::ax, DR_W(REG(op)));
    as->mov(x86::cl, DR_B(DN(op)));
    as->and_(x86::cl, 63);
    jit_if(
        COND::FALSE,
        [] {
            as->mov(x86::dl, CC_X);
            as->mov(CC_C, x86::dl);
            as->test(x86::ax, x86::ax);
            update_nz();
        },
        [op] {
            as->cmp(x86::cl, 31);
            jit_if(
                COND::LE,
                [] {
                    as->mov(x86::dl, CC_X);
                    as->shr(x86::dl, 1);
                },
                [] {
                    as->sub(x86::cl, 32);
                    as->mov(x86::dl, CC_X);
                    as->shr(x86::dl, 1);
                    as->rcl(x86::ax, 31);
                    as->rcl(x86::ax, 1);
                });
            as->rcl(x86::ax, x86::cl);
            as->setc(CC_C);
            as->setc(CC_X);
            as->test(x86::ax, x86::ax);
            update_nz();
            as->mov(DR_W(REG(op)), x86::ax);
        });
}

void rol_w_r(uint16_t op) {
    as->mov(CC_V, 0);
    as->mov(x86::ax, DR_W(REG(op)));
    as->mov(x86::cl, DR_B(DN(op)));
    as->and_(x86::cl, 63);
    jit_if(
        COND::FALSE,
        [] {
            as->mov(x86::dl, CC_X);
            as->mov(CC_C, x86::dl);
            as->test(x86::ax, x86::ax);
            update_nz();
        },
        [op] {
            as->rol(x86::ax, x86::cl);
            as->setc(CC_C);
            as->test(x86::ax, x86::ax);
            update_nz();
            as->mov(DR_W(REG(op)), x86::ax);
        });
}

void asl_l_i(uint16_t op) {
    int cnt = DN(op) ? DN(op) : 8;
    as->mov(x86::eax, DR_L(REG(op)));
    as->sal(DR_L(REG(op)), cnt);
    as->setc(CC_C);
    as->setc(CC_X);
    update_nz();
    as->mov(x86::edx, x86::eax);
    as->sar(x86::edx, 31);
    as->sar(x86::eax, 31 - cnt);
    as->cmp(x86::eax, x86::edx);
    as->setne(CC_V);
}

void lsl_l_i(uint16_t op) {
    int cnt = DN(op) ? DN(op) : 8;
    as->mov(CC_V, 0);
    as->shl(DR_L(REG(op)), cnt);
    as->setc(CC_C);
    as->setc(CC_X);
    update_nz();
}

void roxl_l_i(uint16_t op) {
    int cnt = DN(op) ? DN(op) : 8;
    as->mov(CC_V, 0);
    as->mov(x86::eax, DR_L(REG(op)));
    as->mov(x86::dl, CC_X);
    as->shr(x86::dl, 1);
    as->rcl(x86::eax, cnt);
    as->setc(CC_C);
    as->setc(CC_X);
    as->test(x86::eax, x86::eax);
    update_nz();
    as->mov(DR_L(REG(op)), x86::eax);
}

void rol_l_i(uint16_t op) {
    int cnt = DN(op) ? DN(op) : 8;
    as->mov(CC_V, 0);
    as->mov(x86::eax, DR_L(REG(op)));
    as->rol(x86::eax, cnt);
    as->setc(CC_C);
    as->test(x86::eax, x86::eax);
    update_nz();
    as->mov(DR_L(REG(op)), x86::eax);
}

void asl_l_r(uint16_t op) {
    as->mov(x86::eax, DR_L(REG(op)));
    as->mov(x86::edx, x86::eax);
    as->mov(x86::cl, DR_B(DN(op)));
    as->and_(x86::cl, 63);
    jit_if(
        COND::FALSE,
        [] {
            as->mov(CC_C, 0);
            as->test(x86::eax, x86::eax);
            update_nz();
        },
        [op] {
            as->mov(x86::dil, x86::cl);
            as->cmp(x86::cl, 32);
            jit_if(COND::GE, [] {
                as->sub(x86::cl, 32);
                as->sal(x86::eax, 31);
                as->sal(x86::eax, 1);
            });
            as->sal(x86::eax, x86::cl);
            as->setc(CC_C);
            as->setc(CC_X);
            update_nz();
            as->mov(DR_L(REG(op)), x86::eax);

            // overflow check
            as->mov(x86::eax, x86::edx);
            as->mov(x86::cl, x86::dil);
            as->sar(x86::edx, 31);
            as->cmp(x86::cl, 31);
            jit_if(COND::LE, [] {
                as->mov(x86::si, 31);
                as->sub(x86::si, x86::cx);
                as->mov(x86::cx, x86::si);
                as->sar(x86::eax, x86::cl);
            });
            as->cmp(x86::eax, x86::edx);
            as->setne(CC_V);
        });
}

void lsl_l_r(uint16_t op) {
    as->mov(x86::eax, DR_L(REG(op)));
    as->mov(x86::cl, DR_B(DN(op)));
    as->and_(x86::cl, 63);
    jit_if(
        COND::FALSE, [] { as->mov(CC_C, 0); },
        [op] {
            as->mov(CC_V, 0);

            as->cmp(x86::cl, 32);
            jit_if(COND::GE, [] {
                as->sub(x86::cl, 32);
                as->shl(x86::eax, 31);
                as->shl(x86::eax, 1);
            });
            as->shl(x86::eax, x86::cl);

            as->mov(DR_L(REG(op)), x86::eax);
            as->setc(CC_C);
            as->setc(CC_X);
        });
    as->test(x86::eax, x86::eax);
    update_nz();
}

void roxl_l_r(uint16_t op) {
    as->mov(CC_V, 0);
    as->mov(x86::eax, DR_L(REG(op)));
    as->mov(x86::cl, DR_B(DN(op)));
    as->and_(x86::cl, 63);
    jit_if(
        COND::FALSE,
        [] {
            as->mov(x86::dl, CC_X);
            as->mov(CC_C, x86::dl);
            as->test(x86::eax, x86::eax);
            update_nz();
        },
        [op] {
            as->cmp(x86::cl, 31);
            jit_if(
                COND::LE,
                [] {
                    as->mov(x86::dl, CC_X);
                    as->shr(x86::dl, 1);
                },
                [op] {
                    as->sub(x86::cl, 32);
                    as->mov(x86::dl, CC_X);
                    as->shr(x86::dl, 1);
                    as->rcl(x86::eax, 31);
                    as->rcl(x86::eax, 1);
                });
            as->rcl(x86::eax, x86::cl);
            as->setc(CC_C);
            as->setc(CC_X);
            as->test(x86::eax, x86::eax);
            update_nz();
            as->mov(DR_L(REG(op)), x86::eax);
        });
}

void rol_l_r(uint16_t op) {
    as->mov(CC_V, 0);
    as->mov(x86::eax, DR_L(REG(op)));
    as->mov(x86::cl, DR_B(DN(op)));
    as->and_(x86::cl, 63);
    jit_if(
        COND::FALSE,
        [] {
            as->mov(x86::dl, CC_X);
            as->mov(CC_C, x86::dl);
            as->test(x86::eax, x86::eax);
            update_nz();
        },
        [op] {
            as->cmp(x86::cl, 32);
            jit_if(
                COND::EQ,
                [] {
                    as->mov(x86::edx, x86::eax);
                    as->shr(x86::edx, 1);
                },
                [] { as->rol(x86::eax, x86::cl); });
            as->setc(CC_C);
            as->test(x86::eax, x86::eax);
            update_nz();
            as->mov(DR_L(REG(op)), x86::eax);
        });
}

void asl_ea(uint16_t op) {
    as->mov(CC_V, 0);
    ea_readW_jit(TYPE(op), REG(op), true);
    as->sal(x86::ax, 1);
    as->seto(CC_V);
    as->setc(CC_C);
    as->setc(CC_X);
    update_nz();
    ea_writeW_jit(TYPE(op), REG(op), true);
}

void lsl_ea(uint16_t op) {
    as->mov(CC_V, 0);
    ea_readW_jit(TYPE(op), REG(op), true);
    as->shl(x86::ax, 1);
    as->setc(CC_C);
    as->setc(CC_X);
    update_nz();
    ea_writeW_jit(TYPE(op), REG(op), true);
}

void roxl_ea(uint16_t op) {
    as->mov(CC_V, 0);
    ea_readW_jit(TYPE(op), REG(op), true);
    as->mov(x86::dl, CC_X);
    as->shr(x86::dl, 1);
    as->rcl(x86::ax, 1);
    as->setc(CC_C);
    as->setc(CC_X);
    as->test(x86::ax, x86::ax);
    update_nz();
    ea_writeW_jit(TYPE(op), REG(op), true);
}

void rol_ea(uint16_t op) {
    as->mov(CC_V, 0);
    ea_readW_jit(TYPE(op), REG(op), true);
    as->rol(x86::ax, 1);
    as->setc(CC_C);
    as->test(x86::ax, x86::ax);
    update_nz();
    ea_writeW_jit(TYPE(op), REG(op), true);
}
int bf_common() {
    uint16_t extw = FETCH();
    update_pc();
    int offset = extw >> 6 & 0x1f;
    int width = extw & 0x1f;
    if(extw & 1 << 11) {
        as->mov(x86::r11d, DR_L(offset & 7));
    } else {
        as->mov(x86::r11d, offset);
    }
    if(extw & 1 << 5) {
        as->mov(x86::r12d, DR_L(width & 7));
        as->mov(x86::edx, 32);
        as->test(x86::r12d, x86::r12d);
        as->cmovz(x86::r12d, x86::edx);
    } else {
        as->mov(x86::r12d, width ? width : 32);
    }
    return extw >> 12 & 7;
}

void bftst_dn(uint16_t op) {
    bf_common();
    as->mov(x86::eax, DR_L(REG(op)));
    as->and_(x86::r11d, 31);
    as->mov(x86::cl, x86::r11b);
    as->rol(x86::eax, x86::cl);
    as->mov(x86::edx, -1);
    as->mov(x86::cl, 32);
    as->sub(x86::cl, x86::r12b);
    as->sal(x86::edx, x86::cl);
    as->and_(x86::eax, x86::edx);
    update_nz();
    as->mov(CC_V, 0);
    as->mov(CC_C, 0);
}
void bftst_mem(uint16_t op) {
    bf_common();
    ea_getaddr_jit(TYPE(op), REG(op), 0);
    as->mov(ARG2.r32(), x86::r11d);
    as->mov(ARG3.r32(), x86::r12d);
    as->call(BFTST_M);
}
void bfchg_dn(uint16_t op) {
    bf_common();
    as->mov(x86::eax, DR_L(REG(op)));
    as->mov(x86::esi, x86::eax);
    as->and_(x86::r11d, 31);
    as->mov(x86::cl, x86::r11b);
    as->rol(x86::eax, x86::cl);
    as->mov(x86::edx, -1);
    as->mov(x86::cl, 32);
    as->sub(x86::cl, x86::r12b);
    as->sal(x86::edx, x86::cl);
    as->and_(x86::eax, x86::edx);
    update_nz();
    as->mov(CC_V, 0);
    as->mov(CC_C, 0);
    as->mov(x86::cl, x86::r11b);
    as->ror(x86::edx, x86::cl);
    as->xor_(x86::esi, x86::edx);
    as->mov(DR_L(REG(op)), x86::esi);
}
void bfchg_mem(uint16_t op) {
    bf_common();
    ea_getaddr_jit(TYPE(op), REG(op), 0);
    as->mov(ARG2.r32(), x86::r11d);
    as->mov(ARG3.r32(), x86::r12d);
    as->call(BFCHG_M);
}
void bfclr_dn(uint16_t op) {
    bf_common();
    as->mov(x86::eax, DR_L(REG(op)));
    as->mov(x86::esi, x86::eax);
    as->and_(x86::r11d, 31);
    as->mov(x86::cl, x86::r11b);
    as->rol(x86::eax, x86::cl);
    as->mov(x86::edx, -1);
    as->mov(x86::cl, 32);
    as->sub(x86::cl, x86::r12b);
    as->sal(x86::edx, x86::cl);
    as->and_(x86::eax, x86::edx);
    update_nz();
    as->mov(CC_V, 0);
    as->mov(CC_C, 0);
    as->mov(x86::cl, x86::r11b);
    as->ror(x86::edx, x86::cl);
    as->not_(x86::edx);
    as->and_(x86::esi, x86::edx);
    as->mov(DR_L(REG(op)), x86::esi);
}
void bfclr_mem(uint16_t op) {
    bf_common();
    ea_getaddr_jit(TYPE(op), REG(op), 0);
    as->mov(ARG2.r32(), x86::r11d);
    as->mov(ARG3.r32(), x86::r12d);
    as->call(BFCLR_M);
}
void bfset_dn(uint16_t op) {
    bf_common();
    as->mov(x86::eax, DR_L(REG(op)));
    as->mov(x86::esi, x86::eax);
    as->and_(x86::r11d, 31);
    as->mov(x86::cl, x86::r11b);
    as->rol(x86::eax, x86::cl);
    as->mov(x86::edx, -1);
    as->mov(x86::cl, 32);
    as->sub(x86::cl, x86::r12b);
    as->sal(x86::edx, x86::cl);
    as->and_(x86::eax, x86::edx);
    update_nz();
    as->mov(CC_V, 0);
    as->mov(CC_C, 0);
    as->mov(x86::cl, x86::r11b);
    as->ror(x86::edx, x86::cl);
    as->or_(x86::esi, x86::edx);
    as->mov(DR_L(REG(op)), x86::esi);
}
void bfset_mem(uint16_t op) {
    bf_common();
    ea_getaddr_jit(TYPE(op), REG(op), 0);
    as->mov(ARG2.r32(), x86::r11d);
    as->mov(ARG3.r32(), x86::r12d);
    as->call(BFSET_M);
}
void bfextu_dn(uint16_t op) {
    int dn = bf_common();
    as->mov(x86::eax, DR_L(REG(op)));
    as->and_(x86::r11d, 31);
    as->mov(x86::cl, x86::r11b);
    as->rol(x86::eax, x86::cl);
    as->mov(x86::edx, -1);
    as->mov(x86::cl, 32);
    as->sub(x86::cl, x86::r12b);
    as->sal(x86::edx, x86::cl);
    as->and_(x86::eax, x86::edx);
    update_nz();
    as->mov(CC_V, 0);
    as->mov(CC_C, 0);
    as->shr(x86::eax, x86::cl);
    as->mov(DR_L(dn), x86::eax);
}
void bfextu_mem(uint16_t op) {
    int dn = bf_common();
    ea_getaddr_jit(TYPE(op), REG(op), 0);
    as->mov(ARG2.r32(), x86::r11d);
    as->mov(ARG3.r32(), x86::r12d);
    as->call(BFTST_M);
    as->mov(x86::cl, 32);
    as->sub(x86::cl, x86::r12b);
    as->shr(x86::eax, x86::cl);
    as->mov(DR_L(dn), x86::eax);
}
void bfexts_dn(uint16_t op) {
    int dn = bf_common();
    as->mov(x86::eax, DR_L(REG(op)));
    as->and_(x86::r11d, 31);
    as->mov(x86::cl, x86::r11b);
    as->rol(x86::eax, x86::cl);
    as->mov(x86::edx, -1);
    as->mov(x86::cl, 32);
    as->sub(x86::cl, x86::r12b);
    as->sal(x86::edx, x86::cl);
    as->and_(x86::eax, x86::edx);
    update_nz();
    as->mov(CC_V, 0);
    as->mov(CC_C, 0);
    as->sar(x86::eax, x86::cl);
    as->mov(DR_L(dn), x86::eax);
}
void bfexts_mem(uint16_t op) {
    int dn = bf_common();
    ea_getaddr_jit(TYPE(op), REG(op), 0);
    as->mov(ARG2.r32(), x86::r11d);
    as->mov(ARG3.r32(), x86::r12d);
    as->call(BFTST_M);
    as->mov(x86::cl, 32);
    as->sub(x86::cl, x86::r12b);
    as->sar(x86::eax, x86::cl);
    as->mov(DR_L(dn), x86::eax);
}
void bfffo_dn(uint16_t op) {
    int dn = bf_common();
    as->mov(x86::eax, DR_L(REG(op)));
    as->mov(x86::esi, x86::r11d);
    as->and_(x86::r11d, 31);
    as->mov(x86::cl, x86::r11b);
    as->rol(x86::eax, x86::cl);
    as->mov(x86::edx, -1);
    as->mov(x86::cl, 32);
    as->sub(x86::cl, x86::r12b);
    as->sal(x86::edx, x86::cl);
    as->and_(x86::eax, x86::edx);
    update_nz();
    as->mov(CC_V, 0);
    as->mov(CC_C, 0);
    as->mov(DR_L(dn), x86::esi);
    as->test(x86::eax, x86::eax);
    jit_if(
        COND::FALSE, [] { as->mov(x86::edx, x86::r12d); },
        [] {
            as->bsr(x86::ecx, x86::eax);
            as->mov(x86::edx, 31);
            as->sub(x86::edx, x86::ecx);
        });
    as->add(DR_L(dn), x86::edx);
}
void bfffo_mem(uint16_t op) {
    int dn = bf_common();
    ea_getaddr_jit(TYPE(op), REG(op), 0);
    as->mov(ARG2.r32(), x86::r11d);
    as->mov(ARG3.r32(), x86::r12d);
    as->call(BFTST_M);
    as->mov(DR_L(dn), x86::r11d);
    as->test(x86::eax, x86::eax);
    jit_if(
        COND::FALSE, [] { as->mov(x86::edx, x86::r12d); },
        [] {
            as->bsr(x86::ecx, x86::eax);
            as->mov(x86::edx, 31);
            as->sub(x86::edx, x86::ecx);
        });
    as->add(DR_L(dn), x86::edx);
}

void bfins_dn(uint16_t op) {
    int dn = bf_common();
    as->mov(ARG1.r32(), DR_L(REG(op)));
    as->mov(ARG2.r32(), x86::r11d);
    as->mov(ARG3.r32(), x86::r12d);
    as->mov(ARG4, DR_L(dn));
    as->call(BFINS_D);
    as->mov(DR_L(REG(op)), x86::eax);
}
void bfins_mem(uint16_t op) {
    int dn = bf_common();
    ea_getaddr_jit(TYPE(op), REG(op), 0);
    as->mov(ARG2.r32(), x86::r11d);
    as->mov(ARG3.r32(), x86::r12d);
    as->mov(ARG4, DR_L(dn));
    as->call(BFINS_M);
}

void fline_default(uint16_t) { as->call(FLINE); }

void move16_inc_imm(uint16_t op) {
    uint32_t imm = FETCH32();
    update_pc();
    as->mov(ARG1.r32(), AR_L(REG(op)));
    as->mov(ARG2.r32(), imm);
    as->call(Transfer16);
    as->add(AR_L(REG(op)), 16);
}

void move16_imm_inc(uint16_t op) {
    uint32_t imm = FETCH32();
    update_pc();
    as->mov(ARG1.r32(), imm);
    as->mov(ARG2.r32(), AR_L(REG(op)));
    as->call(Transfer16);
    as->add(AR_L(REG(op)), 16);
}

void move16_base_imm(uint16_t op) {
    uint32_t imm = FETCH32();
    update_pc();
    as->mov(ARG1.r32(), AR_L(REG(op)));
    as->mov(ARG2.r32(), imm);
    as->call(Transfer16);
}

void move16_imm_base(uint16_t op) {
    uint32_t imm = FETCH32();
    update_pc();
    as->mov(ARG1.r32(), imm);
    as->mov(ARG2.r32(), AR_L(REG(op)));
    as->call(Transfer16);
}

void move16_inc_inc(uint16_t op) {
    uint16_t extw = FETCH();
    int ay = extw >> 12 & 7;
    update_pc();
    as->mov(ARG1.r32(), AR_L(REG(op)));
    as->mov(ARG2.r32(), AR_L(ay));
    as->call(Transfer16);
    as->add(AR_L(REG(op)), 16);
    as->add(AR_L(ay), 16);
}

#endif
} // namespace JIT_OP
