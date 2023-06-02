#define BOOST_TEST_DYN_LINK
#include "inline.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_AUTO_TEST_SUITE(BCLR)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(ByImmErr) {
    TEST::SET_W(0, 0004200|  072 );
    cpu.PC = 0;
    BOOST_CHECK_THROW(decode(), DecodeError);
}
BOOST_AUTO_TEST_CASE(ByImm) {
    TEST::SET_W(0, 0004220 | 2);
    TEST::SET_W(2, 3);
    RAM[0x1000] = 0xff;
    cpu.A[2] = 0x1000;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(i == 2);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(RAM[0x1000] == 0xF7);
}
BOOST_AUTO_TEST_CASE(ByRegerr) {
    TEST::SET_W(0, 0000600|  072 );
    cpu.PC = 0;
    BOOST_CHECK_THROW(decode(), DecodeError);
}
BOOST_AUTO_TEST_CASE(ByReg) {
    TEST::SET_W(0, 0000620 | 2 | 4 << 9);
    cpu.D[4] = 3;
    RAM[0x1000] = 0xff;
    cpu.A[2] = 0x1000;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(i == 0);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(RAM[0x1000] == 0xF7);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0000620 | 2 | 4 << 9);
    cpu.D[4] = 3;
    RAM[0x1000] = 0;
    cpu.A[2] = 0x1000;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.Z);
    BOOST_TEST(RAM[0x1000] == 0);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(ByImm) {
    TEST::SET_W(0, 0004200 | 2);
    TEST::SET_W(2, 20);
    cpu.D[2] = 0xffffffff;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(i == 2);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(cpu.D[2] == 0xFFEFFFFF);
}
BOOST_AUTO_TEST_CASE(ByReg) {
    TEST::SET_W(0, 0000600 | 2 | 4 << 9);
    cpu.D[2] = 0xffffffff;
    cpu.D[4] = 20;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(i == 0);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(cpu.D[2] == 0xFFEFFFFF);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0000600 | 2 | 4 << 9);
    cpu.D[2] = 0;
    cpu.D[4] = 20;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.Z);
    BOOST_TEST(cpu.D[2]  == 0);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()