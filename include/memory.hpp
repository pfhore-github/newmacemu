#ifndef CPU_68040_MEM_
#define CPU_68040_MEM_ 1
#include "68040.hpp"
#include "SDL3/SDL_endian.h"
#include <optional>
#include <utility>
#include <vector>
#include <expected>
uint8_t ReadB(uint32_t addr);
uint16_t ReadW(uint32_t addr);
uint16_t FetchW(uint32_t addr);
uint32_t ReadL(uint32_t addr);
void WriteB(uint32_t addr, uint8_t b);
void WriteW(uint32_t addr, uint16_t w);
void WriteL(uint32_t addr, uint32_t l);

struct PTestError {
	virtual ~PTestError() {}
	PTestError(uint16_t v) :code(v) {}
	uint16_t code;
};
struct BusError : PTestError{
	BusError() :PTestError(0) {}
};

uint8_t ReadBImpl(uint32_t addr, bool lock);
uint16_t ReadWImpl(uint32_t addr, bool code, bool lock);
uint32_t ReadLImpl(uint32_t addr, bool lock);

uint8_t ReadSB(uint32_t addr);
uint16_t ReadSW(uint32_t addr);
uint32_t ReadSL(uint32_t addr);
void WriteSB(uint32_t addr, uint8_t b);
void WriteSW(uint32_t addr, uint16_t w);
void WriteSL(uint32_t addr, uint32_t l);
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
    return SDL_Swap16BE(*reinterpret_cast<const uint16_t *>(base));
}

inline uint32_t readBE32(const uint8_t *base) {
    return SDL_Swap32BE(*reinterpret_cast<const uint32_t *>(base));
}

inline void writeBE16(uint8_t *base, uint16_t v) {
    *reinterpret_cast<uint16_t *>(base) = SDL_Swap16BE(v);
}

inline void writeBE32(uint8_t *base, uint32_t v) {
    *reinterpret_cast<uint32_t *>(base) = SDL_Swap32BE(v);
}
#endif
