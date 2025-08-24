#include "68040.hpp"
#include "SDL3/SDL.h"
#include "SDL3/SDL_thread.h"
#include "exception.hpp"
#include "inline.hpp"
#include "memory.hpp"
#include <errno.h>
#include <fcntl.h>
#include <memory>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
Cpu cpu;

void RESET();

void debug_activate();
void run_cpu();

void do_poweroff() { quick_exit(0); }
void debug_activate();
void video_update();
void keydown(const SDL_KeyboardEvent *e);
void keyup(const SDL_KeyboardEvent *e);
void init_emu();
void update_event() {
    SDL_Event e;
    while(SDL_PollEvent(&e)) {
        switch(e.type) {
        case SDL_EVENT_KEY_DOWN:
            keydown(&e.key);
            break;
        case SDL_EVENT_KEY_UP:
            keyup(&e.key);
            break;
        case SDL_EVENT_QUIT:
            quick_exit(0);
        }
    }
}
void trace();
void run_lua();
void init_lua();
int main(int argc, char **argv) {
    init_emu();
    RESET();
    cpu.A[7] = ReadL(cpu.VBR);
    cpu.PC = ReadL(cpu.VBR + 4);
    cpu.movem_run = false;
    for(int i = 1; i < argc; ++i) {
        if(strcmp(argv[i], "-d") == 0) {
            // debug
            debug_activate();
            quick_exit(0);
        } else if(strcmp(argv[i], "-t") == 0) {
            // trace
            trace();
            quick_exit(0);
        } else if(strcmp(argv[i], "-l") == 0) {
            init_lua();
            run_lua();
            quick_exit(0);
        }
    }
    std::thread th{run_cpu};
    th.detach();
    for(;;) {
        if(cpu.inturrupt.load()) {
            cpu.run.resume();
        }
        video_update();
        update_event();
    }
    return 0;
}
