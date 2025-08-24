#include "68040.hpp"
#include "chip/asc.hpp"
#include "inline.hpp"
#include "rom_common.hpp"
#include "rom_proto.hpp"
#include <expected>
#include <stdint.h>
#include <unordered_set>
void Set_TTR_t(Cpu::TTR_t &x, uint32_t v);
namespace LibROM {
extern const AddrMap *gAddrMap;
extern const UniversalInfo *gUnivInfo;

void play_asc_chime1() {
    static const AscChime asc_chime = {
        0x0607, 39, 0, 5000, 4, {0xC09C, 0xE0B6, 0x120EA, 0x182B1}};
    play_asc_chime(&asc_chime);
}

void play_asc_chime4() {
    static const AscChime asc_chime = {
         0x0607, 43, 3000, 25000, 4, {82927, 103659, 124391, 166231}};
    play_asc_chime(&asc_chime);
}
void play_asc_chime(const AscChime *p) {
    asc->write(0x801, 0);
    asc->write(0x807, 0);
    uint16_t volume = p->volume << 2;
    auto version = asc->read(0x800);
    if(version == 0) {
        volume >>= 5;
    }
    asc->volume = volume;
    asc->overflow = version != 0 ? 1 : 0;
    for(int i = 0; i < 4; ++i) {
        asc->phase[i] = 0;
        asc->incr[i] = 0;
    }
    // unknown field
    for(int i = 0; i < 8; ++i) {
        //        asc->pad[i] = -2;
    }
    asc->write(0x801, 2);
    asc->write(0x803, 0);
    uint32_t d0 = version == 0 ? 0x30013f10 : 0xc001ff40;
    for(int i = 0; i < 32; ++i) {
        for(int j = 0; j < 64; ++j) {
            if(i < 16) {
                asc->chA.FIFO[i * 64 + j] = d0;
            } else {
                asc->chB.FIFO[(i - 16) * 64 + j] = d0;
            }
        }
        d0 = d0 >> 8 | d0 << 24;
    }
    int16_t ch = 0;
    int delay = 0;
    size_t v0 = 0;
    long wait = (p->wait) * 1000000000LL / 783360;
    for(int cnt = 0; cnt < p->length; ++cnt) {
        // $0711C
        if(delay-- == 0 && ch < p->ch) {
            delay = p->delay;
            asc->incr[ch] = p->values[ch];
            ch++;
        }

        // $070F8
        uint8_t d4 = asc->chA.FIFO[v0++];
        if(v0 >= 0x0200) {
            v0 -= 0x0200;
        }
        d4 = (d4 + asc->chA.FIFO[v0]) >> 1;
        asc->chB.FIFO[v0 + 0x0200] = d4;
        asc->chB.FIFO[v0] = d4;
        asc->chA.FIFO[v0 + 0x0200] = d4;
        asc->chA.FIFO[v0] = d4;
        /* wait (p->wait)/2 us */
        SDL_DelayPrecise(wait);
    }
    for(int i = 0; i < 4; ++i) {
        asc->incr[i] = 0;
    }
    asc->write(0x801, 0);
}
} // namespace LibROM
