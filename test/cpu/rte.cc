#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "memory.hpp"
#include "test.hpp"
#include "bus.hpp"
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
        // MOVEM.W %D0, (%A5)
        TEST::SET_W(0x42, 0044220 | 5);
        TEST::SET_W(0x44, 1);
        TEST::SET_W(0x46, TEST_BREAK);
        jit_compile(0, 4);
        jit_compile(0x40, 8);
    }};
BOOST_FIXTURE_TEST_SUITE(RTE, Prepare, *boost::unit_test::fixture<F_RTE>())


BOOST_AUTO_TEST_CASE(user) {
    cpu.S = false;
	run_test(0);
	BOOST_TEST( cpu.ex_n == EXCAPTION_NUMBER::PRIV_ERR );
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
	BOOST_TEST(cpu.ex_n == EXCAPTION_NUMBER::TRACE);
}

BOOST_AUTO_TEST_CASE(type1) {
    cpu.S = true;
    cpu.M = false;
    cpu.ISP = cpu.A[7] = 0x100;
    TEST::SET_W(0x100, 0x2000);
    TEST::SET_L(0x102, 0x20);
    TEST::SET_W(0x106, 1 << 12 | 3);

    TEST::SET_W(0x108, 0x1F);
    TEST::SET_L(0x10A, 0x40);
    TEST::SET_W(0x10E, 0 << 12 | 3);

    run_test(0);
    BOOST_TEST(cpu.ISP == 0x110);
    BOOST_TEST(cpu.PC == 0x42);
    BOOST_TEST(!cpu.S);
}
BOOST_AUTO_TEST_CASE(type2) {
    cpu.S = true;
    cpu.M = true;
    cpu.A[7] = 0x100;
    TEST::SET_W(0x100, 0x1F);
    TEST::SET_L(0x102, 0x40);
    TEST::SET_W(0x106, 2 << 12);
    TEST::SET_L(0x108, 0); // unused

    run_test(0);
    BOOST_TEST(cpu.MSP == 0x10C);
    BOOST_TEST(cpu.PC == 0x42);
    BOOST_TEST(!cpu.S);
}

BOOST_AUTO_TEST_CASE(type3) {
    cpu.S = true;
    cpu.M = true;
    cpu.A[7] = 0x100;
    TEST::SET_W(0x100, 0x1F);
    TEST::SET_L(0x102, 0x40);
    TEST::SET_W(0x106, 3 << 12);
    TEST::SET_L(0x108, 0); // unused

    run_test(0);
    BOOST_TEST(cpu.MSP == 0x10C);
    BOOST_TEST(cpu.PC == 0x42);
    BOOST_TEST(!cpu.S);
}

BOOST_AUTO_TEST_CASE(type7_norestart) {
    cpu.S = true;
    cpu.M = true;
    cpu.A[7] = 0x100;
    TEST::SET_W(0x100, 0x1F);
    TEST::SET_L(0x102, 0x40);
    TEST::SET_W(0x106, 7 << 12);
    TEST::SET_L(0x108, 0); // unused
    TEST::SET_W(0x10C, 0);

    run_test(0);
    BOOST_TEST(cpu.MSP == 0x100 + 60);
    BOOST_TEST(cpu.PC == 0x42);
    BOOST_TEST(!cpu.S);
}

BOOST_AUTO_TEST_CASE(type7_movem) {
    cpu.S = true;
    cpu.M = true;
    cpu.A[7] = 0x100;
    cpu.D[0] = 0x1234;
    TEST::SET_W(0x100, 0x1F);
    TEST::SET_L(0x102, 0x42);
    TEST::SET_W(0x106, 7 << 12);
    TEST::SET_L(0x108, 0x200); // EA
    TEST::SET_W(0x10C, 1 << 12);

    run_test(0);
    BOOST_TEST(cpu.MSP == 0x100 + 60);
    BOOST_TEST(cpu.PC == 0x48);
    BOOST_TEST(!cpu.S);
    BOOST_TEST(TEST::GET_W(0x200) == 0x1234);
}

