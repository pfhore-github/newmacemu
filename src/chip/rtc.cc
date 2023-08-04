#include <deque>
#include "via.hpp"
#include "SDL.h"
#include "SDL_timer.h"
void do_irq(int i);
uint32_t rtc_callback(uint32_t, void *) {
    via1.IF.CA2 = true;
    if(via1.IE.CA2) {
        do_irq(1);
    }
    return 1000;
}

void rtc_reset() {
    static SDL_TimerID rtc_timer = 0;
    if(rtc_timer) {
        SDL_RemoveTimer(rtc_timer);
    }
    rtc_timer = SDL_AddTimer(1000, rtc_callback, nullptr);
}

constexpr int XPRAM_SIZE = 256;
uint8_t XPRAM[XPRAM_SIZE];

