#ifndef SCC_HPP_
#define SCC_HPP_ 1
#include "ebus/serial.hpp"
#include <atomic>
#include <deque>
#include <vector>
#include <memory>
#include <stdint.h>
#include <thread>
// Z85C30
struct SCC;
struct SccCh {
    friend class SCC;
    SccCh(SCC *base, bool v) : base(base), isChB(v) {}
    SCC *base;
    const bool isChB;

    // buffer
    std::deque<uint8_t> inBuf;
    uint8_t lastInBuf;
    std::vector<uint8_t> outBuf;

    // register
    uint8_t wR1;     // WR1
    uint8_t recvParam;   // WR3
    uint8_t sendParam1;  // WR4
    uint8_t sendParam2;  // WR5
    uint16_t syncLetter; // WR6/WR7
    uint8_t miscCtl;     // WR10
    uint8_t clkCtl;      // WR11
    uint16_t baudCnt;    // WR12/WR13/RR12/RR13
    uint8_t miscCtl2;   // WR14
    uint8_t exIrqCtl;    // WR15/RR15

    std::atomic<uint8_t> bufStatus;         // RR0
    std::atomic<uint8_t> specialRecvStatus; // RR1
    std::atomic<uint8_t> miscStatus;        // RR10

    std::atomic<bool> DTR;
    std::atomic<bool> CTS;

    bool recvFirstLetter = false;
    bool rr0_is_latched = false;
    

    uint8_t read(uint8_t addr);
    void write(uint8_t addr, uint8_t v);
    void reset();

    void recvData(uint8_t v);
    void eom();
    bool vSCCWrReq;

  private:
    uint8_t nextRecvData();
    void sendDataImpl(uint8_t v);
};

enum class SCC_IRQ {
    CH_B_SEND = 0,
    CH_B_EX,
    CH_B_RECV,
    CH_B_SPECIAL,
    CH_A_SEND,
    CH_A_EX,
    CH_A_RECV,
    CH_A_SPECIAL,
};
struct SCC {
    int curReg = 0;
    SCC() : chA(this, false), chB(this, true) {}
    void doIrq(SCC_IRQ which);
    SccCh chA, chB;
    std::atomic<uint8_t> intVec;     // WR/RR2
    std::atomic<uint8_t> irqPending; // RR3
    uint8_t masterIrqCtl;            // WR9
    uint8_t ius;

    uint8_t read(uint8_t addr);
    void write(uint8_t addr, uint8_t v);
};

void init_scc();

extern std::shared_ptr<SCC> scc;
#endif
