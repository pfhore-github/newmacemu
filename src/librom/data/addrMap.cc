#include "../rom_common.hpp"
#include <unordered_map>
#include <vector>
namespace LibROM {
bool check_glue(const AddrMap *addrMaps);
bool check_mdu(const AddrMap *addrMaps);
bool check_oss(const AddrMap *addrMaps);
bool check_v8(const AddrMap *addrMaps);
bool check_mcu(const AddrMap *addrMaps);
bool check_jaws(const AddrMap *addrMaps);

AddrMap addrMapGLUE{0x00001F3F,
                    0x00000000,
                    0x10,
                    0x00,
                    0xC0,
                    0x00,
                    check_glue,
                    0x04,
                    {
                        0x40800000,
                        0x58000000,
                        0x50F00000,
                        0x50F0C020,
                        0x50F0C020,
                        0x50F0C020,
                        0,
                        0,

                        0x50F10000,
                        0x50F12000,
                        0x50F06000,
                        0x50F02000,
                        0x50F14000,
                    }};

AddrMap addrMapMDU{0x0003FF3F,
                   0x00000000,
                   0x00,
                   0x00,
                   0x00,
                   0x00,
                   check_mdu,
                   0x05,
                   {
                       0x40800000,
                       0x58000000,
                       0x50F00000,
                       0x50F04000,
                       0x50F04000,
                       0x50F16000,
                       0,
                       0,
                       0x50F10000,
                       0x50F12000,
                       0x50F06000,
                       0x50F02000,
                       0x50F14000,
                       0x50F26000,
                       0x50F24000,
                       0x50F18000,
                       0x50F1E020,
                       0x50F0C020,
                   }};

AddrMap addrMapOSS{
    0x001F9007,
    0x00000004,
    0x00,
    0x00,
    0x00,
    0x00,
    check_oss,
    0x06,
    {
        0x40800000, 0x58000000, 0x50F00000, 0,          0,          0,
        0,          0,          0,          0,          0,          0,
        0x50F10000, 0,          0,          0x50F08000, 0x50F1E020, 0x50F0C020,
        0x50F1A000, 0x50F1C000, 0x50F1E000,
    }};

AddrMap addrMapV8{0x0000773F,
                  0x000000A6,
                  0x00,
                  0x00,
                  0x00,
                  0x00,
                  check_v8,
                  0x07,
                  {
                      0x00A00000,
                      0x50F80000,
                      0x50F00000,
                      0x50F04000,
                      0x50F04000,
                      0x50F16000,
                      0,
                      0,
                      0x50F10000,
                      0x50F12000,
                      0x50F06000,
                      0,
                      0x50F14000,
                      0x50F26000,
                      0x50F24000,
                  }};

AddrMap addrMapMCU{
    0x07A3181F,
    0x00000000,
    0x00,
    0xC0,
    0x00,
    0x00,
    check_mcu,
    0x08,
    {
        0x40800000, 0x58000000, 0x50F00000, 0x50F0C020, 0x50F0C020, 0x50F1E000,
        0,          0,          0,          0,          0,          0x50F02000,
        0x50F14000, 0,          0xF9800000, 0,          0x50F1E020, 0x50F0C020,
        0,          0,          0,          0x5000E000, 0,          0x50F0A000,
        0x50F0F000, 0x50F0F402, 0xF9800000,
    }};

AddrMap addrMapJAWS{
    0x00401F3F,
    0x00010032,
    0x00,
    0x00,
    0x00,
    0x00,
    check_jaws,
    0x09,
    {
        0x40800000, 0x58000000, 0x50F00000, 0x50F04000, 0x50F04000, 0x50F16000,
        0,          0,          0x50F10000, 0x50F06000, 0x50F06000, 0x50F02000,
        0x50F14000, 0,          0,          0,          0,          0,
        0,          0,          0,          0,          0x50F80000,
    }};
// $031AC-
const std::vector<const AddrMap *> AddrMapTable{&addrMapGLUE, &addrMapMDU,
                                                &addrMapOSS,  &addrMapV8,
                                                &addrMapMCU,  &addrMapJAWS};

std::unordered_map<uint32_t, const AddrMap *> addrMapMap{
    {0x408032D8, &addrMapGLUE}, {0x4080337C, &addrMapMDU},
    {0x40803420, &addrMapOSS},  {0x408034C4, &addrMapV8},
    {0x40803568, &addrMapMCU},  {0x4080360C, &addrMapJAWS}};
}
