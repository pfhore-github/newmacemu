#ifndef IOP_HPP_
#define IOP_HPP_ 1
/* IIfx, Quadra9x0 IOP processor(only basic support) */
#include <stdint.h>
#include <atomic>
#include "io.hpp"
struct IOP {
    virtual uint8_t readBase(uint8_t reg) = 0;
    virtual void writeBase(uint8_t reg, uint8_t value) = 0;

    uint16_t PC = 0;
    uint16_t RamAddress = 0;
    uint8_t status;
    uint8_t MEM[65536];
    std::atomic<bool> running;
    uint8_t read(uint32_t addr) ;
    void write(uint32_t addr, uint8_t val) ;
};
// TODO
struct IOP_SWIM : public IOP {
     uint8_t readBase(uint8_t /* reg */) override { return 0; }
    void writeBase(uint8_t /* reg */ , uint8_t /* value */) override { }
};

// TODO
struct IOP_SCC : public IOP {
     uint8_t readBase(uint8_t /* reg */) override { return 0; }
    void writeBase(uint8_t /* reg */, uint8_t /* value */) override { }
};
#endif