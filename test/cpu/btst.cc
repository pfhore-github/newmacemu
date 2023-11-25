#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "inline.hpp"
#include "test.hpp"

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_BTST {
    F_BTST() {
        // BTST.B #3, (%A2)
        TEST::SET_W(0, 0004020 | 2);
        TEST::SET_W(2, 3);
        TEST::SET_W(4, TEST_BREAK);

        // BTST.B #3, #8
        TEST::SET_W(6, 0004074);
        TEST::SET_W(8, 3);
        TEST::SET_W(10, 1 << 3);
        TEST::SET_W(12, TEST_BREAK);

        // BTST.B %D4, (%A2)
        TEST::SET_W(14, 0000420 | 2 | 4 << 9);
        TEST::SET_W(16, TEST_BREAK);

        // BTST.B #3, #8
        TEST::SET_W(18, 0000474 | 4 << 9);
        TEST::SET_W(20, 1 << 3);
        TEST::SET_W(22, TEST_BREAK);

        // BTST.L #20, %D2
        TEST::SET_W(24, 0004000 | 2);
        TEST::SET_W(26, 20);
        TEST::SET_W(28, TEST_BREAK);

        // BTST.L %D4, %D2
        TEST::SET_W(30, 0000400 | 2 | 4 << 9);
        TEST::SET_W(32, TEST_BREAK);

        jit_compile(0, 34);
    }};
BOOST_FIXTURE_TEST_SUITE(BTST, Prepare, *boost::unit_test::fixture<F_BTST>())


BOOST_AUTO_TEST_SUITE(Byte)

BOOST_DATA_TEST_CASE(ByImm, bdata::xrange(2), z) {
    RAM[0x100] = z << 3;
    cpu.A[2] = 0x100;
    run_test(0);
    BOOST_TEST(cpu.Z != z);
}
BOOST_AUTO_TEST_CASE(ImmByImm) {
    run_test(6);
    BOOST_TEST(!cpu.Z);
}
BOOST_AUTO_TEST_CASE(ByReg) {
    cpu.D[4] = 3;
    RAM[0x100] = 1 << 3;
    cpu.A[2] = 0x100;
    run_test(14);
    BOOST_TEST(!cpu.Z);
}
BOOST_AUTO_TEST_CASE(ImmByReg) {
    cpu.D[4] = 3;
    run_test(18);
    BOOST_TEST(!cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_DATA_TEST_CASE(ByImm, bdata::xrange(2), z) {
    cpu.D[2] = z << 20;
    run_test(24);
    BOOST_TEST(cpu.Z != z);
}

BOOST_AUTO_TEST_CASE(ByReg) {
    cpu.D[2] = 1 << 20;
    cpu.D[4] = 20;
    run_test(30);
    BOOST_TEST(!cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
