#define BOOST_TEST_DYN_LINK
#include "proto.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
std::string disasm_ea(int type, int reg, int sz);
BOOST_FIXTURE_TEST_SUITE(EA, Prepare)

BOOST_AUTO_TEST_SUITE(dasm)
BOOST_AUTO_TEST_CASE(dn) { BOOST_TEST(disasm_ea(0, 1, 0) == "%D1"); }
BOOST_AUTO_TEST_CASE(An) { BOOST_TEST(disasm_ea(1, 1, 0) == "%A1"); }
BOOST_AUTO_TEST_CASE(Mem) { BOOST_TEST(disasm_ea(2, 1, 0) == "(%A1)"); }
BOOST_AUTO_TEST_CASE(Incr) { BOOST_TEST(disasm_ea(3, 1, 0) == "(%A1)+"); }
BOOST_AUTO_TEST_CASE(Decr) {
    BOOST_TEST(disasm_ea(4, 1, 0) == "-(%A1)");
}
BOOST_AUTO_TEST_CASE(dAn) {
    cpu.PC = 0;
    TEST::SET_W(0, 100);
    BOOST_TEST(disasm_ea(5, 1, 0) == "(100, %A1)");
    BOOST_TEST(cpu.PC == 2);
}
BOOST_AUTO_TEST_SUITE(extBreif)
BOOST_AUTO_TEST_CASE(dn) {
    cpu.PC = 0;
    TEST::SET_W(0, 2 << 12 | 0xFE);
    BOOST_TEST(disasm_ea(6, 1, 0) == "(-2, %A1, %R2*1)");
    BOOST_TEST(cpu.PC == 2);
}
BOOST_AUTO_TEST_CASE(W) {
    cpu.PC = 0;
    TEST::SET_W(0, 2 << 12 | 1 << 11 | 2 );
    BOOST_TEST(disasm_ea(6, 1, 0) == "(2, %A1, %R2.W*1)");
    BOOST_TEST(cpu.PC == 2);
}

BOOST_AUTO_TEST_CASE(cc) {
    cpu.PC = 0;
    TEST::SET_W(0, 2 << 12 | 2 << 9 | 2 );
    BOOST_TEST(disasm_ea(6, 1, 0) == "(2, %A1, %R2*4)");
    BOOST_TEST(cpu.PC == 2);
}

BOOST_AUTO_TEST_SUITE(extLong)
BOOST_AUTO_TEST_CASE(no_bd) {
    cpu.PC = 0;
    TEST::SET_W(0, 2 << 12 | 1 << 8 | 1 << 4 );
    BOOST_TEST(disasm_ea(6, 1, 0) == "(%A1, %R2*1)");
    BOOST_TEST(cpu.PC == 2);
}

BOOST_AUTO_TEST_CASE(a_i) {
    cpu.PC = 0;
    TEST::SET_W(0, 2 << 12 | 1 << 7 | 1 << 8 | 1 << 4 );
    BOOST_TEST(disasm_ea(6, 1, 0) == "(%R2*1)");
    BOOST_TEST(cpu.PC == 2);
}

BOOST_AUTO_TEST_CASE(r_i) {
    cpu.PC = 0;
    TEST::SET_W(0, 2 << 12 | 1 << 6 | 1 << 8 | 1 << 4 );
    BOOST_TEST(disasm_ea(6, 1, 0) == "(%A1)");
    BOOST_TEST(cpu.PC == 2);
}

BOOST_AUTO_TEST_CASE(bd_w) {
    cpu.PC = 0;
    TEST::SET_W(0, 2 << 12 | 1 << 8 | 2 << 4 );
    TEST::SET_W(2, 200);
    BOOST_TEST(disasm_ea(6, 1, 0) == "(200, %A1, %R2*1)");
    BOOST_TEST(cpu.PC == 4);
}

BOOST_AUTO_TEST_CASE(bd_l) {
    cpu.PC = 0;
    TEST::SET_W(0, 2 << 12 | 1 << 8 | 3 << 4 );
    TEST::SET_L(2, 40000);
    BOOST_TEST(disasm_ea(6, 1, 0) == "(40000, %A1, %R2*1)");
    BOOST_TEST(cpu.PC == 6);
}

BOOST_AUTO_TEST_CASE(pre) {
    cpu.PC = 0;
    TEST::SET_W(0, 2 << 12 | 1 << 8 | 1 << 4 | 1);
    BOOST_TEST(disasm_ea(6, 1, 0) == "([%A1, %R2*1])");
    BOOST_TEST(cpu.PC == 2);
}

BOOST_AUTO_TEST_CASE(od_w) {
    cpu.PC = 0;
    TEST::SET_W(0, 2 << 12 | 1 << 8 | 1 << 4 | 2);
    TEST::SET_W(2, 100);
    BOOST_TEST(disasm_ea(6, 1, 0) == "([%A1, %R2*1], 100)");
    BOOST_TEST(cpu.PC == 4);
}

