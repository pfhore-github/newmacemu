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
BOOST_AUTO_TEST_SUITE_END()
