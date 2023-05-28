#define BOOST_TEST_DYN_LINK
#include "inline.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
BOOST_AUTO_TEST_SUITE(bitfield)
BOOST_AUTO_TEST_SUITE(extract)
BOOST_AUTO_TEST_CASE(reg_nonwrap) {
    BOOST_TEST(BF_D(0x12345678, 4, 8) == 0x23000000);
}

BOOST_AUTO_TEST_CASE(reg_wrap) {
    BOOST_TEST(BF_D(0x12345678, 28, 8) == 0x81000000);
}

BOOST_AUTO_TEST_CASE(mem_1) {
    RAM[0x1000] = 0x23;
    BOOST_TEST(BF_M(0x1000, 4, 4) == 0x30000000);
}

BOOST_AUTO_TEST_CASE(mem_2) {
    RAM[0x1000] = 0x23;
    RAM[0x1001] = 0x45;
    BOOST_TEST(BF_M(0x1000, 4, 8) == 0x34000000);
}

BOOST_AUTO_TEST_CASE(mem_3) {
    RAM[0x1000] = 0x23;
    RAM[0x1001] = 0x45;
    RAM[0x1002] = 0x67;
    BOOST_TEST(BF_M(0x1000, 4, 16) == 0x34560000);
}

BOOST_AUTO_TEST_CASE(mem_4) {
    RAM[0x1000] = 0x23;
    RAM[0x1001] = 0x45;
    RAM[0x1002] = 0x67;
    RAM[0x1003] = 0x89;
    BOOST_TEST(BF_M(0x1000, 4, 24) == 0x34567800);
}

BOOST_AUTO_TEST_CASE(mem_5) {
    RAM[0x1000] = 0x23;
    RAM[0x1001] = 0x45;
    RAM[0x1002] = 0x67;
    RAM[0x1003] = 0x89;
    RAM[0x1004] = 0xAB;
    BOOST_TEST(BF_M(0x1000, 4, 32) == 0x3456789A);
}

BOOST_AUTO_TEST_CASE(offset_over) {
    RAM[0x1002] = 0x23;
    BOOST_TEST(BF_M(0x1000, 20, 4) == 0x30000000);
}
BOOST_AUTO_TEST_CASE(offset_neg) {
    RAM[0x1000] = 0x23;
    BOOST_TEST(BF_M(0x1002, -12, 4) == 0x30000000);
}
BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_CASE(bfchg_d) {
    uint32_t v = 0x12345678;
    BFCHG_D(v, 28, 8);
    BOOST_TEST(v == 0xE2345677);
}

BOOST_AUTO_TEST_SUITE(bfchg_m)
BOOST_AUTO_TEST_CASE(mem_1) {
    RAM[0x1000] = 0x23;
    BFCHG_M(0x1000, 4, 4);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1000]) == 0x2C);
}
BOOST_AUTO_TEST_CASE(mem_2) {
    RAM[0x1000] = 0x23;
    RAM[0x1001] = 0x45;
    BFCHG_M(0x1000, 4, 8);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1000]) == 0x2C);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1001]) == 0xB5);
}

BOOST_AUTO_TEST_CASE(mem_3) {
    RAM[0x1000] = 0x23;
    RAM[0x1001] = 0x45;
    RAM[0x1002] = 0x67;
    BFCHG_M(0x1000, 4, 16);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1000]) == 0x2C);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1001]) == 0xBA);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1002]) == 0x97);
}

BOOST_AUTO_TEST_CASE(mem_4) {
    RAM[0x1000] = 0x23;
    RAM[0x1001] = 0x45;
    RAM[0x1002] = 0x67;
    RAM[0x1003] = 0x89;
    BFCHG_M(0x1000, 4, 24);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1000]) == 0x2C);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1001]) == 0xBA);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1002]) == 0x98);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1003]) == 0x79);
}

