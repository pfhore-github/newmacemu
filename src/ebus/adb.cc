#include "ebus/adb.hpp"
#include "SDL3/SDL.h"
#include "SDL3/SDL_scancode.h"
#include "chip/via.hpp"
#include <algorithm>
#include <deque>
#include <list>
#include <memory>
#include <stdint.h>
#include <unordered_map>
#include <vector>
std::deque<uint8_t> buf;
SDL_TimerID adbTimer;
void do_adb_irq() { via1->recieve_sr(); }
uint8_t adb_in() {
    static uint8_t v = 0;
    if(buf.empty()) {
        // empty
        do_adb_irq();
        return v;
    } else {
        v = buf[0];
        buf.pop_front();
        do_adb_irq();
        return v;
    }
}

std::list<uint8_t> keys;
std::unordered_map<SDL_Scancode, uint8_t> mac_keys = {
    {SDL_SCANCODE_ESCAPE, 0x35},
    {SDL_SCANCODE_F1, 0x7A},
    {SDL_SCANCODE_F2, 0x78},
    {SDL_SCANCODE_F3, 0x63},
    {SDL_SCANCODE_F4, 0x76},
    {SDL_SCANCODE_F5, 0x60},
    {SDL_SCANCODE_F6, 0x61},
    {SDL_SCANCODE_F7, 0x62},
    {SDL_SCANCODE_F8, 0x64},
    {SDL_SCANCODE_F9, 0x65},
    {SDL_SCANCODE_F10, 0x6D},
    {SDL_SCANCODE_F11, 0x67},
    {SDL_SCANCODE_F12, 0x6F},
    {SDL_SCANCODE_PRINTSCREEN, 0x69},
    {SDL_SCANCODE_SCROLLLOCK, 0x6B},
    {SDL_SCANCODE_PAUSE, 0x71},

    {SDL_SCANCODE_GRAVE, 0x32},
    {SDL_SCANCODE_1, 0x12},
    {SDL_SCANCODE_2, 0x13},
    {SDL_SCANCODE_3, 0x14},
    {SDL_SCANCODE_4, 0x15},
    {SDL_SCANCODE_5, 0x16},
    {SDL_SCANCODE_6, 0x17},
    {SDL_SCANCODE_7, 0x1A},
    {SDL_SCANCODE_8, 0x1C},
    {SDL_SCANCODE_9, 0x19},
    {SDL_SCANCODE_0, 0x1D},
    {SDL_SCANCODE_MINUS, 0x1B},
    {SDL_SCANCODE_EQUALS, 0x1E},
    {SDL_SCANCODE_BACKSPACE, 0x33},
    {SDL_SCANCODE_INSERT, 0x72},
    {SDL_SCANCODE_HOME, 0x73},
    {SDL_SCANCODE_PAGEUP, 0x74},
    {SDL_SCANCODE_NUMLOCKCLEAR, 0x47},
    // Mac keyboard has ten key "="
    {SDL_SCANCODE_KP_EQUALS, 0x51},
    {SDL_SCANCODE_KP_DIVIDE, 0x4B},
    {SDL_SCANCODE_KP_MULTIPLY, 0x43},
    {SDL_SCANCODE_TAB, 0x30},
    {SDL_SCANCODE_Q, 0x0c},
    {SDL_SCANCODE_W, 0x0d},
    {SDL_SCANCODE_E, 0x0e},
    {SDL_SCANCODE_R, 0x0F},
    {SDL_SCANCODE_T, 0x11},
    {SDL_SCANCODE_Y, 0x10},
    {SDL_SCANCODE_U, 0x20},
    {SDL_SCANCODE_I, 0x22},
    {SDL_SCANCODE_O, 0x1F},
    {SDL_SCANCODE_P, 0x23},
    {SDL_SCANCODE_LEFTBRACKET, 0x21},
    {SDL_SCANCODE_RIGHTBRACKET, 0x1E},
    {SDL_SCANCODE_BACKSLASH, 0x2A},
    {SDL_SCANCODE_DELETE, 0x75},
    {SDL_SCANCODE_END, 0x77},
    {SDL_SCANCODE_PAGEDOWN, 0x79},
    {SDL_SCANCODE_KP_7, 0x59},
    {SDL_SCANCODE_KP_8, 0x5b},
    {SDL_SCANCODE_KP_9, 0x5c},
    {SDL_SCANCODE_KP_MINUS, 0x4E},

    {SDL_SCANCODE_CAPSLOCK, 0x39},
    {SDL_SCANCODE_A, 0x00},
    {SDL_SCANCODE_S, 0x01},
    {SDL_SCANCODE_D, 0x02},
    {SDL_SCANCODE_F, 0x03},
    {SDL_SCANCODE_G, 0x05},
    {SDL_SCANCODE_H, 0x04},
    {SDL_SCANCODE_J, 0x26},
    {SDL_SCANCODE_K, 0x28},
    {SDL_SCANCODE_L, 0x25},
    {SDL_SCANCODE_SEMICOLON, 0x29},
    {SDL_SCANCODE_APOSTROPHE, 0x27},
    {SDL_SCANCODE_RETURN, 0x24},
    {SDL_SCANCODE_KP_4, 0x56},
    {SDL_SCANCODE_KP_5, 0x57},
    {SDL_SCANCODE_KP_6, 0x58},
    {SDL_SCANCODE_KP_PLUS, 0x45},

    {SDL_SCANCODE_LSHIFT, 0x38},
    {SDL_SCANCODE_Z, 0x06},
    {SDL_SCANCODE_X, 0x07},
    {SDL_SCANCODE_C, 0x08},
    {SDL_SCANCODE_V, 0x09},
    {SDL_SCANCODE_B, 0x0B},
    {SDL_SCANCODE_N, 0x2D},
    {SDL_SCANCODE_M, 0x2E},
    {SDL_SCANCODE_COMMA, 0x2B},
    {SDL_SCANCODE_PERIOD, 0x2C},
    {SDL_SCANCODE_RSHIFT, 0x7B},
    {SDL_SCANCODE_UP, 0x3E},

    {SDL_SCANCODE_KP_1, 0x53},
    {SDL_SCANCODE_KP_2, 0x54},
    {SDL_SCANCODE_KP_3, 0x55},

    {SDL_SCANCODE_LCTRL, 0x36},
    {SDL_SCANCODE_LGUI, 0x3A},
    {SDL_SCANCODE_LALT, 0x37},
    {SDL_SCANCODE_SPACE, 0x31},
    {SDL_SCANCODE_RALT, 0x7C},
    {SDL_SCANCODE_RCTRL, 0x7D},
    {SDL_SCANCODE_LEFT, 0x3B},
    {SDL_SCANCODE_DOWN, 0x3D},
    {SDL_SCANCODE_RIGHT, 0x3C},

    {SDL_SCANCODE_KP_0, 0x52},
    {SDL_SCANCODE_KP_PERIOD, 0x41},
    {SDL_SCANCODE_KP_ENTER, 0x4C},

};

