#include "68040.hpp"
#include "bus.hpp"
#include "exception.hpp"
#include "proto.hpp"
#include "memory.hpp"
#include <memory>
#include <utility>


int32_t BF_D(uint32_t v, int offset, uint8_t width) {
    offset &= 31;
    return std::rotl(v, offset) & (-1 << (32 - width));
}

int32_t BF_M(uint32_t addr, int offset, uint8_t width) {
    addr += (offset >> 3);
    offset &= 7;
    uint32_t v = 0;
    v |= ReadB(addr) << (24 + offset);
    if(width + offset > 8) {
        v |= ReadB(addr + 1) << (16 + offset);
    }
    if(width + offset > 16) {
        v |= ReadB(addr + 2) << (8 + offset);
    }
    if(width + offset > 24) {
        v |= ReadB(addr + 3) << offset;
    }
    if(width + offset > 32) {
        v |= ReadB(addr + 4) >> (8 - offset);
    }
    return v & (-1 << (32 - width));
}

int32_t BFTST_D(uint32_t v, int offset, int width) {
    if( width == 0) {
        width = 32;
    }
    int32_t x = BF_D(v, offset, width);
    cpu.V = cpu.C = false;
    TEST_L(x);
    return x;
}
int32_t BFTST_M(uint32_t addr, int offset, int width) {
    if( width == 0) {
        width = 32;
    }
    int32_t x = BF_M(addr, offset, width);
    cpu.V = cpu.C = false;
    TEST_L(x);
    return x;
}

uint32_t BFCHG_D(uint32_t v, int offset, int width) {
    if( width == 0) {
        width = 32;
    }
    BFTST_D(v, offset, width);
    uint32_t mask =
        std::rotr(static_cast<uint32_t>(-1 << (32 - width)), offset);
    return v ^ mask;
}
void BFCHG_M(uint32_t addr, int offset, int width) {
    if( width == 0) {
        width = 32;
    }
    BFTST_M(addr, offset, width);
    addr += (offset >> 3);
    offset &= 7;
    int ln = width + offset;
    if(ln <= 8) {
        uint8_t mask = -1 << (8 - width);
        WriteB(addr, ReadB(addr) ^ (mask >> offset));
    } else if(ln <= 16) {
        // byte0
        uint8_t mask = -1 << offset;
        WriteB(addr, ReadB(addr) ^ (mask >> offset));
        // byte1
        mask = (-1 << (16 - ln));
        WriteB(addr + 1, ReadB(addr + 1) ^ mask);
    } else if(ln <= 24) {
        // byte0
        uint8_t mask = -1 << offset;
        WriteB(addr, ReadB(addr) ^ (mask >> offset));
        // byte1
        WriteB(addr + 1, ~ReadB(addr + 1));
        // byte2
        mask = (-1 << (24 - ln));
        WriteB(addr + 2, ReadB(addr + 2) ^ mask);
    } else if(width + offset <= 32) {
        // byte0
        uint8_t mask = (-1 << offset);
        WriteB(addr, ReadB(addr) ^ (mask >> offset));
        // byte1
        WriteB(addr + 1, ~ReadB(addr + 1));
        // byte2
        WriteB(addr + 2, ~ReadB(addr + 2));
        // byte3
        mask = (-1 << (32 - ln));
        WriteB(addr + 3, ReadB(addr + 3) ^ mask);
    } else {
        // byte0
        uint8_t mask = (-1 << offset);
        WriteB(addr, ReadB(addr) ^ (mask >> offset));
        // byte1
        WriteB(addr + 1, ~ReadB(addr + 1));
        // byte2
        WriteB(addr + 2, ~ReadB(addr + 2));
        // byte3
        WriteB(addr + 3, ~ReadB(addr + 3));
        // byte4
        mask = (-1 << (40 - ln));
        WriteB(addr + 4, ReadB(addr + 4) ^ mask);
    }
}

