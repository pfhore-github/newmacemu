// alternative Mac ROM
#include "68040.hpp"
#include "proto.hpp"
#include <stdint.h>
#include <unordered_map>
void run_op();
void RESET();

uint8_t ReadBImpl(uint32_t addr, bool code);
uint16_t ReadWImpl(uint32_t addr, bool code);
uint32_t ReadLImpl(uint32_t addr, bool code);
void WriteBImpl(uint32_t addr, uint8_t b);
void WriteWImpl(uint32_t addr, uint16_t w);
void WriteLImpl(uint32_t addr, uint32_t l);

void init_via_rbv();

void call_until(uint32_t addr, uint32_t retaddr) {
    cpu.A[6] = retaddr;
    cpu.PC = addr;
    while(cpu.PC != retaddr) {
        run_op();
    }
}

// $04052
void init_mmu1() {
    // Cache clean

    cpu.DTTR[0].logic_base = 0x80;
    cpu.DTTR[0].logic_mask = 0x7F;
    cpu.DTTR[0].E = true;
    cpu.DTTR[0].S = 2;

    cpu.DTTR[1].logic_base = 0x50;
    cpu.DTTR[1].logic_mask = 0x0F;
    cpu.DTTR[0].E = true;
    cpu.DTTR[0].S = 2;
}
void test_via();
// $03162
bool check_jaws() {
    try {
        cpu.A[2] = ReadL(cpu.A[0] + 8); // VIA1
        cpu.A[2] += 0x1C00;
        cpu.D[2] = 0x100000;
        test_via();
        if(!cpu.Z) {
            return false;
        }
        cpu.D[2] >>= 1;
        test_via();
        if(cpu.Z) {
            return false;
        }
        cpu.D[2] >>= 1;
        test_via();
        if(cpu.Z) {
            return false;
        }
        cpu.A[2] = ReadL(cpu.A[0] + 88);
        ReadBImpl(cpu.A[2], false);
        return true;
    } catch(AccessFault&) {
        return false;
    } 
}

std::unordered_map<uint32_t, bool (*)()> io_checks = {
    {0x3162, check_jaws},
};
// $02F18
void getHwInfo() {
    cpu.D[7] |= 1 << 27;
    cpu.A[5] = cpu.A[7];
    if(cpu.D[2] & 0xff) {
        cpu.A[0] = 0x408031C4; // MODEL LIST BASE
        do {
            cpu.A[0] += 4;
            cpu.D[0] = ReadL(cpu.A[0]);
            if(cpu.D[0]) {
                cpu.A[1] = cpu.A[0] + cpu.D[0];
            }
        } while(ReadW(cpu.A[1] + 18) != cpu.D[2]);
    }
    // $02F52
    cpu.D[0] = cpu.A[6];
    cpu.A[1] = 0x408031AC; // Motherboard Master Offset Table
    do {
        cpu.A[0] = cpu.A[1];
        cpu.A[0] += ReadL(cpu.A[1]);
        cpu.A[1] += 4;
        cpu.A[2] = ReadL(cpu.A[0] - 8); // Check function
    } while(!io_checks[(cpu.A[0] + cpu.A[2]) & 0xffffff]());
}
void test_via();

// $046AA
void test_via() {
    ReadBImpl(cpu.A[2] + cpu.D[2], false);
    uint8_t d1_h = ReadBImpl(cpu.A[2], false);
    int8_t d1_l = -1;
    WriteBImpl(cpu.A[2], d1_l);
    do {
        d1_l = -d1_l;
        WriteBImpl(cpu.A[2], d1_l);
        d1_l += d1_l;
        if(!d1_l) {
            break;
        }
        d1_l = -d1_l;
        if(ReadBImpl(cpu.A[2] + cpu.D[2], false) != d1_l) {
            break;
        }
    } while(ReadBImpl(cpu.A[2], false) == d1_l);
    WriteBImpl(cpu.A[2], 0x7f);
    WriteBImpl(cpu.A[2], d1_h);
}

