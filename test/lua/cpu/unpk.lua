-- UNPK %D1, %D3, #5
MEM[0].W = OCT"103601"
MEM[2].W = 5
MEM[4].W = TEST.BREAK

-- UNPK -(%A1), -(%A3), #5
MEM[6].W = OCT"103611" 
MEM[8].W = 5
MEM[10].W = TEST.BREAK

JIT_COMPILE(0, 12)

local run = {}

function run.reg()
    CPU.D[2].B = 0x34
    TEST.RUN(0)
    TEST.CHECK("CPU.D[4].W", 0x0309) 
end

function run.mem()
    MEM[0x1000].B = 0x34
    CPU.A[2] = 0x1001
    CPU.A[4] = 0x1012
    TEST.RUN(6)
    TEST.CHECK("MEM[0x1010].W", 0x0309)
    TEST.CHECK("CPU.A[2]", 0x1000) 
    TEST.CHECK("CPU.A[4]", 0x1010) 
end
return run