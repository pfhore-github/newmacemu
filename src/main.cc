#include "68040.hpp"
#include "SDL.h"
#include "SDL_net.h"
#include "SDL_thread.h"
#include "exception.hpp"
#include "memory.hpp"
#include "inline.hpp"
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
void run_op();
void cpu_thread() {
	for(;;) {
		if( setjmp(cpu.ex) == 0 ) {
			for(;;) {
				run_op();
			}
		} else {
			handle_exception(cpu.ex_n);
            cpu.bus_lock = false;
		}
	}
}
void do_poweroff() { quick_exit(0); }
void debug_activate();
void video_update();
void keydown(const SDL_KeyboardEvent *e);
void keyup(const SDL_KeyboardEvent *e);
void init_emu();
void update_event() {}

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
        }
    }
    std::thread th{cpu_thread};
    SDL_Event e;
    for(;;) {
        if(cpu.inturrupt.load()) {
            cpu.sleeping.store(false);
            cpu.sleeping.notify_one();
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
                quick_exit(0);
            }
        }
    }
    return 0;
}
