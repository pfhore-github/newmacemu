#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(CLR, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0041003);
    BOOST_TEST(disasm() == "CLR.B %D3");
}
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0041000 | 2 );
    cpu.D[2] = 0x12345678;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x12345600);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0041103);
    BOOST_TEST(disasm() == "CLR.W %D3");
}
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0041100 | 2 );
    cpu.D[2] = 0x12345678;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x12340000);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 0);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0041203);
    BOOST_TEST(disasm() == "CLR.L %D3");
}
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0041200 | 2 );
    cpu.D[2] = 0x12345678;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
