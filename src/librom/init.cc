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
void init_hw() {
    cpu.A[7] = 0x2600;
    uint32_t old_lr = cpu.A[6];
    cpu.VBR = (cpu.PC & ~0xFFF00000) | 0x03DEC;
    do {
        getHwInfo(0);
    } while(!(gAddrMapFlags & 1));
    auto rom_offset = jump_to_rom(cpu.PC & ~0xffffff);
    old_lr += rom_offset;
    cpu.PC = 0x2E3E + rom_offset;
    init_hw2(rom_offset);
    if(gAddrMapFlags & 1 << ADDR_OSS) {
        auto oss_io = gAddrMap->addr[ADDR_OSS];
        WriteB(oss_io + 0x204, 13);
    }
    if(gAddrMapFlags & 1 << ADDR_EXP0) {
        auto iifx_exp0 = gAddrMap->addr[ADDR_EXP0];
        uint16_t d3 = 0xF3FF;
        for(int i = 0; i < 16; ++i, d3 >>= 1) {
            WriteB(iifx_exp0, d3);
        }
        WriteB(iifx_exp0 + 0x10, d3);
    }
    init_mcu_jaws();
    cpu.PC = old_lr;
}

void init_via_rbv() {
    if(gAddrMapFlags & 1 << ADDR_VIA1) {
        auto via1 = gAddrMap->addr[ADDR_VIA1];
        auto data1 = gUnivInfo->viaData[0];
        WriteB(via1 + 0x1e00, data1->regA);
        WriteB(via1 + 0x0600, data1->dirA);
        WriteB(via1 + 0x0000, data1->regB);
        WriteB(via1 + 0x0400, data1->dirB);
        WriteB(via1 + 0x1800, data1->pcr);
        WriteB(via1 + 0x1600, data1->acr);
        WriteB(via1 + 0x1C00, 0x7f);
    }
    if(gAddrMapFlags & 1 << ADDR_VIA2) {
        auto via2 = gAddrMap->addr[ADDR_VIA2];
        auto data2 = gUnivInfo->viaData[1];
        WriteB(via2 + 0x1e00, ReadB(via2 + 0x1e00) | data2->regA);
        WriteB(via2 + 0x0600, data2->dirA);
        WriteB(via2 + 0x0000, data2->regB);
        WriteB(via2 + 0x0400, data2->dirB);
        WriteB(via2 + 0x1800, data2->pcr);
        WriteB(via2 + 0x1600, data2->acr);
        WriteB(via2 + 0x1C00, 0x7f);
        WriteB(via2 + 0x0800, 0x6E);
        WriteB(via2 + 0x0A00, 0x19);
    }
    if(gAddrMapFlags & 1 << ADDR_RBV) {
        auto rbv = gAddrMap->addr[ADDR_RBV];
        WriteB(rbv + 0x13, 0x7f);
        WriteB(rbv + 0, 0x8f);
        WriteB(rbv + 0x10, 0x40);
        WriteB(rbv + 0x12, 0xff);
    }
}

// $31C8-$3204
static std::vector<uint32_t> univInfoTable{
    0x408036BC, // SE/30
    0x408036FC, // IIcx
    0x4080373C, // IIci
    0x4080377C, // IIci+PGC
    0x408037BC, // IIfx
    0x408037FC, // IIsi
    0x4080383C, // LC
    0x408038FC, // Q700
    0x4080387C, // Q900
    0x408038BC, // Q950
    0x4080393C, // PB170
    0x4080397C, // GLUE
    0x408039BC, // MDU
    0x408039FC, // OSS
    0x40803A3C, // V8
    0x40803A7C, // JAWS
};

