
#include "68040.hpp"
#include "68040fpu.hpp"
#include "SDL3/SDL_timer.h"
#include "bus.hpp"
#include "inline.hpp"
#include "memory.hpp"
#include <asio.hpp>
#include <deque>
#include <expected>
#include <format>
#include <lua.hpp>
#include <mutex>
#include <print>
#include <signal.h>
#include <thread>
#include <unordered_set>
lua_State *LL;

template <typename O>
static int l_generic_index(
    lua_State *L,
    const std::unordered_map<std::string_view,
                             std::function<int(lua_State *, O &)>> &keys,
    O &obj) {
    const char *key = luaL_checkstring(L, 2);
    auto it = keys.find(key);
    if(it != keys.end()) {
        return it->second(L, obj);
    } else {
        return -1;
    }
}

template <typename T> struct GetMember {
    T Cpu::*mem;
    int operator()(lua_State *L, Cpu &cpu) const {
        lua_pushinteger(L, cpu.*mem);
        return 1;
    }
};

template <typename T> struct SetMember {
    T Cpu::*mem;
    int operator()(lua_State *L, Cpu &cpu) const {
        cpu.*mem = luaL_checkinteger(L, -1);
        return 0;
    }
};

static int lua_cpu_index(lua_State *L) {
    static const std::unordered_map<std::string_view,
                                    std::function<int(lua_State *, Cpu &)>>
        CPUMap = {
            {"PC", GetMember{&Cpu::PC}},
            {"MSP", GetMember{&Cpu::MSP}},
            {"ISP", GetMember{&Cpu::ISP}},
            {"USP", GetMember{&Cpu::USP}},
            {"VBR", GetMember{&Cpu::VBR}},
            {"SFC", GetMember{&Cpu::SFC}},
            {"DFC", GetMember{&Cpu::DFC}},
            {"URP", GetMember{&Cpu::URP}},
            {"SRP", GetMember{&Cpu::SRP}},
            // these are read only
            {"ex_addr", GetMember{&Cpu::ex_addr}},
            {"EA", GetMember{&Cpu::EA}},
        };
    if(l_generic_index(L, CPUMap, cpu) != 1) {
        lua_rawget(L, 1);
    }
    return 1;
}
static int lua_cpu_newindex(lua_State *L) {
    static const std::unordered_map<std::string_view,
                                    std::function<int(lua_State *, Cpu &)>>
        CPUMap2 = {
            {"PC", SetMember{&Cpu::PC}},
            {"MSP", SetMember{&Cpu::MSP}},
            {"ISP", SetMember{&Cpu::ISP}},
            {"USP", SetMember{&Cpu::USP}},
            {"VBR", SetMember{&Cpu::VBR}},
            {"SFC", SetMember{&Cpu::SFC}},
            {"DFC", SetMember{&Cpu::DFC}},
            {"URP", SetMember{&Cpu::URP}},
            {"SRP", SetMember{&Cpu::SRP}},
        };
    if(l_generic_index(L, CPUMap2, cpu) == -1) {
        lua_rawset(L, 1);
    }
    return 0;
}
static luaL_Reg cpu_meta[] = {
    {"__index", lua_cpu_index},
    {"__newindex", lua_cpu_newindex},
    {nullptr, nullptr},
};
template <typename T> struct CastGet {
    int operator()(lua_State *L, uint32_t &val) {
        lua_pushinteger(L, static_cast<T>(val));
        return 1;
    }
};

// L,W,B(unsigned) I,H,C(signed)
static int lua_cpuD_index(lua_State *L) {
    int index = lua_tointeger(L, lua_upvalueindex(1));
    static const std::unordered_map<
        std::string_view, std::function<int(lua_State * L, uint32_t &)>>
        dmap = {
            {"L", CastGet<uint32_t>{}}, {"W", CastGet<uint16_t>{}},
            {"B", CastGet<uint8_t>{}},  {"I", CastGet<int32_t>{}},
            {"H", CastGet<int16_t>{}},  {"C", CastGet<int8_t>{}},
        };
    if(l_generic_index(L, dmap, cpu.D[index]) == -1) {
        return luaL_argerror(L, 2, "invalid type");
    }
    return 1;
}
int lua_cpuD_newindex(lua_State *L) {
    static const std::unordered_map<
        std::string_view, std::function<int(lua_State * L, uint32_t &)>>
        dmap = {
            {"L",
             [](lua_State *L, uint32_t &dst) {
                 dst = luaL_checkinteger(L, -1);
                 return 0;
             }},
            {"I",
             [](lua_State *L, uint32_t &dst) {
                 dst = luaL_checkinteger(L, -1);
                 return 0;
             }},
            {"W",
             [](lua_State *L, uint32_t &dst) {
                 dst = (dst & 0xffff0000) |
                       static_cast<uint16_t>(luaL_checkinteger(L, -1));
                 return 0;
             }},
            {"H",
             [](lua_State *L, uint32_t &dst) {
                 dst =
                     (dst & 0xffff0000) |
                     (static_cast<int16_t>(luaL_checkinteger(L, -1)) & 0xffff);
                 return 0;
             }},
            {"B",
             [](lua_State *L, uint32_t &dst) {
                 dst = (dst & 0xffffff00) |
                       static_cast<uint8_t>(luaL_checkinteger(L, -1));
                 return 0;
             }},
            {"C",
             [](lua_State *L, uint32_t &dst) {
                 dst = (dst & 0xffffff00) |
                       (static_cast<int8_t>(luaL_checkinteger(L, -1)) & 0xff);
                 return 0;
             }},
        };
    if(l_generic_index(L, dmap, cpu.D[lua_tointeger(L, lua_upvalueindex(1))]) ==
       -1) {
        return luaL_argerror(L, 2, "invalid type");
    }
    return 0;
}

static luaL_Reg cpu_Dx_meta[] = {
    {"__index", lua_cpuD_index},
    {"__newindex", lua_cpuD_newindex},
    {nullptr, nullptr},
};

// L,W, I, H
static int lua_cpuA_index(lua_State *L) {
    int index = lua_tointeger(L, 2);
    luaL_argcheck(L, index > 0 && index <= 8, 2, "invalid index");
    lua_pushinteger(L, cpu.A[index - 1]);
    return 1;
}

static int lua_cpuA_newindex(lua_State *L) {
    int index = lua_tointeger(L, 2);
    luaL_argcheck(L, index > 0 && index <= 8, 2, "invalid index");
    uint32_t value = luaL_checkinteger(L, 3);
    cpu.A[index - 1] = value;
    return 0;
}

static luaL_Reg cpu_Ax_meta[] = {
    {"__index", lua_cpuA_index},
    {"__newindex", lua_cpuA_newindex},
    {nullptr, nullptr},
};

template <typename T> struct GetFlag {
    bool T::*f;
    int operator()(lua_State *L, T &o) const {
        lua_pushboolean(L, o.*f);
        return 1;
    }
};
template <typename T> struct SetFlag {
    bool T::*f;
    int operator()(lua_State *L, T &o) const {
        o.*f = lua_toboolean(L, 3);
        return 0;
    }
};

