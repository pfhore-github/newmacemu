-- ORI.B #0x1F, %D1
MEM[0].W = OCT "0000000" | 1
MEM[2].W = 0x1F
MEM[4].W = TEST.BREAK

-- ORI.B #0, %D1
MEM[6].W = OCT "0000000" | 1
MEM[8].W = 0
MEM[10].W = TEST.BREAK

-- ORI.W #0x1FFF, %D2
MEM[12].W = OCT "0000100" | 2
MEM[14].W = 0x1FFF
MEM[16].W = TEST.BREAK

-- ORI.W #0, %D2
MEM[18].W = OCT "0000100" | 2
MEM[20].W = 0
MEM[22].W = TEST.BREAK

-- ORI.L #0x1FFFFFFF, %D3
MEM[24].W = OCT "0000200" | 3
MEM[26].L = 0x1FFFFFFF
MEM[30].W = TEST.BREAK

-- ORI.L #0, %D3
MEM[32].W = OCT "0000200" | 3
MEM[34].L = 0
MEM[38].W = TEST.BREAK

-- ORI.W #0x1F, %CCR
MEM[40].W = OCT "0000074"
MEM[42].W = 0x1F
MEM[44].W = TEST.BREAK

-- ORI.L #0x0700, %SR
MEM[46].W = OCT "0000174"
MEM[48].W = 0x0700
MEM[50].W = TEST.BREAK

JIT_COMPILE(0, 52)

local run = {}
run.B = {
    value = function()
        CPU.D[2].B = 0x20
        TEST.RUN(0)
        TEST.CHECK("CPU.D[2].B", 0x3F)
    end,

    z = farray(TF, function(b)
        CPU.D[2].B = cond(b, 0, 1)
        TEST.RUN(6)
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
        CPU.D[3].W = 0x2000
        TEST.RUN(12)
        TEST.CHECK("CPU.D[3].W", 0x3FFF)
    end,

    z = farray(TF, function(b)
        CPU.D[3].W = cond(b, 0, 1)
        TEST.RUN(18)
        TEST.CHECK("CPU.CC.Z", b)
    end),

    n = farray(TF, function(b)
        CPU.D[3].W = cond(b, 0x8000, 0)
        TEST.RUN(12)
        TEST.CHECK("CPU.CC.N", b)
    end)
}
run.L = {
    value = function()
        CPU.D[4].L = 0x20000000
        TEST.RUN(24)
        TEST.CHECK("CPU.D[4].L", 0x3FFFFFFF)
    end,
    z = farray(TF, function(b)
        CPU.D[4].L = cond(b, 0, 1)
        TEST.RUN(32)
        TEST.CHECK("CPU.CC.Z", b)
    end),
    n = farray(TF, function(b)
        CPU.D[4].L = cond(b, 0x80000000, 0)
        TEST.RUN(24)
        TEST.CHECK("CPU.CC.N", b)
    end)
}

function run.CCR()
    CPU.CC.value = 0
    TEST.RUN(40)
    TEST.CHECK("CPU.CC.X", true)
    TEST.CHECK("CPU.CC.V", true)
    TEST.CHECK("CPU.CC.C", true)
    TEST.CHECK("CPU.CC.N", true)
    TEST.CHECK("CPU.CC.Z", true)
end

run.SR = {
    ng = function() priv_test(46) end,
    ok = function()
        CPU.SR.S = true
        CPU.SR.T = 0
        TEST.RUN(46)
        TEST.CHECK(CPU.SR.I, 7)
    end,
    trace = function() trace_test(46) end
}

return run