// $03060
bool check_glue(const AddrMap *map) {
    try {
        auto base = map->addr[ADDR_VIA1] + 0x1C00;
        return check_via(base, 0x20000);
    } catch(TestError &) {
        return false;
    }
}
bool check_mdu(const AddrMap *map) {
    try {
        auto base = map->addr[ADDR_VIA1] + 0x1C00;
        if(!check_via(base, 0x40000) || check_via(base, 0x20000)) {
            return false;
        }
        ReadBTest(map->addr[ADDR_RBV]);
        ReadBTest(map->addr[ADDR_VDAC]);
        return true;
    } catch(TestError &) {
        return false;
    }
}
bool check_oss(const AddrMap *map) {
    try {
        auto base = map->addr[ADDR_VIA1] + 0x1C00;
        if(!check_via(base, 0x40000) || check_via(base, 0x20000)) {
            return false;
        }
        auto oss = map->addr[ADDR_OSS];
        uint8_t oss_v = ReadBTest(oss);
        for(int d2 = 6; d2 >= 0; --d2) {
            WriteB(oss, d2);
            if((ReadB(oss) & 7) != d2) {
                return false;
            }
        }
        WriteB(oss, oss_v);
        return true;
    } catch(TestError &) {
        return false;
    }
}
bool check_v8(const AddrMap *map) {
    try {
        auto base = map->addr[ADDR_VIA1] + 0x1C00;
        if(!check_via0(base) || check_via(base, 0x40000)) {
            return false;
        }
        ReadBTest(map->addr[ADDR_RBV]);
        ReadBTest(map->addr[ADDR_VDAC]);
        return true;
    } catch(TestError &) {
        return false;
    }
}
bool check_mcu(const AddrMap *map) {
    try {
        auto base = map->addr[ADDR_MCU_CTL];
        ReadLTest(base);
        return true;
    } catch(TestError &) {
        return false;
    }
}
bool check_jaws(const AddrMap *map) {
    try {
        auto base = map->addr[ADDR_VIA1] + 0x1C00;
        if(!check_via(base, 0x100000) || check_via(base, 0x80000) ||
           check_via(base, 0x40000)) {
            return false;
        }
        ReadBTest(map->addr[ADDR_JAWS_CTL]);
        return true;
    } catch(TestError &) {
        return false;
    }
}
// $031AC-$031C8
static uint32_t AddrMapTable[] = {
    0x4080360C, // JAWS
    0x40803568, // MCU
    0x408032D8, // GLUE
    0x4080337C, // MDU
    0x40803420, // OSS
    0x408034C4, // V8
};

extern std::unordered_map<uint32_t, const UniversalInfo *> UniversalInfoMaps;
extern std::unordered_map<uint32_t, const AddrMap *> addrMapMap;

