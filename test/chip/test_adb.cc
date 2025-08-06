#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"

#include "SDL3/SDL.h"
#include "bus.hpp"
#include "chip/via.hpp"
#include "ebus/adb.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <vector>
namespace bdata = boost::unit_test::data;
extern std::deque<uint8_t> buf;
extern std::vector<uint8_t> adb_buf;
void do_adb_irq();
extern std::shared_ptr<ADBDev> adbDevs[];
struct DummyADB : public ADBDev {
    uint8_t regv[3];
    bool flushed = false;
    DummyADB() {
        reg3[0] = 0xAF;
        reg3[1] = 0x01;
    }
    void flush() override { flushed = true; }
    void listen(int reg, const uint8_t *v) override { regv[reg] = *v; }
    std::deque<uint8_t> talk(int reg) override { return {regv[reg]}; }
};
BOOST_FIXTURE_TEST_SUITE(adb, Prepare)

BOOST_AUTO_TEST_CASE(flush) {
    auto test = std::make_shared<DummyADB>();
    adbDevs[2] = test;
    test->regv[1] = 0x40;

    via1->ACR.sr_c = SR_C::OUT_EC;
    via1->dirb[5] = true;
    via1->dirb[4] = true;
    via1->adb_state[0] = false;
    via1->adb_state[1] = false;
    via1->write(10, 0xF1);
    // send cmd
    via1->write(0, (via1->read(0) & ~(3 << 4)) | 1 << 4);
    via1->write(10, 1);
    via1->write(0, (via1->read(0) & ~(3 << 4))| 3 << 4);
    // get response
    via1->ACR.sr_c = SR_C::IN_EC;
    via1->write(0, (via1->read(0) & ~(3 << 4)) | 1 << 4);
    BOOST_TEST(via1->read(13) & 1 << 2);
    uint8_t v = via1->read(10);
    BOOST_TEST(v == 1);
    via1->write(0, via1->read(0) & ~1 << 4);
    BOOST_TEST(test->flushed);
}

BOOST_AUTO_TEST_CASE(listen) {
    auto test = std::make_shared<DummyADB>();
    adbDevs[2] = test;
    test->regv[1] = 0x40;

    via1->ACR.sr_c = SR_C::OUT_EC;
    via1->dirb[5] = true;
    via1->dirb[4] = true;
    via1->adb_state[0] = true;
    via1->adb_state[1] = true;
    via1->write(10, 1);
    via1->write(0, (via1->read(0) & ~(3 << 4)) | 3 << 4);
    via1->write(10, 0xF9);
    via1->write(0, (via1->read(0) & ~(3 << 4)) | 0 << 4);
    via1->write(10, 0x44);
    via1->write(0, (via1->read(0) & ~(3 << 4)) | 1 << 4);
    via1->write(10, 1);
    via1->write(0, (via1->read(0) & ~(3 << 4)) | 3 << 4);
    BOOST_TEST(test->regv[1] == 0x44);
}

BOOST_AUTO_TEST_CASE(talk) {
    auto test = std::make_shared<DummyADB>();
    adbDevs[2] = test;
    test->regv[1] = 0x40;

    via1->ACR.sr_c = SR_C::OUT_EC;
    via1->dirb[5] = true;
    via1->dirb[4] = true;
    via1->adb_state[0] = true;
    via1->adb_state[1] = true;
    test->regv[2] = 0x56;
    via1->write(0, (via1->read(0) & ~(3 << 4)) | 3 << 4);
    via1->write(10, 1);
    via1->write(0, (via1->read(0) & ~(3 << 4)) | 1 << 4);
    via1->write(10, 0xFE);
    via1->write(0, (via1->read(0) & ~(3 << 4)) | 0 << 4);
    via1->write(10, 1);
    via1->write(0, (via1->read(0) & ~(3 << 4)) | 3 << 4);
    via1->ACR.sr_c = SR_C::IN_EC;
    BOOST_TEST(via1->read(13) & 1 << 2);
    uint8_t v = via1->read(10);
    BOOST_TEST(v == 1);
    via1->write(0, (via1->read(0) & ~(3 << 4)) | 1 << 4);
    BOOST_TEST(via1->read(13) & 1 << 2);
    v = via1->read(10);
    BOOST_TEST(v == 0x56);
}

BOOST_AUTO_TEST_SUITE_END()
