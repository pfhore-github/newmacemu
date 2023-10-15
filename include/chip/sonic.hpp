#ifndef CHIP_SONIC_HPP_
#define CHIP_SONIC_HPP_ 1
struct Sonic {
    uint32_t read(uint32_t /* addr */) { return 0; }
    void write(uint32_t /* addr*/, uint32_t /* value */) {}
};
#endif