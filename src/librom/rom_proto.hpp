#ifndef ROM_PROTO_HPP_
#define ROM_PROTO_HPP_ 1
#include "./rom_common.hpp"
#include "inline.hpp"
#include <stdint.h>
#include <expected>
#include <unordered_set>
#include <vector>
void rom_call(uint32_t target, const std::unordered_set<uint32_t> &ret);
void rom_call(uint32_t target, uint32_t ret);
namespace LibROM {
// $0008C-$000B8
void boot();

// $008E0-$008EC
void boot2();

// $02E00-$02E78
void init_hw() ;
// $02E8C-$02F18
void init_via_rbv();
// $02F18-$03060
void getHwInfo(uint8_t id);
// $03060-$0307E
bool check_glue(uint32_t map);
// $0307E-$030BE
bool check_mdu(uint32_t map);
// $030BE-$0311A
bool check_oss(uint32_t map);
// $0311A-$03154
bool check_v8(uint32_t map);
// $03154-$03162
bool check_mcu(uint32_t map);
// $03162-$031A6
bool check_jaws(uint32_t map);
// $03DAE-$03DBA
uint32_t jump_to_rom(uint32_t base);
// $03DBA-$03DD8
void refresh_cpu_dcache(uint32_t* flags);
// $04052-$040A2
void init_mmu() ;
// $04640-$04670
bool invalidate_cpu_dcache();
/* $046A8: base: %A2 */
bool check_via0(uint32_t base); 
/* $046AA-$046D6: base: %A2, offset: %D2 */
bool check_via(uint32_t base, uint32_t offset); 
/* $046D6-$46FE: base: %A2 */
bool check_rbv(uint32_t base) ;
/* $046FE-$4706: base: %A2 */
bool check_scsi_dma(uint32_t base) ;
/* $0477A-$47AE: base: %A2 */
bool check_iop(uint32_t base) ;
/* $047AE-$04848: addrMap:%A0, addrFlags: %D0, ret: %D1*/
uint32_t getHwId(const AddrMap* addrMap, uint32_t addrFlags);
// $04848-$48C8
void init_mcu_jaws();
// $048C8-$4900
void init_hw2(uint32_t offset);

//$07058-$705E
void play_asc_chime1();
//$0706A-$706E
void play_asc_chime4();
struct AscChime {
	int16_t volume;
	int32_t wait;
	int32_t delay;
	int32_t length;
	int16_t ch;
	int32_t values[4];
};
//$0706E-$7158
void play_asc_chime(const AscChime* t);

//$45C1C-$45C2E
void play_sadmacBeep1();

//$45C52-$45C64
void play_sadmacBeep4();

//$45C64-$45E3C
void play_sadmacBeep(int n);

// $45E3C-$45E48
void wait_easc_fifo();

//$45EA2-$45F3C
void play_easc_chime(uint16_t volume, const uint16_t vals[4]);

// $46A80-$46A90
void ex_busErr();

// $46A90-$46A96
void ex_addressErr();

// $46A96-$46A9C
void ex_illInstErr();

// $46A9C-$46AA2
void ex_zeroDivErr();

// $46AA2-$46AA8
void ex_chkErr();

// $46AA8-$46AAE
void ex_trapErr();

// $46AAE-$46AB4
void ex_privErr();

// $46AB4-$46ABA
void ex_traceErr();

// $46ABA-$46AC0
void ex_lineAErr();

// $46AC0-$46AC6
void ex_lineFErr();

// $46AC6-$46ACC
void ex_miscErr();

// $46ACC-$46AD2
void ex_coprocessorErr();

// $46AD2-$46AD8
void ex_formatErr();

// $46AD8-$46ADE
void ex_irqErr();

// $46ADE-$46AE4
void ex_vtrapErr();

// $46AE4-$46AEA
void ex_irq1();

// $46AEA-$46AF0
void ex_irq2();

// $46AF0-$46AF6
void ex_irq3();

// $46AF6-$46AFC
void ex_irq4();

// $46AFC-$46B00
void ex_irq5();

// $46B00-$46B04
void ex_common();

// $46B04-$46B22
void ex_irq6();

// $46B24-$46B96
void ex_irq7();

// $46B96-$46BA0
void ex_fpBsunErr();

// $46BA0-$46BAA
void ex_fpInexErr();

// $46BAA-$46BB4
void ex_fpDiv0Err();

// $46BB4-$46BBE
void ex_fpUnflErr();

// $46BBE-$46BC8
void ex_fpOpErErr();

// $46BC8-$46BD2
void ex_fpOvflErr();

// $46BD2-$46BDC
void ex_fpSnanErr();

// $46BDC-$46BE6
void ex_fp56Err();

// $46BE6-$46BF0
void ex_fp57Err();

// $46BF0-$46BF8
void ex_fp58Err();

// $46BF8-$46C06
void ex_common2();

// $46C06-46FEE
void warm_reset();

// $470BA-$470C8
inline void getHwInfo2(uint8_t d) {
	SetSR(GetSR() | 0x0700);
	getHwInfo(d);
}

void _4711A();

// $471C6-$4721C
bool JawsHandshake();

// $4723A-$47278
bool via2PortAHandshake(uint8_t v);

// $477C4-47800
int init_scc_iop();

// $49AFA-$49B9A
void init_failure();

// $4A7E6-
void _4A7E6();

// $4AE28-$4AE54
void get_scc_addr(uint32_t* r, uint32_t* w);

// $4AE54-$4AE96
void init_modem_port();

// $4AEB0-$4AECC
void send_modem_port(uint8_t value);

// $4AF4A-$4AF72
void activate_timer2_sadmac();

// $4AF9E-$4AFCE
std::optional<uint8_t> read_modem_port();

// $4AFCE-$4AFEC
uint8_t ascii_to_hex(uint8_t v);

// $4B09A-$4B0B8
void rtc_send(uint8_t v); 

// $4B0B8-$4B0DA
uint8_t rtc_recv(); 

// $4B258-$4B3CA
uint8_t xpram_read(uint32_t offset);

// $4B3CA-$4B3EA
void xpram_send_sr(uint8_t v);

// $4B3EA-$4B412
uint8_t xpram_recv_sr();

// $4B412-$4B41E
void throw_adb_data();

// $4B41E-$4B44A
uint8_t xpram_recv_sr_blocked();

// $4B5AE-$4B642
uint32_t _4B5AE();

void _4B642(uint32_t a5);

// $4B89C-$4B906
std::vector<uint8_t> read_modem_port_hex(int len);

// $4B906-$4B95A
void read_modem_port_hex_string(int len, uint32_t dst_addr);

// $4B95A-4B96A
uint8_t hex_to_ascii(uint8_t v);

// $4B96C-4B986
void send_modem_port_crlf();
// $4BAD0-$4BBDC
void mem_check();
// $4BBDC-$4BC14
std::pair<uint32_t, uint8_t> simm_exist_check(uint32_t begin, uint32_t end, uint32_t magic, uint32_t unit);
// $4BC14-$4BC48
uint8_t ram_test_dword(uint32_t addr, uint32_t magic);
void _4BC48(uint32_t ram_size, uint32_t exist_simm_flag);
// $8AC00
void init_pb();
}
#endif
