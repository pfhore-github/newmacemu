#include <memory>

#include "68040.hpp"
#include "SDL_endian.h"
#include "bus.hpp"
#include "exception.hpp"
#include "io.hpp"
#include "memory.hpp"
inline TM GetTM(bool code) {
    return cpu.S  ? code ? TM::SYS_CODE : TM::SYS_DATA
           : code ? TM::USER_CODE
                  : TM::USER_DATA;
}
uint8_t MMU_ReadB(uint32_t addr, bool code) {
    // TODO: MMU convert
    MemBus bus{.A = addr, .tm = GetTM(code), .sz = SIZ::B, .RW = true};
    BusAccess(bus);
    return bus.D;
}
uint16_t MMU_ReadW(uint32_t addr, bool code) {
    // TODO: MMU convert
    MemBus bus{.A = addr, .tm = GetTM(code), .sz = SIZ::W, .RW = true};
    BusAccess(bus);
    return bus.D;
}
uint32_t MMU_ReadL(uint32_t addr, bool code) {
    // TODO: MMU convert
    MemBus bus{.A = addr, .tm = GetTM(code), .sz = SIZ::L, .RW = true};
    BusAccess(bus);
    return bus.D;
}
void MMU_WriteB(uint32_t addr, uint8_t b) {
    // TODO: MMU convert
    MemBus bus{
        .A = addr, .D = b, .tm = GetTM(false), .sz = SIZ::B, .RW = false};
    BusAccess(bus);
}
void MMU_WriteW(uint32_t addr, uint16_t w) {
    // TODO: MMU convert
    MemBus bus{
        .A = addr, .D = w, .tm = GetTM(false), .sz = SIZ::W, .RW = false};
    BusAccess(bus);
}
void MMU_WriteL(uint32_t addr, uint32_t l) {
    // TODO: MMU convert
    MemBus bus{
        .A = addr, .D = l, .tm = GetTM(false), .sz = SIZ::L, .RW = false};
    BusAccess(bus);
}