class ADBMouse : public ADBDev {

    int old_mx;
    int old_my;

  public:
    ADBMouse() {
        reg3[0] = 0x63;
        reg3[1] = 0x01;
        old_mx = 0;
        old_my = 0;
    }
    void flush() override {
        // do nothing about mouse
    }
    void listen(int reg, const uint8_t *v) override {
        if(reg == 3) {
            if(v[1] == 0xfe) // Change address
                reg3[0] = (reg3[0] & 0xf0) | (v[0] & 0x0f);
            else if(v[1] == 1 || v[1] == 2 ||
                    v[1] == 4) // Change device handler ID
                reg3[1] = v[2];
            else if(v[1] == 0x00) // Change address and enable bit
                reg3[0] = (reg3[0] & 0xd0) | (v[0] & 0x2f);
        }
    }
    std::deque<uint8_t> talk(int reg) override {
        std::deque<uint8_t> ret;
        switch(reg) {
        case 0: {
            float x, y;
            auto btn = SDL_GetMouseState(&x, &y);
            int dx = x - old_mx;
            int dy = y - old_my;

            ret.push_back((dx & 0x7f) | (SDL_BUTTON_LMASK & btn ? 0 : 0x80));
            ret.push_back((dy & 0x7f) | (SDL_BUTTON_RMASK & btn ? 0 : 0x80));
            ret.push_back(((dy >> 3) & 0x70) | ((dx >> 7) & 0x07) |
                          (SDL_BUTTON_MMASK & btn ? 0x08 : 0x88));
            old_mx = x;
            old_my = y;
            return ret;
        }

        case 1: // Extended mouse protocol
            return {'a', 'p', 'p', 'l', 300 >> 8, 300 & 0xff, 1, 3};
        case 3: // Address/HandlerID
            ret.push_back((reg3[0] & 0xf0) | (rand() & 0x0f));
            ret.push_back(reg3[1]);
            return ret;
        default:
            return {};
        }
    }
};

