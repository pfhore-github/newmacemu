#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "memory.hpp"
#include "test.hpp"
#include <boost/test/debug.hpp>

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_RTE {
    F_RTE() {
        // RTE
        TEST::SET_W(0, 0047163);
        TEST::SET_W(2, TEST_BREAK);

        TEST::SET_W(0x40, TEST_BREAK);
        jit_compile(0, 4);
        jit_compile(0x40, 2);
    }};
BOOST_FIXTURE_TEST_SUITE(RTE, Prepare, *boost::unit_test::fixture<F_RTE>())


BOOST_AUTO_TEST_CASE(user) {
    cpu.S = false;
	run_test(0);
	BOOST_TEST( cpu.ex_n == 8 );
}
BOOST_AUTO_TEST_SUITE(format)
BOOST_AUTO_TEST_CASE(type0) {
    cpu.S = true;
    cpu.M = true;
    cpu.A[7] = 0x100;
    TEST::SET_W(0x100, 0x1F);
    TEST::SET_L(0x102, 0x40);
    TEST::SET_W(0x106, 0 << 12 | 3);

    run_test(0);
    BOOST_TEST(cpu.PC == 0x42);
    BOOST_TEST(cpu.MSP == 0x108);
    BOOST_TEST(!cpu.S);
}
BOOST_AUTO_TEST_CASE(type0_trace) {
    cpu.S = true;
    cpu.M = true;
    cpu.A[7] = 0x100;
    TEST::SET_W(0x100, 0x1F | 1 << 14);
    TEST::SET_L(0x102, 0x40);
    TEST::SET_W(0x106, 0 << 12 | 3);
	run_test(0);
	BOOST_TEST(cpu.ex_n == 9);
}
#if 0

BOOST_AUTO_TEST_CASE(type1) {
    cpu.S = true;
    cpu.M = false;
    cpu.ISP = cpu.A[7] = 0x1000;
    TEST::SET_W(0x1000, 0x2000);
    TEST::SET_L(0x1002, 0x20);
    TEST::SET_W(0x1006, 1 << 12 | 3);

    TEST::SET_W(0x1008, 0x1F);
    TEST::SET_L(0x100A, 0x40);
    TEST::SET_W(0x100E, 0 << 12 | 3);

    TEST::SET_W(0, 0047163);
    run_test();
    BOOST_TEST(cpu.ISP == 0x1010);
    BOOST_TEST(cpu.PC == 0x40);
    BOOST_TEST(!cpu.S);
}
BOOST_AUTO_TEST_CASE(type2) {
    cpu.S = true;
    cpu.M = true;
    cpu.A[7] = 0x1000;
    TEST::SET_W(0x1000, 0x1F);
    TEST::SET_L(0x1002, 0x40);
    TEST::SET_W(0x1006, 2 << 12);
    TEST::SET_L(0x1008, 0); // unused

    TEST::SET_W(0, 0047163);
    run_test();
    BOOST_TEST(cpu.MSP == 0x100C);
    BOOST_TEST(cpu.PC == 0x40);
    BOOST_TEST(!cpu.S);
}

BOOST_AUTO_TEST_CASE(type3) {
    cpu.S = true;
    cpu.M = true;
    cpu.A[7] = 0x1000;
    TEST::SET_W(0x1000, 0x1F);
    TEST::SET_L(0x1002, 0x40);
    TEST::SET_W(0x1006, 3 << 12);
    TEST::SET_L(0x1008, 0); // unused

    TEST::SET_W(0, 0047163);
    run_test();
    BOOST_TEST(cpu.MSP == 0x100C);
    BOOST_TEST(cpu.PC == 0x40);
    BOOST_TEST(!cpu.S);
}

BOOST_AUTO_TEST_CASE(type7_norestart) {
    cpu.S = true;
    cpu.M = true;
    cpu.A[7] = 0x1000;
    TEST::SET_W(0x1000, 0x1F);
    TEST::SET_L(0x1002, 0x40);
    TEST::SET_W(0x1006, 7 << 12);
    TEST::SET_L(0x1008, 0); // unused
    TEST::SET_W(0x100C, 0);

    TEST::SET_W(0, 0047163);
    run_test();
    BOOST_TEST(cpu.MSP == 0x1000 + 60);
    BOOST_TEST(cpu.PC == 0x40);
    BOOST_TEST(!cpu.S);
}

