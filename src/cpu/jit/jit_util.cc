#include "asmjit/asmjit.h"
#include "jit.hpp"
#include <memory>
#include <vector>
using namespace asmjit;
extern std::unique_ptr<JitRuntime> rt;
#ifdef WIN32
static void pushOp(std::vector<uint16_t> &dst, uint16_t offset, uint8_t code,
                   uint16_t info) {
    dst.push_back(dst, offset | (code << 8) | (info << 12));
}
#include <windows.h>
#define UWOP_PUSH_NONVOL 0
#define UWOP_ALLOC_LARGE 1
#define UWOP_ALLOC_SMALL 2
#define UWOP_SET_FPREG 3
#define UWOP_SAVE_NONVOL 4
#define UWOP_SAVE_NONVOL_FAR 5
#define UWOP_SAVE_XMM128 8
#define UWOP_SAVE_XMM128_FAR 9
#define UWOP_PUSH_MACHFRAME 10

#else
extern "C" {
void __register_frame(const void *);
void __deregister_frame(const void *);
}

static void WriteLength(std::vector<uint8_t> &stream, unsigned int pos,
                        unsigned int v) {
    *(uint32_t *)(&stream[pos]) = v;
}

static void WriteUInt64(std::vector<uint8_t> &stream, uint64_t v) {
    for(int i = 0; i < 8; i++)
        stream.push_back((v >> (i * 8)) & 0xff);
}

static void WriteUInt32(std::vector<uint8_t> &stream, uint32_t v) {
    for(int i = 0; i < 4; i++)
        stream.push_back((v >> (i * 8)) & 0xff);
}

static void WriteUInt16(std::vector<uint8_t> &stream, uint16_t v) {
    for(int i = 0; i < 2; i++)
        stream.push_back((v >> (i * 8)) & 0xff);
}

static void WriteUInt8(std::vector<uint8_t> &stream, uint8_t v) {
    stream.push_back(v);
}

static void WriteULEB128(std::vector<uint8_t> &stream, uint32_t v) {
    while(true) {
        if(v < 128) {
            WriteUInt8(stream, v);
            break;
        } else {
            WriteUInt8(stream, (v & 0x7f) | 0x80);
            v >>= 7;
        }
    }
}

static void WriteSLEB128(std::vector<uint8_t> &stream, int32_t v) {
    if(v >= 0) {
        WriteULEB128(stream, v);
    } else {
        while(true) {
            if(v > -128) {
                WriteUInt8(stream, v & 0x7f);
                break;
            } else {
                WriteUInt8(stream, v);
                v >>= 7;
            }
        }
    }
}

static void WritePadding(std::vector<uint8_t> &stream) {
    int padding = stream.size() % 8;
    if(padding != 0) {
        padding = 8 - padding;
        for(int i = 0; i < padding; i++)
            WriteUInt8(stream, 0);
    }
}

static void WriteCIE(std::vector<uint8_t> &stream,
                     const std::vector<uint8_t> &cieInstructions,
                     uint8_t returnAddressReg) {
    unsigned int lengthPos = stream.size();
    WriteUInt32(stream, 0); // Length
    WriteUInt32(stream, 0); // CIE ID

    WriteUInt8(stream, 1); // CIE Version
    WriteUInt8(stream, 'z');
    WriteUInt8(stream, 'R'); // fde encoding
    WriteUInt8(stream, 0);
    WriteULEB128(stream, 1);
    WriteSLEB128(stream, -1);
    WriteULEB128(stream, returnAddressReg);

    WriteULEB128(stream, 1); // LEB128 augmentation size
    WriteUInt8(stream, 0);   // DW_EH_PE_absptr (FDE uses absolute pointers)

    for(unsigned int i = 0; i < cieInstructions.size(); i++)
        stream.push_back(cieInstructions[i]);

    WritePadding(stream);
    WriteLength(stream, lengthPos, stream.size() - lengthPos - 4);
}

