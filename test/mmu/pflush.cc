#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(PFLUSHN, Prepare)
BOOST_AUTO_TEST_CASE(err) {
    cpu.S = false;
    TEST::SET_W(0, 0172403);
    BOOST_TEST(run_test() == 8);
}

BOOST_AUTO_TEST_CASE(traced) {
    cpu.S = true;
    cpu.T = 1;
    cpu.DFC = 2;
    cpu.A[3] = 0x1 << 12;
    TEST::SET_W(0, 0172403);
    BOOST_TEST(run_test() == 9);
}

BOOST_AUTO_TEST_CASE(user) {
    cpu.S = true;
    cpu.DFC = 2;
    cpu.s_atc[1] = {2, 0, false, 0, false, false, true};
    cpu.s_atc[2] = {2, 0, false, 0, false, false, true};
    cpu.u_atc[1] = {3, 0, false, 0, false, false, true};
    cpu.u_atc[2] = {3, 0, false, 0, false, false, true};
    cpu.sg_atc[1] = {4, 0, false, 0, false, false, true};
    cpu.sg_atc[2] = {4, 0, false, 0, false, false, true};
    cpu.ug_atc[1] = {5, 0, false, 0, false, false, true};
    cpu.ug_atc[2] = {5, 0, false, 0, false, false, true};
    cpu.A[3] = 0x1 << 12;
    TEST::SET_W(0, 0172403);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(!cpu.u_atc.contains(1));
    BOOST_TEST(cpu.s_atc.contains(1));
    BOOST_TEST(cpu.ug_atc.contains(1));
    BOOST_TEST(cpu.sg_atc.contains(1));
    BOOST_TEST(cpu.u_atc.contains(2));
    BOOST_TEST(cpu.s_atc.contains(2));
    BOOST_TEST(cpu.ug_atc.contains(2));
    BOOST_TEST(cpu.sg_atc.contains(2));
}

BOOST_AUTO_TEST_CASE(sys) {
    cpu.S = true;
    cpu.DFC = 5;
    cpu.s_atc[1] = {2, 0, false, 0, false, false, true};
    cpu.s_atc[2] = {2, 0, false, 0, false, false, true};
    cpu.u_atc[1] = {3, 0, false, 0, false, false, true};
    cpu.u_atc[2] = {3, 0, false, 0, false, false, true};
    cpu.sg_atc[1] = {4, 0, false, 0, false, false, true};
    cpu.sg_atc[2] = {4, 0, false, 0, false, false, true};
    cpu.ug_atc[1] = {5, 0, false, 0, false, false, true};
    cpu.ug_atc[2] = {5, 0, false, 0, false, false, true};
    cpu.A[3] = 0x1 << 12;
    TEST::SET_W(0, 0172403);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.u_atc.contains(1));
    BOOST_TEST(!cpu.s_atc.contains(1));
    BOOST_TEST(cpu.ug_atc.contains(1));
    BOOST_TEST(cpu.sg_atc.contains(1));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(PFLUSH, Prepare)
BOOST_AUTO_TEST_CASE(err) {
    cpu.S = false;
    TEST::SET_W(0, 0172413);
    BOOST_TEST(run_test() == 8);
}

BOOST_AUTO_TEST_CASE(traced) {
    cpu.S = true;
    cpu.T = 1;
    cpu.DFC = 2;
    cpu.A[3] = 0x1 << 12;
    TEST::SET_W(0, 0172413);
    BOOST_TEST(run_test() == 9);
}

