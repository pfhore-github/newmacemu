#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(LINK, Prepare)
BOOST_AUTO_TEST_SUITE(Word) 
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0047124);
    TEST::SET_W(2, 2000);
    BOOST_TEST(disasm() == "LINK.W %A4, #2000");
}
BOOST_AUTO_TEST_CASE(run) {
    TEST::SET_W(0, 0047120 | 2 );
    TEST::SET_W(2, 0x40 );
    cpu.A[2] = 0x1100;
    cpu.A[7] = 0x1004;
    auto i = decode_and_run();
    BOOST_TEST(TEST::GET_L(0x1000) == 0x1100);
    BOOST_TEST(cpu.A[2] == 0x1000);
    BOOST_TEST(cpu.A[7] == 0x1040);
    BOOST_TEST(i == 2);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long) 
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0044012);
    TEST::SET_L(2, 2000);
    BOOST_TEST(disasm() == "LINK.L %A2, #2000");
}
BOOST_AUTO_TEST_CASE(run) {
    TEST::SET_W(0, 0044010 | 2 );
    TEST::SET_L(2, 0x40 );
    cpu.A[2] = 0x1100;
    cpu.A[7] = 0x1004;
    auto i = decode_and_run();
    BOOST_TEST(TEST::GET_L(0x1000) == 0x1100);
    BOOST_TEST(cpu.A[2] == 0x1000);
    BOOST_TEST(cpu.A[7] == 0x1040);
    BOOST_TEST(i == 4);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()