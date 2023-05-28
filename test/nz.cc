#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "inline.hpp"

BOOST_AUTO_TEST_SUITE( test_b )
BOOST_AUTO_TEST_CASE( positive )
{
    TEST_B(2);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_CASE( zero )
{
    TEST_B(0);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE( negative )
{
    TEST_B(-1);
    BOOST_TEST(cpu.N);
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE( test_w )
BOOST_AUTO_TEST_CASE( positive )
{
    TEST_W(2000);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_CASE( zero )
{
    TEST_W(0);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE( negative )
{
    TEST_W(-2000);
    BOOST_TEST(cpu.N);
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE( test_l )
BOOST_AUTO_TEST_CASE( positive )
{
    TEST_L(200000);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_CASE( zero )
{
    TEST_L(0);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_CASE( negative )
{
    TEST_L(-200000);
    BOOST_TEST(cpu.N);
    BOOST_TEST(!cpu.Z);
}

BOOST_AUTO_TEST_SUITE_END()