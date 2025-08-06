#include "io.hpp"
#include "bus.hpp"
#include "chip/iifx.hpp"
#include "chip/iop.hpp"
#include "chip/rbv.hpp"
#include "chip/scc.hpp"
#include "chip/scsi.hpp"
#include "chip/swim.hpp"
#include "chip/vdac.hpp"
#include "chip/via.hpp"
#include "mb/glue.hpp"
#include "mb/jaws.hpp"
#include "mb/mcu.hpp"
#include "mb/mdu.hpp"
#include "mb/oss.hpp"
#include "mb/v8.hpp"
#include "memory.hpp"
#include <optional>

extern std::shared_ptr<ASC> asc;
uint8_t *pds = nullptr;

uint8_t GLUE::readB(uint32_t addr) {
    if(addr & 0x8000000) {
        if(pds) {
            return pds[addr & 0x7ffffff];
        } else {
            throw BusError{};
        }
    }
    switch((addr >> 13) & 0xf) {
    case 0:
        return via1->read(addr >> 9 & 0xf);
    case 1:
        return via2->read(addr >> 9 & 0xf);
    case 2:
        return scc->read(addr >> 1 & 3);
    case 3:
        return scsi_handshake.read(addr >> 4 & 3);
    case 8:
        return scsi.read(addr >> 4 & 3);
    case 9:
        return scsi_pdma.read(addr >> 4 & 3);
    case 10:
        return asc->read(addr & 0xfff);
    case 11:
        return swim.read(addr >> 9 & 0xf);
    }
    throw BusError{};
}

void GLUE::writeB(uint32_t addr, uint8_t value) {
    if(addr & 0x8000000) {
        if(pds) {
            pds[addr & 0x7ffffff] = value;
        } else {
            throw BusError{};
        }
    }
    switch((addr >> 13) & 0xf) {
    case 0:
        via1->write(addr >> 9 & 0xf, value);
        return;
    case 1:
        via2->write(addr >> 9 & 0xf, value);
        return;
    case 2:
        scc->write(addr >> 1 & 3, value);
        return;
    case 3:
        scsi_handshake.write(addr >> 4 & 3, value);
        return;
    case 8:
        scsi.write(addr >> 4 & 3, value);
        return;
    case 9:
        scsi_pdma.write(addr >> 4 & 3, value);
        return;
    case 10:
        asc->write(addr & 0xfff, value);
        return;
    case 11:
        swim.write(addr >> 9 & 0xf, value);
        return;
    }
    throw BusError{};
}

uint8_t MDU::readB(uint32_t addr) {
    if(addr & 0x8000000) {
        if(pds) {
            return pds[addr & 0x7ffffff];
        } else {
            throw BusError{};
        }
    }
    switch((addr >> 13) & 0x1f) {
    case 0:
        return via1->read(addr >> 9 & 0xf);
    case 2:
        return scc->read(addr >> 1 & 3);
    case 3:
        return scsi_handshake.read(addr >> 4 & 3);
    case 8:
        return scsi.read(addr >> 4 & 3);
    case 9:
        return scsi_pdma.read(addr >> 4 & 3);
    case 10:
        return asc->read(addr & 0xfff);
    case 11:
        return swim.read(addr >> 9 & 0xf);
    case 18:
        return vdac.read(addr >> 2 & 7);
    case 19:
        return rbv.read(addr & 0xff);
    }
    throw BusError{};
}

void MDU::writeB(uint32_t addr, uint8_t value) {
    if(addr & 0x8000000) {
        if(pds) {
            pds[addr & 0x7ffffff] = value;
        } else {
            throw BusError{};
        }
    }
    switch((addr >> 13) & 0x1f) {
    case 0:
        via1->write(addr >> 9 & 0xf, value);
        return;
    case 2:
        scc->write(addr >> 1 & 3, value);
        return;
    case 3:
        scsi_handshake.write(addr >> 4 & 3, value);
        return;
    case 8:
        scsi.write(addr >> 4 & 3, value);
        return;
    case 9:
        scsi_pdma.write(addr >> 4 & 3, value);
        return;
    case 10:
        asc->write(addr & 0xfff, value);
        return;
    case 11:
        swim.write(addr >> 9 & 0xf, value);
        return;
    case 18:
        vdac.write(addr >> 2 & 7, value);
        return;
    case 19:
        rbv.write(addr & 0xff, value);
        return;
    }
    throw BusError{};
}
uint32_t OSS::readL(uint32_t addr) {
    switch((addr >> 13) & 0x1f) {
    case 4:
        return scsi_dma.read(addr & 0x1fff);
    }
    return IO_BUS::readL(addr);
}
uint8_t OSS::readB(uint32_t addr) {
    if(addr & 0x8000000) {
        if(pds) {
            return pds[addr & 0x7ffffff];
        } else {
            throw BusError{};
        }
    }
    switch((addr >> 13) & 0x1f) {
    case 0:
        return via1->read(addr >> 9 & 0xf);
    case 2:
        return iop_scc->read(addr >> 1 & 0x1f);
    case 8:
        return asc->read(addr & 0xfff);
    case 9:
        return iop_swim.read(addr >> 1 & 0x1f);
    case 13:
        return oss.read((addr & 0xf) | (addr & 0x1E00) >> 5);
    case 14:
        return exp0.read(addr & 0x1fff);
    case 15:
        return exp1.read(addr & 0x1fff);
    }
    throw BusError{};
}
void OSS::writeL(uint32_t addr, uint32_t value) {
    switch((addr >> 13) & 0x1f) {
    case 4:
        scsi_dma.write(addr >> 4 & 3, value);
        return;
    }
    IO_BUS::writeL(addr, value);
}
void OSS::writeB(uint32_t addr, uint8_t value) {
    if(addr & 0x8000000) {
        if(pds) {
            pds[addr & 0x7ffffff] = value;
        } else {
            throw BusError{};
        }
    }
    switch((addr >> 13) & 0x1f) {
    case 0:
        via1->write(addr >> 9 & 0xf, value);
        return;
    case 2:
        iop_scc->write(addr >> 1 & 0x1f, value);
        return;
    case 8:
        asc->write(addr & 0xfff, value);
        return;
    case 9:
        iop_swim.write(addr >> 1 & 0x1f, value);
        return;
    case 13:
        oss.write((addr & 0xf) | (addr & 0x1E00) >> 5, value);
        return;
    case 14:
        exp0.write(addr & 0x1fff, value);
        return;
    case 15:
        exp1.write(addr & 0x1fff, value);
        return;
    }
    throw BusError{};
}

