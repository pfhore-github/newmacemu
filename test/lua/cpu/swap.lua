-- SWAP.W %D2
MEM[0].W = OCT "044100" | 2
MEM[2].W = TEST.BREAK

JIT_COMPILE(0, 4)
local run = {}

function run.value()
    CPU.D[3].L = 0x12345678
    TEST.RUN(0)
    TEST.CHECK("CPU.D[3].L", 0x56781234)
    TEST.CHECK("CPU.CC.V", false)
    TEST.CHECK("CPU.CC.C", false)
end

run.N = farray(TF, function(b)
    CPU.D[3].L = cond(b, 0x11119999, 0x99991111)
    TEST.RUN(0)
    TEST.CHECK("CPU.CC.N", b)
end)

run.Z = farray(TF, function(b)
    CPU.D[3].L = cond(b, 0, 1)
    TEST.RUN(0)
    TEST.CHECK("CPU.CC.Z", b)
end)
return run