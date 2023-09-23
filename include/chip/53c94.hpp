#ifndef CHIP_53C9X_HPP_
#define CHIP_53C9X_HPP_ 1
#include <stdint.h>
class SCSI_53C94 {
    uint8_t read(int addr);
    void write(int addr, uint8_t v);
};
#endif
