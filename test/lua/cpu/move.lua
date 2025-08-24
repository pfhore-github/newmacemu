-- MOVE.B %D2, %D3
MEM[0].W = OCT "010000" | 3 << 9 | 2
MEM[2].W = TEST.BREAK

-- MOVE.B (%A2), %D3
MEM[4].W = OCT "010020" | 3 << 9 | 2
MEM[6].W = TEST.BREAK

-- MOVE.B (%A2)+, %D3
MEM[8].W = OCT "010030" | 3 << 9 | 2
MEM[10].W = TEST.BREAK

-- MOVE.B (%A7)+, %D3
MEM[12].W = OCT "010030" | 3 << 9 | 7
MEM[14].W = TEST.BREAK

-- MOVE.B -(%A2), %D3
MEM[16].W = OCT "010040" | 3 << 9 | 2
MEM[18].W = TEST.BREAK

-- MOVE.B -(%A7), %D3
MEM[20].W = OCT "0010040" | 3 << 9 | 7
MEM[22].W = TEST.BREAK

-- MOVE.B #0x34, %D3
MEM[24].W = OCT "0010074" | 3 << 9
MEM[26].W = 0x34
MEM[28].W = TEST.BREAK

-- MOVE.W %D2, %D3
MEM[30].W = OCT "0030000" | 3 << 9 | 2
MEM[32].W = TEST.BREAK

-- MOVE.W %A2, %D3
MEM[34].W = OCT "0030010" | 3 << 9 | 2
MEM[36].W = TEST.BREAK

-- MOVE.W (%A2), %D3
MEM[38].W = OCT "0030020" | 3 << 9 | 2
MEM[40].W = TEST.BREAK

-- MOVE.W (%A2)+, %D3
MEM[42].W = OCT "0030030" | 3 << 9 | 2
MEM[44].W = TEST.BREAK

-- MOVE.W (%A7)+, %D3
MEM[46].W = OCT "0030030" | 3 << 9 | 7
MEM[48].W = TEST.BREAK

-- MOVE.W -(%A2), %D3
MEM[50].W = OCT "0030040" | 3 << 9 | 2
MEM[52].W = TEST.BREAK

-- MOVE.W -(%A7), %D3
MEM[54].W = OCT "0030040" | 3 << 9 | 7
MEM[56].W = TEST.BREAK

-- MOVE.W #0x3456, %D3
MEM[58].W = OCT "0030074" | 3 << 9
MEM[60].W = 0x3456
MEM[62].W = TEST.BREAK

-- MOVE.W %CCR, %D3
MEM[64].W = OCT "0041300" | 3
MEM[66].W = TEST.BREAK

-- MOVE.W %SR, %D3
MEM[68].W = OCT "0040300" | 3
MEM[70].W = TEST.BREAK

-- MOVE.L %D2, %D3
MEM[72].W = OCT "0020000" | 3 << 9 | 2
MEM[74].W = TEST.BREAK

-- MOVE.L %A2, %D3
MEM[76].W = OCT "0020010" | 3 << 9 | 2
MEM[78].W = TEST.BREAK

-- MOVE.L (%A2), %D3
MEM[80].W = OCT "0020020" | 3 << 9 | 2
MEM[82].W = TEST.BREAK

-- MOVE.L (%A2)+, %D3
MEM[84].W = OCT "0020030" | 3 << 9 | 2
MEM[86].W = TEST.BREAK

-- MOVE.L (%A7)+, %D3
MEM[88].W = OCT "0020030" | 3 << 9 | 7
MEM[90].W = TEST.BREAK

-- MOVE.L -(%A2), %D3
MEM[92].W = OCT "0020040" | 3 << 9 | 2
MEM[94].W = TEST.BREAK

-- MOVE.L -(%A7), %D3
MEM[96].W = OCT "0020040" | 3 << 9 | 7
MEM[98].W = TEST.BREAK