BOOST_AUTO_TEST_CASE(type7_trace) {
    cpu.S = true;
    cpu.M = true;
    cpu.A[7] = 0x100;
    TEST::SET_W(0x100, 0x101F);
    TEST::SET_L(0x102, 0x40);
    TEST::SET_W(0x106, 7 << 12);
    TEST::SET_W(0x10C, 1 << 13);

	run_test(0);
	BOOST_TEST(cpu.ex_n == EXCAPTION_NUMBER::TRACE);
}
// CU/CP not supported
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Occur)

BOOST_AUTO_TEST_CASE(AddressError) {
    cpu.in_exception = false;
    cpu.VBR = 0x200;
    cpu.S = false;
    TEST::SET_L(0x200 | (3 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x10C;
    cpu.oldpc = 0x100;
	cpu.ex_addr = 0x1001;
	handle_exception(EXCAPTION_NUMBER::ADDR_ERR);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x102) == 0x100);
    BOOST_TEST(TEST::GET_W(0x106) == (0x2000 | (3 << 2)));
    BOOST_TEST(TEST::GET_L(0x108) == 0x1000);
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(ILLEGALOP) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x200;
    TEST::SET_L(0x200 | (4 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x108;
    cpu.oldpc = 0x40;
	handle_exception(EXCAPTION_NUMBER::ILLEGAL_OP);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x102) == 0x40);
    BOOST_TEST(TEST::GET_W(0x106) == (0x0000 | (4 << 2)));
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(DIV0) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x200;
    TEST::SET_L(0x200 | (5 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x10C;
    cpu.oldpc = 0x10;
    cpu.PC = 0x12;
    handle_exception(EXCAPTION_NUMBER::DIV0);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x102) == 0x12);
    BOOST_TEST(TEST::GET_W(0x106) == (0x2000 | (5 << 2)));
    BOOST_TEST(TEST::GET_L(0x108) == 0x10);
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(CHK) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x200;
    TEST::SET_L(0x200 | (6 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x10C;
    cpu.oldpc = 0x10;
    cpu.PC = 0x12;
    handle_exception(EXCAPTION_NUMBER::CHK_FAIL);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x102) == 0x12);
    BOOST_TEST(TEST::GET_W(0x106) == (0x2000 | (6 << 2)));
    BOOST_TEST(TEST::GET_L(0x108) == 0x10);
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(TRAPxx) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x200;
    TEST::SET_L(0x200 | (7 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x10C;
    cpu.oldpc = 0x10;
    cpu.PC = 0x12;
    handle_exception(EXCAPTION_NUMBER::TRAPx);
	BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x102) == 0x12);
    BOOST_TEST(TEST::GET_W(0x106) == (0x2000 | (7 << 2)));
    BOOST_TEST(TEST::GET_L(0x108) == 0x10);
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(PRIV_VIOLATION) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x200;
    TEST::SET_L(0x200 | (8 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x108;
    cpu.oldpc = 0x10;
    cpu.PC = 0x12;
	handle_exception(EXCAPTION_NUMBER::PRIV_ERR);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x102) == 0x10);
    BOOST_TEST(TEST::GET_W(0x106) == (0x0000 | (8 << 2)));
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(TRACE) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x200;
    TEST::SET_L(0x200 | (9 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x10C;
    cpu.oldpc = 0x10;
    cpu.PC = 0x12;
	handle_exception(EXCAPTION_NUMBER::TRACE);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x102) == 0x12);
    BOOST_TEST(TEST::GET_W(0x106) == (0x2000 | (9 << 2)));
    BOOST_TEST(TEST::GET_L(0x108) == 0x10);
    BOOST_TEST(cpu.PC == 0x3000);
}
BOOST_AUTO_TEST_CASE(ALINE_) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x200;
    TEST::SET_L(0x200 | (10 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x108;
    cpu.oldpc = 0x10;
    cpu.PC = 0x12;
	handle_exception(EXCAPTION_NUMBER::ALINE);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x102) == 0x10);
    BOOST_TEST(TEST::GET_W(0x106) == (0x0000 | (10 << 2)));
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(FLINE_) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x200;
    TEST::SET_L(0x200 | (11 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x108;
    cpu.oldpc = 0x10;
    cpu.PC = 0x12;
	handle_exception(EXCAPTION_NUMBER::FLINE);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x102) == 0x10);
    BOOST_TEST(TEST::GET_W(0x106) == (0x0000 | (11 << 2)));
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(FMT_ERR) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x200;
    TEST::SET_L(0x200 | (14 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x108;
    cpu.oldpc = 0x10;
    cpu.PC = 0x12;
	handle_exception(EXCAPTION_NUMBER::FORMAT_ERR);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x102) == 0x10);
    BOOST_TEST(TEST::GET_W(0x106) == (0x0000 | (14 << 2)));
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_DATA_TEST_CASE(TRAPn, bdata::xrange(16), n) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x200;
    TEST::SET_L(0x200 | ((32 + n) << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x108;
    cpu.oldpc = 0x10;
    cpu.PC = 0x12;
	handle_exception(EXCAPTION_NUMBER(int(EXCAPTION_NUMBER::TRAP0) + n));
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x102) == 0x12);
    BOOST_TEST(TEST::GET_W(0x106) == (0x0000 | ((32 + n) << 2)));
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(BSUN) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x200;
    TEST::SET_L(0x200 | (48 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x10C;
    cpu.oldpc = 0x10;
    cpu.PC = 0x12;
    cpu.EA = 0x4500;
	handle_exception(EXCAPTION_NUMBER::FP_UNORDER);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x102) == 0x12);
    BOOST_TEST(TEST::GET_W(0x106) == (0x3000 | (48 << 2)));
    BOOST_TEST(TEST::GET_L(0x108) == 0x4500);
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(INEX) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x200;
    TEST::SET_L(0x200 | (49 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x10C;
    cpu.oldpc = 0x10;
    cpu.PC = 0x12;
    cpu.EA = 0x4500;
	handle_exception(EXCAPTION_NUMBER::FP_INEX);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x102) == 0x12);
    BOOST_TEST(TEST::GET_W(0x106) == (0x3000 | (49 << 2)));
    BOOST_TEST(TEST::GET_L(0x108) == 0x4500);
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(DV0) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x200;
    TEST::SET_L(0x200 | (50 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x10C;
    cpu.oldpc = 0x10;
    cpu.PC = 0x12;
    cpu.EA = 0x4500;
	handle_exception(EXCAPTION_NUMBER::FP_DIV0);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x102) == 0x12);
    BOOST_TEST(TEST::GET_W(0x106) == (0x3000 | (50 << 2)));
    BOOST_TEST(TEST::GET_L(0x108) == 0x4500);
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(UNFL) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x200;
    TEST::SET_L(0x200 | (51 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x10C;
    cpu.oldpc = 0x10;
    cpu.PC = 0x12;
    cpu.EA = 0x4500;
	handle_exception(EXCAPTION_NUMBER::FP_UNFL);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x102) == 0x12);
    BOOST_TEST(TEST::GET_W(0x106) == (0x3000 | (51 << 2)));
    BOOST_TEST(TEST::GET_L(0x108) == 0x4500);
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(OPERR) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x200;
    TEST::SET_L(0x200 | (52 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x10C;
    cpu.oldpc = 0x10;
    cpu.PC = 0x12;
    cpu.EA = 0x4500;
	handle_exception(EXCAPTION_NUMBER::FP_OPERR);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x102) == 0x12);
    BOOST_TEST(TEST::GET_W(0x106) == (0x3000 | (52 << 2)));
    BOOST_TEST(TEST::GET_L(0x108) == 0x4500);
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(OVFL) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x200;
    TEST::SET_L(0x200 | (53 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x10C;
    cpu.oldpc = 0x10;
    cpu.PC = 0x12;
    cpu.EA = 0x4500;
	handle_exception(EXCAPTION_NUMBER::FP_OVFL);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x102) == 0x12);
    BOOST_TEST(TEST::GET_W(0x106) == (0x3000 | (53 << 2)));
    BOOST_TEST(TEST::GET_L(0x108) == 0x4500);
    BOOST_TEST(cpu.PC == 0x3000);
}

