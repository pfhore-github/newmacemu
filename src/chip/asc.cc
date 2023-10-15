#include "chip/asc.hpp"
#include "SDL_timer.h"
#include "chip/via.hpp"
#include <algorithm>
#include <utility>
#include <vector>
std::shared_ptr<ASC> asc;
constexpr int DEFAULT_FREQ = 22257;
void asc_reset() { asc->reset(); }

ASC::~ASC() {
    SDL_CloseAudioDevice(speaker);
    SDL_CloseAudioDevice(mic);
}
void ASC::reset() {
    SDL_LockAudioDevice(speaker);
    chA.reset();
    chB.reset();
    for(int i = 0; i < 4; ++i) {
        phase[i] = incr[i] = 0;
    }
    SDL_PauseAudioDevice(speaker, true);
    SDL_UnlockAudioDevice(speaker);
    if(mic) {
        SDL_PauseAudioDevice(mic, true);
    }
}
void ASC_CH::reset() {
    memset(FIFO, 0, 0x400);
    FIFO_READP = 0;
    FIFO_WRITEP = 0;
    left_vol = 0;
    right_vol = 0;
    FIFO_SIZE = 0;
    FIFO_PLAYP = 0;
    fifo_half.store(false);
    fifo_full.store(false);
    xa_last[0] = xa_last[1] = 0;
    chStream =
        SDL_NewAudioStream(AUDIO_S8, 2, 44100, AUDIO_S16SYS, 2, DEFAULT_FREQ);
}
void ASC_CH::fifo_write(uint8_t v) {
    FIFO[FIFO_WRITEP] = v ^ 0x80;

    FIFO_WRITEP = (FIFO_WRITEP + 1) & 0x3ff;
    FIFO_SIZE++;
    fifo_half.store(FIFO_SIZE <= 0x1ff);
    if(FIFO_SIZE == 0x200 && IRQ_CTL) {
        via2->irq(VIA_IRQ::CB1);
    }
    if(FIFO_SIZE == 0x400) {
        FIFO_SIZE = 0;
        fifo_full.store(true);
        via2->irq(VIA_IRQ::CB1);
        SDL_AudioStreamPut(chStream, FIFO, 0x400);
//        SDL_AudioStreamFlush(chStream);
    }
}