template <typename T> struct GetValue {
    uint8_t T::*f;
    int operator()(lua_State *L, T &o) const {
        lua_pushinteger(L, o.*f);
        return 1;
    }
};
template <typename T> struct SetValue {
    uint8_t T::*f;
    int operator()(lua_State *L, T &o) const {
        o.*f = luaL_checkinteger(L, 3);
        return 0;
    }
};

static int lua_getcpuCC(lua_State *L) {
    static const std::unordered_map<std::string_view,
                                    std::function<int(lua_State * L, Cpu &)>>
        ccmap = {
            {"value",
             [](lua_State *L, Cpu &cpu) {
                 lua_pushinteger(L, GetCCR(cpu));
                 return 1;
             }},
            {"C", GetFlag{&Cpu::C}},
            {"V", GetFlag{&Cpu::V}},
            {"Z", GetFlag{&Cpu::Z}},
            {"N", GetFlag{&Cpu::N}},
            {"X", GetFlag{&Cpu::X}},
        };
    if(l_generic_index(L, ccmap, cpu) == -1) {
        return luaL_argerror(L, 2, "invalid name");
    }
    return 1;
}
static int lua_setcpuCC(lua_State *L) {
    static const std::unordered_map<std::string_view,
                                    std::function<int(lua_State * L, Cpu &)>>
        ccmap = {
            {"value",
             [](lua_State *L, Cpu &cpu) {
                 uint32_t value = luaL_checkinteger(L, 3);
                 SetCCR(cpu, value);
                 return 0;
             }},
            {"C", SetFlag{&Cpu::C}},
            {"V", SetFlag{&Cpu::V}},
            {"Z", SetFlag{&Cpu::Z}},
            {"N", SetFlag{&Cpu::N}},
            {"X", SetFlag{&Cpu::X}},
        };
    if(l_generic_index(L, ccmap, cpu) == -1) {
        return luaL_argerror(L, 2, "invalid name");
    }
    return 0;
}

static luaL_Reg cpu_cc_meta[] = {
    {"__index", lua_getcpuCC},
    {"__newindex", lua_setcpuCC},
    {nullptr, nullptr},
};
static const char *TM_str[] = {"ALT 0",       "User data", "User code",
                               "MMU data",    "MMU code",  "system data",
                               "system code", "ALT 7",     nullptr};
static const char *TT_str[] = {"normal", "move16", "alt", "ack", nullptr};
static const char *SIZ_str[] = {"long", "byte", "word", "line", nullptr};
struct ReadFlags {
    uint16_t flag;
    int operator()(lua_State *L, Cpu &cpu) const {
       lua_pushboolean(L, cpu.fault_SSW & flag);
                 return 1;
    }
};
static int lua_getcpuSSW(lua_State *L) {
    static const std::unordered_map<std::string_view,
                                    std::function<int(lua_State * L, Cpu &)>>
        sswmap = {
            {"value",
             [](lua_State *L, Cpu &cpu) {
                 lua_pushinteger(L, cpu.fault_SSW);
                 return 1;
             }},
            {"TTM",
             [](lua_State *L, Cpu &cpu) {
                 lua_pushstring(L, TM_str[cpu.fault_SSW & 7]);
                 return 1;
             }},
            {"TT",
             [](lua_State *L, Cpu &cpu) {
                 lua_pushstring(L, TT_str[cpu.fault_SSW >> 3 & 3]);
                 return 1;
             }},
            {"SZ",
             [](lua_State *L, Cpu &cpu) {
                 lua_pushstring(L, SIZ_str[cpu.fault_SSW >> 3 & 3]);
                 return 1;
             }},
            {"RW", ReadFlags{SSW_RW}},
            {"LK", ReadFlags{SSW_LK}},
            {"ATC", ReadFlags{SSW_ATC}},
            {"MA", ReadFlags{SSW_MA}},
            {"CM", ReadFlags{SSW_CM}},
            {"CT", ReadFlags{SSW_CT}},
            {"CU", ReadFlags{SSW_CU}},
            {"CP", ReadFlags{SSW_CP}},
        };
    if(l_generic_index(L, sswmap, cpu) == -1) {
        return luaL_argerror(L, 2, "invalid name");
    }
    return 1;
}
struct UpdateFlags {
    uint16_t flag;
    int operator()(lua_State *L, Cpu &cpu) const {
        if(lua_toboolean(L, 3)) {
            cpu.fault_SSW |= flag;
        } else {
            cpu.fault_SSW &= ~flag;
        }
        return 0;
    }
};
static int lua_setcpuSSW(lua_State *L) {
    static const std::unordered_map<std::string_view,
                                    std::function<int(lua_State * L, Cpu &)>>
        ccmap = {
            {"value",
             [](lua_State *L, Cpu &cpu) {
                 uint16_t value = luaL_checkinteger(L, 3);
                 cpu.fault_SSW = value;
                 return 0;
             }},
            {"TTM",
             [](lua_State *L, Cpu &cpu) {
                 int ttm = luaL_checkoption(L, 3, nullptr, TM_str);
                 cpu.fault_SSW = (cpu.fault_SSW & ~7) | ttm;
                 return 0;
             }},
            {"TT",
             [](lua_State *L, Cpu &cpu) {
                 int tt = luaL_checkoption(L, 3, nullptr, TT_str);
                 cpu.fault_SSW = (cpu.fault_SSW & ~(3 << 3)) | (tt << 3);
                 return 0;
             }},
            {"SZ",
             [](lua_State *L, Cpu &cpu) {
                 int sz = luaL_checkoption(L, 3, nullptr, SIZ_str);
                 cpu.fault_SSW = (cpu.fault_SSW & ~(3 << 5)) | (sz << 5);
                 return 0;
             }},
            {"RW", UpdateFlags{SSW_RW}},
            {"LK", UpdateFlags{SSW_LK}},
            {"ATC", UpdateFlags{SSW_ATC}},
            {"MA", UpdateFlags{SSW_MA}},
            {"CM", UpdateFlags{SSW_CM}},
            {"CT", UpdateFlags{SSW_CT}},
            {"CU", UpdateFlags{SSW_CU}},
            {"CP", UpdateFlags{SSW_CP}},         
        };
    if(l_generic_index(L, ccmap, cpu) == -1) {
        return luaL_argerror(L, 2, "invalid name");
    }
    return 0;
}

static luaL_Reg cpu_ssw_meta[] = {
    {"__index", lua_getcpuSSW},
    {"__newindex", lua_setcpuSSW},
    {nullptr, nullptr},
};

