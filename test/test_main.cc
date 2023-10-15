#define BOOST_TEST_MODULE test
#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "SDL.h"
#include "memory.hpp"
#include "test.hpp"
#include <boost/test/unit_test.hpp>
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
extern run_t run_table[0x10000];
const double sg_v[] = {-1.0, 1.0};
const mpfr_rnd_t RND_MODES[4] = {MPFR_RNDN, MPFR_RNDZ, MPFR_RNDU, MPFR_RNDD};

void initBus();
void init_fpu();
Prepare::Prepare() {
    reset_fpu();
    for(int i = 0; i < 8; ++i) {
        cpu.D[i] = cpu.A[i] = 0;
    }
    cpu.ISP = cpu.USP = cpu.MSP = cpu.A[7] = 0x6C00;
    cpu.oldpc = 0;
    cpu.Z = cpu.X = cpu.V = cpu.C = cpu.N = false;
    cpu.T = 0;
    memset(RAM.data(), 0, 0x8000);
    cpu.PC = 0;
    cpu.EA = 0;
    cpu.movem_run = false;

    cpu.TCR_E = false;
    cpu.TCR_P = false;
    cpu.must_trace = false;
    for(int i = 0; i < 2; ++i) {
        cpu.l_atc[i].clear();
        cpu.g_atc[i].clear();
    }
    cpu.DTTR[0].E = false;
    cpu.DTTR[1].E = false;
    cpu.ITTR[0].E = false;
    cpu.ITTR[1].E = false;
    cpu.in_exception = false;
    // EXCEPTION TABLE
    cpu.VBR = 0x400;
    for(int i = 0; i < 64; ++i) {
        TEST::SET_L(0x400 + (i << 2), 0x5000 + (i << 2));
    }
    cpu.I = 0;
}
int GET_EXCEPTION() { return (cpu.PC - 0x5000) >> 2; }
void init_run_table();
extern bool rom_is_overlay;
struct MyGlobalFixture {
    MyGlobalFixture() {
        SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER);
        RAM.resize(4 * 1024 * 1024);
        int fd = open("../quadra950.rom", O_RDONLY);
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
        init_fpu();
    }
    ~MyGlobalFixture() { SDL_Quit(); }
};
BOOST_TEST_GLOBAL_FIXTURE(MyGlobalFixture);

bool is_reset = false;
__attribute__((weak)) void bus_reset() { is_reset = true; }
void run_op();

void do_poweroff() { quick_exit(0); }

void do_irq(int i) {
    std::lock_guard<std::mutex> lk(cpu.mtx_);
    cpu.sleeping = false;
    cpu.cond_.notify_one();
    cpu.inturrupt.store(i);
}

int run_test() {
    cpu.in_exception = false;
    if(setjmp(cpu.ex_buf) == 0) {
        run_op();
    }
    if(cpu.PC >= 0x5000) {
        return GET_EXCEPTION();
    } else {
        return 0;
    }
}