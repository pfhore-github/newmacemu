-- UNLK.W %A2
MEM[0].W = OCT "047130" | 2
MEM[2].W = TEST.BREAK


JIT_COMPILE(0, 4)
local run = {}
function run.execute()
    CPU.A[3] = 0x1000
    CPU.A[8] = 0x2000
    MEM[0x1000].L = 0x2200
    TEST.RUN(0)
    TEST.CHECK("CPU.A[3]", 0x2200)
    TEST.CHECK("CPU.A[8]", 0x1004)
end
return run