struct lua_fp80 {
    mpfr_t fp;
    uint64_t nan_v;
};
int lua_FPUFp_index(lua_State *L) {
    int n = luaL_checkinteger(L, 2) - 1;
    if(n < 0 || n > 7) {
        lua_pushnil(L);
        return 1;
    }
    lua_fp80 *fp =
        static_cast<lua_fp80 *>(lua_newuserdata(L, sizeof(lua_fp80)));
    auto fpu = FPU_P();
    luaL_setmetatable(L, "M68K.FP80");
    mpfr_init_set(fp->fp, fpu->FP[n], MPFR_RNDN);
    fp->nan_v = fpu->FP_nan[n];
    return 1;
}

int lua_FPUFp_newindex(lua_State *L) {
    int n = luaL_checkinteger(L, 2) - 1;
    luaL_argcheck(L, n >= 0 && n <= 7, 2, "invalid index");
    if(auto fp = static_cast<lua_fp80 *>(luaL_testudata(L, 3, "M68K.FP80"))) {
        mpfr_set(FPU_P()->FP[n], fp->fp, MPFR_RNDN);
    } else if(lua_isinteger(L, 3)) {
        mpfr_set_si(FPU_P()->FP[n], lua_tointeger(L, 3), MPFR_RNDN);
    } else if(lua_isnumber(L, 3)) {
        mpfr_set_d(FPU_P()->FP[n], lua_tonumber(L, 3), MPFR_RNDN);
    } else {
        luaL_argerror(L, n, "invalid value");
    }
    return 0;
}

static luaL_Reg fpu_fp_meta[] = {
    {"__index", lua_FPUFp_index},
    {"__newindex", lua_FPUFp_newindex},
    {nullptr, nullptr},
};
static inline int rnd2Fpcr(mpfr_rnd_t r) {
    switch(r) {
    case MPFR_RNDN:
        return 0;
    case MPFR_RNDZ:
        return 1;
    case MPFR_RNDU:
        return 3;
    case MPFR_RNDD:
        return 2;
    default:
        return 0;
    }
}
static inline const char *fpcrRndStr(mpfr_rnd_t r) {
    switch(r) {
    case MPFR_RNDN:
        return "RN";
    case MPFR_RNDZ:
        return "RZ";
    case MPFR_RNDU:
        return "RP";
    case MPFR_RNDD:
        return "RM";
    default:
        return "";
    }
}

static const char *prec_map[] = {"X", "S", "D", nullptr};
static int lua_fpuFPCR_index(lua_State *L) {
    static const std::unordered_map<std::string_view,
                                    std::function<int(lua_State * L, FPCR_t &)>>
        fpcrmap = {
            {"value",
             [](lua_State *L, FPCR_t &fpcr) {
                 lua_pushinteger(L, fpcr.value());
                 return 1;
             }},
            {"BSUN", GetFlag{&FPCR_t::BSUN}},
            {"SNAN", GetFlag{&FPCR_t::S_NAN}},
            {"OPERR", GetFlag{&FPCR_t::OPERR}},
            {"OVFL", GetFlag{&FPCR_t::OVFL}},
            {"UNFL", GetFlag{&FPCR_t::UNFL}},
            {"DZ", GetFlag{&FPCR_t::DZ}},
            {"INEX2", GetFlag{&FPCR_t::INEX2}},
            {"INEX1", GetFlag{&FPCR_t::INEX1}},
            {"PREC",
             [](lua_State *L, FPCR_t &fpcr) {
                 lua_pushstring(L, prec_map[(int)fpcr.PREC]);
                 return 1;
             }},
            {"RND",
             [](lua_State *L, FPCR_t &fpcr) {
                 lua_pushstring(L, fpcrRndStr(fpcr.RND));
                 return 1;
             }},
        };
    if(l_generic_index(L, fpcrmap, FPU_P()->FPCR) == -1) {
        return luaL_argerror(L, 2, "invalid name");
    }
    return 1;
}
static int lua_fpuFPCR_newindex(lua_State *L) {
    static const std::unordered_map<std::string_view,
                                    std::function<int(lua_State * L, FPCR_t &)>>
        fpcrmap = {
            {"value",
             [](lua_State *L, FPCR_t &fpcr) {
                 fpcr.set(luaL_checkinteger(L, -1));
                 return 0;
             }},
            {"BSUN", SetFlag{&FPCR_t::BSUN}},
            {"SNAN", SetFlag{&FPCR_t::S_NAN}},
            {"OPERR", SetFlag{&FPCR_t::OPERR}},
            {"OVFL", SetFlag{&FPCR_t::OVFL}},
            {"UNFL", SetFlag{&FPCR_t::UNFL}},
            {"DZ", SetFlag{&FPCR_t::DZ}},
            {"INEX2", SetFlag{&FPCR_t::INEX2}},
            {"INEX1", SetFlag{&FPCR_t::INEX1}},
            {"PREC",
             [](lua_State *L, FPCR_t &fpcr) {
                 fpcr.PREC =
                     FPU_PREC(luaL_checkoption(L, -1, nullptr, prec_map));
                 return 0;
             }},
            {"RND",
             [](lua_State *L, FPCR_t &fpcr) {
                 const char *val = luaL_checkstring(L, 3);
                 if(strcmp(val, "RN") == 0) {
                     fpcr.RND = MPFR_RNDN;
                 } else if(strcmp(val, "RZ") == 0) {
                     fpcr.RND = MPFR_RNDZ;
                 } else if(strcmp(val, "RM") == 0) {
                     fpcr.RND = MPFR_RNDD;
                 } else if(strcmp(val, "RP") == 0) {
                     fpcr.RND = MPFR_RNDU;
                 } else {
                     luaL_argerror(L, 3, "invalid value");
                 }
                 return 0;
             }},
        };
    if(l_generic_index(L, fpcrmap, FPU_P()->FPCR) == -1) {
        return luaL_argerror(L, 2, "invalid name");
    }
    return 0;
}

static luaL_Reg fpu_fpcr_meta[] = {
    {"__index", lua_fpuFPCR_index},
    {"__newindex", lua_fpuFPCR_newindex},
    {nullptr, nullptr},
};

