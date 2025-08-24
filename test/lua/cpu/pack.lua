-- PACK %D1, %D3, #5
MEM[0].W = OCT"0100500" | 3 << 9 | 1
MEM[2].W = 5
MEM[4].W = TEST.BREAK

-- PACK -(%A1), -(%A3), #5
MEM[6].W = OCT"0100510" | 3 << 9 | 1 
MEM[8].W = 5
MEM[10].W = TEST.BREAK

JIT_COMPILE(0, 12)

local run = {}

function run.reg()
    CPU.D[2].W = 0x0304
    TEST.RUN(0)
    TEST.CHECK("CPU.D[4].B", 0x39) 
end

function run.mem()
    MEM[0x1000].W = 0x0304
    CPU.A[2] = 0x1002
    CPU.A[4] = 0x1011
    TEST.RUN(6)
    TEST.CHECK("MEM[0x1010].B", 0x39)
    TEST.CHECK("CPU.A[2]", 0x1000) 
    TEST.CHECK("CPU.A[4]", 0x1010) 
end
return run