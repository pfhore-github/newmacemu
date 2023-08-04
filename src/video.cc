#include "SDL.h"
#include "SDL_render.h"
#include <stdint.h>
#include <vector>
std::vector<uint32_t> clut;
std::vector<std::byte> vram;
SDL_Window *w;
SDL_Renderer *r;
enum MODE { MONO, C4, C16, C256, C32K, C16M };
static MODE mode = C16M;
static bool grayscale = false;
void video_update() {
    SDL_RenderClear(r);
    switch(mode) {
    case MODE::C16M:
    default:
        break;
    }
    for(int y = 0; y < 480; ++y) {
        for(int x = 0; x < 640; ++x) {
        }
    }
    SDL_RenderPresent(r);
}