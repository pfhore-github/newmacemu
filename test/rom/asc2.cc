#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"

#include "../src/librom/rom_proto.hpp"
#include "SDL3/SDL.h"
#include "bus.hpp"
#include "chip/asc.hpp"
#include "mb/glue.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <vector>
namespace bdata = boost::unit_test::data;

struct PrepareROMEASC : Prepare {
    PrepareROMEASC() { asc->isEASC = true; }
};
BOOST_FIXTURE_TEST_SUITE(e_asc, PrepareROMEASC)
BOOST_AUTO_TEST_CASE(play_sadmacBeep1) {
    io = std::make_unique<GLUE>();
    LibROM::play_sadmacBeep1();
}
#if 0
BOOST_AUTO_TEST_CASE(_45C2E) {
    io = std::make_unique<GLUE>();
    cpu.A[3] = 0x50F14000;
    cpu.A[5] = 0x50F00000;
    test_rom(0x45C2E, 0x45E3A, {});
}

BOOST_AUTO_TEST_CASE(_45C40) {
    io = std::make_unique<GLUE>();
    cpu.A[3] = 0x50F14000;
    cpu.A[5] = 0x50F00000;
    test_rom(0x45C40, 0x45E3A, {});
}

#endif
BOOST_AUTO_TEST_CASE(play_sadmacBeep4) {
    io = std::make_unique<GLUE>();
    LibROM::play_sadmacBeep4();
}

BOOST_AUTO_TEST_SUITE_END()