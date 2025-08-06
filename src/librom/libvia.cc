#include "68040.hpp"
#include "SDL_timer.h"
#include "chip/via.hpp"
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
extern bool rtcEnable;
void transmitAdb(uint8_t adbState, uint8_t v);
uint8_t recieveAdb(uint8_t adbState);
uint8_t do_rtc(uint8_t cmd, uint8_t v0, uint8_t v1);
void adb_run(const std::vector<uint8_t> &b);

uint8_t xpram_read(uint8_t reg);
void xpram_write(uint8_t reg, uint8_t v);

namespace LibROM {
extern const AddrMap *gAddrMap;
extern const UniversalInfo *gUnivInfo;

constexpr int VIA1A_CPUID0 = 1;
constexpr int VIA1A_CPUID1 = 2;
constexpr int VIA1A_SYNC = 3;
constexpr int VIA1A_OVERLAY = 4;
constexpr int VIA1A_CPUID2 = 4;
constexpr int VIA1A_HEALSEL = 5;
constexpr int VIA1A_CPUID13 = 6;
constexpr int VIA1A_SCCWrREQ = 7;

constexpr int VIA1B_RTCDATA = 0;
constexpr int VIA1B_RTCCLK = 1;
constexpr int VIA1B_RTCEN = 2;
constexpr int VIA1B_ADBINT = 3;
constexpr int VIA1B_ADBST0 = 4;
constexpr int VIA1B_ADBST1 = 5;
constexpr int VIA1B_PAREN = 6;
constexpr int VIA1B_PARERR = 7;
constexpr int VIA1B_SNDENB = 7;

constexpr int VIA2A_IRQ1 = 0;
constexpr int VIA2A_IRQ2 = 1;
constexpr int VIA2A_IRQ3 = 2;
constexpr int VIA2A_IRQ4 = 3;
constexpr int VIA2A_IRQ5 = 4;
constexpr int VIA2A_IRQ6 = 5;
constexpr int VIA2A_IRQ0 = 6;
constexpr int VIA2A_RAM0 = 6;
constexpr int VIA2A_RAM1 = 7;

constexpr int VIA2B_CACHE_DIS = 0;
constexpr int VIA2B_BUS_LOCK = 1;
constexpr int VIA2B_POWER_OFF = 2;
constexpr int VIA2B_MMU_CTL = 3;
constexpr int VIA2B_NUBUS1 = 4;
constexpr int VIA2B_NUBUS0 = 5;
constexpr int VIA2B_SNDOUT = 6;
constexpr int VIA2B_VBL = 7;

bool JawsHandshake() {
    via2->write(3, 0);
    constexpr int TRA_COUNT = 3200;
    for(int cnt = 0; cnt < TRA_COUNT; ++cnt) {
        if(!via2->readPB(VIA2B_BUS_LOCK)) {
            continue;
        }
        if(via2->read(1) == 0xff) {
            via1->dira[VIA1A_SCCWrREQ] = false;
            if(!via2PortAHandshake(0x10)) {
                return false;
            }
            if(!via2PortAHandshake(1)) {
                return false;
            }
            return via2PortAHandshake(0xB3);
        }
    }
    // time out
    return false;
}

bool via2PortAHandshake(uint8_t v) {
    bool ok = false;
    via2->write(3, 0xff);
    via2->write(1, v);
    via2->writePB(2, false);
    for(int i = 0; i < 1024; ++i) {
        if(via2->readPB(1)) {
		    via2->writePB(2, true);
            for(int j = 0; j < 64; ++j) {
                if(via2->readPB(1)) {
                    ok = true;
                    goto DONE;
                }
            }
            break;
        }
    }
DONE:
    via2->writePB(2, true);
    via2->write(3, 0);
    return ok;
}

void rtc_send(uint8_t v) {
    for(int i = 7; i >= 0; --i) {
        via1->writePB(0, v >> i & 1);
        via1->writePB(1, false);
        via1->writePB(1, true);
    }
}

uint8_t rtc_recv() {
    uint8_t ret = 0;
    via1->dirb[0] = false;
    for(int i = 7; i >= 0; --i) {
        via1->writePB(1, false);
        via1->writePB(1, true);
        ret = ret << 1 | via1->readPB(0);
    }
    return ret;
}

uint8_t xpram_read(uint32_t offset) {
    return ::xpram_read(offset);
}

void xpram_send_sr(uint8_t cmd) { transmitAdb(2, cmd); }

uint8_t xpram_recv_sr() {
    cpu.Z = true;
    return recieveAdb(1);
}
void throw_adb_data() {}

uint8_t xpram_recv_sr_blocked() { return xpram_recv_sr(); }
} // namespace LibROM
