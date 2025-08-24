-- CMP2.B (%A5), %D1
MEM[0].W = OCT "000320" | 5
MEM[2].W = 1 << 12
MEM[4].W = TEST.BREAK

-- CMP2.B (%A5), %A1
MEM[6].W = OCT "000320" | 5
MEM[8].W = (8 + 1) << 12
MEM[10].W = TEST.BREAK

-- CMP2.W (%A5), %D2
MEM[12].W = OCT "001320" | 5
MEM[14].W = 2 << 12
MEM[16].W = TEST.BREAK

-- CMP2.W (%A5), %A2
MEM[18].W = OCT "001320" | 5
MEM[20].W = (8 + 2) << 12
MEM[22].W = TEST.BREAK

-- CMP2.L (%A5), %D3
MEM[24].W = OCT "002320" | 5
MEM[26].W = 3 << 12
MEM[28].W = TEST.BREAK

-- CMP2.L (%A5), %A3
MEM[30].W = OCT "002320" | 5
MEM[32].W = (8 + 3) << 12
MEM[34].W = TEST.BREAK

JIT_COMPILE(0, 36)

local run = {}
function run.pre() CPU.A[6] = 0x1000 end
run.B = {
    pre = function()
        MEM[0x1000].B = 70
        MEM[0x1001].B = 230
    end,
    inRange = function()
        CPU.D[2].B = 150
        TEST.RUN(0)
        TEST.CHECK("CPU.CC.Z", false)
        TEST.CHECK("CPU.CC.C", false)
    end,
    leq = function()
        CPU.D[2].B = 70
        TEST.RUN(0)
        TEST.CHECK("CPU.CC.Z", true)
        TEST.CHECK("CPU.CC.C", false)
    end,
    geq = function()
        CPU.D[2].B = 230
        TEST.RUN(0)
        TEST.CHECK("CPU.CC.Z", true)
        TEST.CHECK("CPU.CC.C", false)
    end,
    under = function()
        CPU.D[2].B = 20
        TEST.RUN(0)
        TEST.CHECK("CPU.CC.C", true)
    end,
    over = function()
        CPU.D[2].B = 250
        TEST.RUN(0)
        TEST.CHECK("CPU.CC.C", true)
    end
}

run.C = {
    pre = function()
        MEM[0x1000].C = -70
        MEM[0x1001].C = 80
    end,
    inRange = function()
        CPU.A[2] = 10
        TEST.RUN(6)
        TEST.CHECK("CPU.CC.Z", false)
        TEST.CHECK("CPU.CC.C", false)
    end,
    leq = function()
        CPU.A[2] = -70
        TEST.RUN(6)
        TEST.CHECK("CPU.CC.Z", true)
        TEST.CHECK("CPU.CC.C", false)
    end,
    geq = function()
        CPU.A[2] = 80
        TEST.RUN(6)
        TEST.CHECK("CPU.CC.Z", true)
        TEST.CHECK("CPU.CC.C", false)
    end,
    under = function()
        CPU.A[2] = -100
        TEST.RUN(6)
        TEST.CHECK("CPU.CC.C", true)
    end,
    over = function()
        CPU.A[2] = 100
        TEST.RUN(6)
        TEST.CHECK("CPU.CC.C", true)
    end
}

run.W = {
    pre = function()
        MEM[0x1000].W = 7000
        MEM[0x1002].W = 23000
    end,
    inRange = function()
        CPU.D[3].W = 15000
        TEST.RUN(12)
        TEST.CHECK("CPU.CC.Z", false)
        TEST.CHECK("CPU.CC.C", false)
    end,
    leq = function()
        CPU.D[3].W = 7000
        TEST.RUN(12)
        TEST.CHECK("CPU.CC.Z", true)
        TEST.CHECK("CPU.CC.C", false)
    end,
    geq = function()
        CPU.D[3].W = 23000
        TEST.RUN(12)
        TEST.CHECK("CPU.CC.Z", true)
        TEST.CHECK("CPU.CC.C", false)
    end,
    under = function()
        CPU.D[3].W = 2000
        TEST.RUN(12)
        TEST.CHECK("CPU.CC.C", true)
    end,
    over = function()
        CPU.D[3].W = 25000
        TEST.RUN(12)
        TEST.CHECK("CPU.CC.C", true)
    end
}

run.H = {
    pre = function()
        MEM[0x1000].H = -700
        MEM[0x1002].H = 800
    end,
    inRange = function()
        CPU.A[3] = 100
        TEST.RUN(18)
        TEST.CHECK("CPU.CC.Z", false)
        TEST.CHECK("CPU.CC.C", false)
    end,
    leq = function()
        CPU.A[3] = -700
        TEST.RUN(18)
        TEST.CHECK("CPU.CC.Z", true)
        TEST.CHECK("CPU.CC.C", false)
    end,
    geq = function()
        CPU.A[3] = 800
        TEST.RUN(18)
        TEST.CHECK("CPU.CC.Z", true)
        TEST.CHECK("CPU.CC.C", false)
    end,
    under = function()
        CPU.A[3] = -1000
        TEST.RUN(18)
        TEST.CHECK("CPU.CC.C", true)
    end,
    over = function()
        CPU.A[3] = 1000
        TEST.RUN(18)
        TEST.CHECK("CPU.CC.C", true)
    end
}

run.L = {
    pre = function()
        MEM[0x1000].L = 700000
        MEM[0x1004].L = 2300000
    end,
    inRange = function()
        CPU.D[4].L = 1500000
        TEST.RUN(24)
        TEST.CHECK("CPU.CC.Z", false)
        TEST.CHECK("CPU.CC.C", false)
    end,
    leq = function()
        CPU.D[4].L = 700000
        TEST.RUN(24)
        TEST.CHECK("CPU.CC.Z", true)
        TEST.CHECK("CPU.CC.C", false)
    end,
    geq = function()
        CPU.D[4].L = 2300000
        TEST.RUN(24)
        TEST.CHECK("CPU.CC.Z", true)
        TEST.CHECK("CPU.CC.C", false)
    end,
    under = function()
        CPU.D[4].L = 200000
        TEST.RUN(24)
        TEST.CHECK("CPU.CC.C", true)
    end,
    over = function()
        CPU.D[4].L = 2500000
        TEST.RUN(24)
        TEST.CHECK("CPU.CC.C", true)
    end
}

run.I = {
    pre = function()
        MEM[0x1000].I = -70000
        MEM[0x1004].I = 80000
    end,
    inRange = function()
        CPU.A[4] = 100
        TEST.RUN(30)
        TEST.CHECK("CPU.CC.Z", false)
        TEST.CHECK("CPU.CC.C", false)
    end,
    leq = function()
        CPU.A[4] = -70000
        TEST.RUN(30)
        TEST.CHECK("CPU.CC.Z", true)
        TEST.CHECK("CPU.CC.C", false)
    end,
    geq = function()
        CPU.A[4] = 80000
        TEST.RUN(30)
        TEST.CHECK("CPU.CC.Z", true)
        TEST.CHECK("CPU.CC.C", false)
    end,
    under = function()
        CPU.A[4] = -100000
        TEST.RUN(30)
        TEST.CHECK("CPU.CC.C", true)
    end,
    over = function()
        CPU.A[4] = 100000
        TEST.RUN(30)
        TEST.CHECK("CPU.CC.C", true)
    end
}

return run
