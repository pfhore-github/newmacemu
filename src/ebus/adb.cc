#include "SDL.h"
#include "SDL_scancode.h"
#include "via.hpp"
#include <algorithm>
#include <deque>
#include <list>
#include <memory>
#include <stdint.h>
#include <unordered_map>
#include <vector>
std::deque<uint8_t> buf;
static uint8_t mouse_reg_3[2] = {0x63, 0x01}; // Mouse ADB register 3

static uint8_t key_reg_2[2] = {0xff, 0xff}; // Keyboard ADB register 2
static uint8_t key_reg_3[2] = {0x62, 0x05}; // Keyboard ADB register 3

uint8_t adb_in() {
    if(buf.empty()) {
        // empty
        return 0;
    } else {
        auto v = buf[0];
        buf.pop_front();
        return v;
    }
}
std::list<uint8_t> keys;
bool adb_irq = false;
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

void keydown(const SDL_KeyboardEvent *e) {
    adb_irq = true;
    if(mac_keys.contains(e->keysym.scancode)) {
        keys.push_back(mac_keys[e->keysym.scancode]);
    }

    via1.recieve_sr();
}

void keyup(const SDL_KeyboardEvent *e) {
    adb_irq = true;
    if(mac_keys.contains(e->keysym.scancode)) {
        auto ret =
            std::find(keys.begin(), keys.end(), mac_keys[e->keysym.scancode]);
        if(ret != keys.end()) {
            keys.erase(ret);
        }
    }
}

static std::vector<uint8_t> listen_buffer;
void talk(uint8_t op, const uint8_t *data) {
    int adr = op >> 4;
    int reg = op & 3;
    if(adr == (mouse_reg_3[0] & 0x0f)) {
        switch(reg) {
        case 3:                 // Address/HandlerID
            if(data[2] == 0xfe) // Change address
                mouse_reg_3[0] = (mouse_reg_3[0] & 0xf0) | (data[1] & 0x0f);
            else if(data[2] == 1 || data[2] == 2 ||
                    data[2] == 4) // Change device handler ID
                mouse_reg_3[1] = data[2];
            else if(data[2] == 0x00) // Change address and enable bit
                mouse_reg_3[0] = (mouse_reg_3[0] & 0xd0) | (data[1] & 0x2f);
            break;
        }
    } else if(adr == (key_reg_3[0] & 0x0f)) {
        switch(reg) {
        case 2: // LEDs/Modifiers
            key_reg_2[0] = data[1];
            key_reg_2[1] = data[2];
            break;
        case 3:                 // Address/HandlerID
            if(data[2] == 0xfe) // Change address
                key_reg_3[0] = (key_reg_3[0] & 0xf0) | (data[1] & 0x0f);
            else if(data[2] == 0x00) // Change address and enable bit
                key_reg_3[0] = (key_reg_3[0] & 0xd0) | (data[1] & 0x2f);
            break;
        }
    }
}
void adb_reset() { SDL_ResetKeyboard(); }
static int old_mx, old_my;
void adb_out(uint8_t v) {
    if(!listen_buffer.empty()) {
        listen_buffer.push_back(v);
        if(listen_buffer.size() == 4) {
            talk(listen_buffer[0], listen_buffer.data() + 1);
            listen_buffer.clear();
            return;
        }
    }
    int adr = v >> 4;
    int cmd = v >> 2 & 3;
    int reg = v & 3;

    if((v & 0xf) == 0) {
        // SendReset
        adb_reset();
        return;
    }
    // Check which device was addressed and act accordingly
    if(adr == (mouse_reg_3[0] & 0x0f)) {

        // Mouse
        if(cmd == 2) {
            // Listen Begin
            listen_buffer.push_back(v);
            return;
        } else if(cmd == 3) {
            // Talk
            switch(reg) {
            case 0: {
                int x, y;
                auto btn = SDL_GetMouseState(&x, &y);
                int dx = x - old_mx;
                int dy = y - old_my;

                buf.push_back((dx & 0x7f) |
                              (SDL_BUTTON_LMASK & btn ? 0 : 0x80));
                buf.push_back((dy & 0x7f) |
                              (SDL_BUTTON_RMASK & btn ? 0 : 0x80));
                buf.push_back(((dy >> 3) & 0x70) | ((dx >> 7) & 0x07) |
                              (SDL_BUTTON_MMASK & btn ? 0x08 : 0x88));
                old_mx = x;
                old_my = y;
                break;
            }

            case 1: // Extended mouse protocol
                buf = {'a', 'p', 'p', 'l', 300 >> 8, 300 & 0xff, 1, 3};
                break;
            case 3: // Address/HandlerID
                buf.push_back((mouse_reg_3[0] & 0xf0) | (rand() & 0x0f));
                buf.push_back(mouse_reg_3[1]);
                break;
            default:
                buf.clear();
                break;
            }
        }

    } else if(adr == (key_reg_3[0] & 0x0f)) {

        // Keyboard
        if(cmd == 2) {
            // Listen Begin
            listen_buffer.push_back(v);
            return;
        } else if(cmd == 3) {

            // Talk
            switch(reg) {
            case 0:
                if(keys.empty()) {
                    buf = {0xff, 0xff};
                } else if(keys.size() == 1) {
                    buf = {*keys.begin(), 0xff};
                    keys.clear();
                } else {
                    buf.clear();
                    auto ky = keys.begin();
                    buf.push_back(*ky);
                    ky = keys.erase(ky);
                    buf.push_back(*ky);
                    ky = keys.erase(ky);
                }
                break;
            case 2: { // LEDs/Modifiers
                auto mod = SDL_GetModState();
                auto kbd = SDL_GetKeyboardState(nullptr);
                uint8_t reg2hi = 0xff;
                uint8_t reg2lo = key_reg_2[1] | 0xf8;
                if(mod & KMOD_SCROLL) // Scroll Lock
                    reg2lo &= ~0x40;
                if(mod & KMOD_NUM) // Num Lock
                    reg2lo &= ~0x80;
                if(mod & KMOD_LGUI) // Command
                    reg2hi &= ~0x01;
                if(mod & KMOD_LALT) // Option
                    reg2hi &= ~0x02;
                if(mod & KMOD_LSHIFT) // Shift
                    reg2hi &= ~0x04;
                if(mod & KMOD_LCTRL) // Control
                    reg2hi &= ~0x08;
                if(mod & KMOD_CAPS) // Caps Lock
                    reg2hi &= ~0x20;
                if(kbd[SDL_SCANCODE_DELETE]) // Delete
                    reg2hi &= ~0x40;
                buf = {reg2hi, reg2lo};
                break;
            }
            case 3: // Address/HandlerID
                buf.push_back((key_reg_3[0] & 0xf0) | (rand() & 0x0f));
                buf.push_back(key_reg_3[1]);
                break;
            default:
                buf.clear();
                break;
            }
        }

    } else // Unknown address
        if(cmd == 3)
            buf.clear(); // Talk: 0 bytes of data
}