#ifndef CPU_68040_MEM_
#define CPU_68040_MEM_ 1
#include "68040.hpp"
#include <vector>
#include <optional>
uint8_t MMU_ReadB(uint32_t addr, bool code = false);
uint16_t MMU_ReadW(uint32_t addr, bool code = false);
uint32_t MMU_ReadL(uint32_t addr, bool code = false);
void MMU_WriteB(uint32_t addr, uint8_t b);
void MMU_WriteW(uint32_t addr, uint16_t w);
void MMU_WriteL(uint32_t addr, uint32_t l);
inline uint16_t FETCH(uint32_t pc) {
    return MMU_ReadW(pc, true);
}
inline uint32_t FETCH32(uint32_t pc) {
    return MMU_ReadL(pc, true);
}
extern std::vector<uint8_t> RAM;
extern const uint8_t* ROM;
extern size_t ROMSize;
#endif