static int lua_fpuFPSR_index(lua_State *L) {
    static const std::unordered_map<std::string_view,
                                    std::function<int(lua_State * L, FPSR_t &)>>
        fpcrmap = {
            {"value",
             [](lua_State *L, FPSR_t &fpsr) {
                 lua_pushinteger(L, fpsr.value());
                 return 1;
             }},
            {"N", GetFlag{&FPSR_t::CC_N}},
            {"Z", GetFlag{&FPSR_t::CC_Z}},
            {"I", GetFlag{&FPSR_t::CC_I}},
            {"NAN", GetFlag{&FPSR_t::CC_NAN}},
            {"Quat",
             [](lua_State *L, FPSR_t &fpsr) {
                 int qt = fpsr.Quat;
                 lua_pushinteger(L, fpsr.QuatSign ? -qt : qt);
                 return 1;
             }},
            {"BSUN", GetFlag{&FPSR_t::BSUN}},
            {"SNAN", GetFlag{&FPSR_t::S_NAN}},
            {"OPERR", GetFlag{&FPSR_t::OPERR}},
            {"OVFL", GetFlag{&FPSR_t::OVFL}},
            {"UNFL", GetFlag{&FPSR_t::UNFL}},
            {"DZ", GetFlag{&FPSR_t::DZ}},
            {"INEX2", GetFlag{&FPSR_t::INEX2}},
            {"INEX1", GetFlag{&FPSR_t::INEX1}},
            {"EXC_IOP", GetFlag{&FPSR_t::EXC_IOP}},
            {"EXC_OVFL", GetFlag{&FPSR_t::EXC_OVFL}},
            {"EXC_UNFL", GetFlag{&FPSR_t::EXC_UNFL}},
            {"EXC_DZ", GetFlag{&FPSR_t::EXC_DZ}},
            {"EXC_INEX", GetFlag{&FPSR_t::EXC_INEX}},
        };
    if(l_generic_index(L, fpcrmap, FPU_P()->FPSR) == -1) {
        return luaL_argerror(L, 2, "invalid name");
    }
    return 1;
}
static int lua_fpuFPSR_newindex(lua_State *L) {
    static const std::unordered_map<std::string_view,
                                    std::function<int(lua_State * L, FPSR_t &)>>
        fpcrmap = {
            {"value",
             [](lua_State *L, FPSR_t &fpsr) {
                 fpsr.set(luaL_checkinteger(L, 3));
                 return 0;
             }},
            {"N", SetFlag{&FPSR_t::CC_N}},
            {"Z", SetFlag{&FPSR_t::CC_Z}},
            {"I", SetFlag{&FPSR_t::CC_I}},
            {"NAN", SetFlag{&FPSR_t::CC_NAN}},
            {"Quat",
             [](lua_State *L, FPSR_t &fpsr) {
                 int val = luaL_checkinteger(L, 3);
                 fpsr.QuatSign = val < 0;
                 fpsr.Quat = abs(val) & 0x7f;
                 return 0;
             }},
            {"BSUN", SetFlag{&FPSR_t::BSUN}},
            {"SNAN", SetFlag{&FPSR_t::S_NAN}},
            {"OPERR", SetFlag{&FPSR_t::OPERR}},
            {"OVFL", SetFlag{&FPSR_t::OVFL}},
            {"UNFL", SetFlag{&FPSR_t::UNFL}},
            {"DZ", SetFlag{&FPSR_t::DZ}},
            {"INEX2", SetFlag{&FPSR_t::INEX2}},
            {"INEX1", SetFlag{&FPSR_t::INEX1}},
            {"EXC_IOP", SetFlag{&FPSR_t::EXC_IOP}},
            {"EXC_OVFL", SetFlag{&FPSR_t::EXC_OVFL}},
            {"EXC_UNFL", SetFlag{&FPSR_t::EXC_UNFL}},
            {"EXC_DZ", SetFlag{&FPSR_t::EXC_DZ}},
            {"EXC_INEX", SetFlag{&FPSR_t::EXC_INEX}},
        };
    if(l_generic_index(L, fpcrmap, FPU_P()->FPSR) == -1) {
        return luaL_argerror(L, 2, "invalid name");
    }
    return 0;
}

static luaL_Reg fpu_fpsr_meta[] = {
    {"__index", lua_fpuFPSR_index},
    {"__newindex", lua_fpuFPSR_newindex},
    {nullptr, nullptr},
};

static int lua_getcpuSR(lua_State *L) {
    static const std::unordered_map<std::string_view,
                                    std::function<int(lua_State * L, Cpu &)>>
        ccmap = {
            {"value",
             [](lua_State *L, Cpu &cpu) {
                 lua_pushinteger(L, GetSR(cpu));
                 return 1;
             }},
            {"C", GetFlag{&Cpu::C}},
            {"V", GetFlag{&Cpu::V}},
            {"Z", GetFlag{&Cpu::Z}},
            {"N", GetFlag{&Cpu::N}},
            {"X", GetFlag{&Cpu::X}},
            {"I", GetValue{&Cpu::I}},
            {"M", GetFlag{&Cpu::M}},
            {"S", GetFlag{&Cpu::S}},
            {"T", GetValue{&Cpu::T}},
        };
    if(l_generic_index(L, ccmap, cpu) == -1) {
        return luaL_argerror(L, 2, "invalid name");
    }
    return 1;
}
static int lua_setcpuSR(lua_State *L) {
    static const std::unordered_map<std::string_view,
                                    std::function<int(lua_State * L, Cpu &)>>
        ccmap = {
            {"value",
             [](lua_State *L, Cpu &cpu) {
                 uint32_t value = luaL_checkinteger(L, 3);
                 SetSR(cpu, value);
                 return 0;
             }},
            {"C", SetFlag{&Cpu::C}},
            {"V", SetFlag{&Cpu::V}},
            {"Z", SetFlag{&Cpu::Z}},
            {"N", SetFlag{&Cpu::N}},
            {"X", SetFlag{&Cpu::X}},
            {"I", SetValue{&Cpu::I}},
            {"M",
             [](lua_State *L, Cpu &cpu) {
                 SaveSP();
                 cpu.M = lua_toboolean(L, 3);
                 LoadSP();
                 return 0;
             }},
            {"S",
             [](lua_State *L, Cpu &cpu) {
                 SaveSP();
                 cpu.S = lua_toboolean(L, 3);
                 LoadSP();
                 return 0;
             }},

            {"T", SetValue{&Cpu::T}},
        };
    if(l_generic_index(L, ccmap, cpu) == -1) {
        return luaL_argerror(L, 2, "invalid name");
    }
    return 0;
}

static luaL_Reg cpu_sr_meta[] = {
    {"__index", lua_getcpuSR},
    {"__newindex", lua_setcpuSR},
    {nullptr, nullptr},
};

static int lua_getcpuTCR(lua_State *L) {
    const char *key = luaL_checkstring(L, 2);
    if(strcmp(key, "value") == 0) {
        lua_pushinteger(L, cpu.TCR_E << 15 | cpu.TCR_P << 14);
    } else if(strcmp(key, "E") == 0) {
        lua_pushboolean(L, cpu.TCR_E);
    } else if(strcmp(key, "P") == 0) {
        lua_pushboolean(L, cpu.TCR_P);
    } else {
        return luaL_argerror(L, 2, "invalid name");
    }
    return 1;
}
static int lua_setcpuTCR(lua_State *L) {
    const char *key = luaL_checkstring(L, 2);
    if(strcmp(key, "value") == 0) {
        uint32_t value = luaL_checkinteger(L, 3);
        cpu.TCR_E = value & 1 << 15;
        cpu.TCR_P = value & 1 << 14;
    } else if(strcmp(key, "E") == 0) {
        cpu.TCR_E = lua_toboolean(L, 3);
    } else if(strcmp(key, "P") == 0) {
        cpu.TCR_P = lua_toboolean(L, 3);
    } else {
        return luaL_argerror(L, 2, "invalid name");
    }
    return 0;
}

