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
    cpu.PC = next;
    if( next & 1) {
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
inline uint8_t AND_B(uint8_t a, uint8_t b) {
    uint8_t re = a & b;
    TEST_B(re);
    cpu.V = false;
    cpu.C = false;
    return re;
}

inline uint8_t OR_B(uint8_t a, uint8_t b) {
    uint8_t re = a | b;
    TEST_B(re);
    cpu.V = false;
    cpu.C = false;
    return re;
}

inline uint8_t XOR_B(uint8_t a, uint8_t b) {
    uint8_t re = a ^ b;
    TEST_B(re);
    cpu.V = false;
    cpu.C = false;
    return re;
}

inline uint16_t AND_W(uint16_t a, uint16_t b) {
    uint16_t re = a & b;
    TEST_W(re);
    cpu.V = false;
    cpu.C = false;
    return re;
}

inline uint16_t OR_W(uint16_t a, uint16_t b) {
    uint16_t re = a | b;
    TEST_W(re);
    cpu.V = false;
    cpu.C = false;
    return re;
}

inline uint16_t XOR_W(uint16_t a, uint16_t b) {
    uint16_t re = a ^ b;
    TEST_W(re);
    cpu.V = false;
    cpu.C = false;
    return re;
}

inline uint32_t AND_L(uint32_t a, uint32_t b) {
    uint32_t re = a & b;
    TEST_L(re);
    cpu.V = false;
    cpu.C = false;
    return re;
}

inline uint32_t OR_L(uint32_t a, uint32_t b) {
    uint32_t re = a | b;
    TEST_L(re);
    cpu.V = false;
    cpu.C = false;
    return re;
}

inline uint32_t XOR_L(uint32_t a, uint32_t b) {
    uint32_t re = a ^ b;
    TEST_L(re);
    cpu.V = false;
    cpu.C = false;
    return re;
}

inline void TEST_CV_B(uint32_t v) {
    cpu.V = static_cast<int32_t>(v) > std::numeric_limits<int8_t>::max() ||
            static_cast<int32_t>(v) < std::numeric_limits<int8_t>::min();
    cpu.C = v >> 8 & 1;
}

inline void TEST_CV_W(uint32_t v) {
    cpu.V = static_cast<int32_t>(v) > std::numeric_limits<int16_t>::max() ||
            static_cast<int32_t>(v) < std::numeric_limits<int16_t>::min();
    cpu.C = v >> 16 & 1;
}

inline uint8_t SUB_B(uint8_t a, uint8_t b) {
    uint32_t ret = static_cast<int8_t>(a);
    ret -= static_cast<int8_t>(b);
    TEST_CV_B(ret);
    cpu.X = cpu.C;
    TEST_B(ret);
    return ret;
}

inline uint16_t SUB_W(uint16_t a, uint16_t b) {
    uint32_t ret = static_cast<int16_t>(a);
    ret -= static_cast<int16_t>(b);
    TEST_CV_W(ret);
    cpu.X = cpu.C;
    TEST_W(ret);
    return ret;
}

inline uint32_t SUB_L(uint32_t a, uint32_t b) {
    uint32_t ret;
    cpu.V = __builtin_sub_overflow_p(int32_t(a), int32_t(b), int32_t(0));
    cpu.X = cpu.C = __builtin_sub_overflow(a, b, &ret);
    TEST_L(ret);
    return ret;
}

inline uint8_t ADD_B(uint8_t a, uint8_t b) {
    uint32_t ret = a;
    ret += b;
    TEST_CV_B(ret);
    cpu.X = cpu.C;
    TEST_B(ret);
    return ret;
}

inline uint16_t ADD_W(uint16_t a, uint16_t b) {
    uint32_t ret = a;
    ret += b;
    TEST_CV_W(ret);
    cpu.X = cpu.C;
    TEST_W(ret);
    return ret;
}

inline uint32_t ADD_L(uint32_t a, uint32_t b) {
    uint32_t ret;
    cpu.V = __builtin_add_overflow_p(int32_t(a), int32_t(b), int32_t(0));
    cpu.X = cpu.C = __builtin_add_overflow(a, b, &ret);
    TEST_L(ret);
    return ret;
}

inline void CMP_B(uint8_t a, uint8_t b) {
    uint32_t ret = a;
    ret -= b;
    TEST_CV_B(ret);
    TEST_B(ret);
}

inline void CMP_W(uint16_t a, uint16_t b) {
    uint32_t ret = a;
    ret -= b;
    TEST_CV_W(ret);
    TEST_W(ret);
}

inline void CMP_L(uint32_t a, uint32_t b) {
    uint32_t ret;
    cpu.V = __builtin_sub_overflow_p(int32_t(a), int32_t(b), int32_t(0));
    cpu.C = __builtin_sub_overflow(a, b, &ret);
    TEST_L(ret);
}

inline void BTST_B(uint8_t v, uint8_t b) { cpu.Z = !(v >> b & 1); }
inline void BTST_L(uint32_t v, uint8_t b) { cpu.Z = !(v >> b & 1); }
inline uint8_t BCHG_B(uint8_t v, uint8_t b) {
    BTST_B(v, b);
    return v ^ (1 << b);
}
inline uint32_t BCHG_L(uint32_t v, uint8_t b) {
    BTST_L(v, b);
    return v ^ (1 << b);
}
inline uint8_t BCLR_B(uint8_t v, uint8_t b) {
    BTST_B(v, b);
    return v & ~(1 << b);
}
inline uint32_t BCLR_L(uint32_t v, uint8_t b) {
    BTST_L(v, b);
    return v & ~(1 << b);
}
inline uint8_t BSET_B(uint8_t v, uint8_t b) {
    BTST_B(v, b);
    return v |(1 << b);
}
inline uint32_t BSET_L(uint32_t v, uint8_t b) {
    BTST_L(v, b);
    return v |(1 << b);
}
inline int BCD2BIN(uint8_t v) { return (v >> 4) * 10 + (v & 0xf); }
inline uint8_t BIN2BCD(int v) { return (v / 10) << 4 | (v % 10); }

inline uint64_t MULU_LL(uint32_t a, uint32_t b) {
    uint64_t v = static_cast<uint64_t>(a) * b;
    cpu.V = cpu.C = false;
    cpu.Z = (v == 0);
    cpu.N = (static_cast<int64_t>(v) < 0);
    return v;
}
inline int64_t MULS_LL(int32_t a, int32_t b) {
    int64_t v = static_cast<int64_t>(a) * b;
    cpu.V = cpu.C = false;
    cpu.Z = (v == 0);
    cpu.N = (v < 0);
    return v;
}

inline uint32_t MULU_L(uint32_t a, uint32_t b) {
    uint32_t v;
    cpu.C = false;
    cpu.V = __builtin_mul_overflow(a, b, &v);
    cpu.Z = (v == 0);
    cpu.N = (v >> 31);
    return v;
}
inline int32_t MULS_L(int32_t a, int32_t b) {
    int32_t v;
    cpu.C = false;
    cpu.V = __builtin_mul_overflow(a, b, &v);
    cpu.Z = (v == 0);
    cpu.N = (v < 0);
    return v;
}

inline uint32_t MULU_W(uint16_t a, uint16_t b) {
    uint32_t v = static_cast<uint32_t>(a) * b;
    cpu.C = cpu.V = false;
    cpu.Z = (v == 0);
    cpu.N = (v >> 31);
    return v;
}
inline int32_t MULS_W(int16_t a, int16_t b) {
    int32_t v = static_cast<int16_t>(a) * b;
    cpu.C = cpu.V = false;
    cpu.Z = (v == 0);
    cpu.N = (v < 0);
    return v;
}

inline std::pair<uint32_t, uint32_t> DIVU_LL(uint64_t a, uint32_t b) {
    if(b == 0) {
        DIV0_ERROR();
    }
    uint64_t q = a / b;
    uint32_t r = a % b;
    cpu.C = false;
    cpu.V = q >> 32;
    cpu.Z = q == 0;
    cpu.N = q >> 31;
    return {static_cast<uint32_t>(q), r};
}

inline std::pair<int32_t, int32_t> DIVS_LL(int64_t a, int32_t b) {
    if(b == 0) {
        DIV0_ERROR();
    }
    int64_t q = a / b;
    int32_t r = a % b;
    cpu.C = false;
    cpu.V = q > std::numeric_limits<int32_t>::max() ||
            q < std::numeric_limits<int32_t>::min();
    cpu.Z = q == 0;
    cpu.N = q >> 31;
    return {static_cast<uint32_t>(q), r};
}

inline std::pair<uint32_t, uint32_t> DIVU_L(uint32_t a, uint32_t b) {
    if(b == 0) {
        DIV0_ERROR();
    }
    uint32_t q = a / b;
    uint32_t r = a % b;
    cpu.C = false;
    cpu.V = false;
    cpu.Z = q == 0;
    cpu.N = q >> 31;
    return {q, r};
}

inline std::pair<int32_t, int32_t> DIVS_L(int32_t a, int32_t b) {
    if(b == 0) {
        DIV0_ERROR();
    }
    if(a == 0x7fffffff && b == -1) {
        cpu.V = true;
        return {0, 0};
    }
    int32_t q = a / b;
    int32_t r = a % b;
    cpu.C = cpu.V = false;
    cpu.Z = q == 0;
    cpu.N = q < 0;
    return {q, r};
}

inline std::pair<uint16_t, uint16_t> DIVU_W(uint32_t a, uint16_t b) {
    if(b == 0) {
        DIV0_ERROR();
    }
    uint32_t q = a / b;
    uint16_t r = a % b;
    cpu.C = false;
    cpu.V = q > 0xffff;
    cpu.Z = q == 0;
    cpu.N = q >> 15;
    return {static_cast<uint16_t>(q), r};
}

inline std::pair<int16_t, int16_t> DIVS_W(int32_t a, int16_t b) {
    if(b == 0) {
        DIV0_ERROR();
    }
    if(a == 0x7fffffff && b == -1) {
        cpu.V = true;
        return {0, 0};
    }
    int32_t q = a / b;
    int16_t r = a % b;
    cpu.C = false;
    cpu.V = q > std::numeric_limits<int16_t>::max() ||
            q < std::numeric_limits<int16_t>::min();
    cpu.Z = q == 0;
    cpu.N = q < 0;
    return {static_cast<int16_t>(q), r};
}

inline int8_t ASR_B(int8_t v, uint8_t c) {
    int8_t re = v >> c;
    cpu.V = false;
    cpu.C = c == 0 ? false : (v >> (c - 1)) & 1;
    TEST_B(re);
    if(c) {
        cpu.X = cpu.C;
    }
    return re;
}
inline int16_t ASR_W(int16_t v, uint8_t c) {
    int16_t re = v >> c;
    cpu.V = false;
    cpu.C = c == 0 ? false : (v >> (c - 1)) & 1;
    TEST_W(re);
    if(c) {
        cpu.X = cpu.C;
    }
    return re;
}
inline int32_t ASR_L(int32_t v, uint8_t c) {
    int32_t re = v >> c;
    cpu.V = false;
    cpu.C = c == 0 ? false : (v >> (c - 1)) & 1;
    TEST_L(re);
    if(c) {
        cpu.X = cpu.C;
    }
    return re;
}

inline int8_t ASL_B(int8_t v, uint8_t c) {
    int8_t re = v << c;
    cpu.C = c == 0 ? false : (v << (c - 1)) & 0x80;
    TEST_B(re);
    if(c) {
        cpu.X = cpu.C;
    }
    int8_t so = v >> (7 - c);
    cpu.V = so != (v >> 7);
    return re;
}

inline int16_t ASL_W(int16_t v, uint8_t c) {
    int16_t re = v << c;
    cpu.C = c == 0 ? false : (v << (c - 1)) & 0x8000;
    TEST_W(re);
    if(c) {
        cpu.X = cpu.C;
    }
    int16_t so = v >> (15 - c);
    cpu.V = so != (v >> 15);
    return re;
}

inline int32_t ASL_L(int32_t v, uint8_t c) {
    int32_t re = v << c;
    cpu.C = c == 0 ? false : (v << (c - 1)) & 0x80000000;
    TEST_L(re);
    if(c) {
        cpu.X = cpu.C;
    }
    int16_t so = v >> (31 - c);
    cpu.V = so != (v >> 31);
    return re;
}

inline uint8_t LSR_B(uint8_t v, uint8_t c) {
    uint8_t re = v >> c;
    cpu.V = false;
    cpu.C = (v >> (c - 1)) & 1;
    TEST_B(re);
    if(c) {
        cpu.X = cpu.C;
    }
    return re;
}
inline uint16_t LSR_W(uint16_t v, uint8_t c) {
    uint16_t re = v >> c;
    cpu.V = false;
    cpu.C = (v >> (c - 1)) & 1;
    TEST_W(re);
    if(c) {
        cpu.X = cpu.C;
    }
    return re;
}
inline uint32_t LSR_L(uint32_t v, uint8_t c) {
    uint32_t re = v >> c;
    cpu.V = false;
    cpu.C = (v >> (c - 1)) & 1;
    TEST_L(re);
    if(c) {
        cpu.X = cpu.C;
    }
    return re;
}

inline uint8_t LSL_B(uint8_t v, uint8_t c) {
    uint8_t re = v << c;
    cpu.C = (v << (c - 1)) & 0x80;
    TEST_B(re);
    if(c) {
        cpu.X = cpu.C;
    }
    cpu.V = false;
    return re;
}

inline uint16_t LSL_W(uint16_t v, uint8_t c) {
    uint16_t re = v << c;
    cpu.C = (v << (c - 1)) & 0x8000;
    TEST_W(re);
    if(c) {
        cpu.X = cpu.C;
    }
    cpu.V = false;
    return re;
}

inline uint32_t LSL_L(int32_t v, uint8_t c) {
    uint32_t re = v << c;
    cpu.C = (v << (c - 1)) & 0x80000000;
    TEST_L(re);
    if(c) {
        cpu.X = cpu.C;
    }
    cpu.V = false;
    return re;
}

inline uint8_t ROXR_B(uint8_t v, uint8_t c) {
    cpu.V = false;
    if(c == 0) {
        TEST_B(v);
        cpu.C = cpu.X;
        return v;
    }
    uint8_t re = v >> c | cpu.X << (8 - c) | v << (9 - c);
    cpu.X = cpu.C = v >> (c - 1) & 1;
    TEST_B(re);
    return re;
}

inline uint16_t ROXR_W(uint16_t v, uint8_t c) {
    cpu.V = false;
    if(c == 0) {
        TEST_W(v);
        cpu.C = cpu.X;
        return v;
    }
    uint16_t re = v >> c | cpu.X << (16 - c) | v << (17 - c);
    cpu.X = cpu.C = v >> (c - 1) & 1;
    TEST_W(re);
    return re;
}
inline uint32_t ROXR_L(uint32_t v, uint8_t c) {
    cpu.V = false;
    if(c == 0) {
        TEST_L(v);
        cpu.C = cpu.X;
        return v;
    }
    uint32_t re = v >> c | cpu.X << (32 - c) | v << (33 - c);
    cpu.X = cpu.C = v >> (c - 1) & 1;
    TEST_L(re);
    return re;
}

inline uint8_t ROXL_B(uint8_t v, uint8_t c) {
    cpu.V = false;
    if(c == 0) {
        TEST_B(v);
        cpu.C = cpu.X;
        return v;
    }
    uint8_t re = v << c | cpu.X << (c - 1) | v >> (9 - c);
    cpu.X = cpu.C = v >> (8 - c) & 1;
    TEST_B(re);
    return re;
}

inline uint16_t ROXL_W(uint16_t v, uint8_t c) {
    cpu.V = false;
    if(c == 0) {
        TEST_W(v);
        cpu.C = cpu.X;
        return v;
    }
    uint16_t re = v << c | cpu.X << (c - 1) | v >> (17 - c);
    cpu.X = cpu.C = v >> (16 - c) & 1;
    TEST_W(re);
    return re;
}

inline uint32_t ROXL_L(uint32_t v, uint8_t c) {
    cpu.V = false;
    if(c == 0) {
        TEST_L(v);
        cpu.C = cpu.X;
        return v;
    }
    uint32_t re = v << c | cpu.X << (c - 1) | v >> (33 - c);
    cpu.X = cpu.C = v >> (32 - c) & 1;
    TEST_L(re);
    return re;
}

inline uint8_t ROR_B(uint8_t v, uint8_t c) {
    cpu.V = false;
    if(c == 0) {
        TEST_B(v);
        cpu.C = false;
        return v;
    }
    uint8_t re = v >> c | v << (8 - c);
    cpu.C = v >> (c - 1) & 1;
    TEST_B(re);
    return re;
}

inline uint16_t ROR_W(uint16_t v, uint8_t c) {
    cpu.V = false;
    if(c == 0) {
        TEST_W(v);
        cpu.C = false;
        return v;
    }
    uint16_t re = v >> c | v << (16 - c);
    cpu.C = v >> (c - 1) & 1;
    TEST_W(re);
    return re;
}
inline uint32_t ROR_L(uint32_t v, uint8_t c) {
    cpu.V = false;
    if(c == 0) {
        TEST_L(v);
        cpu.C = false;
        return v;
    }
    uint32_t re = v >> c | v << (32 - c);
    cpu.C = v >> (c - 1) & 1;
    TEST_L(re);
    return re;
}

inline uint8_t ROL_B(uint8_t v, uint8_t c) {
    cpu.V = false;
    if(c == 0) {
        TEST_B(v);
        cpu.C = false;
        return v;
    }
    uint8_t re = v << c | v >> (8 - c);
    cpu.C = v >> (8 - c) & 1;
    TEST_B(re);
    return re;
}

inline uint16_t ROL_W(uint16_t v, uint8_t c) {
    cpu.V = false;
    if(c == 0) {
        TEST_W(v);
        cpu.C = false;
        return v;
    }
    uint16_t re = v << c | v >> (16 - c);
    cpu.C = v >> (16 - c) & 1;
    TEST_W(re);
    return re;
}

inline uint32_t ROL_L(uint32_t v, uint8_t c) {
    cpu.V = false;
    if(c == 0) {
        TEST_L(v);
        cpu.C = false;
        return v;
    }
    uint32_t re = v << c | v >> (32 - c);
    cpu.X = cpu.C = v >> (32 - c) & 1;
    TEST_L(re);
    return re;
}

inline int32_t BF_D(uint32_t v, int offset, uint8_t width) {
    return std::rotl(v, offset) & (-1 << (32 - width));
}


inline int32_t BF_M(uint32_t addr, int offset, uint8_t width) {
    addr += (offset >> 3);
    offset &= 7;
    uint32_t v = 0;    
    v |= MMU_ReadB(addr) << (24+offset);
    if(width+offset > 8) {
        v |= MMU_ReadB(addr+1) << (16+offset);
    }
    if(width+offset > 16) {
        v |= MMU_ReadB(addr+2) << (8+offset);
    }
    if(width+offset > 24) {
        v |= MMU_ReadB(addr+3) << offset;
    }
    if(width+offset > 32) {
        v |= MMU_ReadB(addr+4) >> (8-offset);
    }
    return v & (-1 << (32 - width));    
}


inline int32_t BFTST_D(uint32_t v, int offset, int width) {
    int32_t x = BF_D(v, offset, width);
    cpu.V = cpu.C = false;
    cpu.Z = x == 0;
    cpu.N = x < 0;
    return x;
}
inline int32_t BFTST_M(uint32_t addr, int offset, int width) {
    int32_t x = BF_M(addr, offset, width);
    cpu.V = cpu.C = false;
    cpu.Z = x == 0;
    cpu.N = x < 0;
    return x;
}

inline void BFCHG_D(uint32_t& v, int offset, int width) {
    BFTST_D(v, offset, width);
    uint32_t mask = std::rotr(static_cast<uint32_t>(-1 << (32-width)), offset);
    v ^= mask;
}
inline void BFCHG_M(uint32_t addr, int offset, int width) {
    BFTST_M(addr, offset, width);
    addr += (offset >> 3);
    offset &= 7;
    int ln = width + offset;
    if( ln <= 8) {
        uint8_t mask = -1 << (8-width);
        MMU_WriteB(addr, MMU_ReadB(addr) ^ (mask >> offset));
    } else if( ln <= 16) {
        // byte0
        uint8_t mask = -1 << offset;
        MMU_WriteB(addr, MMU_ReadB(addr) ^ (mask>>offset));
        // byte1
        mask = (-1 << (16-ln));
        MMU_WriteB(addr+1, MMU_ReadB(addr+1) ^ mask);
    } else if( ln <= 24) {
        // byte0
        uint8_t mask = -1 << offset;
        MMU_WriteB(addr, MMU_ReadB(addr) ^ (mask>>offset));
        // byte1
        MMU_WriteB(addr+1, ~MMU_ReadB(addr+1));
        // byte2
        mask = (-1 << (24-ln));
        MMU_WriteB(addr+2, MMU_ReadB(addr+2) ^ mask);
    }else if( width + offset <= 32) {
        // byte0
        uint8_t mask = (-1 << offset);
        MMU_WriteB(addr, MMU_ReadB(addr) ^ (mask>>offset));
        // byte1
        MMU_WriteB(addr+1, ~MMU_ReadB(addr+1));
        // byte2
        MMU_WriteB(addr+2, ~MMU_ReadB(addr+2));
        // byte3
        mask = (-1 << (32-ln));
        MMU_WriteB(addr+3, MMU_ReadB(addr+3) ^ mask);
    } else {
        // byte0
        uint8_t mask = (-1 << offset);
        MMU_WriteB(addr, MMU_ReadB(addr) ^ (mask>>offset));
       // byte1
        MMU_WriteB(addr+1, ~MMU_ReadB(addr+1));
        // byte2
        MMU_WriteB(addr+2, ~MMU_ReadB(addr+2));
       // byte3
        MMU_WriteB(addr+3, ~MMU_ReadB(addr+3));
        // byte4
        mask = (-1 << (40-ln));
        MMU_WriteB(addr+4, MMU_ReadB(addr+4) ^ mask);
    }
}

inline void BFCLR_D(uint32_t& v, int offset, int width) {
    BFTST_D(v, offset, width);
    uint32_t mask = std::rotr(static_cast<uint32_t>(-1 << (32-width)), offset);
    v &= ~mask;
}
inline void BFCLR_M(uint32_t addr, int offset, int width) {
    BFTST_M(addr, offset, width);
    addr += (offset >> 3);
    offset &= 7;
    int ln = width + offset;
    if( ln <= 8) {
        uint8_t mask = -1 << (8-width);
        MMU_WriteB(addr, MMU_ReadB(addr) &~ (mask >> offset));
    } else if( ln <= 16) {
        // byte0
        uint8_t mask = -1 << offset;
        MMU_WriteB(addr, MMU_ReadB(addr) &~ (mask>>offset));
        // byte1
        mask = (-1 << (16-ln));
        MMU_WriteB(addr+1, MMU_ReadB(addr+1) &~ mask);
    } else if( ln <= 24) {
        // byte0
        uint8_t mask = -1 << offset;
        MMU_WriteB(addr, MMU_ReadB(addr) &~ (mask>>offset));
        // byte1
        MMU_WriteB(addr+1, 0);
        // byte2
        mask = (-1 << (24-ln));
        MMU_WriteB(addr+2, MMU_ReadB(addr+2) &~ mask);
    }else if( width + offset <= 32) {
        // byte0
        uint8_t mask = (-1 << offset);
        MMU_WriteB(addr, MMU_ReadB(addr) &~ (mask>>offset));
        // byte1
        MMU_WriteB(addr+1, 0);
        // byte2
        MMU_WriteB(addr+2, 0);
        // byte3
        mask = (-1 << (32-ln));
        MMU_WriteB(addr+3, MMU_ReadB(addr+3) &~ mask);
    } else {
        // byte0
        uint8_t mask = (-1 << offset);
        MMU_WriteB(addr, MMU_ReadB(addr) &~ (mask>>offset));
       // byte1
        MMU_WriteB(addr+1, 0);
        // byte2
        MMU_WriteB(addr+2, 0);
       // byte3
        MMU_WriteB(addr+3, 0);
        // byte4
        mask = (-1 << (40-ln));
        MMU_WriteB(addr+4, MMU_ReadB(addr+4) &~ mask);
    }
}


inline void BFSET_D(uint32_t& v, int offset, int width) {
    BFTST_D(v, offset, width);
    uint32_t mask = std::rotr(static_cast<uint32_t>(-1 << (32-width)), offset);
    v |= mask;
}
inline void BFSET_M(uint32_t addr, int offset, int width) {
    BFTST_M(addr, offset, width);
    addr += (offset >> 3);
    offset &= 7;
    int ln = width + offset;
    if( ln <= 8) {
        uint8_t mask = -1 << (8-width);
        MMU_WriteB(addr, MMU_ReadB(addr) | (mask >> offset));
    } else if( ln <= 16) {
        // byte0
        uint8_t mask = -1 << offset;
        MMU_WriteB(addr, MMU_ReadB(addr) | (mask>>offset));
        // byte1
        mask = (-1 << (16-ln));
        MMU_WriteB(addr+1, MMU_ReadB(addr+1) | mask);
    } else if( ln <= 24) {
        // byte0
        uint8_t mask = -1 << offset;
        MMU_WriteB(addr, MMU_ReadB(addr) | (mask>>offset));
        // byte1
        MMU_WriteB(addr+1, 0xFF);
        // byte2
        mask = (-1 << (24-ln));
        MMU_WriteB(addr+2, MMU_ReadB(addr+2) | mask);
    }else if( width + offset <= 32) {
        // byte0
        uint8_t mask = (-1 << offset);
        MMU_WriteB(addr, MMU_ReadB(addr) | (mask>>offset));
        // byte1
        MMU_WriteB(addr+1, 0xFF);
        // byte2
        MMU_WriteB(addr+2, 0xFF);
        // byte3
        mask = (-1 << (32-ln));
        MMU_WriteB(addr+3, MMU_ReadB(addr+3) | mask);
    } else {
        // byte0
        uint8_t mask = (-1 << offset);
        MMU_WriteB(addr, MMU_ReadB(addr) | (mask>>offset));
       // byte1
        MMU_WriteB(addr+1, 0xFF);
        // byte2
        MMU_WriteB(addr+2, 0xFF);
       // byte3
        MMU_WriteB(addr+3, 0xFF);
        // byte4
        mask = (-1 << (40-ln));
        MMU_WriteB(addr+4, MMU_ReadB(addr+4) | mask);
    }
}


inline uint32_t BFEXTU_D(uint32_t v, int offset, int width) {
    uint32_t x = BFTST_D(v, offset, width);
    return x >> (32-width);
}
inline uint32_t BFEXTU_M(uint32_t addr, int offset, int width) {
    uint32_t x = BFTST_M(addr, offset, width);
    return x >> (32-width);
}

inline int32_t BFEXTS_D(uint32_t v, int offset, int width) {
    int32_t x = BFTST_D(v, offset, width);
    return x >> (32-width);
}
inline int32_t BFEXTS_M(uint32_t addr, int offset, int width) {
    int32_t x = BFTST_M(addr, offset, width);
    return x >> (32-width);
}

inline uint32_t BFFFO_D(uint32_t v, int offset, int width) {
    uint32_t x = BFTST_D(v, offset, width);
    return offset + width - __builtin_ffs(x >> (32-width));
}
inline uint32_t BFFFO_M(uint32_t addr, int offset, int width) {
    uint32_t x = BFTST_M(addr, offset, width) ;
    return offset + width - __builtin_ffs(x>> (32-width));
}

inline uint32_t BFINS_D(uint32_t old, int offset, uint8_t width, uint32_t v) {
    uint32_t mask = -1 >> (32-width);
    v &= mask;    
    uint32_t x = std::rotl(old, offset);
    x &= ~(-1<<(32-width));
    x |= v<<(32-width);
    return std::rotr(x, offset);
}

inline void BFINS_M(uint32_t addr, int offset, uint8_t width, uint32_t v) {
    addr += (offset >> 3);
    offset &= 7;
    int ln = width + offset;
    if( ln <= 8) {
        uint8_t mask = (-1 << (8-width));
        uint8_t old = MMU_ReadB(addr) &~ (mask >> offset);        
        MMU_WriteB(addr, old | (v & mask >> (8-width)));
    } else if( ln <= 16) {
        // byte0
        uint8_t v1 = v >> (ln-8);
        uint8_t mask = (-1 << offset);
        uint8_t old = MMU_ReadB(addr) &~ mask >> offset;
        MMU_WriteB(addr, old | v1);
        // byte1
        uint8_t v2 = v << (16-ln);
        mask = (-1 << (16-ln));
        old = MMU_ReadB(addr+1) &~ mask;
        MMU_WriteB(addr+1, old | v2);
    } else if( ln <= 24) {
        // byte0
        uint8_t v1 = v >> (ln-8);
        uint8_t mask = (-1 << offset);
        uint8_t old = MMU_ReadB(addr) &~ (mask >> offset);
        MMU_WriteB(addr, old | v1);
        // byte1
        uint8_t v2 = v >> (ln-16);
        MMU_WriteB(addr+1, v2);
        // byte2
        uint8_t v3 = v << (24-ln);
        mask = (-1 << (24-ln));
        old = MMU_ReadB(addr+2) &~ mask;
        MMU_WriteB(addr+2, old | v3);
    }else if( width + offset <= 32) {
        // byte0
        uint8_t v1 = v >> (ln-8);
        uint8_t mask = (-1 << offset);
        uint8_t old = MMU_ReadB(addr) &~ mask >> offset;
        MMU_WriteB(addr, old | v1);
        // byte1
        uint8_t v2 = v >> (ln-16);
        MMU_WriteB(addr+1, v2);
        // byte2
        uint8_t v3 = v >> (ln-24);
        MMU_WriteB(addr+2, v3);
        // byte3
        uint8_t v4 = v << (32-ln);
        mask = (-1 << (32-ln));
        old = MMU_ReadB(addr+3) &~ mask;
        MMU_WriteB(addr+3, old | v4);
    } else {
        // byte0
        uint8_t v1 = v >> (ln-8);
        uint8_t mask = (-1 << offset);
        uint8_t old = MMU_ReadB(addr) &~ mask >> offset;
        MMU_WriteB(addr, old | v1);
        // byte1
        uint8_t v2 = v >> (ln-16);
        MMU_WriteB(addr+1, v2);
        // byte2
        uint8_t v3 = v >> (ln-24);
        MMU_WriteB(addr+2, v3);
        // byte3
        uint8_t v4 = v >> (ln-32);
        MMU_WriteB(addr+3, v4);
        // byte4
        uint8_t v5 = v << (40-ln);
        mask = (-1 << (40-ln));
        old = MMU_ReadB(addr+4) &~ mask;
        MMU_WriteB(addr+4, old | v5);
    }
}
#endif
