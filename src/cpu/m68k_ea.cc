#include "68040.hpp"
#include "exception.hpp"
#include "memory.hpp"
#include "inline.hpp"
#include <errno.h>
#include <fmt/format.h>
#include <memory>
#include <tuple>
#include <utility>

static uint32_t parseExt(int reg) {
    uint32_t base = reg >= 0 ? cpu.A[reg] : cpu.PC;
    uint16_t next = FETCH();
    int ri = next >> 12 & 15;
    bool ri_l = next & 1 << 11;
    int ri_c = next >> 9 & 3;
    uint32_t rn_v = cpu.R(ri);
    if(!ri_l) {
        rn_v = static_cast<int16_t>(rn_v & 0xffff);
    }
    rn_v <<= ri_c;
    if(!(next & 1 << 8)) {
        int8_t d = next & 0xff;
        return d + base + rn_v;
    }
    if(next & 1 << 7) {
        base = 0;
    }
    if(next & 1 << 6) {
        rn_v = 0;
    }
    int32_t bd = 0;
    switch(next >> 4 & 3) {
    case 1:
        break;
    case 2:
        bd = static_cast<int16_t>(FETCH());
        break;
    case 3:
        bd = FETCH32();
        break;
    }
    int32_t od_v = 0;
    switch(next & 3) {
    case 0:
        return bd + base + rn_v;
    case 1:
        break;
    case 2:
        od_v = static_cast<int16_t>(FETCH());
        break;
    case 3:
        od_v = FETCH32();
        break;
    }
    if(!(next & 1 << 2)) {
        return ReadL(bd + base + rn_v) + od_v;
    } else {
        return ReadL(bd + base) + rn_v + od_v;
    }
}

uint32_t ea_getaddr(int type, int reg, int sz) {
    switch(type) {
    case 2:
        return cpu.A[reg];
    case 3:
        if(reg == 7 && sz == 1) {
            sz = 2;
        }
        return std::exchange(cpu.A[reg], cpu.A[reg] + sz);
    case 4:
        if(reg == 7 && sz == 1) {
            sz = 2;
        }
        return cpu.A[reg] -= sz;
    case 5: {
        int16_t d = FETCH();
        return cpu.A[reg] + d;
    }
    case 6:
        return parseExt(reg);
    case 7:
        switch(reg) {
        case 0:
            return FETCH();
        case 1:
            return FETCH32();
        case 2: {
            uint32_t now = cpu.PC;
            int16_t d = FETCH();
            return now + d;
        }
        case 3:
            return parseExt(-1);
        }
    }
    ILLEGAL_OP();
}

uint8_t ea_readB(int type, int reg, bool lk) {
    if(type == 0) {
        return cpu.D[reg];
    } else if(type == 7 && reg == 4) {
        return FETCH();
    } else {
        cpu.EA = ea_getaddr(type, reg, 1);
        cpu.bus_lock = lk;
        return ReadB(cpu.EA);
    }
}

uint32_t ea_readW(int type, int reg, bool lk) {
    if(type == 0) {
        return cpu.D[reg] & 0xffff;
    } else if(type == 1) {
        return static_cast<int16_t>(cpu.A[reg]);
    } else if(type == 7 && reg == 4) {
        return FETCH();
    } else {
        cpu.EA = ea_getaddr(type, reg, 2);
        cpu.bus_lock = lk;
        return ReadW(cpu.EA);
    }
}

uint32_t ea_readL(int type, int reg, bool lk) {
    if(type == 0) {
        return cpu.D[reg];
    } else if(type == 1) {
        return cpu.A[reg];
    } else if(type == 7 && reg == 4) {
        return FETCH32();
    } else {
        cpu.EA = ea_getaddr(type, reg, 4);
        cpu.bus_lock = lk;
        return ReadL(cpu.EA);
    }
}

void ea_writeB(int type, int reg, uint8_t v) {
    if(type == 0) {
        STORE_B(cpu.D[reg], v);
    } else {
        if(!cpu.bus_lock) {
            cpu.EA = ea_getaddr(type, reg, 1);
        }
        WriteB(cpu.EA, v);
        cpu.bus_lock = false;
    }
}

void ea_writeW(int type, int reg, uint16_t v) {
    if(type == 0) {
        STORE_W(cpu.D[reg], v);
    } else if(type == 1) {
        cpu.A[reg] = static_cast<int16_t>(v);
    } else {
        if(!cpu.bus_lock) {
            cpu.EA = ea_getaddr(type, reg, 2);
        }
        WriteW(cpu.EA, v);
        cpu.bus_lock = false;
    }
}

void ea_writeL(int type, int reg, uint32_t v) {
    if(type == 0) {
        STORE_L(cpu.D[reg], v);
    } else if(type == 1) {
        cpu.A[reg] = v;
    } else {
        if(!cpu.bus_lock) {
            cpu.EA = ea_getaddr(type, reg, 4);
        }
        WriteL(cpu.EA, v);
        cpu.bus_lock = false;
    }
}
