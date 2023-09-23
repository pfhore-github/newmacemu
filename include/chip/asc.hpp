#ifndef ASC_HPP_
#define ASC_HPP_

#include "SDL_audio.h"
#include "SDL_timer.h"
#include "io.hpp"
#include <atomic>
#include <mutex>
#include <stdint.h>
struct ASC_common;
struct EASC_CH {
    uint8_t FIFO[0x400];
    uint16_t FIFO_READP;
    uint16_t FIFO_WRITEP;
    uint16_t SAMPLE_RATE;
    float FIFO_PLAYP;
    uint16_t FIFO_SIZE;
    bool fifo_cdxa = false;
    uint8_t CD_XA_const[8];
    SDL_TimerID half_timer, empty_timer;
    std::atomic<bool> fifo_half, fifo_empty;
    bool IRQ_CTL;
    uint8_t left_vol, right_vol;
    EASC_CH() { reset(); }
    void reset();
    void fifo_write(uint8_t v);
    int8_t fifo_read();

    uint8_t read_reg(int t);
    void write_reg(int t, uint8_t v);
};
struct ASC_common {
    SDL_AudioDeviceID speaker = 0;
    SDL_AudioSpec speakerSpec;
    EASC_CH chA, chB;
    int8_t volume = 0;
    void fifo_play(int16_t *stream, int len);
    ASC_common();
    virtual ~ASC_common() {}
};
struct ASC : public CHIP_B, public ASC_common {
    ASC();
    void reopen();
    SDL_AudioSpec wavSpeakerSpec;
    uint32_t phase[4];
    uint32_t incr[4];
    std::atomic<uint8_t> wav_run{0xff};
    uint8_t mode = 0;
    uint8_t ctl = 0;
    uint8_t freq = 0;
    void wave_play(int16_t *stream, int len);
    void reset();
    uint8_t read(uint32_t addr);
    void write(uint32_t addr, uint8_t v);
    void disconnect();
};

struct EASC : public CHIP_B, public ASC_common {
    SDL_AudioDeviceID mic = 0;
    SDL_AudioSpec micSpec;
    bool overflow;
    bool enabled;
    bool chA_is_mic;
    bool chA_is_22;
    void disconnect();

    EASC();
    void reset();
    uint8_t read(uint32_t addr);
    void write(uint32_t addr, uint8_t v);
    ~EASC();
};
extern std::shared_ptr<EASC> easc;
#endif