BOOST_AUTO_TEST_CASE(user) {
    cpu.S = true;
    cpu.DFC = 2;
    cpu.s_atc[1] = {2, 0, false, 0, false, false, true};
    cpu.s_atc[2] = {2, 0, false, 0, false, false, true};
    cpu.u_atc[1] = {3, 0, false, 0, false, false, true};
    cpu.u_atc[2] = {3, 0, false, 0, false, false, true};
    cpu.sg_atc[1] = {4, 0, false, 0, false, false, true};
    cpu.sg_atc[2] = {4, 0, false, 0, false, false, true};
    cpu.ug_atc[1] = {5, 0, false, 0, false, false, true};
    cpu.ug_atc[2] = {5, 0, false, 0, false, false, true};
    cpu.A[3] = 0x1 << 12;
    TEST::SET_W(0, 0172413);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(!cpu.u_atc.contains(1));
    BOOST_TEST(cpu.s_atc.contains(1));
    BOOST_TEST(!cpu.ug_atc.contains(1));
    BOOST_TEST(cpu.sg_atc.contains(1));
    BOOST_TEST(cpu.u_atc.contains(2));
    BOOST_TEST(cpu.s_atc.contains(2));
    BOOST_TEST(cpu.ug_atc.contains(2));
    BOOST_TEST(cpu.sg_atc.contains(2));
}

BOOST_AUTO_TEST_CASE(sys) {
    cpu.S = true;
    cpu.DFC = 5;
    cpu.s_atc[1] = {2, 0, false, 0, false, false, true};
    cpu.s_atc[2] = {2, 0, false, 0, false, false, true};
    cpu.u_atc[1] = {3, 0, false, 0, false, false, true};
    cpu.u_atc[2] = {3, 0, false, 0, false, false, true};
    cpu.sg_atc[1] = {4, 0, false, 0, false, false, true};
    cpu.sg_atc[2] = {4, 0, false, 0, false, false, true};
    cpu.ug_atc[1] = {5, 0, false, 0, false, false, true};
    cpu.ug_atc[2] = {5, 0, false, 0, false, false, true};
    cpu.A[3] = 0x1 << 12;
    TEST::SET_W(0, 0172413);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.u_atc.contains(1));
    BOOST_TEST(!cpu.s_atc.contains(1));
    BOOST_TEST(cpu.ug_atc.contains(1));
    BOOST_TEST(!cpu.sg_atc.contains(1));
    BOOST_TEST(cpu.u_atc.contains(2));
    BOOST_TEST(cpu.s_atc.contains(2));
    BOOST_TEST(cpu.ug_atc.contains(2));
    BOOST_TEST(cpu.sg_atc.contains(2));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(PFLUSHAN, Prepare)
BOOST_AUTO_TEST_CASE(err) {
    cpu.S = false;
    TEST::SET_W(0, 0172423);
    BOOST_TEST(run_test() == 8);
}

BOOST_AUTO_TEST_CASE(traced) {
    cpu.S = true;
    cpu.T = 1;
    cpu.DFC = 2;
    cpu.A[3] = 0x1 << 12;
    TEST::SET_W(0, 0172423);
    BOOST_TEST(run_test() == 9);
}

BOOST_AUTO_TEST_CASE(user) {
    cpu.S = true;
    cpu.DFC = 2;
    cpu.s_atc[1] = {2, 0, false, 0, false, false, true};
    cpu.s_atc[2] = {2, 0, false, 0, false, false, true};
    cpu.u_atc[1] = {3, 0, false, 0, false, false, true};
    cpu.u_atc[2] = {3, 0, false, 0, false, false, true};
    cpu.sg_atc[1] = {4, 0, false, 0, false, false, true};
    cpu.sg_atc[2] = {4, 0, false, 0, false, false, true};
    cpu.ug_atc[1] = {5, 0, false, 0, false, false, true};
    cpu.ug_atc[2] = {5, 0, false, 0, false, false, true};
    cpu.A[3] = 0x1 << 12;
    TEST::SET_W(0, 0172423);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.u_atc.empty());
    BOOST_TEST(cpu.ug_atc.contains(1));
    BOOST_TEST(cpu.s_atc.contains(1));
    BOOST_TEST(cpu.sg_atc.contains(1));
    BOOST_TEST(cpu.ug_atc.contains(2));
    BOOST_TEST(cpu.s_atc.contains(2));
    BOOST_TEST(cpu.sg_atc.contains(2));
}

