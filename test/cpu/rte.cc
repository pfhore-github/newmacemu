#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "bus.hpp"
#include "memory.hpp"
#include "test.hpp"
#include <boost/test/debug.hpp>

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
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
    }
};
BOOST_FIXTURE_TEST_SUITE(RTE, Prepare, *boost::unit_test::fixture<F_RTE>())

BOOST_AUTO_TEST_CASE(user) {
    cpu.S = false;
    run_test(0, EXCEPTION_NUMBER::PRIV_ERR);
}
BOOST_AUTO_TEST_SUITE(format)
BOOST_AUTO_TEST_CASE(type0) {
    cpu.S = true;
    cpu.M = true;
    cpu.A[7] = 0x8800;
    TEST::SET_W(0x6800, 0x1F);
    TEST::SET_L(0x6802, 0x40);
    TEST::SET_W(0x6806, 0 << 12 | 3);

    run_test(0);
    BOOST_TEST(cpu.PC == 0x42);
    BOOST_TEST(cpu.MSP == 0x8808);
    BOOST_TEST(!cpu.S);
}
BOOST_AUTO_TEST_CASE(type0_trace) {
    cpu.S = true;
    cpu.M = true;
    cpu.A[7] = 0x8800;
    TEST::SET_W(0x6800, 0x1F | 1 << 14);
    TEST::SET_L(0x6802, 0x40);
    TEST::SET_W(0x6806, 0 << 12 | 3);
    run_test(0, EXCEPTION_NUMBER::TRACE);
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
    cpu.A[7] = 0x8800;
    TEST::SET_W(0x6800, 0x1F);
    TEST::SET_L(0x6802, 0x40);
    TEST::SET_W(0x6806, 2 << 12);
    TEST::SET_L(0x6808, 0); // unused

    run_test(0);
    BOOST_TEST(cpu.MSP == 0x880C);
    BOOST_TEST(cpu.PC == 0x42);
    BOOST_TEST(!cpu.S);
}

BOOST_AUTO_TEST_CASE(type3) {
    cpu.S = true;
    cpu.M = true;
    cpu.A[7] = 0x8800;
    TEST::SET_W(0x6800, 0x1F);
    TEST::SET_L(0x6802, 0x40);
    TEST::SET_W(0x6806, 3 << 12);
    TEST::SET_L(0x6808, 0); // unused

    run_test(0);
    BOOST_TEST(cpu.MSP == 0x880C);
    BOOST_TEST(cpu.PC == 0x42);
    BOOST_TEST(!cpu.S);
}

BOOST_AUTO_TEST_CASE(type7_norestart) {
    cpu.S = true;
    cpu.M = true;
    cpu.A[7] = 0x8800;
    TEST::SET_W(0x6800, 0x1F);
    TEST::SET_L(0x6802, 0x40);
    TEST::SET_W(0x6806, 7 << 12);
    TEST::SET_L(0x6808, 0); // unused
    TEST::SET_W(0x680C, 0);

    run_test(0);
    BOOST_TEST(cpu.MSP == 0x8800 + 60);
    BOOST_TEST(cpu.PC == 0x42);
    BOOST_TEST(!cpu.S);
}

BOOST_AUTO_TEST_CASE(type7_movem) {
    cpu.S = true;
    cpu.M = true;
    cpu.A[7] = 0x8800;
    cpu.D[0] = 0x1234;
    TEST::SET_W(0x6800, 0x1F);
    TEST::SET_L(0x6802, 0x42);
    TEST::SET_W(0x6806, 7 << 12);
    TEST::SET_L(0x6808, 0x1200); // EA
    TEST::SET_W(0x680C, 1 << 12);

    run_test(0);
    BOOST_TEST(cpu.MSP == 0x8800 + 60);
    BOOST_TEST(cpu.PC == 0x48);
    BOOST_TEST(!cpu.S);
    BOOST_TEST(TEST::GET_W(0x1200) == 0x1234);
}

