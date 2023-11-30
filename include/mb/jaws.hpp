#ifndef MB_JAWS_HPP_
#define MB_JAWS_HPP_ 1
#include "chip/asc.hpp"
#include "chip/scc.hpp"
#include "chip/scsi.hpp"
#include "chip/swim.hpp"
#include "chip/via.hpp"
#include "io.hpp"
/* STUB */
struct JAWS_CTL  {
  public:
    uint8_t read(uint32_t ) { return 0; }
    void write(uint32_t , uint8_t ) {}
};
// PowerBook 170
struct JAWS : public IO_BUS {
    SCC scc;
    SCSI_HANDSHAKE scsi_handshake;
    SCSI_NORMAL scsi;
    SCSI_pDMA scsi_pdma;
    SWIM swim;
    JAWS_CTL jaws;
    uint32_t Read(uint32_t addr)  override;
    void Write(uint32_t addr, uint32_t value) override;
};
#endif