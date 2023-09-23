#include "io.hpp"
#include "chip/asc.hpp"
#include "memory.hpp"
#include "chip/via.hpp"
#include <optional>
// chip may alias in several range
std::unordered_map<int, std::shared_ptr<CHIP_B>> chipsB;
std::unordered_map<int, std::shared_ptr<CHIP_L>> chipsL;
uint8_t LoadIO_B(uint32_t addr) {
    int tp = addr >> 13;
    if(chipsB.contains(tp)) {
        return chipsB[tp]->read(addr);
    }
    throw AccessFault{};
}

// TODO
struct MCU_ctl : CHIP_L {
    uint32_t ctls[64];
    uint32_t read(uint32_t addr) override { return ctls[(addr >> 2) & 63]; }
    void write(uint32_t addr, uint32_t v) override {
        ctls[(addr >> 2) & 63] = v;
    }
};

uint32_t LoadIO_L(uint32_t addr) {
    int tp = addr >> 13;
    if(chipsL.contains(tp)) {
        return chipsL[tp]->read(addr);
    }

    return LoadIO_B(addr) << 24 | LoadIO_B(addr + 1) << 16 |
           LoadIO_B(addr + 2) << 8 | LoadIO_B(addr + 3);
}
void StoreIO_B(uint32_t addr, uint8_t b) {
    int tp = addr >> 13;
    if(chipsB.contains(tp)) {
        return chipsB[tp]->write(addr, b);
    }
    throw AccessFault{};
}

void StoreIO_L(uint32_t addr, uint32_t l) {
    int tp = addr >> 13;
    if(chipsL.contains(tp)) {
        return chipsL[tp]->write(addr, l);
    }
    StoreIO_B(addr, l >> 24);
    StoreIO_B(addr + 1, l >> 16);
    StoreIO_B(addr + 2, l >> 8);
    StoreIO_B(addr + 3, l);
}
void rtc_reset();
void adb_reset();
void asc_reset();
void initBus() {
    // chips initilize
    chipsB.clear();
    chipsB[0] = via1 = std::make_shared<VIA1>();
    chipsB[1] = via2 = std::make_shared<VIA2>();
    chipsB[10] = easc = std::make_shared<EASC>();
    chipsL[7] = std::make_shared<MCU_ctl>();
    // mirror
    for(auto [k, v] : chipsB) {
        for(int i = 1; i < 256; ++i) {
            chipsB[32 * i + k] = v;
        }
    }
    for(auto [k, v] : chipsL) {
        for(int i = 1; i < 256; ++i) {
            chipsL[32 * i + k] = v;
        }
    }
}
void bus_reset() {
    rtc_reset();
    adb_reset();
    asc_reset();
}
