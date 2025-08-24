#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"

#include "SDL3/SDL.h"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include "bus.hpp"
#include <deque>
void init_easc_and_play_sound(uint32_t asc, uint32_t via);
namespace bdata = boost::unit_test::data;
void run_op();
void debug_activate();
void test_rom(
    uint32_t from, uint32_t to,
    const std::unordered_map<uint32_t, std::function<void()>> &testStubs,
    bool trace) {
    from |= 0x40800000;
    to |= 0x40800000;
    cpu.S = true;
    called.clear();
    for(cpu.PC = from; cpu.PC != to;) {
        if(trace) {
            printf("%X:%08x\n", cpu.PC, cpu.D[0]);
        }
        try {
            run_op();
        } catch(M68kException& e) {
            cpu.bus_lock = false;
        }
        auto stubFound = testStubs.find(cpu.PC & ~0x40800000);
        if(stubFound != testStubs.end()) {
            stubFound->second();
        }
    }
}

std::vector<uint32_t> called;
void called_impl() {
    called.push_back(cpu.PC & ~0x40800000);
    cpu.PC = cpu.A[6];
}

bool failure_is_exception = false;
uint8_t DummyIO_B::read(uint32_t addr) {
    if(read_expected.empty() || read_expected[0].first != addr) {
        if(!failure_is_exception) {
            BOOST_TEST_FAIL("unexpected read IO (B) of " << std::hex << addr
                                                         << " at " << std::hex
                                                         << cpu.oldpc);
        } else {
            throw BusError{};
        }
        return 0;
    }
    uint8_t v = read_expected[0].second;
    read_expected.pop_front();
    return v;
}

void DummyIO_B::write(uint32_t addr, uint8_t v) {
    write_expected.emplace_back(addr, v);
}
