#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
uint32_t ptest(uint32_t addr, bool sys, bool code, bool W);
namespace bdata = boost::unit_test::data;
struct AccessFault {};

BOOST_FIXTURE_TEST_SUITE(PSERACH, Prepare)
BOOST_AUTO_TEST_CASE(User) {
    cpu.TCR_E = true;
    cpu.URP = 0x1000;
    cpu.SRP = 0;
    TEST::SET_L(0x1004, 0x2002);
    TEST::SET_L(0x2008, 0x3002);
    TEST::SET_L(0x300C, 0x4001 | 2 << 5 | 1 << 8);
    uint32_t vaddr = 1 << 13 | 2 << 6 | 3;
    ptest(vaddr, false, false, false);
    auto found = cpu.u_atc.find(vaddr);
    BOOST_REQUIRE(!!(found != cpu.u_atc.end()));
    BOOST_TEST(found->second.paddr == 0x4000);
    BOOST_TEST(found->second.U == 1);
    BOOST_TEST(!found->second.S);
    BOOST_TEST(!found->second.M);
    BOOST_TEST(!found->second.W);
    BOOST_TEST(found->second.R);
}

BOOST_AUTO_TEST_CASE(System) {
    cpu.TCR_E = true;
    cpu.URP = 0;
    cpu.SRP = 0x1000;
    TEST::SET_L(0x1004, 0x2002);
    TEST::SET_L(0x2008, 0x3002);
    TEST::SET_L(0x300C, 0x4001 | 2 << 5 | 1 << 8 | 1 << 7);
    uint32_t vaddr = 1 << 13 | 2 << 6 | 3;
    ptest(vaddr, true, false, false);
    auto found = cpu.s_atc.find(vaddr);
    BOOST_TEST(TEST::GET_L(0x1004) == 0x200A);
    BOOST_TEST(TEST::GET_L(0x2008) == 0x300A);
    BOOST_TEST(TEST::GET_L(0x300C) == (0x4009 | 2 << 5 | 1 << 8 | 1 << 7));
    BOOST_REQUIRE(!!(found != cpu.s_atc.end()));
    BOOST_TEST(found->second.paddr == 0x4000);
    BOOST_TEST(found->second.U == 1);
    BOOST_TEST(found->second.S);
    BOOST_TEST(!found->second.M);
    BOOST_TEST(!found->second.W);
    BOOST_TEST(found->second.R);
}

BOOST_AUTO_TEST_CASE(P) {
    cpu.TCR_E = true;
    cpu.TCR_P = true;
    cpu.URP = 0x1000;
    cpu.SRP = 0;
    TEST::SET_L(0x1004, 0x2002);
    TEST::SET_L(0x2008, 0x3002);
    TEST::SET_L(0x300C, 0x4001);
    uint32_t vaddr = 1 << 13 | 2 << 6 | 3 << 1 | 1;
    ptest(vaddr, false, false, false);
    auto found = cpu.u_atc.find(vaddr);
    BOOST_TEST(TEST::GET_L(0x1004) == 0x200A);
    BOOST_TEST(TEST::GET_L(0x2008) == 0x300A);
    BOOST_TEST(TEST::GET_L(0x300C) == 0x4009);
    BOOST_REQUIRE(!!(found != cpu.u_atc.end()));
    BOOST_TEST(found->second.paddr == 0x4000);
    BOOST_TEST(found->second.R);
}

