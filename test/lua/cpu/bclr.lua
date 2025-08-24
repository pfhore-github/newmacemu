-- BCLR.B #3, (%A4)
MEM[0].W = OCT "004220" | 4
MEM[2].W = 3
MEM[4].W = TEST.BREAK

-- BCLR.B %D2, (%A4)
MEM[6].W = OCT "000620" | 2 << 9 | 4
MEM[8].W = TEST.BREAK

-- BCLR.L #20, %D4
MEM[10].W = OCT "004200" | 4
MEM[12].W = 20
MEM[14].W = TEST.BREAK

-- BCLR.L %D2, %D4
MEM[16].W = OCT "000600" | 2 << 9 | 4
MEM[18].W = TEST.BREAK

JIT_COMPILE(0, 20)
local run = {}

run.B = {
	pre = function()
        CPU.A[5] = 0x1000
	end,
    byImm = farray(TF, function(z)
        MEM[0x1000].B = cond(z, 0, 1 << 3)
        TEST.RUN(0)
        TEST.CHECK("CPU.CC.Z", z)
        TEST.CHECK("MEM[0x1000].B", 0)
    end),
    byReg = function()
        MEM[0x1000].B = 1 << 3
        CPU.D[3].L = 3
        TEST.RUN(6)
        TEST.CHECK("CPU.CC.Z", false)
        TEST.CHECK("MEM[0x1000].B", 0)
    end
}

run.L = {
    byImm = farray(TF, function(z)
        CPU.D[5].L = cond(z, 0, 1 << 20)
        TEST.RUN(10)
        TEST.CHECK("CPU.CC.Z", z)
        TEST.CHECK("CPU.D[5].L", 0)
    end),
    byReg = function()
        CPU.D[5].L = 1 << 20
        CPU.D[3].L = 20
        TEST.RUN(16)
        TEST.CHECK("CPU.CC.Z", false)
        TEST.CHECK("CPU.D[5].L", 0)
    end
}
return run
