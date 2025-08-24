-- EXG %D2, %D3
MEM[0].W = OCT "140500" | 2 << 9 | 3
MEM[2].W = TEST.BREAK

-- EXG %A2, %A3
MEM[4].W = OCT "140510" | 2 << 9 | 3
MEM[6].W = TEST.BREAK

-- EXG %D2, %A3
MEM[8].W = OCT "140610" | 2 << 9 | 3
MEM[10].W = TEST.BREAK

JIT_COMPILE(0, 12)
local run = {}

function run.d2d()
    CPU.D[3].L = 0x12345678
    CPU.D[4].L = 0x9ABCDEF0
    TEST.RUN(0)
    TEST.CHECK("CPU.D[3].L", 0x9ABCDEF0)
    TEST.CHECK("CPU.D[4].L", 0x12345678)
end

function run.a2a()
    CPU.A[3] = 0x12345678
    CPU.A[4] = 0x9ABCDEF0
    TEST.RUN(4)
    TEST.CHECK("CPU.A[3]", 0x9ABCDEF0)
    TEST.CHECK("CPU.A[4]", 0x12345678)
end

function run.d2a()
    CPU.D[3].L = 0x12345678
    CPU.A[4] = 0x9ABCDEF0
    TEST.RUN(8)
    TEST.CHECK("CPU.D[3].L", 0x9ABCDEF0)
    TEST.CHECK("CPU.A[4]", 0x12345678)
end


return run
