#include "68040.hpp"
#include "inline.hpp"
#include <stdint.h>
#include <unordered_set>
#include "rom_proto.hpp"
void rom_call(uint32_t target, const std::unordered_set<uint32_t> &ret);
void rom_call(uint32_t target, uint32_t ret);
void bus_reset();
namespace LibROM {
// 0x8C-0xB8
void boot() {
    SetSR(0x2700);
    init_mmu();
    bus_reset();
    init_hw();
    init_pb();
    boot2();
}

void boot2() {
    return warm_reset();
}
}
