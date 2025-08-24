-- BTST.B #3, (%A4) 
MEM[0].W = OCT"004020" | 4
MEM[2].W = 3
MEM[4].W = TEST.BREAK

-- BTST.B %D2, (%A4)
MEM[6].W = OCT"000420" | 2 << 9 | 4
MEM[8].W = TEST.BREAK

-- BTST.B %D2, #8
MEM[10].W = OCT"000474" | 2 << 9
MEM[12].W = 8
MEM[14].W = TEST.BREAK

-- BTST.B #3, #8
MEM[16].W = OCT"004074"
MEM[18].W = 3
MEM[20].W = 8
MEM[22].W = TEST.BREAK

-- BTST.L #20, %D4
MEM[24].W = OCT"004000" | 4
MEM[26].W = 20
MEM[28].W = TEST.BREAK

-- BTST.L %D2, %D4 
MEM[30].W = OCT"000400" | 2 << 9 | 4
MEM[32].W = TEST.BREAK


JIT_COMPILE(0, 34)

local run = {}
run.B = {
    byImm = farray(TF, function(z)
        MEM[0x1000].B = cond(z, 0, 1 << 3)
        CPU.A[5] = 0x1000
        TEST.RUN(0)
        TEST.CHECK("CPU.CC.Z", z)
    end),
    byImmWithImm = function()
        TEST.RUN(16)
        TEST.CHECK("CPU.CC.Z", false)
    end,
    byReg = function()
        MEM[0x1000].B = 1 << 3
        CPU.A[5] = 0x1000
        CPU.D[3].L = 3
        TEST.RUN(6)
        TEST.CHECK("CPU.CC.Z", false)
    end,
    byRegWithImm = function()
        CPU.D[3].L = 3
        TEST.RUN(10)
        TEST.CHECK("CPU.CC.Z", false)
    end,
}

run.L = {
    byImm = farray(TF, function(z)
        CPU.D[5].L = cond(z, 0, 1 << 20)
        TEST.RUN(24)
        TEST.CHECK("CPU.CC.Z", z)
    end),
    byReg = function()
        CPU.D[5].L = 1 << 20
        CPU.D[3].L = 20
        TEST.RUN(30)
        TEST.CHECK("CPU.CC.Z", false)
    end,
}

return run