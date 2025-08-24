-- NEGX.B %D2
MEM[0].W = OCT"0040000" | 2
MEM[2].W = TEST.BREAK

-- NEGX.W %D3
MEM[4].W = OCT"0040100" | 3 
MEM[6].W = TEST.BREAK

-- NEGX.L %D4
MEM[8].W = OCT"0040200" | 4
MEM[10].W = TEST.BREAK

JIT_COMPILE(0, 12)
local run = {}

run.B = {
   value = farray(TF, function(b)
      CPU.D[3].C = -2
      CPU.CC.X = b
      TEST.RUN(0)
      TEST.CHECK("CPU.D[3].C", cond(b, 1, 2))
   end),  
   v = farray(TF, function(b)
      CPU.D[3].C = cond(b, -128, -127)
      CPU.CC.X = false
      TEST.RUN(0)
      TEST.CHECK("CPU.CC.V", b)
   end),    
   n = farray(TF, function(b)
      CPU.D[3].C = cond(b, 5, -5)
      CPU.CC.X = false
      TEST.RUN(0)
      TEST.CHECK("CPU.CC.N", b)
   end),   
   zcx = farray(TF, function(b)
      CPU.D[3].C = cond(b, 0, 1)
      CPU.CC.X = false
      TEST.RUN(0)
      TEST.CHECK("CPU.CC.Z", b)
      TEST.CHECK("CPU.CC.C", not b)
      TEST.CHECK("CPU.CC.X", not b)
   end),    
}

run.W = {
   value = farray(TF, function(b)
      CPU.D[4].H = -500
      CPU.CC.X = b
      TEST.RUN(4)
      TEST.CHECK("CPU.D[4].H", cond(b, 499, 500))
   end),  
   v = farray(TF, function(b)
      CPU.D[4].H = cond(b, 0x8000, 0x8001)
      CPU.CC.X = false
      TEST.RUN(4)
      TEST.CHECK("CPU.CC.V", b)
   end),    
   n = farray(TF, function(b)
      CPU.D[4].H = cond(b, 5000, -5000)
      CPU.CC.X = false
      TEST.RUN(4)
      TEST.CHECK("CPU.CC.N", b)
   end),   
   zcx = farray(TF, function(b)
      CPU.D[4].H = cond(b, 0, 1)
      CPU.CC.X = false
      TEST.RUN(4)
      TEST.CHECK("CPU.CC.Z", b)
      TEST.CHECK("CPU.CC.C", not b)
      TEST.CHECK("CPU.CC.X", not b)
   end),    
}

run.L = {
   value = farray(TF, function(b)
      CPU.D[5].I = -500000
      CPU.CC.X = b
      TEST.RUN(8)
      TEST.CHECK("CPU.D[5].I", cond(b, 499999, 500000))
   end),  
   v = farray(TF, function(b)
      CPU.D[5].I = cond(b, 0x80000000, 0x80000001)
      CPU.CC.X = false
      TEST.RUN(8)
      TEST.CHECK("CPU.CC.V", b)
   end),    
   n = farray(TF, function(b)
      CPU.D[5].I = cond(b, 5000000, -5000000)
      CPU.CC.X = false
      TEST.RUN(8)
      TEST.CHECK("CPU.CC.N", b)
   end),   
   zcx = farray(TF, function(b)
      CPU.D[5].I = cond(b, 0, 1)
      CPU.CC.X = false
      TEST.RUN(8)
      TEST.CHECK("CPU.CC.Z", b)
      TEST.CHECK("CPU.CC.C", not b)
      TEST.CHECK("CPU.CC.X", not b)
   end),    
}

return run