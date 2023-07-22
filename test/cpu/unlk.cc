#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(UNLK, Prepare)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0047134);
    BOOST_TEST(disasm() == "UNLK %A4");
}
BOOST_AUTO_TEST_CASE(UNLK) {
 TEST::SET_W(0, 0047130 | 2 );
    TEST::SET_W(2, 0x40 );
    cpu.A[2] = 0x1000;
    cpu.A[7] = 0x2000;
    TEST::SET_L(0x1000, 0x2500);
    auto i = decode_and_run();
    BOOST_TEST(cpu.A[2] == 0x2500);
    BOOST_TEST(cpu.A[7] == 0x1004);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_SUITE_END()