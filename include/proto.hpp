#ifndef INLINE_HPP
#define INLINE_HPP 1
#include "68040.hpp"
#include "exception.hpp"
#include "memory.hpp"
#include <limits>
#include <stdint.h>
inline void PUSH16(uint16_t v) { MMU_WriteW(cpu.A[7] -= 2, v); }
inline void PUSH32(uint32_t v) { MMU_WriteL(cpu.A[7] -= 4, v); }
inline uint16_t POP16() {
    uint16_t v = MMU_ReadW(cpu.A[7]);
    cpu.A[7] += 2;
    return v;
}
inline void JUMP(uint32_t next) {
    cpu.nextpc = next;
    if(next & 1) {
        ADDRESS_ERROR();
    }
}
inline uint16_t POP32() {
    uint32_t v = MMU_ReadL(cpu.A[7]);
    cpu.A[7] += 4;
    return v;
}
inline uint8_t GetCCR() {
    return cpu.X << 4 | cpu.N << 3 | cpu.Z << 2 | cpu.V << 1 | cpu.C;
}
inline void SetCCR(uint8_t v) {
    cpu.X = v >> 4 & 1;
    cpu.N = v >> 3 & 1;
    cpu.Z = v >> 2 & 1;
    cpu.V = v >> 1 & 1;
    cpu.C = v & 1;
}
inline uint16_t GetSR() {
    return GetCCR() | cpu.I << 8 | cpu.M << 12 | cpu.S << 13 | cpu.T << 14;
}
inline void SaveSP() {
    if(cpu.S)
        if(cpu.M)
            cpu.MSP = cpu.A[7];
        else
            cpu.ISP = cpu.A[7];
    else
        cpu.USP = cpu.A[7];
}
inline void LoadSP() {
    if(cpu.S)
        if(cpu.M)
            cpu.A[7] = cpu.MSP;
        else
            cpu.A[7] = cpu.ISP;
    else
        cpu.A[7] = cpu.USP;
}
inline void SetSR(uint16_t v) {
    SaveSP();
    SetCCR(v);
    cpu.I = v >> 8 & 7;
    cpu.M = v >> 12 & 1;
    cpu.S = v >> 13 & 1;
    cpu.T = v >> 14 & 3;
    LoadSP();
}

inline void TEST_B(int8_t v) {
    cpu.N = v < 0;
    cpu.Z = v == 0;
}
inline void TEST_W(int16_t v) {
    cpu.N = v < 0;
    cpu.Z = v == 0;
}
inline void TEST_L(int32_t v) {
    cpu.N = v < 0;
    cpu.Z = v == 0;
}

inline void STORE_B(uint32_t &v, uint8_t b) { v = (v & 0xffffff00) | b; }
inline void STORE_W(uint32_t &v, uint16_t w) { v = (v & 0xffff0000) | w; }
uint8_t AND_B(uint8_t a, uint8_t b);
uint16_t AND_W(uint16_t a, uint16_t b);
uint32_t AND_L(uint32_t a, uint32_t b);

uint8_t XOR_B(uint8_t a, uint8_t b);
uint16_t XOR_W(uint16_t a, uint16_t b);
uint32_t XOR_L(uint32_t a, uint32_t b);

uint8_t OR_B(uint8_t a, uint8_t b);
uint16_t OR_W(uint16_t a, uint16_t b);
uint32_t OR_L(uint32_t a, uint32_t b);



uint8_t NOT_B(uint8_t v);
uint16_t NOT_W(uint16_t v);
uint32_t NOT_L(uint32_t v);

uint8_t ADD_B(uint8_t a, uint8_t b);
uint16_t ADD_W(uint16_t a, uint16_t b);
uint32_t ADD_L(uint32_t a, uint32_t b);

uint8_t ADDX_B(uint8_t a, uint8_t b, bool old_x);
uint16_t ADDX_W(uint16_t a, uint16_t b, bool old_x);
uint32_t ADDX_L(uint32_t a, uint32_t b, bool old_x);

uint8_t SUB_B(uint8_t a, uint8_t b);
uint16_t SUB_W(uint16_t a, uint16_t b);
uint32_t SUB_L(uint32_t a, uint32_t b);

uint8_t SUBX_B(uint8_t a, uint8_t b, bool old_x);
uint16_t SUBX_W(uint16_t a, uint16_t b, bool old_x);
uint32_t SUBX_L(uint32_t a, uint32_t b, bool old_x);

void CMP_B(uint8_t a, uint8_t b);
void CMP_W(uint16_t a, uint16_t b);
void CMP_L(uint32_t a, uint32_t b);

void BTST_B(uint8_t v, uint8_t b);
void BTST_L(uint32_t v, uint8_t b);
uint8_t BCHG_B(uint8_t v, uint8_t b);
uint32_t BCHG_L(uint32_t v, uint8_t b);
uint8_t BCLR_B(uint8_t v, uint8_t b);
uint32_t BCLR_L(uint32_t v, uint8_t b);
uint8_t BSET_B(uint8_t v, uint8_t b);
uint32_t BSET_L(uint32_t v, uint8_t b);

uint64_t MULU_LL(uint32_t a, uint32_t b); 
int64_t MULS_LL(int32_t a, int32_t b);
uint32_t MULU_L(uint32_t a, uint32_t b) ;
int32_t MULS_L(int32_t a, int32_t b);
uint32_t MULU_W(uint16_t a, uint16_t b);
int32_t MULS_W(int16_t a, int16_t b);

uint8_t NEG_B(uint8_t v);
uint16_t NEG_W(uint16_t v);
uint32_t NEG_L(uint32_t v);
uint8_t NEGX_B(uint8_t v, bool old_x);
uint16_t NEGX_W(uint16_t v, bool old_x);
uint32_t NEGX_L(uint32_t v, bool old_x);

