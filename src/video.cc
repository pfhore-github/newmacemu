#include "SDL3/SDL.h"
#include "SDL3/SDL_render.h"
#include <stdint.h>
#include <vector>
std::vector<uint32_t> clut;
std::vector<uint8_t> vram(16 * 1024 * 1024);
SDL_Window *w;
static SDL_Renderer *r;
static SDL_Texture *t;
enum MODE { MONO, C4, C16, C256, C32K, C16M };
static MODE mode = C16M;
void init_video() {
    r = SDL_CreateRenderer(w, nullptr);
    t = SDL_CreateTexture(r, SDL_PIXELFORMAT_ARGB8888,
                          SDL_TEXTUREACCESS_STREAMING, 640, 480);
}
void video_update() {
    SDL_RenderClear(r);
    SDL_Surface *s;
    SDL_LockTextureToSurface(t, nullptr, &s);
    switch(mode) {
    case MODE::C16M:
        memcpy(s->pixels, vram.data(), sizeof(uint32_t) * 640 * 480);
        break;
    default:
        break;
    }
    SDL_UnlockTexture(t);
    SDL_RenderTexture(r, t, nullptr, nullptr);
    SDL_RenderPresent(r);
}