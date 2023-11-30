#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"

#include "SDL.h"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <vector>
#include "chip/via.hpp"
#include "chip/via.hpp"
#include "chip/iifx.hpp"
#include "mb/glue.hpp"
#include "mb/oss.hpp"
extern bool is_reset;
void initBus();
BOOST_FIXTURE_TEST_SUITE(boot, Prepare)
BOOST_AUTO_TEST_CASE(_2A) {
    test_rom(0x0002A, 0x0008C, {});
    BOOST_TEST_CHECKPOINT("OK");
}
BOOST_AUTO_TEST_CASE(_8C) {
    test_rom(0x0008C, 0x000B4,
             {
                 {0x4052, called_impl},
                 {0x2E00, called_impl},
                 {0x8AC00, called_impl},
             });
    BOOST_TEST(called[0] == 0x4052);
    BOOST_TEST(called[1] == 0x2E00);
    BOOST_TEST(called[2] == 0x8AC00);
    BOOST_TEST(cpu.S);
    BOOST_TEST(cpu.I == 7);
}

BOOST_AUTO_TEST_SUITE(_2E00)
void getHWInfo() {
    BOOST_TEST(cpu.D[2] == 0);
    cpu.D[0] = 1;
    cpu.A[0] = 0x40803568;
    cpu.A[1] = 0x408038BC;
    called.push_back(cpu.PC & ~0x40800000);
    cpu.PC = cpu.A[6];
}

void getHWInfo2() {
    BOOST_TEST(cpu.D[2] == 0);
    cpu.D[0] = 1 | 1 << 18 | 1 << 19;
    cpu.A[0] = 0x40803420;
    cpu.A[1] = 0x408037BC;
    called.push_back(cpu.PC & ~0x40800000);
    cpu.PC = cpu.A[6];
}

void MoveToRom() {
    BOOST_TEST(cpu.A[2] == 0x40800000);
    called.push_back(cpu.PC & ~0x40800000);
    cpu.D[3] = 0;
    cpu.PC = 0x40802E38;
}

void initHwExtra() {
    called.push_back(cpu.PC & ~0x40800000);
    cpu.PC = 0x40802E46;
}

void initHwExtra2() { called.push_back(cpu.PC & ~0x40800000); }
BOOST_AUTO_TEST_CASE(NonFx) {
    io = std::make_unique<GLUE>();
    test_rom(0x02E00, 0x4848,
             {
                 {0x2F18, getHWInfo},
                 {0x3DAE, MoveToRom},
                 {0x48C8, initHwExtra},
                 {0x4848, initHwExtra2},

             });
    BOOST_TEST(cpu.VBR == 0x40803DDC);
    BOOST_TEST(called[0] == 0x2F18);
    BOOST_TEST(called[1] == 0x3DAE);
    BOOST_TEST(called[2] == 0x48C8);
    BOOST_TEST(called[3] == 0x4848);
}