void getHwInfo(uint8_t id) {
    auto lr = cpu.A[6];

    cpu.D[7] |= MEM_TEST;
    if(id) {
        for(auto a1 : univInfoTable) {
            gUnivInfo = UniversalInfoMaps[cpu.A[1] = a1];
            if(gUnivInfo->gId == id) {
                goto FOUND;
            }
        }
    }
    for(;;) {
        for(auto addr : AddrMapTable) {
            gAddrMap = addrMapMap[cpu.A[0] = addr];
            if(gAddrMap->check(gAddrMap)) {
                break;
            }
        }
        gAddrMapFlags = gAddrMap->enabledList;
        auto mid = getHwId(gAddrMap, gAddrMapFlags);
        auto gen = gAddrMap->gen;
        for(auto info : univInfoTable) {
            gUnivInfo = UniversalInfoMaps[cpu.A[1] = info];
            if(gUnivInfo->gen != gen) {
                continue;
            }
            if((mid & gUnivInfo->modelMask) != gUnivInfo->modelId) {
                continue;
            }
            goto FOUND;
        }
    }
FOUND:
    // 0x2F36
    gAddrMap = gUnivInfo->addrMaps;
    for(auto [k, v] : addrMapMap) {
        if(v == gAddrMap) {
            cpu.A[0] = k;
            break;
        }
    }
    gAddrMapFlags = gUnivInfo->addrMapFlags;
    if(!gAddrMapFlags) {
        // 0x2F98
        gAddrMapFlags = gAddrMap->enabledList;
        // RBV
        if(gAddrMapFlags & 1 << ADDR_RBV) {
            if(!check_rbv(gAddrMap->addr[ADDR_RBV] + 19)) {
                gAddrMapFlags &= ~(1 << ADDR_RBV | 1 << ADDR_VDAC);
            }
        }
        // VIA2
        if(gAddrMapFlags & 1 << ADDR_VIA2) {
            try {
                cpu.Z = check_via0(gAddrMap->addr[ADDR_VIA2] + 0x1C00);
            } catch(TestError &) {
                cpu.Z = false;
            }
            if(!cpu.Z) {
                gAddrMapFlags &= ~(1 << ADDR_VIA2);
            }
        }
        // IOP.SWIM
        if(gAddrMapFlags & 1 << ADDR_IOP_SWIM) {
            if(!check_iop(gAddrMap->addr[ADDR_IOP_SWIM])) {
                gAddrMapFlags &= ~(1 << ADDR_IOP_SWIM);
            } else {
                gAddrMapFlags &= ~(1 << ADDR_SWIM);
            }
        }
        // IOP.SCC
        if(gAddrMapFlags & 1 << ADDR_IOP_SCC) {
            if(!check_iop(gAddrMap->addr[ADDR_IOP_SCC])) {
                gAddrMapFlags &= ~(1 << ADDR_IOP_SCC);
            } else {
                gAddrMapFlags &= ~((1 << ADDR_SCC_R) | (1 << ADDR_SCC_W));
            }
        }

        // SCSI.DMA
        if(gAddrMapFlags & 1 << ADDR_SCSI_DMA) {
            if(!check_scsi_dma(gAddrMap->addr[ADDR_SCSI_DMA])) {
                gAddrMapFlags &= ~(1 << ADDR_SCSI_DMA);
            } else {
                gAddrMapFlags &= ~((1 << ADDR_SCSI) | (1 << ADDR_SCSI_pDMA) |
                              (1 << ADDR_SCSI_HANDSHAKE));
            }
        }
    } else {
        // 0x2F44
        gHWCfgFlags =
            gUnivInfo->hWCfgFlagsH << 16 | gUnivInfo->gId << 8 | gUnivInfo->gen;
        refresh_cpu_dcache(&gHWCfgFlags);
    }
    gUnivROMFlags = gUnivInfo->univROMFlags;
    if(!gUnivROMFlags) {
        gUnivROMFlags = gAddrMap->univRomFlags;
    }
    cpu.D[7] &= ~MEM_TEST;
    cpu.PC = lr;
}
uint32_t jump_to_rom(uint32_t base) {
    uint32_t rom_offset = gAddrMap->addr[ADDR_ROM] - base;
    cpu.A[0] += rom_offset;
    cpu.A[1] += rom_offset;
    return rom_offset;
}

void refresh_cpu_dcache(uint32_t *flags) {
    if(*flags & 1 << 28) {
        if(!invalidate_cpu_dcache()) {
            *flags &= ~1 << 28;
        }
    }
}

void init_mmu() {
    // only 68040MMU is supported
    OP::cinva_bc(0);
    cpu.CACR_DE = false;
    cpu.CACR_IE = false;
    cpu.TCR_E = false;
    cpu.TCR_P = false;
    Set_TTR_t(cpu.DTTR[0], 0x807FC040);
    Set_TTR_t(cpu.DTTR[1], 0x500FC040);
    cpu.PC = cpu.A[6];
}

bool invalidate_cpu_dcache() {
    // only 68040MMU is supported
    cpu.CACR_DE = false;
    return true;
}

bool check_via0(uint32_t base) { return check_via(base, 0); }

bool check_via(uint32_t base, uint32_t offset) {
    ReadBTest(base + offset);
    uint16_t old = ReadBTest(base);
    int8_t er = -1;
    WriteB(base, er);
    for(; er != -128 && ReadBTest(base + offset) == er; er *= 2) {
        WriteB(base, -er);
    }
    WriteB(base, 0x7f);
    WriteB(base, old);
    cpu.PC = cpu.A[6];
    return er == -128;
}

bool check_rbv(uint32_t base) {
    try {
        uint16_t old = ReadBTest(base);
        int8_t er = -1;
        WriteB(base, er);
        for(; er != -128 && !((ReadBTest(base) ^ er) & 0x9f); er *= 2) {
            WriteB(base, -er);
        }
        WriteB(base, 0x7f);
        WriteB(base, old);
        cpu.PC = cpu.A[6];
        return er == -128;
    } catch(TestError &) {
        return false;
    }
}

bool check_scsi_dma(uint32_t base) {
    try {
        ReadLTest(base + 0x80);
        return true;
    } catch(TestError &) {
        return false;
    }
}

