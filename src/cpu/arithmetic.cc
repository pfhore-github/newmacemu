#include "68040.hpp"
#include "bus.hpp"
#include "exception.hpp"
#include "memory.hpp"
#include "inline.hpp"
#include <memory>
#include <utility>

uint8_t ADD_B(uint8_t a, uint8_t b) {
    uint16_t c = a + b;
    cpu.X = cpu.C = c & 0x100;
    cpu.V = ((~(a ^ b)) & (a ^ c)) & 0x80;
    TEST_B(c);
    return c;
}

uint16_t ADD_W(uint16_t a, uint16_t b) {
    uint32_t c = a + b;
    cpu.X = cpu.C = c & 0x10000;
    cpu.V = ((~(a ^ b)) & (a ^ c)) & 0x8000;
    TEST_W(c);
    return c;
}

uint32_t ADD_L(uint32_t a, uint32_t b) {
    uint64_t ret = static_cast<uint64_t>(a) + static_cast<uint64_t>(b);
    cpu.X = cpu.C = ret & 0x100000000LLU;
    cpu.V = ((~(a ^ b)) & (a ^ ret)) & 0x80000000;
    TEST_L(ret);
    return ret;
}

uint8_t SUB_B(uint8_t a, uint8_t b) {
    uint16_t c = a - b;
    cpu.X = cpu.C = (c & 0x100);
    cpu.V = ((a ^ b) & ~(b ^ c)) & 0x80;
    TEST_B(c);
    return c;
}

uint16_t SUB_W(uint16_t a, uint16_t b) {
    uint32_t c = a - b;
    cpu.X = cpu.C = (c & 0x10000);
    cpu.V = ((a ^ b) & ~(b ^ c)) & 0x8000;
    TEST_W(c);
    return c;
}

uint32_t SUB_L(uint32_t a, uint32_t b) {
    uint64_t ret = static_cast<uint64_t>(a) - static_cast<uint64_t>(b);
    cpu.X = cpu.C = (ret & 0x100000000LLU);
    cpu.V = ((a ^ b) & ~(b ^ ret)) & 0x80000000;
    TEST_L(ret);
    return ret;
}

uint8_t SUBX_B(uint8_t a, uint8_t b, bool old_x) {
    bool zz = cpu.Z;
    uint8_t v = SUB_B(a, b + old_x);
    cpu.Z = v ? false : zz;
    return v;
}
uint16_t SUBX_W(uint16_t a, uint16_t b, bool old_x) {
    bool zz = cpu.Z;
    uint16_t v = SUB_W(a, b + old_x);
    cpu.Z = v ? false : zz;
    return v;
}
uint32_t SUBX_L(uint32_t a, uint32_t b, bool old_x) {
    bool zz = cpu.Z;
    uint32_t v = SUB_L(a, b + old_x);
    cpu.Z = v ? false : zz;
    return v;
}

uint8_t NEG_B(uint8_t v) { return SUB_B(0, v); }
uint16_t NEG_W(uint16_t v) { return SUB_W(0, v); }
uint32_t NEG_L(uint32_t v) { return SUB_L(0, v); }

uint8_t NEGX_B(uint8_t v, bool old_x) { return SUB_B(0, v + old_x); }
uint16_t NEGX_W(uint16_t v, bool old_x) { return SUB_W(0, v + old_x); }
uint32_t NEGX_L(uint32_t v, bool old_x) { return SUB_L(0, v + old_x); }

uint8_t ADDX_B(uint8_t a, uint8_t b, bool old_x) {
    bool zz = cpu.Z;
    uint8_t v = ADD_B(a, b + old_x);
    cpu.Z = v ? false : zz;
    return v;
}

uint16_t ADDX_W(uint16_t a, uint16_t b, bool old_x) {
    bool zz = cpu.Z;
    uint16_t v = ADD_W(a, b + old_x);
    cpu.Z = v ? false : zz;
    return v;
}

uint32_t ADDX_L(uint32_t a, uint32_t b, bool old_x) {
    bool zz = cpu.Z;
    uint32_t v = ADD_L(a, b + old_x);
    cpu.Z = v ? false : zz;
    return v;
}

void CMP_B(uint8_t a, uint8_t b) {
    uint16_t c = a - b;
    cpu.C = (c & 0x100);
    cpu.V = ((a ^ b) & ~(b ^ c)) & 0x80;
    TEST_B(c);
}

void CMP_W(uint16_t a, uint16_t b) {
    uint32_t c = a - b;
    cpu.C = (c & 0x10000);
    cpu.V = ((a ^ b) & ~(b ^ c)) & 0x8000;
    TEST_W(c);
}

