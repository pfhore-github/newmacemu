#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"

#include "SDL.h"
#include "chip/iifx.hpp"
#include "chip/iop.hpp"
#include "chip/via.hpp"
#include "mb/glue.hpp"
#include "mb/oss.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <vector>
namespace bdata = boost::unit_test::data;
struct PrepareROM : Prepare {
    PrepareROM() { failure_is_exception = false; }
};
BOOST_FIXTURE_TEST_SUITE(init2, PrepareROM)
BOOST_AUTO_TEST_SUITE(_46C06)
static void getHwInfo() {
    BOOST_ASSERT(cpu.D[2] == 0);
    cpu.A[0] = 0x40803568;
    cpu.A[1] = 0x408038BC;
    cpu.D[0] = 0x07A31807;
    cpu.D[1] = 0x00040924;
    cpu.D[2] = 0xDC001408;
    cpu.PC = cpu.A[6];
}
BOOST_AUTO_TEST_CASE(result) {
    cpu.A[0] = 0x408032D8;
    test_rom(0x46C06, 0x46C66, {{0x470BA, getHwInfo}});
    BOOST_TEST(cpu.A[7] == 0x2600);
    BOOST_TEST(cpu.VBR == 0x40846980);
}
auto result477C4(bool v) {
    return [v]() {
        cpu.Z = v;
        cpu.PC = cpu.A[6];
    };
}
BOOST_AUTO_TEST_SUITE(initSCC)
BOOST_AUTO_TEST_CASE(OK) {
    cpu.D[0] = 1 << 17;
    test_rom(0x46C66, 0x46C80,
             {{0x470BA, getHwInfo}, {0x477C4, result477C4(true)}});
}
BOOST_AUTO_TEST_CASE(NG) {
    cpu.D[0] = 1 << 17;
    test_rom(0x46C66, 0x49AFA,
             {{0x470BA, getHwInfo}, {0x477C4, result477C4(false)}});
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(OssCheck2)
BOOST_AUTO_TEST_CASE(off) {
    cpu.D[2] = 6;
    io = std::make_unique<OSS>();
    static_cast<OSS *>(io.get())->oss.v[0x16] = 0;
    test_rom(0x46C8E, 0x46D7C, {});
}
BOOST_AUTO_TEST_CASE(on) {
    cpu.D[0] = 1 << 17;
    io = std::make_unique<OSS>();
    static_cast<OSS *>(io.get())->oss.v[0x16] = 2;
    test_rom(0x46C8E, 0x46D5A, {});
}
BOOST_AUTO_TEST_CASE(NonOSS) {
    cpu.D[0] = 0;
    test_rom(0x46C8E, 0x46CAA, {});
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(LateMachineCheck)
BOOST_AUTO_TEST_CASE(Not) {
    cpu.D[2] = 0x0F << 8;
    test_rom(0x46CAA, 0x46CD8, {});
}
BOOST_DATA_TEST_CASE(Old, bdata::xrange(2), debugEnable) {
    cpu.D[2] = 3 << 8;
    cpu.A[2] = 0x50f00000;
    io = std::make_unique<GLUE>();
    via1->appleTalkDebugEnabled = debugEnable;
    test_rom(0x46CAA, debugEnable ? 0x46D5A : 0x46CD8, {});
}

BOOST_AUTO_TEST_CASE(Quadra) {
    cpu.D[2] = 0x10 << 8;
    cpu.A[2] = 0x50f00000;
    io = std::make_unique<GLUE>();
    via1->appleTalkDebugEnabled = true;
    test_rom(0x46CAA, 0x46D5A, {});
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(SCBI_CHECK)
std::function<void()> test1(uint8_t v) {
    switch(v) {
    case 0:
        return []() {
            cpu.D[4] = 0;
            cpu.PC = cpu.A[6];
        };
    case 1:
        return []() {
            cpu.D[4] = (cpu.D[3] & 0xffff) == 0x78 ? 0x53434249 : 0;
            cpu.PC = cpu.A[6];
        };
    case 2:
        return []() {
            cpu.D[4] = (cpu.D[3] & 0xffff) == 0xF8 ? 0x53434249 : 0;
            cpu.PC = cpu.A[6];
        };
    }
    return []() {};
}
BOOST_AUTO_TEST_CASE(Not) { test_rom(0x46CD8, 0x46D7C, {{0x4721C, test1(0)}}); }
BOOST_AUTO_TEST_CASE(_78) { test_rom(0x46CD8, 0x46CF8, {{0x4721C, test1(1)}}); }
static void getHwInfo() {
    BOOST_ASSERT(cpu.D[2] == 0);
    cpu.A[0] = 0x40803568;
    cpu.A[1] = 0x408038BC;
    cpu.D[0] = 0x07A31807;
    cpu.D[1] = 0x00040924;
    cpu.D[2] = 0xDC001408;
    cpu.PC = cpu.A[6];
}
BOOST_AUTO_TEST_CASE(_F8) {
    test_rom(0x46CD8, 0x46D06,
             {
                 {0x4721C, test1(2)},
                 {0x470BA, getHwInfo},
             });
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(_470BA) {
    cpu.A[0] = 0x408032D8;
    test_rom(0x470BA, 0x2F18, {});
    BOOST_TEST(cpu.I == 7);
}

BOOST_AUTO_TEST_SUITE(_477C4)
struct DummyIOP2 : public IOP {
    uint8_t readBase(uint8_t) override { return 0; }
    void writeBase(uint8_t, uint8_t) override {}
};
struct DummyMB : public IO_BUS {
    DummyIOP2 d;
    uint8_t Read8(uint32_t addr) override { return d.read(addr >> 1 & 0x1f); }
    void Write8(uint32_t addr, uint8_t value) { d.write(addr>>1 & 0x1f, value);}
};
BOOST_AUTO_TEST_CASE(success) {
    cpu.A[3] = 0x50F0C020;
    io = std::make_unique<DummyMB>();

    test_rom(0x477C4, 0x477FE, {});
    BOOST_TEST(cpu.D[0] == 0);
    std::vector<uint8_t> expected = {0xA9, 0x81, 0x8D, 0x30, 0xF0, 0xA9,
                                     0x44, 0x8D, 0x31, 0xF0, 0x80, 0xFE,
                                     0xEE, 0x7F, 0xEE, 0x7F, 0xEE, 0x7F};
    auto iop = &static_cast<DummyMB*>(io.get())->d;
    for(unsigned int i = 0; i < expected.size(); ++i) {
        BOOST_TEST(iop->MEM[0x7FEE + i] == expected[i]);
    }
}
struct DummyIOP3 {
    std::deque<uint8_t> vs = {0xA9, 0x81, 0xff, 0xff};
    uint8_t read(uint32_t) {
        auto p = vs[0];
        vs.pop_front();
        return p;
    }
    void write(uint32_t, uint8_t)  {}
};
struct DummyMB2 : public IO_BUS {
    DummyIOP3 d;
    uint8_t Read8(uint32_t addr) override { return d.read(addr >> 1 & 0x1f); }
    void Write8(uint32_t addr, uint8_t value) { d.write(addr>>1 & 0x1f, value);}
};
BOOST_AUTO_TEST_CASE(fail) {
    cpu.A[3] = 0x50F0C020;
     io = std::make_unique<DummyMB2>();

    test_rom(0x477C4, 0x477FE, {});
    BOOST_TEST(cpu.D[0] == 16);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(_4AE28)
static void getHwInfo2_1() {
    cpu.A[0] = 0x408032D8;
    cpu.A[1] = 0x408036BC;
    cpu.D[0] = 0x00001F3F;
    cpu.D[1] = 0;
    cpu.PC = cpu.A[6];
}
static void getHwInfo_2() {
    BOOST_ASSERT(cpu.D[2] == 0);
    cpu.A[0] = 0x40803568;
    cpu.A[1] = 0x408038BC;
    cpu.D[0] = 0x07A31807;
    cpu.D[1] = 0x00040924;
    cpu.D[2] = 0xDC001408;
    cpu.PC = cpu.A[6];
}
BOOST_AUTO_TEST_CASE(non_iop) {
    test_rom(0x4AE28, 0x4AE52, {{0x470BA, getHwInfo2_1}});
    BOOST_TEST(cpu.A[3] == 0x50F04000);
    BOOST_TEST(cpu.D[3] == 0);
}

BOOST_AUTO_TEST_CASE(iop) {
    test_rom(0x4AE28, 0x4AE52, {{0x470BA, getHwInfo_2}});
    BOOST_TEST(cpu.A[3] == 0x50F0C020);
    BOOST_TEST(cpu.D[3] == 0);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()