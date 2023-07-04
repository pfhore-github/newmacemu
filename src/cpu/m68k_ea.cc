#include "68040.hpp"
#include "exception.hpp"
#include "proto.hpp"
#include "memory.hpp"
#include <errno.h>
#include <memory>
#include <tuple>
#include <utility>
std::pair<std::function<uint32_t()>, int> ea_addr(int type, int reg,
                                                  uint32_t pc, int sz, bool rw);
static auto ea_Dx_R_B(int reg) {
    return [reg]() -> uint8_t { return cpu.D[reg]; };
}
static auto ea_Dx_R_W(int reg) {
    return [reg]() -> uint16_t { return cpu.D[reg]; };
}
static auto ea_Dx_R_L(int reg) {
    return [reg]() -> uint32_t { return cpu.D[reg]; };
}
static auto ea_Ax_R_W(int reg) {
    return [reg]() -> uint16_t { return cpu.A[reg]; };
}
static auto ea_Ax_R_L(int reg) {
    return [reg]() -> uint32_t { return cpu.A[reg]; };
}
static auto ea_Dx_W_B(int reg) {
    return [reg](uint8_t v) { STORE_B(cpu.D[reg], v); };
}
static auto ea_Dx_W_W(int reg) {
    return [reg](uint16_t v) { STORE_W(cpu.D[reg], v); };
}
static auto ea_Dx_W_L(int reg) {
    return [reg](uint32_t v) { cpu.D[reg] = v; };
}
static auto ea_Ax_W_W(int reg) {
    return [reg](uint16_t v) { cpu.A[reg] = static_cast<int16_t>(v); };
}
static auto ea_Ax_W_L(int reg) {
    return [reg](uint32_t v) { cpu.A[reg] = v; };
}
std::pair<std::function<uint8_t()>, int> ea_read8(int type, int reg,
                                                  uint32_t pc) {
    if(type == 0) {
        return {ea_Dx_R_B(reg), 0};
    } else if(type == 7 && reg == 4) {
        return {[b = FETCH(pc)]() -> uint8_t { return b; }, 2};
    } else {
        auto [f, n] = ea_addr(type, reg, pc, 1, false);
        return {[f = std::move(f)]() { return MMU_ReadB(cpu.EA = f()); }, n};
    }
}
std::pair<std::function<uint16_t()>, int> ea_read16(int type, int reg,
                                                    uint32_t pc) {
    if(type == 0) {
        return {ea_Dx_R_W(reg), 0};
    } else if(type == 1) {
        return {ea_Ax_R_W(reg), 0};
    } else if(type == 7 && reg == 4) {
        return {[w = FETCH(pc)]() -> uint16_t { return w; }, 2};
    } else {
        auto [f, n] = ea_addr(type, reg, pc, 2, false);
        return {[f = std::move(f)]() { return MMU_ReadW(cpu.EA = f()); }, n};
    }
}
std::pair<std::function<uint32_t()>, int> ea_read32(int type, int reg,
                                                    uint32_t pc) {
    if(type == 0) {
        return {ea_Dx_R_L(reg), 0};
    } else if(type == 1) {
        return {ea_Ax_R_L(reg), 0};
    } else if(type == 7 && reg == 4) {
        return {[l = FETCH32(pc)]() -> uint32_t { return l; }, 4};
    } else {
        auto [f, n] = ea_addr(type, reg, pc, 4, false);
        return {[f = std::move(f)]() { return MMU_ReadL(cpu.EA = f()); }, n};
    }
}

std::pair<std::function<void(uint8_t)>, int> ea_write8(int type, int reg,
                                                       uint32_t pc) {
    if(type == 0) {
        return {ea_Dx_W_B(reg), 0};
    } else {
        auto [f, n] = ea_addr(type, reg, pc, 1, true);
        return {[f = std::move(f)](uint8_t v) { MMU_WriteB(cpu.EA = f(), v); }, n};
    }
}
std::pair<std::function<void(uint16_t)>, int> ea_write16(int type, int reg,
                                                         uint32_t pc) {
    if(type == 0) {
        return {ea_Dx_W_W(reg), 0};
    } else if(type == 1) {
        return {ea_Ax_W_W(reg), 0};
    } else {
        auto [f, n] = ea_addr(type, reg, pc, 2, true);
        return {[f = std::move(f)](uint32_t v) { return MMU_WriteW(cpu.EA = f(), v); }, n};
    }
}
std::pair<std::function<void(uint32_t)>, int> ea_write32(int type, int reg,
                                                         uint32_t pc) {
    if(type == 0) {
        return {ea_Dx_W_L(reg), 0};
    } else if(type == 1) {
        return {ea_Ax_W_L(reg), 0};
    } else {
        auto [f, n] = ea_addr(type, reg, pc, 4, true);
        return {[f = std::move(f)](uint32_t v) { MMU_WriteL(cpu.EA = f(), v); }, n};
    }
}