void CMP_L(uint32_t a, uint32_t b) {
    uint64_t ret = static_cast<uint64_t>(a) - static_cast<uint64_t>(b);
    cpu.C = ret & 0x100000000LLU;
    cpu.V = ((a ^ b) & ~(b ^ ret)) & 0x80000000;
    TEST_L(ret);
}

uint64_t MULU_LL(uint32_t a, uint32_t b) {
    uint64_t v = static_cast<uint64_t>(a) * static_cast<uint64_t>(b);
    cpu.V = cpu.C = false;
    cpu.Z = (v == 0);
    cpu.N = (static_cast<int64_t>(v) < 0);
    return v;
}

int64_t MULS_LL(int32_t a, int32_t b) {
    int64_t v = static_cast<int64_t>(a) * static_cast<int64_t>(b);
    cpu.V = cpu.C = false;
    cpu.Z = (v == 0);
    cpu.N = (v < 0);
    return v;
}

uint32_t MULU_L(uint32_t a, uint32_t b) {
    uint64_t v = static_cast<uint64_t>(a) * static_cast<uint64_t>(b);
    cpu.C = false;
    cpu.V = !!(v >> 32);
    cpu.Z = (v == 0);
    cpu.N = (v >> 31);
    return v;
}

int32_t MULS_L(int32_t a, int32_t b) {
    int64_t v = static_cast<int64_t>(a) * static_cast<int64_t>(b);
    cpu.C = false;
    cpu.V = v < std::numeric_limits<int32_t>::min() ||
            v > std::numeric_limits<int32_t>::max();
    cpu.Z = (v == 0);
    cpu.N = (v < 0);
    return v;
}

uint32_t MULU_W(uint16_t a, uint16_t b) {
    uint32_t v = static_cast<uint32_t>(a) * b;
    cpu.C = cpu.V = false;
    cpu.Z = (v == 0);
    cpu.N = (v >> 31);
    return v;
}

int32_t MULS_W(int16_t a, int16_t b) {
    int32_t v = static_cast<int32_t>(a) * b;
    cpu.C = cpu.V = false;
    cpu.Z = (v == 0);
    cpu.N = (v < 0);
    return v;
}

std::tuple<uint32_t, uint32_t> DIVU_LL(uint64_t a, uint32_t b) {
    if(b == 0) {
        DIV0_ERROR();
    }
    uint64_t q = a / b;
    uint32_t r = a % b;
    cpu.C = false;
    cpu.V = q >> 32;
    TEST_L(q);
    return {static_cast<uint32_t>(q), r};
}

std::tuple<int32_t, int32_t> DIVS_LL(int64_t a, int32_t b) {
    if(b == 0) {
        DIV0_ERROR();
    }
    int64_t q = a / b;
    int32_t r = a % b;
    cpu.C = false;
    cpu.V = q > std::numeric_limits<int32_t>::max() ||
            q < std::numeric_limits<int32_t>::min();
    TEST_L(q);
    return {static_cast<int32_t>(q), r};
}

std::tuple<uint32_t, uint32_t> DIVU_L(uint32_t a, uint32_t b) {
    if(b == 0) {
        DIV0_ERROR();
    }
    uint32_t q = a / b;
    uint32_t r = a % b;
    cpu.C = false;
    cpu.V = false;
    TEST_L(q);
    return {q, r};
}

std::tuple<int32_t, int32_t> DIVS_L(int32_t a, int32_t b) {
    if(b == 0) {
        DIV0_ERROR();
    }
    if(a == static_cast<int32_t>(0x80000000) && b == -1) {
        cpu.V = true;
        return {0, 0};
    }
    int32_t q = a / b;
    int32_t r = a % b;
    cpu.C = cpu.V = false;
    TEST_L(q);
    return {q, r};
}

std::tuple<uint16_t, uint16_t> DIVU_W(uint32_t a, uint16_t b) {
    if(b == 0) {
        DIV0_ERROR();
    }
    uint32_t q = a / b;
    uint16_t r = a % b;
    cpu.C = false;
    cpu.V = q >> 16;
    TEST_W(q);
    return {static_cast<uint16_t>(q), r};
}

std::tuple<int16_t, int16_t> DIVS_W(int32_t a, int16_t b) {
    if(b == 0) {
        DIV0_ERROR();
    }
    if(a == static_cast<int32_t>(0x80000000) && b == -1) {
        cpu.V = true;
        return {0, 0};
    }
    int32_t q = a / b;
    int16_t r = a % b;
    cpu.C = false;
    cpu.V = q > std::numeric_limits<int16_t>::max() ||
            q < std::numeric_limits<int16_t>::min();
    TEST_W(q);
    return {static_cast<int16_t>(q), r};
}

