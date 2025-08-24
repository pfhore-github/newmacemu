-- TST.B %D2
MEM[0].W = OCT"045000" | 2
MEM[2].W = TEST.BREAK

-- TST.W %D3
MEM[4].W = OCT"045103" | 3
MEM[6].W = TEST.BREAK

-- TST.L %D4
MEM[8].W = OCT"045204" | 4
MEM[10].W = TEST.BREAK

JIT_COMPILE(0, 12)

local run = {}

run.B = {
    n = farray(TF, function(b)
            CPU.D[3].B = cond(b, 0x80, 0)
            TEST.RUN(0)
            TEST.CHECK ("CPU.CC.N", b)
            TEST.CHECK ("CPU.CC.V", false)
            TEST.CHECK ("CPU.CC.C", false)
        end),
    z = farray(TF, function(b)
            CPU.D[3].B = cond(b, 0, 1)
            TEST.RUN(0)
            TEST.CHECK ("CPU.CC.Z", b)
        end),
}

run.W = {
    n = farray(TF, function(b)
            CPU.D[4].W = cond(b, 0x8000, 0)
            TEST.RUN(4)
            TEST.CHECK ("CPU.CC.N", b)
            TEST.CHECK ("CPU.CC.V", false)
            TEST.CHECK ("CPU.CC.C", false)
        end),
    z = farray(TF, function(b)
            CPU.D[4].W = cond(b, 0, 1)
            TEST.RUN(4)
            TEST.CHECK ("CPU.CC.Z", b)
        end),
}

run.L = {
    n = farray(TF, function(b)
            CPU.D[5].L = cond(b, 0x80000000, 0)
            TEST.RUN(8)
            TEST.CHECK ("CPU.CC.N", b)
            TEST.CHECK ("CPU.CC.V", false)
            TEST.CHECK ("CPU.CC.C", false)
        end),
    z = farray(TF, function(b)
            CPU.D[5].L = cond(b, 0, 1)
            TEST.RUN(8)
            TEST.CHECK ("CPU.CC.Z", b)
        end),
}
return run