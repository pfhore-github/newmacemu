-- TAS %D2
MEM[0].W = OCT "045300" | 2
MEM[2].W = TEST.BREAK

JIT_COMPILE(0, 4)

local run = {}

function run.value()
    CPU.D[3].B = 1
    TEST.RUN(0)
    TEST.CHECK("CPU.D[3].B", 0x81)
    TEST.CHECK("CPU.CC.V", false)
    TEST.CHECK("CPU.CC.C", false)
end

run.n = farray(TF, function(b)
    CPU.D[3].B = cond(b, 0x80, 0)
    TEST.RUN(0)
    TEST.CHECK("CPU.CC.N", b)
end)
run.z = farray(TF, function(b)
    CPU.D[3].B = cond(b, 0, 1)
    TEST.RUN(0)
    TEST.CHECK("CPU.CC.Z", b)
end)
return run