// $02E00
void init_hw1() {
    cpu.A[7] = 0x2600;
    cpu.A[4] = cpu.A[6];
    cpu.VBR = 0x3DE0;
    do {
        cpu.D[2] = 0;
        getHwInfo();
    } while(!(cpu.D[0] & 1));
    cpu.A[2] = 0; // cpu.PC &~ 0xffff;

    // $03DAE
    cpu.D[3] = ReadL(cpu.A[0]); // ROM offset
    cpu.D[3] -= cpu.A[2];
    cpu.A[0] += cpu.D[3];
    cpu.A[1] += cpu.D[3];

    // $02E38
    cpu.A[4] += cpu.D[3];

    // Jump to REAL ROM address

    // $048C8
    cpu.A[7] += cpu.D[3];
    cpu.D[0] = 0x40803DEC; // ROM offset table
    cpu.VBR = 0x40803DDC;
    do {
        cpu.D[2] = 0;
        getHwInfo();
    } while(!(cpu.D[0] & 1));
    init_via_rbv();

    // $02E46
    if(cpu.D[0] & 1 << 18) {             // OSS?
        cpu.A[3] = ReadL(cpu.A[0] + 72); // OSS Base
        WriteB(cpu.A[3] + 0x204, 13);
    }
    // $02E56
    if(cpu.D[0] & 1 << 19) {             // IIfx EXP0?
        cpu.A[3] = ReadL(cpu.A[0] + 76); // IIfx EXP0 Base
        uint16_t w = 0xF3FF;
        for(int i = 0; i < 16; ++i, w >>= 1) {
            WriteB(cpu.A[3], w);
        }
    }
    // $02E72

    // $04848
    if(cpu.D[0] & 1 << 21) {             // MCU ctrl
        cpu.A[3] = ReadL(cpu.A[0] + 84); // MCU ctrl
        uint32_t l = 0x124F0810;
        cpu.A[2] = ReadL(cpu.A[0] + 44);      // VIA2
        uint8_t s = ReadB(cpu.A[2] + 0x0400); // DIRB
        WriteB(cpu.A[2], s & ~(1 << 5));      // v2TM0A
        if(ReadB(cpu.A[0]) & 1 << 5) {
            l = 0x138B0810;
        }
        cpu.A[2] = cpu.A[3];
        for(int i = 0; i < 32; ++i, l >>= 1) {
            WriteL(cpu.A[2], l);
            cpu.A[2] += 4;
        }

        cpu.D[4] = 0;
        WriteL(cpu.A[2], 0);
        cpu.A[2] += 4;

        WriteL(cpu.A[3] + 0xA0, -1);
        WriteL(cpu.A[3] + 0xA4, -1);
        WriteL(cpu.A[3] + 0xA8, -1);
        WriteL(cpu.A[3] + 0xAC, -1);
        WriteL(cpu.A[3] + 0xB0, -1);
        WriteL(cpu.A[3] + 0xB4, -1);
        cpu.D[4] = ReadL(cpu.A[3] + 0x48);
        if(cpu.D[4] & 1) {
            cpu.D[4] = 0;
        } else {
            cpu.D[4] = 1;
        }
        WriteL(cpu.A[2], cpu.D[4]); // cpu.A[3] + 0x84
        WriteL(cpu.A[3] + 0xB8, -1);
    }
    // $48AC
    if(cpu.D[0] & 1 << 21) {             // PowerBook ctrl
        cpu.A[2] = ReadL(cpu.A[0] + 88); // PowerBook ctrl
        cpu.A[3] = cpu.A[2] + 0x034000;
        cpu.D[3] = ReadB(cpu.A[3]) & 1;
        WriteB(cpu.A[2], cpu.D[3]);
    }
}

// $02E8C
void init_via_rbv() {
    if(cpu.D[0] & 1 << 2) {
        cpu.A[2] = ReadL(cpu.A[0] + 8);               // VIA1 BASE
        cpu.A[3] = cpu.A[1];                          // VIA_DATA1
        WriteB(cpu.A[2] + 0x1E00, ReadB(cpu.A[3]++)); // REGA
        WriteB(cpu.A[2] + 0x0600, ReadB(cpu.A[3]++)); // DIRA
        WriteB(cpu.A[2] + 0x0000, ReadB(cpu.A[3]++)); // REGB
        WriteB(cpu.A[2] + 0x0400, ReadB(cpu.A[3]++)); // DIRB
        WriteB(cpu.A[2] + 0x1800, ReadB(cpu.A[3]++)); // PCR
        WriteB(cpu.A[2] + 0x1600, ReadB(cpu.A[3]++)); // ACR
        WriteB(cpu.A[2] + 0x1C00, 0x7F);              // IER
    }
    if(cpu.D[0] & 1 << 11) {
        cpu.A[2] = ReadL(cpu.A[0] + 44); // VIA2 BASE
        cpu.A[3] = cpu.A[1];             // VIA_DATA1
        cpu.D[3] = ReadB(cpu.A[2] + 0x1E00);
        cpu.D[3] |= ReadB(cpu.A[3]++);
        WriteB(cpu.A[2] + 0x1E00, cpu.D[3]);          // REGA
        WriteB(cpu.A[2] + 0x0600, ReadB(cpu.A[3]++)); // DIRA
        WriteB(cpu.A[2] + 0x0000, ReadB(cpu.A[3]++)); // REGB
        WriteB(cpu.A[2] + 0x0400, ReadB(cpu.A[3]++)); // DIRB
        WriteB(cpu.A[2] + 0x1800, ReadB(cpu.A[3]++)); // PCR
        WriteB(cpu.A[2] + 0x1600, ReadB(cpu.A[3]++)); // ACR
        WriteB(cpu.A[2] + 0x1C00, 0x7F);              // IER
        WriteB(cpu.A[2] + 0x0800, 0x6E);              // TIMER1.L
        WriteB(cpu.A[2] + 0x0A00, 0x19);              // TIMER1.H
    }

    if(cpu.D[0] & 1 << 13) {
    }
    return;
}

// $8AC00
void R_8AC00() {
    if(cpu.D[2] & 1 << 24) { // ?
    }
    // $8ACBC
    return;
}
// $0008C
void boot() {
    SetSR(0x2700);
    cpu.A[6] = 0x0009C;
    init_mmu1();
    RESET();
    init_hw1();
    call_until(0x8AC00, 0x00B4);

    // goto $08E0
}

std::unordered_map<uint32_t, void (*)()> rom_funcs{};