static luaL_Reg cpu_tcr_meta[] = {
    {"__index", lua_getcpuTCR},
    {"__newindex", lua_setcpuTCR},
    {nullptr, nullptr},
};

uint32_t Get_TTR_t(const Cpu::TTR_t &x);
static int lua_getcpuTTR(lua_State *L) {
    const char *key = luaL_checkstring(L, 2);
    auto tp = (Cpu::TTR_t *)lua_touserdata(L, lua_upvalueindex(1));
    if(strcmp(key, "value") == 0) {
        lua_pushinteger(L, Get_TTR_t(*tp));
    } else if(strcmp(key, "logic_base") == 0) {
        lua_pushinteger(L, tp->logic_base << 24);
    } else if(strcmp(key, "logic_mask") == 0) {
        lua_pushinteger(L, tp->logic_mask << 24);
    } else if(strcmp(key, "E") == 0) {
        lua_pushboolean(L, tp->E);
    } else if(strcmp(key, "S") == 0) {
        lua_pushinteger(L, tp->S);
    } else if(strcmp(key, "U") == 0) {
        lua_pushinteger(L, tp->U);
    } else if(strcmp(key, "CM") == 0) {
        lua_pushinteger(L, tp->CM);
    } else if(strcmp(key, "W") == 0) {
        lua_pushboolean(L, tp->W);
    } else {
        return luaL_argerror(L, 2, "invalid name");
    }
    return 1;
}
void Set_TTR_t(Cpu::TTR_t &x, uint32_t v);
static int lua_setcpuTTR(lua_State *L) {
    const char *key = luaL_checkstring(L, 2);
    auto tp = (Cpu::TTR_t *)lua_touserdata(L, lua_upvalueindex(1));
    if(strcmp(key, "value") == 0) {
        Set_TTR_t(*tp, luaL_checkinteger(L, 3));
    } else if(strcmp(key, "logic_base") == 0) {
        tp->logic_base = luaL_checkinteger(L, 3) >> 24;
    } else if(strcmp(key, "logic_mask") == 0) {
        tp->logic_mask = luaL_checkinteger(L, 3) >> 24;
    } else if(strcmp(key, "E") == 0) {
        tp->E = lua_toboolean(L, 3);
    } else if(strcmp(key, "S") == 0) {
        int s = luaL_checkinteger(L, 3);
        luaL_argcheck(L, s >= 0 && s < 4, 3, "invalid value");
        tp->S = s;
    } else if(strcmp(key, "U") == 0) {
        int u = luaL_checkinteger(L, 3);
        luaL_argcheck(L, u >= 0 && u < 4, 3, "invalid value");
        tp->U = u;
    } else if(strcmp(key, "CM") == 0) {
        int cm = luaL_checkinteger(L, 3);
        luaL_argcheck(L, cm >= 0 && cm < 4, 3, "invalid value");
        tp->CM = cm;
    } else if(strcmp(key, "W") == 0) {
        tp->W = lua_toboolean(L, 3);
    } else {
        return luaL_argerror(L, 2, "invalid name");
    }
    return 0;
}

static luaL_Reg cpu_ttr_meta[] = {
    {"__index", lua_getcpuTTR},
    {"__newindex", lua_setcpuTTR},
    {nullptr, nullptr},
};

static int lua_getcpuMMUSR(lua_State *L) {
    const char *key = luaL_checkstring(L, 2);
    if(strcmp(key, "value") == 0) {
        lua_pushinteger(L, cpu.MMUSR);
    } else if(strcmp(key, "address") == 0) {
        lua_pushinteger(L, cpu.MMUSR & ~0xFFF);
    } else if(strcmp(key, "B") == 0) {
        lua_pushboolean(L, cpu.MMUSR & 1 << 11);
    } else if(strcmp(key, "G") == 0) {
        lua_pushboolean(L, cpu.MMUSR & 1 << 10);
    } else if(strcmp(key, "U") == 0) {
        lua_pushinteger(L, cpu.MMUSR >> 8 & 3);
    } else if(strcmp(key, "S") == 0) {
        lua_pushboolean(L, cpu.MMUSR & 1 << 7);
    } else if(strcmp(key, "CM") == 0) {
        lua_pushinteger(L, cpu.MMUSR >> 5 & 3);
    } else if(strcmp(key, "M") == 0) {
        lua_pushboolean(L, cpu.MMUSR & 1 << 4);
    } else if(strcmp(key, "W") == 0) {
        lua_pushboolean(L, cpu.MMUSR & 1 << 2);
    } else if(strcmp(key, "T") == 0) {
        lua_pushboolean(L, cpu.MMUSR & 1 << 1);
    } else if(strcmp(key, "R") == 0) {
        lua_pushboolean(L, cpu.MMUSR & 1);
    } else {
        return luaL_argerror(L, 2, "invalid name");
    }
    return 1;
}
static int lua_setcpuMMUSR(lua_State *L) {
    const char *key = luaL_checkstring(L, 2);
    if(strcmp(key, "value") == 0) {
        cpu.MMUSR = luaL_checkinteger(L, 3);
    } else if(strcmp(key, "address") == 0) {
        cpu.MMUSR = (cpu.MMUSR & 0xFFF) | (luaL_checkinteger(L, 3) & ~0xFFF);
    } else if(strcmp(key, "B") == 0) {
        cpu.MMUSR = (cpu.MMUSR & ~(1 << 11)) | lua_toboolean(L, 3) << 11;
    } else if(strcmp(key, "G") == 0) {
        cpu.MMUSR = (cpu.MMUSR & ~(1 << 10)) | lua_toboolean(L, 3) << 10;
    } else if(strcmp(key, "U") == 0) {
        int u = luaL_checkinteger(L, 3);
        luaL_argcheck(L, u >= 0 && u < 4, 3, "invalid value");
        cpu.MMUSR = (cpu.MMUSR & ~(3 << 8)) | u << 8;
    } else if(strcmp(key, "S") == 0) {
        cpu.MMUSR = (cpu.MMUSR & ~(1 << 7)) | lua_toboolean(L, 3) << 7;
    } else if(strcmp(key, "CM") == 0) {
        int cm = luaL_checkinteger(L, 3);
        luaL_argcheck(L, cm >= 0 && cm < 4, 3, "invalid value");
        cpu.MMUSR = (cpu.MMUSR & ~(3 << 5)) | cm << 5;
    } else if(strcmp(key, "M") == 0) {
        cpu.MMUSR = (cpu.MMUSR & ~(1 << 4)) | lua_toboolean(L, 3) << 4;
    } else if(strcmp(key, "W") == 0) {
        cpu.MMUSR = (cpu.MMUSR & ~(1 << 2)) | lua_toboolean(L, 3) << 2;
    } else if(strcmp(key, "T") == 0) {
        cpu.MMUSR = (cpu.MMUSR & ~(1 << 1)) | lua_toboolean(L, 3) << 1;
    } else if(strcmp(key, "R") == 0) {
        cpu.MMUSR = (cpu.MMUSR & ~1) | lua_toboolean(L, 3);
    } else {
        return luaL_argerror(L, 2, "invalid name");
    }
    return 0;
}

