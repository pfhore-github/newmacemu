#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"

#include "inline.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(BCHG, Prepare)
struct F {
    F() {
        // BCHG.B #3, (%A2)
        TEST::SET_W(0, 0004120 | 2);
        TEST::SET_W(2, 3);
        TEST::SET_W(4, TEST_BREAK);

        // BCHG.B %D4, (%A2)
        TEST::SET_W(6, 0000520 | 2 | 4 << 9);
        TEST::SET_W(8, TEST_BREAK);

        // BCHG.L #20, %D2
        TEST::SET_W(10, 0004100 | 2);
        TEST::SET_W(12, 20);
        TEST::SET_W(14, TEST_BREAK);

        // BCHG.L %D4, %D2
        TEST::SET_W(16, 0000500 | 2 | 4 << 9);
        TEST::SET_W(18, TEST_BREAK);

        jit_compile(0, 20);
    }
};
BOOST_AUTO_TEST_SUITE(R, *boost::unit_test::fixture<F>())
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_DATA_TEST_CASE(ByImm, bdata::xrange(2), z) {
    RAM[0x100] = z << 3;
    cpu.A[2] = 0x100;
    run_test(0);
    BOOST_TEST(cpu.Z != z);
    BOOST_TEST(RAM[0x100] == !z << 3);
}


BOOST_AUTO_TEST_CASE(ByReg) {
    cpu.D[4] = 3;
    RAM[0x100] = 0xff;
    cpu.A[2] = 0x100;
    run_test(6);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(RAM[0x100] == 0xF7);
}


BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)

BOOST_DATA_TEST_CASE(ByImm, bdata::xrange(2), z) {
    cpu.D[2] = z << 20;
    run_test(10);
    BOOST_TEST(cpu.Z != z);
    BOOST_TEST(cpu.D[2] == !z << 20);
}

BOOST_AUTO_TEST_CASE(ByReg) {
    cpu.D[2] = 0xffffffff;
    cpu.D[4] = 20;
    run_test(16);
    BOOST_TEST(cpu.D[2] == 0xFFEFFFFF);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()