BOOST_AUTO_TEST_CASE(mem_5) {
    RAM[0x1000] = 0x23;
    RAM[0x1001] = 0x45;
    RAM[0x1002] = 0x67;
    RAM[0x1003] = 0x89;
    RAM[0x1004] = 0xAB;
    BFCHG_M(0x1000, 4, 32);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1000]) == 0x2C);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1001]) == 0xBA);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1002]) == 0x98);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1003]) == 0x76);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1004]) == 0x5B);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(bfclr_d) {
    uint32_t v = 0x12345678;
    BFCLR_D(v, 28, 8);
    BOOST_TEST(v == 0x02345670);
}
BOOST_AUTO_TEST_SUITE(bfclr_m)
BOOST_AUTO_TEST_CASE(mem_1) {
    RAM[0x1000] = 0x23;
    BFCLR_M(0x1000, 4, 4);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1000]) == 0x20);
}
BOOST_AUTO_TEST_CASE(mem_2) {
    RAM[0x1000] = 0x23;
    RAM[0x1001] = 0x45;
    BFCLR_M(0x1000, 4, 8);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1000]) == 0x20);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1001]) == 0x05);
}

BOOST_AUTO_TEST_CASE(mem_3) {
    RAM[0x1000] = 0x23;
    RAM[0x1001] = 0x45;
    RAM[0x1002] = 0x67;
    BFCLR_M(0x1000, 4, 16);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1000]) == 0x20);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1001]) == 0x0);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1002]) == 0x07);
}

BOOST_AUTO_TEST_CASE(mem_4) {
    RAM[0x1000] = 0x23;
    RAM[0x1001] = 0x45;
    RAM[0x1002] = 0x67;
    RAM[0x1003] = 0x89;
    BFCLR_M(0x1000, 4, 24);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1000]) == 0x20);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1001]) == 0x0);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1002]) == 0x0);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1003]) == 0x09);
}

BOOST_AUTO_TEST_CASE(mem_5) {
    RAM[0x1000] = 0x23;
    RAM[0x1001] = 0x45;
    RAM[0x1002] = 0x67;
    RAM[0x1003] = 0x89;
    RAM[0x1004] = 0xAB;
    BFCLR_M(0x1000, 4, 32);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1000]) == 0x20);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1001]) == 0x0);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1002]) == 0x0);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1003]) == 0x0);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1004]) == 0x0B);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(bfset_d) {
    uint32_t v = 0x12345678;
    BFSET_D(v, 28, 8);
    BOOST_TEST(v == 0xF234567F);
}
BOOST_AUTO_TEST_SUITE(bfset_m)
BOOST_AUTO_TEST_CASE(mem_1) {
    RAM[0x1000] = 0x23;
    BFSET_M(0x1000, 4, 4);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1000]) == 0x2F);
}
BOOST_AUTO_TEST_CASE(mem_2) {
    RAM[0x1000] = 0x23;
    RAM[0x1001] = 0x45;
    BFSET_M(0x1000, 4, 8);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1000]) == 0x2F);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1001]) == 0xF5);
}

BOOST_AUTO_TEST_CASE(mem_3) {
    RAM[0x1000] = 0x23;
    RAM[0x1001] = 0x45;
    RAM[0x1002] = 0x67;
    BFSET_M(0x1000, 4, 16);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1000]) == 0x2F);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1001]) == 0xFF);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1002]) == 0xF7);
}

BOOST_AUTO_TEST_CASE(mem_4) {
    RAM[0x1000] = 0x23;
    RAM[0x1001] = 0x45;
    RAM[0x1002] = 0x67;
    RAM[0x1003] = 0x89;
    BFSET_M(0x1000, 4, 24);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1000]) == 0x2F);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1001]) == 0xFF);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1002]) == 0xFF);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1003]) == 0xF9);
}

BOOST_AUTO_TEST_CASE(mem_5) {
    RAM[0x1000] = 0x23;
    RAM[0x1001] = 0x45;
    RAM[0x1002] = 0x67;
    RAM[0x1003] = 0x89;
    RAM[0x1004] = 0xAB;
    BFSET_M(0x1000, 4, 32);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1000]) == 0x2F);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1001]) == 0xFF);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1002]) == 0xFF);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1003]) == 0xFF);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1004]) == 0xFB);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(bfextu_d) {
    BOOST_TEST(BFEXTU_D(0x12345678, 28, 8) == 0x81);
}
BOOST_AUTO_TEST_CASE(bfextu_m) {
    RAM[0x1000] = 0x89;
    RAM[0x1001] = 0xAB;
    BOOST_TEST(BFEXTU_M(0x1000, 4, 8) == 0x9A);
}

