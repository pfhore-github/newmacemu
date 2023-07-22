#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(SUBI, Prepare)
// value & flags are tested at SUB
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0002003);
    TEST::SET_W(2, 23);
    BOOST_TEST(disasm() == "SUBI.B #23, %D3");
}
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0002000 | 2);
    TEST::SET_W(2, 14);
    cpu.D[2] = 21;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 7);
    BOOST_TEST(i == 2);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0002103);
    TEST::SET_W(2, 2300);
    BOOST_TEST(disasm() == "SUBI.W #2300, %D3");
}

BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0002100 | 2);
    TEST::SET_W(2, 213);
    cpu.D[2] = 3456;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 3243);
    BOOST_TEST(i == 2);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0002203);
    TEST::SET_L(2, 230000);
    BOOST_TEST(disasm() == "SUBI.L #230000, %D3");
}

BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0002200 | 2);
    TEST::SET_L(2, 44444);
    cpu.D[2] = 123456789;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 123412345);
    BOOST_TEST(i == 4);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
