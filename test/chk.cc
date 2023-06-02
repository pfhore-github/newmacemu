#define BOOST_TEST_DYN_LINK
#include "inline.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_AUTO_TEST_SUITE(CHK)
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(EQ) {
    TEST::SET_W(0, 0040600 | 3 << 9 | 2);
    cpu.D[3] = 1000;
    cpu.D[2] = 2000;
    cpu.PC = 0;
    auto [f, i] = decode();
    BOOST_TEST(i == 0);
    f();
    BOOST_TEST_PASSPOINT();
}

BOOST_AUTO_TEST_CASE(LE) {
    if(setjmp(cpu.ex_buf) == 0) {
        TEST::SET_W(0, 0040600 | 3 << 9 | 2);
        cpu.D[3] = -300;
        cpu.D[2] = 2000;
        cpu.PC = 0;
        auto [f, i] = decode();
        BOOST_TEST(i == 0);
        f();
        BOOST_ERROR("exception unoccured");
    } else {
        BOOST_TEST(cpu.N);
        BOOST_TEST(cpu.ex_n == 6);
    }
}

BOOST_AUTO_TEST_CASE(GT) {
    if(setjmp(cpu.ex_buf) == 0) {
        TEST::SET_W(0, 0040600 | 3 << 9 | 2);
        cpu.D[3] = 2300;
        cpu.D[2] = 2000;
        cpu.PC = 0;
        auto [f, i] = decode();
        BOOST_TEST(i == 0);
        f();
        BOOST_ERROR("exception unoccured");
    } else {
        BOOST_TEST(!cpu.N);
        BOOST_TEST(cpu.ex_n == 6);
    }
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(EQ) {
    TEST::SET_W(0, 0040400 | 3 << 9 | 2);
    cpu.D[3] = 100000;
    cpu.D[2] = 200000;
    cpu.PC = 0;
    auto [f, i] = decode();
    BOOST_TEST(i == 0);
    f();
    BOOST_TEST_PASSPOINT();
}

BOOST_AUTO_TEST_CASE(LE) {
    if(setjmp(cpu.ex_buf) == 0) {
        TEST::SET_W(0, 0040400 | 3 << 9 | 2);
        cpu.D[3] = -30000;
        cpu.D[2] = 200000;
        cpu.PC = 0;
        auto [f, i] = decode();
        BOOST_TEST(i == 0);
        f();
        BOOST_ERROR("exception unoccured");
    } else {
        BOOST_TEST(cpu.N);
        BOOST_TEST(cpu.ex_n == 6);
    }
}

BOOST_AUTO_TEST_CASE(GT) {
    if(setjmp(cpu.ex_buf) == 0) {
        TEST::SET_W(0, 0040400 | 3 << 9 | 2);
        cpu.D[3] = 230000;
        cpu.D[2] = 200000;
        cpu.PC = 0;
        auto [f, i] = decode();
        BOOST_TEST(i == 0);
        f();
        BOOST_ERROR("exception unoccured");
    } else {
        BOOST_TEST(!cpu.N);
        BOOST_TEST(cpu.ex_n == 6);
    }
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
