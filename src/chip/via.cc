#include "via.hpp"
#include "SDL.h"
#include "SDL_timer.h"
#include <deque>
void do_irq(int i);
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
uint32_t via_timer1_callback(uint32_t, void *t) {
    auto v = static_cast<VIA *>(t);
    v->IF.TIMER1 = true;
    v->timer1_base = GetVIACounter();
    if(v->IE.TIMER1) {
        do_irq(t == &via1 ? 1 : 2);
    }
    return v->ACR.T1_REP ? v->timer1_cnt : 0;
}

uint32_t via_timer2_callback(uint32_t, void *t) {
    auto v = static_cast<VIA *>(t);
    v->IF.TIMER2 = true;
    v->timer2_base = GetVIACounter();
    if(v->IE.TIMER2) {
        do_irq(t == &via1 ? 1 : 2);
    }
    return 0;
}

void VIA::recieve_sr() {
    if(IE.SR) {
        do_irq(this == &via1 ? 1 : 2);
    }
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
    case 5:
        timer1_lh[1] = v;
        timer1_cnt = timer1_lh[0] | v << 8;
        IF.TIMER1 = false;
        timer1_base = GetVIACounter();
        timer1 = SDL_AddTimer(timer1_cnt / 1276.6, via_timer1_callback, this);
        break;
    case 8:
        timer2_lh[0] = v;
        break;
    case 9:
        timer2_lh[1] = v;
        timer2_cnt = timer2_lh[0] | v << 8;
        IF.TIMER2 = false;
        timer2_base = GetVIACounter();
        timer2 = SDL_AddTimer(timer2_cnt / 1276.6, via_timer2_callback, this);
        break;
    case 10:
        setSR(v);
        break;
    case 11:
        ACR.PB7_ENABLE = v & 1 << 7;
        ACR.T1_REP = v & 1 << 6;
        ACR.T2_CTL = v & 1 << 5;
        ACR.sr_c = SR_C(v >> 2 & 7);
        ACR.PB_LATCH = v & 1 << 1;
        ACR.PA_LATCH = v & 1;
        break;
    case 12:
        PCR.CB2_CTL = C2_CTL(v >> 5 & 7);
        PCR.CB1_CTL = v & 1 << 4;
        PCR.CA2_CTL = C2_CTL(v >> 1 & 7);
        PCR.CA1_CTL = v & 1;
        break;
    case 13:
        IF.IRQ = v & 1 << 7;
        IF.TIMER1 = v & 1 << 6;
        IF.TIMER2 = v & 1 << 5;
        IF.CB1 = v & 1 << 4;
        IF.CB2 = v & 1 << 3;
        IF.SR = v & 1 << 2;
        IF.CA1 = v & 1 << 1;
        IF.CA2 = v & 1;
        break;
    case 14: {
        bool e = v & 0x80;
        if(v & 1 << 6) {
            IE.TIMER1 = e;
        }
        if(v & 1 << 5) {
            IE.TIMER2 = e;
        }
        if(v & 1 << 4) {
            IE.CB1 = e;
        }
        if(v & 1 << 3) {
            IE.CB2 = e;
        }
        if(v & 1 << 2) {
            IE.SR = e;
        }
        if(v & 1 << 1) {
            IE.CA1 = e;
        }
        if(v & 1) {
            IE.CA2 = e;
        }
    }
    }
}
bool MACHINE_CODE[3] = {
    false,
    false,
};
bool MACHINE_CODE2[4] = {false, false, false, true};
// TODO
extern bool rom_is_overlay;

bool scc_wait_req() { return false; }
void VIA1::writePA(int n, bool v) {
    switch(n) {
    case 4:
        rom_is_overlay = v;
        break;
    default:
        break;
    }
}
bool rtcEnable = false;

bool VIA1::readPA(int n) {
    switch(n) {
    case 1:
        return MACHINE_CODE2[0];
    case 2:
        return MACHINE_CODE2[1];
    case 4:
        return MACHINE_CODE2[2];
    case 6:
        return MACHINE_CODE2[3];
    case 7:
        return scc_wait_req();
    default:
        return false;
    }
}
extern bool adb_irq;
bool VIA1::readPB(int n) {
    switch(n) {
    case 3:
        return adb_irq;
    default:
        return false;
    }
}

time_t rtc_diff = 0;
std::deque<bool> rtc_lines;

void VIA1::writePB(int n, bool v) {
    switch(n) {
    case 2:
        rtcEnable = !v;
        break;
    default:
        break;
    }
}
bool VIA2::readPA(int n) {
    switch(n) {
    default:
        return false;
    }
}
// TODO
uint8_t NuBusInput() { return 0;}
bool VIA2::readPB(int n) {
    switch(n) {
    case 4:
        return NuBusInput() & 2;
    case 5:
        return NuBusInput() & 1;
    case 6:
        return false;
    default:
        return false;
    }
}

void VIA2::writePA(int n, bool) {
    switch(n) {
    default:
        break;
    }
}
void VIA2::writePB(int n, bool v) {
    switch(n) {
   
    case 2:
        // Power OFF
        if(!v) {
            exit(0);
        }
        break;
    default:
        break;
    }
}

uint8_t adb_in();
void adb_out(uint8_t v);

uint8_t VIA1::getSR() { return adb_in(); }
void VIA1::setSR(uint8_t v) { adb_out(v); }
// unused
uint8_t VIA2::getSR() { return 0; }
void VIA2::setSR(uint8_t) {}
