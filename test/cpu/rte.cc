#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(RTE, Prepare)
BOOST_AUTO_TEST_CASE(Disasm) {
    TEST::SET_W(0, 0047163);
    BOOST_TEST(disasm() == "RTE");
}
BOOST_AUTO_TEST_SUITE(format)
BOOST_DATA_TEST_CASE(type0, bdata::xrange(2), T) {
    cpu.S = true;
    cpu.M = true;
    cpu.A[7] = 0x1000;
    TEST::SET_W(0x1000, 0x1F | T << 14);
    TEST::SET_L(0x1002, 0x40);
    TEST::SET_W(0x1006, 0 << 12 | 3);

    TEST::SET_W(0, 0047163);
    decode_and_run();
    BOOST_TEST(cpu.MSP == 0x1008);
    BOOST_TEST(cpu.PC == 0x40);
    BOOST_TEST(cpu.must_trace == !!T);
    BOOST_TEST(!cpu.S);
}

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
    decode_and_run();
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
    decode_and_run();
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
    decode_and_run();
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
    decode_and_run();
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
    decode_and_run();
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
    decode_and_run();
    BOOST_TEST(TEST::GET_W(0x1030) == 0x101F);
    BOOST_TEST(TEST::GET_L(0x1032) == 0x40);
    BOOST_TEST(TEST::GET_W(0x1036) == (0x2000 | (9 << 2)));
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
    if(setjmp(cpu.ex_buf) == 0) {
        ADDRESS_ERROR(0x1001);
    }
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
    if(setjmp(cpu.ex_buf) == 0) {
        ILLEGAL_OP();
    }
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
    if(setjmp(cpu.ex_buf) == 0) {
        DIV0_ERROR();
    }
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
    if(setjmp(cpu.ex_buf) == 0) {
        CHK_ERROR();
    }
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
    if(setjmp(cpu.ex_buf) == 0) {
        TRAPX_ERROR();
    }
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
    if(setjmp(cpu.ex_buf) == 0) {
        PRIV_ERROR();
    }
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
    if(setjmp(cpu.ex_buf) == 0) {
        TRACE();
    }
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
    if(setjmp(cpu.ex_buf) == 0) {
        ALINE();
    }
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
    if(setjmp(cpu.ex_buf) == 0) {
        FLINE();
    }
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
    if(setjmp(cpu.ex_buf) == 0) {
        FORMAT_ERROR();
    }
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
    TEST::SET_L(0x2000 | ((32+n )<< 2), 0x3000);
    cpu.ISP = cpu.MSP = 0x1008;
    cpu.oldpc = 0x100;
    cpu.PC = 0x102;
    if(setjmp(cpu.ex_buf) == 0) {
        TRAP_ERROR(n);
    }
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x102);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x0000 | ((32+n) << 2)));
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
    if(setjmp(cpu.ex_buf) == 0) {
        FP_EX_BSUN();
    }
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
    if(setjmp(cpu.ex_buf) == 0) {
        FP_EX_INEX();
    }
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
    if(setjmp(cpu.ex_buf) == 0) {
        FP_EX_DIV0();
    }
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
    if(setjmp(cpu.ex_buf) == 0) {
        FP_EX_UNFL();
    }
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
    if(setjmp(cpu.ex_buf) == 0) {
        FP_EX_OPERR();
    }
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
    if(setjmp(cpu.ex_buf) == 0) {
        FP_EX_OVFL();
    }
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
    if(setjmp(cpu.ex_buf) == 0) {
        FP_EX_SNAN();
    }
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.T == 0);
    BOOST_TEST(TEST::GET_L(0x1002) == 0x102);
    BOOST_TEST(TEST::GET_W(0x1006) == (0x3000 | (54 << 2)));
    BOOST_TEST(TEST::GET_L(0x1008) == 0x4500);
    BOOST_TEST(cpu.PC == 0x3000);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()