BOOST_AUTO_TEST_CASE(Indirect) {
    cpu.TCR_E = true;
    cpu.TCR_P = false;
    cpu.URP = 0x1000;
    cpu.SRP = 0;
    TEST::SET_L(0x1004, 0x2002);
    TEST::SET_L(0x2008, 0x3002);
    TEST::SET_L(0x300C, 0x4002);
    TEST::SET_L(0x4000, 0x5001);
    uint32_t vaddr = 1 << 13 | 2 << 6 | 3;
    ptest(vaddr, false, false, false);
    auto found = cpu.u_atc.find(vaddr);
    BOOST_TEST(TEST::GET_L(0x1004) == 0x200A);
    BOOST_TEST(TEST::GET_L(0x2008) == 0x300A);
    BOOST_TEST(TEST::GET_L(0x4000) == 0x5009);
    BOOST_REQUIRE(!!(found != cpu.u_atc.end()));
    BOOST_TEST(found->second.paddr == 0x5000);
    BOOST_TEST(found->second.R);
}
BOOST_AUTO_TEST_SUITE(RootNotFound)
BOOST_DATA_TEST_CASE(User, bdata::xrange(2), P) {
    cpu.TCR_E = true;
    cpu.TCR_P = P;
    cpu.URP = 0x1000;
    TEST::SET_L(0x1004, 0x2000);
    uint32_t vaddr = 1 << 13;
    ptest(vaddr, false, false, false);
    auto found = cpu.ug_atc.find(vaddr);
    BOOST_REQUIRE(!!(found != cpu.ug_atc.end()));
    BOOST_TEST(!found->second.R);
}

