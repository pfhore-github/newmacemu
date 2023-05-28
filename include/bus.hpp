#ifndef BUS_HPP_
#define BUS_HPP_ 1
#include <optional>
#include <stdint.h>

void BusAccess(MemBus &bus);
uint8_t BusReadB(uint32_t addr);
void BusWriteB(uint32_t addr, uint8_t v);
uint16_t BusReadW(uint32_t addr);
void BusWriteW(uint32_t addr, uint16_t v);
uint32_t BusReadL(uint32_t addr);
void BusWriteL(uint32_t addr, uint32_t v);
void BusRead16(uint32_t from, uint8_t *to);
void BusWrite16(uint32_t from, const uint8_t *to);
#endif