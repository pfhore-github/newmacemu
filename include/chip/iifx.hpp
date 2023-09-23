#ifndef IIFX_EXP0_HPP_
#define IIFX_EXP0_HPP_ 1
class OSS : public CHIP_B {
  private:
    uint32_t intAddr(uint32_t a) const {
        return (a & 0xF) | (a & 0x1FE00) >> 5;
    }

  public:
    uint8_t v[0xFF0];
    uint8_t read(uint32_t addr) override { return v[intAddr(addr & 0x1fff)]; }
    void write(uint32_t addr, uint8_t b) override {
        v[intAddr(addr & 0x1fff)] = b;
    }
};
class IIfxExp0 : public CHIP_B {
    uint16_t v = 0;
    uint8_t x;

  public:
    uint8_t read(uint32_t ) override { return 0; }
    void write(uint32_t addr, uint8_t b) override {
        if((addr & 0x10) == 0) {
            v = v >> 1 | (b & 1) << 15;
        } else {
            x = b;
        }
    }

    uint16_t TEST_GetV00() { return v; }
    uint8_t TEST_GetV10() { return x; }
};
#endif