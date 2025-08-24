#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <filesystem>
#include <lua.hpp>
#include <math.h>
struct TestEnd {};
static const char *ex_n[] = {"none",
                             "reset",
                             "access fault",
                             "address error",
                             "illegal operation",
                             "div by 0",
                             "chk failure",
                             "trapx",
                             "privilege error",
                             "trace",
                             "aline exception",
                             "fline exception",
                             "unused",
                             "unused",
                             "format error",
                             "undefined irq",
                             "unused",
                             "unused",
                             "unused",
                             "unused",
                             "unused",
                             "unused",
                             "unused",
                             "unused",
                             "unknwon irq",
                             "irq1",
                             "irq2",
                             "irq3",
                             "irq4",
                             "irq5",
                             "irq6",
                             "irq7",
                             "trap0",
                             "trap1",
                             "trap2",
                             "trap3",
                             "trap4",
                             "trap5",
                             "trap6",
                             "trap7",
                             "trap8",
                             "trap9",
                             "trap10",
                             "trap11",
                             "trap12",
                             "trap13",
                             "trap14",
                             "trap15",
                             "unused",
                             "fp unorder",
                             "fp inex",
                             "fp div by 0",
                             "fp underflow",
                             "fp operation error",
                             "fp overflow",
                             "fp signaling nan",
                             "fp unimpleented type",
                             nullptr};
int lua_run_test(lua_State *L) {
    int pc = luaL_checkinteger(L, 1);
    int ex = luaL_checkoption(L, 2, "none", ex_n);
    run_test(pc, EXCEPTION_NUMBER(ex));
    return 0;
}

int lua_test(lua_State *L) {
    lua_Debug d;
    lua_getstack(L, 1, &d);
    lua_getinfo(L, "Snl", &d);
    std::string context = std::format("line={}, function={}", d.currentline,
                                      (d.name ? d.name : "(top)"));
    std::string_view var = luaL_checkstring(L, 1);
    std::string exec = std::format("return {}", var);
    if(luaL_dostring(L, exec.c_str()) == LUA_OK) {
        if(lua_isboolean(L, 2)) {
            bool expected = lua_toboolean(L, 2);
            bool actual = lua_toboolean(L, -1);
            BOOST_TEST(expected == actual, context << ";test: " << var << "="
                                                   << actual << " (should be "
                                                   << expected << ")");
        } else if(lua_isinteger(L, 2)) {
            int expected = lua_tointeger(L, 2);
            int actual = lua_tointeger(L, -1);
            if(var.ends_with("B") || var.ends_with("W") || var.ends_with("L")) {
                BOOST_TEST(expected == actual,
                           context << ";test: " << var << "= 0x" << std::hex
                                   << actual << " (should be 0x" << std::hex
                                   << expected << ")");

            } else {
                BOOST_TEST(expected == actual,
                           context << ";test: " << var << "=" << actual
                                   << "(should be " << expected << ")");
            }
        }
    } else {
        BOOST_TEST_FAIL(lua_tostring(L, -1));
    }
    return 0;
}
void jit_compile(uint32_t begin, uint32_t len);
int lua_jit_compile(lua_State *L) {
    int b = luaL_checkinteger(L, 1);
    int l = luaL_checkinteger(L, 2);
    jit_compile(b, l);
    return 0;
}
extern lua_State *LL;
void init_lua();
void close_lua() { lua_close(LL); }
static int do_test(lua_State *L) {
    const char *name = luaL_checkstring(L, 1);
    BOOST_TEST_CONTEXT(name) {
        if(lua_pcall(L, 0, 0, 0) != LUA_OK) {
            BOOST_FAIL(lua_tostring(L, -1));
        }
    }
    return 0;
}
static int test_reset(lua_State*) {
    Prepare _;
    return 0;
}
struct LuaGlobalFixture {
    LuaGlobalFixture() {
        init_lua();
        lua_register(LL, "JIT_COMPILE", lua_jit_compile);
        lua_newtable(LL);
        lua_pushinteger(LL, TEST_BREAK);
        lua_setfield(LL, 1, "BREAK");
        lua_pushcfunction(LL, lua_test);
        lua_setfield(LL, 1, "CHECK");
        lua_pushcfunction(LL, lua_run_test);
        lua_setfield(LL, 1, "RUN");
        lua_pushcfunction(LL, do_test);
        lua_setfield(LL, 1, "EXEC");
        lua_pushcfunction(LL, test_reset);
        lua_setfield(LL, 1, "RESET");

        lua_setglobal(LL, "TEST");
        luaL_dofile(LL, "../test/lua/lib.lua");
    }
    ~LuaGlobalFixture() { close_lua(); }
};
namespace fs = std::filesystem;
std::vector<fs::path> getAllTests(const char *dir) {
    std::vector<fs::path> ret;
    for(const fs::directory_entry &x : fs::directory_iterator(dir)) {
        if(x.path().extension() == ".lua") {
            ret.push_back(x.path());
        }
    }
    return ret;
}
namespace bdata = boost::unit_test::data;
BOOST_TEST_GLOBAL_FIXTURE(LuaGlobalFixture);
BOOST_AUTO_TEST_SUITE(LUA)

static void run(const std::filesystem::path path) {
    lua_pushnil(LL);
    lua_settop(LL, 0);
    std::string name = path.stem().native();
    if(luaL_dofile(LL, path.native().c_str()) != LUA_OK) {
        BOOST_FAIL(lua_tostring(LL, -1));
    } else {
        if(lua_istable(LL, 1)) {
            lua_getglobal(LL, "run_all");
            lua_pushstring(LL, name.c_str());
            lua_pushvalue(LL, 1);
            try {
                lua_call(LL, 2, 0);
            } catch(TestEnd &) {
                BOOST_FAIL("double fault occured!");
            }
        }
    }
}
BOOST_DATA_TEST_CASE(cpu_, bdata::make(getAllTests("../test/lua/cpu")), path) {
    run(path);
}
BOOST_AUTO_TEST_SUITE_END()