static luaL_Reg cpu_mmusr_meta[] = {
    {"__index", lua_getcpuMMUSR},
    {"__newindex", lua_setcpuMMUSR},
    {nullptr, nullptr},
};

static int lua_getcpuCACR(lua_State *L) {
    const char *key = luaL_checkstring(L, 2);
    if(strcmp(key, "value") == 0) {
        lua_pushinteger(L, cpu.CACR_DE << 31 | cpu.CACR_IE << 15);
    } else if(strcmp(key, "DE") == 0) {
        lua_pushboolean(L, cpu.CACR_DE);
    } else if(strcmp(key, "IE") == 0) {
        lua_pushboolean(L, cpu.CACR_IE);
    } else {
        return luaL_argerror(L, 2, "invalid name");
    }
    return 1;
}
static int lua_setcpuCACR(lua_State *L) {
    const char *key = luaL_checkstring(L, 2);
    if(strcmp(key, "value") == 0) {
        uint32_t value = luaL_checkinteger(L, 3);
        cpu.CACR_DE = value & 1 << 31;
        cpu.CACR_IE = value & 1 << 15;
    } else if(strcmp(key, "DE") == 0) {
        cpu.CACR_DE = lua_toboolean(L, 3);
    } else if(strcmp(key, "IE") == 0) {
        cpu.CACR_IE = lua_toboolean(L, 3);
    } else {
        return luaL_argerror(L, 2, "invalid name");
    }
    return 0;
}

static luaL_Reg cpu_cacr_meta[] = {
    {"__index", lua_getcpuCACR},
    {"__newindex", lua_setcpuCACR},
    {nullptr, nullptr},
};

void loadS(uint32_t v);
void loadD(uint64_t v);
void loadX(uint64_t frac, uint16_t exp);
uint32_t storeS();
uint64_t storeD();
std::tuple<uint64_t, uint16_t> storeX();
int lua_to_mpfr(lua_State *L, int n);

// VMEM[addr].{B/W/L/C/H/I/S/D/X} <- read/write one data
// VMEM[addr]:str(20) <- read string
// VMEM[addr]:write(s) <- write string
static int lua_Vmem_str(lua_State *L) {
    lua_getfield(L, 1, "__addr");
    int addr = lua_tointeger(L, -1);
    int len = luaL_checkinteger(L, 2);
    luaL_Buffer b;
    luaL_buffinitsize(L, &b, len);
    try {
        for(int i = 0; i < len; ++i) {
            char c = ReadB(addr + i);
            luaL_addchar(&b, c);
        }
    } catch(M68kException &e) {
        return luaL_error(L, "CPU exception:%d", int(e.ex_n));
    }
    luaL_pushresultsize(&b, len);
    return 1;
}
static int lua_Vmem_write(lua_State *L) {
    lua_getfield(L, 1, "__addr");
    int addr = lua_tointeger(L, -1);
    size_t len;
    const char *s = lua_tolstring(L, 2, &len);
    try {
        for(size_t i = 0; i < len; ++i) {
            WriteB(addr + i, s[i]);
        }
    } catch(M68kException &e) {
        return luaL_error(L, "CPU exception:%d", int(e.ex_n));
    }
    return 0;
}
static int lua_Vmem_index(lua_State *L) {
    auto fpu = FPU_P();
    lua_getfield(L, 1, "__addr");
    int addr = lua_tointeger(L, -1);
    const char *key = luaL_checkstring(L, 2);

    try {
        if(strcmp(key, "B") == 0) {
            lua_pushinteger(L, ReadB(addr));
        } else if(strcmp(key, "W") == 0) {
            lua_pushinteger(L, ReadW(addr));
        } else if(strcmp(key, "L") == 0) {
            lua_pushinteger(L, ReadL(addr));
        } else if(strcmp(key, "C") == 0) {
            lua_pushinteger(L, static_cast<int8_t>(ReadB(addr)));
        } else if(strcmp(key, "H") == 0) {
            lua_pushinteger(L, static_cast<int16_t>(ReadW(addr)));
        } else if(strcmp(key, "I") == 0) {
            lua_pushinteger(L, static_cast<int32_t>(ReadL(addr)));
        } else if(strcmp(key, "S") == 0) {
            uint32_t value = ReadL(addr);
            fpu->loadS(value);
            if(mpfr_nan_p(fpu->fp_tmp)) {
                lua_pushfstring(L, "nan(%06x)", fpu->fp_tmp_nan >> 41);
            } else {
                lua_pushnumber(L, mpfr_get_flt(fpu->fp_tmp, MPFR_RNDN));
            }
        } else if(strcmp(key, "D") == 0) {
            uint64_t value1 = ReadL(addr);
            uint64_t value2 = ReadL(addr + 4);
            fpu->loadD(value1 << 32 | value2);
            if(mpfr_nan_p(fpu->fp_tmp)) {
                lua_pushfstring(L, "nan(%13x)", fpu->fp_tmp_nan >> 12);
            } else {
                lua_pushnumber(L, mpfr_get_d(fpu->fp_tmp, MPFR_RNDN));
            }
        } else if(strcmp(key, "X") == 0) {
            uint16_t exp = ReadW(addr);
            uint64_t frac1 = ReadL(addr + 4);
            uint64_t frac2 = ReadL(addr + 8);
            fpu->loadX(frac1 << 32 | frac2, exp);
            lua_fp80 *fp =
                static_cast<lua_fp80 *>(lua_newuserdata(L, sizeof(lua_fp80)));
            luaL_setmetatable(L, "M68K.FP80");
            mpfr_init_set(fp->fp, fpu->fp_tmp, MPFR_RNDN);
            fp->nan_v = fpu->fp_tmp_nan;
        }
    } catch(M68kException &e) {
        return luaL_error(L, "CPU exception:%d", int(e.ex_n));
    }
    return 1;
}
static int lua_Vmem_newindex(lua_State *L) {
    auto fpu = FPU_P();
    lua_getfield(L, 1, "__addr");
    int addr = lua_tointeger(L, -1);
    const char *key = luaL_checkstring(L, 2);
    try {
        if(strcmp(key, "B") == 0) {
            WriteB(addr, static_cast<uint8_t>(luaL_checkinteger(L, 3)));
        } else if(strcmp(key, "W") == 0) {
            WriteW(addr, static_cast<uint16_t>(luaL_checkinteger(L, 3)));
        } else if(strcmp(key, "L") == 0) {
            WriteL(addr, static_cast<uint32_t>(luaL_checkinteger(L, 3)));
        } else if(strcmp(key, "C") == 0) {
            WriteB(addr, static_cast<int8_t>(luaL_checkinteger(L, 3)));
        } else if(strcmp(key, "H") == 0) {
            WriteW(addr, static_cast<int16_t>(luaL_checkinteger(L, 3)));
        } else if(strcmp(key, "I") == 0) {
            WriteL(addr, static_cast<int32_t>(luaL_checkinteger(L, 3)));
        } else if(strcmp(key, "S") == 0) {
            lua_to_mpfr(L, 3);
            WriteL(addr, fpu->storeS());
        } else if(strcmp(key, "D") == 0) {
            lua_to_mpfr(L, 3);
            uint64_t result = fpu->storeD();
            WriteL(addr, result >> 32);
            WriteL(addr + 4, result);
        } else if(strcmp(key, "X") == 0) {
            lua_to_mpfr(L, 3);
            auto [frac, exp] = fpu->storeX();
            WriteW(addr, exp);
            WriteL(addr + 4, frac >> 32);
            WriteL(addr + 8, frac);
        }
    } catch(M68kException &e) {
        return luaL_error(L, "CPU exception:%d", int(e.ex_n));
    }
    return 1;
}