BOOST_AUTO_TEST_CASE(type7_trace) {
    cpu.S = true;
    cpu.M = true;
    cpu.A[7] = 0x8800;
    TEST::SET_W(0x6800, 0x101F);
    TEST::SET_L(0x6802, 0x40);
    TEST::SET_W(0x6806, 7 << 12);
    TEST::SET_W(0x680C, 1 << 13);

    run_test(0, EXCEPTION_NUMBER::TRACE);
}
// CU/CP not supported
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Occur)

BOOST_AUTO_TEST_CASE(AddressError) {
    cpu.oldpc = 0x100;
    cpu.ex_addr = 0x1001;
    handle_exception(EXCEPTION_NUMBER::ADDR_ERR);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x6FF6) == 0x100);
    BOOST_TEST(TEST::GET_W(0x6FFA) == (0x2000 | (3 << 2)));
    BOOST_TEST(TEST::GET_L(0x6FFC) == 0x1000);
    BOOST_TEST(cpu.PC == 0x3000 + (3 << 2));
}

BOOST_AUTO_TEST_CASE(ILLEGALOP) {
    cpu.oldpc = 0x40;
    handle_exception(EXCEPTION_NUMBER::ILLEGAL_OP);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x6FFA) == 0x40);
    BOOST_TEST(TEST::GET_W(0x6FFE) == (0x0000 | (4 << 2)));
    BOOST_TEST(cpu.PC == 0x3000 + (4 << 2));
}

BOOST_AUTO_TEST_CASE(DIV0) {
    cpu.oldpc = 0x10;
    cpu.PC = 0x12;
    handle_exception(EXCEPTION_NUMBER::DIV0);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x6FF6) == 0x12);
    BOOST_TEST(TEST::GET_W(0x6FFA) == (0x2000 | (5 << 2)));
    BOOST_TEST(TEST::GET_L(0x6FFC) == 0x10);
    BOOST_TEST(cpu.PC == 0x3000 + ( 5 << 2));
}

BOOST_AUTO_TEST_CASE(CHK) {
    cpu.oldpc = 0x10;
    cpu.PC = 0x12;
    handle_exception(EXCEPTION_NUMBER::CHK_FAIL);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x6FF6) == 0x12);
    BOOST_TEST(TEST::GET_W(0x6FFA) == (0x2000 | (6 << 2)));
    BOOST_TEST(TEST::GET_L(0x6FFC) == 0x10);
    BOOST_TEST(cpu.PC == 0x3000 + ( 6 << 2));
}

BOOST_AUTO_TEST_CASE(TRAPxx) {
    cpu.oldpc = 0x10;
    cpu.PC = 0x12;
    handle_exception(EXCEPTION_NUMBER::TRAPx);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x6FF6) == 0x12);
    BOOST_TEST(TEST::GET_W(0x6FFA) == (0x2000 | (7 << 2)));
    BOOST_TEST(TEST::GET_L(0x6FFC) == 0x10);
    BOOST_TEST(cpu.PC == 0x3000 + ( 7 << 2));
}

BOOST_AUTO_TEST_CASE(PRIV_VIOLATION) {
    cpu.oldpc = 0x10;
    cpu.PC = 0x12;
    handle_exception(EXCEPTION_NUMBER::PRIV_ERR);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x6FFA) == 0x10);
    BOOST_TEST(TEST::GET_W(0x6FFE) == (0x0000 | (8 << 2)));
    BOOST_TEST(cpu.PC == 0x3000 + (8 << 2));
}

BOOST_AUTO_TEST_CASE(TRACE) {
    cpu.oldpc = 0x10;
    cpu.PC = 0x12;
    handle_exception(EXCEPTION_NUMBER::TRACE);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x6FF6) == 0x12);
    BOOST_TEST(TEST::GET_W(0x6FFA) == (0x2000 | (9 << 2)));
    BOOST_TEST(TEST::GET_L(0x6FFC) == 0x10);
    BOOST_TEST(cpu.PC == 0x3000 + ( 9 << 2));
}

BOOST_AUTO_TEST_CASE(ALINE_) {
    cpu.oldpc = 0x10;
    cpu.PC = 0x12;
    handle_exception(EXCEPTION_NUMBER::ALINE);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x6FFA) == 0x10);
    BOOST_TEST(TEST::GET_W(0x6FFE) == (0x0000 | (10 << 2)));
    BOOST_TEST(cpu.PC == 0x3000+ ( 10 << 2));
}

