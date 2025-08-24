#include "68040.hpp"
#include "SDL3/SDL.h"
#include "SDL3_net/SDL_net.h"
#include "exception.hpp"
#include "memory.hpp"
#include "inline.hpp"
#include <errno.h>
#include <fcntl.h>
#include <memory>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <new>
uint8_t *RAM;
size_t RAMSize;
uint8_t *ROM;
size_t ROMSize;
size_t ROMMask;
extern SDL_Window *w;
void do_irq(int i) {
    cpu.run.resume();
    cpu.inturrupt.store(i);
}
void init_run_table();
void initBus();
void init_fpu();
void jit_init();
void init_video();
void init_emu() {
#ifndef CI
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    at_quick_exit(SDL_Quit);
    w = SDL_CreateWindow("macintosh", 640, 480,
                         0); // TODO: We should use GL shader?
    init_video();
    NET_Init();
    at_quick_exit(NET_Quit);
#endif
    RAMSize = 64 * 1024 * 1024;
    RAM = new (std::align_val_t(4096)) uint8_t[RAMSize];
    int fd = open("../quadra950.rom", O_RDONLY);
    if(fd == -1) {
        perror("cannot open rom");
        exit(1);
    }
    struct stat sb;
    if(fstat(fd, &sb) == -1) /* To obtain file size */ {
        perror("fstat");
        exit(1);
    }
    ROMSize = sb.st_size;
    ROMMask = ROMSize - 1;
    ROM = new (std::align_val_t(4096)) uint8_t[ROMSize];
    read(fd, ROM, ROMSize);

    // instead overlay, just copy
    memcpy(RAM, ROM, ROMSize);

    cpu.fpu = createFPU(FPU_TYPE::M68040_FULL);
    cpu.fpu->init();
    init_run_table();
    initBus();
    jit_init();
}