BOOST_AUTO_TEST_CASE(SNAN_) {
    cpu.in_exception = false;
    cpu.S = false;
    cpu.VBR = 0x200;
    TEST::SET_L(0x200 | (54 << 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x10C;
    cpu.oldpc = 0x10;
    cpu.PC = 0x12;
    cpu.EA = 0x4500;
	handle_exception(EXCAPTION_NUMBER::FP_SNAN);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x102) == 0x12);
    BOOST_TEST(TEST::GET_W(0x106) == (0x3000 | (54 << 2)));
    BOOST_TEST(TEST::GET_L(0x108) == 0x4500);
    BOOST_TEST(cpu.PC == 0x3000);
}
struct AccessErrorBase : Prepare {
    AccessErrorBase() {
		cpu.S = false;
        cpu.in_exception = false;
        cpu.VBR = 0x200;
        TEST::SET_L(0x200 | (2 << 2), 0x3000);
        cpu.ISP = cpu.MSP = 0x100 + 60;
        cpu.oldpc = 0x10;
        cpu.PC = 0x12;
		cpu.bus_lock = false;
		cpu.movem_run = false;
    }
};
BOOST_FIXTURE_TEST_SUITE(AccessError, AccessErrorBase)
BOOST_AUTO_TEST_CASE(common) {
    if( setjmp(cpu.ex) == 0) {
		cpu.TCR_E = false;
        cpu.fault_SSW = 0;
        WriteB(0xffffffff, 0);
    } else {
        handle_exception(EXCAPTION_NUMBER::AFAULT);
    }
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x102) == 0x10);
    BOOST_TEST(TEST::GET_W(0x106) == (0x7000 | (2 << 2)));
    BOOST_TEST(TEST::GET_W(0x10C) == (1 << 5 | 1));
    BOOST_TEST(TEST::GET_L(0x114) == 0xffffffff);
    BOOST_TEST(cpu.PC == 0x3000);
}
BOOST_AUTO_TEST_CASE(CT) {
    if( setjmp(cpu.ex) == 0) {
		cpu.must_trace = true;
        ReadB(0x3002);
    } else {
        handle_exception(EXCAPTION_NUMBER::AFAULT);
    }
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_W(0x10C) & SSW_CT);
}
BOOST_AUTO_TEST_CASE(CM) {
    if( setjmp(cpu.ex) == 0) {
        cpu.EA = 0x3000;
		cpu.movem_run = true;
        ReadB(0x3002);
    } else {
        handle_exception(EXCAPTION_NUMBER::AFAULT);
    }
    BOOST_TEST(TEST::GET_L(0x108) == 0x3000);
    BOOST_TEST(TEST::GET_W(0x10C) & SSW_CM);
}