BOOST_AUTO_TEST_CASE(bfexts_d) {
    BOOST_TEST(BFEXTS_D(0x12345678, 28, 8) == 0xffffff81);
}
BOOST_AUTO_TEST_CASE(bfexts_m) {
    RAM[0x1000] = 0x89;
    RAM[0x1001] = 0xAB;
    BOOST_TEST(BFEXTS_M(0x1000, 4, 8) == 0xffffff9A);
}

BOOST_AUTO_TEST_SUITE(bfffo_d)
BOOST_AUTO_TEST_CASE(found) { BOOST_TEST(BFFFO_D(0x800000, 4, 8) == 8); }
BOOST_AUTO_TEST_CASE(not_found) { BOOST_TEST(BFFFO_D(0x800000, 12, 8) == 20); }
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(bfffo_m)
BOOST_AUTO_TEST_CASE(found) {
    RAM[0x1000] = 0x00;
    RAM[0x1001] = 0x40;
    BOOST_TEST(BFFFO_M(0x1000, 4, 8) == 9);
}
BOOST_AUTO_TEST_CASE(not_found) {
    RAM[0x1000] = 0x00;
    RAM[0x1001] = 0x04;
    BOOST_TEST(BFFFO_M(0x1000, 4, 8) == 12);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(bfins_d)
BOOST_AUTO_TEST_CASE(reg_nonwrap) {
    BOOST_TEST(BFINS_D(0x12345678, 4, 8, 0x9A) == 0x19A45678);
}

BOOST_AUTO_TEST_CASE(reg_wrap) {
    BOOST_TEST(BFINS_D(0x12345678, 28, 8, 0x9A) == 0xA2345679);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(bfins_m)
BOOST_AUTO_TEST_CASE(mem_1) {
    RAM[0x1000] = 0x23;
    BFINS_M(0x1000, 4, 4, 0x9);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1000]) == 0x29);
}

BOOST_AUTO_TEST_CASE(mem_2) {
    RAM[0x1000] = 0x23;
    RAM[0x1001] = 0x45;
    BFINS_M(0x1000, 4, 8, 0x9A);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1000]) == 0x29);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1001]) == 0xA5);
}

BOOST_AUTO_TEST_CASE(mem_3) {
    RAM[0x1000] = 0x23;
    RAM[0x1001] = 0x45;
    RAM[0x1002] = 0x67;
    BFINS_M(0x1000, 4, 16, 0x9ABC);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1000]) == 0x29);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1001]) == 0xAB);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1002]) == 0xC7);
}

BOOST_AUTO_TEST_CASE(mem_4) {
    RAM[0x1000] = 0x23;
    RAM[0x1001] = 0x45;
    RAM[0x1002] = 0x67;
    RAM[0x1003] = 0x89;
    BFINS_M(0x1000, 4, 24, 0x9ABCDE);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1000]) == 0x29);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1001]) == 0xAB);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1002]) == 0xCD);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1003]) == 0xE9);
}

BOOST_AUTO_TEST_CASE(mem_5) {
    RAM[0x1000] = 0x23;
    RAM[0x1001] = 0x45;
    RAM[0x1002] = 0x67;
    RAM[0x1003] = 0x89;
    RAM[0x1004] = 0xAB;
    BFINS_M(0x1000, 4, 32, 0x9ABCDEF0);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1000]) == 0x29);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1001]) == 0xAB);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1002]) == 0xCD);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1003]) == 0xEF);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1004]) == 0x0B);
}

BOOST_AUTO_TEST_CASE(offset_over) {
    RAM[0x1002] = 0x23;
    BFINS_M(0x1000, 20, 4, 0x4);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1002]) == 0x24);
}

BOOST_AUTO_TEST_CASE(offset_neg) {
    RAM[0x1000] = 0x23;
    BFINS_M(0x1002, -12, 4, 0x4);
    BOOST_TEST(static_cast<uint8_t>(RAM[0x1000]) == 0x24);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()