#include "chip/via.hpp"
#include "SDL.h"
#include "SDL_timer.h"
#include <deque>
#include <vector>
std::shared_ptr<VIA1> via1;
std::shared_ptr<VIA2> via2;
void do_irq(int i);
inline int64_t GetVIACounter() {
    return SDL_GetPerformanceCounter() * 1'276'600 /
           SDL_GetPerformanceFrequency();
}
extern bool adb_irq;
void do_poweroff();
void VIA::recieve_cb1() {
    if(ACR.PB_LATCH) {
        for(int i = 0; i < 8; ++i) {
            if(!dirb[i]) {
                irb[i] = readPB(i);
            }
        }
    }
}
void VIA::recieve_ca1() {
    if(ACR.PA_LATCH) {
        for(int i = 0; i < 8; ++i) {
            ira[i] = readPA(i);
        }
    }
}

void nanosleep(uint32_t nanos) {
    uint64_t ex = SDL_GetPerformanceCounter();
    uint64_t next = ex + nanos * 1000000000LL / SDL_GetPerformanceFrequency();
    while(SDL_GetPerformanceCounter() < next) {
        __builtin_ia32_pause();
    }
}
uint8_t VIA::read(uint32_t addr) {
    int n = addr >> 9 & 0xf;
    nanosleep(500);
    uint8_t v = 0;
    switch(n) {
    case 0: {
        // IRB
        auto val = irb;
        IF[int(VIA_IRQ::CB1)] = false;
        IF[int(VIA_IRQ::CB2)] = false;
        for(int i = 0; i < 8; ++i) {
            if(!dirb[i]) {
                if(!ACR.PB_LATCH) {
                    val[i] = readPB(i);
                }
            } else {
                val[i] = orb[i];
            }
        }
        return val.to_ulong();
    }
    case 15: {
        // IRA with no handshake
        auto val = ira;
        for(int i = 0; i < 8; ++i) {
            if(!dira[i]) {
                val[i] = readPA(i);
            }
        }
        return val.to_ulong();
    }
    case 1:
        // IRA
        IF[int(VIA_IRQ::CA1)] = false;
        IF[int(VIA_IRQ::CA2)] = false;
        if(!ACR.PA_LATCH) {
            std::bitset<8> val;
            for(int i = 0; i < 8; ++i) {
                val[i] = readPA(i);
            }
            return val.to_ulong();
        } else {
            return ira.to_ulong();
        }
    case 2:
        return dirb.to_ulong();
    case 3:
        return dira.to_ulong();
    case 4:
        IF[int(VIA_IRQ::TIMER1)] = false;
        return (GetVIACounter() - timer1_base) & 0xff;
    case 5:
        return (GetVIACounter() - timer1_base) >> 16;
    case 6:
        return timer1_latch;
    case 7:
        return timer1_latch >> 16;
    case 8:
        IF[int(VIA_IRQ::TIMER2)] = false;
        return (GetVIACounter() - timer2_base) & 0xff;
    case 9:
        return (GetVIACounter() - timer2_base) >> 16;
    case 10:
        IF[int(VIA_IRQ::SR)] = false;
        if(ACR.sr_c != SR_C::DISABLED && !(int(ACR.sr_c) & 4)) {
            sr = getSR();
        }
        adb_irq = false;
        return sr;
    case 11:
        return ACR.PB7_ENABLE << 7 | ACR.T1_REP << 6 | ACR.T2_CTL << 5 |
               int(ACR.sr_c) << 2 | ACR.PB_LATCH << 1 | ACR.PA_LATCH;
    case 12:
        return int(PCR.CB2_CTL) << 5 | PCR.CB1_CTL << 4 |
               int(PCR.CA2_CTL) << 1 | PCR.CA1_CTL;
    case 13:
        return (IF.to_ulong() & 0x7f) | (IF.any() << 7);
    case 14:
        return 0x80 | IE.to_ulong();
    }
    return v;
}

void VIA::irq(VIA_IRQ i) {
    IF[int(i)] = true;
    if(IE[int(i)]) {
        do_irq(irqNum());
    }
}
uint32_t via_timer1_callback(uint32_t, void *t) {
    auto v = static_cast<VIA *>(t);
    v->timer1_base = GetVIACounter();
    v->irq(VIA_IRQ::TIMER1);
    if(v->ACR.PB7_ENABLE) {
        v->writePB(7, true);
    }
    return v->ACR.T1_REP ? v->timer1_cnt : 0;
}

uint32_t via_timer2_callback(uint32_t, void *t) {
    auto v = static_cast<VIA *>(t);
    v->timer2_base = GetVIACounter();
    v->irq(VIA_IRQ::TIMER2);
    return 0;
}

