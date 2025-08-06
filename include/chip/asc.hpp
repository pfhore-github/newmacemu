#ifndef ASC_HPP_
#define ASC_HPP_

#include "SDL3/SDL_audio.h"
#include "SDL3/SDL_thread.h"
#include "SDL3/SDL_timer.h"
#include "io.hpp"
#include <atomic>
#include <stdint.h>
struct ASC_CH {
    int8_t FIFO[0x400];
    SDL_AudioStream *chStream;
    uint16_t FIFO_READP;
    uint16_t FIFO_WRITEP;
    uint16_t FIFO_PLAYP;
    uint16_t SAMPLE_RATE;
    uint16_t FIFO_SIZE;
    std::atomic<bool> fifo_full{false};
    std::atomic<bool> fifo_half{false};
    SDL_TimerID timerH, timerF;
    bool fifo_cdxa = false;
    uint8_t CD_XA_const[8];
    bool IRQ_CTL;
    uint8_t left_vol, right_vol;
    int8_t xa_cnt = -1;
    uint8_t xa_flags = 0;
    uint8_t xa_val = 0;
    int16_t xa_last[2];
    ASC_CH();
    ~ASC_CH() { SDL_DestroyAudioStream(chStream); }
    void reset();
    void fifo_write(uint8_t v);
    int8_t fifo_read();
    int8_t cdxa_read();

    uint8_t read_reg(int t);
    void write_reg(int t, uint8_t v);
    void setSampleRate(uint16_t rate);
    void set_fifotimer();
};
struct ASC {
    ASC_CH chA, chB;
    uint32_t phase[4];
    uint32_t incr[4];
    SDL_AudioDeviceID speaker = 0;
    SDL_AudioDeviceID mic = 0;
    SDL_AudioSpec speakerSpec;
    SDL_AudioSpec micSpec;
    SDL_AudioStream *micStream = nullptr;
    uint8_t wav_run = 0xff;
    uint8_t mode = 0;
    uint8_t freq = 3;
    std::atomic<bool> overflow{false};
    bool chA_is_mic;
    bool chA_is_22;
    bool isEASC;
    uint8_t volume = 0;
    void reset();
    uint8_t read(uint32_t addr);
    void write(uint32_t addr, uint8_t v);
    SDL_AudioStream *wavStream = nullptr;
    void do_wave(int additional_amount);
    ASC(bool isEASC_ = true);
    ~ASC();
};
extern std::shared_ptr<ASC> asc;
#endif