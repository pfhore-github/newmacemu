#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "proto.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
void do_irq(int i);
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(STOP, Prepare)
BOOST_AUTO_TEST_CASE(user) {
    cpu.S = false;
    TEST::SET_W(0, 0047162);
    BOOST_TEST(run_test() == 8);
}
std::atomic<bool> running = true;
BOOST_AUTO_TEST_CASE(sys) {
    cpu.S = true;
    TEST::SET_W(0, 0047162);
    TEST::SET_W(2, 0xf);
    running = true;
    std::thread tx([]() {
        while(running.load()) {
            do_irq(7);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });
    tx.detach();
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 4);
    BOOST_TEST(GetCCR() == 0xf);
    cpu.inturrupt = 0;
    running = false;
}
BOOST_AUTO_TEST_SUITE_END()
