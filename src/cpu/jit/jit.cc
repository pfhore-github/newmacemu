#include "exception.hpp"
#include "jit_common.hpp"
#include "jit_proto.hpp"
#include "memory.hpp"
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
using jit_op = void (*)(uint16_t op);
jit_op jit_compile_op[0x10000];
CpuFeatures feature;
std::unique_ptr<x86::Assembler> as;

void call_prolog() {
}
void call_epilog(bool ) {
}
void movem_w_store_decr_impl(uint16_t regs, int reg);
void movem_w_store_base_impl(uint16_t regs, int type, int reg);
namespace JIT_OP {
extern Label end_lbl;

} // namespace JIT_OP
void init_jit_table_fpu();
void jit_init() {
    rt = std::make_unique<JitRuntime>();

    // Comon
    for(int ea = 0; ea < 075; ++ea) {
        jit_compile_op[0000000 | ea] = JIT_OP::ori_b;
        jit_compile_op[0000100 | ea] = JIT_OP::ori_w;
        jit_compile_op[0000200 | ea] = JIT_OP::ori_l;
        jit_compile_op[0001000 | ea] = JIT_OP::andi_b;
        jit_compile_op[0001100 | ea] = JIT_OP::andi_w;
        jit_compile_op[0001200 | ea] = JIT_OP::andi_l;
        jit_compile_op[0002000 | ea] = JIT_OP::subi_b;
        jit_compile_op[0002100 | ea] = JIT_OP::subi_w;
        jit_compile_op[0002200 | ea] = JIT_OP::subi_l;
        jit_compile_op[0003000 | ea] = JIT_OP::addi_b;
        jit_compile_op[0003100 | ea] = JIT_OP::addi_w;
        jit_compile_op[0003200 | ea] = JIT_OP::addi_l;
        jit_compile_op[0005000 | ea] = JIT_OP::eori_b;
        jit_compile_op[0005100 | ea] = JIT_OP::eori_w;
        jit_compile_op[0005200 | ea] = JIT_OP::eori_l;
        jit_compile_op[0006000 | ea] = JIT_OP::cmpi_b;
        jit_compile_op[0006100 | ea] = JIT_OP::cmpi_w;
        jit_compile_op[0006200 | ea] = JIT_OP::cmpi_l;

        jit_compile_op[0040000 | ea] = JIT_OP::negx_b;
        jit_compile_op[0040100 | ea] = JIT_OP::negx_w;
        jit_compile_op[0040200 | ea] = JIT_OP::negx_l;
        jit_compile_op[0040300 | ea] = JIT_OP::move_from_sr;
        jit_compile_op[0041000 | ea] = JIT_OP::clr_b;
        jit_compile_op[0041100 | ea] = JIT_OP::clr_w;
        jit_compile_op[0041200 | ea] = JIT_OP::clr_l;
        jit_compile_op[0041300 | ea] = JIT_OP::move_from_ccr;
        jit_compile_op[0042000 | ea] = JIT_OP::neg_b;
        jit_compile_op[0042100 | ea] = JIT_OP::neg_w;
        jit_compile_op[0042200 | ea] = JIT_OP::neg_l;
        jit_compile_op[0042300 | ea] = JIT_OP::move_to_ccr;
        jit_compile_op[0043000 | ea] = JIT_OP::not_b;
        jit_compile_op[0043100 | ea] = JIT_OP::not_w;
        jit_compile_op[0043200 | ea] = JIT_OP::not_l;
        jit_compile_op[0043300 | ea] = JIT_OP::move_to_sr;

        jit_compile_op[0044000 | ea] = JIT_OP::nbcd;
        jit_compile_op[0045000 | ea] = JIT_OP::tst_b;
        jit_compile_op[0045100 | ea] = JIT_OP::tst_w;
        jit_compile_op[0045200 | ea] = JIT_OP::tst_l;
        jit_compile_op[0045300 | ea] = JIT_OP::tas;

        jit_compile_op[0046000 | ea] = JIT_OP::mul_l;
        jit_compile_op[0046100 | ea] = JIT_OP::div_l;

        for(int d = 0; d < 8; ++d) {
            jit_compile_op[0010000 | d << 9 | ea] = JIT_OP::move_b;
            jit_compile_op[0020000 | d << 9 | ea] = JIT_OP::move_l;
            jit_compile_op[0030000 | d << 9 | ea] = JIT_OP::move_w;
            jit_compile_op[0020100 | d << 9 | ea] = JIT_OP::movea_l;
            jit_compile_op[0030100 | d << 9 | ea] = JIT_OP::movea_w;
            jit_compile_op[0040400 | d << 9 | ea] = JIT_OP::chk_l;
            jit_compile_op[0040600 | d << 9 | ea] = JIT_OP::chk_l;
            jit_compile_op[0050000 | d << 9 | ea] = JIT_OP::addq_b;
            jit_compile_op[0050100 | d << 9 | ea] = JIT_OP::addq_w;
            jit_compile_op[0050200 | d << 9 | ea] = JIT_OP::addq_l;
            jit_compile_op[0050400 | d << 9 | ea] = JIT_OP::subq_b;
            jit_compile_op[0050500 | d << 9 | ea] = JIT_OP::subq_w;
            jit_compile_op[0050600 | d << 9 | ea] = JIT_OP::subq_l;

            jit_compile_op[0100000 | d << 9 | ea] = JIT_OP::or_b_to_dn;
            jit_compile_op[0100100 | d << 9 | ea] = JIT_OP::or_w_to_dn;
            jit_compile_op[0100200 | d << 9 | ea] = JIT_OP::or_l_to_dn;
            jit_compile_op[0100300 | d << 9 | ea] = JIT_OP::divu_w;
            jit_compile_op[0100700 | d << 9 | ea] = JIT_OP::divs_w;

            jit_compile_op[0110000 | d << 9 | ea] = JIT_OP::sub_b_to_dn;
            jit_compile_op[0110100 | d << 9 | ea] = JIT_OP::sub_w_to_dn;
            jit_compile_op[0110200 | d << 9 | ea] = JIT_OP::sub_l_to_dn;
            jit_compile_op[0110300 | d << 9 | ea] = JIT_OP::suba_w;
            jit_compile_op[0110700 | d << 9 | ea] = JIT_OP::suba_l;

            jit_compile_op[0130000 | d << 9 | ea] = JIT_OP::cmp_b;
            jit_compile_op[0130100 | d << 9 | ea] = JIT_OP::cmp_w;
            jit_compile_op[0130200 | d << 9 | ea] = JIT_OP::cmp_l;
            jit_compile_op[0130300 | d << 9 | ea] = JIT_OP::cmpa_w;

            jit_compile_op[0130400 | d << 9 | ea] = JIT_OP::eor_b;
            jit_compile_op[0130500 | d << 9 | ea] = JIT_OP::eor_w;
            jit_compile_op[0130600 | d << 9 | ea] = JIT_OP::eor_l;
            jit_compile_op[0130700 | d << 9 | ea] = JIT_OP::cmpa_l;

            jit_compile_op[0140000 | d << 9 | ea] = JIT_OP::and_b_to_dn;
            jit_compile_op[0140100 | d << 9 | ea] = JIT_OP::and_w_to_dn;
            jit_compile_op[0140200 | d << 9 | ea] = JIT_OP::and_l_to_dn;
            jit_compile_op[0140300 | d << 9 | ea] = JIT_OP::mulu_w;
            jit_compile_op[0140700 | d << 9 | ea] = JIT_OP::muls_w;

            jit_compile_op[0150000 | d << 9 | ea] = JIT_OP::add_b_to_dn;
            jit_compile_op[0150100 | d << 9 | ea] = JIT_OP::add_w_to_dn;
            jit_compile_op[0150200 | d << 9 | ea] = JIT_OP::add_l_to_dn;
            jit_compile_op[0150300 | d << 9 | ea] = JIT_OP::adda_w;
            jit_compile_op[0150700 | d << 9 | ea] = JIT_OP::adda_l;

            for(int k = 2; k < 8; ++k) {
                jit_compile_op[0010000 | d << 9 | k << 6 | ea] = JIT_OP::move_b;
                jit_compile_op[0020000 | d << 9 | k << 6 | ea] = JIT_OP::move_l;
                jit_compile_op[0030000 | d << 9 | k << 6 | ea] = JIT_OP::move_w;
            }
        }
    }
    // EA = Dn
    for(int i = 0; i < 8; ++i) {
        jit_compile_op[0004000 | i] = JIT_OP::btst_l_i;
        jit_compile_op[0004100 | i] = JIT_OP::bchg_l_i;
        jit_compile_op[0004200 | i] = JIT_OP::bclr_l_i;
        jit_compile_op[0004300 | i] = JIT_OP::bset_l_i;

        jit_compile_op[0044100 | i] = JIT_OP::swap;
        jit_compile_op[0044200 | i] = JIT_OP::ext_w;
        jit_compile_op[0044300 | i] = JIT_OP::ext_l;
        jit_compile_op[0044700 | i] = JIT_OP::extb_l;

        jit_compile_op[0047100 | i] = JIT_OP::trap;

        jit_compile_op[0164300 | i] = JIT_OP::bftst_dn;
        jit_compile_op[0165300 | i] = JIT_OP::bfchg_dn;
        jit_compile_op[0166300 | i] = JIT_OP::bfclr_dn;
        jit_compile_op[0167300 | i] = JIT_OP::bfset_dn;

        jit_compile_op[0164700 | i] = JIT_OP::bfextu_dn;
        jit_compile_op[0165700 | i] = JIT_OP::bfexts_dn;
        jit_compile_op[0166700 | i] = JIT_OP::bfffo_dn;
        jit_compile_op[0167700 | i] = JIT_OP::bfins_dn;

        for(int d = 0; d < 8; ++d) {
            jit_compile_op[0000400 | d << 9 | i] = JIT_OP::btst_l_r;
            jit_compile_op[0000500 | d << 9 | i] = JIT_OP::bchg_l_r;
            jit_compile_op[0000600 | d << 9 | i] = JIT_OP::bclr_l_r;
            jit_compile_op[0000700 | d << 9 | i] = JIT_OP::bset_l_r;

            jit_compile_op[0100400 | d << 9 | i] = JIT_OP::sbcd_d;
            jit_compile_op[0100500 | d << 9 | i] = JIT_OP::pack_d;
            jit_compile_op[0100600 | d << 9 | i] = JIT_OP::unpk_d;
            jit_compile_op[0110400 | d << 9 | i] = JIT_OP::subx_b_d;
            jit_compile_op[0110500 | d << 9 | i] = JIT_OP::subx_w_d;
            jit_compile_op[0110600 | d << 9 | i] = JIT_OP::subx_l_d;

            jit_compile_op[0140400 | d << 9 | i] = JIT_OP::abcd_d;
            jit_compile_op[0140500 | d << 9 | i] = JIT_OP::exg_dn;
            jit_compile_op[0150400 | d << 9 | i] = JIT_OP::addx_b_d;
            jit_compile_op[0150500 | d << 9 | i] = JIT_OP::addx_w_d;
            jit_compile_op[0150600 | d << 9 | i] = JIT_OP::addx_l_d;
        }

        for(int c = 0; c < 16; ++c) {
            jit_compile_op[0050300 | c << 8 | i] = JIT_OP::scc_ea;
        }
    }
    // An
    for(int i = 0; i < 8; ++i) {
        for(int d = 0; d < 8; ++d) {
            jit_compile_op[0000410 | d << 9 | i] = JIT_OP::movep_w_load;
            jit_compile_op[0000510 | d << 9 | i] = JIT_OP::movep_l_load;
            jit_compile_op[0000610 | d << 9 | i] = JIT_OP::movep_w_store;
            jit_compile_op[0000710 | d << 9 | i] = JIT_OP::movep_l_store;

            jit_compile_op[0050110 | d << 9 | i] = JIT_OP::addq_an;
            jit_compile_op[0050210 | d << 9 | i] = JIT_OP::addq_an;
            jit_compile_op[0050510 | d << 9 | i] = JIT_OP::subq_an;
            jit_compile_op[0050610 | d << 9 | i] = JIT_OP::subq_an;

            jit_compile_op[0100410 | d << 9 | i] = JIT_OP::sbcd_m;
            jit_compile_op[0100510 | d << 9 | i] = JIT_OP::pack_m;
            jit_compile_op[0100610 | d << 9 | i] = JIT_OP::unpk_m;

            jit_compile_op[0110410 | d << 9 | i] = JIT_OP::subx_b_m;
            jit_compile_op[0110510 | d << 9 | i] = JIT_OP::subx_w_m;
            jit_compile_op[0110610 | d << 9 | i] = JIT_OP::subx_l_m;

            jit_compile_op[0130410 | d << 9 | i] = JIT_OP::cmpm_b;
            jit_compile_op[0130510 | d << 9 | i] = JIT_OP::cmpm_w;
            jit_compile_op[0130610 | d << 9 | i] = JIT_OP::cmpm_l;

            jit_compile_op[0140410 | d << 9 | i] = JIT_OP::abcd_m;
            jit_compile_op[0140510 | d << 9 | i] = JIT_OP::exg_an;
            jit_compile_op[0140610 | d << 9 | i] = JIT_OP::exg_da;
            jit_compile_op[0150410 | d << 9 | i] = JIT_OP::addx_b_m;
            jit_compile_op[0150510 | d << 9 | i] = JIT_OP::addx_w_m;
            jit_compile_op[0150610 | d << 9 | i] = JIT_OP::addx_l_m;
        }
        jit_compile_op[0044010 | i] = JIT_OP::link_l;
        jit_compile_op[0044110 | i] = JIT_OP::bkpt;
        jit_compile_op[0047110 | i] = JIT_OP::trap;
        for(int c = 0; c < 16; ++c) {
            jit_compile_op[0050310 | c << 8 | i] = JIT_OP::dbcc;
        }
    }
    for(int ea = 020; ea < 074; ++ea) {
        if(ea >= 030 && ea < 040) {
            // Increment only
            jit_compile_op[0046200 | ea] = JIT_OP::movem_w_load_incr;
            jit_compile_op[0046300 | ea] = JIT_OP::movem_l_load_incr;
        }
        if(ea >= 040 && ea < 050) {
            //  Decrement only
            jit_compile_op[0044200 | ea] = JIT_OP::movem_w_store_decr;
            jit_compile_op[0044300 | ea] = JIT_OP::movem_l_store_decr;
        }
        if(ea < 030 || ea >= 050) {
            // Ctl Only
            jit_compile_op[0000300 | ea] = JIT_OP::cmp2_chk2_b;
            jit_compile_op[0001300 | ea] = JIT_OP::cmp2_chk2_w;
            jit_compile_op[0002300 | ea] = JIT_OP::cmp2_chk2_l;
            jit_compile_op[0044100 | ea] = JIT_OP::pea;
            jit_compile_op[0044200 | ea] = JIT_OP::movem_w_store_base;
            jit_compile_op[0044300 | ea] = JIT_OP::movem_l_store_base;
            jit_compile_op[0046200 | ea] = JIT_OP::movem_w_load_base;
            jit_compile_op[0046300 | ea] = JIT_OP::movem_l_load_base;

            jit_compile_op[0047200 | ea] = JIT_OP::jsr;
            jit_compile_op[0047300 | ea] = JIT_OP::jmp;

            jit_compile_op[0164300 | ea] = JIT_OP::bftst_mem;
            jit_compile_op[0165300 | ea] = JIT_OP::bfchg_mem;
            jit_compile_op[0166300 | ea] = JIT_OP::bfclr_mem;
            jit_compile_op[0167300 | ea] = JIT_OP::bfset_mem;

            jit_compile_op[0164700 | ea] = JIT_OP::bfextu_mem;
            jit_compile_op[0165700 | ea] = JIT_OP::bfexts_mem;
            jit_compile_op[0166700 | ea] = JIT_OP::bfffo_mem;
            jit_compile_op[0167700 | ea] = JIT_OP::bfins_mem;

            for(int d = 0; d < 8; ++d) {
                jit_compile_op[0040700 | d << 9 | ea] = JIT_OP::lea;
            }
        }

        jit_compile_op[0004000 | ea] = JIT_OP::btst_b_i;
        jit_compile_op[0004100 | ea] = JIT_OP::bchg_b_i;
        jit_compile_op[0004200 | ea] = JIT_OP::bclr_b_i;
        jit_compile_op[0004300 | ea] = JIT_OP::bset_b_i;
        jit_compile_op[0005300 | ea] = JIT_OP::cas_b;
        jit_compile_op[0006300 | ea] = JIT_OP::cas_w;
        jit_compile_op[0007000 | ea] = JIT_OP::moves_b;
        jit_compile_op[0007100 | ea] = JIT_OP::moves_w;
        jit_compile_op[0007200 | ea] = JIT_OP::moves_l;
        jit_compile_op[0007300 | ea] = JIT_OP::cas_l;

        jit_compile_op[0160300 | ea] = JIT_OP::asr_ea;
        jit_compile_op[0161300 | ea] = JIT_OP::lsr_ea;
        jit_compile_op[0162300 | ea] = JIT_OP::roxr_ea;
        jit_compile_op[0163300 | ea] = JIT_OP::ror_ea;

        jit_compile_op[0160700 | ea] = JIT_OP::asl_ea;
        jit_compile_op[0161700 | ea] = JIT_OP::lsl_ea;
        jit_compile_op[0162700 | ea] = JIT_OP::roxl_ea;
        jit_compile_op[0163700 | ea] = JIT_OP::rol_ea;

        for(int d = 0; d < 8; ++d) {
            jit_compile_op[0000400 | d << 9 | ea] = JIT_OP::btst_b_r;
            jit_compile_op[0000500 | d << 9 | ea] = JIT_OP::bchg_b_r;
            jit_compile_op[0000600 | d << 9 | ea] = JIT_OP::bclr_b_r;
            jit_compile_op[0000700 | d << 9 | ea] = JIT_OP::bset_b_r;

            jit_compile_op[0100400 | d << 9 | ea] = JIT_OP::or_b_to_ea;
            jit_compile_op[0100500 | d << 9 | ea] = JIT_OP::or_w_to_ea;
            jit_compile_op[0100600 | d << 9 | ea] = JIT_OP::or_l_to_ea;

            jit_compile_op[0110400 | d << 9 | ea] = JIT_OP::sub_b_to_ea;
            jit_compile_op[0110500 | d << 9 | ea] = JIT_OP::sub_w_to_ea;
            jit_compile_op[0110600 | d << 9 | ea] = JIT_OP::sub_l_to_ea;

            jit_compile_op[0140400 | d << 9 | ea] = JIT_OP::and_b_to_ea;
            jit_compile_op[0140500 | d << 9 | ea] = JIT_OP::and_w_to_ea;
            jit_compile_op[0140600 | d << 9 | ea] = JIT_OP::and_l_to_ea;

            jit_compile_op[0150400 | d << 9 | ea] = JIT_OP::add_b_to_ea;
            jit_compile_op[0150500 | d << 9 | ea] = JIT_OP::add_w_to_ea;
            jit_compile_op[0150600 | d << 9 | ea] = JIT_OP::add_l_to_ea;
        }

        for(int c = 0; c < 16; ++c) {
            if(ea < 072) {
                jit_compile_op[0050300 | c << 8 | ea] = JIT_OP::scc_ea;
            }
        }
    }
    jit_compile_op[0000074] = JIT_OP::ori_b_ccr;
    jit_compile_op[0000174] = JIT_OP::ori_w_sr;
    jit_compile_op[0001074] = JIT_OP::andi_b_ccr;
    jit_compile_op[0001174] = JIT_OP::andi_w_sr;
    jit_compile_op[0004074] = JIT_OP::btst_b_i_imm;
    jit_compile_op[0005074] = JIT_OP::eori_b_ccr;
    jit_compile_op[0005174] = JIT_OP::eori_w_sr;
    jit_compile_op[0006374] = JIT_OP::cas2_w;
    jit_compile_op[0007374] = JIT_OP::cas2_l;
    jit_compile_op[0042374] = JIT_OP::move_to_ccr;
    jit_compile_op[0043374] = JIT_OP::move_to_sr;
    jit_compile_op[0045374] = JIT_OP::illegal;
    for(int d = 0; d < 8; ++d) {
        jit_compile_op[0000474 | d << 9] = JIT_OP::btst_b_r_imm;
        jit_compile_op[0020074 | d << 9] = JIT_OP::move_b;
        jit_compile_op[0020074 | d << 9] = JIT_OP::move_l;
        jit_compile_op[0030074 | d << 9] = JIT_OP::move_w;
        for(int k = 2; k < 8; ++k) {
            jit_compile_op[0020074 | d << 9 | k << 6] = JIT_OP::move_b;
            jit_compile_op[0020074 | d << 9 | k << 6] = JIT_OP::move_l;
            jit_compile_op[0030074 | d << 9 | k << 6] = JIT_OP::move_w;
        }
    }
    for(int d = 0; d < 8; ++d) {
        jit_compile_op[0047120 | d] = JIT_OP::link_w;
        jit_compile_op[0047130 | d] = JIT_OP::unlk;
        jit_compile_op[0047140 | d] = JIT_OP::move_to_usp;
        jit_compile_op[0047150 | d] = JIT_OP::move_from_usp;

        for(int i = 0; i < 0x100; ++i) {
            jit_compile_op[0070000 | d << 9 | i] = JIT_OP::moveq;
        }
    }
    jit_compile_op[0047160] = JIT_OP::reset;
    jit_compile_op[0047161] = JIT_OP::nop;
    jit_compile_op[0047162] = JIT_OP::stop;
    jit_compile_op[0047163] = JIT_OP::rte;
    jit_compile_op[0047164] = JIT_OP::rtd;
    jit_compile_op[0047165] = JIT_OP::rts;
    jit_compile_op[0047166] = JIT_OP::trapv;
    jit_compile_op[0047167] = JIT_OP::rtr;

    jit_compile_op[0047172] = JIT_OP::movec_from_cr;
    jit_compile_op[0047173] = JIT_OP::movec_to_cr;

    for(int c = 0; c < 16; ++c) {
        jit_compile_op[0050372 | c << 8] = JIT_OP::trapcc;
        jit_compile_op[0050373 | c << 8] = JIT_OP::trapcc;
        jit_compile_op[0050374 | c << 8] = JIT_OP::trapcc;
    }
    for(int v = 0; v < 0x1000; ++v) {
        jit_compile_op[0060000 | v] = JIT_OP::bxx;
        jit_compile_op[0120000 | v] = JIT_OP::aline_ex;
        jit_compile_op[0170000 | v] = JIT_OP::fline_default;
    }

    for(int im = 0; im < 8; ++im) {
        for(int reg = 0; reg < 8; ++reg) {
            jit_compile_op[0160000 | im << 9 | reg] = JIT_OP::asr_b_i;
            jit_compile_op[0160010 | im << 9 | reg] = JIT_OP::lsr_b_i;
            jit_compile_op[0160020 | im << 9 | reg] = JIT_OP::roxr_b_i;
            jit_compile_op[0160030 | im << 9 | reg] = JIT_OP::ror_b_i;
            jit_compile_op[0160040 | im << 9 | reg] = JIT_OP::asr_b_r;
            jit_compile_op[0160050 | im << 9 | reg] = JIT_OP::lsr_b_r;
            jit_compile_op[0160060 | im << 9 | reg] = JIT_OP::roxr_b_r;
            jit_compile_op[0160070 | im << 9 | reg] = JIT_OP::ror_b_r;

            jit_compile_op[0160100 | im << 9 | reg] = JIT_OP::asr_w_i;
            jit_compile_op[0160110 | im << 9 | reg] = JIT_OP::lsr_w_i;
            jit_compile_op[0160120 | im << 9 | reg] = JIT_OP::roxr_w_i;
            jit_compile_op[0160130 | im << 9 | reg] = JIT_OP::ror_w_i;
            jit_compile_op[0160140 | im << 9 | reg] = JIT_OP::asr_w_r;
            jit_compile_op[0160150 | im << 9 | reg] = JIT_OP::lsr_w_r;
            jit_compile_op[0160160 | im << 9 | reg] = JIT_OP::roxr_w_r;
            jit_compile_op[0160170 | im << 9 | reg] = JIT_OP::ror_w_r;

            jit_compile_op[0160200 | im << 9 | reg] = JIT_OP::asr_l_i;
            jit_compile_op[0160210 | im << 9 | reg] = JIT_OP::lsr_l_i;
            jit_compile_op[0160220 | im << 9 | reg] = JIT_OP::roxr_l_i;
            jit_compile_op[0160230 | im << 9 | reg] = JIT_OP::ror_l_i;
            jit_compile_op[0160240 | im << 9 | reg] = JIT_OP::asr_l_r;
            jit_compile_op[0160250 | im << 9 | reg] = JIT_OP::lsr_l_r;
            jit_compile_op[0160260 | im << 9 | reg] = JIT_OP::roxr_l_r;
            jit_compile_op[0160270 | im << 9 | reg] = JIT_OP::ror_l_r;

            jit_compile_op[0160400 | im << 9 | reg] = JIT_OP::asl_b_i;
            jit_compile_op[0160410 | im << 9 | reg] = JIT_OP::lsl_b_i;
            jit_compile_op[0160420 | im << 9 | reg] = JIT_OP::roxl_b_i;
            jit_compile_op[0160430 | im << 9 | reg] = JIT_OP::rol_b_i;
            jit_compile_op[0160440 | im << 9 | reg] = JIT_OP::asl_b_r;
            jit_compile_op[0160450 | im << 9 | reg] = JIT_OP::lsl_b_r;
            jit_compile_op[0160460 | im << 9 | reg] = JIT_OP::roxl_b_r;
            jit_compile_op[0160470 | im << 9 | reg] = JIT_OP::rol_b_r;

            jit_compile_op[0160500 | im << 9 | reg] = JIT_OP::asl_w_i;
            jit_compile_op[0160510 | im << 9 | reg] = JIT_OP::lsl_w_i;
            jit_compile_op[0160520 | im << 9 | reg] = JIT_OP::roxl_w_i;
            jit_compile_op[0160530 | im << 9 | reg] = JIT_OP::rol_w_i;
            jit_compile_op[0160540 | im << 9 | reg] = JIT_OP::asl_w_r;
            jit_compile_op[0160550 | im << 9 | reg] = JIT_OP::lsl_w_r;
            jit_compile_op[0160560 | im << 9 | reg] = JIT_OP::roxl_w_r;
            jit_compile_op[0160570 | im << 9 | reg] = JIT_OP::rol_w_r;

            jit_compile_op[0160600 | im << 9 | reg] = JIT_OP::asl_l_i;
            jit_compile_op[0160610 | im << 9 | reg] = JIT_OP::lsl_l_i;
            jit_compile_op[0160620 | im << 9 | reg] = JIT_OP::roxl_l_i;
            jit_compile_op[0160630 | im << 9 | reg] = JIT_OP::rol_l_i;
            jit_compile_op[0160640 | im << 9 | reg] = JIT_OP::asl_l_r;
            jit_compile_op[0160650 | im << 9 | reg] = JIT_OP::lsl_l_r;
            jit_compile_op[0160660 | im << 9 | reg] = JIT_OP::roxl_l_r;
            jit_compile_op[0160670 | im << 9 | reg] = JIT_OP::rol_l_r;
        }
    }
    for(int i = 0; i < 8; ++i) {
        jit_compile_op[0173000 | i] = JIT_OP::move16_inc_imm;
        jit_compile_op[0173010 | i] = JIT_OP::move16_imm_inc;
        jit_compile_op[0173020 | i] = JIT_OP::move16_base_imm;
        jit_compile_op[0173030 | i] = JIT_OP::move16_imm_base;
        jit_compile_op[0173040 | i] = JIT_OP::move16_inc_inc;
    }
    init_jit_table_fpu();
#ifdef CI
    jit_compile_op[0044117] = JIT_OP::jit_exit;
#endif
}
void op_prologue(uint32_t pc);

