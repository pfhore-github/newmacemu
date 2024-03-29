#include "68040.hpp"
#include "bus.hpp"
#include "exception.hpp"
#include "memory.hpp"
#include "inline.hpp"
#include <memory>
#include <utility>

uint8_t AND_B(uint8_t a, uint8_t b) {
    uint8_t re = a & b;
    TEST_B(re);
    cpu.V = false;
    cpu.C = false;
    return re;
}

uint16_t AND_W(uint16_t a, uint16_t b) {
    uint16_t re = a & b;
    TEST_W(re);
    cpu.V = false;
    cpu.C = false;
    return re;
}

uint32_t AND_L(uint32_t a, uint32_t b) {
    uint32_t re = a & b;
    TEST_L(re);
    cpu.V = false;
    cpu.C = false;
    return re;
}

uint8_t OR_B(uint8_t a, uint8_t b) {
    uint8_t re = a | b;
    TEST_B(re);
    cpu.V = false;
    cpu.C = false;
    return re;
}

uint16_t OR_W(uint16_t a, uint16_t b) {
    uint16_t re = a | b;
    TEST_W(re);
    cpu.V = false;
    cpu.C = false;
    return re;
}

uint32_t OR_L(uint32_t a, uint32_t b) {
    uint32_t re = a | b;
    TEST_L(re);
    cpu.V = false;
    cpu.C = false;
    return re;
}

uint8_t XOR_B(uint8_t a, uint8_t b) {
    uint8_t re = a ^ b;
    TEST_B(re);
    cpu.V = false;
    cpu.C = false;
    return re;
}

uint16_t XOR_W(uint16_t a, uint16_t b) {
    uint16_t re = a ^ b;
    TEST_W(re);
    cpu.V = false;
    cpu.C = false;
    return re;
}

uint32_t XOR_L(uint32_t a, uint32_t b) {
    uint32_t re = a ^ b;
    TEST_L(re);
    cpu.V = false;
    cpu.C = false;
    return re;
}

uint8_t NOT_B(uint8_t v) {
    v = ~v;
    TEST_B(v);
    return v;
}

uint16_t NOT_W(uint16_t v) {
    v = ~v;
    TEST_W(v);
    return v;
}

uint32_t NOT_L(uint32_t v) {
    v = ~v;
    TEST_L(v);
    return v;
}

void BTST_B(uint8_t v, uint8_t b) { cpu.Z = !(v >> b & 1); }
void BTST_L(uint32_t v, uint8_t b) { cpu.Z = !(v >> b & 1); }
uint8_t BCHG_B(uint8_t v, uint8_t b) {
    BTST_B(v, b);
    return v ^ (1 << b);
}
uint32_t BCHG_L(uint32_t v, uint8_t b) {
    BTST_L(v, b);
    return v ^ (1 << b);
}
uint8_t BCLR_B(uint8_t v, uint8_t b) {
    BTST_B(v, b);
    return v & ~(1 << b);
}
uint32_t BCLR_L(uint32_t v, uint8_t b) {
    BTST_L(v, b);
    return v & ~(1 << b);
}
uint8_t BSET_B(uint8_t v, uint8_t b) {
    BTST_B(v, b);
    return v | (1 << b);
}
uint32_t BSET_L(uint32_t v, uint8_t b) {
    BTST_L(v, b);
    return v | (1 << b);
}

int8_t ASL_B(int8_t v, uint8_t c) {
    if(c == 0) {
        cpu.C = false;
		TEST_B(v);
        return v;
    }
    if( c > 31) {
        c = 31;
    }
    int8_t re = v << c;
    cpu.C = (v << (c - 1)) & 0x80;
    TEST_B(re);
    cpu.X = cpu.C;
    int8_t so = c > 7 ? v : v >> (7 - c);
    cpu.V = so != (v >> 7);
    return re;
}

int16_t ASL_W(int16_t v, uint8_t c) {
    if(c == 0) {
        cpu.C = false;
		TEST_W(v);
        return v;
    }
    if( c > 31) {
        c = 31;
    }
    int16_t re = v << c;
    cpu.C = (v << (c - 1)) & 0x8000;
    TEST_W(re);
    cpu.X = cpu.C;
    int16_t so = c > 15 ? v : v >> (15 - c);
    cpu.V = so != (v >> 15);
    return re;
}

int32_t ASL_L(int32_t v, uint8_t c) {
    if(c == 0) {
        cpu.C = false;
		TEST_L(v);
        return v;
    }
    int32_t re;
    int32_t so;
    if( c > 31) {
        re = 0;
        so = v;
		cpu.C = c == 32 ? (v & 1) : false;
    } else {
        re = v << c;
        so = v >> (31 - c);
		cpu.C = (v << (c - 1)) & 0x80000000;
    }
    TEST_L(re);
    cpu.X = cpu.C;
    cpu.V = so != (v >> 31);
    return re;
}

