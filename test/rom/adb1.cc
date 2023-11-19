#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"

#include "SDL.h"
#include "ebus/adb.hpp"
#include "mb/glue.hpp"

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <vector>
extern std::deque<uint8_t> buf;
extern std::vector<uint8_t> adb_buf;
void adb_reset();
struct DummyADB2 : public ADBDev {
    uint8_t regv[3];
    bool flushed = false;
    DummyADB2(uint8_t devId) {
        reg3[0] = 0x60 | devId;
        reg3[1] = 0x01;
    }
    void flush() override { flushed = true; }
    void listen(int reg, const uint8_t *v) override { regv[reg] = *v; }
    std::deque<uint8_t> talk(int reg) override { return {regv[reg]}; }
};
struct PrepareROMAAA : Prepare {
    PrepareROMAAA() {
        failure_is_exception = false;
        io = std::make_unique<GLUE>();
        adb_reset();
    }
};
extern std::shared_ptr<ADBDev> adbDevs[];

BOOST_FIXTURE_TEST_SUITE(adb1, PrepareROMAAA)
BOOST_AUTO_TEST_CASE(_14E8A) {
    cpu.A[1] = 0x50F00000;
    via1->recieve_sr();
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
    via1->adb_state[0] = false;
    via1->adb_state[1] = true;
    auto test = std::make_shared<DummyADB2>(0xF);
    adbDevs[2] = test;
    cpu.D[0] = 0xF1;
    test_rom(0x14E4A, 0x14E68, {});
    BOOST_TEST(test->flushed);
}

BOOST_AUTO_TEST_CASE(_14DA8) {
    cpu.A[1] = 0x50F00000;
    via1->IF[2] = true;
    via1->dirb[3] = false;
    via1->dirb[4] = true;
    via1->dirb[5] = true;
    cpu.D[0] = 1;
    cpu.D[4] = 2;
    test_rom(0x14DA8, 0x14DFA, {});
    BOOST_TEST(!cpu.Z);
}
static void RET() { cpu.PC = cpu.A[5]; }
static void RET2() {
    cpu.Z = false;
    cpu.PC = cpu.A[5];
}

BOOST_AUTO_TEST_CASE(_14CC8) {
    cpu.A[0] = 0x408032D8;
    cpu.D[0] = 0x00010001;
    cpu.D[1] = 0;
    cpu.D[2] = 1;
    via1->dirb[3] = false;
    via1->dirb[4] = true;
    via1->dirb[5] = true;
    via1->adb_state[0] = false;
    via1->adb_state[1] = false;
    auto test = std::make_shared<DummyADB2>(0x1);
    adbDevs[2] = test;
    test_rom(0x14CC8, 0x14DA6, {
        {0x14E98, RET},
        {0x14DA8, RET2},
        }, true);
    BOOST_TEST((cpu.D[0] & 0xffff)== 0);
}
int called2 = 0;
static void RET3() {
    switch(called2++) {
    case 0:
        BOOST_TEST(cpu.D[0] == 0x00010001);
        BOOST_TEST(cpu.D[1] == 0);
        BOOST_TEST(cpu.D[2] == 1);
        cpu.D[0] = 0;
        break;
    case 1:
        BOOST_TEST(cpu.D[0] == 0x001B0001);
        BOOST_TEST(cpu.D[1] == 0);
        BOOST_TEST(cpu.D[2] == 1);
        cpu.D[0] = 0;
        break;
    case 2:
        BOOST_TEST(cpu.D[0] == 0x000E0001);
        BOOST_TEST(cpu.D[1] == 0);
        BOOST_TEST(cpu.D[2] == 1);
        cpu.D[0] = 2;
        break;
    }
    cpu.PC = cpu.A[6];
}
static void getHWInfoX() {
    BOOST_TEST(cpu.D[2] == 0);
    cpu.D[0] = 1;
    cpu.A[0] = 0x40803568;
    cpu.A[1] = 0x408038BC;
    called.push_back(cpu.PC & ~0x40800000);
    cpu.PC = cpu.A[6];
}
BOOST_AUTO_TEST_CASE(_14C4C) {
    called2 = 0;
    via1->dirb[3] = false;
    via1->dirb[4] = true;
    via1->dirb[5] = true;
    via1->adb_state[0] = false;
    via1->adb_state[1] = false;
    auto test = std::make_shared<DummyADB2>(0x1);
    adbDevs[2] = test;
    test_rom(0x14C4C, 0x14CC6,
             {
                 {0x14CC8, RET3},
                 {0x14E98, RET},
                 {0x2F18, getHWInfoX},
             });
    BOOST_TEST(cpu.D[0] != 0);
}
BOOST_AUTO_TEST_SUITE_END()