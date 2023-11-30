#ifndef MB_V8_HPP_
#define MB_V8_HPP_ 1
#include "chip/rbv.hpp"
#include "chip/scc.hpp"
#include "chip/scsi.hpp"
#include "chip/swim.hpp"
#include "chip/vdac.hpp"

// LC
struct V8 : public IO_BUS {
    RBV rbv;
    SCC scc;
    SCSI_HANDSHAKE scsi_handshake;
    SCSI_NORMAL scsi;
    SCSI_pDMA scsi_pdma;
    SWIM swim;
    VDAC vdac;

    uint32_t Read(uint32_t addr)  override;
    void Write(uint32_t addr, uint32_t value) override;
};
#endif