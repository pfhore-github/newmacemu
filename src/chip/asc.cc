#include "chip/asc.hpp"
#include "SDL_timer.h"
#include "chip/via.hpp"
#include <vector>

std::shared_ptr<EASC> easc;
constexpr int DEFAULT_FREQ = 44100;
void asc_reset() { easc->reset(); }

void EASC::disconnect() {
    SDL_CloseAudioDevice(speaker);
    if(mic != 0) {
        SDL_CloseAudioDevice(mic);
    }
}
EASC::~EASC() { disconnect(); }
void EASC::reset() {
    chA.reset();
    chB.reset();
    disconnect();
}
void EASC_CH::reset() {
    memset(FIFO, 0, 0x400);
    FIFO_READP = 0;
    FIFO_WRITEP = 0;
    left_vol = 0;
    right_vol = 0;
    FIFO_PLAYP = 0.0;
    FIFO_SIZE = 0;
    fifo_half.store(false);
    fifo_empty.store(false);
}

void EASC_CH::fifo_write(uint8_t v) {
    FIFO[FIFO_WRITEP] = v;

    FIFO_WRITEP = (FIFO_WRITEP + 1) & 0x3ff;
    if(++FIFO_SIZE == 0x3ff) {
        fifo_empty.store(true);
    }
}

uint8_t EASC_CH::read_reg(int t) {
    switch(t) {
    case 0:
        return GET_B1_W(FIFO_WRITEP);
    case 1:
        return GET_B2_W(FIFO_WRITEP);
    case 2:
        return GET_B1_W(FIFO_READP);
    case 3:
        return GET_B2_W(FIFO_READP);
    case 4:
        return GET_B1_W(SAMPLE_RATE);
    case 5:
        return GET_B2_W(SAMPLE_RATE);
    case 6:
        return left_vol;
    case 7:
        return right_vol;
    case 8:
        return fifo_cdxa << 7;
    case 9:
        return IRQ_CTL;
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x16:
    case 0x17:
        return CD_XA_const[t & 0x7];
    }
    return 0;
}
static void do_fifo(void *p, uint8_t *stream, int len) {
    auto asc = static_cast<ASC_common *>(p);
    asc->fifo_play(reinterpret_cast<int16_t *>(stream), len / 2);
}

ASC_common::ASC_common() {
    speakerSpec.freq = DEFAULT_FREQ;
    speakerSpec.format = AUDIO_S16SYS;
    speakerSpec.channels = 2;
    speakerSpec.samples = 0x200;
    speakerSpec.callback = do_fifo;
    speakerSpec.userdata = static_cast<ASC_common *>(this);
}
EASC::EASC() {
    speaker =
        SDL_OpenAudioDevice(nullptr, false, &speakerSpec, &speakerSpec, 0);
    SDL_PauseAudioDevice(speaker, true);

    micSpec.freq = 22050;
    micSpec.format = AUDIO_U8;
    micSpec.channels = 1;
    micSpec.samples = 1024;
    micSpec.callback = nullptr;
}

// TODO: mic?
uint8_t EASC::read(uint32_t addr) {
    addr &= 0xfff;
    if(addr < 0x400) {
        uint8_t v = chA.FIFO[chA.FIFO_READP];
        chA.FIFO_READP = (chA.FIFO_READP + 1) & 0x3ff;
        if(chA_is_mic && chA.FIFO_READP == 0) {
            SDL_DequeueAudio(mic, chA.FIFO, 1024);
        }
        return v ^ 0x80;
    } else if(addr < 0x800) {
        uint8_t v = chB.FIFO[chB.FIFO_READP];
        chB.FIFO_READP = (chB.FIFO_READP + 1) & 0x3ff;
        return v;
    } else if((addr & 0xF30) == 0xF00) {
        return chA.read_reg(addr & 0x1F);
    } else if((addr & 0xF30) == 0xF20) {
        return chB.read_reg(addr & 0x1F);
    }
    switch(addr) {
    case 0x800:
        return 0xB0;
    case 0x801:
        return enabled;
    case 0x802:
        return overflow << 7;
    case 0x804: {
        auto v1 = chA.fifo_half.exchange(false);
        auto v2 = chA.fifo_empty.exchange(false);
        auto v3 = chB.fifo_half.exchange(false);
        auto v4 = chB.fifo_empty.exchange(false);
        return v1 | v2 << 1 | v3 << 2 | v4 << 3;
    }
    case 0x806:
        return volume << 5;
    case 0x807:
        return 3;
    case 0x80A:
        return chA_is_mic | chA_is_22 << 1;
    }
    return 0;
}
void IRQ(int i);

