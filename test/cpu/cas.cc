#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(CAS, Prepare)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0005320 | 2);
    TEST::SET_W(2, 3 << 6 | 4);
    BOOST_TEST(disasm() == "CAS.B %D4, %D3, (%A2)");
}

BOOST_DATA_TEST_CASE(traced, bdata::xrange(2), tr) {
    TEST::SET_W(0, 0005320 | 2);
    TEST::SET_W(2, 3 << 6 | 4);
    cpu.T = tr;
    auto i = decode_and_run();
    BOOST_TEST(cpu.must_trace == !!tr);
    BOOST_TEST(i == 2);
}

BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0005320 | 2);
    TEST::SET_W(2, 3 << 6 | 4);
    RAM[0x1000] = 0x22;
    cpu.A[2] = 0x1000;
    cpu.D[3] = 0x30;
    cpu.D[4] = 0x22;
    decode_and_run();
    BOOST_TEST(RAM[0x1000] == 0x30);
}

BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0005320 | 2);
    TEST::SET_W(2, 3 << 6 | 4);
    RAM[0x1000] = 0x22;
    cpu.A[2] = 0x1000;
    cpu.D[3] = 0x30;
    cpu.D[4] = 0x35;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0x22);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0006320 | 2);
    TEST::SET_W(2, 3 << 6 | 4);
    BOOST_TEST(disasm() == "CAS.W %D4, %D3, (%A2)");
}

BOOST_DATA_TEST_CASE(traced, bdata::xrange(2), tr) {
    TEST::SET_W(0, 0006320 | 2);
    TEST::SET_W(2, 3 << 6 | 4);
    cpu.T = tr;
    auto i = decode_and_run();
    BOOST_TEST(cpu.must_trace == !!tr);
    BOOST_TEST(i == 2);
}
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0006320 | 2);
    TEST::SET_W(2, 3 << 6 | 4);
    TEST::SET_W(0x1000, 0x2222);
    cpu.A[2] = 0x1000;
    cpu.D[3] = 0x3030;
    cpu.D[4] = 0x2222;
    decode_and_run();
    BOOST_TEST(TEST::GET_W(0x1000) == 0x3030);
}

BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0006320 | 2);
    TEST::SET_W(2, 3 << 6 | 4);
    TEST::SET_W(0x1000, 0x2222);
    cpu.A[2] = 0x1000;
    cpu.D[3] = 0x3030;
    cpu.D[4] = 0x3535;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0x2222);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0007320 | 2);
    TEST::SET_W(2, 3 << 6 | 4);
    BOOST_TEST(disasm() == "CAS.L %D4, %D3, (%A2)");
}


BOOST_DATA_TEST_CASE(traced, bdata::xrange(2), tr) {
    TEST::SET_W(0, 0007320 | 2);
    TEST::SET_W(2, 3 << 6 | 4);
    cpu.T = tr;
    decode_and_run();
    BOOST_TEST(cpu.must_trace == !!tr);
}
BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0, 0007320 | 2);
    TEST::SET_W(2, 3 << 6 | 4);
    TEST::SET_L(0x1000, 0x22222222);
    cpu.A[2] = 0x1000;
    cpu.D[3] = 0x30303030;
    cpu.D[4] = 0x35353535;
    decode_and_run();
    BOOST_TEST(cpu.D[4] == 0x22222222);
}

BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0, 0007320 | 2);
    TEST::SET_W(2, 3 << 6 | 4);
    TEST::SET_L(0x1000, 0x22222222);
    cpu.A[2] = 0x1000;
    cpu.D[3] = 0x30303030;
    cpu.D[4] = 0x22222222;
    decode_and_run();
    BOOST_TEST(TEST::GET_L(0x1000) == 0x30303030);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
