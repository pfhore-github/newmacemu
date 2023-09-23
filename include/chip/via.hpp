#ifndef VIA_HPP_
#define VIA_HPP_
#include "SDL_timer.h"
#include "io.hpp"
#include <bitset>
#include <stdint.h>
enum class SR_C {
    DISABLED = 0,
    IN_T2,
    IN_P2,
    IN_EC,
    OUT_FREE,
    OUT_T2,
    OUT_P2,
    OUT_EC
};
enum class C2_CTL {
    INPUT_NEGATIVE_ACTIVE_EDGE = 0,
    INDEPENDENT_INTERRUPT_INPUT_NEG_EDGE,
    INPUT_POSITIVE_ACTIVE_EDGE,
    INDEPENDENT_INTERRUPT_INPOS_POS_EDGE,
    HANSHAKE_OUTPUT,
    PULSE_OUTPUT,
    LOW_OUTPUT,
    HIGH_OUTPUT,
};
namespace boot::_2EBC {
class Via1;
}
enum class VIA_IRQ {
    CA2,
    CA1,
    SR,
    CB2,
    CB1,
    TIMER2,
    TIMER1,
};
class VIA : public CHIP_B {
  public:
    uint32_t delay_count = 0;
    uint64_t timer1_base;
    uint64_t timer2_base;
    SDL_TimerID timer1;
    SDL_TimerID timer2;
    uint16_t timer1_cnt;
    uint16_t timer2_cnt;
    uint16_t timer1_latch;
    uint16_t timer2_latch;
    struct {
        bool PB7_ENABLE;
        bool T1_REP;
        bool T2_CTL;
        SR_C sr_c;
        bool PB_LATCH;
        bool PA_LATCH;
    } ACR;
    struct {
        C2_CTL CB2_CTL;
        bool CB1_CTL;
        C2_CTL CA2_CTL;
        bool CA1_CTL;
    } PCR;

    uint8_t sr;
    std::bitset<8> ora;
    std::bitset<8> ira;
    std::bitset<8> orb;
    std::bitset<8> irb;
    std::bitset<8> dira;
    std::bitset<8> dirb;
    std::bitset<7> IF;
    std::bitset<7> IE;

    uint8_t read(uint32_t n);
    void write(uint32_t n, uint8_t v);
    void irq(VIA_IRQ i);
    virtual bool readPA(int n) = 0;
    virtual bool readPB(int n) = 0;
    virtual void writePA(int n, bool v) = 0;
    virtual void writePB(int n, bool v) = 0;
    virtual int irqNum() = 0;
    virtual uint8_t getSR() { return 0; }

    void recieve_ca1();
    void recieve_cb1();
    void recieve_sr();
};
struct VIA1 : public VIA {
    bool adb_state[2] = {true, true};
    bool rtc_clock = true;
    bool rtc_val = false;
    bool readPA(int n) override;
    bool readPB(int n) override;
    void writePA(int n, bool v) override;
    void writePB(int n, bool v) override;
    uint8_t getSR() override;
    // TODO: may vary with machine?
    int irqNum() override { return 1; }
};
struct VIA2 : public VIA {
    bool readPA(int n) override;
    bool readPB(int n) override;
    void writePA(int n, bool v) override;
    void writePB(int n, bool v) override;
    // TODO: may vary with machine?
    int irqNum() override { return 2; }
};
extern std::shared_ptr<VIA1> via1;
extern std::shared_ptr<VIA2> via2;
#endif