#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(ADDI, Prepare)
// value & flags are tested at ADD
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0003003);
    TEST::SET_W(2, 23);
    BOOST_TEST(disasm() == "ADDI.B #23, %D3");
}
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0003000 | 2);
    TEST::SET_W(2, 14);
    cpu.D[2] = 21;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 35);
    BOOST_TEST(i == 2);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0003103);
    TEST::SET_W(2, 2300);
    BOOST_TEST(disasm() == "ADDI.W #2300, %D3");
}
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0003100 | 2);
    TEST::SET_W(2, 1420);
    cpu.D[2] = 2133;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 3553);
    BOOST_TEST(i == 2);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0003203);
    TEST::SET_L(2, 230000);
    BOOST_TEST(disasm() == "ADDI.L #230000, %D3");
}

BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0003200 | 2);
    TEST::SET_L(2, 142044);
    cpu.D[2] = 213324;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 355368);
    BOOST_TEST(i == 4);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
