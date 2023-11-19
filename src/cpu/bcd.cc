#include "68040.hpp"
#include "bus.hpp"
#include "exception.hpp"
#include "memory.hpp"
#include "inline.hpp"
#include <memory>
#include <utility>
static uint8_t BCD2BIN(uint8_t v) { return (v >> 4) * 10 + (v & 0xf); }
static uint8_t BIN2BCD(uint8_t v) { return (v / 10) << 4 | (v % 10); }

uint8_t do_abcd(uint8_t v1, uint8_t v2, bool old_x) {
    int d = BCD2BIN(v1);
    d += BCD2BIN(v2) + old_x;
    if((cpu.X = cpu.C = d >= 100)) {
        d -= 100;
    }
    if(d != 0) {
        cpu.Z = false;
    }
    return BIN2BCD(d);
}
uint8_t do_sbcd(uint8_t v1, uint8_t v2, bool old_x) {
    int d = BCD2BIN(v1) - BCD2BIN(v2) - old_x;
    if((cpu.C = d < 0)) {
        d += 100;
    }
    cpu.X = cpu.C;
    if(d != 0) {
        cpu.Z = false;
    }
    return BIN2BCD(d);
}
uint8_t do_nbcd(uint8_t v, bool old_x) {
    uint8_t d = BCD2BIN(v);
    d = 100 - d - old_x;
    cpu.X = cpu.C = (d != 100);
    if(d == 100) {
        d = 0;
    }
    if(d) {
        cpu.Z = false;
    }
    return BIN2BCD(d);
}

uint8_t do_pack(uint16_t v, int16_t adj) {
    uint16_t d = v + adj;
    return (d & 0xf) | (d & 0xf00) >> 4;
}

uint16_t do_unpk(uint8_t v, int16_t adj) {
    uint16_t d = ((v & 0xf0) << 4 | (v & 0xf));
    return static_cast<int16_t>(d) + adj;
}