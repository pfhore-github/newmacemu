#define BOOST_TEST_MODULE test
#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "SDL.h"
#include "jit.hpp"
#include "memory.hpp"
#include "test.hpp"
#include <boost/test/unit_test.hpp>
#include <errno.h>
#include <fcntl.h>
#include <memory>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
Cpu cpu;
extern run_t run_table[0x10000];
const double sg_v[] = {-1.0, 1.0};
const mpfr_rnd_t RND_MODES[4] = {MPFR_RNDN, MPFR_RNDZ, MPFR_RNDU, MPFR_RNDD};

void initBus();
void init_fpu();
void jit_init();
volatile bool testing;
Prepare::Prepare() {
    reset_fpu();
    for(int i = 0; i < 8; ++i) {
        cpu.D[i] = cpu.A[i] = 0;
    }
    cpu.ISP = cpu.USP = cpu.MSP = cpu.A[7] = 0xC00;
    cpu.oldpc = 0;
    cpu.Z = cpu.X = cpu.V = cpu.C = cpu.N = false;
    cpu.S = true;
    cpu.T = 0;
    cpu.PC = 0;
    cpu.EA = 0;
    cpu.movem_run = false;

    cpu.TCR_E = true;
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
    cpu.VBR = 0x4000;
    for(int i = 0; i < 64; ++i) {
        TEST::SET_L(0x4000 + (i << 2), 0x1000 + (i << 2));
    }
    cpu.URP = cpu.SRP = 0x8000;
    TEST::SET_L(0x8000, 0x8202); // ROOT-dsc
    TEST::SET_L(0x8200, 0x8302); // Ptr-dsc
    TEST::SET_L(0x8300, 1);      // TEST Area
    TEST::SET_L(0x8304, 0x1005); // Write protected
    TEST::SET_L(0x8308, 0x2081); // System only
    TEST::SET_L(0x830C, 0x3000); // Invalid Area
    cpu.I = 0;
    cpu.ex_n = 0;
}
void init_run_table();
void init_emu();

struct MyGlobalFixture {
    MyGlobalFixture() { init_emu(); }
};
BOOST_TEST_GLOBAL_FIXTURE(MyGlobalFixture);

void run_op();

void do_poweroff() { quick_exit(0); }
#ifdef TEST_JIT
extern std::unordered_map<uint32_t, std::shared_ptr<jit_cache>> jit_tables;
#endif
void run_test(uint32_t pc) {
    cpu.PC = pc;
    cpu.in_exception = false;
    testing = true;
    if(setjmp(cpu.ex) == 0) {
        while(testing) {
#ifndef TEST_JIT
            run_op();
#else
            auto e = jit_tables[cpu.PC].get();
            (*e->exec)((cpu.PC - e->begin) >> 1);
#endif
        }
    }
}
