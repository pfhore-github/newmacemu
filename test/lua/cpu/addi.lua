-- ADDI.B #10, %D1
MEM[0].W = OCT "003000" | 1
MEM[2].W = 10
MEM[4].W = TEST.BREAK

-- ADDI.W #30000, %D2
MEM[6].W = OCT "003100" | 2
MEM[8].H = 30000
MEM[10].W = TEST.BREAK

-- ADDI.L #1000000000, %D3
MEM[12].W = OCT "003200" | 3
MEM[14].I = 1000000000
MEM[18].W = TEST.BREAK

JIT_COMPILE(0, 20)

local run = {}

run.B = {
    value = function()
        CPU.D[2].C = 50
        TEST.RUN(0)
        TEST.CHECK("CPU.D[2].C", 60)
    end,
    z = farray(TF, function(b)
        CPU.D[2].C = cond(b, -10, 5)
        TEST.RUN(0)
        TEST.CHECK("CPU.CC.Z", b)
    end),
    n = farray(TF, function(b)
        CPU.D[2].C = cond(b, -20, 10)
        TEST.RUN(0)
        TEST.CHECK("CPU.CC.N", b)
    end),
    cx = farray(TF, function(b)
        CPU.D[2].C = cond(b, -1, 20)
        TEST.RUN(0)
        TEST.CHECK("CPU.CC.C", b)
        TEST.CHECK("CPU.CC.X", b)
    end),
    v = farray(TF, function(b)
        CPU.D[2].C = cond(b, 120, 20)
        TEST.RUN(0)
        TEST.CHECK("CPU.CC.V", b)
    end)
}

run.W = {
    value = function()
        CPU.D[3].H = 2000
        TEST.RUN(6)
        TEST.CHECK("CPU.D[3].H", 32000)
    end,
    z = farray(TF, function(b)
        CPU.D[3].H = cond(b, -30000, 5)
        TEST.RUN(6)
        TEST.CHECK("CPU.CC.Z", b)
    end),
    n = farray(TF, function(b)
        CPU.D[3].H = cond(b, -31000, 10)
        TEST.RUN(6)
        TEST.CHECK("CPU.CC.N", b)
    end),
    cx = farray(TF, function(b)
        CPU.D[3].H = cond(b, -1, 2000)
        TEST.RUN(6)
        TEST.CHECK("CPU.CC.C", b)
        TEST.CHECK("CPU.CC.X", b)
    end),
    v = farray(TF, function(b)
        CPU.D[3].H = cond(b, 3000, 20)
        TEST.RUN(6)
        TEST.CHECK("CPU.CC.V", b)
    end)
}

run.L = {
    value = function()
        CPU.D[4].I = 500000000
        TEST.RUN(12)
        TEST.CHECK("CPU.D[4].I", 1500000000)
    end,
    z = farray(TF, function(b)
        CPU.D[4].I = cond(b, -1000000000, 5)
        TEST.RUN(12)
        TEST.CHECK("CPU.CC.Z", b)
    end),
    n = farray(TF, function(b)
        CPU.D[4].I = cond(b, -2000000000, 10)
        TEST.RUN(12)
        TEST.CHECK("CPU.CC.N", b)
    end),
    cx = farray(TF, function(b)
        CPU.D[4].I = cond(b, -1, 3)
        TEST.RUN(12)
        TEST.CHECK("CPU.CC.C", b)
        TEST.CHECK("CPU.CC.X", b)
    end),
    v = farray(TF, function(b)
        CPU.D[4].I = cond(b, 0x7FFFFFFF, 20)
        TEST.RUN(12)
        TEST.CHECK("CPU.CC.V", b)
    end)
}
return run
