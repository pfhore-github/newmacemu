#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_MOVE16 {
    F_MOVE16() {
        // MOVE16 (%A3)+, (%A2)+
        TEST::SET_W(0, 0173040 | 3);
        TEST::SET_W(2, 0x8000 | 2 << 12);
        TEST::SET_W(4, TEST_BREAK);

        // MOVE16 #0x1203, (%A2)
        TEST::SET_W(6, 0173030 | 2);
        TEST::SET_L(8, 0x1203);
        TEST::SET_W(12, TEST_BREAK);

        // MOVE16 #0x1203, (%A2)+
        TEST::SET_W(14, 0173010 | 2);
        TEST::SET_L(16, 0x1203);
        TEST::SET_W(20, TEST_BREAK);

        // MOVE16 (%A2), #0x1303
        TEST::SET_W(22, 0173020 | 2);
        TEST::SET_L(24, 0x1303);
        TEST::SET_W(28, TEST_BREAK);

        // MOVE16 (%A2)+, #0x1303
        TEST::SET_W(30, 0173000 | 2);
        TEST::SET_L(32, 0x1303);
        TEST::SET_W(36, TEST_BREAK);

        jit_compile(0, 38);
    }};
BOOST_FIXTURE_TEST_SUITE(MOVE16, Prepare, *boost::unit_test::fixture<F_MOVE16>())


BOOST_AUTO_TEST_CASE(reg2reg) {
    cpu.A[3] = 0x1207;
    cpu.A[2] = 0x1303;
    TEST::SET_L(0x1200, 0x01234567);
    TEST::SET_L(0x1204, 0x89ABCDEF);
    TEST::SET_L(0x1208, 0xFEDCBA98);
    TEST::SET_L(0x120C, 0x76543210);
    run_test(0);
    BOOST_TEST(cpu.A[3] == 0x1217);
    BOOST_TEST(cpu.A[2] == 0x1313);
    BOOST_TEST(TEST::GET_L(0x1300) == 0x01234567);
    BOOST_TEST(TEST::GET_L(0x1304) == 0x89ABCDEF);
    BOOST_TEST(TEST::GET_L(0x1308) == 0xFEDCBA98);
    BOOST_TEST(TEST::GET_L(0x130C) == 0x76543210);
}

BOOST_AUTO_TEST_CASE(imm2reg) {
    cpu.A[2] = 0x1307;
    TEST::SET_L(0x1200, 0x01234567);
    TEST::SET_L(0x1204, 0x89ABCDEF);
    TEST::SET_L(0x1208, 0xFEDCBA98);
    TEST::SET_L(0x120C, 0x76543210);
    run_test(6);
    BOOST_TEST(cpu.A[2] == 0x1307);
    BOOST_TEST(TEST::GET_L(0x1300) == 0x01234567);
    BOOST_TEST(TEST::GET_L(0x1304) == 0x89ABCDEF);
    BOOST_TEST(TEST::GET_L(0x1308) == 0xFEDCBA98);
    BOOST_TEST(TEST::GET_L(0x130C) == 0x76543210);
}

BOOST_AUTO_TEST_CASE(imm2incr) {
    cpu.A[2] = 0x1307;
    TEST::SET_L(0x1200, 0x01234567);
    TEST::SET_L(0x1204, 0x89ABCDEF);
    TEST::SET_L(0x1208, 0xFEDCBA98);
    TEST::SET_L(0x120C, 0x76543210);
    run_test(14);
    BOOST_TEST(cpu.A[2] == 0x1317);
    BOOST_TEST(TEST::GET_L(0x1300) == 0x01234567);
    BOOST_TEST(TEST::GET_L(0x1304) == 0x89ABCDEF);
    BOOST_TEST(TEST::GET_L(0x1308) == 0xFEDCBA98);
    BOOST_TEST(TEST::GET_L(0x130C) == 0x76543210);
}

BOOST_AUTO_TEST_CASE(reg2imm) {
    cpu.A[2] = 0x1207;
    TEST::SET_L(0x1200, 0x01234567);
    TEST::SET_L(0x1204, 0x89ABCDEF);
    TEST::SET_L(0x1208, 0xFEDCBA98);
    TEST::SET_L(0x120C, 0x76543210);
    run_test(22);
    BOOST_TEST(cpu.A[2] == 0x1207);
    BOOST_TEST(TEST::GET_L(0x1300) == 0x01234567);
    BOOST_TEST(TEST::GET_L(0x1304) == 0x89ABCDEF);
    BOOST_TEST(TEST::GET_L(0x1308) == 0xFEDCBA98);
    BOOST_TEST(TEST::GET_L(0x130C) == 0x76543210);
}

BOOST_AUTO_TEST_CASE(incr2imm) {
    cpu.A[2] = 0x1207;
    TEST::SET_L(0x1200, 0x01234567);
    TEST::SET_L(0x1204, 0x89ABCDEF);
    TEST::SET_L(0x1208, 0xFEDCBA98);
    TEST::SET_L(0x120C, 0x76543210);
    run_test(30);
    BOOST_TEST(cpu.A[2] == 0x1217);
    BOOST_TEST(TEST::GET_L(0x1300) == 0x01234567);
    BOOST_TEST(TEST::GET_L(0x1304) == 0x89ABCDEF);
    BOOST_TEST(TEST::GET_L(0x1308) == 0xFEDCBA98);
    BOOST_TEST(TEST::GET_L(0x130C) == 0x76543210);
}
BOOST_AUTO_TEST_SUITE_END()