void CAS_B(uint32_t addr, int dc, uint8_t du) {
    uint8_t v = ReadB(addr);
    CMP_B(v, cpu.D[dc]);
    if(cpu.Z) {
        WriteB(cpu.EA, du);
    } else {
        STORE_B(cpu.D[dc], v);
    }
    if(cpu.T == 1) {
        cpu.must_trace = true;
    }
}

void CAS_W(uint32_t addr, int dc, uint16_t du) {
    uint16_t v = ReadW(addr);
    CMP_W(v, cpu.D[dc]);
    if(cpu.Z) {
        WriteW(cpu.EA, du);
    } else {
        STORE_W(cpu.D[dc], v);
    }
    if(cpu.T == 1) {
        cpu.must_trace = true;
    }
}

void CAS_L(uint32_t addr, int dc, uint32_t du) {
    uint32_t v = ReadL(addr);
    CMP_L(v, cpu.D[dc]);
    if(cpu.Z) {
        WriteL(cpu.EA, du);
    } else {
        cpu.D[dc] = v;
    }
    if(cpu.T == 1) {
        cpu.must_trace = true;
    }
}

void CAS2_W(uint32_t addr1, int dc1, uint16_t du1, uint32_t addr2, int dc2,
            uint16_t du2) {
    uint16_t v1 = ReadW(addr1);
    uint16_t v2 = ReadW(addr2);
    CMP_W(v1, cpu.D[dc1]);
    if(cpu.Z) {
        CMP_W(v2, cpu.D[dc2]);
    }
    if(cpu.Z) {
        WriteW(addr1, du1);
        WriteW(addr2, du2);
    } else {
        STORE_W(cpu.D[dc1], v1);
        STORE_W(cpu.D[dc2], v2);
    }
    if(cpu.T == 1) {
        cpu.must_trace = true;
    }
}

void CAS2_L(uint32_t addr1, int dc1, uint32_t du1, uint32_t addr2, int dc2,
            uint32_t du2) {
    uint32_t v1 = ReadL(addr1);
    uint32_t v2 = ReadL(addr2);
    CMP_L(v1, cpu.D[dc1]);
    if(cpu.Z) {
        CMP_L(v2, cpu.D[dc2]);
    }
    if(cpu.Z) {
        WriteL(addr1, du1);
        WriteL(addr2, du2);
    } else {
        cpu.D[dc1] = v1;
        cpu.D[dc2] = v2;
    }
    if(cpu.T == 1) {
        cpu.must_trace = true;
    }
}

void CHK_W(int16_t v, int16_t range) {
    if(v >= 0 && v <= range) {
        return;
    }
    cpu.N = v < 0;
    CHK_ERROR();
}
void CHK_L(int32_t v, int32_t range) {
    if(v >= 0 && v <= range) {
        return;
    }
    cpu.N = v < 0;
    CHK_ERROR();
}

void CMP2_B(uint8_t v, uint32_t rangeAddr) {
    uint8_t l = ReadB(rangeAddr);
    uint8_t h = ReadB(rangeAddr + 1);
    cpu.Z = (l == v) || (h == v);
    cpu.C = (l > v) || (h < v);
}
void CMP2_SB(int8_t v, uint32_t rangeAddr) {
    int8_t l = ReadB(rangeAddr);
    int8_t h = ReadB(rangeAddr + 1);
    cpu.Z = (l == v) || (h == v);
    cpu.C = (l > v) || (h < v);
}
void CMP2_W(uint16_t v, uint32_t rangeAddr) {
    uint16_t l = ReadW(rangeAddr);
    uint16_t h = ReadW(rangeAddr + 2);
    cpu.Z = (l == v) || (h == v);
    cpu.C = (l > v) || (h < v);
}
void CMP2_SW(int16_t v, uint32_t rangeAddr) {
    int16_t l = ReadW(rangeAddr);
    int16_t h = ReadW(rangeAddr + 2);
    cpu.Z = (l == v) || (h == v);
    cpu.C = (l > v) || (h < v);
}
void CMP2_L(uint32_t v, uint32_t rangeAddr) {
    uint32_t l = ReadL(rangeAddr);
    uint32_t h = ReadL(rangeAddr + 4);
    cpu.Z = (l == v) || (h == v);
    cpu.C = (l > v) || (h < v);
}
void CMP2_SL(int32_t v, uint32_t rangeAddr) {
    int32_t l = ReadL(rangeAddr);
    int32_t h = ReadL(rangeAddr + 4);
    cpu.Z = (l == v) || (h == v);
    cpu.C = (l > v) || (h < v);
}
