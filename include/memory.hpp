#ifndef CPU_68040_MEM_
#define CPU_68040_MEM_ 1
#include "68040.hpp"
#include <vector>
#include <optional>
#include <utility>
uint8_t ReadB(uint32_t addr, bool code = false);
uint16_t ReadW(uint32_t addr, bool code = false);
uint32_t ReadL(uint32_t addr, bool code = false);
void WriteB(uint32_t addr, uint8_t b);
void WriteW(uint32_t addr, uint16_t w);
void WriteL(uint32_t addr, uint32_t l);
inline uint16_t FETCH() {
    return ReadW(std::exchange(cpu.PC, cpu.PC + 2), true);
}
inline uint32_t FETCH32() {
    return ReadL(std::exchange(cpu.PC, cpu.PC + 4), true);
}
extern std::vector<uint8_t> RAM;
extern const uint8_t* ROM;
extern size_t ROMSize;
#endif