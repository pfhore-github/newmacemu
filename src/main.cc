#include "68040.hpp"
#include "exception.hpp"
#include "proto.hpp"
#include "memory.hpp"
#include <errno.h>
#include <fcntl.h>
#include <memory>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
std::vector<uint8_t> RAM;
const uint8_t *ROM;
size_t ROMSize;
Cpu cpu;
void initBus();
void init_fpu();
std::pair<std::function<void()>, int> decode();
int main() {
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
    cpu.PC = 0x2A;
    cpu.movem_run = false;
    for(;;) {
        if(setjmp(cpu.ex_buf) == 0) {            
            cpu.af_value.ea = 0;
            cpu.must_trace = cpu.T == 2;
            auto o = cpu.icache.find(cpu.PC);
            if(o == cpu.icache.end()) {
                try {
                    auto next = decode();
                    std::tie(o, std::ignore) = cpu.icache.emplace(cpu.PC, next);
                } catch(DecodeError &e) {
                    ILLEGAL_OP();
                };
            }
            cpu.nextpc = cpu.PC + 2 + o->second.second;
            o->second.first();
            cpu.PC = cpu.nextpc;
            if( cpu.T == 2) {
                TRACE();
            }
        } else {
           do_exception();
        }
    }
    return 0;
}
