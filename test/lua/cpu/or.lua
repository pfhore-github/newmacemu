SRC = math.random(1, 8)
repeat
   DST = math.random(1, 8)
until DST ~= SRC

ADDR = math.random(1, 7) -- don't use SP

-- OR.B %Dx, %Dy
MEM[0].W = OCT"0100000" | ((DST-1) << 9) | (SRC-1)
MEM[2].W = TEST.BREAK

-- OR.W %Dx, %Dy
MEM[4].W = OCT"0100100" | ((DST-1) << 9) | (SRC-1)
MEM[6].W = TEST.BREAK

-- OR.L %Dx, %Dy
MEM[8].W = OCT"0100200" | ((DST-1) << 9) | (SRC-1)
MEM[10].W = TEST.BREAK

-- OR.B %Dx, (%Az)
MEM[12].W = OCT"0100420" | ((SRC-1) << 9) | (ADDR-1)
MEM[14].W = TEST.BREAK

-- OR.W %Dx, (%Az)
MEM[16].W = OCT"0100520" | ((SRC-1) << 9) | (ADDR-1)
MEM[18].W = TEST.BREAK

-- OR.L %Dx, (%Az)
MEM[20].W = OCT"0100620" | ((SRC-1) << 9) | (ADDR-1)
MEM[22].W = TEST.BREAK

JIT_COMPILE(0, 24)

run = { Byte = {ToReg = {}, ToMem = {}},
		Word = {ToReg = {}, ToMem = {}},
		Long = {ToReg = {}, ToMem = {}},
	  }
function run.Byte.ToReg.value()
   CPU.D[DST].B = 0x30
   CPU.D[SRC].B = 0x2F
   TEST.RUN(0)

   TEST.CHECK ("CPU.D[DST].B", 0x3F)
   TEST.CHECK ("CPU.CC.V", false)
   TEST.CHECK ("CPU.CC.C", false)
   TEST.CHECK ("CPU.CC.N", false)
   TEST.CHECK ("CPU.CC.Z", false)
end
function run.Byte.ToReg.n()
   CPU.D[DST].B = 0x80
   CPU.D[SRC].B = 0
   TEST.RUN(0)
   TEST.CHECK ("CPU.CC.N", true)
end

function run.Byte.ToReg.z()
   CPU.D[DST].B = 0
   CPU.D[SRC].B = 0
   TEST.RUN(0)
   TEST.CHECK ("CPU.CC.Z", true)
end

function run.Byte.ToMem.value()
   CPU.D[SRC].B = 0x2F
   CPU.A[ADDR].L = 0x1000
   MEM[0x1000].B = 0x30
   TEST.RUN(12)
   TEST.CHECK ("MEM[0x1000].B", 0x3F)
   TEST.CHECK ("CPU.CC.V", false)
   TEST.CHECK ("CPU.CC.C", false)
   TEST.CHECK ("CPU.CC.N", false)
   TEST.CHECK ("CPU.CC.Z", false)
end

function run.Byte.ToMem.n()
   CPU.D[SRC].B = 0
   CPU.A[ADDR].L = 0x1000
   MEM[0x1000].B = 0x80
   TEST.RUN(12)
   TEST.CHECK( "CPU.CC.N", true)
end

function run.Byte.ToMem.z()
   CPU.D[SRC].B = 0
   CPU.A[ADDR].L = 0x1000
   MEM[0x1000].B = 0
   TEST.RUN(12)
   TEST.CHECK( "CPU.CC.Z", true)
end

function run.Word.ToReg.value()
   CPU.D[DST].W = 0x3030
   CPU.D[SRC].W = 0x2F2F
   TEST.RUN(4)

   TEST.CHECK ("CPU.D[DST].W", 0x3F3F)
   TEST.CHECK ("CPU.CC.V", false)
   TEST.CHECK ("CPU.CC.C", false)
   TEST.CHECK ("CPU.CC.N", false)
   TEST.CHECK ("CPU.CC.Z", false)
end

function run.Word.ToReg.n()
   CPU.D[DST].W = 0x8000
   CPU.D[SRC].W = 0
   TEST.RUN(4)
   TEST.CHECK ("CPU.CC.N", true)
end

function run.Word.ToReg.z()
   CPU.D[DST].W = 0
   CPU.D[SRC].W = 0
   TEST.RUN(4)
   TEST.CHECK ("CPU.CC.Z", true)
end

function run.Word.ToMem.value()
   CPU.D[SRC].W = 0x2F2F
   CPU.A[ADDR].L = 0x1000
   MEM[0x1000].W = 0x3030
   TEST.RUN(16)
   TEST.CHECK ("MEM[0x1000].W", 0x3F3F)
   TEST.CHECK ("CPU.CC.V", false)
   TEST.CHECK ("CPU.CC.C", false)
   TEST.CHECK ("CPU.CC.N", false)
   TEST.CHECK ("CPU.CC.Z", false)
end

function run.Word.ToMem.n()
   CPU.D[SRC].W = 0
   CPU.A[ADDR].L = 0x1000
   MEM[0x1000].W = 0x8000
   TEST.RUN(16)
   TEST.CHECK( "CPU.CC.N", true)
end

function run.Word.ToMem.z()
   CPU.D[SRC].W = 0
   CPU.A[ADDR].L = 0x1000
   MEM[0x1000].W = 0
   TEST.RUN(16)
   TEST.CHECK( "CPU.CC.Z", true)
end

function run.Long.ToReg.value()
   CPU.D[DST].L = 0x30303030
   CPU.D[SRC].L = 0x2F2F2F2F
   TEST.RUN(8)

   TEST.CHECK ("CPU.D[DST].L", 0x3F3F3F3F)
   TEST.CHECK ("CPU.CC.V", false)
   TEST.CHECK ("CPU.CC.C", false)
   TEST.CHECK ("CPU.CC.N", false)
   TEST.CHECK ("CPU.CC.Z", false)
end
function run.Long.ToReg.n()
   CPU.D[DST].L = 0x80000000
   CPU.D[SRC].L = 0
   TEST.RUN(8)
   TEST.CHECK ("CPU.CC.N", true)
end

function run.Long.ToReg.z()
   CPU.D[DST].L = 0
   CPU.D[SRC].L = 0
   TEST.RUN(8)
   TEST.CHECK ("CPU.CC.Z", true)
end

function run.Long.ToMem.value()
   CPU.D[SRC].L = 0x2F2F2F2F
   CPU.A[ADDR].L = 0x1000
   MEM[0x1000].L = 0x30303030
   TEST.RUN(20)
   TEST.CHECK ("MEM[0x1000].L", 0x3F3F3F3F)
   TEST.CHECK ("CPU.CC.V", false)
   TEST.CHECK ("CPU.CC.C", false)
   TEST.CHECK ("CPU.CC.N", false)
   TEST.CHECK ("CPU.CC.Z", false)
end

function run.Long.ToMem.n()
   CPU.D[SRC].L = 0
   CPU.A[ADDR].L = 0x1000
   MEM[0x1000].L = 0x80000000
   TEST.RUN(20)
   TEST.CHECK( "CPU.CC.N", true)
end

function run.Long.ToMem.z()
   CPU.D[SRC].L = 0
   CPU.A[ADDR].L = 0x1000
   MEM[0x1000].L = 0
   TEST.RUN(20)
   TEST.CHECK( "CPU.CC.Z", true)
end