BOOST_AUTO_TEST_CASE(sys) {
    cpu.S = true;
    cpu.DFC = 5;
    cpu.s_atc[1] = {2, 0, false, 0, false, false, true};
    cpu.s_atc[2] = {2, 0, false, 0, false, false, true};
    cpu.u_atc[1] = {3, 0, false, 0, false, false, true};
    cpu.u_atc[2] = {3, 0, false, 0, false, false, true};
    cpu.sg_atc[1] = {4, 0, false, 0, false, false, true};
    cpu.sg_atc[2] = {4, 0, false, 0, false, false, true};
    cpu.ug_atc[1] = {5, 0, false, 0, false, false, true};
    cpu.ug_atc[2] = {5, 0, false, 0, false, false, true};
    cpu.A[3] = 0x1 << 12;
    TEST::SET_W(0, 0172423);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.s_atc.empty());
    BOOST_TEST(cpu.ug_atc.contains(1));
    BOOST_TEST(cpu.u_atc.contains(1));
    BOOST_TEST(cpu.sg_atc.contains(1));
    BOOST_TEST(cpu.u_atc.contains(2));
    BOOST_TEST(cpu.ug_atc.contains(2));
    BOOST_TEST(cpu.sg_atc.contains(2));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(PFLUSHA, Prepare)
BOOST_AUTO_TEST_CASE(err) {
    cpu.S = false;
    TEST::SET_W(0, 0172433);
    BOOST_TEST(run_test() == 8);
}

BOOST_AUTO_TEST_CASE(traced) {
    cpu.S = true;
    cpu.T = 1;
    cpu.DFC = 2;
    cpu.A[3] = 0x1 << 12;
    TEST::SET_W(0, 0172433);
    BOOST_TEST(run_test() == 9);
}

BOOST_AUTO_TEST_CASE(user) {
    cpu.S = true;
    cpu.DFC = 2;
    cpu.s_atc[1] = {2, 0, false, 0, false, false, true};
    cpu.s_atc[2] = {2, 0, false, 0, false, false, true};
    cpu.u_atc[1] = {3, 0, false, 0, false, false, true};
    cpu.u_atc[2] = {3, 0, false, 0, false, false, true};
    cpu.sg_atc[1] = {4, 0, false, 0, false, false, true};
    cpu.sg_atc[2] = {4, 0, false, 0, false, false, true};
    cpu.ug_atc[1] = {5, 0, false, 0, false, false, true};
    cpu.ug_atc[2] = {5, 0, false, 0, false, false, true};
    cpu.A[3] = 0x1 << 12;
    TEST::SET_W(0, 0172433);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.PC == 2);
    BOOST_TEST(cpu.u_atc.empty());
    BOOST_TEST(cpu.ug_atc.empty());
    BOOST_TEST(cpu.s_atc.contains(1));
    BOOST_TEST(cpu.sg_atc.contains(1));
    BOOST_TEST(cpu.s_atc.contains(2));
    BOOST_TEST(cpu.sg_atc.contains(2));
}

BOOST_AUTO_TEST_CASE(sys) {
    cpu.S = true;
    cpu.DFC = 5;
    cpu.s_atc[1] = {2, 0, false, 0, false, false, true};
    cpu.s_atc[2] = {2, 0, false, 0, false, false, true};
    cpu.u_atc[1] = {3, 0, false, 0, false, false, true};
    cpu.u_atc[2] = {3, 0, false, 0, false, false, true};
    cpu.sg_atc[1] = {4, 0, false, 0, false, false, true};
    cpu.sg_atc[2] = {4, 0, false, 0, false, false, true};
    cpu.ug_atc[1] = {5, 0, false, 0, false, false, true};
    cpu.ug_atc[2] = {5, 0, false, 0, false, false, true};
    cpu.A[3] = 0x1 << 12;
    TEST::SET_W(0, 0172433);
    BOOST_TEST(run_test() == 0);
    BOOST_TEST(cpu.s_atc.empty());
    BOOST_TEST(cpu.sg_atc.empty());
    BOOST_TEST(cpu.ug_atc.contains(1));
    BOOST_TEST(cpu.u_atc.contains(1));
    BOOST_TEST(cpu.u_atc.contains(2));
    BOOST_TEST(cpu.ug_atc.contains(2));
}

BOOST_AUTO_TEST_SUITE_END()
