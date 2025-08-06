#include "68040.hpp"
#include "SDL_timer.h"
#include "chip/iop.hpp"
#include "chip/scc.hpp"
#include "chip/via.hpp"
#include "inline.hpp"
#include "rom_common.hpp"
#include "rom_proto.hpp"
#include <expected>
#include <stdint.h>
#include <unordered_set>
namespace OP {
void cinva_bc(uint16_t);
void cinva_ic(uint16_t);
} // namespace OP
void Set_TTR_t(Cpu::TTR_t &x, uint32_t v);
namespace LibROM {
extern const AddrMap *gAddrMap;
extern const UniversalInfo *gUnivInfo;
extern uint32_t gAddrMapFlags;
extern uint32_t gUnivROMFlags;
extern uint32_t gHWCfgFlags;

void _4711A() {
    _4BAD0();
    auto a2 = ReadL(cpu.A[7]);
    getHwInfo2(0);
    auto a5 = ReadL(a2) + 0x10000 - 50;
    WriteL(a5 + 46, a2);
    cpu.A[7] = a5;
    WriteL(a5 + 30, ReadL(a2 - 4));
    WriteL(a5 + 42, gAddrMapFlags);
    WriteL(a5 + 38, cpu.A[1]);
    WriteL(a5 + 34, cpu.A[0]);
    cpu.D[0] = ReadB(cpu.A[1] + 18);
    switch(cpu.D[0]) {
    case 0x0C:
        cpu.A[0] = 0x2A;
        break;
    case 0x0E:
        cpu.A[0] = 0x7E;
        break;
    case 0x10:
        cpu.A[0] = 0xD2;
        break;
    case 0x0F:
        cpu.A[0] = 0x126;
        break;
    case 0x0D:
        cpu.A[0] = 0x16E;
        break;
    case 0x14:
        cpu.A[0] = 0x7E;
        break;
    }
    cpu.A[0] += 0x4663C;
    WriteL(a5 + 18, cpu.A[0]);
    cpu.A[7] -= 0x100;
    cpu.A[1] = cpu.A[7];
    // _4708E();
}

constexpr int SCC_PRINTER_CTL = 0;
constexpr int SCC_PRINTER_DATA = 4;
constexpr int SCC_MODEM_CTL = 2;
constexpr int SCC_MODEM_DATA = 6;

// $4AE96
static uint8_t serialData1[] = {0x09, 0xC0, 0x0F, 0x00, 0x04, 0x4C, 0x0B, 0x50,
                                0x0E, 0x00, 0x0C, 0x0A, 0x0D, 0x00, 0x0E, 0x01,
                                0x0A, 0x00, 0x03, 0xC1, 0x05, 0xEA, 0x01, 0x00};

void init_modem_port() {
    if(gAddrMapFlags & 1 << ADDR_IOP_SCC) {
        init_scc_iop();
    }
    scc->read(SCC_PRINTER_CTL);
    for(auto c : serialData1) {
        scc->write(SCC_MODEM_CTL, c);
    }
    cpu.D[7] |= 1 << 17;
    scc->read(SCC_MODEM_DATA);
    via1->dira[3] = true;
    via1->writePA(3, false);
}

void send_modem_port(uint8_t value) {
    scc->write(SCC_MODEM_CTL, 0x30); // ERROR RESET
    scc->write(SCC_MODEM_DATA, value);
    scc->write(SCC_MODEM_CTL, 1);
    // skip response
    while(!(scc->read(SCC_MODEM_CTL) & 1)) {
    }
    return;
}

void activate_timer2_sadmac() {
    via1->IE = 0x20;
    via1->write(8, 0xff);
    via1->write(9, 0xff);
}

constexpr uint8_t SCC_DATA_AVILABLE = 1;

std::optional<uint8_t> read_modem_port() {
    if(!(cpu.D[7] & 1 << 17)) {
        return {};
    }
    if(!(scc->read(SCC_MODEM_CTL) & SCC_DATA_AVILABLE)) {
        return {};
    }
    scc->write(SCC_MODEM_CTL, 1);
    auto rr1 = scc->read(SCC_MODEM_CTL);
    if(!(rr1 & 0x70)) {
        // no err
        // enable /W//REQ for RECV & charcter-wise IRQ
        scc->write(SCC_MODEM_CTL, 0x30);
    }
    return scc->read(SCC_MODEM_DATA);
}

int ascii_to_hex(int8_t v) {
    v = (v & 0x7f) - '0';
    if(v < 0 || v > 16) {
        return 0;
    }
    if(v <= 9) {
        return v;
    }
    return v - 7;
}

void mem_check() {
    switch(gHWCfgFlags & 0xff) { // GEN
    case 0:
    case 1:
    case 2:
    case 3:
    // unsupported
    case 5: // MCU(Do nothing)
        break;
    case 4: // GLUE[$4BB08]
        via2->writePA(6, true);
        via2->writePA(7, true);
        break;
    case 6: // OSS[$4BB16]
    {
        auto exp0 = gAddrMap->addr[ADDR_EXP0];
        for(uint16_t pos = 0, value = 0xf3ef; pos < 16; ++pos, value >>= 1) {
            WriteB(exp0, value);
        }
        WriteB(exp0 + 0x10, 0);
        break;
    }
    case 7: // V8[$4BB4E]
    {
        auto rbv = gAddrMap->addr[ADDR_RBV];
        WriteB(rbv + 1, ReadB(rbv + 1) | 0xE0);
        break;
    }
    case 8: // MCU[$4BB30]
    {
        auto ctl = gAddrMap->addr[ADDR_MCU_CTL];
        for(uint32_t i = 0, value = 0x00030810; i < 18; ++i, value >>= 1) {
            WriteL(ctl + i * 4, value);
        }
        WriteL(ctl + 0xa0, 0);
        break;
    }
    case 9: // JAWS[$4BB5C]
    {
        auto jaws = gAddrMap->addr[ADDR_JAWS_CTL];
        WriteL(jaws + 0x10000, 0);
        WriteL(jaws + 0x12000, 0);
        WriteL(jaws + 0x14000, 0);
        break;
    }
    }
    // 4BB84
    uint32_t invalid_simm_flag = 0; // D6
    uint32_t exist_simm_flag = 0;   // D5
    uint32_t ram_size = 0;          // D0
    constexpr uint32_t MAGIC = 0x54696E61;
    for(auto [a0, a1] : gUnivInfo->ram->ranges) {
        exist_simm_flag = std::rotr(exist_simm_flag, 4);
        invalid_simm_flag = std::rotr(invalid_simm_flag, 4);
        invalid_simm_flag |= ram_test_dword(a0, MAGIC);
        auto d3 = invalid_simm_flag & 0xf;
        if(!d3) {
            auto [sz, flg] =
                simm_exist_check(a0, a1, MAGIC, gUnivInfo->ram->unit);
            ram_size += sz;
            exist_simm_flag |= flg;
            continue;
        }
        if(d3 == 15) {
            invalid_simm_flag ^= d3;
        }
    }
    auto slotLen = gUnivInfo->ram->ranges.size() * 4;
    exist_simm_flag = std::rotl(exist_simm_flag, slotLen);
    invalid_simm_flag = std::rotl(invalid_simm_flag, slotLen);
    if(!invalid_simm_flag) {
        _4BC48(ram_size, exist_simm_flag);
    }
    return;
}

std::pair<uint32_t, uint8_t> simm_exist_check(uint32_t begin, uint32_t end,
                                              uint32_t magic, uint32_t unit) {
    end -= 4;
    do {
        auto d2 = ReadL(end);
        WriteL(end, magic);
        if(ReadL(end) == magic) {
            uint32_t ptr;
            for(ptr = begin - 4; ReadL(ptr) != magic; ptr += unit) {
            }
            WriteL(ptr, d2);
            auto sz = ptr + 4 - begin;
            auto d3 = 0;
            if(sz) {
                for(auto d2 = unit; d2 != ptr + 4; d2 += d2) {
                    d3++;
                }
            }
            return {sz, d3};
        }
        end -= unit;
    } while(begin < end);
    return {0, 0};
}

uint8_t ram_test_dword(uint32_t addr, uint32_t magic) {
    uint8_t v = 0;
    auto save1 = ReadL(addr);
    auto save2 = ReadL(addr + 4);
    WriteL(addr, magic);
    for(int i = 3; i >= 0; --i) {
        WriteL(addr + 4, 0xffffffff);
        if(ReadB(addr + i) == (magic & 0xff)) {
            magic ^= 0xff;
            WriteB(addr + i, ~ReadB(a0 + i));
            if(ReadB(addr + i) != (magic & 0xff)) {
                goto BAD;
            }
        } else {
        BAD:
            v |= 1 << i;
        }
        magic = std::rotr(magic, 8);
    }
    WriteL(addr, save1);
    WriteL(addr + 4, save2);
    return v;
}
constexpr uint32_t PB_DATA = 0x50F84000;

void init_pb() {
    if(!(gHWCfgFlags & 1 << 24)) {
        return;
    }
    auto vaddr = gUnivInfo->exAddress;
    auto vram = vaddr->vram;
    if(ReadL(vram - 48) == FOURCC("SARA")) {
        WriteL(vram - 48, 0);
        rom_call(ReadL(vram - 52), 0x8AC22); // really?
    }
    WriteL(vram - 48, 0);
    uint8_t hwId = gHWCfgFlags & 0xff;
    if(hwId != 9 || !(ReadB(PB_DATA) & 1)) {
        return;
    }
    getHwInfo(hwId);
    auto v = xpram_read(0x73) & 2;
    if(v == (ReadB(PB_DATA) & 2)) {
        return;
    }
    WriteB(PB_DATA, v);
    via2PortAHandshake(0xD0);
    via2PortAHandshake(0xD0);
    SDL_Delay(10000);
    cpu.PC = cpu.A[6];
    return;
}

uint32_t _4B5AE() {
    auto d0v = read_modem_port_hex(6);
    uint32_t d0 = d0v[0] << 16 | d0v[1] << 8 | d0v[2];
    if(!d0) {
        // 4B642
        _4B642(cpu.A[5]);
        return cpu.D[0];
    }
    // 4B5D2
    uint32_t d1 = ReadW(cpu.A[7]);
    cpu.A[7] += 2;
    auto a5 = ReadL(cpu.A[7] + 4); // ???
    auto d3 = cpu.D[3];
    auto a3 = cpu.A[3];
    WriteL(a5, d0);
    WriteL(a5 + 4, d1);
    if(d1) {
        d0v = read_modem_port_hex(d1);
        d0 = 0;
        for(unsigned int i = 0; i < d1; ++d1) {
            d0 = d0 << 8 | d0v[i];
        }
    }
    switch(d0) {
    case 0:
        _4B642(a5);
        break;
    case 1:
        //        _4B6AC();
        break;
    case 2:
        //        _4B6D6();
        break;
    case 3:
        //        _4B67C();
        break;
    case 4:
        //        _4B724();
        break;
    case 5:
        //        _4B73A();
        break;
    case 6:
        //        _4B78A();
        break;
    case 0x10000:
        // _C7AC0();
        break;
    case 0x10001:
        // _C7B80();
        break;
    case 0x10002:
        // _4B7E8();
        break;
    case 0x10003:
        // _4B83E();
        break;
    case 0x10004:
        // _C7C60();
        break;
    case 0x10005:
        // _C7C80();
        break;
    case 0x10006:
        // _C7CA0();
        break;
    case 0x10007:
        // _C7CC0();
        break;
    case 0x10008:
        // _C7CE0();
        break;
    case 0x10009:
        // _C7D00();
        break;
    case 0x1000A:
        // _C7D20();
        break;
    case 0x1000B:
        // _C7D50();
        break;
    case 0x1000C:
        // _C7D70();
        break;
    case 0x1000D:
        // _C7D90();
        break;
    case 0x1000E:
        // _C7DB0();
        break;
    case 0x1000F:
        // _C7DC0();
        break;
    case 0x10010:
        // _C7DD0();
        break;
    case 0x10011:
        // _C7DE0();
        break;
    case 0x10012:
        // _C7DF0();
        break;
    case 0x10013:
        // _C7E00();
        break;
    case 0x10014:
        // _C7E10();
        break;
    case 0x10015:
        // _C7B90();
        break;
    case 0x10016:
        // _C7BF0();
        break;
    }
    cpu.A[7] += 4 + ReadL(a5 + 4);
    cpu.D[3] = d3;
    cpu.A[3] = a3;
    d0 = cpu.D[0];
    read_modem_port_hex_string(4, cpu.A[7]);
    cpu.A[7] -= 8;
    return d0 ? 88 : 0;
}

void _4B642(uint32_t a5) {
    cpu.USP = cpu.A[6];
    _4711A();
    WriteW(a5 + 8, 4);
    cpu.A[7] -= 0xD800;
    WriteL(cpu.A[7], a5 + 22);
    PUSH32(a5);
    PUSH32(cpu.USP);
    PUSH32(0xD800);
    PUSH32(ReadL(a5 + 22));
    _C7AC0();
    cpu.A[7] += 8;
}
std::vector<uint8_t> read_modem_port_hex(int sz) {
    constexpr uint32_t IS_PACKED = 1 << 20;
    constexpr uint32_t IS_ECHO = 1 << 21;
    if(cpu.D[7] & IS_PACKED) {
        sz <<= 1;
    }
    std::vector<uint8_t> ret;
    uint8_t old = 0;
    for(int n = sz; n > 0; --n) {
        int8_t v = [] -> uint8_t {
            for(;;) {
                auto re = read_modem_port();
                if(re) {
                    return *re;
                }
            };
        }();
        if(cpu.D[7] & IS_ECHO) {
            send_modem_port(v);
        }
        if(cpu.D[7] & IS_PACKED) {
            if(v >= 'a') {
                v &= 0xdf;
            }
            v = ascii_to_hex(v);
            if(!(n & 1)) {
                old = v << 4;
            } else {
                ret.push_back(old | v);
            }
        } else {
            ret.push_back(v);
        }
    }
    return ret;
}

void read_modem_port_hex_string(int len, uint32_t addr) {
    constexpr uint32_t DATA_IS_HEX = 1 << 20;
    send_modem_port_crlf();
    do {
        auto d0 = ReadB(addr++);
        if(cpu.D[7] & DATA_IS_HEX) {
            send_modem_port(hex_to_ascii(d0 >> 4));
            d0 = hex_to_ascii(d0 & 0xf);
        }
        send_modem_port(d0);
        if(len > 4 && !(addr & 3)) {
            send_modem_port_crlf();
        }
    } while(--len > 0);
}

uint8_t hex_to_ascii(uint8_t v) {
    if(v < 10) {
        return v + '0';
    } else {
        return v + ('A' - 10);
    }
}

void send_modem_port_crlf() {
    send_modem_port(13);
    send_modem_port(10);
}

void _4BC48(uint32_t ram_size, uint32_t exist_simm_flag) {
    cpu.D[3] = ram_size;
    cpu.A[7] = 0x2600;
    cpu.D[2] = gUnivInfo->ram->unit;
    auto a1 = gUnivInfo->ram->ranges.begin();
    switch(gHWCfgFlags & 0xff) { // GEN
    case 0:
    case 1:
    case 2:
    case 3:
        // unsupported
        // $4BC8A
        cpu.D[1] = 20;
        break;
    case 4: // GLUE[$4BC90]
    {
        static uint8_t MemMask[] = {0xff, 0x3f, 0xff, 0x7f,
                                    0xff, 0xbf, 0xff, 0xff};
        cpu.D[1] = exist_simm_flag & 15;
        if(cpu.D[1]) {
            a1++;
            exist_simm_flag >>= 4;
        }
        via2->wirte(0x1E00, via2->read(0x1E00) & MemMask[cpu.D[1]]);
        break;
    }
    case 5: // MCU[$4BCB4]
    {
        cpu.A[5] = 0;
        a1 += 2;
        for(;;) {
            do {
                if(!exist_simm_flag & oxff) {
                    goto _4BEE4;
                }
                --a1;
                exist_simm_flag <<= 4;
            } while(!((exist_simm_flag >> 8) & 0xf));
            _4BEAA();
            auto a5 = cpu.A[5];
            if(a5 == 0) {
                a5 = a1->first + cpu.D[0];
                WriteL(a5 -= 4, 0);
                WriteL(a5 -= 4, 0);
                WriteL(a5 -= 4, -1);
            }
            WriteL(a5 -= 4, cpu.D[0]);
            WriteL(a5 -= 4, a1->first);
        }
        break;
    }
    case 6: // OSS[$4BCEE]
    {
        cpu.D[0] = ram_size & 1 << 20 ? 0xF3FF : 0xF3EF;
        auto d1 = exist_simm_flag & 0xf;
        if(d1 == 0) {
            ++a1;
            exist_simm_flag >>= 4;
        }
        static uint8_t MemMask[] = {0xff, 0xff, 0xff, 0x3f,
                                    0x7f, 0xbf, 0xff, 0xff};
        ram_size &= MemMask[d1];
        auto a5 = gAddrMap->addr[ADDR_EXP0];
        for(int i = 0; i < 16; ++i, ram_size >>= 1) {
            WriteB(a5, ram_size);
        }
        WriteB(a5 + 0x10, 0xff);
        cpu.D[1] = 28;
        break;
    }
    case 7: // V8[$4BD92]
    {
        
        break;
    }
    case 8: // MCU[$4BD2C]
    {
        auto d1 = exist_simm_flag & 0xffff;
        exist_simm_flag = std::rotl(exist_simm_flag, 16);
        exist_simm_flag = (exist_simm_flag & ~0xffff) | d1;
        auto d6 = cpu.A[1];
        auto sz = 0;
        auto d3 = 0;

        // 4BD6C
        for(;;) {
            cpu.D[1] = exist_simm_flag & 15;
            exist_simm_flag >>= 4;
            _4BEAA();
            sz += ram_size;
            if(a1 + 1 == gUnivInfo->ram->ranges.end()) {
                break;
            }
            ++a1;

            // 4BD3A
            auto mcu = gAddrMap->addr[ADDR_MCU_CTL] + (d3++) * 6 * 4;
            auto d1 = sz >> 22;
            for(int i = 0; i < 6; ++i, mcu += 4, d1 >>= 1) {
                WriteL(mcu, d1);
            }
            WriteB(gAddrMap->addr[ADDR_MCU_CTL] + 0xA0, 0xff);
        }

        cpu.D[1] = 44;
        cpu.D[3] = cpu.A[3];
        cpu.A[1] = d6;
        break;
    }
    case 9: // JAWS[$4BDC8]
        break;
    }
    // $4BEB8

_4BEE4:;
}
} // namespace LibROM