uint8_t V8::readB(uint32_t addr) {
    if(addr > 0xF80000) {
        if(pds) {
            return pds[addr & 0x7ffff];
        } else {
            throw BusError{};
        }
    }
    if(!(addr & 0xf00000)) {
        throw BusError{};
    }
    switch((addr >> 13) & 0x1f) {
    case 0:
        return via1->read(addr >> 9 & 0xf);
    case 2:
        return scc->read(addr >> 1 & 3);
    case 3:
        return scsi_handshake.read(addr >> 4 & 3);
    case 8:
        return scsi.read(addr >> 4 & 3);
    case 9:
        return scsi_pdma.read(addr >> 4 & 3);
    case 10:
        return asc->read(addr & 0xfff);
    case 11:
        return swim.read(addr >> 9 & 0xf);
    case 18:
        return vdac.read(addr >> 2 & 7);
    case 19:
        return rbv.read(addr & 0xff);
    }
    throw BusError{};
}

void V8::writeB(uint32_t addr, uint8_t value) {
    if(addr > 0xF80000) {
        if(pds) {
            pds[addr & 0x7ffff] = value;
        } else {
            throw BusError{};
        }
    }
    if(!(addr & 0xf00000)) {
        throw BusError{};
    }
    switch((addr >> 13) & 0x1f) {
    case 0:
        via1->write(addr >> 9 & 0xf, value);
        return;
    case 2:
        scc->write(addr >> 1 & 3, value);
        return;
    case 3:
        scsi_handshake.write(addr >> 4 & 3, value);
        return;
    case 8:
        scsi.write(addr >> 4 & 3, value);
        return;
    case 9:
        scsi_pdma.write(addr >> 4 & 3, value);
        return;
    case 10:
        asc->write(addr & 0xfff, value);
        return;
    case 11:
        swim.write(addr >> 9 & 0xf, value);
        return;
    case 18:
        vdac.write(addr >> 2 & 7, value);
        return;
    case 19:
        rbv.write(addr & 0xff, value);
        return;
    }
    throw BusError{};
}

