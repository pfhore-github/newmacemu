#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "jit.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_Bcc {
    F_Bcc() {
        for(int cc = 2; cc < 16; ++cc) {
            // Bcc <END>
            TEST::SET_W(4*(cc-2), 0060000 | cc << 8 | (54-4*(cc-2)));
            TEST::SET_W(4*(cc-2)+2, TEST_BREAK);
        }

        TEST::SET_W(56, TEST_BREAK);
        jit_compile(0, 58);
    }};
BOOST_FIXTURE_TEST_SUITE(Bcc, Prepare, *boost::unit_test::fixture<F_Bcc>())


BOOST_AUTO_TEST_CASE(untraced) {
    cpu.T = 0;
    cpu.C = false;
    run_test(8);
    BOOST_TEST(cpu.PC == 58);
    BOOST_TEST(!cpu.must_trace);
}

BOOST_AUTO_TEST_CASE(traced) {
    cpu.T = 1;
    cpu.C = false;
    run_test(8, EXCEPTION_NUMBER::TRACE);
}

BOOST_AUTO_TEST_CASE(trace_not_taken) {
    cpu.T = 1;
    cpu.C = true;
    run_test(8);
    BOOST_TEST(cpu.PC == 12);
    BOOST_TEST(!cpu.must_trace);
}

