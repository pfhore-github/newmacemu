// emulated FP80 Lua wrapper
#include "68040.hpp"
#include "68040fpu.hpp"
#include "mpfr.h"
#include <lua.hpp>
#include <stdlib.h>
#include <string.h>
#include <variant>
void normalize_fp(FPU_PREC p);
struct lua_fp80 {
    mpfr_t fp;
    uint64_t nan_v;
};
// FP80
int lua_tofp80(lua_State *L) {
    auto *ret =
        static_cast<lua_fp80 *>(lua_newuserdatauv(L, sizeof(lua_fp80), 0));
    luaL_setmetatable(L, "M68K.FP80");
    switch(lua_type(L, 1)) {
    case LUA_TNUMBER:
        if(lua_isinteger(L, 1)) {
            mpfr_init_set_si(ret->fp, lua_tointeger(L, 1), MPFR_RNDN);
        } else {
            mpfr_init_set_d(ret->fp, lua_tonumber(L, 1), MPFR_RNDN);
        }
        break;
    case LUA_TSTRING: {
        size_t sz;
        const char *str = lua_tolstring(L, 1, &sz);
        mpfr_init_set_str(ret->fp, str, 0, MPFR_RNDN);
        break;
    }

    default:
        luaL_argerror(L, 1, "invalid type");
    }
    return 1;
}

int lua_tofp80_nan(lua_State *L) {
    auto *ret =
        static_cast<lua_fp80 *>(lua_newuserdatauv(L, sizeof(lua_fp80), 0));
    luaL_setmetatable(L, "M68K.FP80");
    mpfr_set_nan(ret->fp);
    if( lua_isnoneornil(L, 1)) {
        // signaling nan
        ret->nan_v = 0xC000000000000000ULL;
    } else if( lua_isinteger(L, 1)) {
        ret->nan_v = lua_tointeger(L, 1);
    } else {
        return luaL_argerror(L, 1, "invalid type");
    }
    return 1;
}
int lua_to_mpfr(lua_State *L, int n) {
    if(lua_isnumber(L, n)) {
        double value = lua_tonumber(L, 3);
        mpfr_set_d(FPU_P()->fp_tmp, value, MPFR_RNDN);
        return 0;
    } else if(auto fp =
                  static_cast<lua_fp80 *>(luaL_testudata(L, n, "M68K.FP80"))) {
        if(mpfr_nan_p(fp->fp)) {
            mpfr_set_nan(FPU_P()->fp_tmp);
            FPU_P()->fp_tmp_nan = fp->nan_v;
        } else {
            mpfr_set(FPU_P()->fp_tmp, fp->fp, MPFR_RNDN);
        }
        return 0;
    } else {
        return luaL_argerror(L, n, "invalid value");
    }
}
static int fp_tostring(lua_State *L) {
    lua_fp80 *fp = static_cast<lua_fp80 *>(luaL_checkudata(L, 1, "M68K.FP80"));
    if(mpfr_nan_p(fp->fp)) {
        std::string err = std::format("nan(0x{:016x})", fp->nan_v);
        lua_pushfstring(L, "%s", err.c_str());
    } else {
        char *ptr;
        mpfr_asprintf(&ptr, "%RNf", fp->fp);
        lua_pushstring(L, ptr);
        mpfr_free_str(ptr);
    }
    return 1;
}

static int fp_tonumber(lua_State *L) {
    mpfr_t *fp = static_cast<mpfr_t *>(luaL_checkudata(L, 1, "M68K.FP80"));
    lua_pushnumber(L, mpfr_get_d(*fp, MPFR_RNDN));
    return 1;
}

static int fp_getExp(lua_State *L) {
    mpfr_t *fp = static_cast<mpfr_t *>(luaL_checkudata(L, 1, "M68K.FP80"));
    lua_pushinteger(L, (*fp)->_mpfr_exp);
    return 1;
}

static int fp_gc(lua_State *L) {
    mpfr_t *fp = static_cast<mpfr_t *>(luaL_checkudata(L, 1, "M68K.FP80"));
    mpfr_clear(*fp);
    return 0;
}

static void get_mpfr_v(mpfr_t dst, lua_State *L, int n) {
    if(auto fp = static_cast<mpfr_t *>(luaL_testudata(L, n, "M68K.FP80"))) {
        mpfr_set(dst, *fp, MPFR_RNDN);
    } else if(lua_isinteger(L, n)) {
        mpfr_set_si(dst, lua_tointeger(L, n), MPFR_RNDN);
    } else if(lua_isnumber(L, n)) {
        mpfr_set_d(dst, lua_tonumber(L, n), MPFR_RNDN);
    } else if(lua_isstring(L, n)) {
        mpfr_set_nan(dst);
    } else {
        luaL_argerror(L, n, "invalid argument");
    }
}
static int fp_add(lua_State *L) {
    MPFR_DECL_INIT(fp1, 64);
    get_mpfr_v(fp1, L, 1);
    MPFR_DECL_INIT(fp2, 64);
    get_mpfr_v(fp2, L, 2);
    mpfr_t *ret = static_cast<mpfr_t *>(lua_newuserdata(L, sizeof(mpfr_t)));
    luaL_setmetatable(L, "M68K.FP80");
    mpfr_init2(*ret, 64);
    mpfr_add(*ret, fp1, fp2, MPFR_RNDN);
    return 1;
}

