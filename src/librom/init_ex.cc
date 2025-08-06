#include "68040.hpp"
#include "SDL_timer.h"
#include "chip/iop.hpp"
#include "chip/scc.hpp"
#include "chip/via.hpp"
#include "inline.hpp"
#include "rom_common.hpp"
#include "rom_proto.hpp"
#include <expected>
#include <stdint.h>
#include <unordered_set>
namespace OP {
void cinva_bc(uint16_t);
void cinva_ic(uint16_t);
} // namespace OP
void Set_TTR_t(Cpu::TTR_t &x, uint32_t v);
namespace LibROM {
extern const AddrMap *gAddrMap;
extern const UniversalInfo *gUnivInfo;
extern uint32_t gAddrMapFlags;
extern uint32_t gUnivROMFlags;
extern uint32_t gHWCfgFlags;
void ex_busErr() {
    if(cpu.D[7] & 1 << 27) {
        throw TestError{};
    }
    cpu.D[7] |= 0x100;
    return ex_common();
}
void ex_addressErr() {
    cpu.D[7] |= 0x200;
    return ex_common();
}

void ex_illInstErr() {
    cpu.D[7] |= 0x300;
    return ex_common();
}

void ex_zeroDivErr() {
    cpu.D[7] |= 0x400;
    return ex_common();
}

void ex_chkErr() {
    cpu.D[7] |= 0x500;
    return ex_common();
}

void ex_trapErr() {
    cpu.D[7] |= 0x600;
    return ex_common();
}

void ex_privErr() {
    cpu.D[7] |= 0x700;
    return ex_common();
}

void ex_traceErr() {
    cpu.D[7] |= 0x800;
    return ex_common();
}

void ex_lineAErr() {
    cpu.D[7] |= 0x900;
    return ex_common();
}

void ex_lineFErr() {
    cpu.D[7] |= 0xA00;
    return ex_common();
}

void ex_miscErr() {
    cpu.D[7] |= 0xB00;
    return ex_common();
}

void ex_coprocessorErr() {
    cpu.D[7] |= 0xC00;
    return ex_common();
}

void ex_formatErr() {
    cpu.D[7] |= 0xD00;
    return ex_common();
}

void ex_irqErr() {
    cpu.D[7] |= 0xE00;
    return ex_common();
}

void ex_vtrapErr() {
    cpu.D[7] |= 0xF00;
    return ex_common();
}

void ex_irq1() {
    cpu.D[7] |= 0x1000;
    return ex_common();
}

void ex_irq2() {
    cpu.D[7] |= 0x1100;
    return ex_common();
}

void ex_irq3() {
    cpu.D[7] |= 0x1200;
    return ex_common();
}

void ex_irq4() {
    cpu.D[7] |= 0x1300;
    return ex_common();
}

void ex_irq5() {
    cpu.D[7] |= 0x1400;
    return ex_common();
}

void ex_common() { return ex_common2(); }

void ex_irq6() {
    cpu.D[7] |= 0x1500;
    // power off
    via2->writePB(2, false);
    abort();
}

void ex_irq7() {
    if(cpu.D[7] & 1 << 27) {
        if(cpu.D[0] == FOURCC("PGCp")) {
            // IIci PGC ERROR
            if(via1->readPB(7)) {
                return;
            }
        } else if(cpu.D[0] == FOURCC("RPUp")) {
            // handle IIfx/PGC
            if(ReadW(gAddrMap->addr[ADDR_OSS] + 0x202) & 1 << 14) {
                auto exp1 = gAddrMap->addr[ADDR_EXP1];
                WriteB(exp1 + 0x10, 0xff);
                WriteL(exp1, ReadL(exp1) | 1 << 16);
                return;
            }
        }
    }
    cpu.D[7] &= ~0xff00;
    cpu.D[7] |= 0x1600;
    cpu.D[7] |= 1 << 25;
    return ex_common2();
}

void ex_fpBsunErr() {
    cpu.D[7] &= ~0xff00;
    cpu.D[7] |= ~0x1700;
    return ex_common2();
}

void ex_fpInexErr() {
    cpu.D[7] &= ~0xff00;
    cpu.D[7] |= ~0x1800;
    return ex_common2();
}

void ex_fpDiv0Err() {
    cpu.D[7] &= ~0xff00;
    cpu.D[7] |= ~0x1900;
    return ex_common2();
}

void ex_fpUnflErr() {
    cpu.D[7] &= ~0xff00;
    cpu.D[7] |= ~0x1A00;
    return ex_common2();
}

void ex_fpOpErErr() {
    cpu.D[7] &= ~0xff00;
    cpu.D[7] |= ~0x1B00;
    return ex_common2();
}

void ex_fpOvflErr() {
    cpu.D[7] &= ~0xff00;
    cpu.D[7] |= ~0x1C00;
    return ex_common2();
}

void ex_fpSnanErr() {
    cpu.D[7] &= ~0xff00;
    cpu.D[7] |= ~0x1D00;
    return ex_common2();
}

void ex_fp56Err() {
    cpu.D[7] &= ~0xff00;
    cpu.D[7] |= ~0x1E00;
    return ex_common2();
}

void ex_fp57Err() {
    cpu.D[7] &= ~0xff00;
    cpu.D[7] |= ~0x1F00;
    return ex_common2();
}

void ex_fp58Err() {
    cpu.D[7] &= ~0xff00;
    cpu.D[7] |= ~0x2000;
    return ex_common2();
}

void ex_common2() {
    cpu.D[7] |= 1 << 24;
    cpu.D[6] = cpu.A[7];
    cpu.A[7] = 0x2600;
    init_failure();
}

void warm_reset() {
    cpu.A[7] = 0x2600;
    cpu.D[7] = 0;
    cpu.D[6] = 0;
    // set VBR
    cpu.VBR = (cpu.PC & 0xFFF00000) | 0x46C06;
    // PDS init
    try {
        cpu.D[7] = 1 << 27;
        uint32_t pds = gAddrMap->addr[ADDR_PDS];
        if(ReadLTest(pds) == 0xAAAA5555) {
            cpu.A[1] = 0x46C58;
            cpu.A[3] = 0x4B9CA;
            rom_call(ReadLTest(pds + 4), {0x46C58, 0x4B9CA});
            if(cpu.PC == 0x4B9CA) {
                rom_call(0x4B9CA, 0);
                return;
            }
        }
    } catch(TestError &) {
    }
    cpu.D[7] &= ~(1 << 27);

    /* initialize IOP(SCC) */
    if(gAddrMapFlags & 1 << ADDR_IOP_SCC) {
        if(init_scc_iop()) {
            return init_failure();
        }
    }
    /* OSS reset */
    if((gHWCfgFlags & 0xff) == 6) {
        auto oss = gAddrMap->addr[ADDR_OSS];
        if(ReadB(oss + 0x206) & 2) {
            goto REFRESH_ICACHE;
        } else {
            goto NOT_FOUND;
        }
    }
    auto vid = (gHWCfgFlags >> 8) & 0xff;
    if(vid <= 0x0d) {
        goto NOT_POWERBOOK;
    }
    static const uint8_t quadraId[] = {0x0e, 0x10, 0x11, 0x14};
    for(auto q : quadraId) {
        if(q == vid) {
            goto NOT_POWERBOOK;
        }
    }
    goto POWERBOOK;
NOT_POWERBOOK:
    via1->writePA(0, false);
    via1->writePA(1, false);
    if(!via1->readPA(0)) {
        // ->46D5A
        goto NOT_FOUND;
    }
POWERBOOK:
    // $46CD8
    for(uint16_t offset : {0x00F8, 0x0078}) {
        if(xpram_raed_dword(offset) == FOURCC("SCBI")) {
            if([] -> bool {
                   // $46CF8
                   if(gAddrMapFlags & 1 << ADDR_SCSI) {
                       // normal SCSI
                       auto scsi = gAddrMap->addr[ADDR_SCSI];
                       return (ReadB(scsi + 0x40) & 0x1C) == 0x10;
                   } else {
                       // dual SCSI
                       auto scsi = gAddrMap->addr[ADDR_SCSI_QINT];
                       if(gAddrMapFlags & 1 << ADDR_SCSI_QEXT) {
                           scsi = gAddrMap->addr[ADDR_SCSI_QEXT];
                       }
                       return (ReadB(scsi + 0x40) & 0x7) != 4;
                   }
               }()) {
                // $46D3C
                /* rtc_send_3555(); */
                for(int i = 255; i >= 0; --i) {
                    xpram_write(i, 0);
                }
                goto REFRESH_ICACHE;

            } else {
            NOT_FOUND:
                // $46D5A
                for(uint16_t offset : {0x00FC, 0x007C}) {
                    if(xpram_read_dword(offset) == TO_FOURCC("RBBI")) {
                        /* return _49CD0(); */
                    }
                }
            }
        }
        break;
    }
REFRESH_ICACHE:
    OP::cinva_ic(0);
    // ADB dev init
}

struct IOP_FW {
    uint16_t size;
    uint16_t addr;
    std::vector<uint8_t> data;
};
static const IOP_FW iop_fw{0x0011,
                           0x7fee,
                           {0xA9, 0x81, 0x8D, 0x30, 0xF0, 0xA9, 0x44, 0x8D,
                            0x31, 0xF0, 0x80, 0xFE, 0xEE, 0x7F, 0xEE, 0x7F,
                            0xEE, 0x7F}};
int init_scc_iop() {
    iop_scc->write(2, 0x32);
    int16_t sz = iop_fw.size;
    iop_scc->RamAddress = iop_fw.addr;
    for(auto v : iop_fw.data) {
        iop_scc->write(4, v);
    }
    sz = iop_fw.size;
    for(auto p = iop_fw.data.begin(); sz >= 0; --sz, p++) {
        if(iop_scc->read(4) != *p) {
            return sz + 1;
        }
    }
    iop_scc->write(2, 0x36);
    return 0;
}

void detect_ram() {
	switch(gHWCfgFlags & 0xff) {
	case 0:
	case 1:
	case 2:
	case 3:
		/* N/A */
	case 5: // MDU
		break;
	case 4: // GLUE
		via2->Write(0x1E00, via2->Read(0x1E00) | 0xC0);
		break;
	case 6: // OSS
	{
		auto exp0 = gAddrMap[ADDR_EXP0];
		for(unsigned int i = 0, v = 0xf3ff; i < 16;
			++i, v >>= 1 ) {
			WriteB(exp0, v); 
		}
		WriteB(exp0 + 0x10, 0);
		break;
	}
	case 7: // V8
	{
		auto rbv = gAddrMap[ADDR_RBV];
		WriteB(rbv, ReadB(rbv) | 0xE0 );
		break;
	}
	case 8; // MCU
	{
		auto mcu = gAddrMap[ADDR_MCU_CTL];
		for(unsigned int i = 0, v = 0x30810, i < 18;
			++i, v >>= 1 ) {
			WriteL(mcu + 4*i, v);
		}
		WriteL(mcu + 0xA0, -1);
		break;
	}
	case 9: // JAWS
	{
		auto jaws = gAddrMap[ADDR_JAWS_CTL] + 0x10000;
		WriteB(jaws, 0 );
		WriteB(jaws + 0x2000, 0);
		WriteB(jaws + 0x4000, 0);
		break;
	}
	case 10: // N/A
	{
		auto hw = gAddrMap[27];
		WriteB(hw, ReadB(hw) | 0xE0 );
	}
	}
	auto mem = gUnivInfo->ram->ranges.begin();
	constexpr auto Tina = FOURCC("Tina");
	uint32_t size = 0;
	std::vector<uint8_t> d5, d6;
	for(int i = 0;;++i) {
		if( mem == gUnivInfo->ram->ranges.end() ) {
			break;
		}
		auto a0 = mem->first;
		auto a1 = mem->second;
		auto d6_v =  ram_test_dword(a0, Tina);
		d6.push_back( );
		if( d6_v == 0 ) {
			// OK, accessable memory
			auto [ sz, pt] = simm_exist_check(mem->first, mem->second, Tina, gUnivInfo->ram->unit);
			d6.push_back( 0 );
			d5.push_back( pt );
			size += sz;
		} else {
			if( d6_v == 0xf ) {
				// cannot access memory at all
				d6_v = 0;
			}
			d6.push_back( d6_v );
			d5.push_back( 0 );
		}
	}
}
std::pair<uint32_t, uint8_t> simm_exist_check(uint32_t begin, uint32_t end, uint32_t ptn, uint32_t unit) {
	uint8_t d5 = 0;
	for(int sz = end - begin, off = 0; sz >= 0; sz -= unit ) {
		auto d2 = ReadL(begin + sz - 4);
		WriteL(begin + sz - 4, ptn);
		if( ReadL(begin + sz - 4) == ptn ) {
			do {
				off += unit;
			} while( ReadL(begin + off - 4) != ptn );
			WriteL(begin + off - 4, d2);
			sz -= off;
			if( sz ) {
				int d3 = 0;
				for(; unit != sz; d3++, unit *= 2 ) {					
				}
				return { sz, d3}				
			}
		}
	}
	return {0, 0};
}

void init_failure() {
    cpu.A[7] = 0x2600;
    if(cpu.D[7] & 1 << 26) {
        play_sadmacBeep1();
        _4A7E6();
        return;
    }
    if(gAddrMapFlags & 1 << ADDR_PDS) {
        try {
            cpu.D[7] = 1 << 27;
            uint32_t pds = gAddrMap->addr[ADDR_PDS];
            if(ReadLTest(pds) == 0xAAAA5555) {
                cpu.A[1] = 0x49B4C;
                rom_call(ReadLTest(pds + 4), 0x49B4C);
            }
        } catch(TestError &) {
        }
    }
    cpu.D[7] &= ~(1 << 27);
    cpu.D[7] |= 1 << 16;
    cpu.D[7] |= 1 << 22;
    cpu.D[4] = 12;
    activate_timer2_sadmac();
    OP::cinva_ic(0);
    _4A7E6();
}

void _4A7E6() {
    if(gHWCfgFlags & 1 << 24) {
        // power book ?
        JawsHandshake();
    }
    if(!(cpu.D[7] & 1 << 26)) {
        if(gAddrMapFlags & 1 << ADDR_ASC) {
            play_sadmacBeep4();
        }
    }
    init_modem_port();
_4A840:
    if(auto vp = read_modem_port()) {
        int8_t v = *vp;
        v &= 0x7f;
        if(v == '*') {
            cpu.D[7] |= 1 << 19;
        } else if(cpu.D[7] & 1 << 19) {
            if(v != '!') {
                goto _4A95E;
            }
            uint32_t v2 = _4B5AE();
#if 0
            _4AE28();
            _4AE54();
            if(v2 == 0) {
                v2 = 0x21;
            }

            // $4A942
            _4AEB0();
            ERR.B23 = true;
            _4B036(0);
            ERR.B23 = false;
#endif
        } else {
#if 0
            // $4A894
            ERR.B19 = false;
            switch(v & 0xff) {
            case 'S':
                _4AEFC(0);
                // $4AA0C
                break;
            case 'L':
                _4AEFC(4);
                // $4AA1C
                break;
            case 'B':
                _4AEFC(2);
                // $4AA3C
                break;
            case 'D':
                _4AEFC(0);
                // $4AA5A
                break;
            case 'C':
                _4AEFC(0);
                // $4AA7E
                break;
            case 'G':
                _4AEFC(4);
                // $4AAA4
                break;
            case '0':
                _4AEFC(4);
                // $4AABE
                break;
            case '1':
                _4AEFC(4);
                // $4AAD8
                break;
            case '2':
                _4AEFC(4);
                // $4AAF2
                break;
            case '3':
                _4AEFC(0);
                // $4AAFE
                break;
            case '4':
                _4AEFC(0);
                // $4AB46
                break;
            case '5':
                _4AEFC(0);
                // $4AB4E
                break;
            case '6':
                _4AEFC(0);
                // $4AB6A
                break;
            case '7':
                _4AEFC(0);
                // $4AB70
                break;
            case 'A':
                _4AEFC(0);
                // $4AB78
                break;
            case 'H':
                _4AEFC(0);
                // $4AB80
                break;
            case 'R':
                _4AEFC(0);
                // $4AB88
                break;
            case 'M':
                _4AEFC(0);
                // $4ABA8
                break;
            case 'E':
                _4AEFC(0);
                // $4ABCA
                break;
            case 'I':
                _4AEFC(0);
                // $4ABD2
                break;
            case 'P':
                _4AEFC(2);
                // $4ABDC
                break;
            case 'T':
                _4AEFC(4);
                // $4ABE8
                break;
            case 'N':
                _4AEFC(4);
                // $4ACB6
                break;
            case 'V':
                _4AEFC(4);
                // $4ADAC
                break;
            case 'Z':
                _4AEFC(4);
                // $4AE14
                break;
            default:
                // $4A95E
                _4AEB0();
                goto _4A966;
            }
#endif
        }
    }
_4A95E:;
#if 0
_4A966:
    if(ERR.B16) {
        if(ERR.B22) {
            if(via1->vIFR & 1 << 5) { // TIMER2
                via1->vIER = 0x20;
                via1->vT2CL = 0xff;
                via1->vT2CH = 0xff;

            } else {
                goto _4A840;
            }
            // $4A99A
        }
    }
#endif
    // $4AA08
}
void get_scc_addr(uint32_t *r, uint32_t *offset) {
    if(gAddrMapFlags & 1 << ADDR_IOP_SCC) {
        *r = gAddrMap->addr[ADDR_IOP_SCC];
        *offset = 0;
    } else {
        *r = gAddrMap->addr[ADDR_SCC_R];
        *offset = gAddrMap->addr[ADDR_SCC_W] - *r;
    }
}
} // namespace LibROM
