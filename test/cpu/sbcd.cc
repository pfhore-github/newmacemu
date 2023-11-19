#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "inline.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(SBCD, Prepare)
struct F {
    F() {
        // SBCD %D3, %D1
        TEST::SET_W(0, 0100400 | 3 << 9 | 1);
        TEST::SET_W(2, TEST_BREAK);

        // SBCD -(%A3), -(%A1)
        TEST::SET_W(4, 0100410 | 3 << 9 | 1);
        TEST::SET_W(6, TEST_BREAK);


        jit_compile(0, 8);
    }
};
BOOST_AUTO_TEST_SUITE(R, *boost::unit_test::fixture<F>())
BOOST_AUTO_TEST_SUITE(Reg)

BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
    cpu.Z = true;
    cpu.D[1] = 0x71;
    cpu.D[3] = 0x34;
    cpu.X = old_x;
    run_test(0);
    BOOST_TEST(cpu.D[1] == (0x37 - old_x));
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(CX) {
    cpu.D[1] = 0x30;
    cpu.D[3] = 0x44;
    cpu.X = false;
    run_test(0);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(Z) {
    cpu.Z = true;
    cpu.D[1] = 0x10;
    cpu.D[3] = 0x10;
    cpu.X = false;
    run_test(0);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Mem)
BOOST_DATA_TEST_CASE(value, bdata::xrange(2), old_x) {
    RAM[0x100] = 0x47;
    RAM[0x110] = 0x22;
    cpu.A[3] = 0x111;
    cpu.A[1] = 0x101;
    cpu.X = old_x;
    cpu.Z = true;
    run_test(4);
    BOOST_TEST(RAM[0x100] == 0x25 - old_x);
    BOOST_TEST(cpu.A[1] == 0x100);
    BOOST_TEST(cpu.A[3] == 0x110);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.X);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(CX) {
    RAM[0x100] = 0x30;
    RAM[0x110] = 0x44;
    cpu.A[3] = 0x111;
    cpu.A[1] = 0x101;
    cpu.X = true;
    cpu.Z = true;
    run_test(4);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(Z) {
    RAM[0x100] = 0x30;
    RAM[0x110] = 0x30;
    cpu.A[3] = 0x111;
    cpu.A[1] = 0x101;
    cpu.X = false;
    cpu.Z = true;
    run_test(4);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()