#define BOOST_TEST_MODULE test
#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include "memory.hpp"
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
    cpu.u_atc.clear();
    cpu.ug_atc.clear();
    cpu.s_atc.clear();
    cpu.sg_atc.clear();
    cpu.DTTR[0].E = false;
    cpu.DTTR[1].E = false;
    cpu.ITTR[0].E = false;
    cpu.ITTR[1].E = false;

    // EXCEPTION TABLE
    cpu.VBR = 0x400;
    for(int i = 0; i < 64; ++i) {
        TEST::SET_L(0x400 + (i << 2), 0x5000 + (i << 2));
    }
}
int GET_EXCEPTION() { return (cpu.PC - 0x5000) >> 2; }
void init_run_table();
extern bool rom_is_overlay;
struct MyGlobalFixture {
    MyGlobalFixture() {
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
        init_fpu();
        rom_is_overlay = false;
    }
};

BOOST_TEST_GLOBAL_FIXTURE(MyGlobalFixture);

uint8_t LoadIO_B(uint32_t) { return 0; }
uint16_t LoadIO_W(uint32_t) { return 0; }
uint32_t LoadIO_L(uint32_t) { return 0; }
void StoreIO_B(uint32_t, uint8_t) {}
void StoreIO_W(uint32_t, uint16_t) {}
void StoreIO_L(uint32_t, uint32_t) {}
bool is_reset = false;
void bus_reset() { is_reset = true; }
void run_op();
void decode() { throw DecodeError{}; }
int decode_and_run() {
    cpu.in_exception = false;
    cpu.af_value.ea = 0;
    cpu.must_trace = cpu.T == 2;
    cpu.oldpc = cpu.PC;
    uint16_t op = FETCH();
    if(setjmp(cpu.ex_buf) == 0) {
        if(auto p = run_table[op]) {
            (*p)(op, op >> 9 & 7, op >> 3 & 7, op & 7);
        } else {
            ILLEGAL_OP();
        }
    }
    return cpu.PC - cpu.oldpc - 2;
}