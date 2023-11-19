#include "jit.hpp"
#include "68040.hpp"
#include "exception.hpp"
#include "jit_proto.hpp"
#include "memory.hpp"
#include "inline.hpp"
#include <memory>
#include <unordered_map>
#include <utility>
using namespace asmjit;
std::unique_ptr<JitRuntime> rt;

class MyErrorHandler : public ErrorHandler {
  public:
    void handleError(Error, const char *message, BaseEmitter *) override {
        printf("AsmJit error: %s\n", message);
    }
};
std::unordered_map<uint32_t, std::shared_ptr<jit_cache>> jit_tables;
using Func = void (*)(uint32_t base);
using jit_op = void (*)(x86::Assembler &a, uint16_t op);
static jit_op jit_doop[0x10000];
CpuFeatures feature;

void movem_w_store_decr_impl(uint16_t regs, int reg);
void movem_w_store_base_impl(uint16_t regs, int type, int reg);
namespace JIT_OP {
extern Label end_lbl;

} // namespace JIT_OP
void jit_init() {
    rt = std::make_unique<JitRuntime>();

    // Comon
    for(int ea = 0; ea < 075; ++ea) {
        jit_doop[0000000 | ea] = JIT_OP::ori_b;
        jit_doop[0000100 | ea] = JIT_OP::ori_w;
        jit_doop[0000200 | ea] = JIT_OP::ori_l;
        jit_doop[0001000 | ea] = JIT_OP::andi_b;
        jit_doop[0001100 | ea] = JIT_OP::andi_w;
        jit_doop[0001200 | ea] = JIT_OP::andi_l;
        jit_doop[0002000 | ea] = JIT_OP::subi_b;
        jit_doop[0002100 | ea] = JIT_OP::subi_w;
        jit_doop[0002200 | ea] = JIT_OP::subi_l;
        jit_doop[0003000 | ea] = JIT_OP::addi_b;
        jit_doop[0003100 | ea] = JIT_OP::addi_w;
        jit_doop[0003200 | ea] = JIT_OP::addi_l;
        jit_doop[0005000 | ea] = JIT_OP::eori_b;
        jit_doop[0005100 | ea] = JIT_OP::eori_w;
        jit_doop[0005200 | ea] = JIT_OP::eori_l;
        jit_doop[0006000 | ea] = JIT_OP::cmpi_b;
        jit_doop[0006100 | ea] = JIT_OP::cmpi_w;
        jit_doop[0006200 | ea] = JIT_OP::cmpi_l;

        jit_doop[0040000 | ea] = JIT_OP::negx_b;
        jit_doop[0040100 | ea] = JIT_OP::negx_w;
        jit_doop[0040200 | ea] = JIT_OP::negx_l;
        jit_doop[0040300 | ea] = JIT_OP::move_from_sr;
        jit_doop[0041000 | ea] = JIT_OP::clr_b;
        jit_doop[0041100 | ea] = JIT_OP::clr_w;
        jit_doop[0041200 | ea] = JIT_OP::clr_l;
        jit_doop[0041300 | ea] = JIT_OP::move_from_ccr;
        jit_doop[0042000 | ea] = JIT_OP::neg_b;
        jit_doop[0042100 | ea] = JIT_OP::neg_w;
        jit_doop[0042200 | ea] = JIT_OP::neg_l;
        jit_doop[0042300 | ea] = JIT_OP::move_to_ccr;
        jit_doop[0043000 | ea] = JIT_OP::not_b;
        jit_doop[0043100 | ea] = JIT_OP::not_w;
        jit_doop[0043200 | ea] = JIT_OP::not_l;
        jit_doop[0043300 | ea] = JIT_OP::move_to_sr;

        jit_doop[0044000 | ea] = JIT_OP::nbcd;
        jit_doop[0045000 | ea] = JIT_OP::tst_b;
        jit_doop[0045100 | ea] = JIT_OP::tst_w;
        jit_doop[0045200 | ea] = JIT_OP::tst_l;
        jit_doop[0045300 | ea] = JIT_OP::tas;

        jit_doop[0046000 | ea] = JIT_OP::mul_l;
        jit_doop[0046100 | ea] = JIT_OP::div_l;

        for(int d = 0; d < 8; ++d) {
            jit_doop[0010000 | d << 9 | ea] = JIT_OP::move_b;
            jit_doop[0020000 | d << 9 | ea] = JIT_OP::move_l;
            jit_doop[0030000 | d << 9 | ea] = JIT_OP::move_w;
            jit_doop[0020100 | d << 9 | ea] = JIT_OP::movea_l;
            jit_doop[0030100 | d << 9 | ea] = JIT_OP::movea_w;
            jit_doop[0040400 | d << 9 | ea] = JIT_OP::chk_l;
            jit_doop[0040600 | d << 9 | ea] = JIT_OP::chk_l;
            jit_doop[0050000 | d << 9 | ea] = JIT_OP::addq_b;
            jit_doop[0050100 | d << 9 | ea] = JIT_OP::addq_w;
            jit_doop[0050200 | d << 9 | ea] = JIT_OP::addq_l;
            jit_doop[0050400 | d << 9 | ea] = JIT_OP::subq_b;
            jit_doop[0050500 | d << 9 | ea] = JIT_OP::subq_w;
            jit_doop[0050600 | d << 9 | ea] = JIT_OP::subq_l;

            jit_doop[0100000 | d << 9 | ea] = JIT_OP::or_b_to_dn;
            jit_doop[0100100 | d << 9 | ea] = JIT_OP::or_w_to_dn;
            jit_doop[0100200 | d << 9 | ea] = JIT_OP::or_l_to_dn;
            jit_doop[0100300 | d << 9 | ea] = JIT_OP::divu_w;
            jit_doop[0100700 | d << 9 | ea] = JIT_OP::divs_w;

            jit_doop[0110000 | d << 9 | ea] = JIT_OP::sub_b_to_dn;
            jit_doop[0110100 | d << 9 | ea] = JIT_OP::sub_w_to_dn;
            jit_doop[0110200 | d << 9 | ea] = JIT_OP::sub_l_to_dn;
            jit_doop[0110300 | d << 9 | ea] = JIT_OP::suba_w;
            jit_doop[0110700 | d << 9 | ea] = JIT_OP::suba_l;

            for(int k = 2; k < 8; ++k) {
                jit_doop[0010000 | d << 9 | k << 6 | ea] = JIT_OP::move_b;
                jit_doop[0020000 | d << 9 | k << 6 | ea] = JIT_OP::move_l;
                jit_doop[0030000 | d << 9 | k << 6 | ea] = JIT_OP::move_w;
            }
        }
    }
    // EA = Dn
    for(int i = 0; i < 8; ++i) {
        jit_doop[0004000 | i] = JIT_OP::btst_l_i;
        jit_doop[0004100 | i] = JIT_OP::bchg_l_i;
        jit_doop[0004200 | i] = JIT_OP::bclr_l_i;
        jit_doop[0004300 | i] = JIT_OP::bset_l_i;

        jit_doop[0044100 | i] = JIT_OP::swap;
        jit_doop[0044200 | i] = JIT_OP::ext_w;
        jit_doop[0044300 | i] = JIT_OP::ext_l;
        jit_doop[0044700 | i] = JIT_OP::extb_l;

        jit_doop[0047100 | i] = JIT_OP::trap;

        for(int d = 0; d < 8; ++d) {
            jit_doop[0000400 | d << 9 | i] = JIT_OP::btst_l_r;
            jit_doop[0000500 | d << 9 | i] = JIT_OP::bchg_l_r;
            jit_doop[0000600 | d << 9 | i] = JIT_OP::bclr_l_r;
            jit_doop[0000700 | d << 9 | i] = JIT_OP::bset_l_r;

            jit_doop[0100400 | d << 9 | i] = JIT_OP::sbcd_d;
            jit_doop[0100500 | d << 9 | i] = JIT_OP::pack_d;
            jit_doop[0100600 | d << 9 | i] = JIT_OP::unpk_d;
            jit_doop[0110400 | d << 9 | i] = JIT_OP::subx_b_d;
        }

        for(int c = 0; c < 16; ++c) {
            jit_doop[0050300 | c << 8 | i] = JIT_OP::scc_ea;
        }
    }
    // An
    for(int i = 0; i < 8; ++i) {
        for(int d = 0; d < 8; ++d) {
            jit_doop[0000410 | d << 9 | i] = JIT_OP::movep_w_load;
            jit_doop[0000510 | d << 9 | i] = JIT_OP::movep_l_load;
            jit_doop[0000610 | d << 9 | i] = JIT_OP::movep_w_store;
            jit_doop[0000710 | d << 9 | i] = JIT_OP::movep_l_store;

            jit_doop[0050110 | d << 9 | i] = JIT_OP::addq_an;
            jit_doop[0050210 | d << 9 | i] = JIT_OP::addq_an;
            jit_doop[0050510 | d << 9 | i] = JIT_OP::subq_an;
            jit_doop[0050610 | d << 9 | i] = JIT_OP::subq_an;

            jit_doop[0100410 | d << 9 | i] = JIT_OP::sbcd_m;
            jit_doop[0100510 | d << 9 | i] = JIT_OP::pack_m;
            jit_doop[0100610 | d << 9 | i] = JIT_OP::unpk_m;
        }
        jit_doop[0044010 | i] = JIT_OP::link_l;
        jit_doop[0044110 | i] = JIT_OP::bkpt;
        jit_doop[0047110 | i] = JIT_OP::trap;
        for(int c = 0; c < 16; ++c) {
            jit_doop[0050310 | c << 8 | i] = JIT_OP::dbcc;
        }
    }
    for(int ea = 020; ea < 074; ++ea) {
        if(ea >= 030 && ea < 040) {
            // Increment only
            jit_doop[0046200 | ea] = JIT_OP::movem_w_load_incr;
            jit_doop[0046300 | ea] = JIT_OP::movem_l_load_incr;
        }
        if(ea >= 040 && ea < 050) {
            //  Decrement only
            jit_doop[0044200 | ea] = JIT_OP::movem_w_store_decr;
            jit_doop[0044300 | ea] = JIT_OP::movem_l_store_decr;
        }
        if(ea < 030 || ea >= 050) {
            // Ctl Only
            jit_doop[0000300 | ea] = JIT_OP::cmp2_chk2_b;
            jit_doop[0001300 | ea] = JIT_OP::cmp2_chk2_w;
            jit_doop[0002300 | ea] = JIT_OP::cmp2_chk2_l;
            jit_doop[0044100 | ea] = JIT_OP::pea;
            jit_doop[0044200 | ea] = JIT_OP::movem_w_store_base;
            jit_doop[0044300 | ea] = JIT_OP::movem_l_store_base;
            jit_doop[0046200 | ea] = JIT_OP::movem_w_load_base;
            jit_doop[0046300 | ea] = JIT_OP::movem_l_load_base;

            jit_doop[0047200 | ea] = JIT_OP::jsr;
            jit_doop[0047300 | ea] = JIT_OP::jmp;

            for(int d = 0; d < 8; ++d) {
                jit_doop[0040700 | d << 9 | ea] = JIT_OP::lea;
            }
        }

        jit_doop[0004000 | ea] = JIT_OP::btst_b_i;
        jit_doop[0004100 | ea] = JIT_OP::bchg_b_i;
        jit_doop[0004200 | ea] = JIT_OP::bclr_b_i;
        jit_doop[0004300 | ea] = JIT_OP::bset_b_i;
        jit_doop[0005300 | ea] = JIT_OP::cas_b;
        jit_doop[0006300 | ea] = JIT_OP::cas_w;
        jit_doop[0007000 | ea] = JIT_OP::moves_b;
        jit_doop[0007100 | ea] = JIT_OP::moves_w;
        jit_doop[0007200 | ea] = JIT_OP::moves_l;
        jit_doop[0007300 | ea] = JIT_OP::cas_l;
        for(int d = 0; d < 8; ++d) {
            jit_doop[0000400 | d << 9 | ea] = JIT_OP::btst_b_r;
            jit_doop[0000500 | d << 9 | ea] = JIT_OP::bchg_b_r;
            jit_doop[0000600 | d << 9 | ea] = JIT_OP::bclr_b_r;
            jit_doop[0000700 | d << 9 | ea] = JIT_OP::bset_b_r;

            jit_doop[0100400 | d << 9 | ea] = JIT_OP::or_b_to_ea;
            jit_doop[0100500 | d << 9 | ea] = JIT_OP::or_w_to_ea;
            jit_doop[0100600 | d << 9 | ea] = JIT_OP::or_l_to_ea;
        }

        for(int c = 0; c < 16; ++c) {
            if(ea < 072) {
                jit_doop[0050300 | c << 8 | ea] = JIT_OP::scc_ea;
            }
        }
    }
    jit_doop[0000074] = JIT_OP::ori_b_ccr;
    jit_doop[0000174] = JIT_OP::ori_w_sr;
    jit_doop[0001074] = JIT_OP::andi_b_ccr;
    jit_doop[0001174] = JIT_OP::andi_w_sr;
    jit_doop[0004074] = JIT_OP::btst_b_i_imm;
    jit_doop[0005074] = JIT_OP::eori_b_ccr;
    jit_doop[0005174] = JIT_OP::eori_w_sr;
    jit_doop[0006374] = JIT_OP::cas2_w;
    jit_doop[0007374] = JIT_OP::cas2_l;
    jit_doop[0042374] = JIT_OP::move_to_ccr;
    jit_doop[0043374] = JIT_OP::move_to_sr;
    jit_doop[0045374] = JIT_OP::illegal;
    for(int d = 0; d < 8; ++d) {
        jit_doop[0000474 | d << 9] = JIT_OP::btst_b_r_imm;
        jit_doop[0020074 | d << 9] = JIT_OP::move_b;
        jit_doop[0020074 | d << 9] = JIT_OP::move_l;
        jit_doop[0030074 | d << 9] = JIT_OP::move_w;
        for(int k = 2; k < 8; ++k) {
            jit_doop[0020074 | d << 9 | k << 6] = JIT_OP::move_b;
            jit_doop[0020074 | d << 9 | k << 6] = JIT_OP::move_l;
            jit_doop[0030074 | d << 9 | k << 6] = JIT_OP::move_w;
        }
    }
    for(int d = 0; d < 8; ++d) {
        jit_doop[0047120 | d] = JIT_OP::link_w;
        jit_doop[0047130 | d] = JIT_OP::unlk;
        jit_doop[0047140 | d] = JIT_OP::move_to_usp;
        jit_doop[0047150 | d] = JIT_OP::move_from_usp;

        for(int i = 0; i < 0x100; ++i) {
            jit_doop[0070000 | d << 9 | i] = JIT_OP::moveq;
        }
    }
    jit_doop[0047160] = JIT_OP::reset;
    jit_doop[0047161] = JIT_OP::nop;
    jit_doop[0047162] = JIT_OP::stop;
    jit_doop[0047163] = JIT_OP::rte;
    jit_doop[0047164] = JIT_OP::rtd;
    jit_doop[0047165] = JIT_OP::rts;
    jit_doop[0047166] = JIT_OP::trapv;
    jit_doop[0047167] = JIT_OP::rtr;

    jit_doop[0047172] = JIT_OP::movec_from_cr;
    jit_doop[0047173] = JIT_OP::movec_to_cr;

    for(int c = 0; c < 16; ++c) {
        jit_doop[0050372 | c << 8] = JIT_OP::trapcc;
        jit_doop[0050373 | c << 8] = JIT_OP::trapcc;
        jit_doop[0050374 | c << 8] = JIT_OP::trapcc;
    }
    for(int v = 0; v < 0x1000; ++v) {
        jit_doop[0060000 | v] = JIT_OP::bxx;
        //        jit_doop[0120000 | v] = OP::aline_ex;
        //        jit_doop[0170000 | v] = OP::fline_default;
    }
#ifdef CI
    jit_doop[0044117] = JIT_OP::jit_exit;
#endif
}
void op_prologue(uint32_t pc);