BOOST_AUTO_TEST_CASE(MA) {
    if( setjmp(cpu.ex) == 0) {
		cpu.movem_run = true;
        WriteW(0x0FFF, 0);
    } else {
        handle_exception(EXCAPTION_NUMBER::AFAULT);
    }
    BOOST_TEST(TEST::GET_W(0x10C) & SSW_MA);
}

BOOST_AUTO_TEST_CASE(ATC) {
    if( setjmp(cpu.ex) == 0) {
        WriteW(0x3000, 0);
    } else {
        handle_exception(EXCAPTION_NUMBER::AFAULT);
    }
    BOOST_TEST(TEST::GET_W(0x10C) & SSW_ATC);
}

BOOST_AUTO_TEST_CASE(LK) {
    if( setjmp(cpu.ex) == 0) {
		cpu.bus_lock = true;
        WriteW(0x3000, 0);
    } else {
        handle_exception(EXCAPTION_NUMBER::AFAULT);
    }
    BOOST_TEST(TEST::GET_W(0x10C) & SSW_LK);
}

BOOST_AUTO_TEST_CASE(RW) {
    if( setjmp(cpu.ex) == 0) {
        ReadW(0x3000);
    } else {
        handle_exception(EXCAPTION_NUMBER::AFAULT);
    }
    BOOST_TEST(TEST::GET_W(0x10C) & SSW_RW);
}

