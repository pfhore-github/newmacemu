#ifndef IIFX_EXP0_HPP_
#define IIFX_EXP0_HPP_ 1
struct OSS_ctrl {
    uint8_t v[0xFF0];
    uint8_t read(uint32_t addr) { return v[addr]; }
    void write(uint32_t addr, uint8_t b) { v[addr] = b; }
};
struct IIfxExp0 {
    uint16_t v = 0;
    uint8_t x;

    uint8_t read(uint32_t) { return 0; }
    void write(uint32_t addr, uint8_t b) {
        if(addr == 0) {
            v = v >> 1 | (b & 1) << 15;
        } else {
            x = b;
        }
    }
};

// STUB (only works for test@04670)
struct IIfxExp1 {
    uint8_t s[0x100];
    uint8_t a;
    uint8_t read(uint32_t) {
        if(a == 0) {
            return 1;
        } else {
            return s[a];
        }
    }
    void write(uint32_t addr, uint8_t b) {
        if(addr == 0x10) {
            a = b;
        } else {
            s[a] = b;
        }
    }
};

#endif