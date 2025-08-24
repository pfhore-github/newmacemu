-- NOT.B %D2
MEM[0].W = OCT "0043000" | 2
MEM[2].W = TEST.BREAK

-- NOT.W %D3
MEM[4].W = OCT "0043100" | 3
MEM[6].W = TEST.BREAK

-- NOT.L %D4
MEM[8].W = OCT "0043200" | 4
MEM[10].W = TEST.BREAK

JIT_COMPILE(0, 12)

local run = {}

run.B = {
    value = function()
        CPU.D[3].B = 0x87
        TEST.RUN(0)
        TEST.CHECK("CPU.D[3].B", 0x78)
        TEST.CHECK("CPU.CC.V", false)
        TEST.CHECK("CPU.CC.C", false)
    end,
    n = farray(TF, function(b)
        CPU.D[3].B = cond(b, 0x20, 0xC0)
        TEST.RUN(0)
        TEST.CHECK("CPU.CC.N", b)
    end),
    z = farray(TF, function(b)
        CPU.D[3].B = cond(b, 0xFF, 0)
        TEST.RUN(0)
        TEST.CHECK("CPU.CC.Z", b)
    end)
}

run.W = {
    value = function()
        CPU.D[4].W = 0x8421
        TEST.RUN(4)
        TEST.CHECK("CPU.D[4].W", 0x7BDE)
        TEST.CHECK("CPU.CC.V", false)
        TEST.CHECK("CPU.CC.C", false)
    end,
    n = farray(TF, function(b)
        CPU.D[4].W = cond(b, 0x2000, 0xC000)
        TEST.RUN(4)
        TEST.CHECK("CPU.CC.N", b)
    end),
    z = farray(TF, function(b)
        CPU.D[4].W = cond(b, 0xFFFF, 0)
        TEST.RUN(4)
        TEST.CHECK("CPU.CC.Z", b)
    end)
}

run.L = {
    value = function()
        CPU.D[5].L = 0xF2345678
        TEST.RUN(8)
        TEST.CHECK("CPU.D[5].L", 0x0DCBA987)
        TEST.CHECK("CPU.CC.V", false)
        TEST.CHECK("CPU.CC.C", false)
    end,
    n = farray(TF, function(b)
        CPU.D[5].L = cond(b, 0x20000000, 0xC0000000)
        TEST.RUN(8)
        TEST.CHECK("CPU.CC.N", b)
    end),
    z = farray(TF, function(b)
        CPU.D[5].L = cond(b, 0xFFFFFFFF, 0)
        TEST.RUN(8)
        TEST.CHECK("CPU.CC.Z", b)
    end)
}

return run