BOOST_AUTO_TEST_CASE(SZ_W) {
    if( setjmp(cpu.ex) == 0) {
        WriteW(0x3000, 0);
    } else {
        handle_exception(EXCAPTION_NUMBER::AFAULT);
    }
    BOOST_TEST((TEST::GET_W(0x10C) >> 5 & 3) == int(SIZ::W));
}

BOOST_AUTO_TEST_CASE(SZ_L) {
    if( setjmp(cpu.ex) == 0) {
        WriteL(0x3000, 0);
    } else {
        handle_exception(EXCAPTION_NUMBER::AFAULT);
    }
    BOOST_TEST((TEST::GET_W(0x10C) >> 5 & 3) == int(SIZ::L));
}


BOOST_AUTO_TEST_CASE(TT_MOVE16_) {
    if( setjmp(cpu.ex) == 0) {
        MMU_Transfer16(0x100, 0x3000);
    } else {
        handle_exception(EXCAPTION_NUMBER::AFAULT);
    }
	BOOST_TEST( (TEST::GET_W(0x10C) & TT_MASK) == TT_MOVE16 );
}

BOOST_AUTO_TEST_CASE(TT_ALT_) {
    if( setjmp(cpu.ex) == 0) {
		cpu.fault_SSW = (cpu.fault_SSW &~ TT_MASK) | TT_ALT;
		WriteB(0x3000, 0);
    } else {
        handle_exception(EXCAPTION_NUMBER::AFAULT);
    }
	BOOST_TEST( (TEST::GET_W(0x10C) & TT_MASK) == TT_ALT );
}

BOOST_AUTO_TEST_CASE(TM_USER_DATA) {
    if( setjmp(cpu.ex) == 0) {
		cpu.S = false;
		WriteW(0x3000, 0);
    } else {
        handle_exception(EXCAPTION_NUMBER::AFAULT);
    }
	BOOST_TEST( (TEST::GET_W(0x10C) & 7) == int(TM::USER_DATA) );
}

BOOST_AUTO_TEST_CASE(TM_USER_CODE) {
    if( setjmp(cpu.ex) == 0) {
		cpu.S = false;
		FetchW(0x3000);
    } else {
        handle_exception(EXCAPTION_NUMBER::AFAULT);
    }
	BOOST_TEST( (TEST::GET_W(0x10C) & 7) == int(TM::USER_CODE) );
}
BOOST_AUTO_TEST_CASE(TM_SYS_DATA) {
    if( setjmp(cpu.ex) == 0) {
		cpu.S = true;
		WriteW(0x3000, 0);
    } else {
        handle_exception(EXCAPTION_NUMBER::AFAULT);
    }
	BOOST_TEST( (TEST::GET_W(0x10C) & 7) == int(TM::SYS_DATA) );
}

BOOST_AUTO_TEST_CASE(TM_SYS_CODE) {
    if( setjmp(cpu.ex) == 0) {
		cpu.S = true;
		FetchW(0x3000);
    } else {
        handle_exception(EXCAPTION_NUMBER::AFAULT);
    }
	BOOST_TEST( (TEST::GET_W(0x10C) & 7) == int(TM::SYS_CODE) );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
