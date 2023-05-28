#ifndef IO_
#define IO_ 1
#include <stdint.h>
#include <optional>
uint8_t LoadIO_B(uint32_t addr);
uint16_t  LoadIO_W(uint32_t addr);
uint32_t LoadIO_L(uint32_t addr);
void StoreIO_B(uint32_t addr, uint8_t b);
void StoreIO_W(uint32_t addr, uint16_t w);
void StoreIO_L(uint32_t addr, uint32_t l);
#endif