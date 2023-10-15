#include "chip/iop.hpp"
#include "SDL_timer.h"

uint8_t IOP::read(uint32_t addr) {
    switch(addr) {
    case 0:
        return RamAddress >> 8;
    case 1:
        return RamAddress;
    case 2:
        return status;
    case 4:
    case 5: {
        auto v = MEM[RamAddress];
        if(status & 2) {
            RamAddress++;
        }
        return v;
    }
    default:
        return readBase(addr);
    }
}

void IOP::write(uint32_t addr, uint8_t val) {
    switch(addr) {
    case 0:
        RamAddress = (RamAddress & 0xff) | val << 8;
        break;
    case 1:
        RamAddress = (RamAddress & 0xff00) | val;
        break;
    case 2:
        /* TODO */
        if(!(val & 4)) {
            PC = 0;
            running.store(false);
        } else {
            running.store(true);
            running.notify_one();
        }
        status = val;
        break;
    case 4:
    case 5:
        MEM[RamAddress] = val;
        if(status & 2) {
            RamAddress++;
        }
        break;
    default:
        writeBase(addr, val);
        break;
    }
}