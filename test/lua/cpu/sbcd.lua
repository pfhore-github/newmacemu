-- SBCD %D2, %D1
MEM[0].W = OCT"0100400" | 2 << 9 | 1
MEM[2].W = TEST.BREAK

-- SBCD -(%A2), -(%A1)
MEM[4].W = OCT"0100410" | 2 << 9 | 1 
MEM[6].W = TEST.BREAK

JIT_COMPILE(0, 8)

local run = {}

run.reg = {
    value = farray(TF, function(b)
        CPU.CC.Z = true
        CPU.D[2].B = 0x76
        CPU.D[3].B = 0x34
        CPU.CC.X = b
        TEST.RUN(0)
        TEST.CHECK("CPU.D[3].B", cond(b, 0x41, 0x42))
        TEST.CHECK("CPU.CC.Z", false)
    end),
    c1 = function()
        CPU.D[2].B = 0x80
        CPU.D[3].B = 0x22
        CPU.CC.X = false
        TEST.RUN(0)
        TEST.CHECK("CPU.D[3].B", 0x58)
    end,
    c2 = function()
        CPU.D[2].B = 0x87
        CPU.D[3].B = 0x29
        CPU.CC.X = false
        TEST.RUN(0)
        TEST.CHECK("CPU.D[3].B", 0x58)
    end,
    cx = farray(TF, function(b)
        CPU.D[2].B = 0x80
        CPU.D[3].B = cond(b, 0x90, 0x30)
        CPU.CC.X = false
        TEST.RUN(0)
        TEST.CHECK("CPU.CC.C", b)
        TEST.CHECK("CPU.CC.X", b)
    end),
    z = farray(TF, function(b)
        CPU.D[2].B = 0x80
        CPU.D[3].B = cond(b, 0x80, 0x00)
        CPU.CC.X = false
        CPU.CC.Z = true
        TEST.RUN(0)
        TEST.CHECK("CPU.CC.Z", b)
    end),    
}

function run.mem()
    CPU.CC.Z = true
    CPU.A[2] = 0x1001
    CPU.A[3] = 0x1011
    MEM[0x1000].B = 0x76
    MEM[0x1010].B = 0x34
    CPU.CC.X = false
    TEST.RUN(4)
    TEST.CHECK("MEM[0x1010].B", 0x42)
    TEST.CHECK("CPU.A[2]", 0x1000)
    TEST.CHECK("CPU.A[3]", 0x1010)
    TEST.CHECK("CPU.CC.Z", false)
end

return run