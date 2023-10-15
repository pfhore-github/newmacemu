#include "mb/mcu.hpp"
#include "SDL.h"
#include "memory.hpp"
#include <stdint.h>
uint32_t MCU_ctl::read(uint32_t addr) {
    if(addr < 0x1000) {
        return ctls[(addr >> 2) & 63];
    } else if(addr < 0x1400) {
        // SCSI #0
        if((addr & 0xff) == 0xC0) {
            return 4;
        } else {
            throw AccessFault{};
        }
    } else if(addr < 0x1800) {
        // SCSI #1
        return 0;
    } else {
        throw AccessFault{};
    }
}
void MCU_ctl::write(uint32_t addr, uint32_t v) {
    if(addr < 128) {
        addr >>= 2;
        value = (value & ~(1 << addr)) | (v & 1) << addr;
        return;
    }
    ctls[(addr >> 2) & 63] = v;
}