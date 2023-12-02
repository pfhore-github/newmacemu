#ifndef MMU_HPP_
#define MMU_HPP_ 1
#include <stdint.h>
struct mmu_result {
    uint32_t paddr = 0;
    uint8_t Ux = 0;
    uint8_t CM = 0;
    bool R = false;
    bool T = false;
    bool W = false;
    bool M = false;
    bool S = false;
    bool G = false;
    bool B = false;
    uint32_t value() const {
        return R | T << 1 | W << 2 | M << 4 | CM << 5 | S << 7 |
               Ux << 8 | G << 10 | B << 11 | paddr << 12;
    }
};

struct atc_entry {
    // TAG
    uint32_t laddr;
    bool V;      
    bool G; 

    uint32_t paddr;
    uint32_t pg_addr;
    uint8_t U;
    bool S;
    uint8_t CM;
    bool M, W, R;
};
extern atc_entry d_atc[2][16], i_atc[2][16];
mmu_result ptest(uint32_t addr, bool sys, bool code, bool W);
#endif