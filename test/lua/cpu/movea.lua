-- MOVEA.W %D2, %A3
MEM[0].W = OCT "0030100" | 3 << 9 | 2
MEM[2].W = TEST.BREAK

-- MOVEA.L %D2, %A3
MEM[4].W = OCT "0020100" | 3 << 9 | 2
MEM[6].W = TEST.BREAK

JIT_COMPILE(0, 8)

local run = {}
function run.pre()
    CPU.D[3].W = 0xFFF3
end
function run.W()
    TEST.RUN(0)
    TEST.CHECK("CPU.A[4]", 0xFFFFFFF3)
end
function run.L()
    TEST.RUN(4)
    TEST.CHECK("CPU.A[4]", 0x0000FFF3)
end

return run 