BOOST_AUTO_TEST_CASE(IIFx) {
    io = std::make_unique<OSS>();
    auto oss = static_cast<OSS*>(io.get());
    test_rom(0x02E00, 0x4848,
             {
                 {0x2F18, getHWInfo2},
                 {0x3DAE, MoveToRom},
                 {0x48C8, initHwExtra},
                 {0x4848, initHwExtra2},

             });
    BOOST_TEST(cpu.VBR == 0x40803DDC);
    BOOST_TEST(called[0] == 0x2F18);
    BOOST_TEST(called[1] == 0x3DAE);
    BOOST_TEST(called[2] == 0x48C8);
    BOOST_TEST(called[3] == 0x4848);
    BOOST_TEST(oss->oss.v[0x14] == 13);
    BOOST_TEST(oss->exp0.v == 0xF3FF);
    BOOST_TEST(oss->exp0.x == 0x00);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(_2E8C)

BOOST_AUTO_TEST_CASE(None) {
    cpu.D[0] = 1;
    cpu.A[0] = 0x40803568;
    cpu.A[1] = 0x408038BC;
    test_rom(0x02E8C, 0x2F16, {});
}


BOOST_AUTO_TEST_CASE(Via1) {
    cpu.D[0] = 1 << 2;
    cpu.A[0] = 0x40803568;
    cpu.A[1] = 0x408038BC;
    io = std::make_unique<GLUE>();
    test_rom(0x02E8C, 0x2F16, {});
    BOOST_TEST(via1->ora.to_ulong() == 0);
    BOOST_TEST(via1->dira.to_ulong() == 0);
    BOOST_TEST(via1->orb.to_ulong() == 0xC9);
    BOOST_TEST(via1->dirb.to_ulong() == 0xF1);
    BOOST_TEST(via1->read(12) == 0x22);
    BOOST_TEST(via1->read(11) == 0x1C);
    BOOST_TEST(via1->read(14) == 0x80);
  
}

BOOST_AUTO_TEST_CASE(Via2) {
    cpu.D[0] = 1 << 11;
    cpu.A[0] = 0x40803568;
    cpu.A[1] = 0x408038BC;
    io = std::make_unique<GLUE>();
    test_rom(0x02E8C, 0x2F16, {});
    BOOST_TEST(via2->ora.to_ulong() == 0x80);
    BOOST_TEST(via2->dira.to_ulong() == 0x80);
    BOOST_TEST(via2->orb.to_ulong() == 0x03);
    BOOST_TEST(via2->dirb.to_ulong() == 0xC8);
    BOOST_TEST(via2->read(12) == 0x22);
    BOOST_TEST(via2->read(11) == 0xC0);
    BOOST_TEST(via2->read(14) == 0x80);
    BOOST_TEST(via2->timer1_cnt == 0x196E);
}

#if 0
BOOST_AUTO_TEST_CASE(Rbv) {
    cpu.D[0] = 1 << 13;
    cpu.A[0] = 0x4080337C;
    test_rom(0x02E8C, 0x2F16, {});
    BOOST_TEST(write_b_expected.size() == 4);
    BOOST_TEST(write_b_expected[0].first == (0x0F26000 + 0x13));
    BOOST_TEST(exp0->v[0] == 0x7F);
    BOOST_TEST(write_b_expected[1].first == (0x0F26000 + 0));
    BOOST_TEST(exp0->v[1] == 0x8f);
    BOOST_TEST(write_b_expected[2].first == (0x0F26000 + 0x10));
    BOOST_TEST(exp0->v[2] == 0x40);
    BOOST_TEST(write_b_expected[3].first == (0x0F26000 + 0x12));
    BOOST_TEST(exp0->v[3] == 0xFF);
}
#endif

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(_2F18)
void refreshCpuCache() {
    called.push_back(cpu.PC);
    cpu.PC = 0x40803050;
}
void testOk() {
    called.push_back(cpu.PC);
    cpu.PC = 0x40802F64;
}
void testNG() {
    called.push_back(cpu.PC);
    cpu.PC = 0x40802F58;
}

void testhw() {
    called.push_back(cpu.PC);
    cpu.PC = cpu.A[6];
    cpu.Z = true;
}
BOOST_AUTO_TEST_CASE(None) {
    called.clear();
    cpu.D[2] = 0x1408;
    test_rom(0x2F18, 0x305E,
             {
                 {0x3DBA, refreshCpuCache},

             });
    BOOST_TEST(called.size() == 1);
    BOOST_TEST(called[0] == 0x40803DBA);
    BOOST_TEST(cpu.A[0] == 0x40803568);
    BOOST_TEST(cpu.A[1] == 0x408038BC);
    BOOST_TEST(cpu.D[0] == 0x07A31807);
    BOOST_TEST(cpu.D[1] == 0x00040924);
    BOOST_TEST(cpu.D[2] == 0xDC001408);
}

void getHwIDFromVia(uint32_t v) {
    called.push_back(cpu.PC);
    cpu.D[1] = v;
    cpu.PC = cpu.A[6];
}
BOOST_AUTO_TEST_CASE(glue) {
    called.clear();
    cpu.D[2] = 0;
    test_rom(0x2F18, 0x305E,
             {
                 {0x3DBA, refreshCpuCache},
                 {0x3060, testOk},
                 {0x307E, testNG},
                 {0x30BE, testNG},
                 {0x311A, testNG},
                 {0x3154, testNG},
                 {0x3162, testNG},
                 {0x47AE, [] { getHwIDFromVia(0x40000000); }},

             });
    while(called[0] != 0x40803060) {
        called.erase(called.begin());
    }
    BOOST_TEST(called.size() == 3);
    BOOST_TEST(called[0] == 0x40803060);
    BOOST_TEST(called[1] == 0x408047AE);
    BOOST_TEST(called[2] == 0x40803DBA);
    BOOST_TEST(cpu.A[0] == 0x408032D8);
    BOOST_TEST(cpu.A[1] == 0x408036BC);
    BOOST_TEST(cpu.D[0] == 0x00001F3F);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(cpu.D[2] == 0xDC000304);
}

BOOST_AUTO_TEST_CASE(mdu) {
    called.clear();
    cpu.D[2] = 0;
    test_rom(0x2F18, 0x305E,
             {
                 {0x3DBA, refreshCpuCache},
                 {0x3060, testNG},
                 {0x307E, testOk},
                 {0x30BE, testNG},
                 {0x311A, testNG},
                 {0x3154, testNG},
                 {0x3162, testNG},
                 {0x47AE, [] { getHwIDFromVia(0x46000000); }},

             });
    while(called[0] != 0x4080307E) {
        called.erase(called.begin());
    }
    BOOST_TEST(called.size() == 3);
    BOOST_TEST(called[0] == 0x4080307E);
    BOOST_TEST(called[1] == 0x408047AE);
    BOOST_TEST(called[2] == 0x40803DBA);
    BOOST_TEST(cpu.A[0] == 0x4080337C);
    BOOST_TEST(cpu.A[1] == 0x4080373C);
    BOOST_TEST(cpu.D[0] == 0x0000773F);
    BOOST_TEST(cpu.D[1] == 0);
    BOOST_TEST(cpu.D[2] == 0xDC000505);
}

void GetHwInfoExtra() {
    called.push_back(cpu.PC);
    cpu.PC = 0x4080304A;
}

BOOST_AUTO_TEST_CASE(oss) {
    called.clear();
    cpu.D[2] = 0;
    test_rom(0x2F18, 0x305E,
             {
                 {0x3DBA, refreshCpuCache},
                 {0x3060, testNG},
                 {0x307E, testNG},
                 {0x30BE, testOk},
                 {0x311A, testNG},
                 {0x3154, testNG},
                 {0x3162, testNG},
                 {0x46FE, testhw},
                 {0x477A, testhw},
                 {0x3D48, GetHwInfoExtra},
                 {0x47AE, [] { getHwIDFromVia(0x52000000); }},

             });
    while(called[0] != 0x408030BE) {
        called.erase(called.begin());
    }
    BOOST_TEST(called.size() == 7);
    BOOST_TEST(called[0] == 0x408030BE);
    BOOST_TEST(called[1] == 0x408047AE);
    BOOST_TEST(called[2] == 0x4080477A);
    BOOST_TEST(called[3] == 0x4080477A);
    BOOST_TEST(called[4] == 0x408046FE);
    BOOST_TEST(called[5] == 0x40803D48);
    BOOST_TEST(called[6] == 0x40803DBA);
    BOOST_TEST(cpu.A[0] == 0);
    BOOST_TEST(cpu.A[1] == 0x408037BC);
    BOOST_TEST(cpu.D[0] == 0x001F9007);
    BOOST_TEST(cpu.D[1] == 0x4);
    BOOST_TEST(cpu.D[2] == 0xDC000706);
}

BOOST_AUTO_TEST_CASE(v8) {
    called.clear();
    cpu.D[2] = 0;
    test_rom(0x2F18, 0x305E,
             {
                 {0x3DBA, refreshCpuCache},
                 {0x3060, testNG},
                 {0x307E, testNG},
                 {0x30BE, testNG},
                 {0x311A, testOk},
                 {0x3154, testNG},
                 {0x3162, testNG},
                 {0x47AE, [] { getHwIDFromVia(0x54000000); }},

             });
    while(called[0] != 0x4080311A) {
        called.erase(called.begin());
    }
    BOOST_TEST(called.size() == 3);
    BOOST_TEST(called[0] == 0x4080311A);
    BOOST_TEST(called[1] == 0x408047AE);
    BOOST_TEST(called[2] == 0x40803DBA);
    BOOST_TEST(cpu.A[0] == 0x408034C4);
    BOOST_TEST(cpu.A[1] == 0x4080383C);
    BOOST_TEST(cpu.D[0] == 0x0000773F);
    BOOST_TEST(cpu.D[1] == 0x000001A6);
    BOOST_TEST(cpu.D[2] == 0xDC000D07);
}

BOOST_AUTO_TEST_CASE(mcu) {
    called.clear();
    cpu.D[2] = 0;
    test_rom(0x2F18, 0x305E,
             {
                 {0x3DBA, refreshCpuCache},
                 {0x3060, testNG},
                 {0x307E, testNG},
                 {0x30BE, testNG},
                 {0x311A, testNG},
                 {0x3154, testOk},
                 {0x3162, testNG},
                 {0x47AE, [] { getHwIDFromVia(0x10000000); }},

             });
    while(called[0] != 0x40803154) {
        called.erase(called.begin());
    }
    BOOST_TEST(called.size() == 3);
    BOOST_TEST(called[0] == 0x40803154);
    BOOST_TEST(called[1] == 0x408047AE);
    BOOST_TEST(called[2] == 0x40803DBA);
    BOOST_TEST(cpu.A[0] == 0x40803568);
    BOOST_TEST(cpu.A[1] == 0x408038BC);
    BOOST_TEST(cpu.D[0] == 0x07A31807);
    BOOST_TEST(cpu.D[1] == 0x00040924);
    BOOST_TEST(cpu.D[2] == 0xDC001408);
}

BOOST_AUTO_TEST_CASE(jaws) {
    called.clear();
    cpu.D[2] = 0;
    test_rom(0x2F18, 0x305E,
             {
                 {0x3DBA, refreshCpuCache},
                 {0x3060, testNG},
                 {0x307E, testNG},
                 {0x30BE, testNG},
                 {0x311A, testNG},
                 {0x3154, testNG},
                 {0x3162, testOk},
                 {0x47AE, [] { getHwIDFromVia(0x12000000); }},

             });
    while(called[0] != 0x40803162) {
        called.erase(called.begin());
    }
    BOOST_TEST(called.size() == 3);
    BOOST_TEST(called[0] == 0x40803162);
    BOOST_TEST(called[1] == 0x408047AE);
    BOOST_TEST(called[2] == 0x40803DBA);
    BOOST_TEST(cpu.A[0] == 0x4080360C);
    BOOST_TEST(cpu.A[1] == 0x4080393C);
    BOOST_TEST(cpu.D[0] == 0x00401F3F);
    BOOST_TEST(cpu.D[1] == 0x00010932);
    BOOST_TEST(cpu.D[2] == 0xDD000F09);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()