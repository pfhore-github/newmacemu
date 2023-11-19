#include "68040.hpp"
#include "asmjit/asmjit.h"
#include "jit.hpp"
#include "memory.hpp"
#include "inline.hpp"
using namespace asmjit;
extern CpuFeatures feature;
void bus_reset();
void do_rte();
void jit_postop(x86::Assembler &a);
extern volatile bool testing;
extern std::unordered_map<uint32_t, Label> jumpMap;
namespace OP {
void stop_impl(uint16_t nw);
}
bool movec_from_cr_impl(uint16_t extw, uint32_t *rn);
bool movec_to_cr_impl(uint16_t extw, uint32_t rn);
namespace JIT_OP {
#ifdef __x86_64__
constexpr auto EA = x86::dword_ptr(x86::rbx, offsetof(Cpu, EA));
constexpr auto CC_C = x86::byte_ptr(x86::rbx, offsetof(Cpu, C));
constexpr auto CC_V = x86::byte_ptr(x86::rbx, offsetof(Cpu, V));
constexpr auto CC_N = x86::byte_ptr(x86::rbx, offsetof(Cpu, N));
constexpr auto CC_Z = x86::byte_ptr(x86::rbx, offsetof(Cpu, Z));
constexpr auto CC_X = x86::byte_ptr(x86::rbx, offsetof(Cpu, X));
constexpr auto CC_T = x86::byte_ptr(x86::rbx, offsetof(Cpu, T));
constexpr auto CC_S = x86::byte_ptr(x86::rbx, offsetof(Cpu, S));
Label end_lbl;
void update_pc(x86::Assembler &a) {
    a.mov(x86::esi, cpu.PC);
    a.mov(x86::dword_ptr(x86::rbx, offsetof(Cpu, PC)), x86::esi);
}
void update_nz(x86::Assembler &a) {
    a.setz(CC_Z);
    a.sets(CC_N);
}
void update_vc(x86::Assembler &a) {
    a.seto(CC_V);
    a.setc(CC_C);
}
void clear_vc(x86::Assembler &a) {
    a.mov(CC_V, 0);
    a.mov(CC_C, 0);
}
void update_vcx(x86::Assembler &a) {
    a.seto(CC_V);
    a.setc(x86::dl);
    a.mov(CC_C, x86::dl);
    a.mov(CC_X, x86::dl);
}
auto DR_B(int n) { return x86::byte_ptr(x86::rbx, n * sizeof(uint32_t)); }
auto DR_W(int n) { return x86::word_ptr(x86::rbx, n * sizeof(uint32_t)); }
auto DR_L(int n) { return x86::dword_ptr(x86::rbx, n * sizeof(uint32_t)); }
auto AR_B(int n) { return x86::byte_ptr(x86::rbx, (8 + n) * sizeof(uint32_t)); }
auto AR_W(int n) { return x86::word_ptr(x86::rbx, (8 + n) * sizeof(uint32_t)); }
auto AR_L(int n) {
    return x86::dword_ptr(x86::rbx, (8 + n) * sizeof(uint32_t));
}
constexpr auto SP = x86::dword_ptr(x86::rbx, 15 * sizeof(uint32_t));

void jit_push16(x86::Assembler &a) {
    a.mov(x86::r13d, SP);
    a.lea(x86::r13, x86::ptr(x86::r13, -2));
    a.mov(ARG1, x86::r13d);
    a.mov(ARG2, x86::eax);
    a.call(WriteW);

    a.sub(SP, 2);
}

void jit_push32(x86::Assembler &a) {
    a.mov(x86::r12d, x86::eax);
    a.mov(x86::r13d, SP);
    a.lea(ARG1L, x86::ptr(x86::r13, -2));
    a.mov(ARG2, x86::eax);
    a.call(WriteW);

    a.lea(ARG1L, x86::ptr(x86::r13, -4));
    a.mov(ARG2, x86::r12d);
    a.ror(ARG2, 16);
    a.call(WriteW);

    a.sub(SP, 4);
}

void jit_pop16(x86::Assembler &a) {
    a.mov(ARG1, SP);
    a.call(ReadW);
    a.add(SP, 2);
}

void jit_pop32(x86::Assembler &a) {
    a.mov(x86::r12d, SP);
    a.mov(ARG1, x86::r12d);
    a.call(ReadW);
    a.mov(x86::r13d, x86::eax);
    a.ror(x86::r13d, 16);
    a.lea(ARG1L, x86::ptr(x86::r12, 2));
    a.call(ReadW);
    a.or_(x86::eax, x86::r13d);
    a.add(SP, 4);
}

void jit_trace_check(x86::Assembler &a) {
    auto lb = a.newLabel();
    a.mov(x86::al, CC_T);
    a.cmp(x86::al, 1);
    a.jne(lb);
    a.mov(x86::byte_ptr(x86::rbx, offsetof(Cpu, must_trace)), 1);
    a.bind(lb);
}
void jit_trace_branch(x86::Assembler &a) {
    auto l = a.newLabel();
    a.mov(x86::al, CC_T);
    a.test(x86::al, 1);
    a.je(l);
    a.mov(x86::byte_ptr(x86::rbx, offsetof(Cpu, must_trace)), 1);
    a.bind(l);
}
void jit_priv_check(x86::Assembler &a) {
    auto l = a.newLabel();
    a.mov(x86::al, CC_S);
    a.test(x86::al, 1);
    a.jne(l);
    a.call(PRIV_ERROR);
    a.bind(l);
}

void jump(x86::Assembler &a) {
    auto lb = a.newLabel();
    a.test(x86::eax, 1);
    a.jz(lb);
    a.mov(ARG1, x86::eax);
    a.call(ADDRESS_ERROR);
    a.bind(lb);
    a.mov(x86::dword_ptr(x86::rbx, offsetof(Cpu, PC)), x86::eax);
    jit_trace_branch(a);
    jit_postop(a);
    a.jmp(end_lbl);
}

void jumpC(x86::Assembler &a, uint32_t t) {
    if(t & 1) {
        a.call(ADDRESS_ERROR);
        return;
    }
    a.mov(x86::eax, t);
    a.mov(x86::dword_ptr(x86::rbx, offsetof(Cpu, PC)), x86::eax);
    jit_trace_branch(a);
    jit_postop(a);
    a.jmp(jumpMap[t]);
}
#ifdef CI
void jit_exit(x86::Assembler &a, uint16_t) {
    update_pc(a);
    a.mov(x86::rdi, reinterpret_cast<intptr_t>(&testing));
    a.mov(x86::al, x86::byte_ptr(x86::rdi));
    a.xor_(x86::al, x86::al);
    a.mov(x86::byte_ptr(x86::rdi), x86::al);
    a.jmp(end_lbl);
}
#endif
void ori_b(x86::Assembler &a, uint16_t op) {
    uint8_t v = FETCH();
    ea_readB_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.or_(x86::al, v);
    update_nz(a);
    clear_vc(a);
    ea_writeB_jit(a, TYPE(op), REG(op), true);
}

void ori_w(x86::Assembler &a, uint16_t op) {
    uint16_t v = FETCH();
    ea_readW_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.or_(x86::ax, v);
    update_nz(a);
    clear_vc(a);
    ea_writeW_jit(a, TYPE(op), REG(op), true);
}
void ori_l(x86::Assembler &a, uint16_t op) {
    uint32_t v = FETCH32();
    ea_readL_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.or_(x86::eax, v);
    update_nz(a);
    clear_vc(a);
    ea_writeL_jit(a, TYPE(op), REG(op), true);
}

void ori_b_ccr(x86::Assembler &a, uint16_t) {
    uint16_t v = FETCH();
    update_pc(a);
    a.call(GetCCR);
    a.or_(x86::al, v);
    a.mov(ARG1, x86::eax);
    a.call(SetCCR);
}

void ori_w_sr(x86::Assembler &a, uint16_t) {
    uint16_t v = FETCH();
    a.call(PRIV_CHECK);
    update_pc(a);
    a.call(GetSR);
    a.or_(x86::ax, v);
    a.mov(ARG1, x86::eax);
    a.call(SetSR);
    jit_trace_branch(a);
}
void andi_b(x86::Assembler &a, uint16_t op) {
    uint8_t v = FETCH();
    ea_readB_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.and_(x86::al, v);
    update_nz(a);
    clear_vc(a);
    ea_writeB_jit(a, TYPE(op), REG(op), true);
}

void andi_w(x86::Assembler &a, uint16_t op) {
    uint16_t v = FETCH();
    ea_readW_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.and_(x86::ax, v);
    update_nz(a);
    clear_vc(a);
    ea_writeW_jit(a, TYPE(op), REG(op), true);
}
void andi_l(x86::Assembler &a, uint16_t op) {
    uint32_t v = FETCH32();
    ea_readL_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.and_(x86::eax, v);
    update_nz(a);
    clear_vc(a);
    ea_writeL_jit(a, TYPE(op), REG(op), true);
}

void andi_b_ccr(x86::Assembler &a, uint16_t) {
    uint16_t v = FETCH();
    update_pc(a);
    a.call(GetCCR);
    a.and_(x86::al, v);
    a.mov(ARG1, x86::eax);
    a.call(SetCCR);
}

void andi_w_sr(x86::Assembler &a, uint16_t) {
    uint16_t v = FETCH();
    a.call(PRIV_CHECK);
    update_pc(a);
    a.call(GetSR);
    a.and_(x86::ax, v);
    a.mov(ARG1, x86::eax);
    a.call(SetSR);
    jit_trace_branch(a);
}

void eori_b(x86::Assembler &a, uint16_t op) {
    uint8_t v = FETCH();
    ea_readB_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.xor_(x86::al, v);
    update_nz(a);
    clear_vc(a);
    ea_writeB_jit(a, TYPE(op), REG(op), true);
}

void eori_w(x86::Assembler &a, uint16_t op) {
    uint16_t v = FETCH();
    ea_readW_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.xor_(x86::ax, v);
    update_nz(a);
    clear_vc(a);
    ea_writeW_jit(a, TYPE(op), REG(op), true);
}
void eori_l(x86::Assembler &a, uint16_t op) {
    uint32_t v = FETCH32();
    ea_readL_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.xor_(x86::eax, v);
    update_nz(a);
    clear_vc(a);
    ea_writeL_jit(a, TYPE(op), REG(op), true);
}

void eori_b_ccr(x86::Assembler &a, uint16_t) {
    uint16_t v = FETCH();
    update_pc(a);
    a.call(GetCCR);
    a.xor_(x86::al, v);
    a.mov(ARG1, x86::eax);
    a.call(SetCCR);
}

void eori_w_sr(x86::Assembler &a, uint16_t) {
    uint16_t v = FETCH();
    a.call(PRIV_CHECK);
    update_pc(a);
    a.call(GetSR);
    a.xor_(x86::ax, v);
    a.mov(ARG1, x86::eax);
    a.call(SetSR);
    jit_trace_branch(a);
}

void subi_b(x86::Assembler &a, uint16_t op) {
    uint8_t v = FETCH();
    ea_readB_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.sub(x86::al, v);
    update_nz(a);
    update_vcx(a);
    ea_writeB_jit(a, TYPE(op), REG(op), true);
}

void subi_w(x86::Assembler &a, uint16_t op) {
    uint16_t v = FETCH();
    ea_readW_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.sub(x86::ax, v);
    update_nz(a);
    update_vcx(a);
    ea_writeW_jit(a, TYPE(op), REG(op), true);
}
void subi_l(x86::Assembler &a, uint16_t op) {
    uint32_t v = FETCH32();
    ea_readL_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.sub(x86::eax, v);
    update_nz(a);
    update_vcx(a);
    ea_writeL_jit(a, TYPE(op), REG(op), true);
}

void addi_b(x86::Assembler &a, uint16_t op) {
    uint8_t v = FETCH();
    ea_readB_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.add(x86::al, v);
    update_nz(a);
    update_vcx(a);
    ea_writeB_jit(a, TYPE(op), REG(op), true);
}

void addi_w(x86::Assembler &a, uint16_t op) {
    uint16_t v = FETCH();
    ea_readW_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.add(x86::ax, v);
    update_nz(a);
    update_vcx(a);
    ea_writeW_jit(a, TYPE(op), REG(op), true);
}
void addi_l(x86::Assembler &a, uint16_t op) {
    uint32_t v = FETCH32();
    ea_readL_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.add(x86::eax, v);
    update_nz(a);
    update_vcx(a);
    ea_writeL_jit(a, TYPE(op), REG(op), true);
}

void cmpi_b(x86::Assembler &a, uint16_t op) {
    uint8_t v = FETCH();
    ea_readB_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.sub(x86::al, v);
    update_nz(a);
    update_vc(a);
}

void cmpi_w(x86::Assembler &a, uint16_t op) {
    uint16_t v = FETCH();
    ea_readW_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.sub(x86::ax, v);
    update_nz(a);
    update_vc(a);
}
void cmpi_l(x86::Assembler &a, uint16_t op) {
    uint32_t v = FETCH32();
    ea_readL_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.sub(x86::eax, v);
    update_nz(a);
    update_vc(a);
}

void cmp2_chk2_b(x86::Assembler &a, uint16_t op) {
    uint16_t nextop = FETCH();
    bool s = nextop & 1 << 15;
    int rn = nextop >> 12 & 7;
    bool chk2 = nextop & 1 << 11;
    ea_getaddr_jit(a, TYPE(op), REG(op), 1);
    a.mov(x86::r12d, x86::r9d);
    update_pc(a);
    a.mov(ARG1, x86::r12d);
    a.call(ReadB);
    a.mov(x86::r13b, x86::al); // LOW
    a.mov(ARG1, x86::r12d);
    a.add(ARG1, 1);
    a.call(ReadB);
    a.mov(x86::sil, x86::al); // HIGH
    a.mov(x86::dil, DR_B(rn | s << 3));

    a.cmp(x86::dil, x86::r13b);
    a.sete(x86::cl);
    if(s) {
        a.setl(x86::ch);
    } else {
        a.setb(x86::ch);
    }
    a.cmp(x86::sil, x86::dil);
    a.sete(x86::al);
    if(s) {
        a.setl(x86::ah);
    } else {
        a.setb(x86::ah);
    }
    a.or_(x86::ax, x86::cx);
    a.mov(x86::cl, x86::ah);
    a.mov(CC_Z, x86::al);
    a.mov(CC_C, x86::cl);
    if(chk2) {
        auto lb = a.newLabel();
        a.test(x86::cl, 1);
        a.je(lb);
        a.call(CHK_ERROR);
        a.bind(lb);
    }
}

void cmp2_chk2_w(x86::Assembler &a, uint16_t op) {
    uint16_t nextop = FETCH();
    bool s = nextop & 1 << 15;
    int rn = nextop >> 12 & 7;
    bool chk2 = nextop & 1 << 11;
    ea_getaddr_jit(a, TYPE(op), REG(op), 2);
    a.mov(x86::r12d, x86::r9d);
    update_pc(a);
    a.mov(ARG1, x86::r12d);
    a.call(ReadW);
    a.mov(x86::r13w, x86::ax); // LOW
    a.mov(ARG1, x86::r12d);
    a.add(ARG1, 2);
    a.call(ReadW);
    a.mov(x86::si, x86::ax); // HIGH
    a.mov(x86::di, DR_W(rn | s << 3));

    a.cmp(x86::di, x86::r13w);
    a.sete(x86::cl);
    if(s) {
        a.setl(x86::ch);
    } else {
        a.setb(x86::ch);
    }
    a.cmp(x86::si, x86::di);
    a.sete(x86::al);
    if(s) {
        a.setl(x86::ah);
    } else {
        a.setb(x86::ah);
    }
    a.or_(x86::ax, x86::cx);
    a.mov(x86::cl, x86::ah);
    a.mov(CC_Z, x86::al);
    a.mov(CC_C, x86::cl);
    if(chk2) {
        auto lb = a.newLabel();
        a.test(x86::cl, 1);
        a.je(lb);
        a.call(CHK_ERROR);
        a.bind(lb);
    }
}

void cmp2_chk2_l(x86::Assembler &a, uint16_t op) {
    uint16_t nextop = FETCH();
    bool s = nextop & 1 << 15;
    int rn = nextop >> 12 & 7;
    bool chk2 = nextop & 1 << 11;
    ea_getaddr_jit(a, TYPE(op), REG(op), 4);
    a.mov(x86::r12d, x86::r9d);
    update_pc(a);
    a.mov(ARG1, x86::r12d);
    a.call(ReadL);
    a.mov(x86::r13d, x86::eax); // LOW
    a.mov(ARG1, x86::r12d);
    a.add(ARG1, 4);
    a.call(ReadL);
    a.mov(x86::esi, x86::eax); // HIGH
    a.mov(x86::edi, DR_L(rn | s << 3));

    a.cmp(x86::edi, x86::r13d);
    a.sete(x86::cl);
    if(s) {
        a.setl(x86::ch);
    } else {
        a.setb(x86::ch);
    }
    a.cmp(x86::esi, x86::edi);
    a.sete(x86::al);
    if(s) {
        a.setl(x86::ah);
    } else {
        a.setb(x86::ah);
    }
    a.or_(x86::ax, x86::cx);
    a.mov(x86::cl, x86::ah);
    a.mov(CC_Z, x86::al);
    a.mov(CC_C, x86::cl);
    if(chk2) {
        auto lb = a.newLabel();
        a.test(x86::cl, 1);
        a.je(lb);
        a.call(CHK_ERROR);
        a.bind(lb);
    }
}
void btst_l_i(x86::Assembler &a, uint16_t op) {
    int pos = FETCH() & 31;
    update_pc(a);
    a.mov(x86::eax, DR_L(REG(op)));
    a.bt(x86::eax, pos);
    a.setnc(CC_Z);
}

void btst_b_i(x86::Assembler &a, uint16_t op) {
    int pos = FETCH() & 7;
    ea_readB_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.bt(x86::ax, pos);
    a.setnc(CC_Z);
}

void btst_b_i_imm(x86::Assembler &a, uint16_t) {
    int pos = FETCH() & 7;
    uint8_t imm = FETCH();
    update_pc(a);
    a.mov(x86::al, imm);
    a.bt(x86::ax, pos);
    a.setnc(CC_Z);
}

void btst_l_r(x86::Assembler &a, uint16_t op) {
    update_pc(a);
    a.mov(x86::eax, DR_L(REG(op)));
    a.mov(x86::cl, DR_B(DN(op)));
    a.and_(x86::cl, 31);
    a.bt(x86::eax, x86::cl);
    a.setnc(CC_Z);
}

void btst_b_r(x86::Assembler &a, uint16_t op) {
    ea_readB_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.mov(x86::cl, DR_L(DN(op)));
    a.and_(x86::cl, 7);
    a.bt(x86::ax, x86::cl);
    a.setnc(CC_Z);
}

void btst_b_r_imm(x86::Assembler &a, uint16_t op) {
    uint8_t imm = FETCH();
    update_pc(a);
    a.mov(x86::al, imm);
    a.mov(x86::cl, DR_B(DN(op)));
    a.and_(x86::cl, 7);
    a.bt(x86::ax, x86::cl);
    a.setnc(CC_Z);
}

void bchg_l_i(x86::Assembler &a, uint16_t op) {
    int pos = FETCH() & 31;
    update_pc(a);
    a.mov(x86::eax, DR_L(REG(op)));
    a.btc(x86::eax, pos);
    a.setnc(CC_Z);
    a.mov(DR_L(REG(op)), x86::eax);
}

void bchg_b_i(x86::Assembler &a, uint16_t op) {
    int pos = FETCH() & 7;
    ea_readB_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.btc(x86::ax, pos);
    a.setnc(CC_Z);
    ea_writeB_jit(a, TYPE(op), REG(op), true);
}

void bchg_l_r(x86::Assembler &a, uint16_t op) {
    update_pc(a);
    a.mov(x86::eax, DR_L(REG(op)));
    a.mov(x86::cl, DR_B(DN(op)));
    a.and_(x86::cl, 31);
    a.btc(x86::eax, x86::cl);
    a.setnc(CC_Z);
    a.mov(DR_L(REG(op)), x86::eax);
}

void bchg_b_r(x86::Assembler &a, uint16_t op) {
    ea_readB_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.mov(x86::cl, DR_B(DN(op)));
    a.and_(x86::cl, 7);
    a.btc(x86::ax, x86::cl);
    a.setnc(CC_Z);
    ea_writeB_jit(a, TYPE(op), REG(op), true);
}

void bclr_l_i(x86::Assembler &a, uint16_t op) {
    int pos = FETCH() & 31;
    update_pc(a);
    a.mov(x86::eax, DR_L(REG(op)));
    a.btr(x86::eax, pos);
    a.setnc(CC_Z);
    a.mov(DR_L(REG(op)), x86::eax);
}

void bclr_b_i(x86::Assembler &a, uint16_t op) {
    int pos = FETCH() & 7;
    ea_readB_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.btr(x86::ax, pos);
    a.setnc(CC_Z);
    ea_writeB_jit(a, TYPE(op), REG(op), true);
}

void bclr_l_r(x86::Assembler &a, uint16_t op) {
    update_pc(a);
    a.mov(x86::eax, DR_L(REG(op)));
    a.mov(x86::cl, DR_B(DN(op)));
    a.and_(x86::cl, 31);
    a.btr(x86::eax, x86::cl);
    a.setnc(CC_Z);
    a.mov(DR_L(REG(op)), x86::eax);
}

void bclr_b_r(x86::Assembler &a, uint16_t op) {
    ea_readB_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.mov(x86::cl, DR_B(DN(op)));
    a.and_(x86::cl, 7);
    a.btr(x86::ax, x86::cl);
    a.setnc(CC_Z);
    ea_writeB_jit(a, TYPE(op), REG(op), true);
}

void bset_l_i(x86::Assembler &a, uint16_t op) {
    int pos = FETCH() & 31;
    update_pc(a);
    a.mov(x86::eax, DR_L(REG(op)));
    a.bts(x86::eax, pos);
    a.setnc(CC_Z);
    a.mov(DR_L(REG(op)), x86::eax);
}

void bset_b_i(x86::Assembler &a, uint16_t op) {
    int pos = FETCH() & 7;
    ea_readB_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.bts(x86::ax, pos);
    a.setnc(CC_Z);
    ea_writeB_jit(a, TYPE(op), REG(op), true);
}

void bset_l_r(x86::Assembler &a, uint16_t op) {
    update_pc(a);
    a.mov(x86::eax, DR_L(REG(op)));
    a.mov(x86::cl, DR_B(DN(op)));
    a.and_(x86::cl, 31);
    a.bts(x86::eax, x86::cl);
    a.setnc(CC_Z);
    a.mov(DR_L(REG(op)), x86::eax);
}

void bset_b_r(x86::Assembler &a, uint16_t op) {
    ea_readB_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.mov(x86::cl, DR_B(DN(op)));
    a.and_(x86::cl, 7);
    a.bts(x86::al, x86::cl);
    a.setnc(CC_Z);
    ea_writeB_jit(a, TYPE(op), REG(op), true);
}
void movep_w_load(x86::Assembler &a, uint16_t op) {
    int16_t disp = FETCH();
    update_pc(a);
    a.mov(x86::r10d, AR_L(REG(op)));
    a.add(x86::r10d, disp);
    a.mov(ARG1, x86::r10d);
    a.call(ReadB);
    a.mov(x86::r11b, x86::al);
    a.shl(x86::r11w, 8);
    a.lea(x86::r10, x86::ptr(x86::r10, 2));
    a.mov(ARG1, x86::r10d);
    a.call(ReadB);
    a.mov(x86::r11b, x86::al);
    a.mov(DR_W(DN(op)), x86::r11w);
}

void movep_l_load(x86::Assembler &a, uint16_t op) {
    int16_t disp = FETCH();
    update_pc(a);
    a.mov(x86::r10d, AR_L(REG(op)));
    a.add(x86::r10d, disp);
    a.mov(ARG1, x86::r10d);
    a.call(ReadB);
    a.shl(x86::eax, 24);
    a.mov(x86::r11d, x86::eax);

    a.lea(x86::r10, x86::ptr(x86::r10, 2));
    a.mov(ARG1, x86::r10d);
    a.call(ReadB);
    a.shl(x86::eax, 16);
    a.or_(x86::r11d, x86::eax);

    a.lea(x86::r10, x86::ptr(x86::r10, 2));
    a.mov(ARG1, x86::r10d);
    a.call(ReadB);
    a.shl(x86::ax, 8);
    a.or_(x86::r11w, x86::ax);

    a.lea(x86::r10, x86::ptr(x86::r10, 2));
    a.mov(ARG1, x86::r10d);
    a.call(ReadB);
    a.mov(x86::r11b, x86::al);
    a.mov(DR_L(DN(op)), x86::r11d);
}

void movep_w_store(x86::Assembler &a, uint16_t op) {
    int16_t disp = FETCH();
    update_pc(a);
    a.mov(x86::r10d, AR_L(REG(op)));
    a.add(x86::r10d, disp);
    a.mov(x86::r11w, DR_W(DN(op)));

    a.mov(ARG1, x86::r10d);
    a.mov(ARG2, x86::r11d);
    a.shr(ARG2, 8);
    a.call(WriteB);

    a.lea(x86::r10, x86::ptr(x86::r10, 2));
    a.mov(ARG1, x86::r10d);
    a.mov(ARG2, x86::r11d);
    a.call(WriteB);
}

void movep_l_store(x86::Assembler &a, uint16_t op) {
    int16_t disp = FETCH();
    update_pc(a);
    a.mov(x86::r10d, AR_L(REG(op)));
    a.add(x86::r10d, disp);
    a.mov(x86::r11d, DR_L(DN(op)));

    a.mov(ARG1, x86::r10d);
    a.mov(ARG2, x86::r11d);
    a.shr(ARG2, 24);
    a.call(WriteB);

    a.lea(x86::r10, x86::ptr(x86::r10, 2));
    a.mov(ARG1, x86::r10d);
    a.mov(ARG2, x86::r11d);
    a.shr(ARG2, 16);
    a.call(WriteB);

    a.lea(x86::r10, x86::ptr(x86::r10, 2));
    a.mov(ARG1, x86::r10d);
    a.mov(ARG2, x86::r11d);
    a.shr(ARG2, 8);
    a.call(WriteB);

    a.lea(x86::r10, x86::ptr(x86::r10, 2));
    a.mov(ARG1, x86::r10d);
    a.mov(ARG2, x86::r11d);
    a.call(WriteB);
}

void cas_b(x86::Assembler &a, uint16_t op) {
    auto lb = a.newLabel();
    auto lb2 = a.newLabel();
    uint16_t extw = FETCH();
    int du = extw >> 6 & 7;
    int dc = extw & 7;
    ea_readB_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.mov(x86::dl, DR_B(dc));
    a.cmp(x86::dl, x86::al);
    update_nz(a);
    update_vc(a);
    a.jne(lb);
    a.mov(x86::al, DR_B(du));
    ea_writeB_jit(a, TYPE(op), REG(op), true);
    a.jmp(lb2);
    a.bind(lb);
    a.mov(DR_B(dc), x86::al);
    a.bind(lb2);
    jit_trace_check(a);
}

void cas_w(x86::Assembler &a, uint16_t op) {
    auto lb = a.newLabel();
    auto lb2 = a.newLabel();
    uint16_t extw = FETCH();
    int du = extw >> 6 & 7;
    int dc = extw & 7;
    ea_readW_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.mov(x86::dx, DR_W(dc));
    a.cmp(x86::dx, x86::ax);
    update_nz(a);
    update_vc(a);
    a.jne(lb);
    a.mov(x86::ax, DR_W(du));
    ea_writeW_jit(a, TYPE(op), REG(op), true);
    a.jmp(lb2);
    a.bind(lb);
    a.mov(DR_W(dc), x86::ax);
    a.bind(lb2);
    jit_trace_check(a);
}

void cas_l(x86::Assembler &a, uint16_t op) {
    auto lb = a.newLabel();
    auto lb2 = a.newLabel();
    uint16_t extw = FETCH();
    int du = extw >> 6 & 7;
    int dc = extw & 7;
    ea_readL_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.mov(x86::edx, DR_L(dc));
    a.cmp(x86::edx, x86::eax);
    update_nz(a);
    update_vc(a);
    a.jne(lb);
    a.mov(x86::eax, DR_L(du));
    ea_writeL_jit(a, TYPE(op), REG(op), true);
    a.jmp(lb2);
    a.bind(lb);
    a.mov(DR_L(dc), x86::eax);
    a.bind(lb2);
    jit_trace_check(a);
}

void cas2_w(x86::Assembler &a, uint16_t) {
    auto lb1 = a.newLabel();
    auto lb2 = a.newLabel();
    auto lb3 = a.newLabel();
    uint16_t extw1 = FETCH();
    uint16_t extw2 = FETCH();
    int rn1 = extw1 >> 12 & 15;
    int du1 = extw1 >> 6 & 7;
    int dc1 = extw1 & 7;
    int rn2 = extw2 >> 12 & 15;
    int du2 = extw2 >> 6 & 7;
    int dc2 = extw2 & 7;
    update_pc(a);
    a.mov(ARG1, DR_L(rn1));
    a.call(ReadW);
    a.mov(x86::r12w, x86::ax);
    a.mov(ARG1, DR_L(rn2));
    a.call(ReadW);
    a.mov(x86::r13w, x86::ax);

    a.cmp(x86::r12w, DR_W(dc1));
    update_nz(a);
    update_vc(a);
    a.jne(lb1);
    a.cmp(x86::r13w, DR_W(dc2));
    update_nz(a);
    update_vc(a);
    a.bind(lb1);
    a.jne(lb2);
    a.mov(ARG1, DR_L(rn1));
    a.mov(ARG2, DR_W(du1));
    a.call(WriteW);
    a.mov(ARG1, DR_L(rn2));
    a.mov(ARG2, DR_W(du2));
    a.call(WriteW);
    a.jmp(lb3);
    a.bind(lb2);
    a.mov(DR_W(dc1), x86::r12w);
    a.mov(DR_W(dc2), x86::r13w);
    a.bind(lb3);
    jit_trace_check(a);
}

void cas2_l(x86::Assembler &a, uint16_t) {
    auto lb1 = a.newLabel();
    auto lb2 = a.newLabel();
    auto lb3 = a.newLabel();
    uint16_t extw1 = FETCH();
    uint16_t extw2 = FETCH();
    int rn1 = extw1 >> 12 & 15;
    int du1 = extw1 >> 6 & 7;
    int dc1 = extw1 & 7;
    int rn2 = extw2 >> 12 & 15;
    int du2 = extw2 >> 6 & 7;
    int dc2 = extw2 & 7;
    update_pc(a);
    a.mov(ARG1, DR_L(rn1));
    a.call(ReadL);
    a.mov(x86::r12d, x86::eax);
    a.mov(ARG1, DR_L(rn2));
    a.call(ReadL);
    a.mov(x86::r13d, x86::eax);

    a.cmp(x86::r12d, DR_L(dc1));
    update_nz(a);
    update_vc(a);
    a.jne(lb1);
    a.cmp(x86::r13d, DR_L(dc2));
    update_nz(a);
    update_vc(a);
    a.bind(lb1);
    a.jne(lb2);
    a.mov(ARG1, DR_L(rn1));
    a.mov(ARG2, DR_L(du1));
    a.call(WriteL);
    a.mov(ARG1, DR_L(rn2));
    a.mov(ARG2, DR_L(du2));
    a.call(WriteL);
    a.jmp(lb3);
    a.bind(lb2);
    a.mov(DR_L(dc1), x86::r12d);
    a.mov(DR_L(dc2), x86::r13d);
    a.bind(lb3);
    jit_trace_check(a);
}

void moves_b(x86::Assembler &a, uint16_t op) {
    uint16_t extw = FETCH();
    int rn = extw >> 12 & 15;
    jit_priv_check(a);
    a.mov(x86::rdi, reinterpret_cast<intptr_t>(&cpu.faultParam->tt));
    a.mov(x86::byte_ptr(x86::rdi), TT::ALT);
    ea_getaddr_jit(a, TYPE(op), REG(op), 1);
    update_pc(a);
    a.mov(ARG1, x86::r9d);
    if(extw & 1 << 11) {
        a.movzx(ARG2, DR_B(rn));
        a.mov(ARG3, 1);
        a.call(WriteB);
    } else {
        a.mov(ARG2, 1);
        a.call(ReadB);
        a.mov(DR_B(rn), x86::al);
    }
    jit_trace_check(a);
}

void moves_w(x86::Assembler &a, uint16_t op) {
    uint16_t extw = FETCH();
    int rn = extw >> 12 & 15;
    jit_priv_check(a);
    a.mov(x86::rdi, reinterpret_cast<intptr_t>(&cpu.faultParam->tt));
    a.mov(x86::byte_ptr(x86::rdi), TT::ALT);
    ea_getaddr_jit(a, TYPE(op), REG(op), 2);
    update_pc(a);
    a.mov(ARG1, x86::r9d);
    if(extw & 1 << 11) {
        a.movzx(ARG2, DR_W(rn));
        a.mov(ARG3, 1);
        a.call(WriteW);
    } else {
        a.mov(ARG2, 1);
        a.call(ReadW);
        a.mov(DR_W(rn), x86::ax);
    }
    jit_trace_check(a);
}

void moves_l(x86::Assembler &a, uint16_t op) {
    uint16_t extw = FETCH();
    int rn = extw >> 12 & 15;
    jit_priv_check(a);
    ea_getaddr_jit(a, TYPE(op), REG(op), 4);
    a.mov(x86::rdi, reinterpret_cast<intptr_t>(&cpu.faultParam->tt));
    a.mov(x86::byte_ptr(x86::rdi), TT::ALT);
    update_pc(a);
    a.mov(ARG1, x86::r9d);
    if(extw & 1 << 11) {
        a.mov(ARG2, DR_L(rn));
        a.mov(ARG3, 1);
        a.call(WriteL);
    } else {
        a.mov(ARG2, 1);
        a.call(ReadL);
        a.mov(DR_L(rn), x86::eax);
    }
    jit_trace_check(a);
}

void move_b(x86::Assembler &a, uint16_t op) {
    int dst_type = op >> 6 & 7;
    ea_readB_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.or_(x86::al, x86::al);
    update_nz(a);
    clear_vc(a);
    ea_writeB_jit(a, dst_type, DN(op), false);
    update_pc(a);
}

void move_w(x86::Assembler &a, uint16_t op) {
    int dst_type = op >> 6 & 7;
    ea_readW_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.or_(x86::ax, x86::ax);
    update_nz(a);
    clear_vc(a);
    ea_writeW_jit(a, dst_type, DN(op), false);
}

void move_l(x86::Assembler &a, uint16_t op) {
    int dst_type = op >> 6 & 7;
    ea_readL_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.or_(x86::eax, x86::eax);
    update_nz(a);
    clear_vc(a);
    ea_writeL_jit(a, dst_type, DN(op), false);
}

void movea_w(x86::Assembler &a, uint16_t op) {
    ea_readW_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.cwde();
    a.mov(AR_L(DN(op)), x86::eax);
}

void movea_l(x86::Assembler &a, uint16_t op) {
    ea_readL_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.mov(AR_L(DN(op)), x86::eax);
}

void move_from_sr(x86::Assembler &a, uint16_t op) {
    jit_priv_check(a);
    update_pc(a);
    a.call(GetSR);
    ea_writeW_jit(a, TYPE(op), REG(op), false);
}

void move_from_ccr(x86::Assembler &a, uint16_t op) {
    update_pc(a);
    a.call(GetCCR);
    ea_writeW_jit(a, TYPE(op), REG(op), false);
}

void move_to_sr(x86::Assembler &a, uint16_t op) {
    jit_priv_check(a);
    ea_readW_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.movzx(ARG1, x86::ax);
    a.call(SetSR);
    jit_trace_branch(a);
}

void move_to_ccr(x86::Assembler &a, uint16_t op) {
    ea_readW_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.movzx(ARG1, x86::al);
    a.call(SetCCR);
}

void negx_b(x86::Assembler &a, uint16_t op) {
    ea_readB_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.add(x86::al, CC_X);
    a.neg(x86::al);
    update_nz(a);
    update_vcx(a);
    ea_writeB_jit(a, TYPE(op), REG(op), true);
}

void negx_w(x86::Assembler &a, uint16_t op) {
    ea_readW_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.add(x86::ax, CC_X);
    a.neg(x86::ax);
    update_nz(a);
    update_vcx(a);
    ea_writeW_jit(a, TYPE(op), REG(op), true);
}

void negx_l(x86::Assembler &a, uint16_t op) {
    ea_readL_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.add(x86::eax, CC_X);
    a.neg(x86::eax);
    update_nz(a);
    update_vcx(a);
    ea_writeL_jit(a, TYPE(op), REG(op), true);
}

void clr_b(x86::Assembler &a, uint16_t op) {
    a.xor_(x86::al, x86::al);
    update_nz(a);
    clear_vc(a);
    ea_writeB_jit(a, TYPE(op), REG(op), false);
    update_pc(a);
}

void clr_w(x86::Assembler &a, uint16_t op) {
    a.xor_(x86::ax, x86::ax);
    update_nz(a);
    clear_vc(a);
    ea_writeW_jit(a, TYPE(op), REG(op), false);
    update_pc(a);
}

void clr_l(x86::Assembler &a, uint16_t op) {
    a.xor_(x86::eax, x86::eax);
    update_nz(a);
    clear_vc(a);
    ea_writeL_jit(a, TYPE(op), REG(op), false);
    update_pc(a);
}

void neg_b(x86::Assembler &a, uint16_t op) {
    ea_readB_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.neg(x86::al);
    update_nz(a);
    update_vcx(a);
    ea_writeB_jit(a, TYPE(op), REG(op), true);
}

void neg_w(x86::Assembler &a, uint16_t op) {
    ea_readW_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.neg(x86::ax);
    update_nz(a);
    update_vcx(a);
    ea_writeW_jit(a, TYPE(op), REG(op), true);
}

void neg_l(x86::Assembler &a, uint16_t op) {
    ea_readL_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.neg(x86::eax);
    update_nz(a);
    update_vcx(a);
    ea_writeL_jit(a, TYPE(op), REG(op), true);
}

void not_b(x86::Assembler &a, uint16_t op) {
    ea_readB_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.not_(x86::al);
    a.test(x86::al, x86::al);
    update_nz(a);
    clear_vc(a);
    ea_writeB_jit(a, TYPE(op), REG(op), true);
}

void not_w(x86::Assembler &a, uint16_t op) {
    ea_readW_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.not_(x86::ax);
    a.test(x86::ax, x86::ax);
    update_nz(a);
    clear_vc(a);
    ea_writeW_jit(a, TYPE(op), REG(op), true);
}

void not_l(x86::Assembler &a, uint16_t op) {
    ea_readL_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.not_(x86::eax);
    a.test(x86::eax, x86::eax);
    update_nz(a);
    clear_vc(a);
    ea_writeL_jit(a, TYPE(op), REG(op), true);
}
// BCD is rarely used and complecated, so not inlined
void nbcd(x86::Assembler &a, uint16_t op) {
    ea_readB_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.mov(ARG1, x86::eax);
    a.movzx(ARG2, CC_X);
    a.call(do_nbcd);
    ea_writeB_jit(a, TYPE(op), REG(op), true);
}

void link_l(x86::Assembler &a, uint16_t op) {
    int32_t disp = FETCH32();
    int reg = REG(op);
    update_pc(a);
    a.mov(x86::eax, AR_L(reg));
    jit_push32(a);

    a.mov(x86::eax, SP);
    a.mov(AR_L(reg), x86::eax);
    a.add(SP, disp);
}

void swap(x86::Assembler &a, uint16_t op) {
    const auto reg = DR_L(REG(op));
    a.mov(x86::eax, reg);
    clear_vc(a);
    if(feature.x86().hasBMI2()) {
        a.rorx(x86::eax, x86::eax, 16);
    } else {
        a.ror(x86::eax, 16);
    }
    a.test(x86::eax, x86::eax);
    update_nz(a);
    a.mov(reg, x86::eax);
}

void bkpt(x86::Assembler &a, uint16_t) { a.call(ILLEGAL_OP); }

void pea(x86::Assembler &a, uint16_t op) {
    ea_getaddr_jit(a, TYPE(op), REG(op), 0);
    update_pc(a);
    a.mov(x86::eax, x86::r9d);
    jit_push32(a);
}

void ext_w(x86::Assembler &a, uint16_t op) {
    int reg = REG(op);
    a.movsx(x86::ax, DR_B(reg));
    a.test(x86::ax, x86::ax);
    update_nz(a);
    clear_vc(a);
    a.mov(DR_W(reg), x86::ax);
}

void ext_l(x86::Assembler &a, uint16_t op) {
    int reg = REG(op);
    a.movsx(x86::eax, DR_W(reg));
    a.test(x86::eax, x86::eax);
    update_nz(a);
    clear_vc(a);
    a.mov(DR_L(reg), x86::eax);
}
void movem_w_store_decr(x86::Assembler &a, uint16_t op) {
    uint16_t regs = FETCH();
    auto lb = a.newLabel();
    const auto reg = AR_L(REG(op));
    a.mov(x86::al, x86::byte_ptr(x86::rbx, offsetof(Cpu, movem_run)));
    a.test(x86::al, x86::al);
    a.jne(lb);
    a.mov(x86::edx, reg);
    a.lea(x86::rdx, x86::ptr(x86::rdx, -2));
    a.mov(reg, x86::edx);
    a.mov(x86::dword_ptr(x86::rbx, offsetof(Cpu, EA)), x86::edx);
    a.inc(x86::byte_ptr(x86::rbx, offsetof(Cpu, movem_run)));
    a.bind(lb);
    a.mov(x86::r12d, x86::dword_ptr(x86::rbx, offsetof(Cpu, EA)));
    for(int i = 0; i < 16; ++i) {
        if(regs & 1 << i) {
            a.mov(ARG1, x86::r12d);
            a.mov(ARG2, DR_L(15 - i));
            a.call(WriteW);
            a.lea(x86::r12, x86::ptr(x86::r12, -2));
        }
    }
    a.lea(x86::r12, x86::ptr(x86::r12, 2));
    a.mov(reg, x86::r12);
    a.dec(x86::byte_ptr(x86::rbx, offsetof(Cpu, movem_run)));
}
void movem_w_store_base(x86::Assembler &a, uint16_t op) {
    uint16_t regs = FETCH();
    auto lb = a.newLabel();
    a.mov(x86::al, x86::byte_ptr(x86::rbx, offsetof(Cpu, movem_run)));
    a.test(x86::al, x86::al);
    a.jne(lb);
    ea_getaddr_jit(a, TYPE(op), REG(op), 2);
    a.inc(x86::byte_ptr(x86::rbx, offsetof(Cpu, movem_run)));
    a.bind(lb);
    a.mov(x86::r12d, x86::dword_ptr(x86::rbx, offsetof(Cpu, EA)));
    for(int i = 0; i < 16; ++i) {
        if(regs & 1 << i) {
            a.mov(ARG1, x86::r12d);
            a.mov(ARG2, DR_L(i));
            a.call(WriteW);
            a.lea(x86::r12, x86::ptr(x86::r12, 2));
        }
    }
    a.dec(x86::byte_ptr(x86::rbx, offsetof(Cpu, movem_run)));
}
void movem_l_store_decr(x86::Assembler &a, uint16_t op) {
    uint16_t regs = FETCH();
    auto lb = a.newLabel();
    const auto reg = AR_L(REG(op));
    a.mov(x86::al, x86::byte_ptr(x86::rbx, offsetof(Cpu, movem_run)));
    a.test(x86::al, x86::al);
    a.jne(lb);
    a.mov(x86::edx, reg);
    a.lea(x86::rdx, x86::ptr(x86::rdx, -4));
    a.mov(reg, x86::edx);
    a.mov(x86::dword_ptr(x86::rbx, offsetof(Cpu, EA)), x86::edx);
    a.inc(x86::byte_ptr(x86::rbx, offsetof(Cpu, movem_run)));
    a.bind(lb);
    a.mov(x86::r12d, x86::dword_ptr(x86::rbx, offsetof(Cpu, EA)));
    for(int i = 0; i < 16; ++i) {
        if(regs & 1 << i) {
            a.mov(ARG1, x86::r12d);
            a.mov(ARG2, DR_L(15 - i));
            a.call(WriteL);
            a.lea(x86::r12, x86::ptr(x86::r12, -4));
        }
    }
    a.lea(x86::r12, x86::ptr(x86::r12, 4));
    a.mov(reg, x86::r12);
    a.dec(x86::byte_ptr(x86::rbx, offsetof(Cpu, movem_run)));
}
void movem_l_store_base(x86::Assembler &a, uint16_t op) {
    uint16_t regs = FETCH();
    auto lb = a.newLabel();
    a.mov(x86::al, x86::byte_ptr(x86::rbx, offsetof(Cpu, movem_run)));
    a.test(x86::al, x86::al);
    a.jne(lb);
    ea_getaddr_jit(a, TYPE(op), REG(op), 4);
    a.inc(x86::byte_ptr(x86::rbx, offsetof(Cpu, movem_run)));
    a.bind(lb);
    a.mov(x86::r12d, x86::dword_ptr(x86::rbx, offsetof(Cpu, EA)));
    for(int i = 0; i < 16; ++i) {
        if(regs & 1 << i) {
            a.mov(ARG1, x86::r12d);
            a.mov(ARG2, DR_L(i));
            a.call(WriteL);
            a.lea(x86::r12, x86::ptr(x86::r12, 4));
        }
    }
    a.dec(x86::byte_ptr(x86::rbx, offsetof(Cpu, movem_run)));
}

void movem_w_load_incr(x86::Assembler &a, uint16_t op) {
    uint16_t regs = FETCH();
    auto lb = a.newLabel();
    const auto reg = AR_L(REG(op));
    a.mov(x86::al, x86::byte_ptr(x86::rbx, offsetof(Cpu, movem_run)));
    a.test(x86::al, x86::al);
    a.jne(lb);
    a.mov(x86::edx, reg);
    a.mov(x86::dword_ptr(x86::rbx, offsetof(Cpu, EA)), x86::edx);
    a.inc(x86::byte_ptr(x86::rbx, offsetof(Cpu, movem_run)));
    a.bind(lb);
    a.mov(x86::r12d, x86::dword_ptr(x86::rbx, offsetof(Cpu, EA)));
    a.mov(reg, x86::r12d);
    for(int i = 0; i < 16; ++i) {
        if(regs & 1 << i) {
            a.mov(ARG1, x86::r12d);
            a.call(ReadW);
            if(i > 7) {
                a.cwde();
                a.mov(AR_L(i & 7), x86::eax);
            } else {
                a.mov(DR_W(i), x86::ax);
            }
            a.lea(x86::r12, x86::ptr(x86::r12, 2));
            a.mov(reg, x86::r12d);
        }
    }
    a.dec(x86::byte_ptr(x86::rbx, offsetof(Cpu, movem_run)));
}
void movem_w_load_base(x86::Assembler &a, uint16_t op) {
    uint16_t regs = FETCH();
    auto lb = a.newLabel();
    a.mov(x86::al, x86::byte_ptr(x86::rbx, offsetof(Cpu, movem_run)));
    a.test(x86::al, x86::al);
    a.jne(lb);
    ea_getaddr_jit(a, TYPE(op), REG(op), 2);
    a.inc(x86::byte_ptr(x86::rbx, offsetof(Cpu, movem_run)));
    a.bind(lb);
    a.mov(x86::r12d, x86::dword_ptr(x86::rbx, offsetof(Cpu, EA)));
    for(int i = 0; i < 16; ++i) {
        if(regs & 1 << i) {
            a.mov(ARG1, x86::r12d);
            a.call(ReadW);
            if(i > 7) {
                a.cwde();
                a.mov(AR_L(i & 7), x86::eax);
            } else {
                a.mov(DR_W(i), x86::ax);
            }
            a.lea(x86::r12, x86::ptr(x86::r12, 2));
        }
    }
    a.dec(x86::byte_ptr(x86::rbx, offsetof(Cpu, movem_run)));
}
void movem_l_load_incr(x86::Assembler &a, uint16_t op) {
    uint16_t regs = FETCH();
    auto lb = a.newLabel();
    const auto reg = AR_L(REG(op));
    a.mov(x86::al, x86::byte_ptr(x86::rbx, offsetof(Cpu, movem_run)));
    a.test(x86::al, x86::al);
    a.jne(lb);
    a.mov(x86::edx, reg);
    a.mov(x86::dword_ptr(x86::rbx, offsetof(Cpu, EA)), x86::edx);
    a.inc(x86::byte_ptr(x86::rbx, offsetof(Cpu, movem_run)));
    a.bind(lb);
    a.mov(x86::r12d, x86::dword_ptr(x86::rbx, offsetof(Cpu, EA)));
    a.mov(reg, x86::r12d);
    for(int i = 0; i < 16; ++i) {
        if(regs & 1 << i) {
            a.mov(ARG1, x86::r12d);
            a.call(ReadL);
            a.mov(DR_W(i), x86::eax);
            a.lea(x86::r12, x86::ptr(x86::r12, 4));
            a.mov(reg, x86::r12d);
        }
    }
    a.dec(x86::byte_ptr(x86::rbx, offsetof(Cpu, movem_run)));
}
void movem_l_load_base(x86::Assembler &a, uint16_t op) {
    uint16_t regs = FETCH();
    auto lb = a.newLabel();
    a.mov(x86::al, x86::byte_ptr(x86::rbx, offsetof(Cpu, movem_run)));
    a.test(x86::al, x86::al);
    a.jne(lb);
    ea_getaddr_jit(a, TYPE(op), REG(op), 4);
    a.inc(x86::byte_ptr(x86::rbx, offsetof(Cpu, movem_run)));
    a.bind(lb);
    a.mov(x86::r12d, x86::dword_ptr(x86::rbx, offsetof(Cpu, EA)));
    for(int i = 0; i < 16; ++i) {
        if(regs & 1 << i) {
            a.mov(ARG1, x86::r12d);
            a.call(ReadL);
            a.mov(DR_L(i), x86::eax);
            a.lea(x86::r12, x86::ptr(x86::r12, 4));
        }
    }
    a.dec(x86::byte_ptr(x86::rbx, offsetof(Cpu, movem_run)));
}

void tst_b(x86::Assembler &a, uint16_t op) {
    ea_readB_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.test(x86::al, x86::al);
    update_nz(a);
    clear_vc(a);
}
void tst_w(x86::Assembler &a, uint16_t op) {
    ea_readW_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.test(x86::ax, x86::ax);
    update_nz(a);
    clear_vc(a);
}
void tst_l(x86::Assembler &a, uint16_t op) {
    ea_readL_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.test(x86::eax, x86::eax);
    update_nz(a);
    clear_vc(a);
}

void tas(x86::Assembler &a, uint16_t op) {
    ea_readB_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.test(x86::al, x86::al);
    update_nz(a);
    clear_vc(a);
    a.bts(x86::ax, 7);
    ea_writeB_jit(a, TYPE(op), REG(op), true);
}

void illegal(x86::Assembler &a, uint16_t) { a.call(ILLEGAL_OP); }

void mul_l(x86::Assembler &a, uint16_t op) {
    uint16_t ext = FETCH();
    int low = ext >> 12 & 7;
    bool sig = ext & 1 << 11;
    bool dbl = ext & 1 << 10;
    int high = ext & 7;
    ea_readL_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.mov(x86::edx, DR_L(low));
    a.mov(CC_C, 0);
    if(!sig) {
        // MULU.L
        a.mul(x86::edx);
        a.mov(DR_L(low), x86::eax);
        if(!dbl) {
            a.test(x86::edx, x86::edx);
            a.setne(CC_V);
        }
    } else {
        // MULS.L
        if(dbl) {
            a.imul(x86::edx);
        } else {
            a.imul(x86::eax, x86::edx);
            a.seto(CC_V);
        }
    }
    a.mov(DR_L(low), x86::eax);
    if(dbl) {
        a.mov(DR_L(high), x86::edx);
        a.mov(CC_V, 0);
        a.ror(x86::rdx, 32);
        a.or_(x86::rdx, x86::rax);
        a.test(x86::rdx, x86::rdx);
    } else {
        a.test(x86::eax, x86::eax);
    }
    update_nz(a);
}

void div_l(x86::Assembler &a, uint16_t op) {
    uint16_t ext = FETCH();
    int q = ext >> 12 & 7;
    bool sig = ext & 1 << 11;
    bool dbl = ext & 1 << 10;
    int r = ext & 7;
    auto lb = a.newLabel();
    ea_readL_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.mov(x86::ecx, x86::eax);
    a.test(x86::ecx, x86::ecx);
    a.jne(lb);
    a.call(DIV0_ERROR);
    a.bind(lb);
    a.mov(CC_C, 0);
    a.mov(x86::eax, DR_L(q));
    if(!sig) {
        // DIVU.L
        if(!dbl) {
            // unsigned 32bit div doesn't overflow
            a.mov(CC_V, 0);
            a.xor_(x86::edx, x86::edx);
            a.div(x86::ecx);
            a.mov(DR_L(q), x86::eax);
            a.test(x86::eax, x86::eax);
            update_nz(a);
            if(q != r) {
                a.mov(DR_L(r), x86::edx);
            }
        } else {
            // 64bit/32bit div may overflow, so calc 64bit
            a.mov(x86::edx, DR_L(r));
            a.ror(x86::rdx, 32);
            a.or_(x86::rax, x86::rdx);
            a.xor_(x86::rdx, x86::rdx);
            a.div(x86::rcx);
            if(q != r) {
                a.mov(DR_L(r), x86::edx);
            }
            a.mov(DR_L(q), x86::eax);
            a.test(x86::eax, x86::eax);
            update_nz(a);
            a.ror(x86::rax, 32);
            a.test(x86::eax, x86::eax);
            a.setne(CC_V);
        }
    } else {
        // DIVS.L
        if(!dbl) {
            // signed 32bit div overflow only if INT_MIN / -1
            auto lb2 = a.newLabel();
            auto lb3 = a.newLabel();
            a.cmp(x86::eax, 0x80000000);
            a.jne(lb2);
            a.cmp(x86::ecx, -1);
            a.jne(lb2);
            a.mov(CC_V, 1);
            a.jmp(lb3);
            a.bind(lb2);
            a.cdq();
            a.idiv(x86::ecx);
            a.mov(DR_L(q), x86::eax);
            a.test(x86::eax, x86::eax);
            update_nz(a);
            if(q != r) {
                a.mov(DR_L(r), x86::edx);
            }
            a.bind(lb3);
        } else {
            // 64bit/32bit div may overflow, so calc 64bit
            a.mov(x86::edx, DR_L(r));
            a.ror(x86::rdx, 32);
            a.or_(x86::rax, x86::rdx);
            a.movsx(x86::rcx, x86::ecx);
            a.cqo();
            a.idiv(x86::rcx);
            if(q != r) {
                a.mov(DR_L(r), x86::edx);
            }
            a.mov(DR_L(q), x86::eax);
            a.test(x86::eax, x86::eax);
            update_nz(a);
            a.movsxd(x86::rcx, x86::eax);
            a.cmp(x86::rcx, x86::rax);
            a.setne(CC_V);
        }
    }
}
void trap(x86::Assembler &a, uint16_t op) {
    a.mov(ARG1, op & 0xf);
    a.call(TRAP_ERROR);
}

void link_w(x86::Assembler &a, uint16_t op) {
    int16_t disp = FETCH();
    int reg = REG(op);
    update_pc(a);
    a.mov(x86::eax, AR_L(reg));
    jit_push32(a);

    a.mov(x86::eax, SP);
    a.mov(AR_L(reg), x86::eax);
    a.add(SP, disp);
}

void unlk(x86::Assembler &a, uint16_t op) {
    int reg = REG(op);
    a.mov(x86::eax, AR_L(reg));
    a.mov(SP, x86::eax);
    jit_pop32(a);
    a.mov(AR_L(reg), x86::eax);
}

void move_to_usp(x86::Assembler &a, uint16_t op) {
    jit_priv_check(a);
    a.mov(x86::eax, AR_L(REG(op)));
    a.mov(x86::dword_ptr(x86::rbx, offsetof(Cpu, USP)), x86::eax);
    jit_trace_branch(a);
}

void move_from_usp(x86::Assembler &a, uint16_t op) {
    jit_priv_check(a);
    a.mov(x86::eax, x86::dword_ptr(x86::rbx, offsetof(Cpu, USP)));
    a.mov(AR_L(REG(op)), x86::eax);
    jit_trace_branch(a);
}

void reset(x86::Assembler &a, uint16_t) {
    jit_priv_check(a);
    a.call(bus_reset);
}

void nop(x86::Assembler &a, uint16_t) { jit_trace_branch(a); }

void stop(x86::Assembler &a, uint16_t) {
    uint16_t nw = FETCH();
    a.mov(ARG1, nw);
    a.call(OP::stop_impl);
}

void rte(x86::Assembler &a, uint16_t) {
    jit_priv_check(a);
    a.call(do_rte);
    jit_trace_branch(a);
    jit_postop(a);
    a.jmp(end_lbl);
}

void rtd(x86::Assembler &a, uint16_t) {
    int16_t disp = FETCH();
    update_pc(a);
    a.mov(x86::eax, AR_L(7));
    a.lea(x86::edi, x86::ptr(x86::eax, 4));
    a.mov(AR_L(7), x86::edi);
    a.mov(ARG1, x86::eax);
    a.call(ReadL);
    a.mov(x86::edx, AR_L(7));
    a.lea(x86::edx, x86::ptr(x86::edx, disp));
    a.mov(AR_L(7), x86::edx);
    jump(a);
}

void rts(x86::Assembler &a, uint16_t) {
    a.mov(x86::esi, AR_L(7));
    a.lea(x86::rdi, x86::ptr(x86::rsi, 4));
    a.mov(AR_L(7), x86::edi);
    a.mov(ARG1, x86::esi);
    a.call(ReadL);
    jump(a);
}

void trapv(x86::Assembler &a, uint16_t) {
    auto lb = a.newLabel();
    a.test(CC_V, 1);
    a.jz(lb);
    a.call(TRAPX_ERROR);
    a.bind(lb);
}

void rtr(x86::Assembler &a, uint16_t) {
    a.mov(x86::r12d, AR_L(7));
    a.mov(ARG1, x86::r12d);
    a.call(ReadW);
    a.mov(ARG1, x86::eax);
    a.call(SetCCR);
    a.lea(x86::r12, x86::ptr(x86::r12, 2));
    a.mov(ARG1, x86::r12d);
    a.call(ReadL);
    a.lea(x86::r12, x86::ptr(x86::r12, 4));
    a.mov(AR_L(7), x86::r12d);
    jump(a);
}

void movec_from_cr(x86::Assembler &a, uint16_t) {
    uint16_t extw = FETCH();
    int rn = extw >> 12 & 15;
    auto lb = a.newLabel();
    jit_priv_check(a);
    update_pc(a);
    a.mov(ARG1, extw & 0xfff);
    a.lea(ARG2L, DR_L(rn));
    a.call(movec_from_cr_impl);
    a.test(x86::al, x86::al);
    a.jne(lb);
    a.call(ILLEGAL_OP);
    a.bind(lb);
    jit_trace_branch(a);
}

void movec_to_cr(x86::Assembler &a, uint16_t) {
    uint16_t extw = FETCH();
    int rn = extw >> 12 & 15;
    auto lb = a.newLabel();
    jit_priv_check(a);
    update_pc(a);
    a.mov(ARG1, extw & 0xfff);
    a.mov(ARG2, DR_L(rn));
    a.call(movec_to_cr_impl);
    a.test(x86::al, x86::al);
    a.jne(lb);
    a.call(ILLEGAL_OP);
    a.bind(lb);
    jit_trace_branch(a);
}
void jsr(x86::Assembler &a, uint16_t op) {
    ea_getaddr_jit(a, TYPE(op), REG(op), 0);
    a.mov(x86::r12d, x86::r9d);
    update_pc(a);
    a.mov(x86::eax, x86::dword_ptr(x86::rbx, offsetof(Cpu, PC)));
    jit_push32(a);
    a.mov(x86::eax, x86::r9d);
    jump(a);
}

void jmp(x86::Assembler &a, uint16_t op) {
    ea_getaddr_jit(a, TYPE(op), REG(op), 0);
    a.mov(x86::r12d, x86::r9d);
    update_pc(a);
    a.mov(x86::eax, x86::r9d);
    jump(a);
}

void chk_l(x86::Assembler &a, uint16_t op) {
    ea_readL_jit(a, TYPE(op), REG(op));
    a.mov(x86::edx, DR_L(DN(op)));
    auto lb = a.newLabel();
    auto lb2 = a.newLabel();
    a.cmp(x86::edx, 0);
    a.jl(lb);
    a.cmp(x86::edx, x86::eax);
    a.jle(lb2);
    a.bind(lb);
    a.cmp(x86::edx, 0);
    a.setl(CC_N);
    a.call(CHK_ERROR);
    a.bind(lb2);
}

void chk_w(x86::Assembler &a, uint16_t op) {
    ea_readW_jit(a, TYPE(op), REG(op));
    a.mov(x86::dx, DR_W(DN(op)));
    auto lb = a.newLabel();
    auto lb2 = a.newLabel();
    a.cmp(x86::dx, 0);
    a.jl(lb);
    a.cmp(x86::dx, x86::ax);
    a.jle(lb2);
    a.bind(lb);
    a.cmp(x86::dx, 0);
    a.setl(CC_N);
    a.call(CHK_ERROR);
    a.bind(lb2);
}

void extb_l(x86::Assembler &a, uint16_t op) {
    int reg = REG(op);
    a.movsx(x86::eax, DR_B(reg));
    a.test(x86::eax, x86::eax);
    update_nz(a);
    clear_vc(a);
    a.mov(DR_L(reg), x86::eax);
}

void lea(x86::Assembler &a, uint16_t op) {
    ea_getaddr_jit(a, TYPE(op), REG(op), 0);
    a.mov(AR_L(DN(op)), x86::r9d);
}

void addq_b(x86::Assembler &a, uint16_t op) {
    int d = DN(op);
    if(d == 0) {
        d = 8;
    }
    ea_readB_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.add(x86::al, d);
    update_nz(a);
    update_vcx(a);
    ea_writeB_jit(a, TYPE(op), REG(op), true);
}

void addq_w(x86::Assembler &a, uint16_t op) {
    int d = DN(op);
    if(d == 0) {
        d = 8;
    }
    ea_readW_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.add(x86::ax, d);
    update_nz(a);
    update_vcx(a);
    ea_writeW_jit(a, TYPE(op), REG(op), true);
}

void addq_l(x86::Assembler &a, uint16_t op) {
    int d = DN(op);
    if(d == 0) {
        d = 8;
    }
    ea_readL_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.add(x86::eax, d);
    update_nz(a);
    update_vcx(a);
    ea_writeL_jit(a, TYPE(op), REG(op), true);
}

void subq_b(x86::Assembler &a, uint16_t op) {
    int d = DN(op);
    if(d == 0) {
        d = 8;
    }
    ea_readB_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.sub(x86::al, d);
    update_nz(a);
    update_vcx(a);
    ea_writeB_jit(a, TYPE(op), REG(op), true);
}

void subq_w(x86::Assembler &a, uint16_t op) {
    int d = DN(op);
    if(d == 0) {
        d = 8;
    }
    ea_readW_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.sub(x86::ax, d);
    update_nz(a);
    update_vcx(a);
    ea_writeW_jit(a, TYPE(op), REG(op), true);
}

void subq_l(x86::Assembler &a, uint16_t op) {
    int d = DN(op);
    if(d == 0) {
        d = 8;
    }
    ea_readL_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.sub(x86::eax, d);
    update_nz(a);
    update_vcx(a);
    ea_writeL_jit(a, TYPE(op), REG(op), true);
}

void addq_an(x86::Assembler &a, uint16_t op) {
    int d = DN(op);
    if(d == 0) {
        d = 8;
    }
    update_pc(a);
    a.add(AR_L(REG(op)), d);
}

void subq_an(x86::Assembler &a, uint16_t op) {
    int d = DN(op);
    if(d == 0) {
        d = 8;
    }
    update_pc(a);
    a.sub(AR_L(REG(op)), d);
}

void jit_testcc(x86::Assembler &a, int cc) {
    switch(cc) {
    case 0: // T
        a.mov(x86::al, 1);
        return;
    case 1: // F
        a.xor_(x86::al, x86::al);
        return;
    case 2: // HI
        a.mov(x86::al, CC_C);
        a.or_(x86::al, CC_Z);
        a.setz(x86::al);
        return;
    case 3: // LS
        a.mov(x86::al, CC_C);
        a.or_(x86::al, CC_Z);
        return;
    case 4: // CC
        a.mov(x86::al, CC_C);
        a.test(x86::al, x86::al);
        a.setz(x86::al);
        return;
    case 5: // CS
        a.mov(x86::al, CC_C);
        return;
    case 6: // NE
        a.mov(x86::al, CC_Z);
        a.test(x86::al, x86::al);
        a.sete(x86::al);
        return;
    case 7: // EQ
        a.mov(x86::al, CC_Z);
        return;
    case 8: // VC
        a.mov(x86::al, CC_V);
        a.test(x86::al, x86::al);
        a.sete(x86::al);
        return;
    case 9: // VS
        a.mov(x86::al, CC_V);
        return;
    case 10: // PL
        a.mov(x86::al, CC_N);
        a.test(x86::al, x86::al);
        a.sete(x86::al);
        return;
    case 11: // MI
        a.mov(x86::al, CC_N);
        return;
    case 12: // GE
        a.mov(x86::al, CC_N);
        a.xor_(x86::al, CC_V);
        a.sete(x86::al);
        return;
    case 13: // LT
        a.mov(x86::al, CC_N);
        a.xor_(x86::al, CC_V);
        return;
    case 14: // GT
        a.mov(x86::al, CC_N);
        a.xor_(x86::al, CC_V);
        a.or_(x86::al, CC_Z);
        a.sete(x86::al);
        return;
    case 15: // LE
        a.mov(x86::al, CC_N);
        a.xor_(x86::al, CC_V);
        a.or_(x86::al, CC_Z);
        return;
    default:
        __builtin_unreachable();
    }
}

void scc_ea(x86::Assembler &a, uint16_t op) {
    int cc = op >> 8 & 0xf;
    jit_testcc(a, cc);
    a.neg(x86::al);
    a.sbb(x86::dl, x86::dl);
    a.mov(x86::al, x86::dl);
    ea_writeB_jit(a, TYPE(op), REG(op), false);
}

void dbcc(x86::Assembler &a, uint16_t op) {
    int cc = op >> 8 & 0xf;
    int16_t disp = FETCH();
    int reg = REG(op);
    jit_testcc(a, cc);
    auto lb = a.newLabel();
    a.test(x86::al, x86::al);
    a.jne(lb);
    a.mov(x86::cx, DR_W(reg));
    a.dec(x86::cx);
    a.mov(DR_W(reg), x86::cx);
    a.cmp(x86::cx, -1);
    a.je(lb);
    jumpC(a, cpu.PC + disp - 2);
    a.bind(lb);
}

void trapcc(x86::Assembler &a, uint16_t op) {
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
    auto lb = a.newLabel();
    update_pc(a);
    jit_testcc(a, cc);
    a.test(x86::al, x86::al);
    a.je(lb);
    a.call(TRAPX_ERROR);
    a.bind(lb);
}

void bxx(x86::Assembler &a, uint16_t op) {
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
    auto lb = a.newLabel();
    if(cc == 0) {
        // BRA
    } else if(cc == 1) {
        // BSR
        update_pc(a);
        a.mov(x86::eax, x86::dword_ptr(x86::rbx, offsetof(Cpu, PC)));
        jit_push32(a);
    } else {
        // Bcc
        jit_testcc(a, cc);
        a.test(x86::al, x86::al);
        a.je(lb);
    }
    if(jumpMap.contains(target)) {
        jumpC(a, target);
    } else {
        a.mov(x86::eax, target);
        jump(a);
    }
    a.bind(lb);
}

void moveq(x86::Assembler &a, uint16_t op) {
    clear_vc(a);
    a.mov(x86::al, op & 0xff);
    a.test(x86::al, x86::al);
    update_nz(a);
    a.movsx(x86::eax, x86::al);
    a.mov(DR_L(DN(op)), x86::eax);
}

void or_b_to_dn(x86::Assembler &a, uint16_t op) {
    clear_vc(a);
    ea_readB_jit(a, TYPE(op), REG(op));
    a.mov(x86::dl, DR_B(DN(op)));
    a.or_(x86::al, x86::dl);
    update_nz(a);
    a.mov(DR_B(DN(op)), x86::al);
}

void or_w_to_dn(x86::Assembler &a, uint16_t op) {
    clear_vc(a);
    ea_readW_jit(a, TYPE(op), REG(op));
    a.mov(x86::dx, DR_W(DN(op)));
    a.or_(x86::ax, x86::dx);
    update_nz(a);
    a.mov(DR_W(DN(op)), x86::ax);
}

void or_l_to_dn(x86::Assembler &a, uint16_t op) {
    clear_vc(a);
    ea_readL_jit(a, TYPE(op), REG(op));
    a.mov(x86::edx, DR_L(DN(op)));
    a.or_(x86::eax, x86::edx);
    update_nz(a);
    a.mov(DR_L(DN(op)), x86::eax);
}

void divu_w(x86::Assembler &a, uint16_t op) {
    auto lb = a.newLabel();
    ea_readW_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.test(x86::ax, x86::ax);
    a.jne(lb);
    a.call(DIV0_ERROR);
    a.bind(lb);
    a.cwde();
    a.mov(CC_C, 0);
    a.mov(x86::ecx, DR_L(DN(op)));
    a.xchg(x86::ecx, x86::eax);
    a.xor_(x86::edx, x86::edx);
    a.div(x86::ecx);
    a.mov(x86::ecx, x86::eax);
    a.shr(x86::ecx, 16);
    a.test(x86::cx, x86::cx);
    a.setne(CC_V);
    a.test(x86::ax, x86::ax);
    update_nz(a);
    a.movzx(x86::eax, x86::ax);
    a.ror(x86::edx, 16);
    a.or_(x86::eax, x86::edx);
    a.mov(DR_L(DN(op)), x86::eax);
}

void sbcd_d(x86::Assembler &a, uint16_t op) {
    int reg = REG(op);
    a.movzx(ARG1, DR_B(reg));
    a.movzx(ARG2, DR_B(DN(op)));
    a.movzx(ARG3, CC_X);
    a.call(do_sbcd);
    a.mov(DR_B(reg), x86::al);
}

void sbcd_m(x86::Assembler &a, uint16_t op) {
    int reg = REG(op);
    a.mov(x86::r12d, AR_L(reg));
    a.dec(x86::r12d);
    a.mov(AR_L(reg), x86::r12d);
    a.mov(ARG1, x86::r12d);
    a.call(ReadB);
    a.mov(x86::r11d, x86::eax);

    a.mov(x86::edi, AR_L(DN(op)));
    a.dec(x86::edi);
    a.mov(AR_L(DN(op)), x86::edi);
    a.mov(ARG1, x86::edi);
    a.call(ReadB);

    a.mov(ARG1, x86::r11d);
    a.mov(ARG2, x86::eax);
    a.movzx(ARG3, CC_X);
    a.call(do_sbcd);
    a.mov(ARG1, x86::r12d);
    a.movzx(ARG2, x86::al);
    a.call(WriteB);
}

void or_b_to_ea(x86::Assembler &a, uint16_t op) {
    clear_vc(a);
    ea_readB_jit(a, TYPE(op), REG(op));
    a.mov(x86::dl, DR_B(DN(op)));
    a.or_(x86::al, x86::dl);
    update_nz(a);
    ea_writeB_jit(a, TYPE(op), REG(op), true);
}

void pack_d(x86::Assembler &a, uint16_t op) {
    int reg = REG(op);
    uint16_t adj = FETCH();
    update_pc(a);

    a.movzx(ARG1, DR_W(reg));
    a.mov(ARG2, adj);
    a.call(do_pack);
    a.mov(DR_B(DN(op)), x86::al);
}

void pack_m(x86::Assembler &a, uint16_t op) {
    int reg = REG(op);
    uint16_t adj = FETCH();
    update_pc(a);

    a.mov(x86::r12d, AR_L(reg));
    a.dec(x86::r12d);
    a.mov(AR_L(reg), x86::r12d);
    a.mov(ARG1, x86::r12d);
    a.call(ReadB);
    a.movzx(x86::r11d, x86::al);

    a.dec(x86::r12d);
    a.mov(AR_L(reg), x86::r12d);
    a.mov(ARG1, x86::r12d);
    a.call(ReadB);
    a.movzx(x86::eax, x86::al);
    a.xchg(x86::al, x86::ah);
    a.or_(x86::ax, x86::r11w);
    a.movzx(ARG1, x86::ax);
    a.mov(ARG2, adj);
    a.call(do_pack);
    
    a.mov(x86::edi, AR_L(DN(op)));
    a.dec(x86::edi);
    a.mov(AR_L(DN(op)), x86::edi);
    a.mov(ARG1, x86::edi);
    a.movzx(ARG2, x86::al);
    a.call(WriteB);

}

void or_w_to_ea(x86::Assembler &a, uint16_t op) {
    clear_vc(a);
    ea_readW_jit(a, TYPE(op), REG(op));
    a.mov(x86::dx, DR_W(DN(op)));
    a.or_(x86::ax, x86::dx);
    update_nz(a);
    ea_writeW_jit(a, TYPE(op), REG(op), true);
}


void unpk_d(x86::Assembler &a, uint16_t op) {
    int reg = REG(op);
    uint16_t adj = FETCH();
    update_pc(a);

    a.movzx(ARG1, DR_L(reg));
    a.mov(ARG2, adj);
    a.call(do_unpk);
    a.mov(DR_W(DN(op)), x86::ax);
}

void unpk_m(x86::Assembler &a, uint16_t op) {
    int reg = REG(op);
    int dn = DN(op);
    uint16_t adj = FETCH();
    update_pc(a);

    a.mov(x86::esi, AR_L(reg));
    a.dec(x86::esi);
    a.mov(AR_L(reg), x86::esi);
    a.mov(ARG1, x86::esi);
    a.call(ReadB);
    a.movzx(ARG1, x86::al);
    a.mov(ARG2, adj);
    a.call(do_unpk);
    a.movzx(x86::r13d, x86::ax);


    a.mov(x86::r12d, AR_L(dn));
    a.dec(x86::r12d);
    a.mov(AR_L(dn), x86::r12d);    
    a.mov(ARG1, x86::r12d);
    a.movzx(ARG2, x86::r13b);
    a.call(WriteB);

    a.dec(x86::r12d);
    a.mov(AR_L(dn), x86::r12d);    
    a.mov(ARG1, x86::r12d);
    a.shr(x86::r13w, 8);
    a.movzx(ARG2, x86::r13b);
    a.call(WriteB);

}

void or_l_to_ea(x86::Assembler &a, uint16_t op) {
    clear_vc(a);
    ea_readL_jit(a, TYPE(op), REG(op));
    a.mov(x86::edx, DR_L(DN(op)));
    a.or_(x86::eax, x86::edx);
    update_nz(a);
    ea_writeL_jit(a, TYPE(op), REG(op), true);
}
void divs_w(x86::Assembler &a, uint16_t op) {
    auto lb = a.newLabel();
    auto lb2 = a.newLabel();
    auto lb3 = a.newLabel();
    ea_readW_jit(a, TYPE(op), REG(op));
    update_pc(a);
    a.test(x86::ax, x86::ax);
    a.jne(lb);
    a.call(DIV0_ERROR);
    a.bind(lb);
    a.cwde();
    a.mov(CC_C, 0);
    a.mov(x86::ecx, DR_L(DN(op)));
    a.xchg(x86::ecx, x86::eax);
    a.cmp(x86::eax, 0x80000000);
    a.jne(lb3);
    a.cmp(x86::ecx, -1);
    a.jne(lb3);
    a.mov(CC_V, 1);
    a.jmp(lb2);
    a.bind(lb3);
    a.cdq();
    a.idiv(x86::ecx);
    a.mov(x86::ecx, x86::eax);
    a.shr(x86::ecx, 16);
    a.test(x86::cx, x86::cx);
    a.setne(CC_V);
    a.test(x86::ax, x86::ax);
    update_nz(a);
    a.movzx(x86::eax, x86::ax);
    a.shl(x86::edx, 16);
    a.or_(x86::eax, x86::edx);
    a.mov(DR_L(DN(op)), x86::eax);
    a.bind(lb2);
}

void sub_b_to_dn(x86::Assembler &a, uint16_t op) {
    ea_readB_jit(a, TYPE(op), REG(op));
    a.mov(x86::dl, DR_B(DN(op)));
    a.sub(x86::dl, x86::al);
    a.xchg(x86::dl, x86::al);
    update_nz(a);
    update_vcx(a);
    a.mov(DR_B(DN(op)), x86::al);
}

void sub_w_to_dn(x86::Assembler &a, uint16_t op) {
    ea_readW_jit(a, TYPE(op), REG(op));
    a.mov(x86::dx, DR_W(DN(op)));
    a.sub(x86::dx, x86::ax);
    a.xchg(x86::dx, x86::ax);
    update_nz(a);
    update_vcx(a);
    a.mov(DR_W(DN(op)), x86::ax);
}

void sub_l_to_dn(x86::Assembler &a, uint16_t op) {
    ea_readL_jit(a, TYPE(op), REG(op));
    a.mov(x86::edx, DR_L(DN(op)));
    a.sub(x86::edx, x86::eax);
    a.xchg(x86::edx, x86::eax);
    update_nz(a);
    update_vcx(a);
    a.mov(DR_L(DN(op)), x86::eax);
}

void suba_w(x86::Assembler &a, uint16_t op) {
    ea_readW_jit(a, TYPE(op), REG(op));
    a.cwde();
    a.sub(AR_L(DN(op)), x86::eax);
}

void suba_l(x86::Assembler &a, uint16_t op) {
    ea_readL_jit(a, TYPE(op), REG(op));
    a.sub(AR_L(DN(op)), x86::eax);
}

void subx_b_d(x86::Assembler &a, uint16_t op) {
    a.mov(x86::al, DR_B(REG(op)));
    a.mov(x86::dl, DR_B(DN(op)));
    a.mov(x86::cl, CC_X);
    a.shr(x86::cl, 1);
    a.sbb(x86::al, x86::dl);
    update_vcx(a);
    a.sets(CC_N);
    a.setz(x86::sil);
    a.and_(CC_Z, x86::sil);
    a.mov(DR_B(REG(op)), x86::al);    
}

void subx_b_m(x86::Assembler &a, uint16_t op) {
    a.mov(x86::r12d, AR_L(REG(op)));
    a.dec(x86::r12d);
    a.mov(AR_L(REG(op)), x86::r12d);
    a.mov(ARG1, x86::r12d);
    a.call(ReadB);
    a.mov(x86::r13b, x86::al);
    a.mov(ARG1, AR_L(REG(op)));
    a.dec(ARG1);
    a.mov(AR_L(REG(op)), ARG1);
    a.call(ReadB);
    a.mov(x86::cl, CC_X);
    a.shr(x86::cl, 1);
    a.sbb(x86::r13b, x86::al);
    update_vcx(a);
    a.sets(CC_N);
    a.setz(x86::sil);
    a.and_(CC_Z, x86::sil);
    a.mov(ARG1, x86::r12d);
    a.mov(ARG2, x86::r13b);
    a.call(WriteB);
}


#endif
} // namespace JIT_OP