BOOST_DATA_TEST_CASE(System, bdata::xrange(2), P) {
    cpu.TCR_E = true;
    cpu.TCR_P = P;
    cpu.SRP = 0x1000;
    TEST::SET_L(0x1004, 0x2000);
    uint32_t vaddr = 1 << 13;
    ptest(vaddr, true, false, false);
    auto found = cpu.sg_atc.find(vaddr);
    BOOST_REQUIRE(!!(found != cpu.sg_atc.end()));
    BOOST_TEST(!found->second.R);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_DATA_TEST_CASE(RootNotFound, bdata::xrange(2), P) {
    cpu.TCR_E = true;
    cpu.TCR_P = P;
    cpu.URP = 0x1000;
    cpu.SRP = 0;
    TEST::SET_L(0x1004, 0x2000);
    uint32_t vaddr = 1 << 13;
    ptest(vaddr, false, false, false);
    auto found = cpu.ug_atc.find(vaddr);
    BOOST_REQUIRE(!!(found != cpu.ug_atc.end()));
    BOOST_TEST(!found->second.R);
}
BOOST_DATA_TEST_CASE(PtrNotFound, bdata::xrange(2), P) {
    cpu.TCR_E = true;
    cpu.TCR_P = P;
    cpu.URP = 0x1000;
    cpu.SRP = 0;
    TEST::SET_L(0x1004, 0x2002);
    TEST::SET_L(0x2008, 0x3000);
    uint32_t vaddr = 1 << 13 | 2 << 6;
    ptest(vaddr, false, false, false);
    auto found = cpu.ug_atc.find(vaddr);
    BOOST_REQUIRE(!!(found != cpu.ug_atc.end()));
    BOOST_TEST(!found->second.R);
}

BOOST_AUTO_TEST_CASE(PageNotFound) {
    cpu.TCR_E = true;
    cpu.TCR_P = false;
    cpu.URP = 0x1000;
    cpu.SRP = 0;
    TEST::SET_L(0x1004, 0x2002);
    TEST::SET_L(0x2008, 0x3002);
    TEST::SET_L(0x300C, 0x4000);
    uint32_t vaddr = 1 << 13 | 2 << 6 | 3;
    ptest(vaddr, false, false, false);
    auto found = cpu.ug_atc.find(vaddr);
    BOOST_REQUIRE(!!(found != cpu.ug_atc.end()));
    BOOST_TEST(!found->second.R);
}

BOOST_AUTO_TEST_CASE(PPageNotFound) {
    cpu.TCR_E = true;
    cpu.TCR_P = true;
    cpu.URP = 0x1000;
    cpu.SRP = 0;
    TEST::SET_L(0x1004, 0x2002);
    TEST::SET_L(0x2008, 0x3002);
    TEST::SET_L(0x300C, 0x4000);
    uint32_t vaddr = 1 << 13 | 2 << 6 | 3 << 1 | 1;
    ptest(vaddr, false, false, false);
    auto found = cpu.ug_atc.find(vaddr);
    BOOST_REQUIRE(!!(found != cpu.ug_atc.end()));
    BOOST_TEST(!found->second.R);
}

BOOST_AUTO_TEST_SUITE(W)
BOOST_AUTO_TEST_CASE(Root) {
    cpu.TCR_E = true;
    cpu.TCR_P = false;
    cpu.URP = 0x1000;
    cpu.SRP = 0;
    TEST::SET_L(0x1004, 0x2006);
    TEST::SET_L(0x2008, 0x3002);
    TEST::SET_L(0x300C, 0x4001 | 2 << 5 | 1 << 8);
    uint32_t vaddr = 1 << 13 | 2 << 6 | 3;
    ptest(vaddr, false, false, true);
    auto found = cpu.u_atc.find(vaddr);
    BOOST_REQUIRE(!!(found != cpu.u_atc.end()));
    BOOST_TEST(found->second.W);
}

BOOST_AUTO_TEST_CASE(Ptr) {
    cpu.TCR_E = true;
    cpu.TCR_P = false;
    cpu.URP = 0x1000;
    cpu.SRP = 0;
    TEST::SET_L(0x1004, 0x2002);
    TEST::SET_L(0x2008, 0x3006);
    TEST::SET_L(0x300C, 0x4001 | 2 << 5 | 1 << 8);
    uint32_t vaddr = 1 << 13 | 2 << 6 | 3;
    ptest(vaddr, false, false, true);
    auto found = cpu.u_atc.find(vaddr);
    BOOST_REQUIRE(!!(found != cpu.u_atc.end()));
    BOOST_TEST(found->second.W);
}

BOOST_AUTO_TEST_CASE(Page) {
    cpu.TCR_E = true;
    cpu.URP = 0x1000;
    cpu.SRP = 0;
    TEST::SET_L(0x1004, 0x2002);
    TEST::SET_L(0x2008, 0x3002);
    TEST::SET_L(0x300C, 0x4005 | 2 << 5 | 1 << 8);
    uint32_t vaddr = 1 << 13 | 2 << 6 | 3;
    ptest(vaddr, false, false, true);
    auto found = cpu.u_atc.find(vaddr);
    BOOST_REQUIRE(!!(found != cpu.u_atc.end()));
    BOOST_TEST(found->second.W);
}

BOOST_AUTO_TEST_CASE(MUpdate) {
    cpu.TCR_E = true;
    cpu.URP = 0x1000;
    cpu.SRP = 0;
    TEST::SET_L(0x1004, 0x2002);
    TEST::SET_L(0x2008, 0x3002);
    TEST::SET_L(0x300C, 0x4002 | 2 << 5 | 1 << 8);
    uint32_t vaddr = 1 << 13 | 2 << 6 | 3;
    cpu.u_atc[vaddr] = {
        .paddr = 0,
        .U = 0,
        .S = false,
        .CM = 0,
        .M = false,
        .W = false,
        .R = true,
    };
    ptest(vaddr, false, false, true);
    auto found = cpu.u_atc.find(vaddr);
    BOOST_REQUIRE(!!(found != cpu.u_atc.end()));
    BOOST_TEST(found->second.M);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(B) {
    cpu.TCR_E = true;
    cpu.URP = 0x1000;
    cpu.SRP = 0;
    TEST::SET_L(0x1004, 0xFFFFFFF2);
    uint32_t vaddr = 1 << 13;
    BOOST_TEST(ptest(vaddr, false, false, false) & 1 << 11);
}

BOOST_AUTO_TEST_CASE(U) {
    cpu.TCR_E = true;
    cpu.URP = 0x1000;
    cpu.SRP = 0;
    TEST::SET_L(0x1004, 0x2002);
    TEST::SET_L(0x2008, 0x3002);
    TEST::SET_L(0x300C, 0x4001);
    uint32_t vaddr = 1 << 13 | 2 << 6 | 3;
    ptest(vaddr, false, false, false);
    BOOST_TEST(TEST::GET_L(0x1004), 0x200A);
    BOOST_TEST(TEST::GET_L(0x2008), 0x300A);
    BOOST_TEST(TEST::GET_L(0x300C), 0x4009);
}

BOOST_AUTO_TEST_CASE(M) {
    cpu.TCR_E = true;
    cpu.URP = 0x1000;
    cpu.SRP = 0;
    TEST::SET_L(0x1004, 0x2002);
    TEST::SET_L(0x2008, 0x3002);
    TEST::SET_L(0x300C, 0x4001);
    uint32_t vaddr = 1 << 13 | 2 << 6 | 3;
    ptest(vaddr, false, false, true);
    BOOST_TEST(TEST::GET_L(0x1004), 0x2012);
    BOOST_TEST(TEST::GET_L(0x2008), 0x3012);
    BOOST_TEST(TEST::GET_L(0x300C), 0x4011);
}

BOOST_DATA_TEST_CASE(CM, bdata::xrange(4), cm) {
    cpu.TCR_E = true;
    cpu.URP = 0x1000;
    cpu.SRP = 0;
    TEST::SET_L(0x1004, 0x2002);
    TEST::SET_L(0x2008, 0x3002);
    TEST::SET_L(0x300C, 0x4001 | cm << 5);
    uint32_t vaddr = 1 << 13 | 2 << 6 | 3;
    ptest(vaddr, false, false, false);
    auto found = cpu.u_atc.find(vaddr);
    BOOST_TEST(!!(found != cpu.u_atc.end()));
    BOOST_TEST(found->second.CM == cm);
}

BOOST_AUTO_TEST_CASE(S) {
    cpu.TCR_E = true;
    cpu.URP = 0x1000;
    cpu.SRP = 0;
    TEST::SET_L(0x1004, 0x2002);
    TEST::SET_L(0x2008, 0x3002);
    TEST::SET_L(0x300C, 0x4001 | 1 << 7);
    uint32_t vaddr = 1 << 13 | 2 << 6 | 3;
    ptest(vaddr, false, false, false);
    auto found = cpu.u_atc.find(vaddr);
    BOOST_TEST(!!(found != cpu.u_atc.end()));
    BOOST_TEST(found->second.S);
}

BOOST_DATA_TEST_CASE(Ux, bdata::xrange(4), x) {
    cpu.TCR_E = true;
    cpu.URP = 0x1000;
    cpu.SRP = 0;
    TEST::SET_L(0x1004, 0x2002);
    TEST::SET_L(0x2008, 0x3002);
    TEST::SET_L(0x300C, 0x4001 | x << 8);
    uint32_t vaddr = 1 << 13 | 2 << 6 | 3;
    ptest(vaddr, false, false, false);
    auto found = cpu.u_atc.find(vaddr);
    BOOST_TEST(!!(found != cpu.u_atc.end()));
    BOOST_TEST(found->second.U == x);
}

BOOST_AUTO_TEST_CASE(G) {
    cpu.TCR_E = true;
    cpu.URP = 0x1000;
    cpu.SRP = 0;
    TEST::SET_L(0x1004, 0x2002);
    TEST::SET_L(0x2008, 0x3002);
    TEST::SET_L(0x300C, 0x4001 | 1 << 10);
    uint32_t vaddr = 1 << 13 | 2 << 6 | 3;
    ptest(vaddr, false, false, false);
    auto found = cpu.ug_atc.find(vaddr);
    BOOST_TEST(!!(found != cpu.ug_atc.end()));
}

BOOST_AUTO_TEST_CASE(SG) {
    cpu.TCR_E = true;
    cpu.URP = 0;
    cpu.SRP = 0x1000;
    TEST::SET_L(0x1004, 0x2002);
    TEST::SET_L(0x2008, 0x3002);
    TEST::SET_L(0x300C, 0x4001 | 1 << 10);
    uint32_t vaddr = 1 << 13 | 2 << 6 | 3;
    ptest(vaddr, true, false, false);
    auto found = cpu.sg_atc.find(vaddr);
    BOOST_TEST(!!(found != cpu.sg_atc.end()));
}

BOOST_AUTO_TEST_SUITE_END()