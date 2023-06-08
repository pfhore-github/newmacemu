#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(DIVU, Prepare)
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0100300 | 3 << 9 | 2);
    cpu.D[3] = 200000;
    cpu.D[2] = 119;
    int i = decode_and_run();
    BOOST_TEST(cpu.D[3] == (80 << 16 | 1680));
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0100300 | 3 << 9 | 2);
    cpu.D[3] = 0x80000000;
    cpu.D[2] = 1;
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0100300 | 3 << 9 | 2);
    cpu.D[3] = 0;
    cpu.D[2] = 3;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}



BOOST_AUTO_TEST_CASE(Div0) {
    if(setjmp(cpu.ex_buf) == 0) {
        TEST::SET_W(0, 0100300 | 3 << 9 | 2);
        cpu.D[3] = 3;
        cpu.D[2] = 0;
        decode_and_run();
        BOOST_ERROR("exception unoccured");
    } else {
        BOOST_TEST(cpu.ex_n == 5);
    }
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0046100 | 2);
    TEST::SET_W(2, 0000000 | 3 << 12 | 4);
    cpu.D[3] = 200000;
    cpu.D[2] = 7;
    int i = decode_and_run();
    BOOST_TEST(cpu.D[3] == 28571);
    BOOST_TEST(cpu.D[4] == 3);
    BOOST_TEST(i == 2);
}

BOOST_AUTO_TEST_CASE(nomod) {
    TEST::SET_W(0, 0046100 | 2);
    TEST::SET_W(2, 0000000 | 3 << 12 | 3);
    cpu.D[3] = 200000;
    cpu.D[2] = 7;
    decode_and_run();
    BOOST_TEST(cpu.D[3] == 28571);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0046100 | 2);
    TEST::SET_W(2, 0000000 | 3 << 12 | 4);
    cpu.D[3] = 0x80000000;
    cpu.D[2] = 1;
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0046100 | 2);
    TEST::SET_W(2, 0000000 | 3 << 12 | 4);
    cpu.D[3] = 0;
    cpu.D[2] = 7;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(Div0) {
    if(setjmp(cpu.ex_buf) == 0) {
        TEST::SET_W(0, 0046100 | 2);
        TEST::SET_W(2, 0000000 | 3 << 12 | 4);
        cpu.D[3] = 4;
        cpu.D[2] = 0;
        decode_and_run();
        BOOST_ERROR("exception unoccured");
    } else {
        BOOST_TEST(cpu.ex_n == 5);
    }
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Quad)
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0046100 | 2);
    TEST::SET_W(2, 0002000 | 3 << 12 | 4);
    cpu.D[3] = 0;
    cpu.D[4] = 1;
    cpu.D[2] = 3;
    int i = decode_and_run();
    BOOST_TEST(cpu.D[3] == 1431655765);
    BOOST_TEST(cpu.D[4] == 1);
    BOOST_TEST(i == 2);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0046100 | 2);
    TEST::SET_W(2, 0002000 | 3 << 12 | 4);
    cpu.D[3] = 0;
    cpu.D[4] = 1;
    cpu.D[2] = 2;
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0046100 | 2);
    TEST::SET_W(2, 0002000 | 3 << 12 | 4);
    cpu.D[3] = 0;
    cpu.D[4] = 0;
    cpu.D[2] = 7;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(V) {
    TEST::SET_W(0, 0046100 | 2);
    TEST::SET_W(2, 0002000 | 3 << 12 | 4);
    cpu.D[3] = 0;
    cpu.D[4] = 4;
    cpu.D[2] = 2;
    decode_and_run();
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_CASE(Div0) {
    if(setjmp(cpu.ex_buf) == 0) {
        TEST::SET_W(0, 0046100 | 2);
        TEST::SET_W(2, 0002000 | 3 << 12 | 4);
        cpu.D[3] = 1;
        cpu.D[4] = 0;
        cpu.D[2] = 0;
        decode_and_run();
        BOOST_ERROR("exception unoccured");
    } else {
        BOOST_TEST(cpu.ex_n == 5);
    }
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