static luaL_Reg cpu_vmem_x_meta[] = {
    {"__index", lua_Vmem_index},
    {"__newindex", lua_Vmem_newindex},
    {nullptr, nullptr},
};

static int lua_Vmem(lua_State *L) {
    int addr = luaL_checkinteger(L, 2);
    lua_createtable(L, 0, 1); // 3
    lua_pushinteger(L, addr);
    lua_setfield(L, 3, "__addr");
    lua_pushcfunction(L, lua_Vmem_str);
    lua_setfield(L, 3, "str");
    lua_pushcfunction(L, lua_Vmem_write);
    lua_setfield(L, 3, "write");
    luaL_newlib(LL, cpu_vmem_x_meta);
    lua_setmetatable(LL, 3);
    return 1;
}

static luaL_Reg cpu_vmem_meta[] = {
    {"__index", lua_Vmem},
    {nullptr, nullptr},
};

// PMEM[addr].{B/W/L/C/H/I/S/D/X} <- read/write one data
// PMEM[addr]:str(20) <- read string
// PMEM[addr]:write(s) <- write string
static int lua_Pmem_str(lua_State *L) {
    lua_getfield(L, 1, "__addr");
    int addr = lua_tointeger(L, -1);
    int len = luaL_checkinteger(L, 2);
    luaL_Buffer b;
    luaL_buffinitsize(L, &b, len);
    try {
        for(int i = 0; i < len; ++i) {
            char c = BusReadB(addr + i);
            luaL_addchar(&b, c);
        }
    } catch(M68kException &e) {
        return luaL_error(L, "CPU exception:%d", int(e.ex_n));
    }
    luaL_pushresultsize(&b, len);
    return 1;
}
static int lua_Pmem_write(lua_State *L) {
    lua_getfield(L, 1, "__addr");
    int addr = lua_tointeger(L, -1);
    size_t len;
    const char *s = lua_tolstring(L, 2, &len);
    try {
        for(size_t i = 0; i < len; ++i) {
            BusWriteB(addr + i, s[i]);
        }
    } catch(M68kException &e) {
        return luaL_error(L, "CPU exception:%d", int(e.ex_n));
    }
    return 0;
}
static uint16_t BusReadWAny(uint32_t addr) {
    return BusReadB(addr) << 8 | BusReadB(addr + 1);
}
static uint32_t BusReadLAny(uint32_t addr) {
    return BusReadWAny(addr) << 16 | BusReadWAny(addr + 2);
}
static uint64_t BusReadQAny(uint32_t addr) {
    return (uint64_t)BusReadLAny(addr) << 32 | BusReadLAny(addr + 4);
}
static int lua_Pmem_index(lua_State *L) {
    auto fpu = FPU_P();
    lua_getfield(L, 1, "__addr");
    int addr = lua_tointeger(L, -1);
    const char *key = luaL_checkstring(L, 2);
    try {
        if(strcmp(key, "B") == 0) {
            lua_pushinteger(L, BusReadB(addr));
        } else if(strcmp(key, "W") == 0) {
            lua_pushinteger(L, BusReadWAny(addr));
        } else if(strcmp(key, "L") == 0) {
            lua_pushinteger(L, BusReadLAny(addr));
        } else if(strcmp(key, "C") == 0) {
            lua_pushinteger(L, static_cast<int8_t>(BusReadB(addr)));
        } else if(strcmp(key, "H") == 0) {
            int16_t v = BusReadWAny(addr);
            lua_pushinteger(L, v);
        } else if(strcmp(key, "I") == 0) {
            int32_t v = BusReadLAny(addr);
            lua_pushinteger(L, v);
        } else if(strcmp(key, "S") == 0) {
            uint32_t value = BusReadLAny(addr);
            fpu->loadS(value);
            if(mpfr_nan_p(fpu->fp_tmp)) {
                lua_pushfstring(L, "nan(%06x)", fpu->fp_tmp_nan >> 41);
            } else {
                lua_pushnumber(L, mpfr_get_flt(fpu->fp_tmp, MPFR_RNDN));
            }
        } else if(strcmp(key, "D") == 0) {
            uint64_t value = BusReadQAny(addr);
            fpu->loadD(value);
            if(mpfr_nan_p(fpu->fp_tmp)) {
                lua_pushfstring(L, "nan(%13x)", fpu->fp_tmp_nan >> 12);
            } else {
                lua_pushnumber(L, mpfr_get_d(fpu->fp_tmp, MPFR_RNDN));
            }
        } else if(strcmp(key, "X") == 0) {
            uint16_t exp = BusReadWAny(addr);
            uint64_t frac = BusReadQAny(addr + 4);
            fpu->loadX(frac, exp);
            lua_fp80 *fp =
                static_cast<lua_fp80 *>(lua_newuserdata(L, sizeof(lua_fp80)));
            luaL_setmetatable(L, "M68K.FP80");
            mpfr_init_set(fp->fp, fpu->fp_tmp, MPFR_RNDN);
            fp->nan_v = fpu->fp_tmp_nan;
        }
    } catch(M68kException &e) {
        return luaL_error(L, "CPU exception:%d", int(e.ex_n));
    }
    return 1;
}
static void BusWriteWAny(uint32_t addr, uint16_t v) {
    BusWriteB(addr, v >> 8);
    BusWriteB(addr + 1, v);
}
static void BusWriteLAny(uint32_t addr, uint32_t v) {
    BusWriteWAny(addr, v >> 16);
    BusWriteWAny(addr + 2, v);
}
static void BusWriteQAny(uint32_t addr, uint64_t v) {
    BusWriteLAny(addr, v >> 32);
    BusWriteLAny(addr + 4, v);
}