-- MOVE.L #0x34567890, %D3
MEM[100].W = OCT "0020074" | 3 << 9
MEM[102].L = 0x34567890
MEM[106].W = TEST.BREAK

-- MOVE.L %USP, %A3
MEM[108].W = OCT "0047150" | 3
MEM[110].W = TEST.BREAK

-- MOVE.B %D3, (%A2)
MEM[112].W = OCT "0010200" | 2 << 9 | 3
MEM[114].W = TEST.BREAK

-- MOVE.B (0x1000), (0x1200)
MEM[116].W = OCT "0011771"
MEM[118].L = 0x1000
MEM[122].L = 0x1200
MEM[126].W = TEST.BREAK

-- MOVE.W %D3, (%A2)
MEM[128].W = OCT "0030200" | 2 << 9 | 3
MEM[130].W = TEST.BREAK

-- MOVE.W (0x1000), (0x1200)
MEM[132].W = OCT "0031771"
MEM[134].L = 0x1000
MEM[138].L = 0x1200
MEM[142].W = TEST.BREAK

-- MOVE.W %D3, %CCR
MEM[144].W = OCT "0042300" | 3
MEM[146].W = TEST.BREAK

-- MOVE.W %D3, %SR
MEM[148].W = OCT "0043300" | 3
MEM[150].W = TEST.BREAK

-- MOVE.L %D3, (%A2)
MEM[152].W = OCT "0020200" | 2 << 9 | 3
MEM[154].W = TEST.BREAK

-- MOVE.L (0x1000), (0x1200)
MEM[156].W = OCT "0021771"
MEM[158].L = 0x1000
MEM[162].L = 0x1200
MEM[166].W = TEST.BREAK

-- MOVE.L %A3, %USP
MEM[168].W = OCT "0047140" | 3
MEM[170].W = TEST.BREAK
JIT_COMPILE(0, 172)

local run = {}

run.B = {
    n = farray(TF, function(b)
        CPU.D[3].B = cond(b, 0x80, 0)
        TEST.RUN(0)
        TEST.CHECK("CPU.CC.N", b)
    end),
    z = farray(TF, function(b)
        CPU.D[3].B = cond(b, 0, 1)
        TEST.RUN(0)
        TEST.CHECK("CPU.CC.Z", b)
    end),
    dr = function()
        CPU.D[4].L = 0x12345678
        CPU.D[3].L = 0x9ABCDE02
        CPU.CC.C = true
        CPU.CC.V = true
        TEST.RUN(0)
        TEST.CHECK("CPU.D[4].L", 0x12345602)
        TEST.CHECK("CPU.CC.C", false)
        TEST.CHECK("CPU.CC.V", false)
    end,
    mem = function()
        MEM[0x1000].B = 0x34
        CPU.A[3] = 0x1000
        TEST.RUN(4)
        TEST.CHECK("CPU.D[4].B", 0x34)
    end,
    inc = function()
        MEM[0x1000].B = 0x56
        CPU.A[3] = 0x1000
        TEST.RUN(8)
        TEST.CHECK("CPU.D[4].B", 0x56)
        TEST.CHECK("CPU.A[3]", 0x1001)
    end,
    pop = function()
        MEM[0x1000].B = 0x78
        CPU.A[8] = 0x1000
        TEST.RUN(12)
        TEST.CHECK("CPU.D[4].B", 0x78)
        TEST.CHECK("CPU.A[8]", 0x1002)
    end,
    dec = function()
        MEM[0x1000].B = 0x9A
        CPU.A[3] = 0x1001
        TEST.RUN(16)
        TEST.CHECK("CPU.D[4].B", 0x9A)
        TEST.CHECK("CPU.A[3]", 0x1000)
    end,
    dec_sp = function()
        MEM[0x1000].B = 0x9B
        CPU.A[8] = 0x1002
        TEST.RUN(20)
        TEST.CHECK("CPU.D[4].B", 0x9B)
        TEST.CHECK("CPU.A[8]", 0x1000)
    end,
    imm = function()
        TEST.RUN(24)
        TEST.CHECK("CPU.D[4].B", 0x34)
    end,
    to_mem = function()
        CPU.D[4].B = 0xEF
        CPU.A[3] = 0x1000
        TEST.RUN(112)
        TEST.CHECK("MEM[0x1000].B", 0xEF)
    end,
    mem_mem = function()
        MEM[0x1000].B = 0xA9
        TEST.RUN(116)
        TEST.CHECK("MEM[0x1200].B", 0xA9)
    end
}