std::tuple<std::function<uint8_t()>, std::function<void(uint8_t)>, int>
ea_rw8(int type, int reg, uint32_t pc) {
    if(type == 0) {
        return {ea_Dx_R_B(reg), ea_Dx_W_B(reg), 0};
    } else {
        auto [f, n] = ea_addr(type, reg, pc, 1, true);
        return {[f = std::move(f)]() { return MMU_ReadB(cpu.EA = f()); },
                [](uint8_t v) { MMU_WriteB(cpu.EA, v); }, n};
    }
}
std::tuple<std::function<uint16_t()>, std::function<void(uint16_t)>, int>
ea_rw16(int type, int reg, uint32_t pc) {
    if(type == 0) {
        return {ea_Dx_R_W(reg), ea_Dx_W_W(reg), 0};
    } else if(type == 1) {
        return {ea_Ax_R_W(reg), ea_Ax_W_W(reg), 0};
    } else {
        auto [f, n] = ea_addr(type, reg, pc, 2, true);
        return {[f = std::move(f)]() { return MMU_ReadW(cpu.EA = f()); },
                [](uint16_t v) { MMU_WriteW(cpu.EA, v); }, n};
    }
}
std::tuple<std::function<uint32_t()>, std::function<void(uint32_t)>, int>
ea_rw32(int type, int reg, uint32_t pc) {
    if(type == 0) {
        return {ea_Dx_R_L(reg), ea_Dx_W_L(reg), 0};
    } else if(type == 1) {
        return {ea_Ax_R_L(reg), ea_Ax_W_L(reg), 0};
    } else {
        auto [f, n] = ea_addr(type, reg, pc, 4, true);
        return {[f = std::move(f)]() { return MMU_ReadL(cpu.EA = f()); },
                [](uint32_t v) { MMU_WriteL(cpu.EA, v); }, n};
    }
}

