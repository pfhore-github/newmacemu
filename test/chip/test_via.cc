#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"

#include "SDL.h"
#include "bus.hpp"
#include "chip/via.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <vector>
namespace bdata = boost::unit_test::data;
struct DummyVIA : public VIA {
    bool inA_[8];
    bool inB_[8];
    bool outA_[8];
    bool outB_[8];
    uint8_t sr_in;
    bool readPA(int n) override { return inA_[n]; }
    bool readPB(int n) override { return inB_[n]; }
    virtual void writePA(int n, bool v) override { outA_[n] = v; }
    virtual void writePB(int n, bool v) override  { outB_[n] = v; }
    int irqNum() override { return 1; }
};

BOOST_FIXTURE_TEST_SUITE(via, Prepare)
BOOST_AUTO_TEST_SUITE(DRB) 
BOOST_AUTO_TEST_SUITE(WRITE) 
BOOST_DATA_TEST_CASE(OUTPUT, bdata::xrange(8) * bdata::xrange(2), n, tf) {
    DummyVIA via;
    via.dirb[n] = true;
    via.outB_[n] = ! tf;
    via.write(0, tf ? 0xff : 0);
    BOOST_TEST(via.outB_[n] == tf);
}
BOOST_DATA_TEST_CASE(INPUT, bdata::xrange(8) * bdata::xrange(2), n, tf) {
    DummyVIA via;
    via.dirb[n] = false;
    via.outB_[n] = ! tf;
    via.write(0, tf ? 0xff : 0);
    BOOST_TEST(via.outB_[n] == ! tf);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(READ) 
BOOST_DATA_TEST_CASE(OUTPUT, bdata::xrange(8) * bdata::xrange(2), n, tf) {
    DummyVIA via;
    via.dirb[n] = true;
    via.inB_[n] = !tf;
    via.orb[n] = tf;
    BOOST_TEST((via.read(0) >> n & 1) == tf);
}

BOOST_DATA_TEST_CASE(INPUT_NON_LATCHED, bdata::xrange(8) * bdata::xrange(2), n, tf) {
    DummyVIA via;
    via.ACR.PB_LATCH = false;
    via.dirb[n] = false;
    via.inB_[n] = tf;
    BOOST_TEST((via.read(0) >> n & 1) == tf);
}

BOOST_DATA_TEST_CASE(INPUT_LATCHED, bdata::xrange(8) * bdata::xrange(2), n, tf) {
    DummyVIA via;
    via.ACR.PB_LATCH = true;
    via.dirb[n] = false;
    via.inB_[n] = !tf;
    via.irb[n] = tf;
    BOOST_TEST((via.read(0) >> n & 1) == tf);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(DRA) 
BOOST_AUTO_TEST_SUITE(WRITE) 
BOOST_DATA_TEST_CASE(OUTPUT, bdata::xrange(8) * bdata::xrange(2), n, tf) {
    DummyVIA via;
    via.dira[n] = true;
    via.outA_[n] = ! tf;
    via.write(1, tf ? 0xff : 0);
    BOOST_TEST(via.outA_[n] == tf);
}
BOOST_DATA_TEST_CASE(INPUT, bdata::xrange(8) * bdata::xrange(2), n, tf) {
    DummyVIA via;
    via.dira[n] = false;
    via.write(1, tf ? 0xff : 0);
    BOOST_TEST(via.ora[n] == tf);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(READ) 
BOOST_DATA_TEST_CASE(NON_LATCHED, bdata::xrange(8) * bdata::xrange(2), n, tf) {
    DummyVIA via;
    via.ACR.PA_LATCH = false;
    via.inA_[n] = tf;
    BOOST_TEST((via.read(1) >> n & 1) == tf);
}

BOOST_DATA_TEST_CASE(LATCHED, bdata::xrange(8) * bdata::xrange(2), n, tf) {
    DummyVIA via;
    via.ACR.PA_LATCH = true;
    via.ira[n] = tf;
    BOOST_TEST((via.read(1) >> n & 1) == tf);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(DDRB) 
BOOST_DATA_TEST_CASE(WRITE, bdata::xrange(8) * bdata::xrange(2), n, tf) {
    DummyVIA via;
    via.dirb[n] = ! tf;
    via.write(2, tf ? 0xff : 0);
    BOOST_TEST(via.dirb[n] == tf);
}
BOOST_DATA_TEST_CASE(TRANSMIT, bdata::xrange(8) * bdata::xrange(2), n, tf) {
    DummyVIA via;
    via.dirb[n] = false;
    via.orb[n] = tf;
    via.write(2, 0xff);
    BOOST_TEST(via.outB_[n] == tf);
}
BOOST_DATA_TEST_CASE(READ, bdata::xrange(8) * bdata::xrange(2), n, tf) {
    DummyVIA via;
    via.dirb[n] = tf;
    BOOST_TEST((via.read(2) >> n & 1) == tf);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(DDRA) 
BOOST_DATA_TEST_CASE(WRITE, bdata::xrange(8) * bdata::xrange(2), n, tf) {
    DummyVIA via;
    via.dira[n] = ! tf;
    via.write(3, tf ? 0xff : 0);
    BOOST_TEST(via.dira[n] == tf);
}
BOOST_DATA_TEST_CASE(TRANSMIT, bdata::xrange(8) * bdata::xrange(2), n, tf) {
    DummyVIA via;
    via.dira[n] = false;
    via.ora[n] = tf;
    via.write(3, 0xff);
    BOOST_TEST(via.outA_[n] == tf);
}
BOOST_DATA_TEST_CASE(READ, bdata::xrange(8) * bdata::xrange(2), n, tf) {
    DummyVIA via;
    via.dira[n] = tf;
    BOOST_TEST((via.read(3) >> n & 1) == tf);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(TIMER1) 
BOOST_AUTO_TEST_CASE(LATCH) {
    DummyVIA via;
    via.write(4, 0x34);
    via.write(5, 0x56);
    BOOST_TEST(via.read(6) == 0x34);
    BOOST_TEST(via.read(7) == 0x56);
}

BOOST_AUTO_TEST_CASE(LATCH2) {
    DummyVIA via;
    via.write(7, 0x56);
    via.write(6, 0x34);
    BOOST_TEST(via.read(6) == 0x34);
    BOOST_TEST(via.read(7) == 0x56);
}

BOOST_AUTO_TEST_CASE(COUNT) {
    DummyVIA via;
    via.write(4, 0xf0);
    via.write(5, 0x00);
    SDL_Delay(20);
    uint16_t v = via.read(4) | via.read(5) << 8;
    BOOST_TEST(v < 0xf000);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(TIMER2) {
    DummyVIA via;
    via.write(8, 0xf0);
    via.write(9, 0x00);
    SDL_Delay(20);
    uint16_t v = via.read(8) | via.read(9) << 8;
    BOOST_TEST(v < 0xf000);
}
BOOST_AUTO_TEST_SUITE(SR) 
BOOST_AUTO_TEST_SUITE(read) 
BOOST_AUTO_TEST_CASE(disabled) {
    DummyVIA via;
    via.ACR.sr_c = SR_C::DISABLED;
    via.sr_in = 0x12;
    via.sr = 0x10;
    BOOST_TEST(via.read(10) == 0x10);
}
BOOST_AUTO_TEST_CASE(enabled) {
    DummyVIA via;
    via.ACR.sr_c = SR_C::IN_EC;
    via.sr = 0x10;
    BOOST_TEST(via.read(10) == 0x10);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(write) 
BOOST_AUTO_TEST_CASE(disabled) {
    DummyVIA via;
    via.ACR.sr_c = SR_C::DISABLED;
    via.sr = 0x10;
    via.write(10, 0x12);
    BOOST_TEST(via.sr == 0x10);
}
BOOST_AUTO_TEST_CASE(enabled) {
    DummyVIA via;
    via.ACR.sr_c = SR_C::OUT_EC;
    via.sr = 0x10;
    via.write(10, 0x12);
    BOOST_TEST(via.sr == 0x12);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(ACR) 
BOOST_AUTO_TEST_CASE(pb7) {
    DummyVIA via;
    via.write(11, 0x80);
    via.write(4, 0x00);
    via.write(5, 0x80);
    SDL_Delay(100);
    BOOST_TEST(via.outB_[7]);
}

BOOST_AUTO_TEST_CASE(t1_rep) {
    DummyVIA via;
    via.write(11, 0xC0);
    via.write(4, 0x80);
    via.write(5, 0x00);
    SDL_Delay(100);
    BOOST_TEST(via.outB_[7]);
    via.outB_[7] = false;
    SDL_Delay(100);
    BOOST_TEST(via.outB_[7]);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(IFR) 
BOOST_AUTO_TEST_CASE(T1) {
    DummyVIA via;
    via.write(11, 0);
    via.write(4, 0);
    via.write(5, 0x80);
    SDL_Delay(100);
    BOOST_TEST(via.read(13) & 0x80);
    BOOST_TEST(via.read(13) & 0x40);
    via.read(4);
    BOOST_TEST(!(via.read(13) & 0x80));
    BOOST_TEST(!(via.read(13) & 0x40));
}


BOOST_AUTO_TEST_CASE(T2) {
    DummyVIA via;
    via.write(11, 0);
    via.write(8, 0);
    via.write(9, 0x80);
    SDL_Delay(100);
    BOOST_TEST(via.read(13) & 0x80);
    BOOST_TEST(via.read(13) & 0x20);
    via.read(8);
    BOOST_TEST(!(via.read(13) & 0x80));
    BOOST_TEST(!(via.read(13) & 0x20));
}

BOOST_AUTO_TEST_CASE(CB1) {
    DummyVIA via;
    via.irq(VIA_IRQ::CB1);
    BOOST_TEST(via.read(13) & 0x80);
    BOOST_TEST(via.read(13) & 0x10);
    via.read(0);
    BOOST_TEST(!(via.read(13) & 0x80));
    BOOST_TEST(!(via.read(13) & 0x10));
}

BOOST_AUTO_TEST_CASE(CB2) {
    DummyVIA via;
    via.irq(VIA_IRQ::CB2);
    BOOST_TEST(via.read(13) & 0x80);
    BOOST_TEST(via.read(13) & 0x08);
    via.read(0);
    BOOST_TEST(!(via.read(13) & 0x80));
    BOOST_TEST(!(via.read(13) & 0x08));
}

BOOST_AUTO_TEST_CASE(SR) {
    DummyVIA via;
    via.irq(VIA_IRQ::SR);
    BOOST_TEST(via.read(13) & 0x80);
    BOOST_TEST(via.read(13) & 0x04);
    via.read(10);
    BOOST_TEST(!(via.read(13) & 0x80));
    BOOST_TEST(!(via.read(13) & 0x04));
}

BOOST_AUTO_TEST_CASE(CA1) {
    DummyVIA via;
    via.irq(VIA_IRQ::CA1);
    BOOST_TEST(via.read(13) & 0x80);
    BOOST_TEST(via.read(13) & 0x02);
    via.read(1);
    BOOST_TEST(!(via.read(13) & 0x80));
    BOOST_TEST(!(via.read(13) & 0x02));
}

BOOST_AUTO_TEST_CASE(CA2) {
    DummyVIA via;
    via.irq(VIA_IRQ::CA2);
    BOOST_TEST(via.read(13) & 0x80);
    BOOST_TEST(via.read(13) & 0x01);
    via.read(1);
    BOOST_TEST(!(via.read(13) & 0x80));
    BOOST_TEST(!(via.read(13) & 0x01));
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(IER) 
BOOST_DATA_TEST_CASE(SET, bdata::xrange(7), b) {
    DummyVIA via;
    via.write(14, 0x80 | 1 << b);
    BOOST_TEST(via.read(14) & 0x80);
    BOOST_TEST(via.read(14) & 1 << b);
}


BOOST_DATA_TEST_CASE(CLEAR, bdata::xrange(7), b) {
    DummyVIA via;
    via.write(14, 0xff);
    via.write(14, 1 << b);
    BOOST_TEST(via.read(14) & 0x80);
    BOOST_TEST(!(via.read(14) & 1 << b));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(IRQ) {
    DummyVIA via;
    via.write(14, 0xff);
    via.irq(VIA_IRQ::CA2);
    BOOST_TEST( cpu.inturrupt.load() );
}
BOOST_AUTO_TEST_SUITE_END()