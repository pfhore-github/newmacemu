#ifndef ROM_COMMON_HPP_
#define ROM_COMMON_HPP_ 1
#include <stdint.h>
#include <vector>
#include <utility>
#include "memory.hpp"
constexpr uint32_t MEM_TEST = 1 << 27;
enum ADDR {
    ADDR_ROM,
    ADDR_PDS,
    ADDR_VIA1,
    ADDR_SCC_R,
    ADDR_SCC_W,
    ADDR_SWIM,
    ADDR_UNUSED1,
    ADDR_UNUSED2,
    ADDR_SCSI,
    ADDR_SCSI_pDMA,
    ADDR_SCSI_HANDSHAKE,
    ADDR_VIA2,
    ADDR_ASC,
    ADDR_RBV,
    ADDR_VDAC,
    ADDR_SCSI_DMA,
    ADDR_IOP_SWIM,
    ADDR_IOP_SCC,
    ADDR_OSS,
    ADDR_EXP0,
    ADDR_EXP1,
    ADDR_MCU_CTL,
    ADDR_JAWS_CTL,
    ADDR_ETHER,
    ADDR_SCSI_QINT,
    ADDR_SCSI_QEXT,
    ADDR_QUADRA_VCTL,
    ADDR_END
};
struct AddrMap {
    uint32_t enabledList;
    uint32_t univRomFlags;
    uint8_t via1MaskA;
    uint8_t via1MaskB;
    uint8_t via2MaskA;
    uint8_t via2MaskB;
    bool (*check)(const struct AddrMap*);
    uint32_t gen;
    uint32_t addr[ADDR_END];
};

struct RamType {
    uint32_t unit;
	std::vector<std::pair<uint32_t, uint32_t>> ranges;
};

struct ExAddress {
    uint32_t vram;
    uint32_t others1;
    uint32_t others2;
    uint32_t others3;
};

struct UniversalInfo12 {
    uint32_t values0;
    uint32_t values1;
    uint32_t values2;
    uint32_t values3;
};

struct ViaData {
    uint8_t regA;
	uint8_t dirA;
	uint8_t regB;
	uint8_t dirB;
	uint8_t pcr;
	uint8_t acr;
};

struct UniversalInfo {
    const AddrMap* addrMaps;
    const RamType * ram;
    const ExAddress *exAddress;
    const UniversalInfo12 *univ12;
    uint16_t hWCfgFlagsH;
    uint8_t gId;
    uint8_t gen;
    uint16_t unknown2;
    uint8_t gen2;
	uint32_t addrMapFlags;
    uint32_t univROMFlags;
    uint32_t modelMask;
    uint32_t modelId;
    const ViaData *viaData[2];
    void (*initFunc)();
};


struct TestError {};
inline uint8_t ReadBTest(uint32_t addr) {
    if(auto p = ReadBImpl(addr, false)) {
        return p.value();
    } else {
        throw TestError{};
    }
}
inline uint16_t ReadWTest(uint32_t addr) {
    if(auto p = ReadWImpl(addr, false, false)) {
        return p.value();
    } else {
        throw TestError{};
    }
}
inline uint32_t ReadLTest(uint32_t addr) {
    if(auto p = ReadLImpl(addr, false)) {
        return p.value();
    } else {
        throw TestError{};
    }
}

consteval uint32_t FOURCC(const char* a) {
    return a[0] << 24 | a[1] << 16 | a[2] << 8 | a[3];
}
#endif
