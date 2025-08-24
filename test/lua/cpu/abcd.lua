-- ASM(ABCD %D1, %D2)
MEM[0].W = OCT "140400" | 2 << 9 | 1
MEM[2].W = TEST.BREAK

-- ASM(ABCD -(%A1), -(%A2))
MEM[4].W = OCT "140410" | 2 << 9 | 1
MEM[6].W = TEST.BREAK

JIT_COMPILE(0, 8)

local run = {}

run.reg = {
    value = farray(TF, function(b)
        CPU.D[2].B = 0x22
        CPU.D[3].B = 0x39
        CPU.CC.X = b
        TEST.RUN(0)
        TEST.CHECK("CPU.D[3].B", cond(b, 0x62, 0x61))
    end),
    cx = farray(TF, function(b)
        CPU.D[2].B = 0x77
        CPU.D[3].B = cond(b, 0x44, 0x11)
        CPU.CC.X = false
        TEST.RUN(0)
        TEST.CHECK("CPU.CC.C", b)
        TEST.CHECK("CPU.CC.X", b)
    end),
    z = farray(TF, function(b)
        CPU.D[2].B = 0x99
        CPU.D[3].B = cond(b, 0x01, 0x00)
        CPU.CC.X = false
        CPU.CC.Z = true
        TEST.RUN(0)
        TEST.CHECK("CPU.CC.Z", b)
    end)
}

function run.mem()
    CPU.CC.Z = true
    CPU.A[2] = 0x1001
    CPU.A[3] = 0x1011
    MEM[0x1000].B = 0x22
    MEM[0x1010].B = 0x34
    CPU.CC.X = false
    TEST.RUN(4)
    TEST.CHECK("MEM[0x1010].B", 0x56)
    TEST.CHECK("CPU.A[2]", 0x1000)
    TEST.CHECK("CPU.A[3]", 0x1010)
    TEST.CHECK("CPU.CC.Z", false)
end

return run
