#include "chip/scc.hpp"
#include <utility>
// 85C30
std::shared_ptr<SCC> scc;
void sendModemPort(SccCh *ch, const uint8_t *v, size_t);
void sendPrinterPort(SccCh *ch, const uint8_t *v, size_t);
void do_irq(int i);
uint8_t SCC::read(uint8_t addr) {
    switch(addr) {
    case 0:
        return chB.read(curReg);
    case 1:
        return chA.read(curReg);
    case 2:
        return chB.nextRecvData();
    case 3:
        return chA.nextRecvData();
    default:
        __builtin_unreachable();
    }
}

void SCC::write(uint8_t addr, uint8_t v) {
    switch(addr) {
    case 0:
        chB.write(curReg, v);
        return;
    case 1:
        chA.write(curReg, v);
        return;
    case 2:
        chB.sendDataImpl(v);
        return;
    case 3:
        chA.sendDataImpl(v);
        return;
    default:
        __builtin_unreachable();
    }
}
constexpr uint8_t WR1_WREQ_ENABLE = 1 << 7;
constexpr uint8_t WR1_WREQ_FUNC = 1 << 6;
constexpr uint8_t WR1_WREQ_RECV = 1 << 5;

constexpr uint8_t STS_RxCAvilable = 1 << 0;
constexpr uint8_t STS_ZeroCount = 1 << 1;
constexpr uint8_t STS_BufferEmpty = 1 << 2;
constexpr uint8_t STS_DCD = 1 << 3;
constexpr uint8_t STS_SyncHunt = 1 << 4;
constexpr uint8_t STS_CTS = 1 << 5;
constexpr uint8_t STS_TxUnderrunEOM = 1 << 6;
constexpr uint8_t STS_BrkAbt = 1 << 7;

constexpr uint8_t SRC_AllSent = 1 << 0;
constexpr uint8_t SRC_RC2 = 1 << 1;
constexpr uint8_t SRC_RC1 = 1 << 2;
constexpr uint8_t SRC_RC0 = 1 << 3;
constexpr uint8_t SRC_ParityErr = 1 << 4;
constexpr uint8_t SRC_RxOverrunErr = 1 << 5;
constexpr uint8_t SRC_CRCErr = 1 << 6;
constexpr uint8_t SRC_EndOfFrame = 1 << 7;

constexpr uint8_t IP_ext = 1;
constexpr uint8_t IP_Tx = 1 << 1;
constexpr uint8_t IP_Rx = 1 << 2;

constexpr uint8_t MS_ON_LOOP = 1 << 1;

constexpr uint8_t MS_LOOP_SNEDING = 1 << 4;
constexpr uint8_t MS_TWO_CLK_MISSING = 1 << 6;
constexpr uint8_t MS_ONE_CLK_MISSING = 1 << 7;

constexpr uint8_t IE_ZERO_COUNT = 1 << 1;
constexpr uint8_t IE_DCD = 1 << 3;
constexpr uint8_t IE_SYNC_HUNT = 1 << 4;
constexpr uint8_t IE_CTS = 1 << 5;
constexpr uint8_t IE_TX_UNDERRUN_EOM = 1 << 6;
constexpr uint8_t IE_BREAK_ABORT = 1 << 7;

constexpr uint8_t RECV_ENABLE = 1;

constexpr uint8_t MIC_MIE = 1 << 3;
constexpr uint8_t MIC_VECH = 1 << 4;

constexpr uint8_t IRQ_A_RECV = 1 << 5;
constexpr uint8_t IRQ_A_SEND = 1 << 4;
constexpr uint8_t IRQ_A_SPECIAL = 1 << 3;
constexpr uint8_t IRQ_B_RECV = 1 << 2;
constexpr uint8_t IRQ_B_SEND = 1 << 1;
constexpr uint8_t IRQ_B_SPECIAL = 1 << 0;

