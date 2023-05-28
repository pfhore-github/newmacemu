#include "io.hpp"
#include <optional>
uint8_t LoadIO_B(uint32_t addr) {
    switch(addr >> 13) {
    case 0:
        // VIA1
        return 0;
    }

    return 0;
}
uint16_t  LoadIO_W(uint32_t addr) { return 0; }
uint32_t  LoadIO_L(uint32_t addr) { return 0; }
void StoreIO_B(uint32_t addr, uint8_t b) {  }
void StoreIO_W(uint32_t addr, uint16_t w) {  }
void StoreIO_L(uint32_t addr, uint32_t l) {  }
void bus_reset() {}