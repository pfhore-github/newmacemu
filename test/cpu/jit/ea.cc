#define BOOST_TEST_DYN_LINK
#include "68040.hpp"
#include "jit.hpp"
#include "test.hpp"
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
namespace bdata = boost::unit_test::data;
using namespace asmjit;
extern std::unique_ptr<JitRuntime> rt;
void jit_run_test(uint32_t pc);

void jit_prologue(x86::Assembler &a);
void jit_epilogue(x86::Assembler &a);
template <class T, class F> T jit_test_run(const F &e) {
    static CodeHolder code;
    code.reset();
    code.init(rt->environment(), rt->cpuFeatures());

    FuncDetail detail;
    detail.init(FuncSignatureT<T, void>(), rt->environment());

    FuncFrame frame;
    frame.init(detail);
    frame.addAttributes(FuncAttributes::kHasFuncCalls);
    frame.addDirtyRegs(x86::rax, x86::rbx, x86::rcx, x86::rdx, x86::rsi, x86::rbp,
                       x86::rdi, x86::rbx, x86::r9, x86::r10, x86::r11, x86::r12,
                       x86::r13, x86::r14, x86::r15);
    frame.setLocalStackSize(128); // jist in case
    frame.finalize();

    x86::Assembler a(&code);
    // X64 prlogue
    a.emitProlog(frame);
    // save Base
    a.mov(x86::rbx, &cpu);
    e(a);
    a.emitEpilog(frame);
    T (*f)();
    Error err = rt->add(&f, &code);
    BOOST_TEST(!err, DebugUtils::errorAsString(err));
    return f();
}

BOOST_AUTO_TEST_SUITE(Addr)




BOOST_AUTO_TEST_CASE(d_pc_ri_w) {
    cpu.D[3] = -3 & 0xffff;
    cpu.PC = 0x200;
    TEST::SET_W(0x200, 3 << 12 | 0xff);
    jit_test_run<void>([](x86::Assembler &a) { ea_getaddr_jit(a, 7, 3, 0); });
    BOOST_TEST(cpu.EA == 0x200 - 4);
    BOOST_TEST(cpu.PC == 0x202);
}


BOOST_AUTO_TEST_CASE(pc_rn) {
    cpu.D[3] = 4;
    cpu.PC = 0x200;
    TEST::SET_W(0x200, 3 << 12 | 1 << 8 | 1 << 4);
    jit_test_run<void>([](x86::Assembler &a) { ea_getaddr_jit(a, 7, 3, 0); });
    BOOST_TEST(cpu.EA == 0x204);
    BOOST_TEST(cpu.PC == 0x202);
}

BOOST_AUTO_TEST_CASE(bdW_pc_rn) {
    cpu.D[3] = 4;
    cpu.PC = 0x200;
    TEST::SET_W(0x200, 3 << 12 | 1 << 8 | 2 << 4);
    TEST::SET_W(0x202, 0x2000);
    jit_test_run<void>([](x86::Assembler &a) { ea_getaddr_jit(a, 7, 3, 0); });
    BOOST_TEST(cpu.EA == 0x2204);
    BOOST_TEST(cpu.PC == 0x204);
}

BOOST_AUTO_TEST_CASE(bdL_pc_rn) {
    cpu.D[3] = 4;
    cpu.PC = 0x200;
    TEST::SET_W(0x200, 3 << 12 | 1 << 8 | 3 << 4);
    TEST::SET_L(0x202, 0x20000);
    jit_test_run<void>([](x86::Assembler &a) { ea_getaddr_jit(a, 7, 3, 0); });
    BOOST_TEST(cpu.EA == 0x20204);
    BOOST_TEST(cpu.PC == 0x206);
}

BOOST_AUTO_TEST_CASE(bd_rn_without_pc) {
    cpu.D[3] = 4;
    cpu.PC = 0x200;
    TEST::SET_W(0x200, 3 << 12 | 1 << 8 | 2 << 4 | 1 << 7);
    TEST::SET_W(0x202, 0x2000);
    jit_test_run<void>([](x86::Assembler &a) { ea_getaddr_jit(a, 7, 3, 0); });
    BOOST_TEST(cpu.EA == 0x2004);
    BOOST_TEST(cpu.PC == 0x204);
}

