#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(TST, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0045003);
    BOOST_TEST(disasm() == "TST.B %D3");
}
BOOST_AUTO_TEST_CASE(vc) {
    TEST::SET_W(0, 0045000| 2);
    auto i = decode_and_run();
    BOOST_TEST(i == 0);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0045000| 2);
    cpu.D[2] = 0x80;
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0045000| 2);
    cpu.D[2] = 0;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0045103);
    BOOST_TEST(disasm() == "TST.W %D3");
}
BOOST_AUTO_TEST_CASE(vc) {
    TEST::SET_W(0, 0045100| 2);
    auto i = decode_and_run();
    BOOST_TEST(i == 0);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0045100 | 2);
    cpu.D[2] = 0x8000;
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0045100 | 2);
    cpu.D[2] = 0;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0045203);
    BOOST_TEST(disasm() == "TST.L %D3");
}

BOOST_AUTO_TEST_CASE(vc) {
    TEST::SET_W(0, 0045200| 2);
    auto i = decode_and_run();
    BOOST_TEST(i == 0);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0045200 | 2);
    cpu.D[2] = 0x80000000;
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0045200 | 2);
    cpu.D[2] = 0;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
