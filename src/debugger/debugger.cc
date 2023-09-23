#ifndef CI
#include "68040.hpp"
#include "SDL_events.h"
#include "SDL_keycode.h"
#include "SDL_render.h"
#include "SDL_ttf.h"
#include "bus.hpp"
#include "memory.hpp"
#include <condition_variable>
#include <fmt/format.h>
#include <mutex>
#include <thread>
#include <unordered_set>
static std::unordered_set<uint32_t> breakpoints;
constexpr int SIZE = 14;

static char RND_S() {
    switch(cpu.FPCR.RND) {
    case MPFR_RNDN:
        return 'N';
    case MPFR_RNDZ:
        return 'Z';
    case MPFR_RNDD:
        return 'M';
    case MPFR_RNDU:
        return 'P';
    default:
        return '?';
    }
}
static SDL_Window *debugWin;
static SDL_Renderer *renderer;
static TTF_Font *font = nullptr;
constexpr SDL_Color WHITE = SDL_Color{0xff, 0xff, 0xff, 0xff};
constexpr SDL_Color GRAY = SDL_Color{0xff, 0xff, 0xff, 0xff};
static int draw_string(int x, int y, const std::string &s, SDL_Color c = WHITE,
                       bool underline = false) {
    if(underline) {
        TTF_SetFontStyle(font, TTF_STYLE_UNDERLINE);
    }
    auto tt = TTF_RenderUTF8_Solid(font, s.c_str(), c);

    auto tx = SDL_CreateTextureFromSurface(renderer, tt);
    SDL_Rect d;
    d.x = x;
    d.y = y;
    d.w = tt->w;
    d.h = tt->h;
    SDL_RenderCopy(renderer, tx, nullptr, &d);
    SDL_DestroyTexture(tx);
    SDL_FreeSurface(tt);
    TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
    return d.w;
}
static std::string prompt;
static uint8_t vs[8];
static unsigned int pos;
enum class CMD { NONE, MEM, JUMP, CONTINUE };
CMD cmd = CMD::NONE;
std::string disasm();
void run_op();
static char PREC_S[] = {'X', 'S', 'D', '?'};

constexpr SDL_Color RED = {0xff, 0, 0, 0xff};
class Console {
    int x, y;
    int xbase;

  public:
    Console(int x, int y) : x(x), y(y), xbase(x) {}
    void newLine() {
        y += SIZE;
        x = xbase;
    }
    void draw(const std::string &s, SDL_Color c = WHITE, int style = 0) {
        TTF_SetFontStyle(font, style);
        x += draw_string(x, y, s, c);
        TTF_SetFontStyle(font, 0);
    }
    void drawRed(const char *c, bool b) { draw(c, b ? RED : WHITE); }
    void moveTo(int newx, int newy) {
        x = xbase = newx;
        y = newy;
    }
};

