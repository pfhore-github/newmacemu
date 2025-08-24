
-- NBCD %D1
MEM[0].W = OCT"044000" | 1
MEM[2].W = TEST.BREAK

JIT_COMPILE(0, 4)

local run = {}
function run.value()
    CPU.D[2].B = 0x34
    CPU.CC.Z = true
    CPU.CC.X = true
    TEST.RUN(0)
    TEST.CHECK("CPU.D[2].B", 0x65)
end

function run.x()
    CPU.D[2].B = 0x30
    CPU.CC.X = false
    TEST.RUN(0)
    TEST.CHECK("CPU.D[2].B", 0x70)
end

function run.z1()
    CPU.CC.Z = true
    CPU.D[2].B = 0
    CPU.CC.X = false
    TEST.RUN(0)
    TEST.CHECK("CPU.CC.Z", true)
end

function run.z2()
    CPU.CC.Z = true
    CPU.D[2].B = 0x99
    CPU.CC.X = true
    TEST.RUN(0)
    TEST.CHECK("CPU.CC.Z", true)
end

function run.cx()
    CPU.D[2].B = 0
    CPU.CC.X = false
    TEST.RUN(0)
    TEST.CHECK("CPU.CC.C", false)
    TEST.CHECK("CPU.CC.X", false)
end
return run