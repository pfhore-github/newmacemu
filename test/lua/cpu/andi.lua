-- ANDI.B #0xF0, %D1
MEM[0].W = OCT "001000" | 1
MEM[2].W = 0xF0
MEM[4].W = TEST.BREAK

-- ANDI.W #0xFF00, %D2
MEM[6].W = OCT "001100" | 2
MEM[8].W = 0xFF00
MEM[10].W = TEST.BREAK

-- ANDI.L #0xFF000000, %D3
MEM[12].W = OCT "001200" | 3
MEM[14].L = 0xFF000000
MEM[18].W = TEST.BREAK

-- ANDI.W #0x1F, %CCR
MEM[20].W = OCT "001074"
MEM[22].W = 0
MEM[24].W = TEST.BREAK

-- ANDI.L #0x0700, %SR
MEM[26].W = OCT "001174"
MEM[28].W = 0x7000
MEM[30].W = TEST.BREAK

JIT_COMPILE(0, 32)

local run = {}
run.B = {
    value = function()
        CPU.D[2].B = 0xCF
        TEST.RUN(0)
        TEST.CHECK("CPU.D[2].B", 0xC0)
    end,

    z = farray(TF, function(b)
        CPU.D[2].B = cond(b, 0, 0xFF)
        TEST.RUN(0)
        TEST.CHECK("CPU.CC.Z", b)
    end),

    n = farray(TF, function(b)
        CPU.D[2].B = cond(b, 0x80, 0)
        TEST.RUN(0)
        TEST.CHECK("CPU.CC.N", b)
    end)
}
run.W = {
    value = function()
        CPU.D[3].W = 0x4FFF
        TEST.RUN(6)
        TEST.CHECK("CPU.D[3].W", 0x4F00)
    end,

    z = farray(TF, function(b)
        CPU.D[3].W = cond(b, 0, 0xFFFF)
        TEST.RUN(6)
        TEST.CHECK("CPU.CC.Z", b)
    end),

    n = farray(TF, function(b)
        CPU.D[3].W = cond(b, 0x8000, 0)
        TEST.RUN(6)
        TEST.CHECK("CPU.CC.N", b)
    end)
}
run.L = {
    value = function()
        CPU.D[4].L = 0x4BCDEF01
        TEST.RUN(12)
        TEST.CHECK("CPU.D[4].L", 0x4B000000)
    end,

    z = farray(TF, function(b)
        CPU.D[4].L = cond(b, 0, 0xFFFFFFFF)
        TEST.RUN(12)
        TEST.CHECK("CPU.CC.Z", b)
    end),

    n = farray(TF, function(b)
        CPU.D[4].L = cond(b, 0x80000000, 0)
        TEST.RUN(12)
        TEST.CHECK("CPU.CC.N", b)
    end)
}

function run.CCR()
    CPU.CC.value = 0x1F
    TEST.RUN(20)
    TEST.CHECK("CPU.CC.X", false)
    TEST.CHECK("CPU.CC.V", false)
    TEST.CHECK("CPU.CC.C", false)
    TEST.CHECK("CPU.CC.N", false)
    TEST.CHECK("CPU.CC.Z", false)
end
run.SR = {
    ng = function() priv_test(26) end,
    ok = function()
        CPU.SR.S = true
        CPU.SR.T = 0
        CPU.SR.I = 7
        TEST.RUN(26)
        TEST.CHECK(CPU.SR.I, 0)
    end,
    trace = function() trace_test(26) end
}

return run
