-- NEG.B %D2
MEM[0].W = OCT"042000" | 2
MEM[2].W = TEST.BREAK

-- NEG.W %D3
MEM[4].W = OCT"042100" | 3
MEM[6].W = TEST.BREAK

-- NEG.L %D4
MEM[8].W = OCT"042200" | 4
MEM[10].W = TEST.BREAK

JIT_COMPILE(0, 12)

local run = {}

run.B = {
    value = function()
        CPU.D[3].C = -2
        TEST.RUN(0)
        TEST.CHECK("CPU.D[3].C", 2)
    end,
    v = farray(TF, function(b)
        CPU.D[3].C = cond(b, -128, -127)
        TEST.RUN(0)
        TEST.CHECK("CPU.CC.V", b)
    end),    
    n = farray(TF, function(b)
        CPU.D[3].C = cond(b, 5, -5)
        TEST.RUN(0)
        TEST.CHECK("CPU.CC.N", b)
    end),   
    zcx = farray(TF, function(b)
        CPU.D[3].C = cond(b, 0, 1)
        TEST.RUN(0)
        TEST.CHECK("CPU.CC.Z", b)
        TEST.CHECK("CPU.CC.C", not b)
        TEST.CHECK("CPU.CC.X", not b)
    end),    
}

run.W = {
    value = function()
        CPU.D[4].H = -2000
        TEST.RUN(4)
        TEST.CHECK("CPU.D[4].H", 2000)
   end,
   v = farray(TF, function(b)
        CPU.D[4].H = cond(b, 0x8000, 0x8001)
        TEST.RUN(4)
        TEST.CHECK("CPU.CC.V", b)
    end),    
    n = farray(TF, function(b)
        CPU.D[4].H = cond(b, 5000, -5000)
        TEST.RUN(4)
        TEST.CHECK("CPU.CC.N", b)
    end),   
    zcx = farray(TF, function(b)
        CPU.D[4].H = cond(b, 0, 1)
        TEST.RUN(4)
        TEST.CHECK("CPU.CC.Z", b)
        TEST.CHECK("CPU.CC.C", not b)
        TEST.CHECK("CPU.CC.X", not b)
    end),    
}

run.L = {
    value = function()
        CPU.D[5].I = -2000000
        TEST.RUN(8)
        TEST.CHECK("CPU.D[5].I", 2000000)
    end,

    v = farray(TF, function(b)
        CPU.D[5].I = cond(b, 0x80000000, 0x80000001)
        TEST.RUN(8)
        TEST.CHECK("CPU.CC.V", b)
    end),    
    n = farray(TF, function(b)
        CPU.D[5].I = cond(b, 5000000, -5000000)
        TEST.RUN(8)
        TEST.CHECK("CPU.CC.N", b)
    end),   
    zcx = farray(TF, function(b)
        CPU.D[5].I = cond(b, 0, 1)
        TEST.RUN(8)
        TEST.CHECK("CPU.CC.Z", b)
        TEST.CHECK("CPU.CC.C", not b)
        TEST.CHECK("CPU.CC.X", not b)
    end),    
}
return run