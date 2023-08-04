#ifndef VIA_HPP_
#define VIA_HPP_
#include "SDL_timer.h"
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
struct VIA_IRQ {
    bool CA2, CA1, SR, CB2, CB1, TIMER2, TIMER1, IRQ;
};
class VIA {
    uint64_t timer1_base;
    uint64_t timer2_base;
    SDL_TimerID timer1;
    SDL_TimerID timer2;
    uint16_t timer1_cnt;
    uint16_t timer2_cnt;
    uint8_t dirb, dira;
    uint8_t db, da;
    uint8_t timer1_lh[2];
    uint8_t timer2_lh[2];
    uint8_t sr;
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
    VIA_IRQ IF;
    VIA_IRQ IE;

  public:
    friend uint32_t via_timer1_callback(uint32_t, void *t);
    friend uint32_t via_timer2_callback(uint32_t, void *t);
    friend uint32_t rtc_callback(uint32_t, void *);
    uint8_t read(int n);
    void write(int n, uint8_t v);

    virtual bool readPA(int n) = 0;
    virtual bool readPB(int n) = 0;
    virtual void writePA(int n, bool v) = 0;
    virtual void writePB(int n, bool v) = 0;
    virtual uint8_t getSR() = 0;
    virtual void setSR(uint8_t v) = 0;
    void recieve_sr();
};
class VIA1 : public VIA {
    bool readPA(int n) override;
    bool readPB(int n) override;
    void writePA(int n, bool v) override;
    void writePB(int n, bool v) override;
    uint8_t getSR() override;
    void setSR(uint8_t v) override;
};
class VIA2 : public VIA {
    bool readPA(int n) override;
    bool readPB(int n) override;
    void writePA(int n, bool v) override;
    void writePB(int n, bool v) override;
    uint8_t getSR() override;
    void setSR(uint8_t v) override;
};
extern VIA1 via1;
extern VIA2 via2;
#endif