#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"

#include "SDL.h"
#include "mb/glue.hpp"

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <vector>
extern std::deque<uint8_t> buf;
extern std::vector<uint8_t> adb_buf;
extern bool adb_irq;
extern bool adb_attentioned;
struct PrepareROMAAA : Prepare {
    PrepareROMAAA() {
        failure_is_exception = false;
        io = std::make_unique<GLUE>();
    }
};
BOOST_FIXTURE_TEST_SUITE(adb1, PrepareROMAAA)
BOOST_AUTO_TEST_CASE(_14E8A) {
    cpu.A[1] = 0x50F00000;
    via1->IF[2] = true;
    test_rom(0x14E8A, 0x14E96, {});
}

BOOST_AUTO_TEST_CASE(_14E98) {
    cpu.A[1] = 0x50F00000;
    test_rom(0x14E98, 0x14EA2, {});
}
BOOST_AUTO_TEST_CASE(_14E6A) {
    cpu.A[1] = 0x50F00000;
    via1->IF[2] = true;
    via1->dirb[4] = true;
    via1->dirb[5] = true;
    via1->ACR.sr_c = SR_C::IN_EC;
    buf = {0x23};
    test_rom(0x14E6A, 0x14E88, {});
    BOOST_TEST(cpu.D[0] == 0x23);
}

BOOST_AUTO_TEST_CASE(_14E4A) {
    cpu.A[1] = 0x50F00000;
    via1->IF[2] = true;
    via1->dirb[4] = true;
    via1->dirb[5] = true;
    via1->ACR.sr_c = SR_C::OUT_EC;
    adb_attentioned = true;
    cpu.D[0] = 0x20;
    test_rom(0x14E4A, 0x14E68, {});
    BOOST_TEST(!adb_attentioned);
}

BOOST_AUTO_TEST_CASE(_14DA8) {
    cpu.A[1] = 0x50F00000;
    via1->IF[2] = true;
    via1->dirb[4] = true;
    via1->dirb[5] = true;
    adb_attentioned = false;
    cpu.D[0] = 1;
    cpu.D[4] = 20;
    adb_irq = true;
    test_rom(0x14DA8, 0x14DFA, {});
    BOOST_TEST(!cpu.Z);
}
static void RET() { cpu.PC = cpu.A[5]; }
static void RET2() { cpu.PC = 0x40814CC8; }
BOOST_AUTO_TEST_CASE(_14CC8) {
    cpu.A[0] = 0x408032D8;
    cpu.D[0] = 0x00010001;
    cpu.D[1] = 0;
    cpu.D[2] = 1;
    adb_irq = true;
    via1->dirb[4] = true;
    via1->dirb[5] = true;
    test_rom(0x14CC8, 0x14DA8,
             {{0x14E98, RET}, {0x14E8A, RET}, {0x14E00, RET2}}, true);
}
BOOST_AUTO_TEST_SUITE_END()