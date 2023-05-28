#define BOOST_TEST_DYN_LINK
#include "inline.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(and_b) {
    cpu.V = cpu.C = true;
    BOOST_TEST(AND_B(0xFE, 0x33) == 0x32);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(and_w) {
    cpu.V = cpu.C = true;
    BOOST_TEST(AND_W(0xFEFE, 0x3333) == 0x3232);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(and_l) {
    cpu.V = cpu.C = true;
    BOOST_TEST(AND_L(0xFEFEFEFE, 0x33333333) == 0x32323232);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(or_b) {
    cpu.V = cpu.C = true;
    BOOST_TEST(OR_B(0x23, 0x41) == 0x63);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(or_w) {
    cpu.V = cpu.C = true;
    BOOST_TEST(OR_W(0x2323, 0x4141) == 0x6363);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(or_l) {
    cpu.V = cpu.C = true;
    BOOST_TEST(OR_L(0x23232323, 0x41414141) == 0x63636363);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(xor_b) {
    cpu.V = cpu.C = true;
    BOOST_TEST(XOR_B(0x21, 0x61) == 0x40);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(xor_w) {
    cpu.V = cpu.C = true;
    BOOST_TEST(XOR_W(0x2121, 0x6161) == 0x4040);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_CASE(xor_l) {
    cpu.V = cpu.C = true;
    BOOST_TEST(XOR_L(0x21212121, 0x61616161) == 0x40404040);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
}

BOOST_DATA_TEST_CASE(btst_b, boost::unit_test::data::xrange(2), z) {
    BTST_B(z ? 0 : 1 << 4, 4);
    BOOST_TEST(cpu.Z == z);
}

BOOST_DATA_TEST_CASE(btst_l, boost::unit_test::data::xrange(2), z) {
    BTST_L(z ? 0 : 1 << 13, 13);
    BOOST_TEST(cpu.Z == z);
}

BOOST_AUTO_TEST_SUITE(bchg_b)
BOOST_AUTO_TEST_CASE(t) { BOOST_TEST(BCHG_B(0xff, 3) == 0xF7); }

BOOST_AUTO_TEST_CASE(f) { BOOST_TEST(BCHG_B(0, 3) == 0x08); }
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(bchg_l)
BOOST_AUTO_TEST_CASE(t) { BOOST_TEST(BCHG_L(0xffffffff, 20) == 0xFFEFFFFF); }

BOOST_AUTO_TEST_CASE(f) { BOOST_TEST(BCHG_L(0, 20) == 0x100000); }
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(bclr_b) { BOOST_TEST(BCLR_B(0xff, 3) == 0xF7); }

BOOST_AUTO_TEST_CASE(bclr_l) {
    BOOST_TEST(BCLR_L(0xffffffff, 20) == 0xFFEFFFFF);
}

BOOST_AUTO_TEST_CASE(bset_b) { BOOST_TEST(BSET_B(0, 3) == 0x08); }

BOOST_AUTO_TEST_CASE(bset_l) { BOOST_TEST(BSET_L(0, 20) == 0x100000); }

BOOST_AUTO_TEST_SUITE(asr_b)
BOOST_AUTO_TEST_CASE(positive) {
    cpu.V = true;
    cpu.X = false;
    BOOST_TEST(ASR_B(9, 1) == 4);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_CASE(negative) {
    cpu.V = true;
    BOOST_TEST(static_cast<int32_t>(ASR_B(-9, 1)) == -5);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(s0) {
    cpu.V = true;
    cpu.C = false;
    cpu.X = true;
    BOOST_TEST(static_cast<int32_t>(ASR_B(5, 0)) == 5);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(asr_w)
BOOST_AUTO_TEST_CASE(positive) {
    cpu.V = true;
    BOOST_TEST(ASR_W(513, 1) == 256);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_CASE(negative) {
    cpu.V = true;
    BOOST_TEST(static_cast<int32_t>(ASR_W(-501, 1)) == -251);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_CASE(s0) {
    cpu.V = true;
    cpu.C = false;
    cpu.X = true;
    BOOST_TEST(static_cast<int32_t>(ASR_W(513, 0)) == 513);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(asr_l)
BOOST_AUTO_TEST_CASE(positive) {
    cpu.V = true;
    BOOST_TEST(ASR_L(0x10001, 1) == 0x8000);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_CASE(negative) {
    cpu.V = true;
    BOOST_TEST(static_cast<int32_t>(ASR_L(0x80000001, 1)) == 0xC0000000);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_CASE(s0) {
    cpu.V = true;
    cpu.C = false;
    cpu.X = true;
    BOOST_TEST(static_cast<int32_t>(ASR_L(0x1000000, 0)) == 0x1000000);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_SUITE_END()



BOOST_AUTO_TEST_SUITE(asl_b)
BOOST_AUTO_TEST_CASE(regular) {
    cpu.V = true;
    cpu.X = false;
    BOOST_TEST(ASL_B(4, 2) == 16);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(positive_v) {
    cpu.V = false;
    BOOST_TEST(static_cast<int32_t>(ASL_B(0x40, 1)) == static_cast<int8_t>(0x80));
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(positive_v2) {
    cpu.V = false;
    BOOST_TEST(static_cast<int32_t>(ASL_B(0x40, 2)) == 0);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(negative_v) {
    cpu.V = false;
    BOOST_TEST(static_cast<int32_t>(ASL_B(0x80, 1)) == 0);
    BOOST_TEST(cpu.V);
    BOOST_TEST(cpu.C);
}
BOOST_AUTO_TEST_CASE(negative_v2) {
    cpu.V = false;
    BOOST_TEST(static_cast<int32_t>(ASL_B(0xA0, 2)) == static_cast<int8_t>(0x80));
    BOOST_TEST(cpu.V);
    BOOST_TEST(!cpu.C);
}
BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(asl_w)
BOOST_AUTO_TEST_CASE(regular) {
    cpu.V = true;
    cpu.X = false;
    BOOST_TEST(ASL_W(400, 2) == 1600);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(positive_v) {
    cpu.V = false;
    BOOST_TEST(ASL_W(0x4000, 1) == static_cast<int16_t>(0x8000));
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(positive_v2) {
    cpu.V = false;
    BOOST_TEST(ASL_W(0x4000, 2) == 0);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(negative_v) {
    cpu.V = false;
    BOOST_TEST(ASL_W(0x8000, 1) == 0);
    BOOST_TEST(cpu.V);
    BOOST_TEST(cpu.C);
}
BOOST_AUTO_TEST_CASE(negative_v2) {
    cpu.V = false;
    BOOST_TEST(ASL_W(0xA000, 2) == static_cast<int16_t>(0x8000));
    BOOST_TEST(cpu.V);
    BOOST_TEST(!cpu.C);
}
BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(asl_l)
BOOST_AUTO_TEST_CASE(regular) {
    cpu.V = true;
    cpu.X = false;
    BOOST_TEST(ASL_L(40000, 2) == 160000);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(positive_v) {
    cpu.V = false;
    BOOST_TEST(ASL_L(0x40000000, 1) == static_cast<int32_t>(0x80000000));
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(positive_v2) {
    cpu.V = false;
    BOOST_TEST(ASL_L(0x40000000, 2) == 0);
    BOOST_TEST(cpu.V);
}
BOOST_AUTO_TEST_CASE(negative_v) {
    cpu.V = false;
    BOOST_TEST(ASL_L(0x80000000, 1) == 0);
    BOOST_TEST(cpu.V);
    BOOST_TEST(cpu.C);
}
BOOST_AUTO_TEST_CASE(negative_v2) {
    cpu.V = false;
    BOOST_TEST(ASL_L(0xA0000000, 2) == static_cast<int32_t>(0x80000000));
    BOOST_TEST(cpu.V);
    BOOST_TEST(!cpu.C);
}
BOOST_AUTO_TEST_SUITE_END()



BOOST_AUTO_TEST_SUITE(lsr_b)
BOOST_AUTO_TEST_CASE(positive) {
    cpu.V = true;
    cpu.X = false;
    BOOST_TEST(LSR_B(9, 1) == 4);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_CASE(negative) {
    cpu.V = true;
    BOOST_TEST(static_cast<int32_t>(LSR_B(0x81, 1)) == 0x40);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_AUTO_TEST_CASE(s0) {
    cpu.V = true;
    cpu.C = false;
    cpu.X = true;
    BOOST_TEST(static_cast<int32_t>(LSR_B(5, 0)) == 5);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(lsr_w)
BOOST_AUTO_TEST_CASE(positive) {
    cpu.V = true;
    BOOST_TEST(LSR_W(513, 1) == 256);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_CASE(negative) {
    cpu.V = true;
    BOOST_TEST(static_cast<int32_t>(LSR_W(0x8001, 1)) == 0x4000);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_CASE(s0) {
    cpu.V = true;
    cpu.C = false;
    cpu.X = true;
    BOOST_TEST(static_cast<int32_t>(LSR_W(0x8001, 0)) == 0x8001);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(lsr_l)
BOOST_AUTO_TEST_CASE(positive) {
    cpu.V = true;
    BOOST_TEST(LSR_L(0x10001, 1) == 0x8000);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_CASE(negative) {
    cpu.V = true;
    BOOST_TEST(static_cast<int32_t>(LSR_L(0x80000001, 1)) == 0x40000000);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_CASE(s0) {
    cpu.V = true;
    cpu.C = false;
    cpu.X = true;
    BOOST_TEST(static_cast<int32_t>(LSR_L(0x1000000, 0)) == 0x1000000);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(cpu.X);
}
BOOST_AUTO_TEST_SUITE_END()



BOOST_AUTO_TEST_SUITE(lsl_b)
BOOST_AUTO_TEST_CASE(regular) {
    cpu.V = true;
    cpu.X = false;
    BOOST_TEST(LSL_B(4, 2) == 16);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(c) {
    cpu.C = false;
    BOOST_TEST(static_cast<uint32_t>(LSL_B(0x80, 1)) == 0);
    BOOST_TEST(cpu.C);
}
BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(lsl_w)
BOOST_AUTO_TEST_CASE(regular) {
    cpu.V = true;
    cpu.X = false;
    BOOST_TEST(LSL_W(400, 2) == 1600);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(c) {
    cpu.C = false;
    BOOST_TEST(LSL_W(0x8000, 1) == 0);
    BOOST_TEST(cpu.C);
}
BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(lsl_l)
BOOST_AUTO_TEST_CASE(regular) {
    cpu.V = true;
    cpu.X = false;
    BOOST_TEST(LSL_L(40000, 2) == 160000);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(!cpu.X);
}

BOOST_AUTO_TEST_CASE(c) {
    cpu.C = false;
    BOOST_TEST(LSL_L(0x80000000, 1) == 0);
    BOOST_TEST(cpu.C);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(roxr_b)
BOOST_DATA_TEST_CASE(regular, boost::unit_test::data::xrange(2), x) {
    cpu.V = true;
    cpu.X = x;
    BOOST_TEST(ROXR_B(0x43, 2) == ((x << 6) | 0x90));
    BOOST_TEST(!cpu.V);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_DATA_TEST_CASE(zero, boost::unit_test::data::xrange(2), x) {
    cpu.X = x;
    BOOST_TEST(ROXR_B(0x42, 0) == 0x42);
    BOOST_TEST(cpu.C == x);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(roxr_w)
BOOST_DATA_TEST_CASE(regular, boost::unit_test::data::xrange(2), x) {
    cpu.V = true;
    cpu.X = x;
    BOOST_TEST(ROXR_W(0x4003, 2) == ((x << 14) | 0x9000));
    BOOST_TEST(!cpu.V);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_DATA_TEST_CASE(zero, boost::unit_test::data::xrange(2), x) {
    cpu.X = x;
    BOOST_TEST(ROXR_W(0x4002, 0) == 0x4002);
    BOOST_TEST(cpu.C == x);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(roxr_l)
BOOST_DATA_TEST_CASE(regular, boost::unit_test::data::xrange(2), x) {
    cpu.V = true;
    cpu.X = x;
    BOOST_TEST(ROXR_L(0x40000003, 2) == ((x << 30) | 0x90000000));
    BOOST_TEST(!cpu.V);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_DATA_TEST_CASE(zero, boost::unit_test::data::xrange(2), x) {
    cpu.X = x;
    BOOST_TEST(ROXR_L(0x4000002, 0) == 0x4000002);
    BOOST_TEST(cpu.C == x);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(roxl_b)
BOOST_DATA_TEST_CASE(regular, boost::unit_test::data::xrange(2), x) {
    cpu.V = true;
    cpu.X = x;
    BOOST_TEST(ROXL_B(0xE0, 2) == ((x << 1) | 0x81));
    BOOST_TEST(!cpu.V);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_DATA_TEST_CASE(zero, boost::unit_test::data::xrange(2), x) {
    cpu.X = x;
    BOOST_TEST(ROXL_B(0x42, 0) == 0x42);
    BOOST_TEST(cpu.C == x);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(roxl_w)
BOOST_DATA_TEST_CASE(regular, boost::unit_test::data::xrange(2), x) {
    cpu.V = true;
    cpu.X = x;
    BOOST_TEST(ROXL_W(0x6000, 2) == ((x << 1) | 0x8000));
    BOOST_TEST(!cpu.V);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_DATA_TEST_CASE(zero, boost::unit_test::data::xrange(2), x) {
    cpu.X = x;
    BOOST_TEST(ROXL_W(0x6000, 0) == 0x6000);
    BOOST_TEST(cpu.C == x);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(roxl_l)
BOOST_DATA_TEST_CASE(regular, boost::unit_test::data::xrange(2), x) {
    cpu.V = true;
    cpu.X = x;
    BOOST_TEST(ROXL_L(0x60000000, 2) == ((x << 1) | 0x80000000));
    BOOST_TEST(!cpu.V);
    BOOST_TEST(cpu.C);
    BOOST_TEST(cpu.X);
}

BOOST_DATA_TEST_CASE(zero, boost::unit_test::data::xrange(2), x) {
    cpu.X = x;
    BOOST_TEST(ROXL_L(0x60000000, 0) == 0x60000000);
    BOOST_TEST(cpu.C == x);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(ror_b)
BOOST_AUTO_TEST_CASE(regular) {
    cpu.V = true;
    BOOST_TEST(ROR_B(0xC7, 2) ==  0xF1);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(zero) {
    BOOST_TEST(ROR_B(0x42, 0) == 0x42);
    BOOST_TEST(!cpu.C );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(ror_w)
BOOST_AUTO_TEST_CASE(regular) {
    cpu.V = true;
    BOOST_TEST(ROR_W(0x4003, 2) == 0xD000);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(zero) {
    BOOST_TEST(ROR_W(0x4002, 0) == 0x4002);
    BOOST_TEST(!cpu.C );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ror_l)
BOOST_AUTO_TEST_CASE(regular) {
    cpu.V = true;
    BOOST_TEST(ROR_L(0x40000003, 2) == 0xD0000000);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(zero) {
    BOOST_TEST(ROR_L(0x40000002, 0) == 0x40000002);
    BOOST_TEST(!cpu.C );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(rol_b)
BOOST_AUTO_TEST_CASE(regular) {
    cpu.V = true;
    BOOST_TEST(ROL_B(0xC1, 2) == 0x7);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(zero) {
    BOOST_TEST(ROL_B(0x42, 0) == 0x42);
    BOOST_TEST(!cpu.C );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(rol_w)
BOOST_AUTO_TEST_CASE(regular) {
    cpu.V = true;
    BOOST_TEST(ROL_W(0xC001, 2) == 0x7);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(zero) {
    BOOST_TEST(ROL_W(0x6000, 0) == 0x6000);
    BOOST_TEST(!cpu.C);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(rol_l)
BOOST_AUTO_TEST_CASE(regular) {
    cpu.V = true;
    BOOST_TEST(ROL_L(0xC0000001, 2) == 0x7);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(cpu.C);
}

BOOST_AUTO_TEST_CASE(zero) {
    BOOST_TEST(ROL_L(0x60000000, 0) == 0x60000000);
    BOOST_TEST(!cpu.C );
}

BOOST_AUTO_TEST_SUITE_END()
