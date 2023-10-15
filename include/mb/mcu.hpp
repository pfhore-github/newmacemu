#ifndef MB_MCU_HPP_
#define MB_MCU_HPP_ 1
#include "chip/asc.hpp"
#include "chip/iop.hpp"
#include "chip/scc.hpp"
#include "chip/scsi.hpp"
#include "chip/sonic.hpp"
#include "chip/swim.hpp"
#include "chip/via.hpp"
#include "chip/vdac.hpp"
#include "io.hpp"
extern std::shared_ptr<ASC> asc;
struct MCU_ctl {
    uint32_t value;
    uint32_t ctls[64];
    uint32_t read(uint32_t addr);
    void write(uint32_t addr, uint32_t v);
};
// Quadra 9x0
struct MCU_Q900 : public IO_BUS {
    IOP_SWIM iop_swim;
    IOP_SCC iop_scc;
    Sonic sonic;
    VDAC vdac;
    MCU_ctl mcu;
    uint8_t Read8(uint32_t addr) override;
    void Write8(uint32_t addr, uint8_t value) override;
    uint32_t Read32(uint32_t addr) override;
    void Write32(uint32_t addr, uint32_t value) override;
};

struct MCU_Q700 : public IO_BUS {
    SWIM swim;
    SCC scc;
    Sonic sonic;
    VDAC vdac;
    MCU_ctl mcu;
    uint8_t Read8(uint32_t addr) override;
    void Write8(uint32_t addr, uint8_t value) override;
    uint32_t Read32(uint32_t addr) override;
    void Write32(uint32_t addr, uint32_t value) override;
};

#endif