void drawReg() {
    Console cc(0, 0);
    for(int i = 0; i < 8; ++i) {
        cc.draw(fmt::format("D{}: {:08X}", i, cpu.D[i]));
        cc.newLine();
    }
    for(int i = 0; i < 8; ++i) {
        cc.draw(fmt::format("A{}: {:08X}", i, cpu.A[i]));
        cc.newLine();
    }
    cc.draw(fmt::format("SR: T{}", cpu.T));
    cc.drawRed("S", cpu.S);
    cc.drawRed("M", cpu.M);
    cc.draw(fmt::format("I{}/", cpu.I));
    cc.drawRed("X", cpu.X);
    cc.drawRed("N", cpu.N);
    cc.drawRed("Z", cpu.Z);
    cc.drawRed("V", cpu.V);
    cc.drawRed("C", cpu.C);
    cc.newLine();
    cc.draw(fmt::format("PC: {:08X}", cpu.PC));

    cc.moveTo(120, 0);
    for(int i = 0; i < 8; ++i) {
        char *p;
        mpfr_asprintf(&p, "%RNf", cpu.FP[i]);
        cc.draw(fmt::format("FP{} : {}", i, p));
        cc.newLine();
        mpfr_free_str(p);
    }
    cc.draw("FPCR:");
    cc.drawRed("B", cpu.FPCR.BSUN);
    cc.drawRed("S", cpu.FPCR.S_NAN);
    cc.drawRed("E", cpu.FPCR.OPERR);
    cc.drawRed("O", cpu.FPCR.OVFL);
    cc.drawRed("U", cpu.FPCR.UNFL);
    cc.drawRed("Z", cpu.FPCR.DZ);
    cc.drawRed("I", cpu.FPCR.INEX2);
    cc.drawRed("i", cpu.FPCR.INEX1);
    cc.draw(fmt::format("{}{}", RND_S(), PREC_S[int(cpu.FPCR.PREC)]));
    cc.newLine();

    cc.draw("FPSR:");
    cc.drawRed("N", cpu.FPSR.CC_N);
    cc.drawRed("Z", cpu.FPSR.CC_Z);
    cc.drawRed("I", cpu.FPSR.CC_I);
    cc.drawRed("n", cpu.FPSR.CC_NAN);
    cc.draw(
        fmt::format("F{}{:<3d}", cpu.FPSR.QuatSign ? '-' : '+', cpu.FPSR.Quat));
    cc.newLine();

    cc.draw(" EXE;");
    cc.drawRed("B", cpu.FPSR.BSUN);
    cc.drawRed("X", cpu.FPSR.S_NAN);
    cc.drawRed("E", cpu.FPSR.OPERR);
    cc.drawRed("O", cpu.FPSR.OVFL);
    cc.drawRed("U", cpu.FPSR.UNFL);
    cc.drawRed("Z", cpu.FPSR.DZ);
    cc.drawRed("I", cpu.FPSR.INEX2);
    cc.drawRed("i", cpu.FPSR.INEX1);
    cc.newLine();

    cc.draw(" AcE;");
    cc.drawRed("E", cpu.FPSR.EXC_IOP);
    cc.drawRed("O", cpu.FPSR.EXC_OVFL);
    cc.drawRed("U", cpu.FPSR.EXC_UNFL);
    cc.drawRed("Z", cpu.FPSR.EXC_DZ);
    cc.drawRed("I", cpu.FPSR.EXC_INEX);
    cc.newLine();

    cc.draw(fmt::format("FPIR:{:08X}", cpu.FPIAR));
    cc.newLine();
    cc.draw(fmt::format("VBR :{:08X}", cpu.VBR));
    cc.newLine();
    cc.draw(fmt::format("SFC:{}, DFC:{}", cpu.SFC, cpu.DFC));
    cc.newLine();

    cc.draw(fmt::format("ITTR0:{:02X}{:02X}", cpu.ITTR[0].logic_base,
                        cpu.ITTR[0].logic_mask));
    cc.drawRed("E", cpu.ITTR[0].E);
    cc.draw(
        fmt::format("S{}U{}C{}", cpu.ITTR[0].S, cpu.ITTR[0].U, cpu.ITTR[0].CM));
    cc.drawRed("W", cpu.ITTR[0].W);
    cc.newLine();

    cc.draw(fmt::format("    1:{:02X}{:02X}", cpu.ITTR[1].logic_base,
                        cpu.ITTR[1].logic_mask));
    cc.drawRed("E", cpu.ITTR[0].E);
    cc.draw(
        fmt::format("S{}U{}C{}", cpu.ITTR[1].S, cpu.ITTR[1].U, cpu.ITTR[1].CM));
    cc.drawRed("W", cpu.ITTR[1].W);
    cc.newLine();

    cc.draw(fmt::format("DTTR0:{:02X}{:02X}", cpu.DTTR[0].logic_base,
                        cpu.DTTR[0].logic_mask));
    cc.drawRed("E", cpu.DTTR[0].E);
    cc.draw(
        fmt::format("S{}U{}C{}", cpu.DTTR[0].S, cpu.DTTR[0].U, cpu.DTTR[0].CM));
    cc.drawRed("W", cpu.DTTR[0].W);
    cc.newLine();

    cc.draw(fmt::format("    1:{:02X}{:02X}", cpu.DTTR[1].logic_base,
                        cpu.DTTR[1].logic_mask));
    cc.drawRed("E", cpu.DTTR[0].E);
    cc.draw(
        fmt::format("S{}U{}C{}", cpu.DTTR[1].S, cpu.DTTR[1].U, cpu.DTTR[1].CM));
    cc.drawRed("W", cpu.DTTR[1].W);
    cc.newLine();
}
uint32_t mem_v_addr = 0;

