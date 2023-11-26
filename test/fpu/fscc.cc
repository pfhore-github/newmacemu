#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
struct F_FScc {
    F_FScc() {
        // FSEQ %D1
        TEST::SET_W(0, 0171100 | 1);
        TEST::SET_W(2, 0x1);
        TEST::SET_W(4, TEST_BREAK);

        // FSNE %D1
        TEST::SET_W(6, 0171100 | 1);
        TEST::SET_W(8, 0xE);
        TEST::SET_W(10, TEST_BREAK);

        // FSGT %D1
        TEST::SET_W(12, 0171100 | 1);
        TEST::SET_W(14, 0x12);
        TEST::SET_W(16, TEST_BREAK);

        // FSNGT %D1
        TEST::SET_W(18, 0171100 | 1);
        TEST::SET_W(20, 0x1D);
        TEST::SET_W(22, TEST_BREAK);

        // FSGE %D1
        TEST::SET_W(24, 0171100 | 1);
        TEST::SET_W(26, 0x13);
        TEST::SET_W(28, TEST_BREAK);

        // FSNGE %D1
        TEST::SET_W(30, 0171100 | 1);
        TEST::SET_W(32, 0x1C);
        TEST::SET_W(34, TEST_BREAK);

        // FSLT %D1
        TEST::SET_W(36, 0171100 | 1);
        TEST::SET_W(38, 0x14);
        TEST::SET_W(40, TEST_BREAK);

        // FSNLT %D1
        TEST::SET_W(42, 0171100 | 1);
        TEST::SET_W(44, 0x1B);
        TEST::SET_W(46, TEST_BREAK);

        // FSLE %D1
        TEST::SET_W(48, 0171100 | 1);
        TEST::SET_W(50, 0x15);
        TEST::SET_W(52, TEST_BREAK);

        // FSNLE %D1
        TEST::SET_W(54, 0171100 | 1);
        TEST::SET_W(56, 0x1A);
        TEST::SET_W(58, TEST_BREAK);

        // FSGL %D1
        TEST::SET_W(60, 0171100 | 1);
        TEST::SET_W(62, 0x16);
        TEST::SET_W(64, TEST_BREAK);

        // FSNGL %D1
        TEST::SET_W(66, 0171100 | 1);
        TEST::SET_W(68, 0x19);
        TEST::SET_W(70, TEST_BREAK);

        // FSGLE %D1
        TEST::SET_W(72, 0171100 | 1);
        TEST::SET_W(74, 0x17);
        TEST::SET_W(76, TEST_BREAK);

        // FSNGLE %D1
        TEST::SET_W(78, 0171100 | 1);
        TEST::SET_W(80, 0x18);
        TEST::SET_W(82, TEST_BREAK);

        // FSOGT %D1
        TEST::SET_W(84, 0171100 | 1);
        TEST::SET_W(86, 0x2);
        TEST::SET_W(88, TEST_BREAK);

        // FSULE %D1
        TEST::SET_W(90, 0171100 | 1);
        TEST::SET_W(92, 0xD);
        TEST::SET_W(94, TEST_BREAK);

        // FSOGE %D1
        TEST::SET_W(96, 0171100 | 1);
        TEST::SET_W(98, 0x3);
        TEST::SET_W(100, TEST_BREAK);

        // FSULT %D1
        TEST::SET_W(102, 0171100 | 1);
        TEST::SET_W(104, 0xC);
        TEST::SET_W(106, TEST_BREAK);

        // FSOLT %D1
        TEST::SET_W(108, 0171100 | 1);
        TEST::SET_W(110, 0x4);
        TEST::SET_W(112, TEST_BREAK);

        // FSUGE %D1
        TEST::SET_W(114, 0171100 | 1);
        TEST::SET_W(116, 0xB);
        TEST::SET_W(118, TEST_BREAK);

        // FSOLE %D1
        TEST::SET_W(120, 0171100 | 1);
        TEST::SET_W(122, 0x5);
        TEST::SET_W(124, TEST_BREAK);

        // FSUGT %D1
        TEST::SET_W(126, 0171100 | 1);
        TEST::SET_W(128, 0xA);
        TEST::SET_W(130, TEST_BREAK);

        // FSOGL %D1
        TEST::SET_W(132, 0171100 | 1);
        TEST::SET_W(134, 0x6);
        TEST::SET_W(136, TEST_BREAK);

        // FSUEQ %D1
        TEST::SET_W(138, 0171100 | 1);
        TEST::SET_W(140, 0x9);
        TEST::SET_W(142, TEST_BREAK);

        // FSOR %D1
        TEST::SET_W(144, 0171100 | 1);
        TEST::SET_W(146, 0x7);
        TEST::SET_W(148, TEST_BREAK);

        // FSUN %D1
        TEST::SET_W(150, 0171100 | 1);
        TEST::SET_W(152, 0x8);
        TEST::SET_W(154, TEST_BREAK);

        // FSF %D1
        TEST::SET_W(156, 0171100 | 1);
        TEST::SET_W(158, 0);
        TEST::SET_W(160, TEST_BREAK);

        // FST %D1
        TEST::SET_W(162, 0171100 | 1);
        TEST::SET_W(164, 0xF);
        TEST::SET_W(166, TEST_BREAK);

        // FSSF %D1
        TEST::SET_W(168, 0171100 | 1);
        TEST::SET_W(170, 0x10);
        TEST::SET_W(172, TEST_BREAK);

        // FSFT %D1
        TEST::SET_W(174, 0171100 | 1);
        TEST::SET_W(176, 0x1F);
        TEST::SET_W(178, TEST_BREAK);

        // FSSEQ %D1
        TEST::SET_W(180, 0171100 | 1);
        TEST::SET_W(182, 0x11);
        TEST::SET_W(184, TEST_BREAK);

        // FSSNE %D1
        TEST::SET_W(186, 0171100 | 1);
        TEST::SET_W(188, 0x1E);
        TEST::SET_W(190, TEST_BREAK);
        jit_compile(0, 192);
    }
};
BOOST_FIXTURE_TEST_SUITE(FScc, Prepare, *boost::unit_test::fixture<F_FScc>())
BOOST_AUTO_TEST_SUITE(EQ)
BOOST_AUTO_TEST_CASE(T) {
    cpu.FPSR.CC_Z = true;
    run_test(0);
    BOOST_TEST(cpu.D[1] == 0xff);
}
BOOST_AUTO_TEST_CASE(F) {
    cpu.FPSR.CC_Z = false;
    run_test(0);
    BOOST_TEST(cpu.D[1] == 0);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(NE)
BOOST_AUTO_TEST_CASE(T) {
    cpu.FPSR.CC_Z = false;
    run_test(6);
    BOOST_TEST(cpu.D[1] == 0xff);
}
BOOST_AUTO_TEST_CASE(F) {
    cpu.FPSR.CC_Z = true;
    run_test(6);
    BOOST_TEST(cpu.D[1] == 0);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GT)
BOOST_AUTO_TEST_CASE(T) {
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    cpu.FPSR.CC_N = false;
    run_test(12);
    BOOST_TEST(cpu.D[1] == 0xff);
}
BOOST_AUTO_TEST_CASE(F1) {
    cpu.FPSR.CC_NAN = true;
    run_test(12);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F2) {
    cpu.FPSR.CC_Z = true;
    run_test(12);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F3) {
    cpu.FPSR.CC_N = true;
    run_test(12);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(NGT)
BOOST_AUTO_TEST_CASE(T1) {
    cpu.FPSR.CC_NAN = true;
    run_test(18);
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(T2) {
    cpu.FPSR.CC_Z = true;
    run_test(18);
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(T3) {
    cpu.FPSR.CC_N = true;
    run_test(18);
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F) {
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    cpu.FPSR.CC_N = false;
    run_test(18);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GE)
BOOST_AUTO_TEST_CASE(T1) {
    cpu.FPSR.CC_Z = true;
    run_test(24);
    BOOST_TEST(cpu.D[1] == 0xff);
}
BOOST_AUTO_TEST_CASE(T2) {
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_N = false;
    run_test(24);
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(F1) {
    cpu.FPSR.CC_Z = false;
    cpu.FPSR.CC_NAN = true;
    run_test(24);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F2) {
    cpu.FPSR.CC_Z = false;
    cpu.FPSR.CC_N = true;
    run_test(24);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(NGE)
BOOST_AUTO_TEST_CASE(T1) {
    cpu.FPSR.CC_NAN = true;
    run_test(30);
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(T2) {
    cpu.FPSR.CC_Z = false;
    cpu.FPSR.CC_N = true;
    run_test(30);
    BOOST_TEST(cpu.D[1] == 0xff);
}

BOOST_AUTO_TEST_CASE(F1) {
    cpu.FPSR.CC_N = false;
    cpu.FPSR.CC_NAN = false;
    run_test(30);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F2) {
    cpu.FPSR.CC_Z = true;
    cpu.FPSR.CC_NAN = false;
    run_test(30);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(LT)
BOOST_AUTO_TEST_CASE(T1) {
    cpu.FPSR.CC_N = true;
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    run_test(36);
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F1) {
    cpu.FPSR.CC_N = false;
    run_test(36);
    BOOST_TEST(cpu.D[1] == 0);
}

BOOST_AUTO_TEST_CASE(F2) {
    cpu.FPSR.CC_NAN = true;
    run_test(36);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(F3) {
    cpu.FPSR.CC_Z = true;
    run_test(36);
    BOOST_TEST(cpu.D[1] == 0);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(NLT)
BOOST_AUTO_TEST_CASE(T1) {
    cpu.FPSR.CC_NAN = true;
    run_test(42);
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(T2) {
    cpu.FPSR.CC_Z = true;
    cpu.FPSR.CC_N = false;
    run_test(42);
    BOOST_TEST(cpu.D[1] == 0xff);
}
BOOST_AUTO_TEST_CASE(F1) {
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    run_test(42);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(F2) {
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_N = true;
    run_test(42);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(LE)
BOOST_AUTO_TEST_CASE(T1) {
    cpu.FPSR.CC_Z = true;
    run_test(48);
    BOOST_TEST(cpu.D[1] == 0xff);
}
BOOST_AUTO_TEST_CASE(T2) {
    cpu.FPSR.CC_N = true;
    cpu.FPSR.CC_NAN = false;
    run_test(48);
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F1) {
    cpu.FPSR.CC_N = false;
    cpu.FPSR.CC_Z = false;
    run_test(48);
    BOOST_TEST(cpu.D[1] == 0);
}

BOOST_AUTO_TEST_CASE(F2) {
    cpu.FPSR.CC_NAN = true;
    cpu.FPSR.CC_Z = false;
    run_test(48);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(NLE)
BOOST_AUTO_TEST_CASE(T1) {
    cpu.FPSR.CC_NAN = true;
    run_test(54);
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(T2) {
    cpu.FPSR.CC_Z = true;
    cpu.FPSR.CC_N = false;
    run_test(54);
    BOOST_TEST(cpu.D[1] == 0xff);
}
BOOST_AUTO_TEST_CASE(F1) {
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    run_test(54);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(F2) {
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_N = true;
    run_test(54);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GL)
BOOST_AUTO_TEST_CASE(T) {
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    run_test(60);
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F1) {
    cpu.FPSR.CC_NAN = true;
    run_test(60);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(F2) {
    cpu.FPSR.CC_Z = true;
    run_test(60);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(NGL)
BOOST_AUTO_TEST_CASE(T1) {
    cpu.FPSR.CC_NAN = true;
    run_test(66);
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(T2) {
    cpu.FPSR.CC_Z = true;
    run_test(66);
    BOOST_TEST(cpu.D[1] == 0xff);
}

BOOST_AUTO_TEST_CASE(F) {
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    run_test(66);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GLE)
BOOST_AUTO_TEST_CASE(T) {
    cpu.FPSR.CC_NAN = false;
    run_test(72);
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(F) {
    cpu.FPSR.CC_NAN = true;
    run_test(72);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(NGLE)
BOOST_AUTO_TEST_CASE(T) {
    cpu.FPSR.CC_NAN = true;
    run_test(78);
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(F) {
    cpu.FPSR.CC_NAN = false;
    run_test(78);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(OGT)
BOOST_AUTO_TEST_CASE(T) {
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    cpu.FPSR.CC_N = false;
    run_test(84);
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F1) {
    cpu.FPSR.CC_NAN = true;
    run_test(84);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F2) {
    cpu.FPSR.CC_Z = true;
    run_test(84);
    BOOST_TEST(cpu.D[1] == 0);
}
BOOST_AUTO_TEST_CASE(F3) {
    cpu.FPSR.CC_N = true;
    run_test(84);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ULE)
BOOST_AUTO_TEST_CASE(T1) {
    cpu.FPSR.CC_NAN = true;
    run_test(90);
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(T2) {
    cpu.FPSR.CC_Z = true;
    run_test(90);
    BOOST_TEST(cpu.D[1] == 0xff);
}

BOOST_AUTO_TEST_CASE(T3) {
    cpu.FPSR.CC_N = true;
    run_test(90);
    BOOST_TEST(cpu.D[1] == 0xff);
}
BOOST_AUTO_TEST_CASE(F) {
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    cpu.FPSR.CC_N = false;
    run_test(90);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(OGE)
BOOST_AUTO_TEST_CASE(T1) {
    cpu.FPSR.CC_Z = true;
    run_test(96);
    BOOST_TEST(cpu.D[1] == 0xff);
}
BOOST_AUTO_TEST_CASE(T2) {
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_N = false;
    run_test(96);
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(F1) {
    cpu.FPSR.CC_Z = false;
    cpu.FPSR.CC_NAN = true;
    run_test(96);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F2) {
    cpu.FPSR.CC_Z = false;
    cpu.FPSR.CC_N = true;
    run_test(96);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ULT)
BOOST_AUTO_TEST_CASE(T1) {
    cpu.FPSR.CC_NAN = true;
    run_test(102);
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(T2) {
    cpu.FPSR.CC_Z = false;
    cpu.FPSR.CC_N = true;
    run_test(102);
    BOOST_TEST(cpu.D[1] == 0xff);
}

BOOST_AUTO_TEST_CASE(F1) {
    cpu.FPSR.CC_N = false;
    cpu.FPSR.CC_NAN = false;
    run_test(102);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F2) {
    cpu.FPSR.CC_Z = true;
    cpu.FPSR.CC_NAN = false;
    run_test(102);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(OLT)
BOOST_AUTO_TEST_CASE(T1) {
    cpu.FPSR.CC_N = true;
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    run_test(108);
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F1) {
    cpu.FPSR.CC_N = false;
    run_test(108);
    BOOST_TEST(cpu.D[1] == 0);
}

BOOST_AUTO_TEST_CASE(F2) {
    cpu.FPSR.CC_NAN = true;
    run_test(108);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F3) {
    cpu.FPSR.CC_Z = true;
    run_test(108);
    BOOST_TEST(cpu.D[1] == 0);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(UGE)
BOOST_AUTO_TEST_CASE(T1) {
    cpu.FPSR.CC_NAN = true;
    run_test(114);
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(T2) {
    cpu.FPSR.CC_Z = true;
    cpu.FPSR.CC_N = false;
    run_test(114);
    BOOST_TEST(cpu.D[1] == 0xff);
}
BOOST_AUTO_TEST_CASE(F1) {
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    run_test(114);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(F2) {
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_N = true;
    run_test(114);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(OLE)
BOOST_AUTO_TEST_CASE(T1) {
    cpu.FPSR.CC_Z = true;
    run_test(120);
    BOOST_TEST(cpu.D[1] == 0xff);
}
BOOST_AUTO_TEST_CASE(T2) {
    cpu.FPSR.CC_N = true;
    cpu.FPSR.CC_NAN = false;
    run_test(120);
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F1) {
    cpu.FPSR.CC_N = false;
    cpu.FPSR.CC_Z = false;
    run_test(120);
    BOOST_TEST(cpu.D[1] == 0);
}

BOOST_AUTO_TEST_CASE(F2) {
    cpu.FPSR.CC_NAN = true;
    cpu.FPSR.CC_Z = false;
    run_test(120);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(UGT)
BOOST_AUTO_TEST_CASE(T1) {
    cpu.FPSR.CC_NAN = true;
    run_test(126);
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(T2) {
    cpu.FPSR.CC_Z = true;
    cpu.FPSR.CC_N = false;
    run_test(126);
    BOOST_TEST(cpu.D[1] == 0xff);
}
BOOST_AUTO_TEST_CASE(F1) {
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    run_test(126);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(F2) {
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_N = true;
    run_test(126);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(OGL)
BOOST_AUTO_TEST_CASE(T) {
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    run_test(132);
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(F1) {
    cpu.FPSR.CC_NAN = true;
    run_test(132);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(F2) {
    cpu.FPSR.CC_Z = true;
    run_test(132);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(UEQ)
BOOST_AUTO_TEST_CASE(T1) {
    cpu.FPSR.CC_NAN = true;
    run_test(138);
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}
BOOST_AUTO_TEST_CASE(T2) {
    cpu.FPSR.CC_Z = true;
    run_test(138);
    BOOST_TEST(cpu.D[1] == 0xff);
}

BOOST_AUTO_TEST_CASE(F) {
    cpu.FPSR.CC_NAN = false;
    cpu.FPSR.CC_Z = false;
    run_test(138);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(OR)
BOOST_AUTO_TEST_CASE(T) {
    cpu.FPSR.CC_NAN = false;
    run_test(144);
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(F) {
    cpu.FPSR.CC_NAN = true;
    run_test(144);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(UN)
BOOST_AUTO_TEST_CASE(T) {
    cpu.FPSR.CC_NAN = true;
    run_test(150);
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_CASE(F) {
    cpu.FPSR.CC_NAN = false;
    run_test(150);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(!cpu.FPSR.BSUN);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(F) {
    run_test(156);
    BOOST_TEST(cpu.D[1] == 0);
}
BOOST_AUTO_TEST_CASE(T) {
    run_test(162);
    BOOST_TEST(cpu.D[1] == 0xff);
}
BOOST_DATA_TEST_CASE(SF, bdata::xrange(2), is_nan) {
    cpu.FPSR.CC_NAN = is_nan;
    run_test(168);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(cpu.FPSR.BSUN == is_nan);
}

BOOST_DATA_TEST_CASE(ST, bdata::xrange(2), is_nan) {
    cpu.FPSR.CC_NAN = is_nan;
    run_test(174);
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(cpu.FPSR.BSUN == is_nan);
}

BOOST_AUTO_TEST_SUITE(SEQ)
BOOST_DATA_TEST_CASE(T, bdata::xrange(2), is_nan) {
    cpu.FPSR.CC_Z = true;
    cpu.FPSR.CC_NAN = is_nan;
    run_test(180);
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(cpu.FPSR.BSUN == is_nan);
}
BOOST_DATA_TEST_CASE(F, bdata::xrange(2), is_nan) {
    cpu.FPSR.CC_Z = false;
    cpu.FPSR.CC_NAN = is_nan;
    run_test(180);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(cpu.FPSR.BSUN == is_nan);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(SNE)
BOOST_DATA_TEST_CASE(T, bdata::xrange(2), is_nan) {
    cpu.FPSR.CC_Z = false;
    cpu.FPSR.CC_NAN = is_nan;
    run_test(186);
    BOOST_TEST(cpu.D[1] == 0xff);
    BOOST_TEST(cpu.FPSR.BSUN == is_nan);
}
BOOST_DATA_TEST_CASE(F, bdata::xrange(2), is_nan) {
    cpu.FPSR.CC_Z = true;
    cpu.FPSR.CC_NAN = is_nan;
    run_test(186);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(cpu.FPSR.BSUN == is_nan);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