int8_t ASR_B(int8_t v, uint8_t c) {
    if(c == 0) {
        cpu.C = false;
		TEST_B(v);
        return v;
    }
	if( c > 31 ) {
		c = 31;
	}
    cpu.V = false;
    int8_t re = v >> c;
    cpu.C = (v >> (c - 1)) & 1;
    TEST_B(re);
    cpu.X = cpu.C;
    return re;
}
int16_t ASR_W(int16_t v, uint8_t c) {
    if(c == 0) {
        cpu.C = false;
		TEST_W(v);
        return v;
    }
	if( c > 31 ) {
		c = 31;
	}
    int16_t re = v >> c;
    cpu.V = false;
    cpu.C = (v >> (c - 1)) & 1;
    TEST_W(re);
    cpu.X = cpu.C;
    return re;
}
int32_t ASR_L(int32_t v, uint8_t c) {
    if(c == 0) {
        cpu.C = false;
		TEST_L(v);
        return v;
    }
    cpu.V = false;
	int32_t re;
	if( c < 32 )  [[likely]] {
		re = v >> c;
		cpu.C = (v >> (c - 1)) & 1;
	} else {
		re = (v >> 31) >> 1;
		cpu.C = (v >> 31) & 1;
	}
    TEST_L(re);
    cpu.X = cpu.C;
    return re;
}

uint8_t LSR_B(uint8_t v, uint8_t c) {
    if(c == 0) {
        cpu.C = false;
		TEST_B(v);
        return v;
    }
	if( c > 31 ) {
		c = 31;
	}
    uint8_t re = v >> c;
    cpu.V = false;
    cpu.C = (v >> (c - 1)) & 1;
    TEST_B(re);
    cpu.X = cpu.C;
    return re;
}

uint16_t LSR_W(uint16_t v, uint8_t c) {
    if(c == 0) {
        cpu.C = false;
		TEST_W(v);
        return v;
    }
	if( c > 31 ) {
		c = 31;
	}
    uint16_t re = v >> c;
    cpu.V = false;
    cpu.C = (v >> (c - 1)) & 1;
    TEST_W(re);
    cpu.X = cpu.C;
    return re;
}

uint32_t LSR_L(uint32_t v, uint8_t c) {
    if(c == 0) {
        cpu.C = false;
		TEST_L(v);
        return v;
    }
    cpu.V = false;
	uint32_t re;
	if( c > 31 ) {
		re = 0;
		cpu.C = ((uint64_t)v >> (c - 1)) & 1;
	} else {
		re = v >> c;
		cpu.C = (v >> (c - 1)) & 1;
	}
    TEST_L(re);
    cpu.X = cpu.C;
    return re;
}

uint8_t LSL_B(uint8_t v, uint8_t c) {
    if(c == 0) {
        cpu.C = false;
		TEST_B(v);
        return v;
    }
	if( c > 31 ) {
		c = 31;
	}
    uint8_t re = v << c;
    cpu.X = cpu.C = (v << (c - 1)) & 0x80;
    TEST_B(re);
    cpu.V = false;
    return re;
}

uint16_t LSL_W(uint16_t v, uint8_t c) {
    if(c == 0) {
        cpu.C = false;
		TEST_W(v);
        return v;
    }
	if( c > 31 ) {
		c = 31;
	}
    uint16_t re = v << c;
    cpu.X = cpu.C = (v << (c - 1)) & 0x8000;
    TEST_W(re);
    cpu.V = false;
    return re;
}

uint32_t LSL_L(uint32_t v, uint8_t c) {
    if(c == 0) {
        cpu.C = false;
		TEST_L(v);
        return v;
    }
	uint32_t re;
	if( c > 31 ) {
		re = 0;
		cpu.X = cpu.C = c == 32 ? (v & 1) : false;
	} else {
		re = v << c;
		cpu.X = cpu.C = (v << (c - 1)) & 0x80000000;
	}
    TEST_L(re);
    cpu.V = false;
    return re;
}

uint8_t ROR_B(uint8_t v, uint8_t c) {
    cpu.V = false;
    if(c == 0) {
        TEST_B(v);
        cpu.C = false;
        return v;
    }
	if( c > 8 ) {
		c &= 7;
	}
    uint8_t re = v >> c | v << (8 - c);
    cpu.C = v >> (c - 1) & 1;
    TEST_B(re);
    return re;
}

