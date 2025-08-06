#include "68040.hpp"
#include "inline.hpp"
#include "rom_common.hpp"
#include "rom_proto.hpp"
#include <expected>
#include <stdint.h>
#include <unordered_set>
namespace OP {
void cinva_bc(uint16_t);
}
void Set_TTR_t(Cpu::TTR_t &x, uint32_t v);
namespace LibROM {
const AddrMap *gAddrMap;
const UniversalInfo *gUnivInfo;
uint32_t gAddrMapFlags;
uint32_t gUnivROMFlags;
uint32_t gHWCfgFlags;
uint8_t ram_test_dword(uint32_t addr, uint32_t ptn) {
    auto d3 = ReadL(addr);
    auto d4 = ReadL(addr + 4);
    uint8_t e = 0;
    WriteL(addr, ptn);
    for(int i = 3; i >= 0; --i) {
        uint8_t v = ptn & 0xff;
        WriteL(addr + 4, -1);
        if(ReadB(addr + i) != ptn) {
            goto FAIL;
        }
        WriteB(addr + i, ~ReadB(addr + i));
        if(ReadB(addr + i) != (v ^ 0xff)) {
        FAIL:
            e |= 1 << i;
        }
        ptn = std::rotr(ptn, 8);
    }
    WriteL(addr, d3);
    WriteL(addr+4, d4);
    cpu.PC = cpu.A[6];
    return e;
}
}