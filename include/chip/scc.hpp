#ifndef SCC_HPP_
#define SCC_HPP_ 1
#include <stdint.h>
struct SCC {
    uint8_t read(uint8_t addr);
    void write(uint8_t addr, uint8_t v);
};
#endif