uint8_t SccCh::nextRecvData() {
    if(!inBuf.empty()) {
        lastInBuf = inBuf.front();
        inBuf.pop_front();
    } else {
        if(wR1 & WR1_WREQ_ENABLE && !(wR1 & WR1_WREQ_FUNC)) {
            if(wR1 & WR1_WREQ_RECV) {
                vSCCWrReq = false;
            }
        }
    }
    return lastInBuf;
}

void SCC::doIrq(SCC_IRQ which) {
    ius = 0x3f;
    int vecH = masterIrqCtl & MIC_VECH ? 4 : 1;
    if(!(masterIrqCtl & MIC_MIE)) {
        return;
    }
    intVec &= ~(0x7 << vecH);
    intVec |= int(which) << vecH;
    switch(which) {
    case SCC_IRQ::CH_A_RECV:
        irqPending |= IRQ_A_RECV;
        ius &= ~IRQ_A_RECV;
        do_irq(4);
        return;
    case SCC_IRQ::CH_A_SEND:
        if(!(ius & ~IRQ_A_SEND)) {
            irqPending |= IRQ_A_RECV;
            ius &= ~IRQ_A_SEND;
            do_irq(4);
            return;
        }
        break;
    case SCC_IRQ::CH_A_EX:
    case SCC_IRQ::CH_A_SPECIAL:
        if(!(ius & ~IRQ_A_SPECIAL)) {
            irqPending |= IRQ_A_SPECIAL;
            ius &= ~IRQ_A_SEND;
            do_irq(4);
            return;
        }
        break;
    case SCC_IRQ::CH_B_RECV:
        if(!(ius & ~IRQ_B_RECV)) {
            irqPending |= IRQ_B_RECV;
            ius &= ~IRQ_B_RECV;
            do_irq(4);
            return;
        }
        break;
    case SCC_IRQ::CH_B_SEND:
        if(!(ius & ~IRQ_B_SEND)) {
            irqPending |= IRQ_B_RECV;
            ius &= ~IRQ_B_SEND;
            do_irq(4);
            return;
        }
        break;
    case SCC_IRQ::CH_B_EX:
    case SCC_IRQ::CH_B_SPECIAL:
        if(!(ius & ~IRQ_B_SPECIAL)) {
            irqPending |= IRQ_B_SPECIAL;
            ius &= ~IRQ_B_SEND;
            do_irq(4);
            return;
        }
        break;
    }
}

void SccCh::reset() {
    wR1 = wR1 & 0x24;
    recvParam = recvParam & ~1;
    sendParam1 = sendParam1 | 3;
    sendParam2 = sendParam2 & 0x61;
    base->masterIrqCtl = 0xC0 | (base->masterIrqCtl & 3);
    miscCtl = 0;
    clkCtl = 0x8;
    miscCtl2 = (miscCtl2 & 0xC0) | 0x30;
    exIrqCtl = 0xf8;

    bufStatus.store((bufStatus.load() & 0xB8) | 0x44);
    specialRecvStatus.store((specialRecvStatus.load() & 1) | 6);
    miscStatus.store(miscStatus.load() & 0x40);
}

