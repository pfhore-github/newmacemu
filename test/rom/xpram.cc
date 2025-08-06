#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"

#include "SDL3/SDL.h"
#include "chip/iifx.hpp"
#include "chip/iop.hpp"
#include "chip/rbv.hpp"
#include "chip/scsi.hpp"
#include "chip/vdac.hpp"
#include "chip/via.hpp"
#include "mb/jaws.hpp"
#include "mb/mcu.hpp"

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <vector>
struct PrepareROM : Prepare {
    PrepareROM() { failure_is_exception = false; }
};
BOOST_FIXTURE_TEST_SUITE(xpram, PrepareROM)
BOOST_AUTO_TEST_SUITE(_4721C)
static void xpram_read() {
    static uint8_t vv[] = {0x12, 0x34, 0x56, 0x78};
    cpu.D[1] = vv[cpu.D[1] & 3];
    cpu.PC = cpu.A[6];
}
BOOST_AUTO_TEST_CASE(run) {
    test_rom(0x4721C, 0x47238, {{0x4B258, xpram_read}});
    BOOST_TEST(cpu.D[4] == 0x12345678);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()