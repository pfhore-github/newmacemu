#include <stdint.h>
struct OSS {
    uint8_t ch;
};
static OSS oss;
uint8_t READ_OSS(uint16_t address) {
    switch(address) {
    case 0:
        return oss.ch;
    }
    return 0;
}

void WRITE_OSS(uint16_t address, uint8_t v) {
    switch(address) {
    case 0:
        oss.ch = v & 7;
        return;
    }
}