-- MOVEP.W %D1, (%A3, 0x10)
MEM[0].W = OCT "000610" | 1 << 9 | 3
MEM[2].W = 0x10
MEM[4].W = TEST.BREAK

-- MOVEP.L %D2, (%A3, 0x10)
MEM[6].W = OCT "000710" | 2 << 9 | 3
MEM[8].W = 0x10
MEM[10].W = TEST.BREAK

-- MOVEP.W (%A3, 0x10), %D1
MEM[12].W = OCT "000410" | 1 << 9 | 3
MEM[14].W = 0x10
MEM[16].W = TEST.BREAK

-- MOVEP.W (%A4, 0x10), %D2
MEM[18].W = OCT "000510" | 2 << 9 | 3
MEM[20].W = 0x10
MEM[22].W = TEST.BREAK

JIT_COMPILE(0, 24)
local run = {}
run.W = {
    store = function()
        CPU.D[2].W = 0x1234
        CPU.A[4] = 0x1000
        TEST.RUN(0)
        TEST.CHECK("MEM[0x1010].B", 0x12)
        TEST.CHECK("MEM[0x1012].B", 0x34)
    end,
    load = function()
        CPU.A[4] = 0x1000
        MEM[0x1010].L = 0x12345678
        TEST.RUN(12)
        TEST.CHECK("CPU.D[2].W", 0x1256)
    end
}

run.L = {
    store = function()
        CPU.D[3].L = 0x12345678
        CPU.A[4] = 0x1000
        TEST.RUN(6)
        TEST.CHECK("MEM[0x1010].B", 0x12)
        TEST.CHECK("MEM[0x1012].B", 0x34)
        TEST.CHECK("MEM[0x1014].B", 0x56)
        TEST.CHECK("MEM[0x1016].B", 0x78)
    end,
    load = function()
        CPU.A[4] = 0x1000
        MEM[0x1010].L = 0x12345678
        MEM[0x1014].L = 0x9ABCDEF0
        TEST.RUN(18)
        TEST.CHECK("CPU.D[3].L", 0x12569ADE)
    end
}

return run