BOOST_AUTO_TEST_CASE(type7_movem) {
    cpu.S = true;
    cpu.M = true;
    cpu.A[7] = 0x1000;
    TEST::SET_W(0x1000, 0x1F);
    TEST::SET_L(0x1002, 0x40);
    TEST::SET_W(0x1006, 7 << 12);
    TEST::SET_L(0x1008, 0x2400); // EA
    TEST::SET_W(0x100C, 1 << 12);

    TEST::SET_W(0, 0047163);
    run_test();
    BOOST_TEST(cpu.MSP == 0x1000 + 60);
    BOOST_TEST(cpu.PC == 0x40);
    BOOST_TEST(!cpu.S);
    BOOST_TEST(cpu.EA == 0x2400);
    BOOST_TEST(cpu.movem_run);
}

BOOST_AUTO_TEST_CASE(type7_trace) {
    cpu.S = true;
    cpu.M = true;
    cpu.A[7] = 0x1000;
    TEST::SET_W(0x1000, 0x101F);
    TEST::SET_L(0x1002, 0x40);
    TEST::SET_W(0x1006, 7 << 12);
    TEST::SET_W(0x100C, 1 << 13);

    TEST::SET_W(0, 0047163);
	run_test(0);
	BOOST_TEST(cpu.ex_n == 9);
}
// CU/CP not supported
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Occur)