bool check_iop(uint32_t base) {
    try {
        uint8_t old_ctrl = ReadBTest(base - 28);
        uint8_t vx = old_ctrl & 6;
        uint16_t old_addr = ReadWTest(base - 31);
        WriteW(base - 28, vx | 2);
        ReadLTest(base - 24);
        uint16_t new_addr = ReadWTest(base - 31) - 4;
        WriteW(base - 31, old_addr);
        WriteB(base - 28, old_ctrl & 0xcf);
        return new_addr == old_addr;
    } catch(TestError &) {
        return false;
    }
}

uint32_t getHwId(const AddrMap *addrMap, uint32_t addrFlags) {
    uint32_t ret = 0;
    if(addrFlags & 1 << ADDR_VIA1) {
        auto via1 = addrMap->addr[ADDR_VIA1];
        auto old_va = ReadB(via1 + 0x1E00);
        auto oldDirA = ReadB(via1 + 0x0600);
        WriteB(via1 + 0x0600, oldDirA & addrMap->via1MaskA);
        ret |= ReadB(via1 + 0x1E00) << 24;
        WriteB(via1 + 0x0600, oldDirA);
        WriteB(via1 + 0x1E00, old_va);

        auto old_vb = ReadB(via1 + 0x0400);
        auto oldDirB = ReadB(via1 + 0);
        WriteB(via1 + 0, oldDirB & addrMap->via1MaskB);
        ret |= ReadB(via1 + 0x0400) << 16;
        WriteB(via1 + 0, oldDirB);
        WriteB(via1 + 0x0400, old_vb);
    }

    if(addrFlags & 1 << ADDR_VIA2) {
        auto via2 = addrMap->addr[ADDR_VIA2];
        auto old_va = ReadB(via2 + 0x1E00);
        auto oldDirA = ReadB(via2 + 0x0600);
        WriteB(via2 + 0x0600, oldDirA & addrMap->via2MaskA);
        ret |= ReadB(via2 + 0x1E00) << 8;
        WriteB(via2 + 0x0600, oldDirA);
        WriteB(via2 + 0x1E00, old_va);

        auto old_vb = ReadB(via2 + 0x0400);
        auto oldDirB = ReadB(via2 + 0);
        WriteB(via2 + 0, oldDirB & addrMap->via2MaskB);
        ret |= ReadB(via2 + 0x0400) << 0;
        WriteB(via2 + 0, oldDirB);
        WriteB(via2 + 0x0400, old_vb);
    }
    return ret;
}
void init_mcu_jaws() {
    if(gAddrMapFlags & 1 << ADDR_MCU_CTL) {
        auto mcu = gAddrMap->addr[ADDR_MCU_CTL];
        auto via2 = gAddrMap->addr[ADDR_VIA2];
        uint32_t v = 0x124F0810;
        WriteB(via2 + 0x0400, ReadB(via2 + 0x0400) & ~1 << 4);
        if(ReadB(via2) & 1 << 5) {
            v = 0x138B0810;
        }
        uint32_t mcu_p2 = mcu;
        for(int i = 0; i < 32; ++i, v >>= 1, mcu_p2 += 4) {
            WriteL(mcu_p2, v);
        }
        WriteL(mcu_p2, 0);
        mcu_p2 += 4;
        WriteL(mcu + 0xA0, 0xffff);
        WriteL(mcu + 0xA4, 0xffff);
        WriteL(mcu + 0xA8, 0xffff);
        WriteL(mcu + 0xAC, 0xffff);
        WriteL(mcu + 0xB0, 0xffff);
        WriteL(mcu + 0xB4, 0xffff);
        auto vv = ReadL(mcu + 0x48);
        WriteL(mcu_p2, !(vv & 1));
        WriteL(mcu + 0xB8, 0xffff);
    }
    if(gAddrMapFlags & 1 << ADDR_JAWS_CTL) {
        auto jaws = gAddrMap->addr[ADDR_JAWS_CTL];
        WriteL(jaws, ReadL(jaws + 0x34000) & 1);
    }
}
void init_hw2(uint32_t offset) {
    cpu.A[7] += offset;
    cpu.VBR = (cpu.PC & ~0xFFF00000) | 0x03DEC;
    do {
        getHwInfo(0);
    } while(!(gAddrMapFlags & 1));
    init_via_rbv();
    cpu.PC = 0x40802E46;
}
}
