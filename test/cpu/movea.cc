#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(MOVEA, Prepare)
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0030100 | 3 << 9 | 2);
    BOOST_TEST(disasm() == "MOVEA.W %D2, %A3");
}
BOOST_AUTO_TEST_CASE(Decode) {
    TEST::SET_W(0, 0030100 | 3 << 9 | 2);
    cpu.D[2] = 0xFFF3;
    decode_and_run();
    BOOST_TEST(cpu.A[3] == 0xFFFFFFF3);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0020100 | 3 << 9 | 2);
    BOOST_TEST(disasm() == "MOVEA.L %D2, %A3");
}

BOOST_AUTO_TEST_CASE(Decode) {
    TEST::SET_W(0, 0020100 | 3 << 9 | 2);
    cpu.D[2] = 0xFFF3;
    decode_and_run();
    BOOST_TEST(cpu.A[3] == 0x0000FFF3);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