run.W = {
    n = farray(TF, function(b)
        CPU.D[3].W = cond(b, 0x8000, 0)
        TEST.RUN(30)
        TEST.CHECK("CPU.CC.N", b)
    end),
    z = farray(TF, function(b)
        CPU.D[3].W = cond(b, 0, 1)
        TEST.RUN(30)
        TEST.CHECK("CPU.CC.Z", b)
    end),
    dr = function()
        CPU.D[4].L = 0x12345678
        CPU.D[3].L = 0x9ABCDE02
        CPU.CC.C = true
        CPU.CC.V = true
        TEST.RUN(30)
        TEST.CHECK("CPU.D[4].L", 0x1234DE02)
        TEST.CHECK("CPU.CC.C", false)
        TEST.CHECK("CPU.CC.V", false)
    end,
    ar = function()
        CPU.D[4].L = 0x12345678
        CPU.A[3] = 0x9ABCDE02
        CPU.CC.C = true
        CPU.CC.V = true
        TEST.RUN(34)
        TEST.CHECK("CPU.D[4].L", 0x1234DE02)
    end,
    mem = function()
        MEM[0x1000].W = 0x1234
        CPU.A[3] = 0x1000
        TEST.RUN(38)
        TEST.CHECK("CPU.D[4].W", 0x1234)
    end,
    inc = function()
        MEM[0x1000].W = 0x5643
        CPU.A[3] = 0x1000
        TEST.RUN(42)
        TEST.CHECK("CPU.D[4].W", 0x5643)
        TEST.CHECK("CPU.A[3]", 0x1002)
    end,
    pop = function()
        MEM[0x1000].W = 0x7890
        CPU.A[8] = 0x1000
        TEST.RUN(46)
        TEST.CHECK("CPU.D[4].W", 0x7890)
        TEST.CHECK("CPU.A[8]", 0x1002)
    end,
    dec = function()
        MEM[0x1000].W = 0x9ABC
        CPU.A[3] = 0x1002
        TEST.RUN(50)
        TEST.CHECK("CPU.D[4].W", 0x9ABC)
        TEST.CHECK("CPU.A[3]", 0x1000)
    end,
    dec_sp = function()
        MEM[0x1000].W = 0x9BCD
        CPU.A[8] = 0x1002
        TEST.RUN(54)
        TEST.CHECK("CPU.D[4].W", 0x9BCD)
        TEST.CHECK("CPU.A[8]", 0x1000)
    end,
    imm = function()
        TEST.RUN(58)
        TEST.CHECK("CPU.D[4].W", 0x3456)
    end,
    cr = function()
        CPU.CC.value = 0x1F
        TEST.RUN(64)
        TEST.CHECK("CPU.D[4].W", 0x1F)
    end,
    sr = {
        ng = function() priv_test(68) end,
        ok = function()
            CPU.SR.value = 0x201F
            TEST.RUN(68)
            TEST.CHECK("CPU.D[4].W", 0x201F)
        end
    },
    to_mem = function()
        CPU.D[4].W = 0x4532
        CPU.A[3] = 0x1000
        TEST.RUN(128)
        TEST.CHECK("MEM[0x1000].W", 0x4532)
    end,
    mem_mem = function()
        MEM[0x1000].W = 0x4258
        TEST.RUN(132)
        TEST.CHECK("MEM[0x1200].W", 0x4258)
    end,
    to_cr = function()
        CPU.D[4].W = 0x1F
        TEST.RUN(144)
        TEST.CHECK("CPU.CC.value", 0x1F)
    end,
    to_sr = {
        ng = function() priv_test(148) end,
        ok = function()
            CPU.SR.S = true
            CPU.D[4].W = 0x201F
            TEST.RUN(148)
            TEST.CHECK("CPU.SR.value", 0x201F)
        end
    },
}

