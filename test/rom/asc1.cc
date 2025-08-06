#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include "../src/librom/rom_proto.hpp"
#include "SDL3/SDL.h"
#include "bus.hpp"
#include "mb/glue.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <vector>
namespace bdata = boost::unit_test::data;
struct PrepareROMASC : Prepare {
    PrepareROMASC() {
        asc->isEASC = false;
    }
};
BOOST_FIXTURE_TEST_SUITE(test_asc, PrepareROMASC)
BOOST_AUTO_TEST_CASE(play_asc_chime1) {
    io = std::make_unique<GLUE>();
    LibROM::play_asc_chime1(); 
}
#if 0
BOOST_AUTO_TEST_CASE(_705E) {
    cpu.A[3] = 0x50F14000;
    cpu.A[5] = 0x50F00000;
    io = std::make_unique<GLUE>();
    test_rom(0x705E, 0x7156, {});
}

BOOST_AUTO_TEST_CASE(_7064) {
    cpu.A[3] = 0x50F14000;
    cpu.A[5] = 0x50F00000;
    io = std::make_unique<GLUE>();
    test_rom(0x7064, 0x7156, {});
}

#endif
BOOST_AUTO_TEST_CASE(play_asc_chime4) {
    io = std::make_unique<GLUE>();
    LibROM::play_asc_chime4(); 
}
BOOST_AUTO_TEST_SUITE_END()