inline uint32_t bit_ext(uint16_t v) { return static_cast<int16_t>(v); }
inline int32_t get_ri(int ri, int cc, bool w) {
    int32_t r = cpu.R(ri);
    if(w) {
        r = bit_ext(r);
    }
    return (r << cc);
}
std::pair<std::function<uint32_t()>, int>
ea_addr(int type, int reg, uint32_t pc, int sz, bool rw) {
    switch(type) {
    case 2: // (An)
        return {[reg]() { return cpu.A[reg]; }, 0};
    case 3: // (An)+
        if(sz == 0) {
            throw DecodeError{};
        }
        return {
            [reg, sz]() { return std::exchange(cpu.A[reg], cpu.A[reg] + sz); },
            0};
    case 4: // -(An)
        if(sz == 0) {
            throw DecodeError{};
        }
        return {[reg, sz]() { return cpu.A[reg] -= sz; }, 0};
    case 5: // (d, An)
    {
        int16_t d = FETCH(pc);
        return {[d, reg]() { return cpu.A[reg] + d; }, 2};
    }
    case 6: {
        uint16_t next = FETCH(pc);
        int ri = next >> 12 & 15;
        bool w = next >> 11 & 1;
        int cc = next >> 9 & 3;
        if(!(next & 1 << 8)) {
            int8_t d = next & 0xff;
            return {[w, d, reg, ri, cc]() {
                        return d + cpu.A[reg] + get_ri(ri, cc, w);
                    },
                    2};
        } else {
            if( next & 1 << 3) {
                throw DecodeError{};
            }
            bool a_i = next >> 7 & 1;
            bool r_i = next >> 6 & 1;
            int bd_c = next >> 4 & 3;
            bool pre = next >> 2 & 1;
            int od_c = next & 3;
            int of = 2;
            uint32_t bd = 0, od = 0;
            switch(bd_c) {
            case 0:
                throw DecodeError{};
            case 1:
                break;
            case 2:
                bd = FETCH(pc + of);
                of += 2;
                break;
            case 3:
                bd = FETCH32(pc + of);
                of += 4;
                break;
            }
            if(od_c == 2) {
                od = FETCH(pc + of);
                of += 2;
            } else if(od_c == 3) {
                od = FETCH32(pc + of);
                of += 4;
            }
            if(!pre) {
                if(od_c == 0) {
                    return {[bd, reg, ri, cc, w, a_i, r_i]() {
                                uint32_t base = bd;
                                if(!a_i) {
                                    base += cpu.A[reg];
                                }
                                if(!r_i) {
                                    base += get_ri(ri, cc, w);
                                }
                                return base;
                            },
                            of};
                } else {
                    return {[bd, od, reg, ri, cc, w, a_i, r_i]() {
                                uint32_t base = bd;
                                if(!a_i) {
                                    base += cpu.A[reg];
                                }
                                if(!r_i) {
                                    base += get_ri(ri, cc, w);
                                }
                                return MMU_ReadL(base) + od;
                            },
                            of};
                }
            } else {
                if(od_c == 0 || (a_i && r_i)) {
                    throw DecodeError{};
                }
                return {[bd, od, reg, ri, cc, w, a_i, r_i]() {
                            uint32_t base = bd;
                            if(!a_i) {
                                base += cpu.A[reg];
                            }
                            base = MMU_ReadL(base);
                            if(!r_i) {
                                base += get_ri(ri, cc, w);
                            }
                            return base + od;
                        },
                        of};
            }
        }
    }
    case 7:
        switch(reg) {
        case 0: {
            uint16_t wd = FETCH(pc);
            return {[wd]() { return wd; }, 2};
        }
        case 1: {
            uint32_t wd = FETCH32(pc);
            return {[wd]() { return wd; }, 4};
        }
        case 2: // (d, PC)
            if(rw) {
                throw DecodeError{};
            } else {
                int16_t d = FETCH(pc);
                return {[d, pc = pc]() { return pc + d; }, 2};
            }
        case 3:
            if(rw) {
                throw DecodeError{};
            } else {
                uint16_t next = FETCH(pc);
                int ri = next >> 12 & 15;
                bool w = next >> 11 & 1;
                int cc = next >> 9 & 3;
                if(!(next & 1 << 8)) {
                    int8_t d = next & 0xff;
                    return {[w, d, pc, ri, cc]() {
                                return d + pc + get_ri(ri, cc, w);
                            },
                            2};
                } else {
                    if( next & 1 << 3) {
                        throw DecodeError{};
                    }
                    bool a_i = next >> 7 & 1;
                    bool r_i = next >> 6 & 1;
                    int bd_c = next >> 4 & 3;
                    bool pre = next >> 2 & 1;
                    int od_c = next & 3;
                    int of = 2;
                    uint32_t bd = 0, od = 0;
                    switch(bd_c) {
                    case 0:
                        throw DecodeError{};
                    case 1:
                        break;
                    case 2:
                        bd = FETCH(pc + of);
                        of += 2;
                        break;
                    case 3:
                        bd = FETCH32(pc + of);
                        of += 4;
                        break;
                    }
                    if(od_c == 2) {
                        od = FETCH(pc + of);
                        of += 2;
                    } else if(od_c == 3) {
                        od = FETCH32(pc + of);
                        of += 4;
                    }
                    if(!pre) {
                        if(od_c == 0) {
                            return {[bd, pc, ri, cc, w, a_i, r_i]() {
                                        uint32_t base = bd;
                                        if(!a_i) {
                                            base += pc;
                                        }
                                        if(!r_i) {
                                            base += get_ri(ri, cc, w);
                                        }
                                        return base;
                                    },
                                    of};
                        } else {
                            return {[bd, od, pc, ri, cc, w, a_i, r_i]() {
                                        uint32_t base = bd;
                                        if(!a_i) {
                                            base += pc;
                                        }
                                        if(!r_i) {
                                            base += get_ri(ri, cc, w);
                                        }
                                        return MMU_ReadL(base) + od;
                                    },
                                    of};
                        }
                    } else {
                        if(od_c == 0 || (a_i && r_i)) {
                            throw DecodeError{};
                        }
                        return {[bd, od, pc, ri, cc, w, a_i, r_i]() {
                                    uint32_t base = bd;
                                    if(!a_i) {
                                        base += pc;
                                    }
                                    base = MMU_ReadL(base);
                                    if(!r_i) {
                                        base += get_ri(ri, cc, w);
                                    }
                                    return base + od;
                                },
                                of};
                    }
                }
            }
        }
    }
    throw DecodeError{};
}
