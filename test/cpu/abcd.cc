#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "inline.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(ABCD, Prepare)
struct F {
    F() {
        // ABCD %D1, %D3
        TEST::SET_W(0, 0140400 | 3 << 9 | 1);
        TEST::SET_W(2, TEST_BREAK);

        // ABCD -(%A1), -(%A3)
        TEST::SET_W(4, 0140410 | 3 << 9 | 1);
        TEST::SET_W(6, TEST_BREAK);

        jit_compile(0, 8);
    }
};
BOOST_AUTO_TEST_SUITE(R, *boost::unit_test::fixture<F>())
BOOST_AUTO_TEST_SUITE(Reg)
BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
    cpu.Z = true;
    cpu.X = old_x;
    cpu.D[3] = 0x22;
    cpu.D[1] = 0x34;
    run_test(0);
    BOOST_TEST(cpu.D[3] == 0x56 + old_x);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(CX) {
    cpu.X = false;
    cpu.D[3] = 0x77;
    cpu.D[1] = 0x44;
    run_test(0);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.Z = true;
    cpu.X = false;
    cpu.D[3] = 0;
    cpu.D[1] = 0;
    run_test(0);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Mem)
BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
    cpu.X = old_x;
    cpu.Z = true;
    RAM[0x100] = 0x22;
    RAM[0x110] = 0x34;
    cpu.A[3] = 0x101;
    cpu.A[1] = 0x111;
    run_test(4);
    BOOST_TEST(RAM[0x100] == 0x56 + old_x);
    BOOST_TEST(cpu.A[3] == 0x100);
    BOOST_TEST(cpu.A[1] == 0x110);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(CX) {
    cpu.X = false;
    RAM[0x100] = 0x77;
    RAM[0x110] = 0x44;
    cpu.A[3] = 0x101;
    cpu.A[1] = 0x111;
    run_test(4);
    BOOST_TEST(cpu.X);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.X = false;
    cpu.Z = true;
    RAM[0x100] = 0;
    RAM[0x110] = 0;
    cpu.A[3] = 0x101;
    cpu.A[1] = 0x111;
    run_test(4);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
