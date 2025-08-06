#ifndef BUS_HPP_
#define BUS_HPP_ 1
#include <stdint.h>
uint8_t BusReadB(uint32_t addr);
uint16_t BusReadW(uint32_t addr);
uint32_t BusReadL(uint32_t addr);
void BusWriteB(uint32_t addr, uint8_t v);
void BusWriteW(uint32_t addr, uint16_t v);
void BusWriteL(uint32_t addr, uint32_t v);
void Transfer16(uint32_t from, uint32_t to);
#endif
