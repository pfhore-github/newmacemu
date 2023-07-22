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
    case 0:
        // VIA1
        return via1.read(addr >> 9 & 0xf) << 8 | via1.read((addr+1) >> 9 & 0xf);
    case 1:
        // VIA2
        return via2.read(addr >> 9 & 0xf) << 8 | via2.read((addr+1) >> 9 & 0xf);
    }

    return 0;
}
uint32_t LoadIO_L(uint32_t addr) {
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
void StoreIO_W(uint32_t addr, uint16_t w) {}
void StoreIO_L(uint32_t addr, uint32_t l) {}
void rtc_reset() ;
void bus_reset() {
    rtc_reset();
}