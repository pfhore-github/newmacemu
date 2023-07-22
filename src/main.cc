#include "68040.hpp"
#include "SDL.h"
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

void run_op();
int main() {
    SDL_Init(SDL_INIT_EVERYTHING);
    atexit(SDL_Quit);
    RAM.resize(64 * 1024 * 1024);
    int fd = open("../quadra700.rom", O_RDONLY);
    if(fd == -1) {
        perror("cannot open rom");
    }
    struct stat sb;

    if(fstat(fd, &sb) == -1) /* To obtain file size */
        perror("fstat");
    ROMSize = sb.st_size;
    ROM = static_cast<uint8_t *>(
        mmap(nullptr, ROMSize, PROT_READ, MAP_SHARED, fd, 0));
    init_run_table();
    initBus();
    RESET();
    cpu.PC = 0x2A;
    cpu.movem_run = false;
    for(;;) {
        printf("%06X: D=", cpu.PC);
        for(int i = 0; i < 8; ++i) {
            printf("%08x,", cpu.D[i]);
        }
        printf("A=");
        for(int i = 0; i < 8; ++i) {
            printf("%08x,", cpu.A[i]);
        }
        printf("\n");
        run_op();
    }
    return 0;
}
