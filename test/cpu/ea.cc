#define BOOST_TEST_DYN_LINK
#include "inline.hpp"
#include <boost/test/unit_test.hpp>
#include "test.hpp"
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(EA, Prepare)
BOOST_AUTO_TEST_SUITE(Read)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(D) {
    cpu.D[3] = 0x12345678;
    auto [v, i] = ea_read8(0, 3, 0);
    BOOST_TEST(static_cast<uint32_t>(v()) == 0x78);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(A) {
    BOOST_CHECK_THROW( ea_read8(1, 3, 0), DecodeError);
}

BOOST_AUTO_TEST_CASE(Addr) {
    RAM[0x1000] = 0x34;
    cpu.A[2] = 0x1000;
    auto [v, i] = ea_read8(2, 2, 0);
    BOOST_TEST(static_cast<uint32_t>(v()) == 0x34);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(Imm) {
    RAM[0x1003] = 0x34;
    cpu.A[2] = 0x1000;
    auto [v, i] = ea_read8(7, 4, 0x1002);
    BOOST_TEST(static_cast<uint32_t>(v()) == 0x34);
    BOOST_TEST(i == 2);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(D) {
    cpu.D[3] = 0x12345678;
    auto [v, i] = ea_read16(0, 3, 0);
    BOOST_TEST(v() == 0x5678);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(A) {
    cpu.A[4] = 0x12345678;
    auto [v, i] = ea_read16(1, 4, 0);
    BOOST_TEST(v() == 0x5678);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(Addr) {
    TEST::SET_W(0x1000, 0x3456);
    cpu.A[2] = 0x1000;
    auto [v, i] = ea_read16(2, 2, 0);
    BOOST_TEST(v() == 0x3456);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(Imm) {
    TEST::SET_W(0x1002, 0x3456);
    auto [v, i] = ea_read16(7, 4, 0x1002);
    BOOST_TEST(v() == 0x3456);
    BOOST_TEST(i == 2);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(D) {
    cpu.D[3] = 0x12345678;
    auto [v, i] = ea_read32(0, 3, 0);
    BOOST_TEST(v() == 0x12345678);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(A) {
    cpu.A[4] = 0x12345678;
    auto [v, i] = ea_read32(1, 4, 0);
    BOOST_TEST(v() == 0x12345678);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(Addr) {
    TEST::SET_L(0x1000, 0x3456789A);
    cpu.A[2] = 0x1000;
    auto [v, i] = ea_read32(2, 2, 0);
    BOOST_TEST(v() == 0x3456789A);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(Imm) {
    TEST::SET_L(0x1002, 0x3456789A);
    auto [v, i] = ea_read32(7, 4, 0x1002);
    BOOST_TEST(v() == 0x3456789A);
    BOOST_TEST(i == 4);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Write)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(D) {
    cpu.D[3] = 0x12345678;
    auto [v, i] = ea_write8(0, 3, 0);
    v(0xff);    
    BOOST_TEST(cpu.D[3] == 0x123456FF);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(A) {
    BOOST_CHECK_THROW( ea_write8(1, 3, 0), DecodeError);
}

BOOST_AUTO_TEST_CASE(Addr) {
    RAM[0x1000] = 0x34;
    cpu.A[2] = 0x1000;
    auto [v, i] = ea_write8(2, 2, 0);
     v(0xff);   
    BOOST_TEST(static_cast<uint32_t>(RAM[0x1000]) == 0xff);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(Imm) {
    BOOST_CHECK_THROW( ea_write8(7, 4, 0x1002), DecodeError);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(D) {
    cpu.D[3] = 0x12345678;
    auto [v, i] = ea_write16(0, 3, 0);
    v(0xABCD);    
    BOOST_TEST(cpu.D[3] == 0x1234ABCD);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(A) {
    cpu.A[4] = 0x12345678;
    auto [v, i] = ea_write16(1, 4, 0);
    v(0xfff3);
    BOOST_TEST(cpu.A[4] == 0xfffffff3);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(Addr) {
    RAM[0x1000] = 0x34;
    RAM[0x1001] = 0x56;
    cpu.A[2] = 0x1000;
    auto [v, i] = ea_write16(2, 2, 0);
    v(0x789A);
    BOOST_TEST(TEST::GET_W(0x1000) == 0x789A);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(Imm) {
    BOOST_CHECK_THROW( ea_write16(7, 4, 0x1002), DecodeError);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(D) {
    cpu.D[3] = 0x12345678;
    auto [v, i] = ea_write32(0, 3, 0);
    v(0xABCDEF01);    
    BOOST_TEST(cpu.D[3] == 0xABCDEF01);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(A) {
     cpu.A[4] = 0x12345678;
    auto [v, i] = ea_write32(1, 4, 0);
    v(0x20000000);
    BOOST_TEST(cpu.A[4] == 0x20000000);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(Addr) {
    RAM[0x1000] = 0x34;
    RAM[0x1001] = 0x56;
    RAM[0x1001] = 0x78;
    RAM[0x1001] = 0x9A;
    cpu.A[2] = 0x1000;
    auto [v, i] = ea_write32(2, 2, 0);
    v(0x01234567);
    BOOST_TEST(TEST::GET_L(0x1000) == 0x01234567);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(Imm) {
    BOOST_CHECK_THROW( ea_write32(7, 4, 0x1002), DecodeError);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(ADDR)
BOOST_AUTO_TEST_CASE(An) {
    cpu.A[2] = 0x1000;
    auto [a, o] = ea_addr(2, 2, 0, 0, true);
    BOOST_TEST( a() == 0x1000);
    BOOST_TEST( o == 0);
}
BOOST_AUTO_TEST_SUITE(Inc)
BOOST_AUTO_TEST_CASE(Zero) {
    BOOST_CHECK_THROW( ea_addr(3, 2, 0, 0, true), DecodeError);
}
BOOST_AUTO_TEST_CASE(NonZero) {
    cpu.A[2] = 0x1000;
    auto [a, o] = ea_addr(3, 2, 0, 4, true);
    BOOST_TEST( a() == 0x1000);
    BOOST_TEST( cpu.A[2] == 0x1004);
    BOOST_TEST( o == 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Dec)
BOOST_AUTO_TEST_CASE(Zero) {
    BOOST_CHECK_THROW( ea_addr(4, 2, 0, 0, true), DecodeError);
}
BOOST_AUTO_TEST_CASE(NonZero) {
    cpu.A[2] = 0x1004;
    auto [a, o] = ea_addr(4, 2, 0, 4, true);
    BOOST_TEST( a() == 0x1000);
    BOOST_TEST( cpu.A[2] == 0x1000);
    BOOST_TEST( o == 0);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(d16) {
    cpu.A[2] = 0x2000;
    TEST::SET_W(0, 0xfffe);
    auto [a, o] = ea_addr(5, 2, 0, 0, true);
    BOOST_TEST( a() == 0x1ffe);
    BOOST_TEST( o == 2);
}
BOOST_AUTO_TEST_SUITE(brief)
BOOST_AUTO_TEST_CASE(normal) {
    cpu.A[2] = 0x2000;
    cpu.D[3] = 0x10;
    TEST::SET_W(2, 3 << 12 | 0xF3);
    auto [a, o] = ea_addr(6, 2, 2, 0, true);
    BOOST_TEST( a() == 0x2003);
    BOOST_TEST( o == 2);
}

BOOST_AUTO_TEST_CASE(w) {
    cpu.A[2] = 0x2000;
    cpu.D[3] = 0xFFF8;
    TEST::SET_W(2, 3 << 12 | 1 << 11 | 0x10);
    auto [a, o] = ea_addr(6, 2, 2, 0, true);
    BOOST_TEST( a() == 0x2008);
    BOOST_TEST( o == 2);
}

BOOST_DATA_TEST_CASE(cc, bdata::xrange(4), value) {
    cpu.A[2] = 0x2000;
    cpu.D[3] = 0x10;
    TEST::SET_W(2, 3 << 12 | value << 9 | 0x08);
    auto [a, o] = ea_addr(6, 2, 2, 0, true);
    BOOST_TEST( a() == 0x2008 + (0x10 << value));
    BOOST_TEST( o == 2);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Ext)
BOOST_AUTO_TEST_CASE(Ill1) {
    TEST::SET_W(2, 0x0100);
    BOOST_CHECK_THROW( ea_addr(6, 2, 2, 0, true), DecodeError);
}
BOOST_AUTO_TEST_SUITE(direct)
BOOST_AUTO_TEST_CASE(full) {
    cpu.A[2] = 0x2000;
    cpu.D[3] = 0x100;
    TEST::SET_W(2, 3 << 12 | 1 << 8 | 2 << 4);
    TEST::SET_W(4, 0x400);
    auto [a, o] = ea_addr(6, 2, 2, 0, true);
    BOOST_TEST( a() == 0x2500);
    BOOST_TEST( o == 4);
}

BOOST_AUTO_TEST_CASE(bd_l) {
    cpu.A[2] = 0x2000;
    cpu.D[3] = 0x100;
    TEST::SET_W(2, 3 << 12 | 1 << 8 | 3 << 4);
    TEST::SET_L(4, 0x10000);
    auto [a, o] = ea_addr(6, 2, 2, 0, true);
    BOOST_TEST( a() == 0x12100);
    BOOST_TEST( o == 6);
}

BOOST_AUTO_TEST_CASE(no_bd) {
    cpu.A[2] = 0x2000;
    cpu.D[3] = 0x100;
    TEST::SET_W(2, 3 << 12 | 1 << 8 | 1 << 4);
    auto [a, o] = ea_addr(6, 2, 2, 0, true);
    BOOST_TEST( a() == 0x2100);
    BOOST_TEST( o == 2);
}

BOOST_AUTO_TEST_CASE(a_supress) {
    cpu.A[2] = 0x2000;
    cpu.D[3] = 0x100;
    TEST::SET_W(2, 3 << 12 | 1 << 8 | 1 << 7 | 2 << 4);
    TEST::SET_W(4, 0x400);
    auto [a, o] = ea_addr(6, 2, 2, 0, true);
    BOOST_TEST( a() == 0x0500);
    BOOST_TEST( o == 4);
}

BOOST_AUTO_TEST_CASE(r_supress) {
    cpu.A[2] = 0x2000;
    cpu.D[3] = 0x100;
    TEST::SET_W(2, 3 << 12 | 1 << 8 | 1 << 6 | 2 << 4);
    TEST::SET_W(4, 0x400);
    auto [a, o] = ea_addr(6, 2, 2, 0, true);
    BOOST_TEST( a() == 0x2400);
    BOOST_TEST( o == 4);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(preindex)
BOOST_AUTO_TEST_CASE(no_od) {
    cpu.A[2] = 0x1000;
    cpu.D[3] = 0x100;
    TEST::SET_W(2, 3 << 12 | 1 << 8 | 2 << 4 | 1);
    TEST::SET_W(4, 0x400);
    TEST::SET_L(0x1500, 0x2000);
    auto [a, o] = ea_addr(6, 2, 2, 0, true);
    BOOST_TEST( a() == 0x2000);
    BOOST_TEST( o == 4);
}

BOOST_AUTO_TEST_CASE(od_w) {
    cpu.A[2] = 0x1000;
    cpu.D[3] = 0x100;
    TEST::SET_W(2, 3 << 12 | 1 << 8 | 2 << 4 | 2);
    TEST::SET_W(4, 0x400);
    TEST::SET_W(6, 0x300);
    TEST::SET_L(0x1500, 0x2000);
    auto [a, o] = ea_addr(6, 2, 2, 0, true);
    BOOST_TEST( a() == 0x2300);
    BOOST_TEST( o == 6);
}

BOOST_AUTO_TEST_CASE(od_l) {
    cpu.A[2] = 0x1000;
    cpu.D[3] = 0x100;
    TEST::SET_W(2, 3 << 12 | 1 << 8 | 2 << 4 | 3);
    TEST::SET_W(4, 0x400);
    TEST::SET_L(6, 0x10000);
    TEST::SET_L(0x1500, 0x2000);
    auto [a, o] = ea_addr(6, 2, 2, 0, true);
    BOOST_TEST( a() == 0x12000);
    BOOST_TEST( o == 8);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(Ill2) {
    TEST::SET_W(2, 0x0104);
    BOOST_CHECK_THROW( ea_addr(6, 2, 2, 0, true), DecodeError);
}

BOOST_AUTO_TEST_SUITE(postindex)
BOOST_AUTO_TEST_CASE(no_od) {
    cpu.A[2] = 0x1000;
    cpu.D[3] = 0x100;
    TEST::SET_W(2, 3 << 12 | 1 << 8 | 2 << 4 | 1 << 2 | 1);
    TEST::SET_W(4, 0x400);
    TEST::SET_L(0x1400, 0x500);
    auto [a, o] = ea_addr(6, 2, 2, 0, true);
    BOOST_TEST( a() == 0x600);
    BOOST_TEST( o == 4);
}

BOOST_AUTO_TEST_CASE(od_w) {
    cpu.A[2] = 0x1000;
    cpu.D[3] = 0x100;
    TEST::SET_W(2, 3 << 12 | 1 << 8 | 2 << 4 |  1 << 2 | 2);
    TEST::SET_W(4, 0x400);
    TEST::SET_W(6, 0x500);
    TEST::SET_L(0x1400, 0x2000);
    auto [a, o] = ea_addr(6, 2, 2, 0, true);
    BOOST_TEST( a() == 0x2600);
    BOOST_TEST( o == 6);
}

BOOST_AUTO_TEST_CASE(od_l) {
    cpu.A[2] = 0x1000;
    cpu.D[3] = 0x100;
    TEST::SET_W(2, 3 << 12 | 1 << 8 | 2 << 4 | 1 << 2 | 3);
    TEST::SET_W(4, 0x400);
    TEST::SET_L(6, 0x10000);
    TEST::SET_L(0x1400, 0x2000);
    auto [a, o] = ea_addr(6, 2, 2, 0, true);
    BOOST_TEST( a() == 0x12100);
    BOOST_TEST( o == 8);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(Ill3) {
    TEST::SET_W(2, 0x0108);
    BOOST_CHECK_THROW( ea_addr(6, 2, 2, 0, true), DecodeError);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(addr_w) {
    TEST::SET_W(2, 0x2000);
    auto [a, o] = ea_addr(7, 0, 2, 0, true);
    BOOST_TEST( a() == 0x2000);
    BOOST_TEST( o == 2);
}
BOOST_AUTO_TEST_CASE(addr_l) {
    TEST::SET_L(2, 0x200000);
    auto [a, o] = ea_addr(7, 1, 2, 0, true);
    BOOST_TEST( a() == 0x200000);
    BOOST_TEST( o == 4);
}
BOOST_AUTO_TEST_SUITE(pc_d16)

BOOST_AUTO_TEST_CASE(r) {
    TEST::SET_W(0x1002, 0xfffe);
    auto [a, o] = ea_addr(7, 2, 0x1002, 0, false);
    BOOST_TEST( a() == 0x1000);
    BOOST_TEST( o == 2);
}

BOOST_AUTO_TEST_CASE(w) {
    TEST::SET_W(2, 0x0104);
    BOOST_CHECK_THROW( ea_addr(7, 2, 0x1002, 0, true), DecodeError);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(brief_pc)
BOOST_AUTO_TEST_CASE(write) {
    TEST::SET_W(2, 0x0104);
    BOOST_CHECK_THROW( ea_addr(7, 3, 0x1002, 0, true), DecodeError);
}
BOOST_AUTO_TEST_CASE(normal) {
    cpu.D[3] = 0x10;
    TEST::SET_W(2, 3 << 12 | 0x20);
    auto [a, o] = ea_addr(7, 3, 0x2, 0, false);
    BOOST_TEST( a() == 0x32);
    BOOST_TEST( o == 2);
}

BOOST_AUTO_TEST_CASE(w) {
    cpu.D[3] = 0xFFF8;
    TEST::SET_W(2, 3 << 12 | 1 << 11 | 0x10);
    auto [a, o] = ea_addr(7, 3, 2, 0, false);
    BOOST_TEST( a() == 0x000A);
    BOOST_TEST( o == 2);
}
BOOST_DATA_TEST_CASE(cc, bdata::xrange(4), value) {
    cpu.D[3] = 0x10;
    TEST::SET_W(2, 3 << 12 | value << 9 | 0x08);
    auto [a, o] = ea_addr(7, 3, 2, 0, false);
    BOOST_TEST( a() == 0x0A + (0x10 << value));
    BOOST_TEST( o == 2);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Ext)
BOOST_AUTO_TEST_CASE(Ill1) {
    TEST::SET_W(2, 0x0100);
    BOOST_CHECK_THROW( ea_addr(7, 3, 2, 0, false), DecodeError);
}
BOOST_AUTO_TEST_SUITE(direct)
BOOST_AUTO_TEST_CASE(full) {
    cpu.D[3] = 0x100;
    TEST::SET_W(2, 3 << 12 | 1 << 8 | 2 << 4);
    TEST::SET_W(4, 0x400);
    auto [a, o] = ea_addr(7, 3, 2, 0, false);
    BOOST_TEST( a() == 0x502);
    BOOST_TEST( o == 4);
}

BOOST_AUTO_TEST_CASE(bd_l) {
    cpu.D[3] = 0x100;
    TEST::SET_W(2, 3 << 12 | 1 << 8 | 3 << 4);
    TEST::SET_L(4, 0x10000);
    auto [a, o] = ea_addr(7, 3, 2, 0, false);
    BOOST_TEST( a() == 0x10102);
    BOOST_TEST( o == 6);
}

BOOST_AUTO_TEST_CASE(no_bd) {
    cpu.D[3] = 0x100;
    TEST::SET_W(2, 3 << 12 | 1 << 8 | 1 << 4);
    auto [a, o] = ea_addr(7, 3, 2, 0, false);
    BOOST_TEST( a() == 0x0102);
    BOOST_TEST( o == 2);
}

BOOST_AUTO_TEST_CASE(zeropc) {
    cpu.D[3] = 0x100;
    TEST::SET_W(2, 3 << 12 | 1 << 8 | 1 << 7 | 2 << 4);
    TEST::SET_W(4, 0x400);
    auto [a, o] = ea_addr(7, 3, 2, 0, false);
    BOOST_TEST( a() == 0x0500);
    BOOST_TEST( o == 4);
}
BOOST_AUTO_TEST_CASE(r_supress) {
    cpu.D[3] = 0x100;
    TEST::SET_W(2, 3 << 12 | 1 << 8 | 1 << 6 | 2 << 4);
    TEST::SET_W(4, 0x400);
    auto [a, o] = ea_addr(7, 3, 2, 0, false);
    BOOST_TEST( a() == 0x0402);
    BOOST_TEST( o == 4);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(preindex)
BOOST_AUTO_TEST_CASE(no_od) {
    cpu.D[3] = 0x100;
    TEST::SET_W(2, 3 << 12 | 1 << 8 | 2 << 4 | 1);
    TEST::SET_W(4, 0x400);
    TEST::SET_L(0x0502, 0x2000);
    auto [a, o] = ea_addr(7, 3, 2, 0, false);
    BOOST_TEST( a() == 0x2000);
    BOOST_TEST( o == 4);
}

BOOST_AUTO_TEST_CASE(od_w) {
    cpu.D[3] = 0x100;
    TEST::SET_W(2, 3 << 12 | 1 << 8 | 2 << 4 | 2);
    TEST::SET_W(4, 0x400);
    TEST::SET_W(6, 0x300);
    TEST::SET_L(0x0502, 0x2000);
    auto [a, o] = ea_addr(7, 3, 2, 0, false);
    BOOST_TEST( a() == 0x2300);
    BOOST_TEST( o == 6);
}

BOOST_AUTO_TEST_CASE(od_l) {
    cpu.D[3] = 0x100;
    TEST::SET_W(2, 3 << 12 | 1 << 8 | 2 << 4 | 3);
    TEST::SET_W(4, 0x400);
    TEST::SET_L(6, 0x10000);
    TEST::SET_L(0x0502, 0x2000);
    auto [a, o] = ea_addr(7, 3, 2, 0, false);
    BOOST_TEST( a() == 0x12000);
    BOOST_TEST( o == 8);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(Ill2) {
    TEST::SET_W(2, 0x0104);
    BOOST_CHECK_THROW( ea_addr(7, 3, 2, 0, false), DecodeError);
}

BOOST_AUTO_TEST_SUITE(postindex)

BOOST_AUTO_TEST_CASE(no_od) {
    cpu.D[3] = 0x100;
    TEST::SET_W(2, 3 << 12 | 1 << 8 | 2 << 4 | 1 << 2 | 1);
    TEST::SET_W(4, 0x400);
    TEST::SET_L(0x0402, 0x500);
    auto [a, o] = ea_addr(7, 3, 2, 0, false);
    BOOST_TEST( a() == 0x600);
    BOOST_TEST( o == 4);
}

BOOST_AUTO_TEST_CASE(od_w) {
    cpu.D[3] = 0x100;
    TEST::SET_W(2, 3 << 12 | 1 << 8 | 2 << 4 |  1 << 2 | 2);
    TEST::SET_W(4, 0x400);
    TEST::SET_W(6, 0x500);
    TEST::SET_L(0x0402, 0x2000);
    auto [a, o] = ea_addr(7, 3, 2, 0, false);
    BOOST_TEST( a() == 0x2600);
    BOOST_TEST( o == 6);
}

BOOST_AUTO_TEST_CASE(od_l) {
    cpu.D[3] = 0x100;
    TEST::SET_W(2, 3 << 12 | 1 << 8 | 2 << 4 | 1 << 2 | 3);
    TEST::SET_W(4, 0x400);
    TEST::SET_L(6, 0x10000);
    TEST::SET_L(0x0402, 0x2000);
    auto [a, o] = ea_addr(7, 3, 2, 0, false);
    BOOST_TEST( a() == 0x12100);
    BOOST_TEST( o == 8);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(Ill3) {
    TEST::SET_W(2, 0x0108);
    BOOST_CHECK_THROW( ea_addr(7, 3, 2, 0, false), DecodeError);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
