-- CAS2.W %D3:%D4, %D5:%D6, (%A1):(%A2)
MEM[0].W = OCT"0006374"
MEM[2].W = 1 << 15 | 1 << 12 | 5 << 6 | 3
MEM[4].W = 1 << 15 | 2 << 12 | 6 << 6 | 4
MEM[6].W = TEST.BREAK

-- CAS2.L %D4:%D5, %D6:%D7, (%A2):(%A3)
MEM[8].W = OCT"0007374"
MEM[10].W = 1 << 15 | 2 << 12 | 6 << 6 | 4
MEM[12].W = 1 << 15 | 3 << 12 | 7 << 6 | 5
MEM[14].W = TEST.BREAK

JIT_COMPILE(0, 16)

local run = {}
run.W = {
    t = function()
        MEM[0x1000].W = 0x2222
        MEM[0x1010].W = 0x3333
        CPU.A[2] = 0x1000
        CPU.A[3] = 0x1010
        CPU.D[4].W = 0x2222
        CPU.D[5].W = 0x3333
        CPU.D[6].W = 0x3030
        CPU.D[7].W = 0x4040
        TEST.RUN(0)
        TEST.CHECK("MEM[0x1000].W", 0x3030)
        TEST.CHECK("MEM[0x1010].W", 0x4040)
    end,
    f1 = function()
        MEM[0x1000].W = 0x2222
        MEM[0x1010].W = 0x3333
        CPU.A[2] = 0x1000
        CPU.A[3] = 0x1010
        CPU.D[4].W = 0x2227
        CPU.D[5].W = 0x3030
        CPU.D[6].W = 0x3333
        CPU.D[7].W = 0x4040
        TEST.RUN(0)
        TEST.CHECK("CPU.D[4].W", 0x2222)
        TEST.CHECK("CPU.D[5].W", 0x3333)
    end,
    f2 = function()
        MEM[0x1000].W = 0x2222
        MEM[0x1010].W = 0x3333
        CPU.A[2] = 0x1000
        CPU.A[3] = 0x1010
        CPU.D[4].W = 0x2222
        CPU.D[5].W = 0x3030
        CPU.D[6].W = 0x3339
        CPU.D[7].W = 0x4040
        TEST.RUN(0)
        TEST.CHECK("CPU.D[4].W", 0x2222)
        TEST.CHECK("CPU.D[5].W", 0x3333)
    end,
    trace = function()
        MEM[0x1000].W = 0x2222
        MEM[0x1010].W = 0x3333
        CPU.A[2] = 0x1000
        CPU.A[3] = 0x1010
        trace_test(0)
   end,
}

run.L = {
    t = function()
        MEM[0x1000].L = 0x22222222
        MEM[0x1010].L = 0x33333333
        CPU.A[3] = 0x1000
        CPU.A[4] = 0x1010
        CPU.D[5].L = 0x22222222
        CPU.D[6].L = 0x33333333
        CPU.D[7].L = 0x30303030
        CPU.D[8].L = 0x40404040
        TEST.RUN(8)
        TEST.CHECK("MEM[0x1000].L", 0x30303030)
        TEST.CHECK("MEM[0x1010].L", 0x40404040)
    end,
    f1 = function()
        MEM[0x1000].L = 0x22222222
        MEM[0x1010].L = 0x33333333
        CPU.A[3] = 0x1000
        CPU.A[4] = 0x1010
        CPU.D[5].L = 0x22222227
        CPU.D[6].L = 0x30303030
        CPU.D[7].L = 0x33333333
        CPU.D[8].L = 0x40404040
        TEST.RUN(8)
        TEST.CHECK("CPU.D[5].L", 0x22222222)
        TEST.CHECK("CPU.D[6].L", 0x33333333)
    end,
    f2 = function()
        MEM[0x1000].L = 0x22222222
        MEM[0x1010].L = 0x33333333
        CPU.A[3] = 0x1000
        CPU.A[4] = 0x1010
        CPU.D[5].L = 0x22222222
        CPU.D[6].L = 0x30303030
        CPU.D[7].L = 0x33333339
        CPU.D[8].L = 0x40404040
        TEST.RUN(8)
        TEST.CHECK("CPU.D[5].L", 0x22222222)
        TEST.CHECK("CPU.D[6].L", 0x33333333)
    end,
    trace = function()
        MEM[0x1000].L = 0x22222222
        MEM[0x1010].L = 0x33333333
        CPU.A[3] = 0x1000
        CPU.A[4] = 0x1010
        trace_test(8)
   end,
}

return run