#define BOOST_TEST_DYN_LINK
#include "68040.hpp"

#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_MOVEP {
    F_MOVEP() {
        // MOVEP.W %D3, (%A2, 0x10)
        TEST::SET_W(0, 0000610 | 3 << 9 | 2);
        TEST::SET_W(2, 0x10);
        TEST::SET_W(4, TEST_BREAK);

        // MOVEP.L %D3, (%A2, 0x10)
        TEST::SET_W(6, 0000710 | 3 << 9 | 2);
        TEST::SET_W(8, 0x10);
        TEST::SET_W(10, TEST_BREAK);

        // MOVEP.W (%A2, 0x10), %D3
        TEST::SET_W(12, 0000410 | 3 << 9 | 2);
        TEST::SET_W(14, 0x10);
        TEST::SET_W(16, TEST_BREAK);

        // MOVEP.L (%A2, 0x10), %D3
        TEST::SET_W(18, 0000510 | 3 << 9 | 2);
        TEST::SET_W(20, 0x10);
        TEST::SET_W(22, TEST_BREAK);

        jit_compile(0, 24);
    }
};
BOOST_FIXTURE_TEST_SUITE(MOVEP, Prepare, *boost::unit_test::fixture<F_MOVEP>())

BOOST_AUTO_TEST_SUITE(Store)

BOOST_AUTO_TEST_CASE(Word) {
    cpu.D[3] = 0x1234;
    cpu.A[2] = 0x1000;
    run_test(0);
    BOOST_TEST(RAM[0x1010] == 0x12);
    BOOST_TEST(RAM[0x1012] == 0x34);
}

BOOST_AUTO_TEST_CASE(Long) {
    cpu.D[3] = 0x12345678;
    cpu.A[2] = 0x1000;
    run_test(6);
    BOOST_TEST(RAM[0x1010] == 0x12);
    BOOST_TEST(RAM[0x1012] == 0x34);
    BOOST_TEST(RAM[0x1014] == 0x56);
    BOOST_TEST(RAM[0x1016] == 0x78);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Load)

BOOST_AUTO_TEST_CASE(Word) {
    cpu.A[2] = 0x1000;
    TEST::SET_L(0x1010, 0x12345678);
    run_test(12);
    BOOST_TEST(cpu.D[3] == 0x1256);
}

BOOST_AUTO_TEST_CASE(Long) {
    cpu.D[3] = 0x12345678;
    cpu.A[2] = 0x1000;
    TEST::SET_L(0x1010, 0x12345678);
    TEST::SET_L(0x1014, 0x9ABCDEF0);
    run_test(18);
    BOOST_TEST(cpu.D[3] == 0x12569ADE);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
