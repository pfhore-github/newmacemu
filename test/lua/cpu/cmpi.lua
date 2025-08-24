-- CMPI.B #10, %D1
MEM[0].W = OCT "006000" | 1
MEM[2].W = 10
MEM[4].W = TEST.BREAK

-- CMPI.W #30000, %D2
MEM[6].W = OCT "006100" | 2
MEM[8].H = 30000
MEM[10].W = TEST.BREAK

-- CMPI.L #1000000000, %D3
MEM[12].W = OCT "006200" | 3
MEM[14].I = 1000000000
MEM[18].W = TEST.BREAK

JIT_COMPILE(0, 20)
local run = {}
run.B = {
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
    c = farray(TF, function(b)
        CPU.D[2].C = cond(b, 5, 20)
        TEST.RUN(0)
        TEST.CHECK("CPU.CC.C", b)
    end),
    v = farray(TF, function(b)
        CPU.D[2].C = cond(b, -125, 20)
        TEST.RUN(0)
        TEST.CHECK("CPU.CC.V", b)
    end)
}

run.W = {
    z = farray(TF, function(b)
        CPU.D[3].H = cond(b, 30000, 5)
        TEST.RUN(6)
        TEST.CHECK("CPU.CC.Z", b)
    end),
    n = farray(TF, function(b)
        CPU.D[3].H = cond(b, -100, 31000)
        TEST.RUN(6)
        TEST.CHECK("CPU.CC.N", b)
    end),
    c = farray(TF, function(b)
        CPU.D[3].H = cond(b, 20000, 31000)
        TEST.RUN(6)
        TEST.CHECK("CPU.CC.C", b)
    end),
    v = farray(TF, function(b)
        CPU.D[3].H = cond(b, -20000, 20)
        TEST.RUN(6)
        TEST.CHECK("CPU.CC.V", b)
    end)
}

run.L = {
    z = farray(TF, function(b)
        CPU.D[4].I = cond(b, 1000000000, 5)
        TEST.RUN(12)
        TEST.CHECK("CPU.CC.Z", b)
    end),
    n = farray(TF, function(b)
        CPU.D[4].I = cond(b, -100, 1200000000)
        TEST.RUN(12)
        TEST.CHECK("CPU.CC.N", b)
    end),
    c = farray(TF, function(b)
        CPU.D[4].I = cond(b, 900000000, 1200000000)
        TEST.RUN(12)
        TEST.CHECK("CPU.CC.C", b)
    end),
    v = farray(TF, function(b)
        CPU.D[4].I = cond(b, -1500000000, 20)
        TEST.RUN(12)
        TEST.CHECK("CPU.CC.V", b)
    end)
}

return run
