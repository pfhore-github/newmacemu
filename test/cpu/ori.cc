#define BOOST_TEST_DYN_LINK
#include "inline.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_AUTO_TEST_SUITE(ORI)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0000003);
    TEST::SET_W(2, 0x0034);
    cpu.D[3] = 0x57;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.V = cpu.C = cpu.X = true;
    f();
    BOOST_TEST(cpu.D[3] == (0x57 | 0x34));
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(cpu.X);
    BOOST_TEST(i == 2);
}
BOOST_AUTO_TEST_CASE(err) {
    TEST::SET_W(0, 0000072);
    TEST::SET_W(2, 0x0034);
    TEST::SET_W(4, 0x1000);
    cpu.PC = 0;
    BOOST_CHECK_THROW(decode(), DecodeError);
}
BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0000003);
    TEST::SET_W(2, 0x0080);
    cpu.D[3] = 0x00;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0000003);
    TEST::SET_W(2, 0x0000);
    cpu.D[3] = 0x00;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0000103);
    TEST::SET_W(2, 0x1234);
    cpu.D[3] = 0x5757;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.V = cpu.C = cpu.X = true;
    f();
    BOOST_TEST(cpu.D[3] == (0x5757 | 0x1234));
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(cpu.X);
    BOOST_TEST(i == 2);
}
BOOST_AUTO_TEST_CASE(err) {
    TEST::SET_W(0, 0000172);
    TEST::SET_W(2, 0x0034);
    TEST::SET_W(4, 0x1000);
    cpu.PC = 0;
    BOOST_CHECK_THROW(decode(), DecodeError);
}
BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0000103);
    TEST::SET_W(2, 0x8000);
    cpu.D[3] = 0x00;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0000103);
    TEST::SET_W(2, 0x0000);
    cpu.D[3] = 0x00;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0000203);
    TEST::SET_L(2, 0x12345678);
    cpu.D[3] = 0x57575757;
    cpu.PC = 0;
    auto [f, i] = decode();
    cpu.V = cpu.C = cpu.X = true;
    f();
    BOOST_TEST(cpu.D[3] == (0x57575757 | 0x12345678));
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(cpu.X);
    BOOST_TEST(i == 4);
}
BOOST_AUTO_TEST_CASE(err) {
    TEST::SET_W(0, 0000272);
    TEST::SET_L(2, 0x0034);
    TEST::SET_W(6, 0x1000);
    cpu.PC = 0;
    BOOST_CHECK_THROW(decode(), DecodeError);
}
BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0000203);
    TEST::SET_L(2, 0x80000000);
    cpu.D[3] = 0x00;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0000203);
    TEST::SET_L(2, 0x00000000);
    cpu.D[3] = 0x00;
    cpu.PC = 0;
    auto [f, i] = decode();
    f();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
