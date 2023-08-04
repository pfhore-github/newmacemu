#include "io.hpp"
#include "via.hpp"
#include <optional>
uint8_t LoadIO_B(uint32_t addr) {
    switch(addr >> 13) {
    case 0:
        // VIA1
        return via1.read(addr >> 9 & 0xf);
    case 1:
        // VIA2
        return via2.read(addr >> 9 & 0xf);
    }

    return 0;
}
uint16_t LoadIO_W(uint32_t addr) {
    switch(addr >> 13) {
    default:
        break;
    }

    return LoadIO_B(addr) << 8 | LoadIO_B(addr + 1);
}

uint32_t LoadIO_L(uint32_t addr) {
    switch(addr >> 13) {
    default:
        break;
    }

    return LoadIO_W(addr) << 16 | LoadIO_B(addr + 2);
}
void StoreIO_B(uint32_t addr, uint8_t b) {
    switch(addr >> 13) {
    case 0:
        // VIA1
        via1.write(addr >> 9 & 0xf, b);
        break;
    case 1:
        // VIA2
        via2.write(addr >> 9 & 0xf, b);
        break;
    }
}
void StoreIO_W(uint32_t addr, uint16_t w) {
    StoreIO_B(addr, w >> 8);
    StoreIO_B(addr + 1, w);
}
// TODO 
static uint32_t mc_ctls[64];
void StoreIO_L(uint32_t addr, uint32_t l) {
    switch(addr >> 13) {
    case 7:
        mc_ctls[(addr >> 2) & 63] = l;
        return;
    }
    StoreIO_W(addr, l >> 16);
    StoreIO_W(addr + 2, l);
}
void rtc_reset();
void adb_reset();
void bus_reset() {
    rtc_reset();
    adb_reset();
}