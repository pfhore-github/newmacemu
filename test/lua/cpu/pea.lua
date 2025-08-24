-- PEA (%A3)
MEM[0].W = OCT "044120" | 3
MEM[2].W = TEST.BREAK

JIT_COMPILE(0, 4)

local run = {}
function run.value()
    CPU.A[4] = 0x200
    TEST.RUN(0)
    TEST.CHECK("MEM[0x5FFC].L", 0x200)
    TEST.CHECK("CPU.A[8]", 0x5FFC)
end
return run