static int fp_sub(lua_State *L) {
    MPFR_DECL_INIT(fp1, 64);
    get_mpfr_v(fp1, L, 1);
    MPFR_DECL_INIT(fp2, 64);
    get_mpfr_v(fp2, L, 2);
    mpfr_t *ret = static_cast<mpfr_t *>(lua_newuserdata(L, sizeof(mpfr_t)));
    luaL_setmetatable(L, "M68K.FP80");
    mpfr_init2(*ret, 64);
    mpfr_sub(*ret, fp1, fp2, MPFR_RNDN);
    return 1;
}

static int fp_mul(lua_State *L) {
    MPFR_DECL_INIT(fp1, 64);
    get_mpfr_v(fp1, L, 1);
    MPFR_DECL_INIT(fp2, 64);
    get_mpfr_v(fp2, L, 2);
    mpfr_t *ret = static_cast<mpfr_t *>(lua_newuserdata(L, sizeof(mpfr_t)));
    luaL_setmetatable(L, "M68K.FP80");
    mpfr_init2(*ret, 64);
    mpfr_mul(*ret, fp1, fp2, MPFR_RNDN);
    return 1;
}

static int fp_div(lua_State *L) {
    MPFR_DECL_INIT(fp1, 64);
    get_mpfr_v(fp1, L, 1);
    MPFR_DECL_INIT(fp2, 64);
    get_mpfr_v(fp2, L, 2);
    mpfr_t *ret = static_cast<mpfr_t *>(lua_newuserdata(L, sizeof(mpfr_t)));
    luaL_setmetatable(L, "M68K.FP80");
    mpfr_init2(*ret, 64);
    mpfr_div(*ret, fp1, fp2, MPFR_RNDN);
    return 1;
}

static int fp_pow(lua_State *L) {
    MPFR_DECL_INIT(fp1, 64);
    get_mpfr_v(fp1, L, 1);
    MPFR_DECL_INIT(fp2, 64);
    get_mpfr_v(fp2, L, 2);
    mpfr_t *ret = static_cast<mpfr_t *>(lua_newuserdata(L, sizeof(mpfr_t)));
    luaL_setmetatable(L, "M68K.FP80");
    mpfr_init2(*ret, 64);
    mpfr_pow(*ret, fp1, fp2, MPFR_RNDN);
    return 1;
}

static int fp_neg(lua_State *L) {
    mpfr_t *ret = static_cast<mpfr_t *>(lua_newuserdata(L, sizeof(mpfr_t)));
    luaL_setmetatable(L, "M68K.FP80");
    mpfr_init2(*ret, 64);

    mpfr_t *fp1 = static_cast<mpfr_t *>(luaL_checkudata(L, 1, "M68K.FP80"));
    mpfr_neg(*ret, *fp1, MPFR_RNDN);
    return 1;
}

static int fp_eq(lua_State *L) {
    mpfr_t *ret = static_cast<mpfr_t *>(lua_newuserdata(L, sizeof(mpfr_t)));
    luaL_setmetatable(L, "M68K.FP80");
    mpfr_init2(*ret, 64);

    mpfr_t *fp1 = static_cast<mpfr_t *>(luaL_checkudata(L, 1, "M68K.FP80"));
    mpfr_t *fp2 = static_cast<mpfr_t *>(luaL_checkudata(L, 1, "M68K.FP80"));
    lua_pushboolean(L, mpfr_equal_p(*fp1, *fp2));
    return 1;
}

static int fp_lt(lua_State *L) {
    MPFR_DECL_INIT(fp1, 64);
    get_mpfr_v(fp1, L, 1);
    MPFR_DECL_INIT(fp2, 64);
    get_mpfr_v(fp2, L, 2);
    lua_pushboolean(L, mpfr_less_p(fp1, fp2));
    return 1;
}

static int fp_le(lua_State *L) {
    MPFR_DECL_INIT(fp1, 64);
    get_mpfr_v(fp1, L, 1);
    MPFR_DECL_INIT(fp2, 64);
    get_mpfr_v(fp2, L, 2);
    lua_pushboolean(L, mpfr_lessequal_p(fp1, fp2));
    return 1;
}
luaL_Reg l_fp_meta[] = {
    {"__add", fp_add}, {"__sub", fp_sub},
    {"__mul", fp_mul}, {"__div", fp_div},
    {"__unm", fp_neg}, {"__pow", fp_pow},
    {"__eq", fp_eq},   {"__lt", fp_lt},
    {"__le", fp_le},   {"__tostring", fp_tostring},
    {"__gc", fp_gc},   {nullptr, nullptr},
};
luaL_Reg l_fp_call[] = {
    {"toNumber", fp_tonumber},
    {"getExp", fp_getExp},
    {nullptr, nullptr},
};

void init_fp(lua_State *L) {
    luaL_newmetatable(L, "M68K.FP80");
    lua_newtable(L);
    luaL_setfuncs(L, l_fp_call, 0);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, l_fp_meta, 0);

    lua_pushcfunction(L, lua_tofp80);
    lua_setglobal(L, "FP80");

    lua_pushcfunction(L, lua_tofp80_nan);
    lua_setglobal(L, "FP80_nan");
}