#ifndef IOP_HPP
#define IOP_HPP_
/* IIfx, Quadra9x0 IOP processor(only basic support) */
#include <stdint.h>
#include <atomic>
class IOP {
  protected:
    virtual uint8_t readBase(uint8_t reg) = 0;
    virtual void writeBase(uint8_t reg, uint8_t value) = 0;

    uint16_t PC;
    uint16_t RamAddress;
    uint8_t status;
    uint8_t MEM[65536];
    std::atomic<bool> running;
  public:
    uint8_t read(uint8_t addr);
    void write(uint8_t addr, uint8_t val);
};
#endif