BOOST_AUTO_TEST_CASE(bd_pc) {
    cpu.D[3] = 4;
    cpu.PC = 0x200;
    TEST::SET_W(0x200, 3 << 12 | 1 << 8 | 2 << 4 | 1 << 6);
    TEST::SET_W(0x202, 0x2000);
    jit_test_run<void>([](x86::Assembler &a) { ea_getaddr_jit(a, 7, 3, 0); });
    BOOST_TEST(cpu.EA == 0x2200);
    BOOST_TEST(cpu.PC == 0x204);
}

BOOST_AUTO_TEST_CASE(pc_pre_indirect) {
    cpu.D[3] = 16;
    cpu.PC = 0x100;
    TEST::SET_W(0x100, 3 << 12 | 1 << 8 | 2 << 4 | 1);
    TEST::SET_W(0x102, 0x300);
    TEST::SET_L(0x410, 0x500);
    jit_test_run<void>([](x86::Assembler &a) { ea_getaddr_jit(a, 7, 3, 0); });
    BOOST_TEST(cpu.EA == 0x500);
    BOOST_TEST(cpu.PC == 0x104);
}

BOOST_AUTO_TEST_CASE(pc_pre_odW) {
    cpu.D[3] = 16;
    cpu.PC = 0x100;
    TEST::SET_W(0x100, 3 << 12 | 1 << 8 | 2 << 4 | 2);
    TEST::SET_W(0x102, 0x200);
    TEST::SET_W(0x104, 0x300);
    TEST::SET_L(0x310, 0x400);
    jit_test_run<void>([](x86::Assembler &a) { ea_getaddr_jit(a, 7, 3, 0); });
    BOOST_TEST(cpu.EA == 0x700);
    BOOST_TEST(cpu.PC == 0x106);
}


BOOST_AUTO_TEST_CASE(pc_pre_odL) {
    cpu.D[3] = 16;
    cpu.PC = 0x100;
    TEST::SET_W(0x100, 3 << 12 | 1 << 8 | 2 << 4 | 3);
    TEST::SET_W(0x102, 0x200);
    TEST::SET_L(0x104, 0x300);
    TEST::SET_L(0x310, 0x400);
    jit_test_run<void>([](x86::Assembler &a) { ea_getaddr_jit(a, 7, 3, 0); });
    BOOST_TEST(cpu.EA == 0x700);
    BOOST_TEST(cpu.PC == 0x108);
}

BOOST_AUTO_TEST_CASE(pc_post_indirect) {
    cpu.D[3] = 16;
    cpu.PC = 0x100;
    TEST::SET_W(0x100, 3 << 12 | 1 << 8 | 2 << 4 | 1 << 2 | 1);
    TEST::SET_W(0x102, 0x200);
    TEST::SET_L(0x300, 0x400);
    jit_test_run<void>([](x86::Assembler &a) { ea_getaddr_jit(a, 7, 3, 0); });
    BOOST_TEST(cpu.EA == 0x410);
    BOOST_TEST(cpu.PC == 0x104);
}

BOOST_AUTO_TEST_CASE(pc_post_odW) {
    cpu.D[3] = 16;
    cpu.PC = 0x100;
    TEST::SET_W(0x100, 3 << 12 | 1 << 8 | 2 << 4 | 1 << 2| 2);
    TEST::SET_W(0x102, 0x200);
    TEST::SET_W(0x104, 0x400);
    TEST::SET_L(0x300, 0x500);
    jit_test_run<void>([](x86::Assembler &a) { ea_getaddr_jit(a, 7, 3, 0); });
    BOOST_TEST(cpu.EA == 0x910);
    BOOST_TEST(cpu.PC == 0x106);
}

BOOST_AUTO_TEST_CASE(pc_post_odL) {
    cpu.D[3] = 16;
    cpu.PC = 0x100;
    TEST::SET_W(0x100, 3 << 12 | 1 << 8 | 2 << 4 | 1 << 2| 3);
    TEST::SET_W(0x102, 0x200);
    TEST::SET_L(0x104, 0x400);
    TEST::SET_L(0x300, 0x500);
    jit_test_run<void>([](x86::Assembler &a) { ea_getaddr_jit(a, 7, 3, 0); });
    BOOST_TEST(cpu.EA == 0x910);
    BOOST_TEST(cpu.PC == 0x108);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
