#define BOOST_TEST_MODULE test
#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include <boost/test/unit_test.hpp>
#include <errno.h>
#include <fcntl.h>
#include <memory>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
std::vector<uint8_t> RAM;
const std::uint8_t *ROM;
size_t ROMSize;
Cpu cpu;
void initBus();
void init_fpu();

extern bool rom_is_overlay;
struct MyGlobalFixture {
    MyGlobalFixture() {
        RAM.resize(64 * 1024 * 1024);
        int fd = open("../quadra700.rom", O_RDONLY);
        if(fd == -1) {
            perror("cannot open rom");
        }
        struct stat sb;

        if(fstat(fd, &sb) == -1) /* To obtain file size */
            perror("fstat");
        ROMSize = sb.st_size;
        ROM = static_cast<uint8_t *>(
            mmap(nullptr, ROMSize, PROT_READ, MAP_SHARED, fd, 0));
        initBus();
        init_fpu();
        rom_is_overlay = false;
    }
};

BOOST_TEST_GLOBAL_FIXTURE(MyGlobalFixture);
