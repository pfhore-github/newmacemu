#include "SDL3/SDL.h"
#include "SDL3/SDL_timer.h"
#include "chip/via.hpp"
#include <deque>
#include <time.h>
#include <vector>
void do_irq(int i);
static uint8_t rtc_cmd = 0;
static int rtc_cnt_w, rtc_cnt_r = 0;
static std::vector<uint8_t> rtc_lists;
static uint8_t rtc_val = 0;
constexpr int XPRAM_SIZE = 256;
uint8_t XPRAM[XPRAM_SIZE];

constexpr uint64_t MAC_TIME_DIFF = 2082844800;
uint8_t xpram_read(uint8_t reg) {
    if(reg < 8) {
        uint32_t t = time(nullptr) + MAC_TIME_DIFF;
        switch(reg & 3) {
        case 0:
            return t;
        case 1:
            return t >> 8;
        case 2:
            return t >> 16;
        case 3:
            return t >> 24;
        }
    }
    return XPRAM[reg];
}
void xpram_write(uint8_t reg, uint8_t v) { XPRAM[reg] = v; }

uint8_t do_rtc(uint8_t cmd, uint8_t v0, uint8_t v1) {
    if((cmd & 0xB8) == 0xB8) {
        // XPRAM READ
        uint8_t param = v0;
        uint8_t reg = ((cmd << 5) & 0xe0) | ((param >> 2) & 0x1f);
        return xpram_read(reg);
    } else if((cmd & 0xB8) == 0x38) {
        // XPRAM WRITE
        uint8_t param = v0;
        uint8_t value = v1;
        uint8_t reg = ((cmd << 5) & 0xe0) | ((param >> 2) & 0x1f);
        xpram_write(reg, value);
        return 0;
    } else {
        // PRAM, RTC and other clock registers
        uint8_t reg = (cmd >> 2) & 0x1f;
        bool is_read = cmd & 0x80;
        if(is_read) {
            return xpram_read(reg);
        } else {
            xpram_write(reg, v0);
            return 0;
        }
    }
}
void send_rtc(bool v) {
    rtc_cmd = rtc_cmd | v << (rtc_cnt_w++);
    rtc_cnt_w &= 7;
    if(rtc_cnt_w == 0) {
        rtc_lists.push_back(rtc_cmd);
        rtc_cmd = 0;
        if((rtc_lists[0] & 0xB8) == 0xB8 && rtc_lists.size() == 2) {
            // XPRAM READ
            rtc_val = do_rtc(rtc_lists[0], rtc_lists[1], 0);
            rtc_cnt_r = 0;
            rtc_lists.clear();
        } else if((rtc_lists[0] & 0xB8) == 0x38 && rtc_lists.size() == 3) {
            // XPRAM WRITE
            do_rtc(rtc_lists[0], rtc_lists[1], rtc_lists[2]);
            rtc_lists.clear();
        } else {
            // PRAM, RTC and other clock registers
            bool is_read = rtc_lists[0] & 0x80;
            if(is_read && rtc_lists.size() == 1) {
                rtc_val = do_rtc(rtc_lists[0], 0, 0);
                rtc_cnt_r = 0;
                rtc_lists.clear();
            } else if(rtc_lists.size() == 2) {
                do_rtc(rtc_lists[0], rtc_lists[1], 0);
                rtc_lists.clear();
            }
        }
    }
}
bool recv_rtc() {
    rtc_cnt_r &= 7;
    return rtc_val >> (rtc_cnt_r++) & 1;
}
uint32_t rtc_callback(void*, SDL_TimerID, uint32_t) {
    via1->irq(VIA_IRQ::CA2);
    return 1000;
}

void rtc_reset() {
    static SDL_TimerID rtc_timer = 0;
    if(rtc_timer) {
        SDL_RemoveTimer(rtc_timer);
    }
    rtc_timer = SDL_AddTimer(1000, rtc_callback, nullptr);
}
