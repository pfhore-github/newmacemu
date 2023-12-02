#ifndef MB_OSS_HPP_
#define MB_OSS_HPP_ 1
#include "chip/iifx.hpp"
#include "chip/iop.hpp"
#include "chip/scc.hpp"
#include "chip/scsi.hpp"
// IIfx

struct OSS : public IO_BUS {
    SCSI_DMA scsi_dma;
    IOP_SWIM iop_swim;
    IOP_SCC iop_scc;
    OSS_ctrl oss;
    IIfxExp0 exp0;
    IIfxExp1 exp1;
    uint8_t readB(uint32_t addr)  override;
    void writeB(uint32_t addr, uint8_t value) override;
    uint32_t readL(uint32_t addr)  override;
    void writeL(uint32_t addr, uint32_t value) override;
};
#endif