#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"

#include "SDL3/SDL.h"
#include "bus.hpp"
#include "chip/asc.hpp"
#include "chip/via.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <vector>
namespace bdata = boost::unit_test::data;
struct PrepareASCForChipTest : Prepare {
    PrepareASCForChipTest() { asc->isEASC = false; }
};
struct PrepareEASCForChipTest : Prepare {
    PrepareEASCForChipTest() { asc->isEASC = true; }
};
BOOST_FIXTURE_TEST_SUITE(asc_, PrepareASCForChipTest)
BOOST_AUTO_TEST_CASE(version) { BOOST_TEST(asc->read(0x800) == 0); }
BOOST_AUTO_TEST_SUITE(wavetable)
static void load_table(int n) {
    for(int i = 0; i < 0x200; ++i) {
        float p = 0.0;
        switch(n) {
        case 0: // sine
            p = sin(i / 256.0 * M_PI);
            break;
        case 1: // square
            p = i < 0x100 ? 1 : -1;
            break;
        case 2: // triangle
            p = (i < 0x100 ? (i - 128) : (0x150 - i)) / 128.0;
            break;
        case 3: // sawtooth
            p = (i - 256) / 128.0;
            break;
        }
        asc->write(i, p * 127.0);
        asc->write(i + 0x200, p * 127.0);
        asc->write(i + 0x400, p * 127.0);
        asc->write(i + 0x600, p * 127.0);
    }
}
BOOST_DATA_TEST_CASE(incr, bdata::xrange(4), p) {
    asc->write(0x807, 0);
    asc->write(0x801, 2);
    asc->write(0x806, 0x7f);
    load_table(p);

    asc->write(0x811, 0);
    asc->write(0x812, 0);
    asc->write(0x813, 0);
    asc->write(0x815, 1);
    asc->write(0x816, 0x40);
    asc->write(0x817, 0x00);
    asc->write(0x819, 0);
    asc->write(0x81A, 0);
    asc->write(0x81B, 0);
    asc->write(0x81D, 1);
    asc->write(0x81E, 0xC0);
    asc->write(0x81F, 0x00);
    asc->write(0x821, 0);
    asc->write(0x822, 0);
    asc->write(0x823, 0);
    asc->write(0x825, 2);
    asc->write(0x826, 0x00);
    asc->write(0x827, 0x00);
    asc->write(0x829, 0);
    asc->write(0x82A, 0);
    asc->write(0x82B, 0);
    asc->write(0x82D, 2);
    asc->write(0x82E, 0x80);
    asc->write(0x82F, 0x00);
    SDL_Delay(500);
    asc->write(0x801, 0);
    SDL_Delay(500);
}

BOOST_DATA_TEST_CASE(freq, bdata::xrange(4), p) {
    if(p == 1) {
        return;
    }
    asc->write(0x807, p);
    asc->write(0x801, 2);
    asc->write(0x806, 0x7f);
    load_table(0);

    asc->write(0x811, 0);
    asc->write(0x812, 0);
    asc->write(0x813, 0);
    asc->write(0x815, 1);
    asc->write(0x816, 0x40);
    asc->write(0x817, 0x00);
    asc->write(0x819, 0);
    asc->write(0x81A, 0);
    asc->write(0x81B, 0);
    asc->write(0x81D, 1);
    asc->write(0x81E, 0xC0);
    asc->write(0x81F, 0x00);
    asc->write(0x821, 0);
    asc->write(0x822, 0);
    asc->write(0x823, 0);
    asc->write(0x825, 2);
    asc->write(0x826, 0x00);
    asc->write(0x827, 0x00);
    asc->write(0x829, 0);
    asc->write(0x82A, 0);
    asc->write(0x82B, 0);
    asc->write(0x82D, 2);
    asc->write(0x82E, 0x80);
    asc->write(0x82F, 0x00);
    SDL_Delay(500);
    asc->write(0x801, 0);
    SDL_Delay(500);
}

BOOST_DATA_TEST_CASE(ctl, bdata::xrange(7), p) {
    asc->write(0x807, 0);
    asc->write(0x801, 2);
    asc->write(0x806, 0x7f);
    asc->write(0x805, p);
    load_table(0);

    asc->write(0x811, 0);
    asc->write(0x812, 0);
    asc->write(0x813, 0);
    asc->write(0x815, 1);
    asc->write(0x816, 0x40);
    asc->write(0x817, 0x00);
    asc->write(0x819, 0);
    asc->write(0x81A, 0);
    asc->write(0x81B, 0);
    asc->write(0x81D, 1);
    asc->write(0x81E, 0xC0);
    asc->write(0x81F, 0x00);
    asc->write(0x821, 0);
    asc->write(0x822, 0);
    asc->write(0x823, 0);
    asc->write(0x825, 2);
    asc->write(0x826, 0x00);
    asc->write(0x827, 0x00);
    asc->write(0x829, 0);
    asc->write(0x82A, 0);
    asc->write(0x82B, 0);
    asc->write(0x82D, 2);
    asc->write(0x82E, 0x80);
    asc->write(0x82F, 0x00);
    SDL_Delay(500);
    asc->write(0x801, 0);
    asc->write(0x805, 0xff);
    SDL_Delay(500);
}