static void WriteFDE(std::vector<uint8_t> &stream,
                     const std::vector<uint8_t> &fdeInstructions,
                     uint32_t cieLocation, void *begin, size_t sz) {
    unsigned int lengthPos = stream.size();
    WriteUInt32(stream, 0); // Length
    uint32_t offsetToCIE = stream.size() - cieLocation;
    WriteUInt32(stream, offsetToCIE);

    WriteUInt64(stream, (intptr_t)begin); // func start
    WriteUInt64(stream, sz);              // func size

    WriteULEB128(stream, 0); // LEB128 augmentation size

    for(unsigned int i = 0; i < fdeInstructions.size(); i++)
        stream.push_back(fdeInstructions[i]);

    WritePadding(stream);
    WriteLength(stream, lengthPos, stream.size() - lengthPos - 4);
}

static void WriteAdvanceLoc(std::vector<uint8_t> &fdeInstructions,
                            uint64_t offset, uint64_t &lastOffset) {
    uint64_t delta = offset - lastOffset;
    if(delta < (1 << 6)) {
        WriteUInt8(fdeInstructions, (1 << 6) | delta); // DW_CFA_advance_loc
    } else if(delta < (1 << 8)) {
        WriteUInt8(fdeInstructions, 2); // DW_CFA_advance_loc1
        WriteUInt8(fdeInstructions, delta);
    } else if(delta < (1 << 16)) {
        WriteUInt8(fdeInstructions, 3); // DW_CFA_advance_loc2
        WriteUInt16(fdeInstructions, delta);
    } else {
        WriteUInt8(fdeInstructions, 4); // DW_CFA_advance_loc3
        WriteUInt32(fdeInstructions, delta);
    }
    lastOffset = offset;
}

static void WriteDefineCFA(std::vector<uint8_t> &cieInstructions,
                           int dwarfRegId, int stackOffset) {
    WriteUInt8(cieInstructions, 0x0c); // DW_CFA_def_cfa
    WriteULEB128(cieInstructions, dwarfRegId);
    WriteULEB128(cieInstructions, stackOffset);
}

static void WriteDefineStackOffset(std::vector<uint8_t> &fdeInstructions,
                                   int stackOffset) {
    WriteUInt8(fdeInstructions, 0x0e); // DW_CFA_def_cfa_offset
    WriteULEB128(fdeInstructions, stackOffset);
}

static void WriteRegisterStackLocation(std::vector<uint8_t> &instructions,
                                       int dwarfRegId, int stackLocation) {
    WriteUInt8(instructions, (2 << 6) | dwarfRegId); // DW_CFA_offset
    WriteULEB128(instructions, stackLocation);
}