BOOST_AUTO_TEST_SUITE(HI)
BOOST_AUTO_TEST_CASE(T) {
    cpu.C = false;
    cpu.Z = false;
    run_test(0);
    BOOST_TEST(cpu.PC == 58);
}
BOOST_AUTO_TEST_CASE(F1) {
    cpu.C = true;
    run_test(0);
    BOOST_TEST(cpu.PC == 4);
}
BOOST_AUTO_TEST_CASE(F2) {
    cpu.Z = true;
    run_test(0);
    BOOST_TEST(cpu.PC == 4);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(LS)
BOOST_AUTO_TEST_CASE(T1) {
    cpu.C = true;
    run_test(4);
    BOOST_TEST(cpu.PC == 58);
}
BOOST_AUTO_TEST_CASE(T2) {
    cpu.Z = true;
    run_test(4);
    BOOST_TEST(cpu.PC == 58);
}
BOOST_AUTO_TEST_CASE(F) {
    cpu.Z = false;
    cpu.C = false;
    run_test(4);
    BOOST_TEST(cpu.PC == 8);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(CC)
BOOST_AUTO_TEST_CASE(T) {
    cpu.C = false;
    run_test(8);
    BOOST_TEST(cpu.PC == 58);
}
BOOST_AUTO_TEST_CASE(F) {
    cpu.C = true;
    run_test(8);
    BOOST_TEST(cpu.PC == 12);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(CS)
BOOST_AUTO_TEST_CASE(T) {
    cpu.C = true;
    run_test(12);
    BOOST_TEST(cpu.PC == 58);
}
BOOST_AUTO_TEST_CASE(F) {
    cpu.C = false;
    run_test(12);
    BOOST_TEST(cpu.PC == 16);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(NE)
BOOST_AUTO_TEST_CASE(T) {
    cpu.Z = false;
    run_test(16);
    BOOST_TEST(cpu.PC == 58);
}
BOOST_AUTO_TEST_CASE(F) {
    cpu.Z = true;
    run_test(16);
    BOOST_TEST(cpu.PC == 20);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(EQ)
BOOST_AUTO_TEST_CASE(T) {
    cpu.Z = true;
    run_test(20);
    BOOST_TEST(cpu.PC == 58);
}
BOOST_AUTO_TEST_CASE(F) {
    cpu.Z = false;
    run_test(20);
    BOOST_TEST(cpu.PC == 24);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(VC)
BOOST_AUTO_TEST_CASE(T) {
    cpu.V = false;
    run_test(24);
    BOOST_TEST(cpu.PC == 58);
}
BOOST_AUTO_TEST_CASE(F) {
    cpu.V = true;
    run_test(24);
    BOOST_TEST(cpu.PC == 28);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(VS)
BOOST_AUTO_TEST_CASE(T) {
    cpu.V = true;
    run_test(28);
    BOOST_TEST(cpu.PC == 58);
}
BOOST_AUTO_TEST_CASE(F) {
    cpu.V = false;
    run_test(28);
    BOOST_TEST(cpu.PC == 32);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(PL)
BOOST_AUTO_TEST_CASE(T) {
    cpu.N = false;
    run_test(32);
    BOOST_TEST(cpu.PC == 58);
}
BOOST_AUTO_TEST_CASE(F) {
    cpu.N = true;
    run_test(32);
    BOOST_TEST(cpu.PC == 36);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(MI)
BOOST_AUTO_TEST_CASE(T) {
    cpu.N = true;
    run_test(36);
    BOOST_TEST(cpu.PC == 58);
}
BOOST_AUTO_TEST_CASE(F) {
    cpu.N = false;
    run_test(36);
    BOOST_TEST(cpu.PC == 40);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GE)
BOOST_AUTO_TEST_CASE(T1) {
    cpu.N = false;
    cpu.V = false;
    run_test(40);
    BOOST_TEST(cpu.PC == 58);
}

BOOST_AUTO_TEST_CASE(T2) {
    cpu.N = true;
    cpu.V = true;
    run_test(40);
    BOOST_TEST(cpu.PC == 58);
}

BOOST_AUTO_TEST_CASE(F1) {
    cpu.N = true;
    cpu.V = false;
    run_test(40);
    BOOST_TEST(cpu.PC == 44);
}

BOOST_AUTO_TEST_CASE(F2) {
    cpu.N = false;
    cpu.V = true;
    run_test(40);
    BOOST_TEST(cpu.PC == 44);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(LT)
BOOST_AUTO_TEST_CASE(T1) {
    cpu.N = true;
    cpu.V = false;
    run_test(44);
    BOOST_TEST(cpu.PC == 58);
}
BOOST_AUTO_TEST_CASE(T2) {
    cpu.N = false;
    cpu.V = true;
    run_test(44);
    BOOST_TEST(cpu.PC == 58);
}
BOOST_AUTO_TEST_CASE(F1) {
    cpu.N = false;
    cpu.V = false;
    run_test(44);
    BOOST_TEST(cpu.PC == 48);
}

BOOST_AUTO_TEST_CASE(F2) {
    cpu.N = true;
    cpu.V = true;
    run_test(44);
    BOOST_TEST(cpu.PC == 48);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GT)
BOOST_AUTO_TEST_CASE(T1) {
    cpu.N = false;
    cpu.V = false;
    cpu.Z = false;
    run_test(48);
    BOOST_TEST(cpu.PC == 58);
}

BOOST_AUTO_TEST_CASE(T2) {
    cpu.N = true;
    cpu.V = true;
    cpu.Z = false;
    run_test(48);
    BOOST_TEST(cpu.PC == 58);
}

BOOST_AUTO_TEST_CASE(F1) {
    cpu.N = true;
    cpu.V = false;
    run_test(48);
    BOOST_TEST(cpu.PC == 52);
}

BOOST_AUTO_TEST_CASE(F2) {
    cpu.N = false;
    cpu.V = true;
    run_test(48);
    BOOST_TEST(cpu.PC == 52);
}

BOOST_AUTO_TEST_CASE(F3) {
    cpu.Z = true;
    run_test(48);
    BOOST_TEST(cpu.PC == 52);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(LE)
BOOST_AUTO_TEST_CASE(T1) {
    cpu.Z = true;
    run_test(52);
    BOOST_TEST(cpu.PC == 58);
}
BOOST_AUTO_TEST_CASE(T2) {
    cpu.N = false;
    cpu.V = true;
    run_test(52);
    BOOST_TEST(cpu.PC == 58);
}
BOOST_AUTO_TEST_CASE(T3) {
    cpu.N = true;
    cpu.V = false;
    run_test(52);
    BOOST_TEST(cpu.PC == 58);
}
BOOST_AUTO_TEST_CASE(F1) {
    cpu.Z = false;
    cpu.N = false;
    cpu.V = false;
    run_test(52);
    BOOST_TEST(cpu.PC == 56);
}

BOOST_AUTO_TEST_CASE(F2) {
    cpu.Z = false;
    cpu.N = true;
    cpu.V = true;
    run_test(52);
    BOOST_TEST(cpu.PC == 56);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
