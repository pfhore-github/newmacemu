#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "inline.hpp"

#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <thread>
void do_irq(int i);
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(STOP, Prepare)
struct F {
    F() {
        // STOP
        TEST::SET_W(0, 0047162);
        TEST::SET_W(2, 0xf);
        TEST::SET_W(4, TEST_BREAK);

        jit_compile(0, 6);
    }
};
BOOST_AUTO_TEST_SUITE(R, *boost::unit_test::fixture<F>())
BOOST_AUTO_TEST_CASE(user) {
    cpu.S = false;	
	run_test(0);
	BOOST_TEST( cpu.ex_n == 8 );
}
std::atomic<bool> running = true;
BOOST_AUTO_TEST_CASE(sys) {
    cpu.S = true;
    running = true;
    std::thread tx([]() {
        while(running.load()) {
            do_irq(7);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });
    tx.detach();
	run_test(0);
    BOOST_TEST(GetCCR() == 0xf);
    cpu.inturrupt = 0;
    running = false;
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
