#include "rbv.hpp"
struct RBV : public CHIP_B {
  public:
    uint8_t rbufB;
    uint8_t rExp;
    uint8_t rSIFR;
    uint8_t rIFR;
    uint8_t rMonP;
    uint8_t rChpT;
    uint8_t rSIER;
    uint8_t rIER;    
    uint8_t read(uint32_t n);
    void write(uint32_t n, uint8_t v);
};