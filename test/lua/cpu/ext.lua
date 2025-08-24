-- EXT.W %D2
MEM[0].W = OCT "044200" | 2
MEM[2].W = TEST.BREAK

-- EXT.L %D3
MEM[4].W = OCT "044300" | 3
MEM[6].W = TEST.BREAK

-- EXTB.L %D4
MEM[8].W = OCT "044700" | 4
MEM[10].W = TEST.BREAK

JIT_COMPILE(0, 12)

local run = {}

run.W = {
    value = function()
        CPU.D[3].B = 0xFF
        TEST.RUN(0)
        TEST.CHECK("CPU.D[3].W", 0xFFFF)
    end,
    n = farray(TF, function(b)
        CPU.D[3].B = cond(b, 0xFF, 1)
        TEST.RUN(0)
        TEST.CHECK("CPU.CC.N", b)
    end),
    z = farray(TF, function(b)
        CPU.D[3].B = cond(b, 0, 1)
        TEST.RUN(0)
        TEST.CHECK("CPU.CC.Z", b)
    end),
}


run.L = {
    value = function()
        CPU.D[4].W = 0xFFFF
        TEST.RUN(4)
        TEST.CHECK("CPU.D[4].L", 0xFFFFFFFF)
    end,
    n = farray(TF, function(b)
        CPU.D[4].W = cond(b, 0xFFFF, 1)
        TEST.RUN(4)
        TEST.CHECK("CPU.CC.N", b)
    end),
    z = farray(TF, function(b)
        CPU.D[4].B = cond(b, 0, 1)
        TEST.RUN(4)
        TEST.CHECK("CPU.CC.Z", b)
    end),
}

run.B2L = {
    value = function()
        CPU.D[5].B = 0xFF
        TEST.RUN(8)
        TEST.CHECK("CPU.D[5].L", 0xFFFFFFFF)
    end,
    n = farray(TF, function(b)
        CPU.D[5].B = cond(b, 0xFF, 1)
        TEST.RUN(8)
        TEST.CHECK("CPU.CC.N", b)
    end),
    z = farray(TF, function(b)
        CPU.D[5].B = cond(b, 0, 1)
        TEST.RUN(8)
        TEST.CHECK("CPU.CC.Z", b)
    end),
}
return run
