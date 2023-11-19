#include "io.hpp"
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

extern std::shared_ptr<VIA1> via1;
extern std::shared_ptr<VIA2> via2;
extern std::shared_ptr<ASC> asc;
extern std::shared_ptr<ASC> asc;
uint8_t *pds = nullptr;

uint8_t GLUE::Read8(uint32_t addr) {
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
        return scc.read(addr >> 1 & 3);
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

void GLUE::Write8(uint32_t addr, uint8_t value) {
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
        scc.write(addr >> 1 & 3, value);
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

uint8_t MDU::Read8(uint32_t addr) {
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
        return scc.read(addr >> 1 & 3);
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

void MDU::Write8(uint32_t addr, uint8_t value) {
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
        scc.write(addr >> 1 & 3, value);
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

uint8_t OSS::Read8(uint32_t addr) {
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
        return iop_scc.read(addr >> 1 & 0x1f);
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
void OSS::Write8(uint32_t addr, uint8_t value) {
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
        iop_scc.write(addr >> 1 & 0x1f, value);
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

uint32_t OSS::Read32(uint32_t addr) {
    if(!(addr & 0x8000000)) {
        switch((addr >> 13) & 0x1f) {
        case 4:
            return scsi_dma.read(addr & 0x1fff);
        }
    }
    return IO_BUS::Read32(addr);
}
void OSS::Write32(uint32_t addr, uint32_t v) {
    if(!(addr & 0x8000000)) {
        switch((addr >> 13) & 0x1f) {
        case 4:
            scsi_dma.write(addr >> 4 & 3, v);
            return;
        }
    }
    IO_BUS::Write32(addr, v);
}

uint8_t V8::Read8(uint32_t addr) {
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
        return scc.read(addr >> 1 & 3);
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

void V8::Write8(uint32_t addr, uint8_t value) {
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
        scc.write(addr >> 1 & 3, value);
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

uint8_t MCU_Q900::Read8(uint32_t addr) {
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
        return iop_scc.read(addr >> 1 & 0x1f);
    case 7:
        return mcu.read(addr & 0x1fff);
    case 10:
        return asc->read(addr & 0xfff);
    case 15:
        return iop_swim.read(addr >> 1 & 0x1f);
    }
    throw BusError{};
}
void MCU_Q900::Write8(uint32_t addr, uint8_t value) {
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
        iop_scc.write(addr >> 1 & 0x1f, value);
        return;
    case 7:
        mcu.write(addr & 0x1fff, value);
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

uint32_t MCU_Q900::Read32(uint32_t addr) {
    switch((addr >> 13) & 0x1f) {
    case 7:
        return mcu.read(addr & 0x1fff);
    }
    return IO_BUS::Read32(addr);
}

void MCU_Q900::Write32(uint32_t addr, uint32_t value) {
    switch((addr >> 13) & 0x1f) {
    case 7:
        mcu.write(addr & 0x1fff, value);
        return;
    }
    IO_BUS::Write32(addr, value);
}

uint8_t MCU_Q700::Read8(uint32_t addr) {
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
        return scc.read(addr >> 1 & 0x1f);
    case 7:
        return mcu.read(addr & 0x1fff);
    case 10:
        return asc->read(addr & 0xfff);
    case 15:
        return swim.read(addr >> 1 & 0x1f);
    }
    throw BusError{};
}
void MCU_Q700::Write8(uint32_t addr, uint8_t value) {
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
        scc.write(addr >> 1 & 0x1f, value);
        return;
    case 7:
        mcu.write(addr & 0x1fff, value);
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

uint32_t MCU_Q700::Read32(uint32_t addr) {
    switch((addr >> 13) & 0x1f) {
    case 7:
        return mcu.read(addr & 0x1fff);
    }
    return IO_BUS::Read32(addr);
}

void MCU_Q700::Write32(uint32_t addr, uint32_t value) {
    switch((addr >> 13) & 0x1f) {
    case 7:
        mcu.write(addr & 0x1fff, value);
        return;
    }
    IO_BUS::Write32(addr, value);
}

uint8_t JAWS::Read8(uint32_t addr) {
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
        return scc.read(addr >> 1 & 3);
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

void JAWS::Write8(uint32_t addr, uint8_t value) {
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
        scc.write(addr >> 1 & 3, value);
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
void initBus() {
    // chips initilize
    asc = std::make_shared<ASC>();
    via1 = std::make_shared<VIA1>();
    via2 = std::make_shared<VIA2>();
    io = std::make_unique<MCU_Q900>();
}
// for test
bool is_reset = false;
void bus_reset() {
    rtc_reset();
    adb_reset();
    asc_reset();
    is_reset = true;
}

std::unique_ptr<IO_BUS> io;