void EASC_CH::write_reg(int t, uint8_t v) {
    switch(t) {
    case 0:
        FIFO_WRITEP = SET_B1_W(FIFO_WRITEP, v);
        return;
    case 1:
        FIFO_WRITEP = SET_B2_W(FIFO_WRITEP, v);
        return;
    case 2:
        FIFO_READP = SET_B1_W(FIFO_READP, v);
        return;
    case 3:
        FIFO_READP = SET_B2_W(FIFO_READP, v);
        return;
    case 4:
        SAMPLE_RATE = SET_B1_W(SAMPLE_RATE, v);
        return;
    case 5:
        SAMPLE_RATE = SET_B2_W(SAMPLE_RATE, v);
        return;
    case 6:
        left_vol = v;
        return;
    case 7:
        right_vol = v;
        return;
    case 8:
        fifo_cdxa = v & 0x80 ? true : false;
        return;
    case 9:
        IRQ_CTL = v & 1;
        return;
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x16:
    case 0x17:
        CD_XA_const[t & 0x0f] = v;
        return;
    }
}
int8_t EASC_CH::fifo_read() {
    int8_t v = FIFO[(int)FIFO_PLAYP] ^ 0x80;
    float newpos = FIFO_PLAYP + (float)SAMPLE_RATE / DEFAULT_FREQ;
    for(int cnt = (int)newpos - (int)FIFO_PLAYP; cnt > 0; --cnt) {
        if(FIFO_SIZE) {
            --FIFO_SIZE;
        }
        if(FIFO_SIZE == 0x1FF) {
            fifo_half.store(true);
            if(IRQ_CTL) {
                via2->irq(VIA_IRQ::CB1);
            }
        }
        if(FIFO_SIZE == 1) {
            fifo_half.store(true);
            fifo_empty.store(true);
            if(IRQ_CTL) {
                via2->irq(VIA_IRQ::CB1);
            }
        }
    }
    FIFO_PLAYP = fmod(newpos, 0x400);
    return v;
}
void ASC_common::fifo_play(int16_t *dst, int len) {
    for(int i = 0; i < len / 2; i++) {
        int8_t cha_v = chA.fifo_read();
        int8_t chb_v = chB.fifo_read();
#if 0
// TODO: mix?
        int16_t left = cha_v * chA.left_vol + chb_v * chB.left_vol;
        int16_t right = cha_v * chA.right_vol + chb_v * chB.right_vol;
        dst[2 * i] = left * volume / 8;
        dst[2 * i + 1] = right * volume / 8;
#else
        dst[2 * i] = cha_v * 256;
        dst[2 * i + 1] = chb_v * 256;
#endif
    }
}