uint32_t MCU_Q900::readL(uint32_t addr) {
    switch((addr >> 13) & 0x1f) {
    case 7:
        return mcu.read(addr & 0x1fff);
    }
    return IO_BUS::readL(addr);
}
uint8_t MCU_Q900::readB(uint32_t addr) {
    if(addr & 0x8000000) {
        if(pds) {
            return pds[addr & 0x7ffffff];
        } else {
            throw BusError{};
        }
    }
    switch((addr >> 13) & 0x1f) {
    case 0:
        return via1->read(addr >> 9 & 0xf);
    case 1:
        return via2->read(addr >> 9 & 0xf);
    case 6:
        return iop_scc->read(addr >> 1 & 0x1f);
    case 10:
        return asc->read(addr & 0xfff);
    case 15:
        return iop_swim.read(addr >> 1 & 0x1f);
    }
    throw BusError{};
}
void MCU_Q900::writeL(uint32_t addr, uint32_t value) {
    switch((addr >> 13) & 0x1f) {
    case 7:
        mcu.write(addr & 0x1fff, value);
        return;
    }
    IO_BUS::writeL(addr, value);
}
void MCU_Q900::writeB(uint32_t addr, uint8_t value) {
    if(addr & 0x8000000) {
        if(pds) {
            pds[addr & 0x7ffffff] = value;
        } else {
            throw BusError{};
        }
    }
    switch((addr >> 13) & 0x1f) {
    case 0:
        via1->write(addr >> 9 & 0xf, value);
        return;
    case 1:
        via2->write(addr >> 9 & 0xf, value);
        return;
    case 6:
        iop_scc->write(addr >> 1 & 0x1f, value);
        return;

    case 10:
        asc->write(addr & 0xfff, value);
        return;
    case 15:
        iop_swim.write(addr >> 1 & 0x1f, value);
        return;
    }
    throw BusError{};
}
uint32_t MCU_Q700::readL(uint32_t addr) {
    switch((addr >> 13) & 0x1f) {
    case 7:
        return mcu.read(addr & 0x1fff);
    }
    return IO_BUS::readL(addr);
}
uint8_t MCU_Q700::readB(uint32_t addr) {
    if(addr & 0x8000000) {
        if(pds) {
            return pds[addr & 0x7ffffff];
        } else {
            throw BusError{};
        }
    }
    switch((addr >> 13) & 0x1f) {
    case 0:
        return via1->read(addr >> 9 & 0xf);
    case 1:
        return via2->read(addr >> 9 & 0xf);
    case 6:
        return scc->read(addr >> 1 & 0x1f);
    case 7:
        return mcu.read(addr & 0x1fff);
    case 10:
        return asc->read(addr & 0xfff);
    case 15:
        return swim.read(addr >> 1 & 0x1f);
    }
    throw BusError{};
}
void MCU_Q700::writeL(uint32_t addr, uint32_t value) {
    switch((addr >> 13) & 0x1f) {
    case 7:
        mcu.write(addr & 0x1fff, value);
        return;
    }
    return IO_BUS::writeL(addr, value);
}
void MCU_Q700::writeB(uint32_t addr, uint8_t value) {
    if(addr & 0x8000000) {
        if(pds) {
            pds[addr & 0x7ffffff] = value;
        } else {
            throw BusError{};
        }
    }
    switch((addr >> 13) & 0x1f) {
    case 0:
        via1->write(addr >> 9 & 0xf, value);
        return;
    case 1:
        via2->write(addr >> 9 & 0xf, value);
        return;
    case 6:
        scc->write(addr >> 1 & 0x1f, value);
        return;
    case 10:
        asc->write(addr & 0xfff, value);
        return;
    case 15:
        swim.write(addr >> 1 & 0x1f, value);
        return;
    }
    throw BusError{};
}

uint8_t JAWS::readB(uint32_t addr) {
    if(addr & 0x8000000) {
        if(pds) {
            return pds[addr & 0x7ffffff];
        } else {
            throw BusError{};
        }
    }
    switch((addr >> 13) & 0x7f) {
    case 0:
        return via1->read(addr >> 9 & 0xf);
    case 1:
        return via2->read(addr >> 9 & 0xf);
    case 2:
        return scc->read(addr >> 1 & 3);
    case 3:
        return scsi_handshake.read(addr >> 4 & 3);
    case 8:
        return scsi.read(addr >> 4 & 3);
    case 9:
        return scsi_pdma.read(addr >> 4 & 3);
    case 10:
        return asc->read(addr & 0xfff);
    case 11:
        return swim.read(addr >> 9 & 0xf);
    default:
        if((addr & 0xfffff) >= 0x80000) {
            return jaws.read(addr);
        }
        break;
    }
    throw BusError{};
}

void JAWS::writeB(uint32_t addr, uint8_t value) {
    if(addr & 0x8000000) {
        if(pds) {
            pds[addr & 0x7ffffff] = value;
        } else {
            throw BusError{};
        }
    }
    switch((addr >> 13) & 0x7f) {
    case 0:
        via1->write(addr >> 9 & 0xf, value);
        return;
    case 1:
        via2->write(addr >> 9 & 0xf, value);
        return;
    case 2:
        scc->write(addr >> 1 & 3, value);
        return;
    case 3:
        scsi_handshake.write(addr >> 4 & 3, value);
        return;
    case 8:
        scsi.write(addr >> 4 & 3, value);
        return;
    case 9:
        scsi_pdma.write(addr >> 4 & 3, value);
        return;
    case 10:
        asc->write(addr & 0xfff, value);
        return;
    case 11:
        swim.write(addr >> 9 & 0xf, value);
        return;
    default:
        if((addr & 0xfffff) >= 0x80000) {
            return jaws.write(addr, value);
        }
        break;
    }
    throw BusError{};
}
// TODO
void rtc_reset();
void adb_reset();
void asc_reset();
void scc_reset();
void initBus() {
    // chips initilize
    asc = std::make_shared<ASC>();
    via1 = std::make_shared<VIA1>();
    via2 = std::make_shared<VIA2>();
    io = std::make_unique<MCU_Q900>();
    scc = std::make_shared<SCC>();
    iop_scc = std::make_shared<IOP_SCC>();
}
// for test
bool is_reset = false;

void bus_reset() {
    rtc_reset();
    adb_reset();
    asc_reset();
    scc_reset();
    is_reset = true;
}

std::unique_ptr<IO_BUS> io;