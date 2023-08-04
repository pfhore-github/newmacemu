#include "68040.hpp"
#include "SDL.h"
#include "SDL_net.h"
#include "SDL_thread.h"
#include "exception.hpp"
#include "memory.hpp"
#include "proto.hpp"
#include <errno.h>
#include <fcntl.h>
#include <memory>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
std::vector<uint8_t> RAM;
const uint8_t *ROM;
size_t ROMSize;
Cpu cpu;
void initBus();
void init_fpu();
void RESET();
void init_run_table();
void debug_activate();
void run_op();
void IRQ(int i);
void do_irq(int i) {
    std::lock_guard<std::mutex> lk(cpu.mtx_);
    cpu.sleeping = false;
    cpu.cond_.notify_one();
    cpu.inturrupt.store(i);
}
int cpu_thread(void *) {
    while(cpu.run.load()) {
        if(int i = cpu.inturrupt.load(); i >= 0) {
            IRQ(i);
            cpu.inturrupt.store(-1);
        } else {
            run_op();
        }
    }
    return 0;
}
void debug_activate();
void video_update();
void keydown(const SDL_KeyboardEvent *e);
void keyup(const SDL_KeyboardEvent *e);
extern SDL_Window *w;
extern SDL_Renderer *r;
void update_event() {}
int main(int argc, char **argv) {
    SDL_Init(SDL_INIT_EVERYTHING);
    atexit(SDL_Quit);
    SDLNet_Init();
    atexit(SDLNet_Quit);
    w = SDL_CreateWindow("macintosh", SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED, 640, 480,
                         0); // TODO We should use GL shader?
    r = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED);
    RAM.resize(64 * 1024 * 1024);
    int fd = open("../quadra950.ROM", O_RDONLY);
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
    ROM = static_cast<uint8_t *>(
        mmap(nullptr, ROMSize, PROT_READ, MAP_SHARED, fd, 0));
    init_run_table();
    initBus();
    init_fpu();
    RESET();
    cpu.PC = 0x2A;
    cpu.movem_run = false;
    for(int i = 1; i < argc; ++i) {
        if(strcmp(argv[i], "-d") == 0) {
            // debug
            debug_activate();
            SDL_DestroyWindow(w);
            exit(0);
        }
    }
    cpu.run.store(true);
    SDL_Thread *th = SDL_CreateThread(cpu_thread, "CPU", nullptr);
    SDL_Event e;
    for(;;) {
        if(cpu.inturrupt.load()) {
            {
                std::lock_guard<std::mutex> lk(cpu.mtx_);
                cpu.sleeping = false;
                cpu.cond_.notify_one();
            }
        }
        video_update();
        while(SDL_PollEvent(&e)) {
            switch(e.type) {
            case SDL_KEYDOWN:
                keydown(&e.key);
                break;
            case SDL_KEYUP:
                keyup(&e.key);
                break;
            case SDL_QUIT:
                cpu.run.store(false);
                SDL_WaitThread(th, nullptr);
                SDL_DestroyWindow(w);
                exit(0);
            }
        }
    }
    return 0;
}