BOOST_AUTO_TEST_CASE(FLINE_) {
    cpu.oldpc = 0x10;
    cpu.PC = 0x12;
    handle_exception(EXCEPTION_NUMBER::FLINE);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x6FFA) == 0x10);
    BOOST_TEST(TEST::GET_W(0x6FFE) == (0x0000 | (11 << 2)));
    BOOST_TEST(cpu.PC == 0x3000+ ( 11 << 2));
}

BOOST_AUTO_TEST_CASE(FMT_ERR) {
    cpu.oldpc = 0x10;
    cpu.PC = 0x12;
    handle_exception(EXCEPTION_NUMBER::FORMAT_ERR);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x6FFA) == 0x10);
    BOOST_TEST(TEST::GET_W(0x6FFE) == (0x0000 | (14 << 2)));
    BOOST_TEST(cpu.PC == 0x3000+ ( 14 << 2));
}

BOOST_DATA_TEST_CASE(TRAPn, bdata::xrange(16), n) {
    cpu.oldpc = 0x10;
    cpu.PC = 0x12;
    handle_exception(EXCEPTION_NUMBER(int(EXCEPTION_NUMBER::TRAP0) + n));
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x6FFA) == 0x12);
    BOOST_TEST(TEST::GET_W(0x6FFE) == (0x0000 | ((32 + n) << 2)));
    BOOST_TEST(cpu.PC == 0x3000+ (( 32+n) << 2));
}

BOOST_AUTO_TEST_CASE(BSUN) {
    cpu.oldpc = 0x10;
    cpu.PC = 0x12;
    cpu.EA = 0x4500;
    handle_exception(EXCEPTION_NUMBER::FP_UNORDER);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x6FF6) == 0x12);
    BOOST_TEST(TEST::GET_W(0x6FFA) == (0x3000 | (48 << 2)));
    BOOST_TEST(TEST::GET_L(0x6FFC) == 0x4500);
    BOOST_TEST(cpu.PC == 0x3000 + (48 << 2));
}

BOOST_AUTO_TEST_CASE(INEX) {
    cpu.S = false;
    cpu.PC = 0x12;
    cpu.EA = 0x4500;
    handle_exception(EXCEPTION_NUMBER::FP_INEX);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x6FF6) == 0x12);
    BOOST_TEST(TEST::GET_W(0x6FFA) == (0x3000 | (49 << 2)));
    BOOST_TEST(TEST::GET_L(0x6FFC) == 0x4500);
    BOOST_TEST(cpu.PC == 0x3000 + (49 << 2));
}

BOOST_AUTO_TEST_CASE(DV0) {
    cpu.oldpc = 0x10;
    cpu.PC = 0x12;
    cpu.EA = 0x4500;
    handle_exception(EXCEPTION_NUMBER::FP_DIV0);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x6FF6) == 0x12);
    BOOST_TEST(TEST::GET_W(0x6FFA) == (0x3000 | (50 << 2)));
    BOOST_TEST(TEST::GET_L(0x6FFC) == 0x4500);
    BOOST_TEST(cpu.PC == 0x3000 + (50 << 2));
}

BOOST_AUTO_TEST_CASE(UNFL) {
    cpu.oldpc = 0x10;
    cpu.PC = 0x12;
    cpu.EA = 0x4500;
    handle_exception(EXCEPTION_NUMBER::FP_UNFL);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x6FF6) == 0x12);
    BOOST_TEST(TEST::GET_W(0x6FFA) == (0x3000 | (51 << 2)));
    BOOST_TEST(TEST::GET_L(0x6FFC) == 0x4500);
    BOOST_TEST(cpu.PC == 0x3000 + (51 << 2));
}

