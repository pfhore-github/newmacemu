#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "inline.hpp"

BOOST_AUTO_TEST_CASE( store_b )
{
    uint32_t x = 0x12345678;
    STORE_B(x, 0xab);
    BOOST_TEST(x == 0x123456ab);
}

BOOST_AUTO_TEST_CASE( store_w )
{
    uint32_t x = 0x12345678;
    STORE_W(x, 0xabcd);
    BOOST_TEST(x == 0x1234abcd);
}