void keydown(const SDL_KeyboardEvent *e);
void keyup(const SDL_KeyboardEvent *e);

void update() {
    SDL_RenderClear(renderer);
    drawReg();
    draw_string(300, 0, "MEMORY VIEW");
    for(int y = 0; y < 12; ++y) {
        std::string line = fmt::format("{:08X}: ", mem_v_addr + 8 * y);
        for(int x = 0; x < 8; ++x) {
            try {
                line +=
                    fmt::format("{:02x} ", BusReadB(mem_v_addr + 8 * y + x));
            } catch(AccessFault &) {
                line += "?? ";
            }
        }
        line += " ";
        for(int x = 0; x < 8; ++x) {
            try {
                char c = BusReadB(mem_v_addr + 8 * y + x);
                if(isprint(c)) {
                    line += c;
                } else {
                    line += ".";
                }
            } catch(AccessFault &) {
                line += "?";
            }
        }
        draw_string(300, (SIZE) * (y + 1), line);
    }

    draw_string(300, SIZE * 15, "DISASSEMLBER");
    uint32_t pc = cpu.PC;
    for(int u = 0; u < 8; ++u) {
        uint32_t p = cpu.PC;
        std::string ret = disasm();
        draw_string(300, SIZE * (16 + u),
                    fmt::format("{}{:08X}: {}", u == 0 ? '>' : ' ', p, ret));
    }
    cpu.PC = pc;

    if(!prompt.empty()) {
        draw_string(0, 300, prompt);

        int x = 0;
        for(int k = 7; k >= 0; --k) {
            auto v2 = fmt::format("{:X}", vs[k]);
            x += draw_string(x, 300 + SIZE, v2, WHITE,
                             static_cast<int>(pos) == k);
        }
    }
    SDL_RenderPresent(renderer);
}
void video_update();
std::atomic<uint32_t> until = 0;
std::atomic<int64_t> stepc = 0;
static int debugger_cpu(void *) {
    for(;;) {
        while(cpu.run.load() && (cpu.PC & 0xfffff) != (until & 0xfffff) &&
              --stepc >= 0) {
            if(setjmp(cpu.ex_buf) == 0) {
                run_op();
            }
        }
        cpu.run.store(false);
        cpu.run.wait(false);
    }
    return 0;
}
uint32_t nextpc();
void continue_cpu() {
    cpu.run.store(true);
    cpu.run.notify_one();
}
void input_hex(int c) {
    vs[pos] = c;
    if(pos != 0) {
        --pos;
    }
}