class ADBKeyboard : public ADBDev {
    uint8_t reg2[2]; // Keyboard ADB register 2
  public:
    ADBKeyboard() {
        reg2[0] = 0xff;
        reg2[1] = 0xff;
        reg3[0] = 0x62;
        reg3[1] = 0x05;
    }
    void flush() override {
        // key  queue clear
        SDL_ResetKeyboard();
        keys.clear();
    }
    void listen(int reg, const uint8_t *v) override {
        switch(reg) {
        case 2:
            // LEDs/Modifiers
            // sorry, SDL doesn't support change Keyboard LED!
            reg2[0] = v[0];
            reg2[1] = v[1];
            break;
        case 3:
            if(v[1] == 0xfe) // Change address
                reg3[0] = (reg3[0] & 0xf0) | (v[0] & 0x0f);
            else if(v[1] == 0x00) // Change address and enable bit
                reg3[0] = (reg3[0] & 0xd0) | (v[0] & 0x2f);
            break;
        }
    }
    std::deque<uint8_t> talk(int reg) override {
        std::deque<uint8_t> ret = {1};
        switch(reg) {
        case 0:
            if(keys.empty()) {
                ret.push_back(0xff);
                ret.push_back(0xff);
            } else if(keys.size() == 1) {
                uint8_t v = *keys.begin();
                keys.clear();
                ret.push_back(v);
                ret.push_back(0xff);
            } else {
                auto ky = keys.begin();
                ret.push_back(*ky);
                ky = keys.erase(ky);
                ret.push_back(*ky);
                keys.erase(ky);
            }
            break;
        case 2: { // LEDs/Modifiers
            auto mod = SDL_GetModState();
            auto kbd = SDL_GetKeyboardState(nullptr);
            uint8_t reg2hi = 0xff;
            uint8_t reg2lo = reg2[1] | 0xf8;
            if(mod & SDL_KMOD_SCROLL) // Scroll Lock
                reg2lo &= ~0x40;
            if(mod & SDL_KMOD_NUM) // Num Lock
                reg2lo &= ~0x80;
            if(mod & SDL_KMOD_LGUI) // Command
                reg2hi &= ~0x01;
            if(mod & SDL_KMOD_LALT) // Option
                reg2hi &= ~0x02;
            if(mod & SDL_KMOD_LSHIFT) // Shift
                reg2hi &= ~0x04;
            if(mod & SDL_KMOD_LCTRL) // Control
                reg2hi &= ~0x08;
            if(mod & SDL_KMOD_CAPS) // Caps Lock
                reg2hi &= ~0x20;
            if(kbd[SDL_SCANCODE_DELETE]) // Delete
                reg2hi &= ~0x40;
            ret.push_back(reg2hi);
            ret.push_back(reg2lo);
            break;
        }
        case 3: // Address/HandlerID
            ret.push_back(1);
            ret.push_back((reg3[0] & 0xf0) | (rand() & 0x0f));
            ret.push_back(reg3[1]);
            break;
        }
        return ret;
    }
};

uint8_t xpram_read(uint8_t reg);
void xpram_write(uint8_t reg, uint8_t v);
bool adb_xprammode = false;
std::shared_ptr<ADBDev> adbDevs[4];
void adb_reset() {
    adbDevs[0] = std::make_shared<ADBMouse>();
    adbDevs[1] = std::make_shared<ADBKeyboard>();
    SDL_ResetKeyboard();
}
void adb_attention() {
    buf = {1};
    do_adb_irq();
    return;
}
void adb_run(const std::vector<uint8_t>& b) {
    if(b.empty()) {
        // do nothing
        return;
    }
    uint8_t v = b[0];
    int adr = v >> 4;
    int cmd = v >> 2 & 3;
    int reg = v & 3;
    if(cmd == 0 && reg == 0) {
        // SendReset
        adb_reset();
        return;
    }
    // Check which device was addressed and act accordingly
    if(v == 1) {
        // RTC flash
        adb_xprammode = true;
        buf = {1, 0};
        do_adb_irq();
        return;
    }
    if(v == 7) {
        // RTC write
        xpram_write(b[1] << 8 | b[2], b[3]);
        return;
    } else if(v == 12) {
        // RTC read
        buf = {1, 0, 0, 7, xpram_read(b[1] << 8 | b[2]), 0};
        do_adb_irq();
        return;
    }
    for(auto &d : adbDevs) {
        if(d) {
            if(adr == (d->reg3[0] & 0x0f)) {
                if(cmd == 0 && reg == 1) {
                    // flush
                    d->flush();
                } else if(cmd == 2) {
                    // Listen
                    d->listen(reg, &b[1]);
                    return;
                } else if(cmd == 3) {
                    // Talk
                    buf = d->talk(reg);
                    // terminate byte
                    buf.push_back(0);
                }
                // stop bit
                buf.push_back(1);
                do_adb_irq();
            }
        }
    }
}
std::vector<uint8_t> adb_buf;
uint8_t recieveAdb(uint8_t adbState) {
    switch(adbState) {
    case 0:
        buf.clear();
        return 0;
    case 1:
    case 2:
        return adb_in();
    case 3:
        return 0;
    default:
        __builtin_unreachable();
    }
}

void transmitAdb(uint8_t adbState, uint8_t v) {
    do_adb_irq();
    if(adbState == 0) {
         if(!adb_buf.empty()) {
            adb_run(adb_buf);
            adb_buf.clear();
        } 
        adb_buf.clear();
    }
    if(v == 1) {
        if(!adb_buf.empty()) {
            adb_run(adb_buf);
            adb_buf.clear();
        } else {
            adb_attention();
        }
        return;
    }
    if(v) {
        adb_buf.push_back(v);
    }
}
void keydown(const SDL_KeyboardEvent *e) {
    if(mac_keys.contains(e->scancode)) {
        keys.push_back(mac_keys[e->scancode]);
    }

    do_adb_irq();
}

void keyup(const SDL_KeyboardEvent *e) {
    if(mac_keys.contains(e->scancode)) {
        auto ret =
            std::find(keys.begin(), keys.end(), mac_keys[e->scancode]);
        if(ret != keys.end()) {
            keys.erase(ret);
        }
    }
    do_adb_irq();
}
