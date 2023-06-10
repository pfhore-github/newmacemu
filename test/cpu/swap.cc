#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(SWAP, Prepare)
BOOST_AUTO_TEST_CASE(value) {
    TEST::SET_W(0, 0044100 | 2 );
    cpu.D[2] = 0x12345678;
    auto i = decode_and_run();
    BOOST_TEST(cpu.D[2] == 0x56781234);
    BOOST_TEST(!cpu.N);
    BOOST_TEST(!cpu.Z);
    BOOST_TEST(!cpu.V);
    BOOST_TEST(!cpu.C);
    BOOST_TEST(i == 0);
}

BOOST_AUTO_TEST_CASE(N) {
    TEST::SET_W(0, 0044100 | 2 );
    cpu.D[2] = 0x123489AB;
    decode_and_run();
    BOOST_TEST(cpu.N);
}

BOOST_AUTO_TEST_CASE(Z) {
    TEST::SET_W(0, 0044100 | 2 );
    cpu.D[2] = 0;
    decode_and_run();
    BOOST_TEST(cpu.Z);
}
BOOST_AUTO_TEST_SUITE_END()