-- CAS.B %D1, %D2, (%A3)
MEM[0].W = OCT"0005320" | 3
MEM[2].W = 2 << 6 | 1
MEM[4].W = TEST.BREAK

-- CAS.W %D2, %D3, (%A4)
MEM[6].W = OCT"0006320" | 4
MEM[8].W = 3 << 6 | 2
MEM[10].W = TEST.BREAK

-- CAS.L %D3, %D4, (%A5)
MEM[12].W = OCT"0007320" | 5
MEM[14].W = 4 << 6 | 3
MEM[16].W = TEST.BREAK

JIT_COMPILE(0, 18)

local run = {}

run.B = {
   pre = function()
      MEM[0x1000].B = 0x22
      CPU.A[4] = 0x1000
      CPU.D[3].B = 0x30
   end,
   t = function()
      CPU.D[2].B = 0x22
      TEST.RUN(0)
      TEST.CHECK("MEM[0x1000].B", 0x30)
   end,
   f = function()
      CPU.D[2].B = 0x55
      TEST.RUN(0)
      TEST.CHECK("CPU.D[2].B", 0x22)
   end,
   trace = function()
      CPU.A[4] = 0x1000
      trace_test(0)
   end,
}

run.W = {
   pre = function()
      MEM[0x1000].W = 0x2222
      CPU.A[5] = 0x1000
      CPU.D[4].W = 0x3030
   end,
   t = function()
      CPU.D[3].W = 0x2222
      TEST.RUN(6)
      TEST.CHECK("MEM[0x1000].W", 0x3030)
   end,
   f = function()
      CPU.D[3].W = 0x5555
      TEST.RUN(6)
      TEST.CHECK("CPU.D[3].W", 0x2222)
   end,
   trace = function()
      CPU.A[5] = 0x1000
      trace_test(6)
   end,
}

run.L = {
   pre = function()
      MEM[0x1000].L = 0x22222222
      CPU.A[6] = 0x1000
      CPU.D[5].L = 0x30303030
   end,
   t = function()
      CPU.D[4].L = 0x22222222
      TEST.RUN(12)
      TEST.CHECK("MEM[0x1000].L", 0x30303030)
   end,
   f = function()
      CPU.D[4].L = 0x55555555
      TEST.RUN(12)
      TEST.CHECK("CPU.D[4].L", 0x22222222)
   end,
   trace = function()
      CPU.A[6] = 0x1000
      trace_test(12)
   end,
}

return run