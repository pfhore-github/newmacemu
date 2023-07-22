#include "via.hpp"
#include "SDL.h"
#include "SDL_timer.h"
VIA1 via1;
VIA2 via2;
inline int64_t GetVIACounter() {
    return SDL_GetPerformanceCounter() * 1'276'600 /
           SDL_GetPerformanceFrequency();
}
uint8_t VIA::read(int n) {
    uint8_t v = 0;
    switch(n) {
    case 0:
        // ORB
        for(int i = 0; i < 8; ++i) {
            if(dirb & 1 << i) {
                v |= db & 1 << i;
            } else {
                v |= readPB(i) << i;
            }
        }
        break;
    case 1:
    case 15:
        // ORA
        for(int i = 0; i < 8; ++i) {
            if(dira & 1 << i) {
                v |= da & 1 << i;
            } else {
                v |= readPA(i) << i;
            }
        }
        break;
    case 2:
        return dirb;
    case 3:
        return dira;
    case 4:
        IF.TIMER1 = false;
        return (GetVIACounter() - timer1_base) & 0xff;
    case 5:
        return (GetVIACounter() - timer1_base) >> 16;
    case 6:
        return timer1_lh[0];
    case 7:
        return timer1_lh[1];
    case 8:
        IF.TIMER2 = false;
        return (GetVIACounter() - timer2_base) & 0xff;
    case 9:
        return (GetVIACounter() - timer2_base) >> 16;
    case 10:
        return getSR();
    case 11:
        return ACR.PB7_ENABLE << 7 | ACR.T1_REP << 6 | ACR.T2_CTL << 5 |
               int(ACR.sr_c) << 2 | ACR.PB_LATCH << 1 | ACR.PA_LATCH;
    case 12:
        return int(PCR.CB2_CTL) >> 5 | PCR.CB1_CTL << 4 |
               int(PCR.CA2_CTL) >> 1 | PCR.CA1_CTL;
    case 13:
        return IF.IRQ << 7 | IF.TIMER1 << 6 | IF.TIMER2 << 5 | IF.CB1 << 4 |
               IF.CB2 << 3 | IF.SR << 2 | IF.CA1 << 1 | IF.CA2;
    case 14:
        return 0x80 | IE.TIMER1 << 6 | IE.TIMER2 << 5 | IE.CB1 << 4 |
               IE.CB2 << 3 | IE.SR << 2 | IE.CA1 << 1 | IF.CA2;
    }
    return v;
}
void IRQ(int n);
uint32_t via_timer1_callback(uint32_t, void* t) {
    auto v = static_cast<VIA*>(t);
    v->IF.TIMER1 = true;
    v->timer1_base = GetVIACounter();
    if( v->IE.TIMER1) {
        IRQ(t == &via1 ? 1 : 2);
    }
    return v->ACR.T1_REP ? v->timer1_cnt : 0;
}

uint32_t via_timer2_callback(uint32_t, void* t) {
    auto v = static_cast<VIA*>(t);
    v->IF.TIMER2 = true;
    v->timer2_base = GetVIACounter();
    if( v->IE.TIMER2) {
        IRQ(t == &via1 ? 1 : 2);
    }
    return 0;
}
void VIA::write(int n, uint8_t v) {
    switch(n) {
    case 0:
        // ORB
        for(int i = 0; i < 8; ++i) {
            if(dirb & 1 << i) {
                writePB(i, v >> i & 1);
            } else {
                db = (db & ~(1 << i)) | (v & 1 << i);
            }
        }
        break;
    case 1:
    case 15:
        // ORA
        for(int i = 0; i < 8; ++i) {
            if(dira & 1 << i) {
                writePA(i, v >> i & 1);
            } else {
                da = (da & ~(1 << i)) | (v & 1 << i);
            }
        }
        break;
    case 2:
        for(int i = 0; i < 8; ++i) {
            if(v & 1 << i) {
                writePB(i, db >> i & 1);
            } else {
                db = (db & ~(1 << i)) | (readPB(i) << i);
            }
        }
        dirb = v;
        break;
    case 3:
        for(int i = 0; i < 8; ++i) {
            if(v & 1 << i) {
                writePA(i, da >> i & 1);
            } else {
                da = (da & ~(1 << i)) | (readPA(i) << i);
            }
        }
        dira = v;
        break;
    case 4:
    case 6:
        timer1_lh[0] = v;
        break;
    case 5: {
        timer1_lh[1] = v;
        timer1_cnt = timer1_lh[0] | v << 8;
        IF.TIMER1 = false;
        timer1_base = GetVIACounter();
        timer1 = SDL_AddTimer(timer1_cnt / 1276.6, via_timer1_callback, this); 
        break;
    case 8:
        timer1_lh[1] = v;
        break;
    }
    }
}
bool MACHINE_CODE[4] = {false, false, false, true};
// TODO
bool scc_wait_req() { return false; }
void VIA1::writePA(int n, bool ) {
    switch(n) {
        default:
        break;
    }
}
void VIA1::writePB(int n, bool ) {
    switch(n) {
        default:
        break;
    }
}
bool VIA1::readPA(int n) {
    switch(n) {
    case 1:
        return MACHINE_CODE[0];
    case 2:
        return MACHINE_CODE[1];
    case 4:
        return MACHINE_CODE[2];
    case 5:
        return MACHINE_CODE[3];
    case 7:
        return scc_wait_req();
    default:
        return false;
    }
}
bool adb_irq() { return false; }
bool VIA1::readPB(int n) {
    switch(n) {
    case 3:
        return adb_irq();
    case 0:
        return MACHINE_CODE[1];
    case 4:
        return MACHINE_CODE[2];
    case 5:
        return MACHINE_CODE[3];
    case 7:
        return scc_wait_req();
    default:
        return false;
    }
}


bool VIA2::readPA(int n) {
    switch(n) {
    default:
        return false;
    }
}
bool VIA2::readPB(int n) {
    switch(n) {
    default:
        return false;
    }
}

void VIA2::writePA(int n, bool ) {
    switch(n) {
        default:
        break;
    }
}
void VIA2::writePB(int n, bool ) {
    switch(n) {
        default:
        break;
    }
}

// TODO
uint8_t adb_in() { return 0;}
void adb_out(uint8_t) {}

uint8_t VIA1::getSR() { return adb_in(); }
void VIA1::setSR(uint8_t v) { adb_out(v); }
// unused
uint8_t VIA2::getSR() { return 0;}
void VIA2::setSR(uint8_t) {}

uint32_t rtc_callback(uint32_t, void* ) {
    via1.IF.CA2 = true;
    if( via1.IE.CA2) {
        IRQ(1);
    }
    return 1000;
}
void rtc_reset() {
    static SDL_TimerID rtc_timer = 0;
    if( rtc_timer) {
        SDL_RemoveTimer(rtc_timer);
    }
    rtc_timer = SDL_AddTimer(1000, rtc_callback, nullptr); 
}