void SccCh::eom() {
    base->doIrq(isChB ? SCC_IRQ::CH_B_SPECIAL : SCC_IRQ::CH_A_SPECIAL);
}
void SccCh::recvData(uint8_t v) {
    if(wR1 & WR1_WREQ_ENABLE) {
        if(inBuf.empty()) {
            if(wR1 & WR1_WREQ_FUNC) {
                vSCCWrReq = true;
            } else {
                vSCCWrReq = false;
            }
        }
    }
    if(inBuf.size() < 8) {
        if((recvParam & 1 << 1) && (syncLetter & 0xff) == v) {
            return;
        }
        inBuf.push_back(v);
        bufStatus |= STS_RxCAvilable;
        if((wR1 >> 3 & 3) == 2 ||
           ((wR1 >> 3 & 3) == 1 && !std::exchange(recvFirstLetter, true))) {
            if(!recvFirstLetter) {
                base->doIrq(isChB ? SCC_IRQ::CH_B_RECV : SCC_IRQ::CH_A_RECV);
            }
        }
    } else {
        // overrun
        specialRecvStatus |= SRC_RxOverrunErr;
        base->doIrq(isChB ? SCC_IRQ::CH_B_SPECIAL : SCC_IRQ::CH_A_SPECIAL);
    }
}
uint8_t SccCh::read(uint8_t addr) {
    base->curReg = 0;
    switch(addr) {
    case 0:
    case 4:
        return bufStatus.load();
    case 1:
    case 5:
        return specialRecvStatus.load();
    case 2:
    case 6:
        return base->intVec.load();
    case 3:
    case 7:
        return isChB ? 0 : base->irqPending.load();
    case 8:
        return nextRecvData();
    case 9:
    case 13:
        return baudCnt >> 8;
    case 10:
    case 14:
        return miscCtl;
    case 11:
    case 15:
        return exIrqCtl;
    case 12:
        return baudCnt & 0xff;
    default:
        __builtin_unreachable();
    }
}

void SccCh::sendDataImpl(uint8_t v) {
    outBuf.push_back(v);
    if(outBuf.size() == 4) {
        if(isChB) {
            sendPrinterPort(this, outBuf.data(), 4);
            base->doIrq(SCC_IRQ::CH_B_SEND);
        } else {
            sendModemPort(this, outBuf.data(), 4);
            base->doIrq(SCC_IRQ::CH_A_SEND);
        }
        outBuf.clear();
    }
    vSCCWrReq = true;
}

void SccCh::write(uint8_t addr, uint8_t v) {
    base->curReg = 0;
    switch(addr) {
    case 0:
        switch(v >> 6) {
        case 0:
            break;
        case 1:
            // TODO: Reset RxCRC
            break;
        case 2:
            // TODO: Reset TxCRC
            break;
        case 3:
            // TODO: Reset Transmit Underrun/EOM Latch
            bufStatus &= ~IE_TX_UNDERRUN_EOM;
            break;
        }
        base->curReg = v & 3;
        switch(v >> 3 & 7) {
        case 0: // NullCode
            break;
        case 1: // PointHigh
            base->curReg |= 8;
            break;
        case 2:
            // Reset External/Status Interrupts Command
            rr0_is_latched = false;
            break;
        case 3:
            // TODO:SEND Abort
            outBuf.clear();
            bufStatus |= IE_TX_UNDERRUN_EOM;
            break;
        case 4:
            // Enable Interrupt On Next Rx Character
            recvFirstLetter = false;
            if(!inBuf.empty()) {
                base->doIrq(isChB ? SCC_IRQ::CH_B_RECV : SCC_IRQ::CH_A_RECV);
            }
            break;
        case 5:
            // Reset Tx Interrupt Pending
            if(isChB) {
                sendPrinterPort(this, outBuf.data(), outBuf.size());
            } else {
                sendModemPort(this, outBuf.data(), outBuf.size());
            }
            outBuf.clear();
            break;
        case 6:
            // TODO: Error Reset
            specialRecvStatus.store(0);
            break;
        case 7:
            // Reset Highest IUS
            {
                uint i = base->ius;

                base->ius |= base->ius >> 1;
                base->ius |= base->ius >> 2;
                base->ius |= base->ius >> 4;

                base->ius = (base->ius >> 1) & i;
            }
            break;
        default:
            __builtin_unreachable();
        }
        break;
    case 1:
        if(!(v & WR1_WREQ_ENABLE)) {
            vSCCWrReq = true;
        }
        wR1 = v;
        break;
    case 2:
        base->intVec.store(v);
        break;
    case 3:
        break;
    case 8:
        sendDataImpl(v);
        break;
    default:
        __builtin_unreachable();
    }
}

void scc_reset() {
    scc->chA.reset();
    scc->chB.reset();
}