uint32_t debug_value() {
    uint32_t x = 0;
    for(int k = 0; k < 8; ++k) {
        x |= vs[k] << (k * 4);
    }
    return x;
}
void handler_debuggerEvent(const SDL_Event &e) {
    switch(e.type) {
    case SDL_KEYDOWN:
        if(e.key.windowID == SDL_GetWindowID(debugWin)) {
            if(cpu.run.load()) {
                if(e.key.keysym.sym == SDLK_SPACE) {
                    cpu.run.store(false);
                }
                return;
            }
            switch(e.key.keysym.sym) {
            case SDLK_m:
                prompt = "address";
                cmd = CMD::MEM;
                for(int i = 0; i < 8; ++i) {
                    vs[i] = (mem_v_addr >> (4 * i)) & 0xf;
                }
                pos = 7;
                break;
            case SDLK_RETURN:
                switch(cmd) {
                case CMD::MEM: 
                    mem_v_addr = debug_value()  & ~7;
                    break;
                case CMD::CONTINUE:
                    until = debug_value()  & ~1;
                    stepc = 0x7FFFFFFFLL;
                    continue_cpu();
                    break;
                case CMD::JUMP:
                    cpu.PC = debug_value()  & ~1;
                    break;
                }
                prompt = "";
                cmd = CMD::NONE;
                break;
            case SDLK_0:
            case SDLK_KP_0:
                if(prompt == "address") {
                    input_hex(0);
                }
                break;
            case SDLK_1:
            case SDLK_KP_1:
                if(prompt == "address") {
                    input_hex(1);
                }
                break;
            case SDLK_2:
            case SDLK_KP_2:
                if(prompt == "address") {
                    input_hex(2);
                }
                break;
            case SDLK_3:
            case SDLK_KP_3:
                if(prompt == "address") {
                    input_hex(3);
                }
                break;
            case SDLK_4:
            case SDLK_KP_4:
                if(prompt == "address") {
                    input_hex(4);
                }
                break;
            case SDLK_5:
            case SDLK_KP_5:
                if(prompt == "address") {
                    input_hex(5);
                }
                break;
            case SDLK_6:
            case SDLK_KP_6:
                if(prompt == "address") {
                    input_hex(6);
                }
                break;
            case SDLK_7:
            case SDLK_KP_7:
                if(prompt == "address") {
                    input_hex(7);
                }
                break;
            case SDLK_8:
            case SDLK_KP_8:
                if(prompt == "address") {
                    input_hex(8);
                }
                break;
            case SDLK_9:
            case SDLK_KP_9:
                if(prompt == "address") {
                    input_hex(9);
                }
                break;
            case SDLK_a:
            case SDLK_KP_A:
                if(prompt == "address") {
                    input_hex(10);
                }
                break;
            case SDLK_b:
            case SDLK_KP_B:
                if(prompt == "address") {
                    input_hex(11);
                }
                break;
            case SDLK_KP_C:
                if(prompt == "address") {
                    input_hex(12);
                }
                break;
            case SDLK_d:
            case SDLK_KP_D:
                if(prompt == "address") {
                    input_hex(13);
                }
                break;
            case SDLK_e:
            case SDLK_KP_E:
                if(prompt == "address") {
                    input_hex(14);
                }
                break;
            case SDLK_f:
            case SDLK_KP_F:
                if(prompt == "address") {
                    input_hex(15);
                }
                break;
            case SDLK_q:
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(debugWin);
                return;
            case SDLK_s:
                stepc = 1;
                until = 0;
                continue_cpu();
                break;
            case SDLK_r:
                until = cpu.A[6];
                stepc = 0x7FFFFFFFFFFFFFFFLL;
                continue_cpu();
                break;

            case SDLK_c:
                if(prompt == "address") {
                    input_hex(12);
                } else {
                    prompt = "address";
                    cmd = CMD::CONTINUE;
                    pos = 7;
                }
                break;
            case SDLK_g:
                prompt = "address";
                cmd = CMD::JUMP;
                pos = 7;
                break;
            case SDLK_ESCAPE:
                prompt = "";
                cmd = CMD::NONE;
                break;
            case SDLK_UP:
                if(prompt == "address") {
                    vs[pos] = (vs[pos] - 1) & 0xf;
                } else {
                    mem_v_addr -= 8;
                }
                break;
            case SDLK_DOWN:
                if(prompt == "address") {
                    vs[pos] = (vs[pos] + 1) & 0xf;
                } else {
                    mem_v_addr += 8;
                }
                break;
            case SDLK_LEFT:
                if(prompt == "address") {
                    pos = std::min(7u, pos + 1);
                } else {
                    mem_v_addr -= 0x100;
                }
                break;
            case SDLK_RIGHT:
                if(prompt == "address") {
                    pos = std::max(0u, pos - 1);
                } else {
                    mem_v_addr += 0x100;
                }
                break;
            }
        } else {
            keydown(&e.key);
        }
        break;
    case SDL_KEYUP:
        keyup(&e.key);
        break;

    case SDL_QUIT:
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(debugWin);
        exit(0);
    }
}

void debug_activate() {
    if(!TTF_WasInit()) {
        TTF_Init();
        atexit(TTF_Quit);
        font = TTF_OpenFont("../unifont.otf", SIZE);
    }
    debugWin =
        SDL_CreateWindow("DEBUGGER", SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(debugWin, -1, SDL_RENDERER_ACCELERATED);
    SDL_Thread *tt = SDL_CreateThread(debugger_cpu, "CPU", nullptr);
    SDL_DetachThread(tt);

    cpu.run.store(false);
    for(;;) {
        //        video_update();
        update();
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            handler_debuggerEvent(e);
        }
        SDL_Delay(20);
    }
}
#else
void debug_activate() {}
#endif