BOOST_AUTO_TEST_CASE(od_l) {
    cpu.PC = 0;
    TEST::SET_W(0, 2 << 12 | 1 << 8 | 1 << 4 | 3);
    TEST::SET_L(2, 10000);
    BOOST_TEST(disasm_ea(6, 1, 0) == "([%A1, %R2*1], 10000)");
    BOOST_TEST(cpu.PC == 6);
}

BOOST_AUTO_TEST_CASE(post) {
    cpu.PC = 0;
    TEST::SET_W(0, 2 << 12 | 1 << 8 | 1 << 4 | 1 << 2 | 1);
    BOOST_TEST(disasm_ea(6, 1, 0) == "([%A1], %R2*1)");
    BOOST_TEST(cpu.PC == 2);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(iaddr_w) {
    cpu.PC = 0;
    TEST::SET_W(0, 0x100);
    BOOST_TEST(disasm_ea(7, 0, 0) == "(0x100)");
    BOOST_TEST(cpu.PC == 2);
}
BOOST_AUTO_TEST_CASE(iaddr_l) {
    cpu.PC = 0;
    TEST::SET_L(0, 0x10000);
    BOOST_TEST(disasm_ea(7, 1, 0) == "(0x10000)");
    BOOST_TEST(cpu.PC == 4);
}

BOOST_AUTO_TEST_CASE(pc_d) {
    cpu.PC = 0;
    TEST::SET_W(0, 50);
    BOOST_TEST(disasm_ea(7, 2, 0) == "(50, %PC)");
    BOOST_TEST(cpu.PC == 2);
}

BOOST_AUTO_TEST_CASE(pc_ext) {
    cpu.PC = 0;
    TEST::SET_W(0, 2 << 12 | 0xFE);
    BOOST_TEST(disasm_ea(7, 3, 0) == "(-2, %PC, %R2*1)");
    BOOST_TEST(cpu.PC == 2);
}

BOOST_AUTO_TEST_CASE(immB) {
    cpu.PC = 0;
    TEST::SET_W(0, 30);
    BOOST_TEST(disasm_ea(7, 4, 1) == "#30");
    BOOST_TEST(cpu.PC == 2);
}

BOOST_AUTO_TEST_CASE(immW) {
    cpu.PC = 0;
    TEST::SET_W(0, 3000);
    BOOST_TEST(disasm_ea(7, 4, 2) == "#3000");
    BOOST_TEST(cpu.PC == 2);
}

BOOST_AUTO_TEST_CASE(immL) {
    cpu.PC = 0;
    TEST::SET_L(0, 300000);
    BOOST_TEST(disasm_ea(7, 4, 4) == "#300000");
    BOOST_TEST(cpu.PC == 4);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(D) {
    cpu.D[2] = 0x12345678;
    BOOST_TEST(ea_readB(0, 2) == 0x78);
    BOOST_TEST(ea_readW(0, 2) == 0x5678);
    BOOST_TEST(ea_readL(0, 2) == 0x12345678);
    ea_writeB(0, 2, 0x9a, false);
    BOOST_TEST(cpu.D[2] == 0x1234569a);
    ea_writeW(0, 2, 0x9abc, false);
    BOOST_TEST(cpu.D[2] == 0x12349abc);
    ea_writeL(0, 2, 0xabcdef01, false);
    BOOST_TEST(cpu.D[2] == 0xabcdef01);
}

BOOST_AUTO_TEST_CASE(A) {
    cpu.A[2] = 0x12345678;
    BOOST_TEST(ea_readW(1, 2) == 0x5678);
    BOOST_TEST(ea_readL(1, 2) == 0x12345678);
    ea_writeW(1, 2, 0xA000, false);
    BOOST_TEST(cpu.A[2] == 0xFFFFA000);
    ea_writeL(1, 2, 0x4000, false);
    BOOST_TEST(cpu.A[2] == 0x4000);
}

BOOST_AUTO_TEST_CASE(Imm) {
    cpu.PC = 0;
    TEST::SET_L(0x0, 0x12345678);
    BOOST_TEST(ea_readB(7, 4) == 0x34);
    BOOST_TEST(cpu.PC == 2);
    cpu.PC = 0;
    BOOST_TEST(ea_readW(7, 4) == 0x1234);
    BOOST_TEST(cpu.PC == 2);
    cpu.PC = 0;
    BOOST_TEST(ea_readL(7, 4) == 0x12345678);
    BOOST_TEST(cpu.PC == 4);
}

BOOST_AUTO_TEST_CASE(Mem) {
    TEST::SET_L(0x1000, 0x12345678);
    cpu.A[2] = 0x1000;
    BOOST_TEST(ea_readB(2, 2) == 0x12);
    BOOST_TEST(ea_readW(2, 2) == 0x1234);
    BOOST_TEST(ea_readL(2, 2) == 0x12345678);
    ea_writeB(2, 2, 0xcd, false);
    BOOST_TEST(TEST::GET_L(0x1000) == 0xcd345678);
    ea_writeW(2, 2, 0x789A, false);
    BOOST_TEST(TEST::GET_L(0x1000) == 0x789A5678);
    ea_writeL(2, 2, 0x98765432, false);
    BOOST_TEST(TEST::GET_L(0x1000) == 0x98765432);
}

BOOST_AUTO_TEST_SUITE(Addr)

BOOST_AUTO_TEST_CASE(Base) {
    cpu.A[3] = 0x1000;
    BOOST_TEST(ea_getaddr(2, 3, 0) == 0x1000);
}

BOOST_DATA_TEST_CASE(Inc, bdata::xrange(3), sz) {
    cpu.A[3] = 0x1000;
    BOOST_TEST(ea_getaddr(3, 3, 1 << sz) == 0x1000);
    BOOST_TEST(cpu.A[3] == (0x1000 + (1 << sz)));
}

BOOST_DATA_TEST_CASE(Dec, bdata::xrange(3), sz) {
    cpu.A[3] = 0x1040;
    uint32_t expected = 0x1040 - (1 << sz);
    BOOST_TEST(ea_getaddr(4, 3, 1 << sz) == expected);
    BOOST_TEST(cpu.A[3] == expected);
}

BOOST_AUTO_TEST_CASE(d16) {
    cpu.A[2] = 0x2000;
    TEST::SET_W(0, 0xfffe);
    BOOST_TEST(ea_getaddr(5, 2, 0) == 0x1ffe);
    BOOST_TEST(cpu.PC == 2);
}

BOOST_AUTO_TEST_SUITE(brief)
BOOST_AUTO_TEST_CASE(normal) {
    cpu.A[2] = 0x2000;
    cpu.D[3] = 0x10;
    TEST::SET_W(0, 3 << 12 | 0xF3);
    BOOST_TEST(ea_getaddr(6, 2, 0) == 0x2003);
    BOOST_TEST(cpu.PC == 2);
}

BOOST_AUTO_TEST_CASE(w) {
    cpu.A[2] = 0x2000;
    cpu.D[3] = 0xFFF8;
    TEST::SET_W(0, 3 << 12 | 1 << 11 | 0x10);
    BOOST_TEST(ea_getaddr(6, 2, 0) == 0x2008);
    BOOST_TEST(cpu.PC == 2);
}

BOOST_DATA_TEST_CASE(cc, bdata::xrange(4), value) {
    cpu.A[2] = 0x2000;
    cpu.D[3] = 0x10;
    TEST::SET_W(0, 3 << 12 | value << 9 | 0x08);
    BOOST_TEST(ea_getaddr(6, 2, 0) == 0x2008 + (0x10 << value));
    BOOST_TEST(cpu.PC == 2);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Ext)
BOOST_AUTO_TEST_SUITE(direct)
BOOST_AUTO_TEST_CASE(full) {
    cpu.A[2] = 0x2000;
    cpu.D[3] = 0x100;
    TEST::SET_W(0, 3 << 12 | 1 << 8 | 2 << 4);
    TEST::SET_W(2, 0x400);
    BOOST_TEST(ea_getaddr(6, 2, 0) == 0x2500);
    BOOST_TEST(cpu.PC == 4);
}

BOOST_AUTO_TEST_CASE(bd_l) {
    cpu.A[2] = 0x2000;
    cpu.D[3] = 0x100;
    TEST::SET_W(0, 3 << 12 | 1 << 8 | 3 << 4);
    TEST::SET_L(2, 0x10000);
    BOOST_TEST(ea_getaddr(6, 2, 0) == 0x12100);
    BOOST_TEST(cpu.PC == 6);
}

BOOST_AUTO_TEST_CASE(no_bd) {
    cpu.A[2] = 0x2000;
    cpu.D[3] = 0x100;
    TEST::SET_W(0, 3 << 12 | 1 << 8 | 1 << 4);
    BOOST_TEST(ea_getaddr(6, 2, 0) == 0x2100);
    BOOST_TEST(cpu.PC == 2);
}

BOOST_AUTO_TEST_CASE(a_supress) {
    cpu.A[2] = 0x2000;
    cpu.D[3] = 0x100;
    TEST::SET_W(0, 3 << 12 | 1 << 8 | 1 << 7 | 2 << 4);
    TEST::SET_W(2, 0x400);
    BOOST_TEST(ea_getaddr(6, 2, 0) == 0x0500);
    BOOST_TEST(cpu.PC == 4);
}

BOOST_AUTO_TEST_CASE(r_supress) {
    cpu.A[2] = 0x2000;
    cpu.D[3] = 0x100;
    TEST::SET_W(0, 3 << 12 | 1 << 8 | 1 << 6 | 2 << 4);
    TEST::SET_W(2, 0x400);
    BOOST_TEST(ea_getaddr(6, 2, 0) == 0x2400);
    BOOST_TEST(cpu.PC == 4);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(preindex)
BOOST_AUTO_TEST_CASE(no_od) {
    cpu.A[2] = 0x1000;
    cpu.D[3] = 0x100;
    TEST::SET_W(0, 3 << 12 | 1 << 8 | 2 << 4 | 1);
    TEST::SET_W(2, 0x400);
    TEST::SET_L(0x1500, 0x2000);
    BOOST_TEST(ea_getaddr(6, 2, 0) == 0x2000);
    BOOST_TEST(cpu.PC == 4);
}

BOOST_AUTO_TEST_CASE(od_w) {
    cpu.A[2] = 0x1000;
    cpu.D[3] = 0x100;
    TEST::SET_W(0, 3 << 12 | 1 << 8 | 2 << 4 | 2);
    TEST::SET_W(2, 0x400);
    TEST::SET_W(4, 0x300);
    TEST::SET_L(0x1500, 0x2000);
    BOOST_TEST(ea_getaddr(6, 2, 0) == 0x2300);
    BOOST_TEST(cpu.PC == 6);
}

BOOST_AUTO_TEST_CASE(od_l) {
    cpu.A[2] = 0x1000;
    cpu.D[3] = 0x100;
    TEST::SET_W(0, 3 << 12 | 1 << 8 | 2 << 4 | 3);
    TEST::SET_W(2, 0x400);
    TEST::SET_L(4, 0x10000);
    TEST::SET_L(0x1500, 0x2000);
    BOOST_TEST(ea_getaddr(6, 2, 0) == 0x12000);
    BOOST_TEST(cpu.PC == 8);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(postindex)
BOOST_AUTO_TEST_CASE(no_od) {
    cpu.A[2] = 0x1000;
    cpu.D[3] = 0x100;
    TEST::SET_W(0, 3 << 12 | 1 << 8 | 2 << 4 | 1 << 2 | 1);
    TEST::SET_W(2, 0x400);
    TEST::SET_L(0x1400, 0x500);
    BOOST_TEST(ea_getaddr(6, 2, 0) == 0x600);
    BOOST_TEST(cpu.PC == 4);
}

BOOST_AUTO_TEST_CASE(od_w) {
    cpu.A[2] = 0x1000;
    cpu.D[3] = 0x100;
    TEST::SET_W(0, 3 << 12 | 1 << 8 | 2 << 4 | 1 << 2 | 2);
    TEST::SET_W(2, 0x400);
    TEST::SET_W(4, 0x500);
    TEST::SET_L(0x1400, 0x2000);
    BOOST_TEST(ea_getaddr(6, 2, 0) == 0x2600);
    BOOST_TEST(cpu.PC == 6);
}

BOOST_AUTO_TEST_CASE(od_l) {
    cpu.A[2] = 0x1000;
    cpu.D[3] = 0x100;
    TEST::SET_W(0, 3 << 12 | 1 << 8 | 2 << 4 | 1 << 2 | 3);
    TEST::SET_W(2, 0x400);
    TEST::SET_L(4, 0x10000);
    TEST::SET_L(0x1400, 0x2000);
    BOOST_TEST(ea_getaddr(6, 2, 0) == 0x12100);
    BOOST_TEST(cpu.PC == 8);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(addr_w) {
    TEST::SET_W(0, 0x2000);
    BOOST_TEST(ea_getaddr(7, 0, 0) == 0x2000);
    BOOST_TEST(cpu.PC == 2);
}
BOOST_AUTO_TEST_CASE(addr_l) {
    TEST::SET_L(0, 0x200000);
    BOOST_TEST(ea_getaddr(7, 1, 0) == 0x200000);
    BOOST_TEST(cpu.PC == 4);
}

BOOST_AUTO_TEST_CASE(pc_d16) {
    cpu.PC = 0x1002;
    TEST::SET_W(0x1002, 0xfffe);
    BOOST_TEST(ea_getaddr(7, 2, 0) == 0x1000);
    BOOST_TEST(cpu.PC == 0x1004);
}

BOOST_AUTO_TEST_CASE(pcExt) {
    cpu.PC = 2;
    cpu.D[3] = 0x10;
    TEST::SET_W(2, 3 << 12 | 0x20);
    BOOST_TEST(ea_getaddr(7, 3, 0) == 0x32);
    BOOST_TEST(cpu.PC == 0x4);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
