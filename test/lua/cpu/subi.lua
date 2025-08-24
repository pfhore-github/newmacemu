-- SUBI.B #10, %D1
MEM[0].W = OCT "002000" | 1
MEM[2].W = 10
MEM[4].W = TEST.BREAK

-- SUBI.W #30000, %D2
MEM[6].W = OCT "002100" | 2
MEM[8].H = 30000
MEM[10].W = TEST.BREAK

-- SUBI.L #1000000000, %D3
MEM[12].W = OCT "002200" | 3
MEM[14].I = 1000000000
MEM[18].W = TEST.BREAK

JIT_COMPILE(0, 20)

local run = {}
run.B = {
    value = function()
        CPU.D[2].C = 120
        TEST.RUN(0)
        TEST.CHECK("CPU.D[2].C", 110)
    end,
    z = farray(TF, function(b)
        CPU.D[2].C = cond(b, 10, 5)
        TEST.RUN(0)
        TEST.CHECK("CPU.CC.Z", b)
    end),
    n = farray(TF, function(b)
        CPU.D[2].C = cond(b, -10, 100)
        TEST.RUN(0)
        TEST.CHECK("CPU.CC.N", b)
    end),
    cx = farray(TF, function(b)
        CPU.D[2].C = cond(b, 5, 20)
        TEST.RUN(0)
        TEST.CHECK("CPU.CC.C", b)
        TEST.CHECK("CPU.CC.X", b)
    end),
    v = farray(TF, function(b)
        CPU.D[2].C = cond(b, -125, 20)
        TEST.RUN(0)
        TEST.CHECK("CPU.CC.V", b)
    end)
}

run.W = {
    value = function()
        CPU.D[3].H = 31000
        TEST.RUN(6)
        TEST.CHECK("CPU.D[3].H", 1000)
    end,
    z = farray(TF, function(b)
        CPU.D[3].H = cond(b, 30000, 0)
        TEST.RUN(6)
        TEST.CHECK("CPU.CC.Z", b)
    end),
    n = farray(TF, function(b)
        CPU.D[3].H = cond(b, -1000, 40000)
        TEST.RUN(6)
        TEST.CHECK("CPU.CC.N", b)
    end),
    cx = farray(TF, function(b)
        CPU.D[3].H = cond(b, 20000, 40000)
        TEST.RUN(6)
        TEST.CHECK("CPU.CC.C", b)
        TEST.CHECK("CPU.CC.X", b)
    end),
    v = farray(TF, function(b)
        CPU.D[3].H = cond(b, -20000, 0)
        TEST.RUN(6)
        TEST.CHECK("CPU.CC.V", b)
    end)
}

run.L = {
    value = function()
        CPU.D[4].I = 1100000000
        TEST.RUN(12)
        TEST.CHECK("CPU.D[4].I", 100000000)
    end,
    z = farray(TF, function(b)
        CPU.D[4].I = cond(b, 1000000000, 0)
        TEST.RUN(12)
        TEST.CHECK("CPU.CC.Z", b)
    end),
    n = farray(TF, function(b)
        CPU.D[4].I = cond(b, -100, 1300000000)
        TEST.RUN(12)
        TEST.CHECK("CPU.CC.N", b)
    end),
    cx = farray(TF, function(b)
        CPU.D[4].I = cond(b, 900000000, 1300000000)
        TEST.RUN(12)
        TEST.CHECK("CPU.CC.C", b)
        TEST.CHECK("CPU.CC.X", b)
    end),
    v = farray(TF, function(b)
        CPU.D[4].I = cond(b, -1500000000, 0)
        TEST.RUN(12)
        TEST.CHECK("CPU.CC.V", b)
    end)
}

return run
