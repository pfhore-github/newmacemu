#define BOOST_TEST_DYN_LINK
#include "inline.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
BOOST_AUTO_TEST_SUITE(add_b)
BOOST_AUTO_TEST_CASE(value) { BOOST_TEST(ADD_B(7, -3) == 4); }
BOOST_AUTO_TEST_CASE(z) {
    cpu.Z = false;
    ADD_B(7, -7);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(n) {
    cpu.N = false;
    ADD_B(7, -10);
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_CASE(c) {
    cpu.X = cpu.C = false;
    ADD_B(0xff, 1);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_CASE(v1) {
    cpu.V = false;
    ADD_B(0x7f, 1);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(v2) {
    cpu.V = false;
    ADD_B(0x80, -1);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(add_w)
BOOST_AUTO_TEST_CASE(value) { BOOST_TEST(ADD_W(700, -300) == 400); }
BOOST_AUTO_TEST_CASE(z) {
    cpu.Z = false;
    ADD_W(700, -700);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(n) {
    cpu.N = false;
    ADD_W(700, -1000);
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_CASE(c) {
    cpu.C = cpu.X = false;
    ADD_W(0xffff, 1);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_CASE(v1) {
    cpu.V = false;
    ADD_W(0x7fff, 1);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(v2) {
    cpu.V = false;
    ADD_W(0x8000, -1);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(add_l)
BOOST_AUTO_TEST_CASE(value) { BOOST_TEST(ADD_L(70000, -30000) == 40000); }
BOOST_AUTO_TEST_CASE(z) {
    cpu.Z = false;
    ADD_L(70000, -70000);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(n) {
    cpu.N = false;
    ADD_L(70000, -100000);
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_CASE(c) {
    cpu.C = cpu.X = false;
    ADD_L(0xffffffff, 1);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_CASE(v1) {
    cpu.V = false;
    ADD_L(0x7fffffff, 1);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(v2) {
    cpu.V = false;
    ADD_L(0x80000000, -1);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(sub_b)
BOOST_AUTO_TEST_CASE(value) { BOOST_TEST(SUB_B(7, -3) == 10); }
BOOST_AUTO_TEST_CASE(z) {
    cpu.Z = false;
    SUB_B(7, 7);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(n) {
    cpu.N = false;
    SUB_B(7, 10);
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_CASE(c) {
    cpu.X = cpu.C = false;
    SUB_B(0, 1);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_CASE(v1) {
    cpu.V = false;
    SUB_B(0x80, 1);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(v2) {
    cpu.V = false;
    SUB_B(0x7f, -1);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(sub_w)
BOOST_AUTO_TEST_CASE(value) { BOOST_TEST(SUB_W(700, -300) == 1000); }
BOOST_AUTO_TEST_CASE(z) {
    cpu.Z = false;
    SUB_W(700, 700);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(n) {
    cpu.N = false;
    SUB_W(700, 1000);
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_CASE(c) {
    cpu.C = cpu.X = false;
    SUB_W(0, 1);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_CASE(v1) {
    cpu.V = false;
    SUB_W(0x8000, 1);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(v2) {
    cpu.V = false;
    SUB_W(0x7fff, -1);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(sub_l)
BOOST_AUTO_TEST_CASE(value) { BOOST_TEST(SUB_L(70000, -30000) == 100000); }
BOOST_AUTO_TEST_CASE(z) {
    cpu.Z = false;
    SUB_L(70000, 70000);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(n) {
    cpu.N = false;
    SUB_L(70000, 100000);
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_CASE(c) {
    cpu.C = cpu.X = false;
    SUB_L(0, 1);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_CASE(v1) {
    cpu.V = false;
    SUB_L(0x80000000, 1);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(v2) {
    cpu.V = false;
    SUB_L(0x7fffffff, -1);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(bcd2bin) { BOOST_TEST(BCD2BIN(0x23) == 23); }

BOOST_AUTO_TEST_CASE(bin2bcd) { BOOST_TEST(BIN2BCD(23) == 0x23); }
BOOST_AUTO_TEST_SUITE(mulu_ll)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(MULU_LL(12345, 67890) == 838102050ULL);
}
BOOST_AUTO_TEST_CASE(z) {
    MULU_LL(12345, 0);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(n) {
    MULU_LL(0xC0000000U, 0xC0000000U);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(muls_ll)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(MULS_LL(-12345, 67890) == -838102050LL);
}
BOOST_AUTO_TEST_CASE(z) {
    MULS_LL(12345, 0);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(n) {
    MULS_LL(-1, 1);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(mulu_l)
BOOST_AUTO_TEST_CASE(value) { BOOST_TEST(MULU_L(12345, 67890) == 838102050U); }
BOOST_AUTO_TEST_CASE(z) {
    MULU_L(12345, 0);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(n) {
    MULU_L(0x40000000U, 2);
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_CASE(v) {
    MULU_L(0x80000000U, 2);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(muls_l)
BOOST_AUTO_TEST_CASE(value) { BOOST_TEST(MULS_L(-12345, 67890) == -838102050); }
BOOST_AUTO_TEST_CASE(z) {
    MULS_L(12345, 0);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(n) {
    MULS_L(-1, 2);
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_CASE(v) {
    MULS_L(0x40000000U, 2);
    BOOST_TEST(cpu.V);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(mulu_w)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(MULU_W(1234, 6789) == 8377626);
}
BOOST_AUTO_TEST_CASE(z) {
    MULU_W(12345, 0);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(n) {
    MULU_W(0xC000, 0xC000);
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(muls_w)
BOOST_AUTO_TEST_CASE(value) {
    BOOST_TEST(MULS_W(-1234, 6789) == -8377626);
}
BOOST_AUTO_TEST_CASE(z) {
    MULS_W(12345, 0);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(n) {
    MULS_W(-1, 1);
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(divu_ll)
BOOST_AUTO_TEST_CASE(value) {
    auto [q, r] = DIVU_LL(1234567, 345);
    BOOST_TEST(q == 3578);
    BOOST_TEST(r == 157);
}
BOOST_AUTO_TEST_CASE(z) {
    DIVU_LL(0, 4);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(n) {
    DIVU_LL(0x100000000LLU, 2);
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_CASE(v) {
    DIVU_LL(0x200000000LLU, 2);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(dz) {
    if( setjmp(cpu.ex_buf) == 0) {
        DIVU_LL(3, 0);    
        BOOST_FAIL("not handled");
    } else {
        BOOST_TEST(cpu.ex_n == 5);
    }
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(divs_ll)
BOOST_AUTO_TEST_CASE(value) {
    auto [q, r] = DIVS_LL(-1234567, 345);
    BOOST_TEST(q == -3578);
    BOOST_TEST(r == -157);
}
BOOST_AUTO_TEST_CASE(z) {
    DIVS_LL(0, 4);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(n) {
    DIVS_LL(-6, 2);
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_CASE(v) {
    DIVS_LL(0x200000000LL, 2);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(dz) {
    if( setjmp(cpu.ex_buf) == 0) {
        DIVS_LL(3, 0);    
        BOOST_FAIL("not handled");
    } else {
        BOOST_TEST(cpu.ex_n == 5);
    }
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(divu_l)
BOOST_AUTO_TEST_CASE(value) {
    auto [q, r] = DIVU_L(1234567, 345);
    BOOST_TEST(q == 3578);
    BOOST_TEST(r == 157);
}
BOOST_AUTO_TEST_CASE(z) {
    DIVU_L(0, 4);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(n) {
    DIVU_L(0x80000000U, 1);
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_CASE(dz) {
    if( setjmp(cpu.ex_buf) == 0) {
        DIVU_L(3, 0);    
        BOOST_FAIL("not handled");
    } else {
        BOOST_TEST(cpu.ex_n == 5);
    }
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(divs_l)
BOOST_AUTO_TEST_CASE(value) {
    auto [q, r] = DIVS_L(-1234567, 345);
    BOOST_TEST(q == -3578);
    BOOST_TEST(r == -157);
}
BOOST_AUTO_TEST_CASE(z) {
    DIVS_L(0, 4);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(n) {
    DIVS_L(-6, 2);
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_CASE(v) {
    DIVS_L(0x80000000, -1);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(dz) {
    if( setjmp(cpu.ex_buf) == 0) {
        DIVS_L(3, 0);    
        BOOST_FAIL("not handled");
    } else {
        BOOST_TEST(cpu.ex_n == 5);
    }
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(divu_w)
BOOST_AUTO_TEST_CASE(value) {
    auto [q, r] = DIVU_W(1234567, 345);
    BOOST_TEST(q == 3578);
    BOOST_TEST(r == 157);
}
BOOST_AUTO_TEST_CASE(z) {
    DIVU_W(0, 4);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(n) {
    DIVU_W(0x10000, 2);
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_CASE(dz) {
    if( setjmp(cpu.ex_buf) == 0) {
        DIVU_W(3, 0);    
        BOOST_FAIL("not handled");
    } else {
        BOOST_TEST(cpu.ex_n == 5);
    }
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(divs_w)
BOOST_AUTO_TEST_CASE(value) {
    auto [q, r] = DIVS_W(-1234567, 345);
    BOOST_TEST(q == -3578);
    BOOST_TEST(r == -157);
}
BOOST_AUTO_TEST_CASE(z) {
    DIVS_W(0, 4);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(n) {
    DIVS_W(-6, 2);
    BOOST_TEST(cpu.N);
}
BOOST_AUTO_TEST_CASE(v) {
    DIVS_W(0x7fffffff, -1);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(dz) {
    if( setjmp(cpu.ex_buf) == 0) {
        DIVS_W(3, 0);    
        BOOST_FAIL("not handled");
    } else {
        BOOST_TEST(cpu.ex_n == 5);
    }
}
BOOST_AUTO_TEST_SUITE_END()