uint32_t BFCLR_D(uint32_t v, int offset, int width) {
    if( width == 0) {
        width = 32;
    }
    BFTST_D(v, offset, width);
    uint32_t mask =
        std::rotr(static_cast<uint32_t>(-1 << (32 - width)), offset);
    return v & ~mask;
}
void BFCLR_M(uint32_t addr, int offset, int width) {
    if( width == 0) {
        width = 32;
    }
    BFTST_M(addr, offset, width);
    addr += (offset >> 3);
    offset &= 7;
    int ln = width + offset;
    if(ln <= 8) {
        uint8_t mask = -1 << (8 - width);
        WriteB(addr, ReadB(addr) & ~(mask >> offset));
    } else if(ln <= 16) {
        // byte0
        uint8_t mask = -1 << offset;
        WriteB(addr, ReadB(addr) & ~(mask >> offset));
        // byte1
        mask = (-1 << (16 - ln));
        WriteB(addr + 1, ReadB(addr + 1) & ~mask);
    } else if(ln <= 24) {
        // byte0
        uint8_t mask = -1 << offset;
        WriteB(addr, ReadB(addr) & ~(mask >> offset));
        // byte1
        WriteB(addr + 1, 0);
        // byte2
        mask = (-1 << (24 - ln));
        WriteB(addr + 2, ReadB(addr + 2) & ~mask);
    } else if(width + offset <= 32) {
        // byte0
        uint8_t mask = (-1 << offset);
        WriteB(addr, ReadB(addr) & ~(mask >> offset));
        // byte1
        WriteB(addr + 1, 0);
        // byte2
        WriteB(addr + 2, 0);
        // byte3
        mask = (-1 << (32 - ln));
        WriteB(addr + 3, ReadB(addr + 3) & ~mask);
    } else {
        // byte0
        uint8_t mask = (-1 << offset);
        WriteB(addr, ReadB(addr) & ~(mask >> offset));
        // byte1
        WriteB(addr + 1, 0);
        // byte2
        WriteB(addr + 2, 0);
        // byte3
        WriteB(addr + 3, 0);
        // byte4
        mask = (-1 << (40 - ln));
        WriteB(addr + 4, ReadB(addr + 4) & ~mask);
    }
}

uint32_t BFSET_D(uint32_t v, int offset, int width) {
    if( width == 0) {
        width = 32;
    }
    BFTST_D(v, offset, width);
    uint32_t mask =
        std::rotr(static_cast<uint32_t>(-1 << (32 - width)), offset);
    return v | mask;
}
void BFSET_M(uint32_t addr, int offset, int width) {
    if( width == 0) {
        width = 32;
    }
    BFTST_M(addr, offset, width);
    addr += (offset >> 3);
    offset &= 7;
    int ln = width + offset;
    if(ln <= 8) {
        uint8_t mask = -1 << (8 - width);
        WriteB(addr, ReadB(addr) | (mask >> offset));
    } else if(ln <= 16) {
        // byte0
        uint8_t mask = -1 << offset;
        WriteB(addr, ReadB(addr) | (mask >> offset));
        // byte1
        mask = (-1 << (16 - ln));
        WriteB(addr + 1, ReadB(addr + 1) | mask);
    } else if(ln <= 24) {
        // byte0
        uint8_t mask = -1 << offset;
        WriteB(addr, ReadB(addr) | (mask >> offset));
        // byte1
        WriteB(addr + 1, 0xFF);
        // byte2
        mask = (-1 << (24 - ln));
        WriteB(addr + 2, ReadB(addr + 2) | mask);
    } else if(width + offset <= 32) {
        // byte0
        uint8_t mask = (-1 << offset);
        WriteB(addr, ReadB(addr) | (mask >> offset));
        // byte1
        WriteB(addr + 1, 0xFF);
        // byte2
        WriteB(addr + 2, 0xFF);
        // byte3
        mask = (-1 << (32 - ln));
        WriteB(addr + 3, ReadB(addr + 3) | mask);
    } else {
        // byte0
        uint8_t mask = (-1 << offset);
        WriteB(addr, ReadB(addr) | (mask >> offset));
        // byte1
        WriteB(addr + 1, 0xFF);
        // byte2
        WriteB(addr + 2, 0xFF);
        // byte3
        WriteB(addr + 3, 0xFF);
        // byte4
        mask = (-1 << (40 - ln));
        WriteB(addr + 4, ReadB(addr + 4) | mask);
    }
}

uint32_t BFEXTU_D(uint32_t v, int offset, int width) {
    if( width == 0) {
        width = 32;
    }
    uint32_t x = BFTST_D(v, offset, width);
    return x >> (32 - width);
}
uint32_t BFEXTU_M(uint32_t addr, int offset, int width) {
    if( width == 0) {
        width = 32;
    }
    uint32_t x = BFTST_M(addr, offset, width);
    return x >> (32 - width);
}

