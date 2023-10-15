#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"

#include "SDL.h"
#include "chip/iifx.hpp"
#include "chip/iop.hpp"
#include "chip/rbv.hpp"
#include "chip/scsi.hpp"
#include "chip/vdac.hpp"
#include "chip/via.hpp"
#include "mb/glue.hpp"
#include "mb/jaws.hpp"
#include "mb/mcu.hpp"
#include "mb/mdu.hpp"
#include "mb/oss.hpp"
#include "mb/v8.hpp"

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <vector>
extern uint32_t io_mask;
namespace bdata = boost::unit_test::data;
uint32_t test_addr[] = {0x2f58, 0x2f64};
extern bool MACHINE_CODE[2];
extern bool MACHINE_CODE2[4];
struct PrepareROM : Prepare {
    PrepareROM() { failure_is_exception = false; }
};
BOOST_FIXTURE_TEST_SUITE(init, PrepareROM)
struct BadIO : public IO_BUS {
    uint8_t Read8(uint32_t) { return 0; }
    void Write8(uint32_t, uint8_t) {}
};
struct BadIO2 : public IO_BUS {
    uint8_t Read8(uint32_t) { throw AccessFault{}; }
    void Write8(uint32_t, uint8_t) {}
};
struct BadIO3 : public IO_BUS {
    RBV rbv;
    uint8_t Read8(uint32_t addr) {
        switch((addr >> 13) & 0x1f) {
        case 19:
            return rbv.read(addr & 0xff);
        }
        throw AccessFault{};
    }
    void Write8(uint32_t, uint8_t) {}
};
BOOST_AUTO_TEST_SUITE(_3060)
auto CheckVIA(bool t) {
    return [t]() {
        BOOST_TEST(cpu.D[2] == 0x20000);
        cpu.Z = t;
        cpu.PC = cpu.A[6];
    };
}
BOOST_DATA_TEST_CASE(result, bdata::xrange(2), v) {
    cpu.A[0] = 0x408032D8;
    test_rom(0x3060, test_addr[v], {{0x46AA, CheckVIA(v)}});
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(_307E)
auto CheckVIA() {
    cpu.Z = cpu.D[2] == 0x40000;
    cpu.PC = cpu.A[6];
}
BOOST_AUTO_TEST_CASE(ok) {

    cpu.A[0] = 0x4080337C;
    io = std::make_unique<MDU>();
    test_rom(0x307E, 0x2F64, {{0x46AA, CheckVIA}});
}

BOOST_AUTO_TEST_CASE(ng1) {
    cpu.A[0] = 0x4080337C;
    test_rom(0x307E, 0x2F58, {{0x46AA, []() {
                                   cpu.Z = false;
                                   cpu.PC = cpu.A[6];
                               }}});
}

BOOST_AUTO_TEST_CASE(ng2) {
    cpu.A[0] = 0x4080337C;
    cpu.VBR = 0x40803DD8;
    cpu.A[6] = 0x40802F58;
    cpu.D[7] |= 1 << 27;
    failure_is_exception = true;
    io = std::make_unique<BadIO2>();
    test_rom(0x307E, 0x2F58, {{0x46AA, CheckVIA}});
}

BOOST_AUTO_TEST_CASE(ng3) {
    cpu.A[0] = 0x4080337C;
    cpu.VBR = 0x40803DD8;
    cpu.A[6] = 0x40802F58;
    cpu.D[7] |= 1 << 27;
    failure_is_exception = true;
    io = std::make_unique<BadIO3>();
    test_rom(0x307E, 0x2F58, {{0x46AA, CheckVIA}});
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(_30BE)
auto CheckVIA() {
    cpu.Z = cpu.D[2] == 0x40000;
    cpu.PC = cpu.A[6];
}
BOOST_AUTO_TEST_CASE(ok) {
    cpu.A[0] = 0x40803420;
    io = std::make_unique<OSS>();
    test_rom(0x30BE, 0x2F64, {{0x46AA, CheckVIA}});
}
BOOST_AUTO_TEST_CASE(ng1) {
    cpu.A[0] = 0x40803420;
    test_rom(0x30BE, 0x2F58, {{0x46AA, []() {
                                   cpu.Z = false;
                                   cpu.PC = cpu.A[6];
                               }}});
}

BOOST_AUTO_TEST_CASE(ng2) {
    cpu.A[0] = 0x40803420;
    test_rom(0x30BE, 0x2F58, {{0x46AA, []() {
                                   cpu.Z = true;
                                   cpu.PC = cpu.A[6];
                               }}});
}
BOOST_AUTO_TEST_CASE(ng3) {
    cpu.A[0] = 0x40803420;
    cpu.VBR = 0x40803DD8;
    cpu.A[6] = 0x40802F58;
    cpu.D[7] |= 1 << 27;
    io = std::make_unique<GLUE>();
    failure_is_exception = true;
    test_rom(0x30BE, 0x2F58, {{0x46AA, CheckVIA}});
}

BOOST_AUTO_TEST_CASE(ng4) {
    cpu.A[0] = 0x40803420;
    io = std::make_unique<BadIO>();
    test_rom(0x30BE, 0x2F58, {{0x46AA, CheckVIA}});
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(_311A)
auto CheckVIA0() {
    cpu.Z = true;
    cpu.PC = cpu.A[6];
}
auto CheckVIA() {
    cpu.Z = false;
    cpu.PC = cpu.A[6];
}
BOOST_AUTO_TEST_CASE(ok) {
    cpu.A[0] = 0x408034C4;
    io = std::make_unique<V8>();
    test_rom(0x311A, 0x2F64, {{0x46AA, CheckVIA}, {0x46A8, CheckVIA0}});
}
BOOST_AUTO_TEST_CASE(ng1) {
    cpu.A[0] = 0x4080337C;
    io = std::make_unique<V8>();
    test_rom(0x311A, 0x2F58, {{0x46AA, CheckVIA}, {0x46A8, CheckVIA}});
}

BOOST_AUTO_TEST_CASE(ng2) {
    cpu.A[0] = 0x4080337C;
    cpu.VBR = 0x40803DD8;
    cpu.A[6] = 0x40802F58;
    cpu.D[7] |= 1 << 27;
    io = std::make_unique<BadIO2>();
    failure_is_exception = true;
    test_rom(0x311A, 0x2F58, {{0x46AA, CheckVIA}, {0x46A8, CheckVIA0}});
}

BOOST_AUTO_TEST_CASE(ng3) {
    cpu.A[0] = 0x4080337C;
    cpu.VBR = 0x40803DD8;
    cpu.A[6] = 0x40802F58;
    cpu.D[7] |= 1 << 27;
    io = std::make_unique<BadIO3>();
    failure_is_exception = true;
    test_rom(0x311A, 0x2F58, {{0x46AA, CheckVIA}, {0x46A8, CheckVIA0}});
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(_3154)
BOOST_AUTO_TEST_CASE(ok) {
    cpu.A[0] = 0x40803568;
    io = std::make_unique<MCU_Q900>();
    test_rom(0x3154, 0x2F64, {});
}
BOOST_AUTO_TEST_CASE(ng1) {
    cpu.A[0] = 0x40803568;
    cpu.A[6] = 0x40802F58;
    cpu.VBR = 0x40803DD8;
    cpu.D[7] |= 1 << 27;
    failure_is_exception = true;
    io = std::make_unique<GLUE>();
    test_rom(0x3154, 0x2F58, {});
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(_3162)
auto CheckVIA() {
    cpu.Z = cpu.D[2] == 0x100000;
    cpu.PC = cpu.A[6];
}
BOOST_AUTO_TEST_CASE(ok) {
    cpu.A[0] = 0x4080360C;
    io = std::make_unique<JAWS>();
    test_rom(0x3162, 0x2F64, {{0x46AA, CheckVIA}});
}
BOOST_AUTO_TEST_CASE(ng) {
    cpu.A[0] = 0x4080360C;
    cpu.A[6] = 0x40802F58;
    cpu.VBR = 0x40803DD8;
    cpu.D[7] |= 1 << 27;
    io = std::make_unique<GLUE>();
    test_rom(0x3162, 0x2F58, {{0x46AA, CheckVIA}});
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(_3D48)
auto CheckSCSI_MCU(bool v) {
    BOOST_TEST(cpu.A[2] == 0x50F0F000);
    cpu.Z = v;
    cpu.PC = cpu.A[6];
}
BOOST_AUTO_TEST_CASE(None) {
    cpu.D[0] = 0;
    test_rom(0x3D48, 0x3DAA, {{0x4706, []() { CheckSCSI_MCU(true); }}});
    BOOST_TEST(cpu.D[0] == 0);
}

BOOST_AUTO_TEST_CASE(QuadraScsiFailure) {
    cpu.D[0] = 0x1008700;
    cpu.A[0] = 0x40803568;
    cpu.A[1] = 0x408038BC;
    test_rom(0x3D48, 0x3DAA, {{0x4706, []() { CheckSCSI_MCU(false); }}});
    BOOST_TEST(cpu.D[0] == 0x008700);
}

BOOST_AUTO_TEST_CASE(QuadraScsi1) {
    cpu.D[0] = 0x1008700;
    cpu.A[0] = 0x40803568;
    cpu.A[1] = 0x408038BC;
    test_rom(0x3D48, 0x3DAA, {{0x4706, []() { CheckSCSI_MCU(true); }}});
    BOOST_TEST(cpu.D[0] == 0x1000000);
}

BOOST_AUTO_TEST_CASE(QuadraScsi2Failure) {
    cpu.D[0] = 0x3008700;
    cpu.A[0] = 0x40803568;
    cpu.A[1] = 0x408038BC;
    test_rom(0x3D48, 0x3DAA, {{0x4706, []() {
                                   cpu.Z = cpu.A[2] == 0x50F0F000;
                                   cpu.PC = cpu.A[6];
                               }}});
    BOOST_TEST(cpu.D[0] == 0x1000000);
}

BOOST_AUTO_TEST_CASE(QuadraScsi2) {
    cpu.D[0] = 0x3008700;
    cpu.A[0] = 0x40803568;
    cpu.A[1] = 0x408038BC;
    test_rom(0x3D48, 0x3DAA, {{0x4706, []() {
                                   cpu.Z = true;
                                   cpu.PC = cpu.A[6];
                               }}});
    BOOST_TEST(cpu.D[0] == 0x3000000);
}

BOOST_DATA_TEST_CASE(IIfxExp1, bdata::xrange(2), v) {
    cpu.D[0] = 0x100000;
    cpu.A[0] = 0x40803420;
    cpu.A[1] = 0x408037BC;
    test_rom(0x3D48, 0x3DAA, {{0x4670, [v]() {
                                   cpu.Z = v;
                                   cpu.PC = cpu.A[6];
                               }}});
    BOOST_TEST(cpu.D[0] == (v << 20));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(_3DBA)
auto cacheTest(bool z) {
    cpu.Z = z;
    cpu.PC = cpu.A[6];
}
BOOST_AUTO_TEST_CASE(none) {
    cpu.D[2] = 0;
    test_rom(0x3DBA, 0x3050, {});
    BOOST_TEST(cpu.D[2] == 0);
}
BOOST_AUTO_TEST_CASE(ok) {
    cpu.D[2] = 1 << 28;
    test_rom(0x3DBA, 0x3050, {{0x4640, []() { cacheTest(true); }}});
    BOOST_TEST(cpu.D[2] == 1 << 28);
}
BOOST_AUTO_TEST_CASE(ng) {
    cpu.D[2] = 1 << 28;
    test_rom(0x3DBA, 0x3050, {{0x4640, []() { cacheTest(false); }}});
    BOOST_TEST(cpu.D[2] == 0);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(_4052) {
    test_rom(0x04052, 0x040A2, {});
    BOOST_TEST(!cpu.TCR_E);
    BOOST_TEST(!cpu.TCR_P);
    BOOST_TEST(cpu.DTTR[0].logic_base == 0x80);
    BOOST_TEST(cpu.DTTR[0].logic_mask == 0x7F);
    BOOST_TEST(cpu.DTTR[0].E);
    BOOST_TEST(cpu.DTTR[0].S == 2);
    BOOST_TEST(cpu.DTTR[0].CM == 2);

    BOOST_TEST(cpu.DTTR[1].logic_base == 0x50);
    BOOST_TEST(cpu.DTTR[1].logic_mask == 0x0F);
    BOOST_TEST(cpu.DTTR[1].E);
    BOOST_TEST(cpu.DTTR[1].S == 2);
    BOOST_TEST(cpu.DTTR[1].CM == 2);
}

BOOST_AUTO_TEST_CASE(_4640) {
    cpu.CACR_DE = true;
    test_rom(0x04640, 0x466E, {});
    BOOST_TEST(!cpu.CACR_DE);
}
BOOST_AUTO_TEST_SUITE(_4670)
BOOST_AUTO_TEST_CASE(success) {
    cpu.A[2] = 0x50F1E000;
    io = std::make_unique<OSS>();
    test_rom(0x04670, 0x46A6, {});
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(failure1) {
    cpu.A[2] = 0x50F1E000;
    cpu.A[6] = 0x40802F58;
    cpu.VBR = 0x40803DD8;
    cpu.D[7] |= 1 << 27;
    failure_is_exception = true;
    io = std::make_unique<GLUE>();
    test_rom(0x04670, 0x2F58, {});
}

BOOST_AUTO_TEST_CASE(fail1) {
    cpu.A[2] = 0x50F1E000;
    io = std::make_unique<BadIO>();
    test_rom(0x04670, 0x46A6, {});
    BOOST_TEST(!cpu.Z);
}
struct BadIIfx2_2 : public IO_BUS {
    uint8_t Read8(uint32_t) { return 1; }
    void Write8(uint32_t, uint8_t) {}
};
BOOST_AUTO_TEST_CASE(fail2) {
    cpu.A[2] = 0x50F1E000;
    io = std::make_unique<BadIIfx2_2>();
    test_rom(0x04670, 0x46A6, {});
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(_46AA)

BOOST_AUTO_TEST_CASE(success) {
    cpu.A[2] = 0x50F01C00;
    cpu.D[2] = 0x20000;
    io = std::make_unique<GLUE>();
    test_rom(0x46AA, 0x46D4, {});
    BOOST_TEST(cpu.Z);
}
struct BadVIA : public IO_BUS {
    uint8_t Read8(uint32_t) { throw AccessFault{}; }
    void Write8(uint32_t, uint8_t) {}
};
BOOST_AUTO_TEST_CASE(fail1) {
    cpu.A[2] = 0x50F01C00;
    cpu.D[2] = 0x20000;
    cpu.A[6] = 0x40802F58;
    cpu.VBR = 0x40803DD8;
    cpu.D[7] |= 1 << 27;
    io = std::make_unique<BadIO2>();
    failure_is_exception = true;
    test_rom(0x46AA, 0x2F58, {});
    BOOST_TEST(!cpu.Z);
}
BOOST_AUTO_TEST_CASE(fail2) {
    cpu.A[2] = 0x50F01C00;
    cpu.D[2] = 0x20000;
    cpu.A[6] = 0x40802F58;
    cpu.VBR = 0x40803DD8;
    cpu.D[7] |= 1 << 27;
    io = std::make_unique<MCU_Q900>();
    failure_is_exception = true;
    test_rom(0x46AA, 0x2F58, {});
    BOOST_TEST(!cpu.Z);
}
struct BadVIA2 : public IO_BUS {
    VIA1 via1;
    VIA2 via2;
    uint8_t Read8(uint32_t addr) {
        switch((addr >> 13) & 0x1f) {
        case 0:
            return via1.read(addr >> 9 & 0xf);
        case 16:
            return via2.read(addr >> 9 & 0xf);
        }
        return 0;
    }
    void Write8(uint32_t, uint8_t) {}
};
BOOST_AUTO_TEST_CASE(fail3) {
    cpu.A[2] = 0x50F01C00;
    cpu.D[2] = 0x20000;
    io = std::make_unique<BadVIA2>();
    test_rom(0x46AA, 0x46D4, {});
    BOOST_TEST(!cpu.Z);
}

struct BadVIA3 : public IO_BUS {
    VIA1 via1;
    uint8_t Read8(uint32_t addr) override {
        switch((addr >> 13) & 0x1f) {
        case 0:
            return via1.read(addr >> 9 & 0xf);
        case 16:
            return 0;
        }
        return 0;
    }
    void Write8(uint32_t addr, uint8_t v) override {
        switch((addr >> 13) & 0x1f) {
        case 0:
            return via1.write(addr >> 9 & 0xf, v);
        }
    }
};

BOOST_AUTO_TEST_CASE(fail4) {
    cpu.A[2] = 0x50F01C00;
    cpu.D[2] = 0x20000;
    io = std::make_unique<BadVIA3>();
    test_rom(0x46AA, 0x46D4, {});
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(_46D6)
BOOST_AUTO_TEST_CASE(success) {
    cpu.A[2] = 0x50F26013;
    io = std::make_unique<MDU>();
    test_rom(0x46D6, 0x46FC, {});
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(fail1) {
    cpu.A[2] = 0x50F26013;
    cpu.A[6] = 0x40802F58;
    cpu.VBR = 0x40803DD8;
    cpu.D[7] |= 1 << 27;
    failure_is_exception = true;
    io = std::make_unique<GLUE>();
    test_rom(0x46D6, 0x2F58, {});
    BOOST_TEST(!cpu.Z);
}
struct BadRBV {
    uint8_t read(uint32_t) { return 0; }
    void write(uint32_t, uint8_t) {}
};

BOOST_AUTO_TEST_CASE(fail2) {
    cpu.A[2] = 0x50F26013;
    cpu.A[6] = 0x40802F58;
    cpu.VBR = 0x40803DD8;
    cpu.D[7] |= 1 << 27;
    io = std::make_unique<BadIO>();
    test_rom(0x46D6, 0x46FC, {});
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(_46FE)
BOOST_AUTO_TEST_CASE(success) {
    cpu.A[2] = 0x50F08000;
    io = std::make_unique<OSS>();
    test_rom(0x46FE, 0x4704, {});
    BOOST_TEST(cpu.Z);
}

BOOST_AUTO_TEST_CASE(fail1) {
    cpu.A[2] = 0x50F18000;
    cpu.A[6] = 0x40802F58;
    cpu.VBR = 0x40803DD8;
    cpu.D[7] |= 1 << 27;
    failure_is_exception = true;
    io = std::make_unique<GLUE>();
    test_rom(0x46FE, 0x2F58, {});
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(_4706)
BOOST_AUTO_TEST_CASE(success) {
    cpu.A[2] = 0x50F0F000;
    io = std::make_unique<MCU_Q900>();
    test_rom(0x4706, 0x4710, {});
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(fail1) {
    cpu.A[2] = 0x50F0F000;
    cpu.A[6] = 0x40802F58;
    cpu.VBR = 0x40803DD8;
    cpu.D[7] |= 1 << 27;
    failure_is_exception = true;
    io = std::make_unique<GLUE>();
    test_rom(0x4706, 0x2F58, {});
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_CASE(fail2) {
    cpu.A[2] = 0x50F0F000;
    cpu.D[2] = 0x20000;
    io = std::make_unique<BadIO>();
    test_rom(0x4706, 0x4710, {});
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()
struct DummyIOP : public IOP {
    uint8_t readBase(uint8_t) override { return 0; }
    void writeBase(uint8_t, uint8_t) override {}
};

BOOST_AUTO_TEST_SUITE(_477A)
BOOST_AUTO_TEST_CASE(success) {
    cpu.A[2] = 0x50F04020;
    io = std::make_unique<OSS>();
    test_rom(0x477A, 0x47AC, {});
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE(fail1) {
    cpu.A[2] = 0x50F1E020;
    cpu.A[6] = 0x40802F58;
    cpu.VBR = 0x40803DD8;
    cpu.D[7] |= 1 << 27;
    failure_is_exception = true;
    io = std::make_unique<GLUE>();
    test_rom(0x477A, 0x2F58, {});
    BOOST_TEST(!cpu.Z);
}

struct DummyIOP2 : public IO_BUS {
    uint8_t Read8(uint32_t) override { return 0; }
    void Write8(uint32_t, uint8_t) override {}
};

BOOST_AUTO_TEST_CASE(fail2) {
    cpu.A[2] = 0x50F1E020;
    io = std::make_unique<DummyIOP2>();
    test_rom(0x477A, 0x47AC, {});
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(_47AE)
BOOST_AUTO_TEST_CASE(None) {
    cpu.A[0] = 0x408032D8;
    cpu.D[0] = 0;
    test_rom(0x47AE, 0x4846, {});
    BOOST_TEST(cpu.D[1] == 0);
}
BOOST_AUTO_TEST_CASE(VIA1) {
    cpu.A[0] = 0x408032D8;
    cpu.D[0] = 1 << 2;
    io = std::make_unique<GLUE>();
    test_rom(0x47AE, 0x4846, {});

    BOOST_TEST(cpu.D[1] == 0x41080000);
}
BOOST_AUTO_TEST_CASE(VIA2) {
    cpu.A[0] = 0x408032D8;
    cpu.D[0] = 1 << 11;
    io = std::make_unique<GLUE>();
    test_rom(0x47AE, 0x4846, {});

    BOOST_TEST(cpu.D[1] == 0);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(_4848)
BOOST_AUTO_TEST_CASE(None) {
    cpu.A[0] = 0x408032D8;
    cpu.D[0] = 0;
    test_rom(0x4848, 0x48C6, {});
    BOOST_TEST(cpu.D[1] == 0);
}
BOOST_AUTO_TEST_CASE(MCU1) {
    cpu.A[0] = 0x40803568;
    cpu.D[0] = 1 << 21;
    io = std::make_unique<MCU_Q900>();
    test_rom(0x4848, 0x48C6, {});
    auto mcu = &static_cast<MCU_Q900*>(io.get())->mcu;
    BOOST_TEST(mcu->value == 0x124F0810);
    BOOST_TEST(mcu->ctls[32] == 0);
    BOOST_TEST(mcu->ctls[33] == 1);
    BOOST_TEST(mcu->ctls[40] == 0xffff);
    BOOST_TEST(mcu->ctls[41] == 0xffff);
    BOOST_TEST(mcu->ctls[42] == 0xffff);
    BOOST_TEST(mcu->ctls[43] == 0xffff);
    BOOST_TEST(mcu->ctls[44] == 0xffff);
    BOOST_TEST(mcu->ctls[45] == 0xffff);
    BOOST_TEST(mcu->ctls[46] == 0xffff);
}

BOOST_AUTO_TEST_CASE(PB) {
    cpu.A[0] = 0x4080360C;
    cpu.D[0] = 1 << 22;
    io = std::make_unique<JAWS>();
    test_rom(0x4848, 0x48C6, {});
}
BOOST_AUTO_TEST_SUITE_END()

static void getHwInfo() {
    BOOST_ASSERT(cpu.D[2] == 0);
    cpu.D[0] = 1;
    cpu.PC = cpu.A[6];
}
BOOST_AUTO_TEST_CASE(_48C8) {
    cpu.A[0] = 0x408032D8;
    test_rom(0x48C8, 0x48FC, {{0x2F18, getHwInfo}, {0x2E8C, called_impl}});
    BOOST_TEST(cpu.VBR == 0x40803DDC);
}
BOOST_AUTO_TEST_SUITE_END()