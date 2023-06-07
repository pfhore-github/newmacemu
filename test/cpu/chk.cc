#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(CHK, Prepare)
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(In) {
    TEST::SET_W(0, 0040600 | 3 << 9 | 2);
    cpu.D[3] = 1000;
    cpu.D[2] = 2000;
    auto [f, i] = decode();
    BOOST_TEST(i == 0);
    f();
    BOOST_TEST_PASSPOINT();
}

BOOST_AUTO_TEST_CASE(Under) {
    if(setjmp(cpu.ex_buf) == 0) {
        TEST::SET_W(0, 0040600 | 3 << 9 | 2);
        cpu.D[3] = -300;
        cpu.D[2] = 2000;
        auto [f, i] = decode();
        BOOST_TEST(i == 0);
        f();
        BOOST_ERROR("exception unoccured");
    } else {
        BOOST_TEST(cpu.N);
        BOOST_TEST(cpu.ex_n == 6);
    }
}

BOOST_AUTO_TEST_CASE(Over) {
    if(setjmp(cpu.ex_buf) == 0) {
        TEST::SET_W(0, 0040600 | 3 << 9 | 2);
        cpu.D[3] = 2300;
        cpu.D[2] = 2000;
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
BOOST_AUTO_TEST_CASE(In) {
    TEST::SET_W(0, 0040400 | 3 << 9 | 2);
    cpu.D[3] = 100000;
    cpu.D[2] = 200000;
    auto i = decode_and_run();
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(Under) {
    if(setjmp(cpu.ex_buf) == 0) {
        TEST::SET_W(0, 0040400 | 3 << 9 | 2);
        cpu.D[3] = -30000;
        cpu.D[2] = 200000;
        auto [f, i] = decode();
        BOOST_TEST(i == 0);
        f();
        BOOST_ERROR("exception unoccured");
    } else {
        BOOST_TEST(cpu.N);
        BOOST_TEST(cpu.ex_n == 6);
    }
}

BOOST_AUTO_TEST_CASE(Over) {
    if(setjmp(cpu.ex_buf) == 0) {
        TEST::SET_W(0, 0040400 | 3 << 9 | 2);
        cpu.D[3] = 230000;
        cpu.D[2] = 200000;
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
