#ifndef JIT_PROTO_HPP_
#define JIT_PROTO_HPP_ 1
#include "asmjit/asmjit.h"
#include "jit.hpp"
#include <stdint.h>
#define JIT_OP_(name_) void name_(uint16_t op)
namespace JIT_OP {
JIT_OP_(jit_exit);
JIT_OP_(ori_b);
JIT_OP_(ori_w);
JIT_OP_(ori_l);
JIT_OP_(ori_b_ccr);
JIT_OP_(ori_w_sr);

JIT_OP_(andi_b);

JIT_OP_(andi_w);
JIT_OP_(andi_l);

JIT_OP_(andi_b_ccr);

JIT_OP_(andi_w_sr);

JIT_OP_(eori_b);

JIT_OP_(eori_w);
JIT_OP_(eori_l);

JIT_OP_(eori_b_ccr);

JIT_OP_(eori_w_sr);

JIT_OP_(subi_b);

JIT_OP_(subi_w);
JIT_OP_(subi_l);

JIT_OP_(addi_b);

JIT_OP_(addi_w);
JIT_OP_(addi_l);

JIT_OP_(cmpi_b);

JIT_OP_(cmpi_w);
JIT_OP_(cmpi_l);

JIT_OP_(cmp2_chk2_b);

JIT_OP_(cmp2_chk2_w);

JIT_OP_(cmp2_chk2_l);
JIT_OP_(btst_l_i);

JIT_OP_(btst_b_i);

JIT_OP_(btst_b_i_imm);

JIT_OP_(btst_l_r);

JIT_OP_(btst_b_r);

JIT_OP_(btst_b_r_imm);

JIT_OP_(bchg_l_i);

JIT_OP_(bchg_b_i);

JIT_OP_(bchg_l_r);

JIT_OP_(bchg_b_r);

JIT_OP_(bclr_l_i);

JIT_OP_(bclr_b_i);

JIT_OP_(bclr_l_r);

JIT_OP_(bclr_b_r);

JIT_OP_(bset_l_i);

JIT_OP_(bset_b_i);

JIT_OP_(bset_l_r);

JIT_OP_(bset_b_r);
JIT_OP_(movep_w_load);

JIT_OP_(movep_l_load);

JIT_OP_(movep_w_store);

JIT_OP_(movep_l_store);

JIT_OP_(cas_b);

JIT_OP_(cas_w);

JIT_OP_(cas_l);

JIT_OP_(cas2_w);

JIT_OP_(cas2_l);

JIT_OP_(moves_b);


JIT_OP_(moves_w);

JIT_OP_(moves_l);


JIT_OP_(move_b);

JIT_OP_(move_w);

JIT_OP_(move_l);

JIT_OP_(movea_w);

JIT_OP_(movea_l);

JIT_OP_(move_from_sr);

JIT_OP_(move_from_ccr);

JIT_OP_(move_to_sr);

JIT_OP_(move_to_ccr);

JIT_OP_(negx_b);

JIT_OP_(negx_w);

JIT_OP_(negx_l);

JIT_OP_(clr_b);

JIT_OP_(clr_w);

JIT_OP_(clr_l);

JIT_OP_(neg_b);

JIT_OP_(neg_w);

JIT_OP_(neg_l);

JIT_OP_(not_b);

JIT_OP_(not_w);

JIT_OP_(not_l);
JIT_OP_(nbcd);

JIT_OP_(link_l);

JIT_OP_(swap);

JIT_OP_(bkpt);

JIT_OP_(pea);

JIT_OP_(ext_w);

JIT_OP_(ext_l);

JIT_OP_(movem_w_store_decr);
JIT_OP_(movem_w_store_base);
JIT_OP_(movem_l_store_decr);
JIT_OP_(movem_l_store_base);

JIT_OP_(movem_w_load_incr);
JIT_OP_(movem_w_load_base);
JIT_OP_(movem_l_load_incr);
JIT_OP_(movem_l_load_base);

JIT_OP_(tst_b);
JIT_OP_(tst_w);
JIT_OP_(tst_l);
JIT_OP_(tas);

JIT_OP_(illegal);
JIT_OP_(mul_l);
JIT_OP_(div_l);

JIT_OP_(trap);
JIT_OP_(link_w);

JIT_OP_(unlk);
JIT_OP_(move_to_usp);
JIT_OP_(move_from_usp);
JIT_OP_(reset);
JIT_OP_(nop);
JIT_OP_(stop);
JIT_OP_(rte);
JIT_OP_(rtd);
JIT_OP_(rts);
JIT_OP_(trapv);
JIT_OP_(rtr);
JIT_OP_(movec_from_cr);
JIT_OP_(movec_to_cr);
JIT_OP_(jsr);
JIT_OP_(jmp);
JIT_OP_(chk_l);
JIT_OP_(chk_w);
JIT_OP_(extb_l);
JIT_OP_(lea);
JIT_OP_(addq_b);
JIT_OP_(addq_w);
JIT_OP_(addq_l);
JIT_OP_(subq_b);
JIT_OP_(subq_w);
JIT_OP_(subq_l);
JIT_OP_(addq_an);
JIT_OP_(subq_an);
JIT_OP_(scc_ea);
JIT_OP_(dbcc);
JIT_OP_(trapcc);
JIT_OP_(bxx);
JIT_OP_(moveq);
JIT_OP_(or_b_to_dn);
JIT_OP_(or_w_to_dn);
JIT_OP_(or_l_to_dn);
JIT_OP_(divu_w);
JIT_OP_(sbcd_d);
JIT_OP_(sbcd_m);
JIT_OP_(or_b_to_ea);
JIT_OP_(sbcd_m);
JIT_OP_(pack_d);
JIT_OP_(pack_m);
JIT_OP_(or_w_to_ea);
JIT_OP_(unpk_d);
JIT_OP_(unpk_m);
JIT_OP_(or_l_to_ea);
JIT_OP_(divs_w);
JIT_OP_(sub_b_to_dn);
JIT_OP_(sub_w_to_dn);
JIT_OP_(sub_l_to_dn);
JIT_OP_(suba_w);
JIT_OP_(suba_l);
JIT_OP_(subx_b_d);
JIT_OP_(subx_b_m);
JIT_OP_(subx_w_d);
JIT_OP_(subx_w_m);
JIT_OP_(subx_l_d);
JIT_OP_(subx_l_m);
JIT_OP_(sub_b_to_ea);
JIT_OP_(sub_w_to_ea);
JIT_OP_(sub_l_to_ea);
JIT_OP_(aline_ex);
JIT_OP_(cmp_b);
JIT_OP_(cmp_w);
JIT_OP_(cmp_l);
JIT_OP_(cmpa_w);
JIT_OP_(cmpa_l);
JIT_OP_(eor_b);
JIT_OP_(eor_w);
JIT_OP_(eor_l);
JIT_OP_(cmpm_b);
JIT_OP_(cmpm_w);
JIT_OP_(cmpm_l);
JIT_OP_(and_b_to_dn);
JIT_OP_(and_w_to_dn);
JIT_OP_(and_l_to_dn);
JIT_OP_(mulu_w);
JIT_OP_(abcd_d);
JIT_OP_(abcd_m);
JIT_OP_(and_b_to_ea);
JIT_OP_(and_w_to_ea);
JIT_OP_(and_l_to_ea);
JIT_OP_(muls_w);
JIT_OP_(exg_dn);
JIT_OP_(exg_an);
JIT_OP_(exg_da);
JIT_OP_(add_b_to_dn);
JIT_OP_(add_w_to_dn);
JIT_OP_(add_l_to_dn);
JIT_OP_(adda_w);
JIT_OP_(adda_l);
JIT_OP_(addx_b_d);
JIT_OP_(addx_b_m);
JIT_OP_(addx_w_d);
JIT_OP_(addx_w_m);
JIT_OP_(addx_l_d);
JIT_OP_(addx_l_m);
JIT_OP_(add_b_to_ea);
JIT_OP_(add_w_to_ea);
JIT_OP_(add_l_to_ea);
JIT_OP_(asr_b_i);
JIT_OP_(lsr_b_i);
JIT_OP_(roxr_b_i);
JIT_OP_(ror_b_i);
JIT_OP_(asr_b_r);
JIT_OP_(lsr_b_r);
JIT_OP_(roxr_b_r);
JIT_OP_(ror_b_r);
JIT_OP_(asr_w_i);
JIT_OP_(lsr_w_i);
JIT_OP_(roxr_w_i);
JIT_OP_(ror_w_i);
JIT_OP_(asr_w_r);
JIT_OP_(lsr_w_r);
JIT_OP_(roxr_w_r);
JIT_OP_(ror_w_r);
JIT_OP_(asr_w_i);
JIT_OP_(lsr_w_i);
JIT_OP_(roxr_w_i);
JIT_OP_(ror_w_i);
JIT_OP_(asr_w_r);
JIT_OP_(lsr_w_r);
JIT_OP_(roxr_w_r);
JIT_OP_(ror_w_r);
JIT_OP_(asr_l_i);
JIT_OP_(lsr_l_i);
JIT_OP_(roxr_l_i);
JIT_OP_(ror_l_i);
JIT_OP_(asr_l_r);
JIT_OP_(lsr_l_r);
JIT_OP_(roxr_l_r);
JIT_OP_(ror_l_r);
JIT_OP_(asr_l_i);
JIT_OP_(lsr_l_i);
JIT_OP_(roxr_l_i);
JIT_OP_(ror_l_i);
JIT_OP_(asr_l_r);
JIT_OP_(lsr_l_r);
JIT_OP_(roxr_l_r);
JIT_OP_(ror_l_r);
JIT_OP_(asr_ea);
JIT_OP_(lsr_ea);
JIT_OP_(roxr_ea);
JIT_OP_(ror_ea);

JIT_OP_(asl_b_i);
JIT_OP_(lsl_b_i);
JIT_OP_(roxl_b_i);
JIT_OP_(rol_b_i);
JIT_OP_(asl_b_r);
JIT_OP_(lsl_b_r);
JIT_OP_(roxl_b_r);
JIT_OP_(rol_b_r);
JIT_OP_(asl_w_i);
JIT_OP_(lsl_w_i);
JIT_OP_(roxl_w_i);
JIT_OP_(rol_w_i);
JIT_OP_(asl_w_r);
JIT_OP_(lsl_w_r);
JIT_OP_(roxl_w_r);
JIT_OP_(rol_w_r);
JIT_OP_(asl_w_i);
JIT_OP_(lsl_w_i);
JIT_OP_(roxl_w_i);
JIT_OP_(rol_w_i);
JIT_OP_(asl_w_r);
JIT_OP_(lsl_w_r);
JIT_OP_(roxl_w_r);
JIT_OP_(rol_w_r);
JIT_OP_(asl_l_i);
JIT_OP_(lsl_l_i);
JIT_OP_(roxl_l_i);
JIT_OP_(rol_l_i);
JIT_OP_(asl_l_r);
JIT_OP_(lsl_l_r);
JIT_OP_(roxl_l_r);
JIT_OP_(rol_l_r);
JIT_OP_(asl_l_i);
JIT_OP_(lsl_l_i);
JIT_OP_(roxl_l_i);
JIT_OP_(rol_l_i);
JIT_OP_(asl_l_r);
JIT_OP_(lsl_l_r);
JIT_OP_(roxl_l_r);
JIT_OP_(rol_l_r);
JIT_OP_(asl_ea);
JIT_OP_(lsl_ea);
JIT_OP_(roxl_ea);
JIT_OP_(rol_ea);

JIT_OP_(bftst_dn);
JIT_OP_(bftst_mem);
JIT_OP_(bfchg_dn);
JIT_OP_(bfchg_mem);
JIT_OP_(bfclr_dn);
JIT_OP_(bfclr_mem);
JIT_OP_(bfset_dn);
JIT_OP_(bfset_mem);
JIT_OP_(bfextu_dn);
JIT_OP_(bfextu_mem);
JIT_OP_(bfexts_dn);
JIT_OP_(bfexts_mem);
JIT_OP_(bfffo_dn);
JIT_OP_(bfffo_mem);
JIT_OP_(bfins_dn);
JIT_OP_(bfins_mem);

JIT_OP_(fline_default);
JIT_OP_(move16_inc_imm);
JIT_OP_(move16_imm_inc);
JIT_OP_(move16_base_imm);
JIT_OP_(move16_imm_base);
JIT_OP_(move16_inc_inc);
}
#endif