BOOST_AUTO_TEST_CASE(AddressError) {
    cpu.in_exception = false;
    cpu.VBR = 0x2000;
    cpu.S = false;
    TEST::SET_L(0x2000 | (3 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x100C;
    cpu.oldpc = 0x100;
	cpu.ex_addr = 0x1001;
	handle_exception(3);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x100);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x2000 | (3 << 2)));
    BOOST_TEST(TEST::GET_L(0x1008) == 0x1000);
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(ILLEGALOP) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x2000;
    TEST::SET_L(0x2000 | (4 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x1008;
    cpu.oldpc = 0x100;
	handle_exception(4);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x100);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x0000 | (4 << 2)));
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(DIV0) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x2000;
    TEST::SET_L(0x2000 | (5 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x100C;
    cpu.oldpc = 0x100;
    cpu.PC = 0x102;
    handle_exception(5);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x102);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x2000 | (5 << 2)));
    BOOST_TEST(TEST::GET_L(0x1008) == 0x100);
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(CHK) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x2000;
    TEST::SET_L(0x2000 | (6 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x100C;
    cpu.oldpc = 0x100;
    cpu.PC = 0x102;
    CHK_ERROR().raise();
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x102);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x2000 | (6 << 2)));
    BOOST_TEST(TEST::GET_L(0x1008) == 0x100);
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(TRAPxx) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x2000;
    TEST::SET_L(0x2000 | (7 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x100C;
    cpu.oldpc = 0x100;
    cpu.PC = 0x102;
	handle_exception(7);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x102);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x2000 | (7 << 2)));
    BOOST_TEST(TEST::GET_L(0x1008) == 0x100);
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(PRIV_VIOLATION) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x2000;
    TEST::SET_L(0x2000 | (8 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x1008;
    cpu.oldpc = 0x100;
    cpu.PC = 0x102;
	handle_exception(8);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x100);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x0000 | (8 << 2)));
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(TRC) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x2000;
    TEST::SET_L(0x2000 | (9 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x100C;
    cpu.oldpc = 0x100;
    cpu.PC = 0x102;
	handle_exception(9);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x102);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x2000 | (9 << 2)));
    BOOST_TEST(TEST::GET_L(0x1008) == 0x100);
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(ALINE_) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x2000;
    TEST::SET_L(0x2000 | (10 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x1008;
    cpu.oldpc = 0x100;
    cpu.PC = 0x102;
    ALINE().raise();
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x100);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x0000 | (10 << 2)));
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(FLINE_) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x2000;
    TEST::SET_L(0x2000 | (11 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x1008;
    cpu.oldpc = 0x100;
    cpu.PC = 0x102;
    FLINE().raise();
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x100);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x0000 | (11 << 2)));
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(FMT_ERR) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x2000;
    TEST::SET_L(0x2000 | (14 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x1008;
    cpu.oldpc = 0x100;
    cpu.PC = 0x102;
    FORMAT_ERROR().raise();
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x100);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x0000 | (14 << 2)));
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_DATA_TEST_CASE(TRAPn, bdata::xrange(16), n) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x2000;
    TEST::SET_L(0x2000 | ((32 + n) << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x1008;
    cpu.oldpc = 0x100;
    cpu.PC = 0x102;
    TRAP_ERROR(n).raise();
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x102);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x0000 | ((32 + n) << 2)));
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(BSUN) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x2000;
    TEST::SET_L(0x2000 | (48 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x100C;
    cpu.oldpc = 0x100;
    cpu.PC = 0x102;
    cpu.EA = 0x4500;
    FP_EX_BSUN().raise();
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x102);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x3000 | (48 << 2)));
    BOOST_TEST(TEST::GET_L(0x1008) == 0x4500);
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(INEX) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x2000;
    TEST::SET_L(0x2000 | (49 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x100C;
    cpu.oldpc = 0x100;
    cpu.PC = 0x102;
    cpu.EA = 0x4500;
    FP_EX_INEX().raise();
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x102);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x3000 | (49 << 2)));
    BOOST_TEST(TEST::GET_L(0x1008) == 0x4500);
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(DV0) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x2000;
    TEST::SET_L(0x2000 | (50 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x100C;
    cpu.oldpc = 0x100;
    cpu.PC = 0x102;
    cpu.EA = 0x4500;
    FP_EX_DIV0().raise();
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x102);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x3000 | (50 << 2)));
    BOOST_TEST(TEST::GET_L(0x1008) == 0x4500);
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(UNFL) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x2000;
    TEST::SET_L(0x2000 | (51 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x100C;
    cpu.oldpc = 0x100;
    cpu.PC = 0x102;
    cpu.EA = 0x4500;
    FP_EX_UNFL().raise();
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x102);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x3000 | (51 << 2)));
    BOOST_TEST(TEST::GET_L(0x1008) == 0x4500);
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(OPERR) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x2000;
    TEST::SET_L(0x2000 | (52 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x100C;
    cpu.oldpc = 0x100;
    cpu.PC = 0x102;
    cpu.EA = 0x4500;
    FP_EX_OPERR().raise();
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x102);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x3000 | (52 << 2)));
    BOOST_TEST(TEST::GET_L(0x1008) == 0x4500);
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(OVFL) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x2000;
    TEST::SET_L(0x2000 | (53 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x100C;
    cpu.oldpc = 0x100;
    cpu.PC = 0x102;
    cpu.EA = 0x4500;
    FP_EX_OVFL().raise();
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x102);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x3000 | (53 << 2)));
    BOOST_TEST(TEST::GET_L(0x1008) == 0x4500);
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(SNAN_) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x2000;
    TEST::SET_L(0x2000 | (54 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x100C;
    cpu.oldpc = 0x100;
    cpu.PC = 0x102;
    cpu.EA = 0x4500;
    FP_EX_SNAN().raise();
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x102);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x3000 | (54 << 2)));
    BOOST_TEST(TEST::GET_L(0x1008) == 0x4500);
    BOOST_TEST(cpu.PC == 0x3000);
}
struct AccessErrorBase : Prepare {
    AccessErrorBase() {
        cpu.in_exception = false;
        cpu.S = false;
        cpu.VBR = 0x2000;
        TEST::SET_L(0x2000 | (2 << 2), 0x3000);
        cpu.ISP = cpu.MSP = 0x1000 + 60;
        cpu.oldpc = 0x100;
        cpu.PC = 0x102;
    }
};
BOOST_FIXTURE_TEST_SUITE(AccessError, AccessErrorBase)
BOOST_AUTO_TEST_SUITE(Read)
BOOST_AUTO_TEST_CASE(B) {
    try {
        cpu.EA = 0xfffffff0;
        ReadB(0xfffffffe);
    } catch(ACCESS_FAULT &e) {
        e.raise();
    }
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x100);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x7000 | (2 << 2)));
    BOOST_TEST(TEST::GET_W(0x100C) == (1 << 8 | 1 << 5 | 1));
    BOOST_TEST(TEST::GET_L(0x1014) == 0xfffffffe);
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(W) {
    try {
        ReadW(0xfffffffe);
    } catch(ACCESS_FAULT & e) {
        e.raise();
    }
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x100);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x7000 | (2 << 2)));
    BOOST_TEST(TEST::GET_W(0x100C) == (1 << 8 | 2 << 5 | 1));
    BOOST_TEST(TEST::GET_L(0x1014) == 0xfffffffe);
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(W_half) {
    uint32_t a = 0x3FFFFFF;
    try {
        ReadW(a);
    } catch(ACCESS_FAULT & e) {
        e.raise();
    }
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x100);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x7000 | (2 << 2)));
    BOOST_TEST(TEST::GET_W(0x100C) == (1 << 11 | 1 << 8 | 2 << 5 | 1));
    BOOST_TEST(TEST::GET_L(0x1014) == a);
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(L) {
    try {
        ReadL(0xfffffffe);
    } catch(ACCESS_FAULT & e) {
        e.raise();
    }
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x100);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x7000 | (2 << 2)));
    BOOST_TEST(TEST::GET_W(0x100C) == (1 << 8 | 0 << 5 | 1));
    BOOST_TEST(TEST::GET_L(0x1014) == 0xfffffffe);
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(L_half) {
    uint32_t a = 0x3FFFFFE;
    try {
        ReadL(a);
    } catch(ACCESS_FAULT & e) {
        e.raise();
    }
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x100);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x7000 | (2 << 2)));
    BOOST_TEST(TEST::GET_W(0x100C) == (1 << 11 | 1 << 8 | 0 << 5 | 1));
    BOOST_TEST(TEST::GET_L(0x1014) == a);
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Write)
BOOST_AUTO_TEST_CASE(B) {
    try {
        WriteB(0xfffffffe, 1);
    } catch(ACCESS_FAULT & e) {
        e.raise();
    }
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x100);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x7000 | (2 << 2)));
    BOOST_TEST(TEST::GET_W(0x100C) == (1 << 5 | 1));
    BOOST_TEST(TEST::GET_L(0x1014) == 0xfffffffe);
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(W) {
    try {
        WriteW(0xfffffffe, 1);
    } catch(ACCESS_FAULT & e) {
        e.raise();
    }
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x100);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x7000 | (2 << 2)));
    BOOST_TEST(TEST::GET_W(0x100C) == (2 << 5 | 1));
    BOOST_TEST(TEST::GET_L(0x1014) == 0xfffffffe);
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(W_half) {
    uint32_t a = 0x3FFFFFF;
    try {
        WriteW(a, 1);
    } catch(ACCESS_FAULT & e) {
        e.raise();
    }
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x100);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x7000 | (2 << 2)));
    BOOST_TEST(TEST::GET_W(0x100C) == (1 << 11 | 2 << 5 | 1));
    BOOST_TEST(TEST::GET_L(0x1014) == a);
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(L) {
    try {
        WriteL(0xfffffff0, 1);
    } catch(ACCESS_FAULT & e) {
        e.raise();
    }
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x100);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x7000 | (2 << 2)));
    BOOST_TEST(TEST::GET_W(0x100C) == (0 << 5 | 1));
    BOOST_TEST(TEST::GET_L(0x1014) == 0xfffffff0);
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(L_half) {
    uint32_t a = 0x3FFFFFE;
    try {
        WriteL(a, 1);
    } catch(ACCESS_FAULT & e) {
        e.raise();
    }
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x100);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x7000 | (2 << 2)));
    BOOST_TEST(TEST::GET_W(0x100C) == (1 << 11 | 0 << 5 | 1));
    BOOST_TEST(TEST::GET_L(0x1014) == a);
    BOOST_TEST(cpu.PC == 0x3000);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(movem) {
    cpu.EA = 0xffffffe0;
    try {
        ReadL(0xfffffff0);
    } catch(ACCESS_FAULT &e) {
        e.CM = true;
        e.raise();
    }
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x100);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x7000 | (2 << 2)));
    BOOST_TEST(TEST::GET_L(0x1008) == 0xffffffe0);
    BOOST_TEST(TEST::GET_W(0x100C) & 1 << 12);
    BOOST_TEST(TEST::GET_L(0x1014) == 0xfffffff0);
    BOOST_TEST(cpu.PC == 0x3000);
}
BOOST_AUTO_TEST_CASE(trace) {
    cpu.must_trace = true;
    cpu.EA = 0xffffffe0;
    try {
        ReadL(0xfffffff0);
    } catch(ACCESS_FAULT & e) {
        e.raise();
    }
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x100);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x7000 | (2 << 2)));
    BOOST_TEST(TEST::GET_L(0x1008) == 0xffffffe0);
    BOOST_TEST(TEST::GET_W(0x100C) & 1 << 13);
    BOOST_TEST(TEST::GET_L(0x1014) == 0xfffffff0);
    BOOST_TEST(cpu.PC == 0x3000);
}
BOOST_AUTO_TEST_SUITE_END()
#endif
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