int32_t BFEXTS_D(uint32_t v, int offset, int width) {
    if( width == 0) {
        width = 32;
    }
    int32_t x = BFTST_D(v, offset, width);
    return x >> (32 - width);
}
int32_t BFEXTS_M(uint32_t addr, int offset, int width) {
    if( width == 0) {
        width = 32;
    }
    int32_t x = BFTST_M(addr, offset, width);
    return x >> (32 - width);
}

int32_t BFFFO_D(uint32_t v, int offset, int width) {
    if( width == 0) {
        width = 32;
    }
    uint32_t x = BFTST_D(v, offset, width);
    return offset + (x ? std::countl_zero(x) : width);
}
int32_t BFFFO_M(uint32_t addr, int offset, int width) {
    if( width == 0) {
        width = 32;
    }
    uint32_t x = BFTST_M(addr, offset, width);
    return offset + (x ? std::countl_zero(x) : width);
}

uint32_t BFINS_D(uint32_t old, int offset, uint8_t width, uint32_t v) {
    if( width == 0) {
        width = 32;
    }
    TEST_L(v << (32 - width));
    cpu.V = false;
    cpu.C = false;
    uint32_t mask = -1 >> (32 - width);
    v &= mask;
    uint32_t x = std::rotl(old, offset);
    x &= ~(-1 << (32 - width));
    x |= v << (32 - width);
    return std::rotr(x, offset);
}

void BFINS_M(uint32_t addr, int offset, uint8_t width, uint32_t v) {
    if( width == 0) {
        width = 32;
    }
    addr += (offset >> 3);
    offset &= 7;
    int ln = width + offset;
    if(ln <= 8) {
        uint8_t mask = static_cast<uint8_t>(-1 << (8 - width)) >> offset;
        uint8_t old = ReadB(addr) & ~mask ;
        WriteB(addr, old | ((v << (8-ln)) & mask));
    } else if(ln <= 16) {
        // byte0
        uint8_t v1 = v >> (ln - 8);
        uint8_t mask = (-1 << offset);
        uint8_t old = ReadB(addr) & ~mask >> offset;
        WriteB(addr, old | v1);
        // byte1
        uint8_t v2 = v << (16 - ln);
        mask = (-1 << (16 - ln));
        old = ReadB(addr + 1) & ~mask;
        WriteB(addr + 1, old | v2);
    } else if(ln <= 24) {
        // byte0
        uint8_t v1 = v >> (ln - 8);
        uint8_t mask = (-1 << offset);
        uint8_t old = ReadB(addr) & ~(mask >> offset);
        WriteB(addr, old | v1);
        // byte1
        uint8_t v2 = v >> (ln - 16);
        WriteB(addr + 1, v2);
        // byte2
        uint8_t v3 = v << (24 - ln);
        mask = (-1 << (24 - ln));
        old = ReadB(addr + 2) & ~mask;
        WriteB(addr + 2, old | v3);
    } else if(width + offset <= 32) {
        // byte0
        uint8_t v1 = v >> (ln - 8);
        uint8_t mask = (-1 << offset);
        uint8_t old = ReadB(addr) & ~mask >> offset;
        WriteB(addr, old | v1);
        // byte1
        uint8_t v2 = v >> (ln - 16);
        WriteB(addr + 1, v2);
        // byte2
        uint8_t v3 = v >> (ln - 24);
        WriteB(addr + 2, v3);
        // byte3
        uint8_t v4 = v << (32 - ln);
        mask = (-1 << (32 - ln));
        old = ReadB(addr + 3) & ~mask;
        WriteB(addr + 3, old | v4);
    } else {
        // byte0
        uint8_t v1 = v >> (ln - 8);
        uint8_t mask = (-1 << offset);
        uint8_t old = ReadB(addr) & ~mask >> offset;
        WriteB(addr, old | v1);
        // byte1
        uint8_t v2 = v >> (ln - 16);
        WriteB(addr + 1, v2);
        // byte2
        uint8_t v3 = v >> (ln - 24);
        WriteB(addr + 2, v3);
        // byte3
        uint8_t v4 = v >> (ln - 32);
        WriteB(addr + 3, v4);
        // byte4
        uint8_t v5 = v << (40 - ln);
        mask = (-1 << (40 - ln));
        old = ReadB(addr + 4) & ~mask;
        WriteB(addr + 4, old | v5);
    }
}