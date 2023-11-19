#ifndef EBUS_ADB_HPP_
#define EBUS_ADB_HPP_ 1
#include <stdint.h>
#include <deque>
class ADBDev {
  public:
    uint8_t reg3[2];
    virtual void flush() = 0;
    virtual void listen(int reg, const uint8_t *v) = 0;
    virtual std::deque<uint8_t> talk(int reg) = 0;
};
#endif