static int lua_Pmem_newindex(lua_State *L) {
    auto fpu = FPU_P();
    lua_getfield(L, 1, "__addr");
    int addr = lua_tointeger(L, -1);
    const char *key = luaL_checkstring(L, 2);
    try {
        if(strcmp(key, "B") == 0) {
            BusWriteB(addr, static_cast<uint8_t>(luaL_checkinteger(L, 3)));
        } else if(strcmp(key, "W") == 0) {
            BusWriteWAny(addr, static_cast<uint16_t>(luaL_checkinteger(L, 3)));
        } else if(strcmp(key, "L") == 0) {
            BusWriteLAny(addr, static_cast<uint32_t>(luaL_checkinteger(L, 3)));
        } else if(strcmp(key, "C") == 0) {
            BusWriteB(addr, static_cast<int8_t>(luaL_checkinteger(L, 3)));
        } else if(strcmp(key, "H") == 0) {
            BusWriteWAny(addr, static_cast<int16_t>(luaL_checkinteger(L, 3)));
        } else if(strcmp(key, "I") == 0) {
            BusWriteLAny(addr, static_cast<int32_t>(luaL_checkinteger(L, 3)));
        } else if(strcmp(key, "S") == 0) {
            lua_to_mpfr(L, 3);
            BusWriteLAny(addr, fpu->storeS());
        } else if(strcmp(key, "D") == 0) {
            lua_to_mpfr(L, 3);
            uint64_t result = fpu->storeD();
            BusWriteQAny(addr, result);
        } else if(strcmp(key, "X") == 0) {
            lua_to_mpfr(L, 3);
            auto [frac, exp] = fpu->storeX();
            BusWriteWAny(addr, exp);
            BusWriteQAny(addr + 4, frac);
        }
    } catch(M68kException &e) {
        return luaL_error(L, "CPU exception:%d", int(e.ex_n));
    }
    return 1;
}

static luaL_Reg cpu_pmem_x_meta[] = {
    {"__index", lua_Pmem_index},
    {"__newindex", lua_Pmem_newindex},
    {nullptr, nullptr},
};

static int lua_Pmem(lua_State *L) {
    int addr = luaL_checkinteger(L, 2);
    lua_createtable(L, 0, 1); // 3
    lua_pushinteger(L, addr);
    lua_setfield(L, 3, "__addr");
    lua_pushcfunction(L, lua_Pmem_str);
    lua_setfield(L, 3, "str");
    lua_pushcfunction(L, lua_Pmem_write);
    lua_setfield(L, 3, "write");
    luaL_newlib(LL, cpu_pmem_x_meta);
    lua_setmetatable(LL, 3);
    return 1;
}

static luaL_Reg cpu_pmem_meta[] = {
    {"__index", lua_Pmem},
    {nullptr, nullptr},
};
void init_fp(lua_State *L);
int lua_tofp80(lua_State *L);

void init_lua() {
    LL = luaL_newstate();
    luaL_openlibs(LL);
    lua_settop(LL, 0);

    init_fp(LL);
    lua_settop(LL, 0);

    lua_newtable(LL);

    lua_settop(LL, 1);
    lua_createtable(LL, 8, 0);
    for(int i = 0; i < 8; ++i) {
        lua_newtable(LL);
        lua_newtable(LL);
        lua_pushinteger(LL, i);

        luaL_setfuncs(LL, cpu_Dx_meta, 1);
        lua_setmetatable(LL, 3);
        lua_rawseti(LL, 2, i + 1);
    }
    lua_setfield(LL, 1, "D");
    lua_settop(LL, 1);

    lua_newtable(LL);
    luaL_newlib(LL, cpu_Ax_meta);
    lua_setmetatable(LL, 2);
    lua_setfield(LL, 1, "A");

    lua_newtable(LL);
    luaL_newlib(LL, cpu_cc_meta);
    lua_setmetatable(LL, 2);
    lua_setfield(LL, 1, "CC");

    lua_newtable(LL);
    luaL_newlib(LL, cpu_sr_meta);
    lua_setmetatable(LL, 2);
    lua_setfield(LL, 1, "SR");

    lua_newtable(LL);
    luaL_newlib(LL, cpu_ssw_meta);
    lua_setmetatable(LL, 2);
    lua_setfield(LL, 1, "SSW");

    lua_newtable(LL);
    luaL_newlib(LL, cpu_tcr_meta);
    lua_setmetatable(LL, 2);
    lua_setfield(LL, 1, "TCR");

    lua_newtable(LL);
    lua_newtable(LL);
    lua_pushlightuserdata(LL, &cpu.DTTR[0]);
    luaL_setfuncs(LL, cpu_ttr_meta, 1);
    lua_setmetatable(LL, 2);
    lua_setfield(LL, 1, "DTTR0");

    lua_newtable(LL);
    lua_newtable(LL);
    lua_pushlightuserdata(LL, &cpu.DTTR[1]);
    luaL_setfuncs(LL, cpu_ttr_meta, 1);
    lua_setmetatable(LL, 2);
    lua_setfield(LL, 1, "DTTR1");

    lua_newtable(LL);
    lua_newtable(LL);
    lua_pushlightuserdata(LL, &cpu.ITTR[0]);
    luaL_setfuncs(LL, cpu_ttr_meta, 1);
    lua_setmetatable(LL, 2);
    lua_setfield(LL, 1, "ITTR0");

    lua_newtable(LL);
    lua_newtable(LL);
    lua_pushlightuserdata(LL, &cpu.ITTR[1]);
    luaL_setfuncs(LL, cpu_ttr_meta, 1);
    lua_setmetatable(LL, 2);
    lua_setfield(LL, 1, "ITTR1");

    lua_newtable(LL);
    luaL_newlib(LL, cpu_mmusr_meta);
    lua_setmetatable(LL, 2);
    lua_setfield(LL, 1, "MMUSR");

    lua_newtable(LL);
    luaL_newlib(LL, cpu_cacr_meta);
    lua_setmetatable(LL, 2);
    lua_setfield(LL, 1, "CACR");

    luaL_newlib(LL, cpu_meta);
    lua_setmetatable(LL, 1);
    lua_setglobal(LL, "CPU");
    lua_settop(LL, 0);

    lua_newtable(LL);
    lua_newtable(LL);
    luaL_newlib(LL, fpu_fp_meta);
    lua_setmetatable(LL, 2);
    lua_setfield(LL, 1, "FP");

    lua_newtable(LL);
    luaL_newlib(LL, fpu_fpcr_meta);
    lua_setmetatable(LL, 2);
    lua_setfield(LL, 1, "FPCR");

    lua_newtable(LL);
    luaL_newlib(LL, fpu_fpsr_meta);
    lua_setmetatable(LL, 2);
    lua_setfield(LL, 1, "FPSR");

    lua_settop(LL, 1);
    lua_setglobal(LL, "FPU");
    lua_settop(LL, 0);

    lua_newtable(LL);
    luaL_newlib(LL, cpu_vmem_meta);
    lua_setmetatable(LL, 1);
    lua_setglobal(LL, "VMEM");

    lua_newtable(LL);
    luaL_newlib(LL, cpu_pmem_meta);
    lua_setmetatable(LL, 1);
    lua_setglobal(LL, "MEM");
}