std::pair<uint32_t, uint32_t> DIVU_LL(uint64_t a, uint32_t b);
std::pair<int32_t, int32_t> DIVS_LL(int64_t a, int32_t b);
std::pair<uint32_t, uint32_t> DIVU_L(uint32_t a, uint32_t b);

std::pair<int32_t, int32_t> DIVS_L(int32_t a, int32_t b);
std::pair<uint16_t, uint16_t> DIVU_W(uint32_t a, uint16_t b);
std::pair<int16_t, int16_t> DIVS_W(int32_t a, int16_t b);

int8_t ASR_B(int8_t v, uint8_t c);
int16_t ASR_W(int16_t v, uint8_t c);
int32_t ASR_L(int32_t v, uint8_t c);

int8_t ASL_B(int8_t v, uint8_t c);
int16_t ASL_W(int16_t v, uint8_t c);
int32_t ASL_L(int32_t v, uint8_t c);

uint8_t LSR_B(uint8_t v, uint8_t c);
uint16_t LSR_W(uint16_t v, uint8_t c);
uint32_t LSR_L(uint32_t v, uint8_t c);

uint8_t LSL_B(uint8_t v, uint8_t c);
uint16_t LSL_W(uint16_t v, uint8_t c);
uint32_t LSL_L(uint32_t v, uint8_t c);


uint8_t ROR_B(uint8_t v, uint8_t c) ;
uint16_t ROR_W(uint16_t v, uint8_t c) ;
uint32_t ROR_L(uint32_t v, uint8_t c) ;

uint8_t ROL_B(uint8_t v, uint8_t c) ;
uint16_t ROL_W(uint16_t v, uint8_t c) ;
uint32_t ROL_L(uint32_t v, uint8_t c) ;

uint8_t ROXR_B(uint8_t v, uint8_t c, bool old_x) ;
uint16_t ROXR_W(uint16_t v, uint8_t c, bool old_x) ;
uint32_t ROXR_L(uint32_t v, uint8_t c, bool old_x) ;

uint8_t ROXL_B(uint8_t v, uint8_t c, bool old_x) ;
uint16_t ROXL_W(uint16_t v, uint8_t c, bool old_x) ;
uint32_t ROXL_L(uint32_t v, uint8_t c, bool old_x) ;

int32_t BFTST_D(uint32_t v, int offset, int width);
int32_t BFTST_M(uint32_t addr, int offset, int width);
uint32_t BFCHG_D(uint32_t v, int offset, int width);
void BFCHG_M(uint32_t addr, int offset, int width);
uint32_t BFCLR_D(uint32_t v, int offset, int width);
void BFCLR_M(uint32_t addr, int offset, int width);
uint32_t BFSET_D(uint32_t v, int offset, int width);
void BFSET_M(uint32_t addr, int offset, int width);
uint32_t BFEXTU_D(uint32_t v, int offset, int width);
uint32_t BFEXTU_M(uint32_t addr, int offset, int width);
int32_t BFEXTS_D(uint32_t v, int offset, int width);
int32_t BFEXTS_M(uint32_t addr, int offset, int width);
int32_t BFFFO_D(uint32_t v, int offset, int width);
int32_t BFFFO_M(uint32_t addr, int offset, int width);
uint32_t BFINS_D(uint32_t old, int offset, uint8_t width, uint32_t v);
void BFINS_M(uint32_t addr, int offset, uint8_t width, uint32_t v);

void CAS_B(uint32_t addr, int dc, uint8_t du);
void CAS_W(uint32_t addr, int dc, uint16_t du);
void CAS_L(uint32_t addr, int dc, uint32_t du);

void CAS2_W(uint32_t addr1, int dc1, uint16_t du1, uint32_t addr2, int dc2,
            uint16_t du2);
void CAS2_L(uint32_t addr1, int dc1, uint32_t du1, uint32_t addr2, int dc2,
            uint32_t du2);

void CHK_W(int16_t v, int16_t range);
void CHK_L(int32_t v, int32_t range);

void CMP2_B(uint8_t v, uint32_t rangeAddr);
void CMP2_SB(int32_t v, uint32_t rangeAddr);
void CMP2_W(uint16_t v, uint32_t rangeAddr);
void CMP2_SW(int32_t v, uint32_t rangeAddr);
void CMP2_L(uint32_t v, uint32_t rangeAddr);
void CMP2_SL(int32_t v, uint32_t rangeAddr);

void do_bra(int32_t ix);
void do_bsr(int32_t ix, int off);
void do_bcc(bool cond, int ix) ;
void do_dbcc(bool cond, int reg, int16_t d);
void do_rtd(int16_t disp);
void do_rtr();
void do_rts();

void save_register(uint16_t regs, bool inv = false) ;
void MOVEM_W_TO_MEM_DECR(uint16_t regs, uint8_t reg);
void MOVEM_W_TO_MEM_ADDR(uint16_t regs);
void MOVEM_W_FROM_MEM_INCR(uint16_t regs, uint8_t reg);
void MOVEM_W_FROM_MEM_ADDR(uint16_t regs);
void MOVEM_L_TO_MEM_DECR(uint16_t regs, uint8_t reg);
void MOVEM_L_TO_MEM_ADDR(uint16_t regs);
void MOVEM_L_FROM_MEM_INCR(uint16_t regs, uint8_t reg);
void MOVEM_L_FROM_MEM_ADDR(uint16_t regs);

uint8_t do_abcd(uint8_t v1, uint8_t v2, bool old_x);
uint8_t do_nbcd(uint8_t v1, bool old_x);
uint8_t do_pack(uint16_t v, int16_t adj);
uint8_t do_sbcd(uint8_t v1, uint8_t v2, bool old_x);

#endif