void VIA::recieve_sr() { irq(VIA_IRQ::SR); }
void VIA::write(uint32_t addr, uint8_t v) {
    nanosleep(500);
    int n = addr >> 9 & 0xf;
    switch(n) {
    case 0:
        // ORB
        IF[int(VIA_IRQ::CB1)] = false;
        IF[int(VIA_IRQ::CB2)] = false;
        for(int i = 0; i < 8; ++i) {
            bool p = v & 1 << i;
            if(dirb[i]) {
                writePB(i, p);
            } else {
                orb[i] = p;
            }
        }
        break;
    case 1:
        // ORA
        IF[int(VIA_IRQ::CA1)] = false;
        IF[int(VIA_IRQ::CA2)] = false;
        /* fall through*/
    case 15:
        // ORA (no handshake)
        for(int i = 0; i < 8; ++i) {
            bool p = v & 1 << i;
            if(dira[i]) {
                writePA(i, p);
            } else {
                ora[i] = p;
            }
        }
        break;
    case 2:
        for(int i = 0; i < 8; ++i) {
            if((dirb[i] = v & 1 << i)) {
                writePB(i, orb[i]);
            } else {
                irb[i] = readPB(i);
            }
        }
        break;
    case 3:
        for(int i = 0; i < 8; ++i) {
            if((dira[i] = v & 1 << i)) {
                writePA(i, ora[i]);
            } else {
                ira[i] = readPA(i);
            }
        }
        break;
    case 4:
    case 6:
        timer1_latch = (timer1_latch & 0xff00) | v;
        break;
    case 5:
        timer1_cnt = timer1_latch = (timer1_latch & 0xff) | (v << 8);
        IF[int(VIA_IRQ::TIMER1)] = false;
        timer1_base = GetVIACounter();
        timer1 = SDL_AddTimer(timer1_cnt / 1276.6, via_timer1_callback, this);
        break;
    case 8:
        timer2_latch = (timer2_latch & 0xff00) | v;
        break;
    case 9:
        timer2_cnt = timer2_latch = (timer2_latch & 0xff) | (v << 8);
        IF[int(VIA_IRQ::TIMER2)] = false;
        timer2_base = GetVIACounter();
        timer2 = SDL_AddTimer(timer2_cnt / 1276.6, via_timer2_callback, this);
        break;
    case 10:
        IF[int(VIA_IRQ::SR)] = false;
        if((int(ACR.sr_c) & 4)) {
            sr = v;
        }
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
        IF = v;
        break;
    case 14: {
        bool e = v & 0x80;
        for(int i = 0; i < 7; ++i) {
            if(v & 1 << i) {
                IE[i] = e;
            }
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
void VIA1::writePA(int n, bool v) {
    switch(n) {
    case 4:
        //        rom_is_overlay = v;
        break;
    default:
        break;
    }
}
bool rtcEnable = false;

bool VIA1::readPB(int n) {
    switch(n) {
    case 3:
        return adb_irq;
    case 0:
        return rtc_val;
    default:
        return false;
    }
}
void change_adbstate(uint8_t st);
time_t rtc_diff = 0;
std::deque<bool> rtc_lines;
static std::vector<uint8_t> adb_buf;
void adb_run(const std::vector<uint8_t> &b);
void send_rtc(bool v);
bool recv_rtc();
void VIA1::writePB(int n, bool v) {
    switch(n) {
    case 5:
    case 4:
        adb_state[n - 4] = v;
        switch(adb_state[1] << 1 | adb_state[0]) {
        case 0:
            adb_buf.clear();
            adb_buf.push_back(sr);
            break;
        case 1:
        case 2:
            adb_buf.push_back(sr);
            break;
        case 3:
            adb_run(adb_buf);
            adb_buf.clear();
            break;
        }
        break;
    case 2:
        rtcEnable = !v;
        break;
    case 1:
        if(rtcEnable) {
            if(rtc_clock != v && rtc_clock) {
                if(dirb[0]) {
                    send_rtc(rtc_val);
                } else {
                    rtc_val = recv_rtc();
                }
            }
            rtc_clock = v;
        }
        break;
    case 0:
        if(rtcEnable) {
            rtc_val = v;
        }
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
uint8_t NuBusInput() { return 0; }
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
    case 7:
        // VBL IRQ
        via1->recieve_ca1();
        break;
    case 2:
        // Power OFF
        if(!v) {
            do_poweroff();
        }
        break;
    default:
        break;
    }
}

uint8_t adb_in();

uint8_t VIA1::getSR() { return adb_in(); }
