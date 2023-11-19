#ifndef CPU_68040_MEM_
#define CPU_68040_MEM_ 1
#include "68040.hpp"
#include "SDL_endian.h"
#include <optional>
#include <utility>
#include <vector>
struct BusError {};
uint8_t ReadB(uint32_t addr);
uint16_t ReadW(uint32_t addr);
uint16_t FetchW(uint32_t addr);
uint32_t ReadL(uint32_t addr);
void WriteB(uint32_t addr, uint8_t b);
void WriteW(uint32_t addr, uint16_t w);
void WriteL(uint32_t addr, uint32_t l);
inline uint16_t FETCH() { return FetchW(std::exchange(cpu.PC, cpu.PC + 2)); }
inline uint32_t FETCH32() {
    uint32_t v = FETCH();
    v = v << 16 | FETCH();
    return v;
}
extern uint8_t *RAM;
extern size_t RAMSize;
extern uint8_t *ROM;
extern size_t ROMSize;
extern size_t ROMMask;
inline uint16_t readBE16(const uint8_t *base) {
    return SDL_SwapBE16(*reinterpret_cast<const uint16_t *>(base));
}

inline uint32_t readBE32(const uint8_t *base) {
    return SDL_SwapBE32(*reinterpret_cast<const uint32_t *>(base));
}

inline void writeBE16(uint8_t *base, uint16_t v) {
    *reinterpret_cast<uint16_t *>(base) = SDL_SwapBE16(v);
}

inline void writeBE32(uint8_t *base, uint32_t v) {
    *reinterpret_cast<uint32_t *>(base) = SDL_SwapBE32(v);
}
#endif