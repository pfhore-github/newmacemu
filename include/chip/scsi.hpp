#ifndef CHIP_SCSI_HPP_
#define CHIP_SCSI_HPP_ 1
#include <stdint.h>
#include "memory.hpp"
#include "bus.hpp"
struct SCSI_DMA  {
    uint32_t read(uint32_t addr)  {
        if( addr == 0x80) {
            return 0;
        } else {
            throw BusError{};
        }
    }
    void write(uint32_t , uint32_t )  {}
};
// TODO
struct SCSI_NORMAL {
    uint8_t read(uint32_t /* addr */) {
        return 0;
    }
    void write(uint32_t /* addr */, uint8_t /* value */) {}
};
struct SCSI_HANDSHAKE {
    uint8_t read(uint32_t /* addr */) {
        return 0;
    }
    void write(uint32_t /* addr */, uint8_t /* value */) {}
};
struct SCSI_pDMA {
    uint8_t read(uint32_t /* addr */) {
        return 0;
    }
    void write(uint32_t /* addr */, uint8_t /* value */) {}
};

struct SCSI_Quadra {
    uint32_t read(uint32_t /* addr */) {
        return 0;
    }
    void write(uint32_t /* addr */, uint32_t /* value */) {}
};

#endif
