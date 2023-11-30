#ifndef MB_GLUE_HPP_
#define MB_GLUE_HPP_ 1
#include "chip/asc.hpp"
#include "chip/scc.hpp"
#include "chip/scsi.hpp"
#include "chip/swim.hpp"
#include "chip/via.hpp"
#include "io.hpp"
// SE/30, IIcx
struct GLUE : public IO_BUS {
    SCC scc;
    SCSI_HANDSHAKE scsi_handshake;
    SCSI_NORMAL scsi;
    SCSI_pDMA scsi_pdma;
    SWIM swim;
    uint32_t Read(uint32_t addr)  override;
    void Write(uint32_t addr, uint32_t value) override;
};
#endif