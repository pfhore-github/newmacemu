#include "ebus/serial.hpp"
#include "SDL3/SDL.h"
#include "SDL3_net/SDL_net.h"
#include "chip/scc.hpp"
#include <arpa/inet.h>
#include <deque>
#include <stdint.h>
NET_Server* base, * stty;
bool enableModemTty = true;
#define QUEUELIMIT 5
void initModemPort(SccCh *ch) {
    if(enableModemTty) {
    }
}
bool scc_wait_req() { return !(scc->chA.vSCCWrReq || scc->chB.vSCCWrReq); }
void sendModemPort(SccCh *, const uint8_t *v, size_t ln) {
    if(stty) {
//        SDLNet_TCP_Send(stty, v, ln);
    }
}

void sendPrinterPort(SccCh *, const uint8_t *, size_t) {}
