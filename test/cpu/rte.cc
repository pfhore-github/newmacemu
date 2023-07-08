#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(RTE, Prepare)
BOOST_AUTO_TEST_SUITE(format)
BOOST_DATA_TEST_CASE(type0, bdata::xrange(2), T) {
    cpu.S = true;
    cpu.A[7] = 0x1000;
    TEST::SET_W(0x1000, 0x1F | T << 14);
    TEST::SET_L(0x1002, 0x40);
    TEST::SET_W(0x1006, 0 << 12 | 3);
    TEST::SET_L(0x1008, 0x2000);

    TEST::SET_W(0, 0047163);
    cpu.A[7] = 0x1000;
    auto i = decode_and_run();
    BOOST_TEST(cpu.nextpc == 0x40);
    BOOST_TEST(i == 0);
    BOOST_TEST(cpu.must_trace == !!T);
}

BOOST_DATA_TEST_CASE(type1, bdata::xrange(2), T) {
    cpu.S = true;
    cpu.ISP = cpu.A[7] = 0x1000;
    TEST::SET_W(0x1000, 0x2000);
    TEST::SET_L(0x1002, 0x20);
    TEST::SET_W(0x1006, 1 << 12 | 3);

    TEST::SET_W(0x1008, 0x1F | T << 14);
    TEST::SET_L(0x100A, 0x40);
    TEST::SET_W(0x100E, 0 << 12 | 3);

    TEST::SET_W(0, 0047163);
    auto i = decode_and_run();
    BOOST_TEST(cpu.nextpc == 0x40);
    BOOST_TEST(i == 0);
    BOOST_TEST(cpu.must_trace == !!T);
}
BOOST_DATA_TEST_CASE(type2, bdata::xrange(2), T) {
    cpu.S = true;
    cpu.A[7] = 0x1000;
    TEST::SET_W(0x1000, 0x1F | T << 14);
    TEST::SET_L(0x1002, 0x40);
    TEST::SET_W(0x1006, 2 << 12);
    TEST::SET_L(0x1008, 0); // unused

    TEST::SET_W(0, 0047163);
    cpu.A[7] = 0x1000;
    auto i = decode_and_run();
    BOOST_TEST(cpu.nextpc == 0x40);
    BOOST_TEST(i == 0);
    BOOST_TEST(cpu.must_trace == !!T);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Occur )

BOOST_AUTO_TEST_CASE(AddressError) {
    cpu.in_exception = false;
    cpu.VBR = 0x2000;
    cpu.S = false;
    TEST::SET_L(0x2000 | (3 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x100C;
    cpu.PC = 0x100;
    if( setjmp(cpu.ex_buf) == 0) {
        ADDRESS_ERROR(0x1001);
    }
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x100);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x2000 | (3 << 2)));
    BOOST_TEST(TEST::GET_L(0x1008) == 0x1000);
}


BOOST_AUTO_TEST_CASE(ILLEGALOP) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x2000;
    TEST::SET_L(0x2000 | (3 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x1008;
    cpu.PC = 0x100;
    if( setjmp(cpu.ex_buf) == 0) {
        ILLEGAL_OP();
    }
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x100);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x0000 | (4 << 2)));
}

BOOST_AUTO_TEST_CASE(DIV0) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x2000;
    TEST::SET_L(0x2000 | (4 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x100C;
    cpu.PC = 0x100;
    cpu.nextpc = 0x102;
    if( setjmp(cpu.ex_buf) == 0) {
        DIV0_ERROR();
    }
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x102);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x2000 | (5 << 2)));
    BOOST_TEST(TEST::GET_L(0x1008) == 0x100);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()