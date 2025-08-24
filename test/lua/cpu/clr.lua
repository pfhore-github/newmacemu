-- CLR.B %D2
MEM[0].W = OCT"041000" | 2
MEM[2].W = TEST.BREAK

-- CLR.W %D3
MEM[4].W = OCT"041100" | 3
MEM[6].W = TEST.BREAK

-- CLR.L %D4
MEM[8].W = OCT"041200" | 4
MEM[10].W = TEST.BREAK
JIT_COMPILE(0, 12)

local run = {}

function run.B()
    CPU.D[3].L = 0x12345678
    TEST.RUN(0)
    TEST.CHECK("CPU.D[3].L", 0x12345600)
    TEST.CHECK("CPU.CC.Z", true)
    TEST.CHECK("CPU.CC.V", false)
    TEST.CHECK("CPU.CC.N", false)
    TEST.CHECK("CPU.CC.C", false)
end

function run.W()
    CPU.D[4].L = 0x12345678
    TEST.RUN(4)
    TEST.CHECK("CPU.D[4].L", 0x12340000)
    TEST.CHECK("CPU.CC.Z", true)
    TEST.CHECK("CPU.CC.V", false)
    TEST.CHECK("CPU.CC.N", false)
    TEST.CHECK("CPU.CC.C", false)
end


function run.L()
    CPU.D[5].L = 0x12345678
    TEST.RUN(8)
    TEST.CHECK("CPU.D[5].L", 0)
    TEST.CHECK("CPU.CC.Z", true)
    TEST.CHECK("CPU.CC.V", false)
    TEST.CHECK("CPU.CC.N", false)
    TEST.CHECK("CPU.CC.C", false)
end
return run