uint16_t ROR_W(uint16_t v, uint8_t c) {
    cpu.V = false;
    if(c == 0) {
        TEST_W(v);
        cpu.C = false;
        return v;
    }
	if( c > 16 ) {
		c &= 15;
	}
    uint16_t re = v >> c | v << (16 - c);
    cpu.C = v >> (c - 1) & 1;
    TEST_W(re);
    return re;
}

uint32_t ROR_L(uint32_t v, uint8_t c) {
    cpu.V = false;
    if(c == 0) {
        TEST_L(v);
        cpu.C = false;
        return v;
    }
	c &= 31;
	uint32_t re;
	if( c == 0 ) {
		re = v;
		cpu.C = v >> 31 & 1;
	} else {
		re = v >> c | v << (32 - c);
		cpu.C = v >> (c - 1) & 1;
	}
    TEST_L(re);
    return re;
}

uint8_t ROL_B(uint8_t v, uint8_t c) {
    cpu.V = false;
    if(c == 0) {
        TEST_B(v);
        cpu.C = false;
        return v;
    }
	c &= 7;
    uint8_t re = v << c | v >> (8 - c);
    cpu.C = v >> (8 - c) & 1;
    TEST_B(re);
    return re;
}

uint16_t ROL_W(uint16_t v, uint8_t c) {
    cpu.V = false;
    if(c == 0) {
        TEST_W(v);
        cpu.C = false;
        return v;
    }
	c &= 15;
    uint16_t re = v << c | v >> (16 - c);
    cpu.C = v >> (16 - c) & 1;
    TEST_W(re);
    return re;
}

uint32_t ROL_L(uint32_t v, uint8_t c) {
    cpu.V = false;
    if(c == 0) {
        TEST_L(v);
        cpu.C = false;
        return v;
    }
	c &= 31;
    uint32_t re = v << c | v >> (32 - c);
    cpu.X = cpu.C = v >> (32 - c) & 1;
    TEST_L(re);
    return re;
}

uint8_t ROXR_B(uint8_t v, uint8_t c, bool old_x) {
    cpu.V = false;
    if(c == 0) {
        TEST_B(v);
        cpu.C = old_x;
        return v;
    }
	c = (c&63) % 9;
    uint8_t re = v >> c | old_x << (8 - c) | v << (9 - c);
    cpu.X = cpu.C = v >> (c - 1) & 1;
    TEST_B(re);
    return re;
}

uint16_t ROXR_W(uint16_t v, uint8_t c, bool old_x) {
    cpu.V = false;
    if(c == 0) {
        TEST_W(v);
        cpu.C = old_x;
        return v;
    }
	c = (c&63) % 17;
    uint16_t re = v >> c | old_x << (16 - c) | v << (17 - c);
    cpu.X = cpu.C = v >> (c - 1) & 1;
    TEST_W(re);
    return re;
}

uint32_t ROXR_L(uint32_t v, uint8_t c, bool old_x) {
    cpu.V = false;
    if(c == 0) {
        TEST_L(v);
        cpu.C = old_x;
        return v;
    }
	c = (c&63) % 33;
    uint32_t re = (uint64_t)v >> c | old_x << (32 - c) | (uint64_t)v << (33 - c);
    cpu.X = cpu.C = v >> (c - 1) & 1;
    TEST_L(re);
    return re;
}

uint8_t ROXL_B(uint8_t v, uint8_t c, bool old_x) {
    cpu.V = false;
    if(c == 0) {
        TEST_B(v);
        cpu.C = old_x;
        return v;
    }
	c = (c&63) % 9;
    uint8_t re = v << c | old_x << (c - 1) | v >> (9 - c);
    cpu.X = cpu.C = v >> (8 - c) & 1;
    TEST_B(re);
    return re;
}

uint16_t ROXL_W(uint16_t v, uint8_t c, bool old_x) {
    cpu.V = false;
    if(c == 0) {
        TEST_W(v);
        cpu.C = old_x;
        return v;
    }
	c = (c&63) % 17;
    uint16_t re = v << c | old_x << (c - 1) | v >> (17 - c);
    cpu.X = cpu.C = v >> (16 - c) & 1;
    TEST_W(re);
    return re;
}

uint32_t ROXL_L(uint32_t v, uint8_t c, bool old_x) {
    cpu.V = false;
    if(c == 0) {
        TEST_L(v);
        cpu.C = old_x;
        return v;
    }
	c = (c&63) % 33;
	uint32_t re = 0;
	if( c == 0 ) {
		re = v;
		cpu.X = cpu.C = old_x;
	} else if( c == 32 ) {
		re = v >> 1 | old_x << 31;
		cpu.X = cpu.C = v & 1;		
	} else {
		re = v << c | old_x << (c - 1) | (c == 1 ? 0 : v >> (33 - c));
		cpu.X = cpu.C = v >> (32 - c) & 1;
	}
    TEST_L(re);
    return re;
}



