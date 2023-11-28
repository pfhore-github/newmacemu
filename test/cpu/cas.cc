#define BOOST_TEST_DYN_LINK
#include "68040.hpp"

#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_CAS {
    F_CAS() {
        // CAS.B %D4, %D3, (%A2)
        TEST::SET_W(0, 0005320 | 2);
        TEST::SET_W(2, 3 << 6 | 4);
        TEST::SET_W(4, TEST_BREAK);

        // CAS.W %D4, %D3, (%A2)
        TEST::SET_W(6, 0006320 | 2);
        TEST::SET_W(8, 3 << 6 | 4);
        TEST::SET_W(10, TEST_BREAK);

        // CAS.L %D4, %D3, (%A2)
        TEST::SET_W(12, 0007320 | 2);
        TEST::SET_W(14, 3 << 6 | 4);
        TEST::SET_W(16, TEST_BREAK);

        jit_compile(0, 18);
    }};
BOOST_FIXTURE_TEST_SUITE(CAS, Prepare, *boost::unit_test::fixture<F_CAS>())


BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(T) {
    RAM[0x100] = 0x22;
    cpu.A[2] = 0x100;
    cpu.D[3] = 0x30;
    cpu.D[4] = 0x22;
    run_test(0);
    BOOST_TEST(RAM[0x100] == 0x30);
}

BOOST_AUTO_TEST_CASE(F) {
    RAM[0x100] = 0x22;
    cpu.A[2] = 0x100;
    cpu.D[3] = 0x30;
    cpu.D[4] = 0x35;
    run_test(0);
    BOOST_TEST(cpu.D[4] == 0x22);
}

BOOST_AUTO_TEST_CASE(traced) {
    cpu.T = 1;
	run_test(0);
	BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::TRACE );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_W(0x100, 0x2222);
    cpu.A[2] = 0x100;
    cpu.D[3] = 0x3030;
    cpu.D[4] = 0x2222;
    run_test(6);
    BOOST_TEST(TEST::GET_W(0x100) == 0x3030);
}

BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_W(0x100, 0x2222);
    cpu.A[2] = 0x100;
    cpu.D[3] = 0x3030;
    cpu.D[4] = 0x3535;
    run_test(6);
    BOOST_TEST(cpu.D[4] == 0x2222);
}

BOOST_AUTO_TEST_CASE(traced) {
    cpu.T = 1;
	run_test(6);
	BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::TRACE );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(T) {
    TEST::SET_L(0x100, 0x22222222);
    cpu.A[2] = 0x100;
    cpu.D[3] = 0x30303030;
    cpu.D[4] = 0x35353535;
    run_test(12);
    BOOST_TEST(cpu.D[4] == 0x22222222);
}

BOOST_AUTO_TEST_CASE(F) {
    TEST::SET_L(0x100, 0x22222222);
    cpu.A[2] = 0x100;
    cpu.D[3] = 0x30303030;
    cpu.D[4] = 0x22222222;
    run_test(12);
    BOOST_TEST(TEST::GET_L(0x100) == 0x30303030);
}

BOOST_AUTO_TEST_CASE(traced) {
    cpu.T = 1;
	run_test(12);
	BOOST_TEST(cpu.ex_n == EXCEPTION_NUMBER::TRACE );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