BOOST_AUTO_TEST_CASE(OPERR) {
    cpu.oldpc = 0x10;
    cpu.PC = 0x12;
    cpu.EA = 0x4500;
    handle_exception(EXCEPTION_NUMBER::FP_OPERR);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x6FF6) == 0x12);
    BOOST_TEST(TEST::GET_W(0x6FFA) == (0x3000 | (52 << 2)));
    BOOST_TEST(TEST::GET_L(0x6FFC) == 0x4500);
    BOOST_TEST(cpu.PC == 0x3000 + (52 << 2));
}

BOOST_AUTO_TEST_CASE(OVFL) {
    cpu.oldpc = 0x10;
    cpu.PC = 0x12;
    cpu.EA = 0x4500;
    handle_exception(EXCEPTION_NUMBER::FP_OVFL);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x6FF6) == 0x12);
    BOOST_TEST(TEST::GET_W(0x6FFA) == (0x3000 | (53 << 2)));
    BOOST_TEST(TEST::GET_L(0x6FFC) == 0x4500);
    BOOST_TEST(cpu.PC == 0x3000 + (53 << 2));
}

BOOST_AUTO_TEST_CASE(SNAN_) {
    cpu.oldpc = 0x10;
    cpu.PC = 0x12;
    cpu.EA = 0x4500;
    handle_exception(EXCEPTION_NUMBER::FP_SNAN);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x6FF6) == 0x12);
    BOOST_TEST(TEST::GET_W(0x6FFA) == (0x3000 | (54 << 2)));
    BOOST_TEST(TEST::GET_L(0x6FFC) == 0x4500);
    BOOST_TEST(cpu.PC == 0x3000 + (54 << 2));
}
struct AccessErrorBase : Prepare {
    AccessErrorBase() {
        cpu.oldpc = 0x10;
        cpu.PC = 0x12;
        cpu.bus_lock = false;
        cpu.movem_run = false;
        cpu.inJit = false;
    }
};
constexpr uint32_t FAULT_STACK_BASE = 0x6FC4;
BOOST_FIXTURE_TEST_SUITE(AccessError, AccessErrorBase)
BOOST_AUTO_TEST_CASE(common) {
    try {
        cpu.TCR_E = false;
        cpu.MSP = 0x7000;
        cpu.VBR = 0x7000;
        WriteB(0xefffffff, 0);
    } catch(M68kException& e) {
        handle_exception(EXCEPTION_NUMBER::AFAULT);
    }
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(FAULT_STACK_BASE + 2) == 0x10);
    BOOST_TEST(TEST::GET_W(FAULT_STACK_BASE + 6) == (0x7000 | (2 << 2)));
    BOOST_TEST(TEST::GET_W(FAULT_STACK_BASE + 0x0C) == (1 << 5 | 1));
    BOOST_TEST(TEST::GET_L(FAULT_STACK_BASE + 0x14) == 0xefffffff);
    BOOST_TEST(cpu.PC == 0x3000+ (2 << 2));
}
BOOST_AUTO_TEST_CASE(CT) {
    try {
        cpu.must_trace = true;
        WriteB(0x3002, 0xff);
    } catch(M68kException& e) {
        handle_exception(EXCEPTION_NUMBER::AFAULT);
    }
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_W(FAULT_STACK_BASE + 0x0C) & SSW_CT);
}
BOOST_AUTO_TEST_CASE(CM) {
    try {
        cpu.EA = 0x3000;
        cpu.movem_run = true;
        WriteB(0x3002, 0xff);
    } catch(M68kException& e) {
        handle_exception(EXCEPTION_NUMBER::AFAULT);
    }
    BOOST_TEST(TEST::GET_L(FAULT_STACK_BASE + 8) == 0x3000);
    BOOST_TEST(TEST::GET_W(FAULT_STACK_BASE + 0x0C) & SSW_CM);
}

BOOST_AUTO_TEST_CASE(MA) {
    try {
        cpu.movem_run = true;
        WriteW(0x1FFF, 0);
    } catch(M68kException& e) {
        handle_exception(EXCEPTION_NUMBER::AFAULT);
    }
    BOOST_TEST(TEST::GET_W(FAULT_STACK_BASE + 0x0C) & SSW_MA);
}

