-- MOVE16 (%A3)+, (%A4)+
MEM[0].W = OCT "173040" | 3
MEM[2].W = 0x8000 | (4 << 12)
MEM[4].W = TEST.BREAK

-- MOVE16 #0x1203, (%A3)
MEM[6].W = OCT "173030" | 3
MEM[8].L = 0x1203
MEM[12].W = TEST.BREAK

-- MOVE16 #0x1203, (%A3)+
MEM[14].W = OCT "173010" | 3
MEM[16].L = 0x1203
MEM[20].W = TEST.BREAK

-- MOVE16 (%A3), #0x1303
MEM[22].W = OCT "173020" | 3
MEM[24].L = 0x1303
MEM[28].W = TEST.BREAK

-- MOVE16 (%A3)+, #0x1303
MEM[30].W = OCT "173000" | 3
MEM[32].L = 0x1303
MEM[36].W = TEST.BREAK

JIT_COMPILE(0, 38)

local run = {}
function run.pre()
    MEM[0x1200].L = 0x01234567
    MEM[0x1204].L = 0x89ABCDEF
    MEM[0x1208].L = 0xFEDCBA98
    MEM[0x120C].L = 0x76543210
end

function run.post()
    TEST.CHECK("MEM[0x1300].L", 0x01234567)
    TEST.CHECK("MEM[0x1304].L", 0x89ABCDEF)
    TEST.CHECK("MEM[0x1308].L", 0xFEDCBA98)
    TEST.CHECK("MEM[0x130C].L", 0x76543210)
end

function run.reg2reg()
    CPU.A[4] = 0x1207
    CPU.A[5] = 0x1303
    TEST.RUN(0)
    TEST.CHECK("CPU.A[4]", 0x1217)
    TEST.CHECK("CPU.A[5]", 0x1313)
end

function run.imm2reg()
    CPU.A[4] = 0x1307
    TEST.RUN(6)
    TEST.CHECK("CPU.A[4]", 0x1307)
end

function run.imm2inc()
    CPU.A[4] = 0x1307
    TEST.RUN(14)
    TEST.CHECK("CPU.A[4]", 0x1317)
end

function run.reg2imm()
    CPU.A[4] = 0x1207
    TEST.RUN(22)
    TEST.CHECK("CPU.A[4]", 0x1207)
end

function run.incr2mem()
    CPU.A[4] = 0x1207
    TEST.RUN(30)
    TEST.CHECK("CPU.A[4]", 0x1217)
end
return run