void jit_jumptable(x86::Assembler &a, jit_cache &jit) {
    // jump to PC
    a.mov(x86::eax, ARG1);
    a.mov(x86::rdx, jit.offset.data());
    a.movzx(x86::rdx, x86::word_ptr(x86::rdx, x86::rax, 1));
    a.lea(x86::rax, x86::ptr(x86::rip));
    int bd = a.offset();
    a.lea(x86::rax, x86::ptr(x86::rax, x86::rdx, 0, -bd));
    a.jmp(x86::rax);
    int ep = a.offset();
    std::fill(jit.offset.begin(), jit.offset.end(), ep);
}
void jit_postop(x86::Assembler &a) {
    Label lb = a.newLabel();
    a.mov(x86::al, x86::byte_ptr(x86::rbx, offsetof(Cpu, must_trace)));
    a.mov(x86::byte_ptr(x86::rbx, offsetof(Cpu, must_trace)), 0);
    a.test(x86::al, 1);
    a.je(lb);
    a.call(TRACE);
    a.bind(lb);
}
using Func = void (*)(uint32_t addr);
void AddJitFunction(jit_cache *jit, FuncFrame &frame, Func fn, size_t fn_size);
std::unordered_map<uint32_t, Label> jumpMap;
void jit_compile(uint32_t base, uint32_t len) {
    MyErrorHandler myErrorHandler;
    CodeHolder code;
    feature = rt->cpuFeatures();
    code.init(rt->environment(), feature);
    code.setErrorHandler(&myErrorHandler);

    FuncDetail detail;
    detail.init(FuncSignatureT<void, uint32_t>(), rt->environment());

    FuncFrame frame;
    frame.init(detail);
    frame.addAttributes(FuncAttributes::kHasFuncCalls);
    frame.addDirtyRegs(x86::rax, x86::rbx, x86::rcx, x86::rdx, x86::rsi,
                       x86::rdi, x86::rbx, x86::r9, x86::r10, x86::r11,
                       x86::r12, x86::r13, x86::r14, x86::r15);
    frame.setLocalStackSize(128); // jist in case
    frame.finalize();

    x86::Assembler a(&code);

    auto cc = std::make_shared<jit_cache>();
    cc->begin = base;
    cc->offset.resize(len / 2);
    JIT_OP::end_lbl = a.newLabel();
    // X64 prlogue
    a.emitProlog(frame);
    // save Base
    a.mov(x86::rbx, &cpu);
    jit_jumptable(a, *cc);

    uint32_t oldpc = cpu.PC;
    uint32_t end = base + len;
    jumpMap.clear();
    for(uint32_t pc = base; pc < end; pc += 2) {
        jumpMap[pc] = a.newLabel();
    }
    for(cpu.PC = base; cpu.PC < end;) {
        auto pc = cpu.PC;
        uint16_t op = FETCH();
        jit_tables.insert_or_assign(pc, cc);
        try {
            if(jit_doop[op]) {
                a.bind(jumpMap[pc]);
                cc->offset[(pc - cc->begin) >> 1] = a.offset();
                a.mov(ARG1, pc);
                a.call(op_prologue);
                jit_doop[op](a, op);
                jit_postop(a);
            } else {
                throw JitError{};
            }
        } catch(JitError &) {
            a.mov(ARG1, pc);
            a.call(op_prologue);
            a.call(ILLEGAL_OP);
            //            break;
        }
    }
    cpu.PC = oldpc;
    a.bind(JIT_OP::end_lbl);
    a.emitEpilog(frame);

    size_t sz = a.offset();
    Error err = rt->add(&cc->exec, &code);
    if(err) {
        printf("AsmJit failed: %s\n", DebugUtils::errorAsString(err));
        // TODO: jit fialure
    }
    AddJitFunction(cc.get(), frame, cc->exec, sz);
}

void jit_run(uint32_t pc) {
    cpu.PC = pc;
    if(!jit_tables.count(pc)) {
        jit_compile(pc, 0x4000);
    }
    auto e = jit_tables[pc].get();
    if(setjmp(cpu.ex) == 0) {
        (*e->exec)((pc - e->begin) >> 1);
    } else {
        handle_exception(cpu.ex_n);
    }
}