run.L = {
    n = farray(TF, function(b)
        CPU.D[3].L = cond(b, 0x80000000, 0)
        TEST.RUN(72)
        TEST.CHECK("CPU.CC.N", b)
    end),
    z = farray(TF, function(b)
        CPU.D[3].L = cond(b, 0, 1)
        TEST.RUN(72)
        TEST.CHECK("CPU.CC.Z", b)
    end),
    dr = function()
        CPU.D[4].L = 0x12345678
        CPU.D[3].L = 0x9ABCDE02
        CPU.CC.C = true
        CPU.CC.V = true
        TEST.RUN(72)
        TEST.CHECK("CPU.D[4].L", 0x9ABCDE02)
        TEST.CHECK("CPU.CC.C", false)
        TEST.CHECK("CPU.CC.V", false)
    end,
    ar = function()
        CPU.D[4].L = 0x12345678
        CPU.A[3] = 0x9ABCDE02
        CPU.CC.C = true
        CPU.CC.V = true
        TEST.RUN(76)
        TEST.CHECK("CPU.D[4].L", 0x9ABCDE02)
    end,
    mem = function()
        MEM[0x1000].L = 0x12345678
        CPU.A[3] = 0x1000
        TEST.RUN(80)
        TEST.CHECK("CPU.D[4].L", 0x12345678)
    end,
    inc = function()
        MEM[0x1000].L = 0x5643210F
        CPU.A[3] = 0x1000
        TEST.RUN(84)
        TEST.CHECK("CPU.D[4].L", 0x5643210F)
        TEST.CHECK("CPU.A[3]", 0x1004)
    end,
    pop = function()
        MEM[0x1000].L = 0x78901234
        CPU.A[8] = 0x1000
        TEST.RUN(88)
        TEST.CHECK("CPU.D[4].L", 0x78901234)
        TEST.CHECK("CPU.A[8]", 0x1004)
    end,
    dec = function()
        MEM[0x1000].L = 0x9ABCDEF0
        CPU.A[3] = 0x1004
        TEST.RUN(92)
        TEST.CHECK("CPU.D[4].L", 0x9ABCDEF0)
        TEST.CHECK("CPU.A[3]", 0x1000)
    end,
    dec_sp = function()
        MEM[0x1000].L = 0x9BCDEF01
        CPU.A[8] = 0x1004
        TEST.RUN(96)
        TEST.CHECK("CPU.D[4].L", 0x9BCDEF01)
        TEST.CHECK("CPU.A[8]", 0x1000)
    end,
    imm = function()
        TEST.RUN(100)
        TEST.CHECK("CPU.D[4].L", 0x34567890)
    end,
    usp = {
        ng = function() priv_test(108) end,
        ok = function()
            CPU.SR.S = true
            CPU.SR.T = 0
            CPU.USP = 0x5F00
            TEST.RUN(108)
            TEST.CHECK("CPU.A[4]", 0x5F00)
        end
    },
    to_mem = function()
        CPU.D[4].L = 0x45329754
        CPU.A[3] = 0x1000
        TEST.RUN(152)
        TEST.CHECK("MEM[0x1000].L", 0x45329754)
    end,
    mem_mem = function()
        MEM[0x1000].L = 0x42581234
        TEST.RUN(156)
        TEST.CHECK("MEM[0x1200].L", 0x42581234)
    end,
    to_usp = {
        ng = function() priv_test(168) end,
        ok = function()
            CPU.SR.S = true
            CPU.A[4] = 0x3000
            TEST.RUN(168)
            TEST.CHECK("CPU.USP", 0x3000)
        end
    },
}
return run