uint8_t ASC_CH::read_reg(int t) {
    switch(t) {
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
    SDL_memset(stream, 0, len);
    auto asc = static_cast<ASC *>(p);
    asc->fifo_play(stream, len);
}

static void do_wave(void *p, uint8_t *stream, int len) {
    SDL_memset(stream, 0, len);
    auto asc = static_cast<ASC *>(p);
    asc->wave_play(stream, len);
}

ASC::ASC() {
    speakerSpec.freq = DEFAULT_FREQ;
    speakerSpec.format = AUDIO_S16SYS;
    speakerSpec.channels = 2;
    speakerSpec.samples = 0x100;
    speakerSpec.callback = do_fifo;
    speakerSpec.userdata = this;

    speaker =
        SDL_OpenAudioDevice(nullptr, false, &speakerSpec, &speakerSpec, 0);
    SDL_PauseAudioDevice(speaker, 1);

    wavSpec.freq = 22257;
    wavSpec.format = AUDIO_S8;
    wavSpec.channels = 1;
    wavSpec.samples = 4096;
    wavSpec.callback = do_wave;
    wavSpec.userdata = this;

    speakerW = SDL_OpenAudioDevice(nullptr, false, &wavSpec, &wavSpec, 0);
    SDL_PauseAudioDevice(speakerW, 1);

    micSpec.freq = 22050;
    micSpec.format = AUDIO_U8;
    micSpec.channels = 1;
    micSpec.samples = 1024;
    micSpec.callback = nullptr;
}

uint8_t ASC::read(uint32_t addr) {
    if(addr < 0x400) {
        if(mode == 1) {
            uint8_t v = chA.FIFO[chA.FIFO_READP];
            chA.FIFO_READP = (chA.FIFO_READP + 1) & 0x3ff;
            // TODO: mic?
            if(chA_is_mic && chA.FIFO_READP == 0) {
                SDL_DequeueAudio(mic, chA.FIFO, 1024);
            }
            return v ^ 0x80;
        } else {
            return chA.FIFO[addr];
        }
    } else if(addr < 0x800) {
        if(mode == 2) {
            return chB.FIFO[addr & 0x3ff];
        } else {
            return 0;
        }
    } else if((addr & 0xF30) == 0xF00) {
        return chA.read_reg(addr & 0x1F);
    } else if((addr & 0xF30) == 0xF20) {
        return chB.read_reg(addr & 0x1F);
    }
    switch(addr & 0x3f) {
    case 0:
        return isEASC ? 0xB0 : 0;
    case 1:
        return mode;
    case 2:
        return overflow.load() << 7;
    case 4: {
        via2->IF[int(VIA_IRQ::CB1)] = false;
        uint8_t v1 = chA.fifo_half | chA.fifo_full << 1 | chB.fifo_half << 2 |
                     chB.fifo_full << 3;
        chA.fifo_full = false;
        chB.fifo_full = false;
        chA.fifo_half = false;
        chB.fifo_half = false;
        return v1;
    }
    case 0x11:
    case 0x12:
    case 0x13:
        return GET_Bn(phase[0], addr & 3);
    case 0x15:
    case 0x16:
    case 0x17:
        return GET_Bn(incr[0], addr & 3);
    case 0x19:
    case 0x1A:
    case 0x1B:
        return GET_Bn(phase[1], addr & 3);
    case 0x1D:
    case 0x1E:
    case 0x1F:
        return GET_Bn(incr[1], addr & 3);
    case 0x21:
    case 0x22:
    case 0x23:
        return GET_Bn(phase[2], addr & 3);
    case 0x25:
    case 0x26:
    case 0x27:
        return GET_Bn(incr[2], addr & 3);
    case 0x29:
    case 0x2A:
    case 0x2B:
        return GET_Bn(phase[3], addr & 3);
    case 0x2D:
    case 0x2E:
    case 0x2F:
        return GET_Bn(incr[3], addr & 3);
    }
    return 0;
}
void IRQ(int i);

void ASC_CH::write_reg(int t, uint8_t v) {
    switch(t) {
    case 4:
        SAMPLE_RATE = SET_B1_W(SAMPLE_RATE, v);
        SDL_FreeAudioStream(chStream);
        chStream = SDL_NewAudioStream(AUDIO_S8, 1, SAMPLE_RATE, AUDIO_S16SYS, 1,
                                      DEFAULT_FREQ);
        return;
    case 5:
        SAMPLE_RATE = SET_B2_W(SAMPLE_RATE, v);
        SDL_FreeAudioStream(chStream);
        chStream = SDL_NewAudioStream(AUDIO_S8, 1, SAMPLE_RATE, AUDIO_S16SYS, 1,
                                      DEFAULT_FREQ);
        return;
    case 6:
        left_vol = v;
        return;
    case 7:
        right_vol = v;
        return;
    case 8:
        fifo_cdxa = v & 2 ? true : false;
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

int8_t ASC_CH::cdxa_read() {
    if(xa_cnt == -1) {
        xa_flags = fifo_read();
        xa_cnt = 0;
    }
    int shift = xa_flags & 0xf;
    int filter = xa_flags >> 5;
    int16_t f0 = CD_XA_const[(filter << 1) + 1];
    int16_t f1 = CD_XA_const[(filter << 1)];
    int16_t d;
    if((xa_cnt & 1) == 0) {
        xa_val = fifo_read();
        d = (xa_val & 0xf) << 12;
    } else {
        d = (xa_val & 0xf0) << 8;
    }
    int32_t t =
        (d >> shift) + (((xa_last[0] * f0) + (xa_last[1] * f1) + 32) >> 6);
    t = std::clamp(t, -32768, 23767);
    xa_last[1] = xa_last[0];
    xa_last[0] = t;
    xa_cnt++;
    if(xa_cnt == 28) {
        xa_cnt = -1;
    }
    return static_cast<int8_t>(t / 256);
}
int8_t ASC_CH::fifo_read() {

    if(FIFO_SIZE) {
        FIFO_SIZE--;

        auto v = FIFO[FIFO_PLAYP];
        FIFO_PLAYP = (FIFO_PLAYP + 1) & 0x3ff;
        fifo_half.store(FIFO_SIZE <= 0x1ff);
        if(FIFO_SIZE == 0x1ff && IRQ_CTL) {
            via2->irq(VIA_IRQ::CB1);
        }
        if(FIFO_SIZE == 0) {
            fifo_full.store(true);
            via2->irq(VIA_IRQ::CB1);
        }
        return v;
    } 
    return 0;
}

void ASC::fifo_play(uint8_t *dstp, int len) {
    int ln = len / 4;
    if(SDL_AudioStreamAvailable(chA.chStream) == 0 &&
       SDL_AudioStreamAvailable(chB.chStream) == 0) {
        overflow.store(true);
        chA.fifo_half.store(true);
        chB.fifo_half.store(true);
        chA.fifo_full.store(true);
        chB.fifo_full.store(true);
        via2->irq(VIA_IRQ::CB1);
        return;
    }

    std::vector<int16_t> v1(ln), v2(ln);
    SDL_AudioStreamGet(chA.chStream, v1.data(), v1.size() * 2);
    if(SDL_AudioStreamAvailable(chA.chStream) < 0x200) {
        chA.fifo_half.store(true);
        if(chA.IRQ_CTL) {
            via2->irq(VIA_IRQ::CB1);
        }
    }
    SDL_AudioStreamGet(chB.chStream, v2.data(), v2.size() * 2);
    if(SDL_AudioStreamAvailable(chB.chStream) < 0x200) {
        chB.fifo_half.store(true);
        if(chB.IRQ_CTL) {
            via2->irq(VIA_IRQ::CB1);
        }
    }
    int16_t *p = reinterpret_cast<int16_t *>(dstp);
    for(int i = 0; i < ln; i++) {
        int16_t cha_v = v1[i];
        int16_t chb_v = v2[i];
        int32_t left = cha_v * chA.left_vol + chb_v * chB.left_vol;
        int32_t right = cha_v * chA.right_vol + chb_v * chB.right_vol;
        *p++ = left / 256;
        *p++ = right / 256;
    }
    SDL_MixAudioFormat(dstp, dstp, AUDIO_S16SYS, len, volume);
}

void ASC::write(uint32_t addr, uint8_t v) {
    if(addr < 0x400) {
        if(mode != 2) {
            chA.fifo_write(v);
        } else {
            chA.FIFO[addr] = v;
        }
    } else if(addr < 0x800) {
        if(mode != 2) {
            chB.fifo_write(v);
        } else {
            chB.FIFO[addr & 0x3ff] = v;
        }
    } else if((addr & 0xF30) == 0xF00) {
        chA.write_reg(addr & 0x1F, v);
    } else if((addr & 0xF30) == 0xF20) {
        chB.write_reg(addr & 0x1F, v);
    } else {
        switch(addr & 0x3f) {
        case 1:
            mode = v;
            SDL_PauseAudioDevice(speaker, mode != 1);
            SDL_PauseAudioDevice(speakerW, mode != 2);
            break;
        case 3:
            if(v & 1 << 7) {
                SDL_LockAudioDevice(speaker);
                chA.reset();
                chB.reset();
                SDL_UnlockAudioDevice(speaker);
            }
            break;
        case 4:
            chA.fifo_half = v & 1;
            chA.fifo_full = v & 2;
            chB.fifo_half = v & 4;
            chB.fifo_full = v & 8;
            break;
        case 5:
            wav_run = v;
            break;
        case 6:
            volume = v;
            break;
        case 7:
            if(!isEASC) {
                if(freq == v) {
                    return;
                }
                freq = v;
                SDL_CloseAudioDevice(speakerW);
                static const int vv[] = {22257, 0, 22050, 44100};
                wavSpec.freq = vv[v & 3];
                speakerW =
                    SDL_OpenAudioDevice(nullptr, false, &wavSpec, &wavSpec, 0);
            }
            break;
        case 0xA:
            SDL_CloseAudioDevice(mic);
            chA_is_mic = v & 1;
            chA_is_22 = v & 2;
            if(chA_is_mic) {
                micSpec.freq = chA_is_22 ? 22050 : 11025;
                mic = SDL_OpenAudioDevice(nullptr, true, &micSpec, &micSpec, 0);
            }
            break;
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
            phase[0] = SET_Bn(phase[0], v, addr & 3);
            break;
        case 0x14:
        case 0x15:
        case 0x16:
        case 0x17:
            incr[0] = SET_Bn(incr[0], v, addr & 3);
            break;
        case 0x18:
        case 0x19:
        case 0x1A:
        case 0x1B:
            phase[1] = SET_Bn(phase[1], v, addr & 3);
            break;
        case 0x1C:
        case 0x1D:
        case 0x1E:
        case 0x1F:
            incr[1] = SET_Bn(incr[1], v, addr & 3);
            break;
        case 0x20:
        case 0x21:
        case 0x22:
        case 0x23:
            phase[2] = SET_Bn(phase[2], v, addr & 3);
            break;
        case 0x24:
        case 0x25:
        case 0x26:
        case 0x27:
            incr[2] = SET_Bn(incr[2], v, addr & 3);
            break;
        case 0x28:
        case 0x29:
        case 0x2A:
        case 0x2B:
            phase[3] = SET_Bn(phase[3], v, addr & 3);
            break;
        case 0x2C:
        case 0x2D:
        case 0x2E:
        case 0x2F:
            incr[3] = SET_Bn(incr[3], v, addr & 3);
            break;
        }
    }
}

void ASC::wave_play(uint8_t *dst, int len) {
    for(int i = 0; i < len; i++) {
        for(int ch = 0; ch < 4; ch++) {
            if(wav_run & (1 << ch)) {
                phase[ch] = (phase[ch] + incr[ch]) & 0xffffff;
            }
        }
        int16_t mix = chA.FIFO[phase[0] >> 15] ^ 0x80;
        mix += chA.FIFO[phase[1] >> 15 | 0x200] ^ 0x80;
        mix += chB.FIFO[phase[2] >> 15] ^ 0x80;
        mix += chB.FIFO[phase[3] >> 15 | 0x200] ^ 0x80;
        *dst++ = static_cast<int8_t>(mix * volume / 2048);
    }
}
