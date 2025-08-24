#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "mpfr.h"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <math.h>
namespace bdata = boost::unit_test::data;
struct F_FINT {
    F_FINT() {
        // FINT.X %FP3, %FP2
        TEST::SET_W(0, 0171000);
        TEST::SET_W(2, 0B0000001 | 3 << 10 | 2 << 7);
        TEST::SET_W(4, TEST_BREAK);

        jit_compile(0, 6);
    }
};

BOOST_FIXTURE_TEST_SUITE(FINT, Prepare, *boost::unit_test::fixture<F_FINT>())
BOOST_AUTO_TEST_CASE(qnan) { qnan_test(0); }

BOOST_AUTO_TEST_CASE(snan) { snan_test(0); }
BOOST_DATA_TEST_CASE(inf, sg_v, sg) {
    auto fpu = FPU_P();
    TEST::SET_FP(3, copysign(INFINITY, sg));
    run_test(0);
    BOOST_TEST(mpfr_inf_p(fpu->FP[2]));
    BOOST_TEST(!!mpfr_signbit(fpu->FP[2]) == !!signbit(sg));
}

BOOST_DATA_TEST_CASE(zero, sg_v, sg) {
    auto fpu = FPU_P();
    TEST::SET_FP(3, copysign(0.0, sg));
    run_test(0);
    BOOST_TEST(mpfr_zero_p(fpu->FP[2]));
    BOOST_TEST(!!mpfr_signbit(fpu->FP[2]) == !!signbit(sg));
}
static double args[] = {1.3, 1.5, 2.5, 2.7, -1.3, -1.5, -2.5, -2.7};
static double expected_u[] = {2.0, 2.0, 3.0, 3.0, -1.0, -1.0, -2.0, -2.0};
BOOST_DATA_TEST_CASE(to_p_inf, args ^ expected_u, a, e) {
    auto fpu = FPU_P();
    TEST::SET_FP(3, a);
    fpu->FPCR.RND = MPFR_RNDU;
    run_test(0);
    BOOST_TEST(TEST::GET_FP(2) == e);
}

static double expected_d[] = {1.0, 1.0, 2.0, 2.0, -2.0, -2.0, -3.0, -3.0};
BOOST_DATA_TEST_CASE(to_m_inf, args ^ expected_d, a, e) {
    auto fpu = FPU_P();
    TEST::SET_FP(3, a);
    fpu->FPCR.RND = MPFR_RNDD;
    run_test(0);
    BOOST_TEST(TEST::GET_FP(2) == e);
}

static double expected_z[] = {1.0, 1.0, 2.0, 2.0, -1.0, -1.0, -2.0, -2.0};
BOOST_DATA_TEST_CASE(to_zero, args ^ expected_z, a, e) {
    auto fpu = FPU_P();
    TEST::SET_FP(3, a);
    fpu->FPCR.RND = MPFR_RNDZ;
    run_test(0);
    BOOST_TEST(TEST::GET_FP(2) == e);
}

static double expected_n[] = {1.0, 2.0, 2.0, 3.0, -1.0, -2.0, -2.0, -3.0};
BOOST_DATA_TEST_CASE(nearest, args ^ expected_n, a, e) {
    auto fpu = FPU_P();
    TEST::SET_FP(3, a);
    fpu->FPCR.RND = MPFR_RNDN;
    run_test(0);
    BOOST_TEST(TEST::GET_FP(2) == e);
}

BOOST_AUTO_TEST_SUITE_END()
