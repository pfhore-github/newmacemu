# Apple Hardwere Reference Manual
## Memory Map
|:-address-:|:-type-:|
|$00000000-$0FFFFFFFF|ROM(initial)/RAM(after access $4xxxxxxx)|
|$10000000-$3FFFFFFFF|RAM|
|$4xxx0000-$4xxxFFFFF|ROM(mirror)|
|$5xx00000|VIA1|
|$5xx02000|VIA2(except IIfx, LC)|
|$5xx04000|SCC(except IIfx, Quadra)|
|$5xx06000|SCSI handshake(except Quadra, IIfx)/SCSI pDMA(powerBook 160)|
|$5xx08000|SCSI DMA(IIfx)|
|$5xx0A000|Ethernet(Quadra)|
|$5xx0C000|IOP-SCC(Quadra, IIci/IIsi?)|
|$5xx0E000|Quadra specific register(includeing Quadra SCSI)|
|$5xx10000|SCSI(except Quadra, IIfx)/ASC(IIfx)|
|$5xx12000|SCSI pDMA(except Powerbook, Quadra, IIfx)/IOP-SWIM(IIfx)|
|$5xx14000|ASC(except IIfx)|
|$5xx16000|SWIM(except Quadra, IIfx)|
|$5xx18000|SCSI DMA(IIci, IIsi)|
|$5xx1A000|IIfx specific register|
|$5xx1C000|IIfx expansion 0|
|$5xx1E000|IIfx expansion 1/IOP-SWIM(Quadra, IIci/IIsi?)|
|$5xx20000|mirror of VIA1(IIcx,SE/30)|
|$5xx22000|mirror of VIA2(IIcx,SE/30)|
|$5xx24000|mirror of SCC(IIcx,SE/30)/VDAC(IIci, IIsi, LC)|
|$5xx26000|mirror of SCSI handshake(IIcx,SE/30)/RDV(IIci, IIsi, LC)|
|$5xx80000|PowerBook sepcific register|

## VIA 
|offset|
|$0000|ORB/IRB|
|$0200|ORA/IRA|
|$0400|DDRB|
|$0600|DDRA|
|$0800|TIMER1.L|
|$0A00|TIMER1.H|
|$0C00|TIMER1-latch.L|
|$0E00|TIMER1-latch.H|
|$1000|TIMER2.L|
|$1200|TIMER2.H|
|$1400|SR|
|$1600|ACR|
|$1800|PCR|
|$1A00|IF|
|$1C00|IE|
|$1E00|ORA/IRA;no handshake|
