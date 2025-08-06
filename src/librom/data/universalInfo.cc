#include "../rom_common.hpp"
#include "68040.hpp"
#include <stdint.h>
#include <unordered_map>
namespace LibROM {
extern const AddrMap addrMapGLUE;
extern const AddrMap addrMapMDU;
extern const AddrMap addrMapOSS;
extern const AddrMap addrMapV8;
extern const AddrMap addrMapMCU;
extern const AddrMap addrMapJAWS;
void init_quadra9x0() {}
void init_quadra700() {}
void init_pb170() {}
// GLU/MDU/OSS
static const RamType ram1{0x00100000,
                          {{0, 0x04000000}, {0x04000000, 0x08000000}}};
// LC
static const RamType ramLC{0x00100000,
                           {{0, 0x00800000}, {0x00800000, 0x00A00000}}};
static const RamType ramQ9x0{0x00400000,
                             {{0, 0x04000000},
                              {0x04000000, 0x08000000},
                              {0x08000000, 0x0C000000},
                              {0x0C000000, 0x10000000}}};
static const RamType ramQ700{0x00400000,
                             {
                                 {0, 0x04000000},
                                 {0x04000000, 0x08000000},
                             }};
static const RamType ramJaws{0x00080000,
                             {{0, 0x00200000},
                              {0x00200000, 0x00400000},
                              {0x00400000, 0x00600000},
                              {0x00600000, 0x00800000}}};

/* special address(VRAM?) map */
static const ExAddress exAddressIIci{0, 0xFBB08000, 0xFBB08000, 0x0B560000};
static const ExAddress exAddressIIsi{0, 0xFEE08000, 0xFEE08000, 0x0E6E0000};
static const ExAddress exAddressV8{0x50F40000, 0x50F40000, 0x50F40000,
                                   0x0B560000};
[[gnu::unused]] static const ExAddress exAddressUnused{0x009F5000, 0x009F5000,
                                                       0x50F40000, 0x0B560000};
static const ExAddress exAddressJAWS{0xFEE08000, 0xFEE08000, 0xFEE08000,
                                     0x0B560000};
static const ExAddress exAddressQuadra{0xF9001000, 0xF9001000, 0, 0x09460000};

/* pattern? map */
static const UniversalInfo12 univ12IIfx = {0, 0, 0x00070707, 0x07070700};
static const UniversalInfo12 univ12SE30 = {0, 0, 0x00131313, 0x01010300};
static const UniversalInfo12 univ12IIcx = {0, 0, 0x00070707, 0x01010100};
static const UniversalInfo12 univ12IIci = {0x03000000, 0, 0x00010108,
                                           0x07070700};
[[gnu::unused]] static const UniversalInfo12 univ12Unused = {
    0x03000000, 0, 0x00010108, 0x13131300};
static const UniversalInfo12 univ12IIsi = {0x03000000, 0, 0x00131313,
                                           0x01010800};
static const UniversalInfo12 univ12LC = {0x03080808, 0, 0x08090908, 0x09091300};
static const UniversalInfo12 univ12Q9x0 = {0x03000000, 0, 0x000A0707,
                                           0x07070700};
static const UniversalInfo12 univ12Q700 = {0x03000000, 0, 0x000A0808,
                                           0x08070700};
static const UniversalInfo12 univ12PB170 = {0x03000000, 0, 0x00080808,
                                            0x08080800};

/* VIA init map*/
static const ViaData glueVia1{0x01, 0x3f, 0x37, 0xb7, 0x00, 0x00};
static const ViaData mduVia1{0x21, 0x2f, 0xf7, 0xf7, 0x00, 0x00};
static const ViaData v8Via{0x26, 0x2f, 0x48, 0x30, 0x00, 0x1c};
static const ViaData ossIopVia{0x01, 0x07, 0x87, 0x87, 0x00, 0x00};
static const ViaData mcuIopVia1{0x00, 0x00, 0xC9, 0xF1, 0x22, 0x1C};
static const ViaData mcuVia1{0x20, 0x28, 0xF7, 0xF7, 0x22, 0x00};
static const ViaData jawsVia1{0x21, 0x28, 0x87, 0x87, 0x22, 0x00};

static const ViaData glueVia2{0x00, 0xC0, 0x07, 0x80, 0x60, 0xC0};
static const ViaData mduVia2{0x00, 0xC0, 0x0F, 0x89, 0x00, 0xC0};
static const ViaData mcuIopVia2{0x80, 0x80, 0x03, 0xC8, 0x22, 0xC0};
static const ViaData mcuVia2{0x80, 0x80, 0x06, 0x99, 0x22, 0xC0};
static const ViaData jawsVia2{0x00, 0x00, 0xE6, 0xBD, 0x22, 0x00};

static const UniversalInfo universalSE30{&addrMapGLUE,
                                         &ram1,
                                         nullptr,
                                         &univ12SE30,
                                         0xDC00,
                                         0x03,
                                         0x04,
                                         0x3FFF,
                                         0x01,
                                         0x00001F3F,
                                         0,
                                         0x40000008,
                                         0x40000000,
                                         {&glueVia1, &glueVia2},
                                         nullptr};

static const UniversalInfo universalIIcx{&addrMapGLUE,
                                         &ram1,
                                         nullptr,
                                         &univ12IIcx,
                                         0xDC00,
                                         0x02,
                                         0x04,
                                         0x3FFF,
                                         0x01,
                                         0x00001F3F,
                                         0,
                                         0x40000008,
                                         0x40000008,
                                         {&glueVia1, &glueVia2},
                                         nullptr};

static const UniversalInfo universalIIci{&addrMapMDU,
                                         &ram1,
                                         &exAddressIIci,
                                         &univ12IIci,
                                         0xDC00,
                                         0x05,
                                         0x05,
                                         0x3FFF,
                                         0x01,
                                         0x0000773F,
                                         0,
                                         0x56000000,
                                         0x46000000,
                                         {&mduVia1, nullptr},
                                         nullptr};

static const UniversalInfo universalIIciPGC{&addrMapMDU,
                                            &ram1,
                                            &exAddressIIci,
                                            &univ12IIci,
                                            0xDC00,
                                            0x05,
                                            0x05,
                                            0x3FFF,
                                            0x01,
                                            0x0000773F,
                                            0x1,
                                            0x56000000,
                                            0x56000000,
                                            {&mduVia1, nullptr},
                                            nullptr};

static const UniversalInfo universalIIfx{&addrMapOSS, &ram1,
                                         nullptr,     &univ12IIfx,
                                         0xDC00,      0x07,
                                         0x06,        0x3FFF,
                                         0x02,        0,
                                         0x4,         0x56000000,
                                         0x52000000,  {&ossIopVia, nullptr},
                                         nullptr};

static const UniversalInfo universalIIsi{&addrMapMDU,    &ram1,
                                         &exAddressIIsi, &univ12IIsi,
                                         0xDC00,         0x0C,
                                         0x05,           0x3FFF,
                                         0x04,           0x0000773F,
                                         0x00000126,     0x56000000,
                                         0x16000000,     {&mduVia1, nullptr},
                                         nullptr};

static const UniversalInfo universalLC{&addrMapV8,   &ramLC,
                                       &exAddressV8, &univ12LC,
                                       0xDC00,       0x0D,
                                       0x07,         0x3FFF,
                                       0x04,         0x0000773F,
                                       0x000001A6,   0x56000000,
                                       0x54000000,   {&v8Via, nullptr},
                                       nullptr};

static const UniversalInfo universalQuadra900{&addrMapMCU,
                                              &ramQ9x0,
                                              &exAddressQuadra,
                                              &univ12Q9x0,
                                              0xDC00,
                                              0x0E,
                                              0x08,
                                              0x3FFF,
                                              0x02,
                                              0x07A31807,
                                              0x00040924,
                                              0x56000000,
                                              0x50000000,
                                              {&mcuIopVia1, &mcuIopVia2},
                                              init_quadra9x0};

static const UniversalInfo universalQuadra950{&addrMapMCU,
                                              &ramQ9x0,
                                              &exAddressQuadra,
                                              &univ12Q9x0,
                                              0xDC00,
                                              0x14,
                                              0x08,
                                              0x3FFF,
                                              0x02,
                                              0x07A31807,
                                              0x00040924,
                                              0x56000000,
                                              0x10000000,
                                              {&mcuIopVia1, &mcuIopVia2},
                                              init_quadra9x0};

static const UniversalInfo universalQuadra700{&addrMapMCU,
                                              &ramQ700,
                                              &exAddressQuadra,
                                              &univ12Q700,
                                              0xDC00,
                                              0x10,
                                              0x08,
                                              0x3FFF,
                                              0x02,
                                              0x05A0183F,
                                              0x00000900,
                                              0x56000000,
                                              0x40000000,
                                              {&mcuVia1, &mcuVia2},
                                              init_quadra700};

static const UniversalInfo universalPB170{
    &addrMapJAWS,   &ramJaws,
    &exAddressJAWS, &univ12PB170,
    0xDD00,         0x0F,
    0x09,           0x3FFF,
    0x04,           0x00401F3F,
    0x00010932,     0x56000000,
    0x12000000,     {&jawsVia1, &jawsVia2},
    init_pb170};

static const UniversalInfo universalGenericGLUE{&addrMapGLUE,
                                                &ram1,
                                                nullptr,
                                                &univ12IIfx,
                                                0xDC00,
                                                0xFD,
                                                0x04,
                                                0x3FFF,
                                                0x01,
                                                0,
                                                0,
                                                0,
                                                0,
                                                {&glueVia1, &glueVia2},
                                                nullptr};

static const UniversalInfo universalGenericMDU{&addrMapMDU,
                                               &ram1,
                                               &exAddressIIci,
                                               &univ12IIfx,
                                               0xDC00,
                                               0xFD,
                                               0x05,
                                               0x3FFF,
                                               0x01,
                                               0,
                                               0,
                                               0,
                                               0,
                                               {&mduVia1, &mduVia2},
                                               nullptr};

static const UniversalInfo universalGenericOSS{
    &addrMapOSS, &ram1,
    nullptr,     &univ12IIfx,
    0xDC00,      0xFD,
    0x06,        0x3FFF,
    0x02,        0,
    0,           0,
    0,           {&ossIopVia, nullptr},
    nullptr};

static const UniversalInfo universalGenericV8{&addrMapV8,
                                              &ramLC,
                                              &exAddressV8,
                                              &univ12LC,
                                              0xCC00,
                                              0xFD,
                                              0x07,
                                              0x7FFF,
                                              0x04,
                                              0,
                                              0,
                                              0,
                                              0,
                                              {&v8Via, nullptr},
                                              nullptr};

static const UniversalInfo universalGenericJAWS{&addrMapJAWS,
                                                &ramJaws,
                                                &exAddressJAWS,
                                                &univ12PB170,
                                                0xDD00,
                                                0xFD,
                                                0x09,
                                                0x3FFF,
                                                0x02,
                                                0,
                                                0x10032,
                                                0,
                                                0,
                                                {&jawsVia1, &jawsVia2},
                                                nullptr};

// $031C8-$03208
const std::vector<const UniversalInfo *> UniversalInfoTable{
    &universalSE30,        &universalIIcx,       &universalIIci,
    &universalIIciPGC,     &universalIIfx,       &universalIIsi,
    &universalLC,          &universalQuadra700,  &universalQuadra900,
    &universalQuadra950,   &universalPB170,      &universalGenericGLUE,
    &universalGenericMDU,  &universalGenericOSS, &universalGenericV8,
    &universalGenericJAWS,
};
std::unordered_map<uint32_t, const UniversalInfo *> UniversalInfoMaps{
    {0x408036BC, &universalSE30},       {0x408036FC, &universalIIcx},
    {0x4080373C, &universalIIci},       {0x4080377C, &universalIIciPGC},
    {0x408037BC, &universalIIfx},       {0x408037FC, &universalIIsi},
    {0x4080383C, &universalLC},         {0x408038FC, &universalQuadra700},
    {0x4080387C, &universalQuadra900},  {0x408038BC, &universalQuadra950},
    {0x4080393C, &universalPB170},      {0x4080397C, &universalGenericGLUE},
    {0x408039BC, &universalGenericMDU}, {0x408039FC, &universalGenericOSS},
    {0x40803A3C, &universalGenericV8},  {0x40803A7C, &universalGenericJAWS},
};
}
