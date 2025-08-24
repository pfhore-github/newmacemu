#define BOOST_TEST_MODULE test
#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "SDL3/SDL.h"
#include "exception.hpp"
#include "jit.hpp"
#include "memory.hpp"
#include "mmu.hpp"
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
extern EXCEPTION_NUMBER ex_n;
volatile bool testing;
/* TEST MEM TABLE
    (pysical)
    0x0000-0x0FFF: CODE AREA
    0x1000-0x1FFF: USER-SCRATCH
    0x2000-0x2FFF: SYS-SCRATCH
    0x3000-0x3FFF: GUARD AREA(read only)
    0x4000-0x4FFF: GUARD AREA(system only)
    0x5000-0x5FFF: USER-STACK
    0x6000-0x6FFF: SYS-STACK
    0x7000-0x7FFF: EXCEPTION TABLE
    0x8000-0x8FFF: USER-pTABLE
    0x9000-0x9FFF: SYS-pTABLE
    0xA000-0xAFFF: I-STACK

    (user-logical)
    0x0000-0x0FFF: CODE AREA
    0x1000-0x1FFF: SCRATCH
    0x2000-0x2FFF: GUARD AREA(read only)
    0x3000-0x3FFF: GUARD AREA(system only)
    0x4000-0x4FFF: NON-RESIENT
    0x5000-0x5FFF: STACK

    (sys-logical)
    0x0000-0x0FFF: CODE AREA
    0x1000-0x1FFF: USER-SCRATCH
    0x2000-0x2FFF: GUARD AREA(read only)
    0x3000-0x3FFF: GUARD AREA(system only)
    0x4000-0x4FFF: NON-RESIENT
    0x5000-0x5FFF: USER-STACK
    0x6000-0x6FFF: EXCEPTION TABLE
    0x7000-0x7FFF: SYS-SCRATCH
    0x8000-0x8FFF: SYS-STACK
    0x9000-0x9FFF: I-STACK
*/
Prepare::Prepare() {
    if( cpu.fpu ) {
        cpu.fpu->reset();
    }
    for(int i = 0; i < 8; ++i) {
        cpu.D[i] = cpu.A[i] = 0;
    }
    cpu.ISP = 0xA000;
    cpu.MSP = 0x9000;
    cpu.USP = cpu.A[7] = 0x6000;
    cpu.oldpc = 0;
    cpu.Z = cpu.X = cpu.V = cpu.C = cpu.N = false;
    cpu.S = false;
    cpu.M = true;
    cpu.T = 0;
    cpu.PC = 0;
    cpu.EA = 0;
    cpu.movem_run = false;

    cpu.TCR_E = true;
    cpu.TCR_P = false;
    cpu.must_trace = false;
    for(int i = 0; i < 2; ++i) {
        for(int k = 0; k < 16; ++k) {
            d_atc[i][k].V = false;
            i_atc[i][k].V = false;
        }
    }
    cpu.DTTR[0].E = false;
    cpu.DTTR[1].E = false;
    cpu.ITTR[0].E = false;
    cpu.ITTR[1].E = false;
    // EXCEPTION TABLE
    cpu.VBR = 0x6000;
    for(int i = 0; i < 64; ++i) {
        TEST::SET_L(0x7000 + (i << 2), 0x3000 + (i << 2));
    }
    cpu.URP = 0x8000;
    cpu.SRP = 0x9000;
    TEST::SET_L(0x8000, 0x8202); // ROOT-dsc
    TEST::SET_L(0x8200, 0x8302); // Ptr-dsc
    TEST::SET_L(0x8300, 0x040D); // CODE AREA
    TEST::SET_L(0x8304, 0x1019); // SCRATCH
    TEST::SET_L(0x8308, 0x340D); // Write protected
    TEST::SET_L(0x830C, 0x4489); // System only
    TEST::SET_L(0x8310, 0x0000); // Invalid Area
    TEST::SET_L(0x8314, 0x5019); // Stack

    TEST::SET_L(0x9000, 0x9202); // ROOT-dsc
    TEST::SET_L(0x9200, 0x9302); // Ptr-dsc
    TEST::SET_L(0x9300, 0x040D); // CODE AREA
    TEST::SET_L(0x9304, 0x1019); // U-SCRATCH
    TEST::SET_L(0x9308, 0x340D); // Write protected
    TEST::SET_L(0x930C, 0x4489); // System only
    TEST::SET_L(0x9310, 0x0000); // Invalid Area
    TEST::SET_L(0x9314, 0x5019); // user-Stack
    TEST::SET_L(0x9318, 0x7085); // EXCEPTION TABLE
    TEST::SET_L(0x931C, 0x2099); // SYS-SCRATCH
    TEST::SET_L(0x9320, 0x6099); // SYS-STACK
    TEST::SET_L(0x9324, 0xA099); // I-STACK

    cpu.I = 0;
    ex_n = EXCEPTION_NUMBER::NO_ERR;
    cpu.fault_SSW = 0;
}
void init_run_table();
void init_emu();

struct MyGlobalFixture {
    MyGlobalFixture() { init_emu(); }
};
BOOST_TEST_GLOBAL_FIXTURE(MyGlobalFixture);

void run_op();

void do_poweroff() { quick_exit(0); }
void jit_run(uint32_t pc, int block);
void run_test(uint32_t pc, EXCEPTION_NUMBER ex) {
    cpu.PC = pc;
    testing = true;
    try {
        while(testing) {
#ifndef TEST_JIT
            cpu.inJit = false;
            run_op();
#else
            jit_run(cpu.PC, 0);
#endif
        }
        BOOST_TEST(ex == EXCEPTION_NUMBER::NO_ERR);
    } catch(M68kException &e) {
        if(ex == EXCEPTION_NUMBER::NO_ERR) {
            ex_n = e.ex_n;
            cpu.bus_lock = false;
        } else {
            BOOST_TEST(ex == e.ex_n);
        }
    }
}