BOOST_AUTO_TEST_CASE(ATC) {
    try {
        WriteW(0x4000, 0);
    } catch(M68kException& e) {
        handle_exception(EXCEPTION_NUMBER::AFAULT);
    }
    BOOST_TEST(TEST::GET_W(FAULT_STACK_BASE + 0x0C) & SSW_ATC);
}

BOOST_AUTO_TEST_CASE(LK) {
    try {
        cpu.bus_lock = true;
        WriteW(0x2000, 0);
    } catch(M68kException& e) {
        handle_exception(EXCEPTION_NUMBER::AFAULT);
    }
    BOOST_TEST(TEST::GET_W(FAULT_STACK_BASE + 0x0C) & SSW_LK);
}

BOOST_AUTO_TEST_CASE(RW) {
    try {
        ReadW(0x3000);
    } catch(M68kException& e) {
        handle_exception(EXCEPTION_NUMBER::AFAULT);
    }
    BOOST_TEST(TEST::GET_W(FAULT_STACK_BASE + 0x0C) & SSW_RW);
}

BOOST_AUTO_TEST_CASE(SZ_W) {
    try {
        WriteW(0x3000, 0);
    } catch(M68kException& e) {
        handle_exception(EXCEPTION_NUMBER::AFAULT);
    }
    BOOST_TEST((TEST::GET_W(FAULT_STACK_BASE + 0x0C) >> 5 & 3) == int(SIZ::W));
}

BOOST_AUTO_TEST_CASE(SZ_L) {
    try {
        WriteL(0x3000, 0);
    } catch(M68kException& e) {
        handle_exception(EXCEPTION_NUMBER::AFAULT);
    }
    BOOST_TEST((TEST::GET_W(FAULT_STACK_BASE + 0x0C) >> 5 & 3) == int(SIZ::L));
}

BOOST_AUTO_TEST_CASE(TT_MOVE16_) {
    try {
        Transfer16(0x100, 0x3000);
    } catch(M68kException& e) {
        handle_exception(EXCEPTION_NUMBER::AFAULT);
    }
    BOOST_TEST((TEST::GET_W(FAULT_STACK_BASE + 0x0C) & TT_MASK) == TT_MOVE16);
}

BOOST_AUTO_TEST_CASE(TT_ALT_) {
    try {
        WriteSB(0x3000, 0);
    } catch(M68kException& e) {
        handle_exception(EXCEPTION_NUMBER::AFAULT);
    }
    BOOST_TEST((TEST::GET_W(FAULT_STACK_BASE + 0x0C) & TT_MASK) == TT_ALT);
}

BOOST_AUTO_TEST_CASE(TM_USER_DATA) {
    try {
        WriteW(0x3000, 0);
    } catch(M68kException& e) {
        handle_exception(EXCEPTION_NUMBER::AFAULT);
    }
    BOOST_TEST((TEST::GET_W(FAULT_STACK_BASE + 0x0C) & 7) == int(TM::USER_DATA));
}

BOOST_AUTO_TEST_CASE(TM_USER_CODE) {
    try {
        FetchW(0x3000);
    } catch(M68kException& e) {
        handle_exception(EXCEPTION_NUMBER::AFAULT);
    }
    BOOST_TEST((TEST::GET_W(FAULT_STACK_BASE + 0x0C) & 7) == int(TM::USER_CODE));
}
BOOST_AUTO_TEST_CASE(TM_SYS_DATA) {
    try {
        cpu.S = true;
        WriteW(0x2000, 0);
    } catch(M68kException& e) {
        handle_exception(EXCEPTION_NUMBER::AFAULT);
    }
    BOOST_TEST((TEST::GET_W(FAULT_STACK_BASE + 0x0C) & 7) == int(TM::SYS_DATA));
}

BOOST_AUTO_TEST_CASE(TM_SYS_CODE) {
    try {
        cpu.S = true;
        FetchW(0x4000);
    } catch(M68kException& e) {
        handle_exception(EXCEPTION_NUMBER::AFAULT);
    }
    BOOST_TEST((TEST::GET_W(FAULT_STACK_BASE + 0x0C) & 7) == int(TM::SYS_CODE));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
