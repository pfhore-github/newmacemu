#ifndef CI
#include "68040.hpp"
#include "SDL_events.h"
#include "SDL_keycode.h"
#include "SDL_ttf.h"
#include "bus.hpp"
#include "memory.hpp"
#include <fmt/format.h>
#include <unordered_set>
static std::unordered_set<uint32_t> breakpoints;
static uint32_t stoppoints;
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
std::string disasm();
void run_op();
static char PREC_S[] = {'X', 'S', 'D', '?'};
static SDL_Window *debugWin;
static SDL_Renderer *renderer;
static TTF_Font *font = nullptr;
constexpr SDL_Color WHITE = SDL_Color{0xff, 0xff, 0xff, 0xff};
constexpr SDL_Color GRAY = SDL_Color{0xff, 0xff, 0xff, 0xff};
static int draw_string(int x, int y, const std::string &s,
                       SDL_Color c = WHITE) {
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
    return d.w;
}
constexpr SDL_Color RED = {0xff, 0, 0, 0xff};
void drawReg() {
    for(int i = 0; i < 8; ++i) {
        draw_string(0, SIZE * i, fmt::format("D{}: {:08X}", i, cpu.D[i]));
    }
    for(int i = 0; i < 8; ++i) {
        draw_string(0, SIZE * (i + 8), fmt::format("A{}: {:08X}", i, cpu.A[i]));
    }
    int x1 = draw_string(0, SIZE * 16, fmt::format("SR: T{}", cpu.T));

    x1 += draw_string(x1, SIZE * 16, "S", cpu.S ? RED : WHITE);
    x1 += draw_string(x1, SIZE * 16, "M", cpu.M ? RED : WHITE);
    x1 += draw_string(x1, SIZE * 16, fmt::format("I{}/", cpu.I));
    x1 += draw_string(x1, SIZE * 16, "X", cpu.X ? RED : WHITE);
    x1 += draw_string(x1, SIZE * 16, "N", cpu.N ? RED : WHITE);
    x1 += draw_string(x1, SIZE * 16, "Z", cpu.Z ? RED : WHITE);
    x1 += draw_string(x1, SIZE * 16, "V", cpu.V ? RED : WHITE);
    x1 += draw_string(x1, SIZE * 16, "C", cpu.C ? RED : WHITE);
    draw_string(0, SIZE * 17, fmt::format("PC: {:08x}", cpu.PC));

    for(int i = 0; i < 8; ++i) {
        char *p;
        mpfr_asprintf(&p, "%RNf", cpu.FP[i]);
        draw_string(120, SIZE * i, fmt::format("FP{} : {}", i, p));
        mpfr_free_str(p);
    }
    x1 = 120;
    x1 += draw_string(x1, SIZE * 8, "FPCR:");
    x1 += draw_string(x1, SIZE * 8, "B", cpu.FPCR.BSUN ? RED : WHITE);
    x1 += draw_string(x1, SIZE * 8, "S", cpu.FPCR.S_NAN ? RED : WHITE);
    x1 += draw_string(x1, SIZE * 8, "E", cpu.FPCR.OPERR ? RED : WHITE);
    x1 += draw_string(x1, SIZE * 8, "O", cpu.FPCR.OVFL ? RED : WHITE);
    x1 += draw_string(x1, SIZE * 8, "U", cpu.FPCR.UNFL ? RED : WHITE);
    x1 += draw_string(x1, SIZE * 8, "Z", cpu.FPCR.DZ ? RED : WHITE);
    x1 += draw_string(x1, SIZE * 8, "I", cpu.FPCR.INEX2 ? RED : WHITE);
    x1 += draw_string(x1, SIZE * 8, "i", cpu.FPCR.INEX1 ? RED : WHITE);
    x1 +=
        draw_string(x1, SIZE * 8,
                    fmt::format("{} {}", RND_S(), PREC_S[int(cpu.FPCR.PREC)]));

    x1 = 120;
    x1 += draw_string(x1, SIZE * 9, "FPSR:");
    x1 += draw_string(x1, SIZE * 9, "N", cpu.FPSR.CC_N ? RED : WHITE);
    x1 += draw_string(x1, SIZE * 9, "Z", cpu.FPSR.CC_Z ? RED : WHITE);
    x1 += draw_string(x1, SIZE * 9, "I", cpu.FPSR.CC_I ? RED : WHITE);
    x1 += draw_string(x1, SIZE * 9, "n", cpu.FPSR.CC_NAN ? RED : WHITE);
    x1 += draw_string(
        x1, SIZE * 9,
        fmt::format("F{}{:<3d}", cpu.FPSR.QuatSign ? '-' : '+', cpu.FPSR.Quat));
    x1 = 120;
    x1 += draw_string(x1, SIZE * 10, " EXE;");
    x1 += draw_string(x1, SIZE * 10, "B", cpu.FPSR.BSUN ? RED : WHITE);
    x1 += draw_string(x1, SIZE * 10, "X", cpu.FPSR.S_NAN ? RED : WHITE);
    x1 += draw_string(x1, SIZE * 10, "E", cpu.FPSR.OPERR ? RED : WHITE);
    x1 += draw_string(x1, SIZE * 10, "O", cpu.FPSR.OVFL ? RED : WHITE);
    x1 += draw_string(x1, SIZE * 10, "U", cpu.FPSR.UNFL ? RED : WHITE);
    x1 += draw_string(x1, SIZE * 10, "Z", cpu.FPSR.DZ ? RED : WHITE);
    x1 += draw_string(x1, SIZE * 10, "I", cpu.FPSR.INEX2 ? RED : WHITE);
    x1 += draw_string(x1, SIZE * 10, "i", cpu.FPSR.INEX1 ? RED : WHITE);
    x1 = 120;
    x1 += draw_string(x1, SIZE * 11, " Acc;");
    x1 += draw_string(x1, SIZE * 11, "E", cpu.FPSR.EXC_IOP ? RED : WHITE);
    x1 += draw_string(x1, SIZE * 11, "O", cpu.FPSR.EXC_OVFL ? RED : WHITE);
    x1 += draw_string(x1, SIZE * 11, "U", cpu.FPSR.EXC_UNFL ? RED : WHITE);
    x1 += draw_string(x1, SIZE * 11, "Z", cpu.FPSR.EXC_DZ ? RED : WHITE);
    x1 += draw_string(x1, SIZE * 11, "I", cpu.FPSR.EXC_INEX ? RED : WHITE);
    draw_string(120, SIZE * 12, fmt::format("FPIR:{:08x}", cpu.FPIAR));
    draw_string(120, SIZE * 13, fmt::format("VBR :{:08x}", cpu.VBR));
    draw_string(120, SIZE * 14,
                fmt::format("SFC:{}, DFC:{}", cpu.SFC, cpu.DFC));

    x1 = 120;
    x1 += draw_string(x1, SIZE * 15,
                      fmt::format("ITTR0:{:02X}{:02X}", cpu.ITTR[0].logic_base,
                                  cpu.ITTR[0].logic_mask));
    x1 += draw_string(x1, SIZE * 15, "E", cpu.ITTR[0].E ? RED : WHITE);
    x1 += draw_string(x1, SIZE * 15,
                      fmt::format("S{}U{}C{}", cpu.ITTR[0].S, cpu.ITTR[0].U,
                                  cpu.ITTR[0].CM));
    x1 += draw_string(x1, SIZE * 15, "W", cpu.ITTR[0].W ? RED : WHITE);

    x1 = 120;
    x1 += draw_string(x1, SIZE * 16,
                      fmt::format("    1:{:02X}{:02X}", cpu.ITTR[1].logic_base,
                                  cpu.ITTR[1].logic_mask));
    x1 += draw_string(x1, SIZE * 16, "E", cpu.ITTR[1].E ? RED : WHITE);
    x1 += draw_string(x1, SIZE * 16,
                      fmt::format("S{}U{}C{}", cpu.ITTR[1].S, cpu.ITTR[1].U,
                                  cpu.ITTR[1].CM));
    x1 += draw_string(x1, SIZE * 16, "W", cpu.ITTR[1].W ? RED : WHITE);

    x1 = 120;
    x1 += draw_string(x1, SIZE * 17,
                      fmt::format("DTTR0:{:02X}{:02X}", cpu.DTTR[0].logic_base,
                                  cpu.DTTR[0].logic_mask));
    x1 += draw_string(x1, SIZE * 17, "E", cpu.DTTR[0].E ? RED : WHITE);
    x1 += draw_string(x1, SIZE * 17,
                      fmt::format("S{}U{}C{}", cpu.DTTR[0].S, cpu.DTTR[0].U,
                                  cpu.DTTR[0].CM));
    x1 += draw_string(x1, SIZE * 17, "W", cpu.DTTR[0].W ? RED : WHITE);

    x1 = 120;
    x1 += draw_string(x1, SIZE * 18,
                      fmt::format("    1:{:02X}{:02X}", cpu.DTTR[1].logic_base,
                                  cpu.DTTR[1].logic_mask));
    x1 += draw_string(x1, SIZE * 18, "E", cpu.DTTR[1].E ? RED : WHITE);
    x1 += draw_string(x1, SIZE * 18,
                      fmt::format("S{}U{}C{}", cpu.DTTR[1].S, cpu.DTTR[1].U,
                                  cpu.DTTR[1].CM));
    x1 += draw_string(x1, SIZE * 18, "W", cpu.DTTR[1].W ? RED : WHITE);

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
    SDL_RenderPresent(renderer);
}
void video_update();
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
    for(;;) {
        video_update();
        update();
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            switch(e.type) {
            case SDL_KEYDOWN:
                if(e.key.windowID == SDL_GetWindowID(debugWin)) {
                    switch(e.key.keysym.sym) {
                    case SDLK_m: {
#if 0
    printf("address?");
            char cc[64];
            echo();
            mvwgetstr(adr, 1, 0, cc);
            noecho();
            mem_v_addr = strtoul(cc, nullptr, 16) & ~7;
            delwin(adr);
            break;
#endif
                    }
                    case SDLK_q:
                        SDL_DestroyRenderer(renderer);
                        SDL_DestroyWindow(debugWin);
                        return;
                    case SDLK_s:
                        run_op();
                        break;
                    case SDLK_r: {
                        uint32_t cont = cpu.A[6];
                        while(cpu.PC != cont) {
                            run_op();
                            update();
                        }
                        break;
                    }

                    case SDLK_c: {
#if 0
                        auto adr = subwin(stdscr, 4, 20, 8, 50);
                        wbkgd(adr, COLOR_PAIR(0));
                        werase(adr);
                        mvwprintw(adr, 0, 0, "address?");
                        char cc[64];
                        echo();
                        mvwgetstr(adr, 1, 0, cc);
                        noecho();
                        delwin(adr);
                        if(cc[0] != '\0') {
                            uint32_t cont = strtoul(cc, nullptr, 16) & ~1;
                            while(cpu.PC != cont) {
                                run_op();
                                update();
                            }
                        } else {
                            timeout(1);
                            while(getch() == -1) {
                                run_op();
                                update();
                            }
                        }
#endif
                        break;
                    }

                    case SDLK_UP:
                        mem_v_addr -= 8;
                        break;
                    case SDLK_DOWN:
                        mem_v_addr += 8;
                        break;
                    case SDLK_LEFT:
                        mem_v_addr -= 0x100;
                        break;
                    case SDLK_RIGHT:
                        mem_v_addr += 0x100;
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
    }
}
#else
void debug_activate() {}
#endif