void jit_jumptable(jit_cache &jit) {
    // jump to PC
    as->mov(x86::eax, ARG1.r32());
    as->mov(x86::rdx, jit.offset.data());
    as->movzx(x86::rdx, x86::word_ptr(x86::rdx, x86::rax, 1));
    as->lea(x86::rax, x86::ptr(x86::rip));
    int bd = as->offset();
    as->lea(x86::rax, x86::ptr(x86::rax, x86::rdx, 0, -bd));
    as->jmp(x86::rax);
    int ep = as->offset();
    std::fill(jit.offset.begin(), jit.offset.end(), ep);
}
void jit_postop() {
    Label lb = as->newLabel();
    as->mov(x86::al, x86::byte_ptr(x86::rbx, offsetof(Cpu, must_trace)));
    as->mov(x86::byte_ptr(x86::rbx, offsetof(Cpu, must_trace)), 0);
    as->test(x86::al, 1);
    as->je(lb);
    as->call(TRACE);
    as->bind(lb);
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
    frame.setLocalStackSize(8 * 10);
    frame.finalize();
    as = std::make_unique<x86::Assembler>(&code);

    auto cc = std::make_shared<jit_cache>();
    cc->begin = base;
    cc->offset.resize(len / 2);
    JIT_OP::end_lbl = as->newLabel();
    // X64 prlogue
    as->emitProlog(frame);
    // save Base
    as->mov(x86::rbx, &cpu);
    jit_jumptable(*cc);

    uint32_t oldpc = cpu.PC;
    uint32_t end = base + len;
    jumpMap.clear();
    for(uint32_t pc = base; pc < end; pc += 2) {
        jumpMap[pc] = as->newLabel();
    }
    for(cpu.PC = base; cpu.PC < end;) {
        auto pc = cpu.PC;
        uint16_t op = FETCH();
        jit_tables.insert_or_assign(pc, cc);
        try {
            if(jit_compile_op[op]) {
                as->bind(jumpMap[pc]);
                cc->offset[(pc - cc->begin) >> 1] = as->offset();
                as->mov(ARG1.r32(), pc);
                as->call(op_prologue);
                jit_compile_op[op](op);
                jit_postop();
            } else {
                throw JitError{};
            }
        } catch(JitError &) {
            as->mov(ARG1.r32(), pc);
            as->call(op_prologue);
            as->call(ILLEGAL_OP);
            //            break;
        }
    }
    cpu.PC = oldpc;
    as->bind(JIT_OP::end_lbl);
    as->emitEpilog(frame);

    size_t sz = as->offset();
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
