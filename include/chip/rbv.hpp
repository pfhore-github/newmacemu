#ifndef RBV_HPP_
#define RBV_HPP_ 1
#include <stdint.h>
#include <bitset>
/* STUB */
struct RBV  {
  public:
    uint8_t rbufB;
    uint8_t rExp;
    uint8_t rSIFR;
    std::bitset<5> rIFR;
    uint8_t rMonP;
    uint8_t rChpT;
    uint8_t rSIER;
    std::bitset<5> rIER;    
    uint8_t read(uint32_t n);
    void write(uint32_t n, uint8_t v);
};
#endif