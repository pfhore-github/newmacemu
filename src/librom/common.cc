#include <unordered_set>
#include <stdint.h>
#include "68040.hpp"
#include "rom_proto.hpp"
extern std::unordered_set<uint32_t> rom_stops;
void rom_call(uint32_t target, const std::unordered_set<uint32_t>& ret) {
    cpu.PC = (cpu.PC & 0xFFF00000) | target;
    rom_stops = ret;
    // add ROM addr
    for(auto v : ret) {
        rom_stops.insert(v | 0x40000000);
        rom_stops.insert(v | 0x40800000);
    }

    // resume CPU
    cpu.sleeping.store(false);
    cpu.sleeping.notify_one();
    while(!cpu.sleeping.load() && ! ret.contains(cpu.PC)) {
        cpu.sleeping.wait(false);
    } 

    // pause CPU
    cpu.sleeping.store(true);
    cpu.sleeping.notify_one();
}

void rom_call(uint32_t target, uint32_t ret) {
    std::unordered_set<uint32_t> v;
    v.insert(ret);
    rom_call(target, v);
}



extern std::unordered_map<uint32_t, void (*)()> rom_funcs ;
struct InitRom {
    InitRom() {
        rom_funcs[0x8C] = LibROM::boot;
        rom_funcs[0x46A80] = LibROM::ex_busErr;
        rom_funcs[0x46A90] = LibROM::ex_addressErr;
        rom_funcs[0x46A96] = LibROM::ex_illInstErr;
        rom_funcs[0x46A9C] = LibROM::ex_zeroDivErr;
        rom_funcs[0x46AA2] = LibROM::ex_chkErr;
        rom_funcs[0x46AA8] = LibROM::ex_trapErr;
        rom_funcs[0x46AAE] = LibROM::ex_privErr;
        rom_funcs[0x46AB4] = LibROM::ex_traceErr;
        rom_funcs[0x46ABA] = LibROM::ex_lineAErr;
        rom_funcs[0x46AC0] = LibROM::ex_lineFErr;
        rom_funcs[0x46AC6] = LibROM::ex_miscErr;
        rom_funcs[0x46ACC] = LibROM::ex_coprocessorErr;
        rom_funcs[0x46AD2] = LibROM::ex_formatErr;
        rom_funcs[0x46AD8] = LibROM::ex_irqErr;
        rom_funcs[0x46ADE] = LibROM::ex_vtrapErr;
        rom_funcs[0x46AE4] = LibROM::ex_irq1;
        rom_funcs[0x46AEA] = LibROM::ex_irq2;
        rom_funcs[0x46AF0] = LibROM::ex_irq3;
        rom_funcs[0x46AF6] = LibROM::ex_irq4;
        rom_funcs[0x46AFC] = LibROM::ex_irq5;
        rom_funcs[0x46B04] = LibROM::ex_irq6;
        rom_funcs[0x46B24] = LibROM::ex_irq7;
        rom_funcs[0x46B96] = LibROM::ex_fpBsunErr;
        rom_funcs[0x46BA0] = LibROM::ex_fpInexErr;
        rom_funcs[0x46BAA] = LibROM::ex_fpDiv0Err;
        rom_funcs[0x46BB4] = LibROM::ex_fpUnflErr;
        rom_funcs[0x46BBE] = LibROM::ex_fpOpErErr;
        rom_funcs[0x46BC8] = LibROM::ex_fpOvflErr;
        rom_funcs[0x46BD2] = LibROM::ex_fpSnanErr;
        rom_funcs[0x46BDC] = LibROM::ex_fp56Err;
        rom_funcs[0x46BE6] = LibROM::ex_fp57Err;
        rom_funcs[0x46BF0] = LibROM::ex_fp58Err;
        rom_funcs[0x46C06] = LibROM::warm_reset;
    }
};
static InitRom _;