void EASC::write(uint32_t addr, uint8_t v) {
    addr &= 0xfff;
    if(addr < 0x400) {
        chA.fifo_write(v);
    } else if(addr < 0x800) {
        chB.fifo_write(v);
    } else if((addr & 0xF30) == 0xF00) {
        chA.write_reg(addr & 0x1F, v);
    } else if((addr & 0xF30) == 0xF20) {
        chB.write_reg(addr & 0x1F, v);
    } else {
        switch(addr) {
        case 0x801:
            enabled = v & 1;
            SDL_PauseAudioDevice(speaker, !enabled);
            SDL_PauseAudioDevice(mic, !enabled);
            if(enabled) {
                chA.reset();
                chB.reset();
            }
            break;
        case 0x803:
            if(v & 1 << 7) {
                chA.reset();
                chB.reset();
            }
            break;
        case 0x804:
            chA.fifo_half.store(v & 1);
            chA.fifo_empty.store(v & 2);
            chB.fifo_half.store(v & 4);
            chA.fifo_empty.store(v & 8);
            break;
        case 0x806:
            volume = v >> 5;
            break;
        case 0x80A:
            if(mic != 0) {
                SDL_CloseAudioDevice(mic);
            }
            chA_is_mic = v & 1;
            chA_is_22 = v & 2;
            if(chA_is_mic) {
                micSpec.freq = chA_is_22 ? 22050 : 11025;
                mic = SDL_OpenAudioDevice(nullptr, true, &micSpec, &micSpec, 0);
            }
            break;
        }
    }
}
static void do_wave(void *p, uint8_t *stream, int len) {
    auto asc = static_cast<ASC *>(p);
    asc->wave_play(reinterpret_cast<int16_t *>(stream), len / 2);
}
ASC::ASC() {
    wavSpeakerSpec.freq = 22257;
    wavSpeakerSpec.format = AUDIO_S16SYS;
    wavSpeakerSpec.channels = 1;
    wavSpeakerSpec.samples = 4096;
    wavSpeakerSpec.callback = do_wave;
    wavSpeakerSpec.userdata = this;
}

void ASC::disconnect() { SDL_CloseAudioDevice(speaker); }

// TODO: mic?
uint8_t ASC::read(uint32_t addr) {
    addr &= 0xfff;
    if(addr < 0x400) {
        return chA.FIFO[addr];
    } else if(addr < 0x800) {
        return chB.FIFO[addr & 0x3ff];
    } else if((addr & 0xF30) == 0xF00) {
        return chA.read_reg(addr & 0x1F);
    } else if((addr & 0xF30) == 0xF20) {
        return chB.read_reg(addr & 0x1F);
    }
    switch(addr) {
    case 0x800:
        return 0;
    case 0x801:
        return mode;
    case 0x802:
        return ctl;
    case 0x804:
        return chA.fifo_half.exchange(false) |
               chA.fifo_empty.exchange(false) << 1 |
               chB.fifo_half.exchange(false) << 2 |
               chB.fifo_empty.exchange(false) << 3;
    case 0x805:
        return wav_run.load();
    case 0x806:
        return volume << 2;
    case 0x807:
        return freq;
    case 0x80A:
        return 0;
    case 0x810:
    case 0x818:
    case 0x820:
    case 0x828:
        return GET_B1(phase[(addr - 0x810) >> 3]);
    case 0x811:
    case 0x819:
    case 0x821:
    case 0x829:
        return GET_B2(phase[(addr - 0x810) >> 3]);
    case 0x812:
    case 0x81A:
    case 0x822:
    case 0x82A:
        return GET_B3(phase[(addr - 0x810) >> 3]);
    case 0x813:
    case 0x81B:
    case 0x823:
    case 0x82B:
        return GET_B4(phase[(addr - 0x810) >> 3]);
    case 0x814:
    case 0x81C:
    case 0x824:
    case 0x82C:
        return GET_B1(incr[(addr - 0x814) >> 3]);
    case 0x815:
    case 0x81D:
    case 0x825:
    case 0x82D:
        return GET_B2(incr[(addr - 0x814) >> 3]);
    case 0x816:
    case 0x81E:
    case 0x826:
    case 0x82E:
        return GET_B3(incr[(addr - 0x814) >> 3]);
    case 0x817:
    case 0x81F:
    case 0x827:
    case 0x82F:
        return GET_B4(incr[(addr - 0x814) >> 3]);
    }
    return 0;
}

void ASC::wave_play(int16_t *dst, int len) {
    for(int i = 0; i < len; i++) {
        int16_t mix = 0;
        for(int ch = 0; ch < 4; ch++) {
            if(wav_run.load() & (1 << ch)) {
                phase[ch] += incr[ch];
                int off = ((phase[ch] >> 15) & 0x1ff) + (ch & 1 ? 0x200 : 0);
                int8_t smpl =
                    ch < 2 ? (int8_t)chA.FIFO[off] : (int8_t)chB.FIFO[off];

                smpl ^= 0x80;
                mix += smpl;
            }
        }
        *dst++ = mix * volume;
    }
}