BOOST_DATA_TEST_CASE(vol, bdata::xrange(4), p) {
    asc->write(0x807, 0);
    asc->write(0x801, 2);
    asc->write(0x806, p << 5);
    load_table(p);

    asc->write(0x811, 0);
    asc->write(0x812, 0);
    asc->write(0x813, 0);
    asc->write(0x815, 1);
    asc->write(0x816, 0x40);
    asc->write(0x817, 0x00);
    asc->write(0x819, 0);
    asc->write(0x81A, 0);
    asc->write(0x81B, 0);
    asc->write(0x81D, 1);
    asc->write(0x81E, 0xC0);
    asc->write(0x81F, 0x00);
    asc->write(0x821, 0);
    asc->write(0x822, 0);
    asc->write(0x823, 0);
    asc->write(0x825, 2);
    asc->write(0x826, 0x00);
    asc->write(0x827, 0x00);
    asc->write(0x829, 0);
    asc->write(0x82A, 0);
    asc->write(0x82B, 0);
    asc->write(0x82D, 2);
    asc->write(0x82E, 0x80);
    asc->write(0x82F, 0x00);
    SDL_Delay(500);
    asc->write(0x801, 0);
    asc->write(0x806, 0x7f);
    SDL_Delay(500);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_FIXTURE_TEST_SUITE(easc_, PrepareEASCForChipTest)
static void load(int n) {
    constexpr float freq = 128;
    for(int i = 0; i < 0x400; ++i) {
        float t = fmod(i / freq, 1.0);
        float p = 0.0;
        switch(n) {
        case 0: // sine
            p = sin(t * 2 * M_PI);
            break;
        case 1: // square
            p = t < 0.5 ? 1 : -1;
            break;
        case 2: // triangle
            p = (t < 0.5 ? t - .25 : .75 - t);
            break;
        case 3: // sawtooth
            p = 2 * (t - .5);
            break;
        }
        asc->write(0, p * 64.0);
        asc->write(0x400, p * 64.0);
    }
}
BOOST_AUTO_TEST_CASE(version) { BOOST_TEST(asc->read(0x800) == 0xB0); }

BOOST_DATA_TEST_CASE(play, bdata::xrange(4), p) {
    asc->write(0x801, 1);
    asc->write(0x806, 0x40);
    asc->write(0xF04, 0xac);
    asc->write(0xF05, 0x44);
    asc->write(0xF06, 0x7f);
    asc->write(0xF07, 0);
    asc->write(0xF24, 0xac);
    asc->write(0xF25, 0x44);
    asc->write(0xF26, 0);
    asc->write(0xF27, 0x7f);
    asc->write(0x803, 0x80);
    load(p);
    auto ret = asc->read(0x804);
    BOOST_TEST(ret & 2);
    BOOST_TEST(ret & 8);
    for(int i = 0; i < 20; ++i) {
        load(p);
    }
    do {
        ret = asc->read(0x804);
        SDL_Delay(0);
    } while(!(ret & 10));
    asc->write(0x801, 0);
}

BOOST_AUTO_TEST_CASE(half) {
    asc->write(0x801, 1);
    asc->write(0x806, 0x40);
    asc->write(0xF04, 0xac);
    asc->write(0xF05, 0x44);
    asc->write(0xF06, 0x7f);
    asc->write(0xF07, 0);
    asc->write(0xF24, 0xac);
    asc->write(0xF25, 0x44);
    asc->write(0xF26, 0);
    asc->write(0xF27, 0x7f);
    asc->write(0x803, 0x80);
    for(int i = 0; i < 0x400; ++i) {
        asc->write(0, 0x80);
        asc->write(0x400, 0x80);
    }

    SDL_Delay(500);
    auto ret = asc->read(0x804);
    BOOST_TEST(ret & 1);
    BOOST_TEST(ret & 4);
    asc->write(0x801, 0);
}

BOOST_DATA_TEST_CASE(stereo, bdata::xrange(2), tt) {
    asc->write(0x801, 1);
    asc->write(0x806, 0x40);
    asc->write(0xF04, 0xac);
    asc->write(0xF05, 0x44);
    if(tt) {
        asc->write(0xF06, 0x7f);
        asc->write(0xF07, 0);
        asc->write(0xF26, 0x7f);
        asc->write(0xF27, 0);
    } else {
        asc->write(0xF06, 0);
        asc->write(0xF07, 0x7f);
        asc->write(0xF26, 0);
        asc->write(0xF27, 0x7f);
    }
    asc->write(0xF24, 0xac);
    asc->write(0xF25, 0x44);
    asc->write(0x803, 0x80);
    for(int i = 0; i < 20; ++i) {
        load(0);
    }
    SDL_Delay(500);
    asc->write(0x801, 0);
}

BOOST_DATA_TEST_CASE(rate, bdata::xrange(3), r) {
    asc->write(0x801, 1);
    asc->write(0x806, 0x40);
    short rr = 44100 >> r;
    asc->write(0xF04, rr >> 8);
    asc->write(0xF05, rr);
    asc->write(0xF06, 0x7f);
    asc->write(0xF07, 0);
    asc->write(0xF24, rr >> 8);
    asc->write(0xF25, rr);
    asc->write(0xF26, 0);
    asc->write(0xF27, 0x7f);
    asc->write(0x803, 0x80);
    for(int i = 0; i < 20; ++i) {
        load(0);
    }
    SDL_Delay(500);
    asc->write(0x801, 0);
}

BOOST_AUTO_TEST_SUITE_END()
