-- CHK.W %D1, %D4
MEM[0].W = OCT"044601"
MEM[2].W = TEST.BREAK

-- CHK.L %D2, %D4
MEM[4].W = OCT"044402"
MEM[6].W = TEST.BREAK

JIT_COMPILE(0, 8)

local run = {}
run.W = {
   inRage = function()
      CPU.D[5].H = 2000
      CPU.D[2].H = 1000
      TEST.RUN(0)
   end,
   under = function()
      CPU.D[5].H = -300
      CPU.D[2].H = 2000
      TEST.RUN(0, "chk failure")
      TEST.CHECK("CPU.CC.N", true)
   end,
   over = function()
      CPU.D[5].H = 2300
      CPU.D[2].H = 2000
      TEST.RUN(0, "chk failure")
      TEST.CHECK("CPU.CC.N", false)
   end,
}

run.L = {
   inRage = function()
      CPU.D[5].I = 200000
      CPU.D[3].I = 100000
      TEST.RUN(4)
   end,
   under = function()
      CPU.D[5].I = -30000
      CPU.D[3].I = 200000
      TEST.RUN(4, "chk failure")
      TEST.CHECK("CPU.CC.N", true)
   end,
   over = function()
      CPU.D[5].I = 230000
      CPU.D[3].I = 200000
      TEST.RUN(4, "chk failure")
      TEST.CHECK("CPU.CC.N", false)
   end,
}

return run