void ASC::reopen() {
    SDL_PauseAudioDevice(speaker, 1);
    SDL_CloseAudioDevice(speaker);
    chA.reset();
    chB.reset();
    switch(mode) {
    case 0: // stop
        speaker = 0;
        break;
    case 1: // FIFO
        speaker =
            SDL_OpenAudioDevice(nullptr, false, &speakerSpec, &speakerSpec, 0);
        SDL_PauseAudioDevice(speaker, 0);
        break;
    case 2: // WaveTable
        speaker = SDL_OpenAudioDevice(nullptr, false, &wavSpeakerSpec,
                                      &wavSpeakerSpec, 0);
        SDL_PauseAudioDevice(speaker, 0);
        break;
    }
}
void ASC::write(uint32_t addr, uint8_t v) {
    addr &= 0xfff;
    if(addr < 0x400) {
        if(mode == 1) {
            chA.fifo_write(v);
        } else {
            chA.FIFO[addr] = v;
        }
    } else if(addr < 0x800) {
        if(mode == 1) {
            chB.fifo_write(v);
        } else {
            chB.FIFO[addr & 0x3ff] = v;
        }
    } else if((addr & 0xF30) == 0xF00) {
        chA.write_reg(addr & 0x1F, v);
    } else if((addr & 0xF30) == 0xF20) {
        chB.write_reg(addr & 0x1F, v);
    } else {
        switch(addr) {
        case 0x801:
            mode = v;
            reopen();
            break;
        case 0x803:
            if(v & 1 << 7) {
                chA.reset();
                chB.reset();
            }
            break;
        case 0x804:
            chA.fifo_half.store(v & 1);
            chA.fifo_empty.store(v & 2);
            chB.fifo_half.store(v & 4);
            chA.fifo_empty.store(v & 8);
            break;
        case 0x805:
            wav_run.store(v);
            break;
        case 0x806:
            volume = v >> 2;
            break;
        case 0x807:
            wavSpeakerSpec.freq = speakerSpec.freq = v == 0   ? 22257
                                                     : v == 2 ? 22050
                                                              : 44100;
            reopen();
            break;
        case 0x810:
        case 0x818:
        case 0x820:
        case 0x828:
            phase[(addr - 0x810) >> 3] = SET_B1(phase[(addr - 0x810) >> 3], v);
            break;
        case 0x811:
        case 0x819:
        case 0x821:
        case 0x829:
            phase[(addr - 0x810) >> 3] = SET_B2(phase[(addr - 0x810) >> 3], v);
            break;
        case 0x812:
        case 0x81A:
        case 0x822:
        case 0x82A:
            phase[(addr - 0x810) >> 3] = SET_B3(phase[(addr - 0x810) >> 3], v);
            break;
        case 0x813:
        case 0x81B:
        case 0x823:
        case 0x82B:
            phase[(addr - 0x810) >> 3] = SET_B4(phase[(addr - 0x810) >> 3], v);
            break;
        case 0x814:
        case 0x81C:
        case 0x824:
        case 0x82C:
            incr[(addr - 0x810) >> 3] = SET_B1(incr[(addr - 0x810) >> 3], v);
            break;
        case 0x815:
        case 0x81D:
        case 0x825:
        case 0x82D:
            incr[(addr - 0x810) >> 3] = SET_B2(incr[(addr - 0x810) >> 3], v);
            break;
        case 0x816:
        case 0x81E:
        case 0x826:
        case 0x82E:
            incr[(addr - 0x810) >> 3] = SET_B3(incr[(addr - 0x810) >> 3], v);
            break;
        case 0x817:
        case 0x81F:
        case 0x827:
        case 0x82F:
            incr[(addr - 0x810) >> 3] = SET_B4(incr[(addr - 0x810) >> 3], v);
            break;
        }
    }
}