static inline uint32_t getXmmMovInst(const FuncFrame &frame) {
    bool avx = frame.isAvxEnabled();
    bool aligned = frame.hasAlignedVecSR();

    return aligned ? (avx ? x86::Inst::kIdVmovaps : x86::Inst::kIdMovaps)
                   : (avx ? x86::Inst::kIdVmovups : x86::Inst::kIdMovups);
}
static inline void X86Internal_setupSaveRestoreInfo(RegGroup group,
                                                    const FuncFrame &frame,
                                                    x86::Reg &xReg,
                                                    uint32_t &xInst,
                                                    uint32_t &xSize) noexcept {
    switch(group) {
    case RegGroup::kVec:
        xReg = x86::xmm(0);
        xInst = getXmmMovInst(frame);
        xSize = xReg.size();
        break;
    case RegGroup::kX86_K:
        xReg = x86::k(0);
        xInst = x86::Inst::kIdKmovq;
        xSize = xReg.size();
        break;
    case RegGroup::kX86_MM:
        xReg = x86::mm(0);
        xInst = x86::Inst::kIdMovq;
        xSize = xReg.size();
        break;
    default:
        break;
    }
}
#endif
static std::vector<uint8_t> CreateUnwindInfo(const FuncFrame &frame,
                                             void *begin, size_t sz) {
    // We need a dummy emitter for instruction size calculations
    CodeHolder code;
    code.init(rt->environment(), rt->cpuFeatures());

    x86::Assembler assembler(&code);
    auto emitter = &assembler;

    // Note: the following code must match exactly what X86Internal::emitProlog
    // does
    uint32_t gpSaved = frame.savedRegs(RegGroup::kGp);

    x86::Gp zsp = emitter->zsp();   // ESP|RSP register.
    x86::Gp zbp = emitter->zbp();   // EBP|RBP register.
    x86::Gp gpReg = emitter->zsp(); // General purpose register (temporary).
    x86::Gp saReg = emitter->zsp(); // Stack-arguments base register.

#ifdef WIN32
#define UWOP_PUSH_NONVOL 0
#define UWOP_ALLOC_LARGE 1
#define UWOP_ALLOC_SMALL 2
#define UWOP_SET_FPREG 3
#define UWOP_SAVE_NONVOL 4
#define UWOP_SAVE_NONVOL_FAR 5
#define UWOP_SAVE_XMM128 8
#define UWOP_SAVE_XMM128_FAR 9
#define UWOP_PUSH_MACHFRAME 10
    std::vector<uint16_t> codes;
#else
    // Build .eh_frame:
    //
    // The documentation for this can be found in the DWARF standard
    // The x64 specific details are described in "System V Application Binary
    // Interface AMD64 Architecture Processor Supplement"
    //
    // See appendix D.6 "Call Frame Information Example" in the DWARF 5 spec.
    //
    // The CFI_Parser<A>::decodeFDE parser on the other side..
    // https://github.com/llvm-mirror/libunwind/blob/master/src/DwarfParser.hpp

    // Asmjit -> DWARF register id
    int dwarfRegId[16];
    dwarfRegId[x86::Gp::kIdAx] = 0;
    dwarfRegId[x86::Gp::kIdDx] = 1;
    dwarfRegId[x86::Gp::kIdCx] = 2;
    dwarfRegId[x86::Gp::kIdBx] = 3;
    dwarfRegId[x86::Gp::kIdSi] = 4;
    dwarfRegId[x86::Gp::kIdDi] = 5;
    dwarfRegId[x86::Gp::kIdBp] = 6;
    dwarfRegId[x86::Gp::kIdSp] = 7;
    dwarfRegId[x86::Gp::kIdR8] = 8;
    dwarfRegId[x86::Gp::kIdR9] = 9;
    dwarfRegId[x86::Gp::kIdR10] = 10;
    dwarfRegId[x86::Gp::kIdR11] = 11;
    dwarfRegId[x86::Gp::kIdR12] = 12;
    dwarfRegId[x86::Gp::kIdR13] = 13;
    dwarfRegId[x86::Gp::kIdR14] = 14;
    dwarfRegId[x86::Gp::kIdR15] = 15;
    int dwarfRegRAId = 16;
    int dwarfRegXmmId = 17;

    std::vector<uint8_t> cieInstructions;
    std::vector<uint8_t> fdeInstructions;

    uint8_t returnAddressReg = dwarfRegRAId;
    int stackOffset =
        8; // Offset from RSP to the Canonical Frame Address (CFA) - stack
           // position where the CALL return address is stored

    WriteDefineCFA(cieInstructions, dwarfRegId[x86::Gp::kIdSp], stackOffset);
    WriteRegisterStackLocation(cieInstructions, returnAddressReg, stackOffset);
    uint64_t lastOffset = 0;
#endif

    // Emit: 'push zbp'
    //       'mov  zbp, zsp'.
    if(frame.hasPreservedFP()) {
        gpSaved &= ~Support::bitMask(x86::Gp::kIdBp);
        emitter->push(zbp);

#ifdef WIN32
        pushOp(codes, assembler.offset(), UWOP_PUSH_NONVOL, x86::Gp::kIdBp);
#else
        stackOffset += 8;
        WriteAdvanceLoc(fdeInstructions, assembler.offset(), lastOffset);
        WriteDefineStackOffset(fdeInstructions, stackOffset);
        WriteRegisterStackLocation(fdeInstructions, dwarfRegId[x86::Gp::kIdBp],
                                   stackOffset);
#endif
        emitter->mov(zbp, zsp);
    }

    {
        Support::BitWordIterator<RegMask> it(gpSaved);
        while(it.hasNext()) {
            int regId = it.next();
            gpReg.setId(regId);
            emitter->push(gpReg);
#ifdef WIN32
            pushOp(codes, assembler.offset(), UWOP_PUSH_NONVOL, regId);
#else
            stackOffset += 8;
            WriteAdvanceLoc(fdeInstructions, assembler.offset(), lastOffset);
            WriteDefineStackOffset(fdeInstructions, stackOffset);
            WriteRegisterStackLocation(fdeInstructions, dwarfRegId[regId],
                                       stackOffset);
#endif
        }
    }

    // Emit: 'mov saReg, zsp'.
    uint32_t saRegId = frame.saRegId();
    if(saRegId != BaseReg::kIdBad && saRegId != x86::Gp::kIdSp) {
        saReg.setId(saRegId);
        if(frame.hasPreservedFP()) {
            if(saRegId != x86::Gp::kIdBp)
                emitter->mov(saReg, zbp);
        } else {
            emitter->mov(saReg, zsp);
        }
    }

    if(frame.hasDynamicAlignment()) {
        // Emit: 'and zsp, StackAlignment'.
        emitter->and_(zsp, -static_cast<int32_t>(frame.finalStackAlignment()));
    }

    if(frame.hasStackAdjustment()) {
        // Emit: 'sub zsp, StackAdjustment'.
        auto stackadjust = frame.stackAdjustment();
        emitter->sub(zsp, stackadjust);
#if WIN32
        if(stackadjust <= 128) {
            pushOp(codes, assembler.offset(), UWOP_ALLOC_SMALL,
                   stackadjust / 8 - 1);
        } else if(stackadjust <= 512 * 1024 - 8) {
            codes.push_back(stackadjust / 8);
            pushOp(codes, assembler.offset(), UWOP_ALLOC_LARGE, 0);
        } else {
            codes.push_back((uint16_t)(stackadjust >> 16));
            codes.push_back((uint16_t)stackadjust);
            pushOp(codes, assembler.offset(), UWOP_ALLOC_LARGE, 1);
        }
#else
        stackOffset += stackadjust;
        WriteAdvanceLoc(fdeInstructions, assembler.offset(), lastOffset);
        WriteDefineStackOffset(fdeInstructions, stackOffset);
#endif
    }

    if(frame.hasDynamicAlignment() && frame.hasDAOffset()) {
        // Emit: 'mov [zsp + dsaSlot], saReg'.
        x86::Mem saMem = x86::ptr(zsp, int32_t(frame.daOffset()));
        emitter->mov(saMem, saReg);
    }

    // Emit 'movxxx [zsp + X], {[x|y|z]mm, k}'.
    {
        x86::Reg xReg;
        int xOffset = frame.extraRegSaveOffset();
        x86::Mem xBase = x86::ptr(zsp, int32_t(xOffset));

        uint32_t xInst;
        uint32_t xSize;

        for(RegGroup group :
            Support::EnumValues<RegGroup, RegGroup(1), RegGroup::kMaxVirt>{}) {
            Support::BitWordIterator<RegMask> it(frame.savedRegs(group));
            if(it.hasNext()) {
                X86Internal_setupSaveRestoreInfo(group, frame, xReg, xInst,
                                                 xSize);
                do {
                    int regId = it.next();
                    xReg.setId(regId);
                    emitter->emit(xInst, xBase, xReg);
                    xBase.addOffsetLo32(int32_t(xSize));
#if WIN32
                    if(xBase.offsetLo32() / xSize < (1 << 16)) {
                        codes.push_back(xBase.offsetLo32() / vecSize);
                        pushOp(codes, assembler.offset(), UWOP_SAVE_XMM128,
                               regId);
                    } else {
                        codes.push_back((uint16_t)(xBase.offsetLo32() >> 16));
                        codes.push_back((uint16_t)xBase.offsetLo32());
                        pushOp(codes, assembler.offset(), UWOP_SAVE_XMM128_FAR,
                               regId);
                    }
#else
                    WriteAdvanceLoc(fdeInstructions, assembler.offset(),
                                    lastOffset);
                    WriteRegisterStackLocation(fdeInstructions,
                                               dwarfRegXmmId + regId,
                                               stackOffset - xOffset);
#endif
                    xOffset += static_cast<int32_t>(xSize);

                } while(it.hasNext());
            }
        }
    }
#if WIN32
    // Build the UNWIND_INFO structure:

    uint16_t version = 1, flags = 0, frameRegister = 0, frameOffset = 0;
    uint16_t sizeOfProlog = (uint16_t)assembler.getOffset();
    uint16_t countOfCodes = (uint16_t)codes.Size();

    std::vector<uint16_t> info;
    info.Push(version | (flags << 3) | (sizeOfProlog << 8));
    info.Push(countOfCodes | (frameRegister << 8) | (frameOffset << 12));

    for(unsigned int i = codes.Size(); i > 0; i--)
        info.Push(codes[i - 1]);

    if(codes.Size() % 2 == 1)
        info.Push(0);
    std::vector<uint8_t> ret(info.size() * 2);
    memcpy(ret.data(), info.data(), info.size() * 2);
    return ret;
#else
    std::vector<uint8_t> stream;
    WriteCIE(stream, cieInstructions, returnAddressReg);
    WriteFDE(stream, fdeInstructions, 0, begin, sz);
    WriteUInt32(stream, 0);
    return stream;
#endif
}
using Func = void (*)(uint32_t addr);
void AddJitFunction(jit_cache *jit, FuncFrame &frame, Func fn, size_t fn_size) {
    jit->unwindInfo = CreateUnwindInfo(frame, (void *)fn, fn_size);
#if WIN32
    jit->table.BeginAddress = (intptr_t)fn;
    jit->table.EndAddress = (intptr_t)((uint8_t *)(fn) + fn_size);
#ifndef __MINGW64__
    jit->table.UnwindInfoAddress = (intprt_t)jit->unwindInfo.data();
#else
    jit->table.UnwindData = (intprt_t)jit->unwindInfo.data();
#endif
    RtlAddFunctionTable(table, 1, 0);
#else
    if(jit->unwindInfo.size() > 0) {

#ifdef __APPLE__
        // On macOS __register_frame takes a single FDE as an argument
        uint8_t *entry = unwindInfo.data();
        while(true) {
            uint32_t length = *((uint32_t *)entry);
            if(length == 0)
                break;

            if(length == 0xffffffff) {
                uint64_t length64 = *((uint64_t *)(entry + 4));
                if(length64 == 0)
                    break;

                uint64_t offset = *((uint64_t *)(entry + 12));
                if(offset != 0) {
                    __register_frame(entry);
                }
                entry += length64 + 12;
            } else {
                uint32_t offset = *((uint32_t *)(entry + 4));
                if(offset != 0) {
                    __register_frame(entry);
                }
                entry += length + 4;
            }
        }
#else
        // On Linux it takes a pointer to the entire .eh_frame
        __register_frame(jit->unwindInfo.data());
    }
#endif
#endif

    return;
}

void deregister_frame(struct jit_cache *p) {
#if WIN32
    RtlDeleteFunctionTable(&p->table);
#else
        __deregister_frame(p->unwindInfo.data());
#endif
}