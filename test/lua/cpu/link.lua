-- LINK.W %A2, #-64
MEM[0].W = OCT "047120" | 2
MEM[2].H = -64
MEM[4].W = TEST.BREAK

-- LINK.L %A3, #-64
MEM[6].W = OCT"044010" | 3
MEM[8].I = -64
MEM[12].W = TEST.BREAK

JIT_COMPILE(0, 14)
local run = {}
function run.W()
    CPU.A[3] = 0x1010
    TEST.RUN(0)
    TEST.CHECK("MEM[0x5FFC].L", 0x1010)
    TEST.CHECK("CPU.A[3]", 0x5FFC)
    TEST.CHECK("CPU.A[8]", 0x5FBC)
end

function run.L()
    CPU.A[4] = 0x1010
    TEST.RUN(6)
    TEST.CHECK("MEM[0x5FFC].L", 0x1010)
    TEST.CHECK("CPU.A[4]", 0x5FFC)
    TEST.CHECK("CPU.A[8]", 0x5FBC)
end

return run