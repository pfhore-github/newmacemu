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
    SCSI_HANDSHAKE scsi_handshake;
    SCSI_NORMAL scsi;
    SCSI_pDMA scsi_pdma;
    SWIM swim;
    uint8_t readB(uint32_t addr)  override;
    void writeB(uint32_t addr, uint8_t value) override;
};
#endif