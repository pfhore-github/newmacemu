#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "inline.hpp"
#include "test.hpp"

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_BSET {
    F_BSET() {
        // BSET.B #3, (%A2)
        TEST::SET_W(0, 0004320 | 2);
        TEST::SET_W(2, 3);
        TEST::SET_W(4, TEST_BREAK);

        // BSET.B %D4, (%A2)
        TEST::SET_W(6, 0000720 | 2 | 4 << 9);
        TEST::SET_W(8, TEST_BREAK);

        // BSET.L #20, %D2
        TEST::SET_W(10, 0004300 | 2);
        TEST::SET_W(12, 20);
        TEST::SET_W(14, TEST_BREAK);

        // BSET.L %D4, %D2
        TEST::SET_W(16, 0000700 | 2 | 4 << 9);
        TEST::SET_W(18, TEST_BREAK);

        jit_compile(0, 20);
    }
};
BOOST_FIXTURE_TEST_SUITE(BSET, Prepare, *boost::unit_test::fixture<F_BSET>())

BOOST_AUTO_TEST_SUITE(Byte)
BOOST_DATA_TEST_CASE(ByImm, bdata::xrange(2), z) {
    RAM[0x100] = !z << 3;
    cpu.A[2] = 0x100;
    run_test(0);
    BOOST_TEST(cpu.Z == z);
    BOOST_TEST(RAM[0x100] == 1 << 3);
}

BOOST_AUTO_TEST_CASE(ByReg) {
    cpu.D[4] = 3;
    RAM[0x100] = 0;
    cpu.A[2] = 0x100;
    run_test(6);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(RAM[0x100] == 8);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)

BOOST_DATA_TEST_CASE(ByImm, bdata::xrange(2), z) {
    cpu.D[2] = !z << 20;
    run_test(10);
    BOOST_TEST(cpu.Z == z);
    BOOST_TEST(cpu.D[2] == 1 << 20);
}

BOOST_AUTO_TEST_CASE(ByReg) {
    cpu.D[2] = 0xFFEFFFFF;
    cpu.D[4] = 20;
    run_test(16);
    BOOST_TEST(cpu.Z);
    BOOST_TEST(cpu.D[2] == 0xFFFFFFFF);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
