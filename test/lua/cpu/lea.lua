-- LEA (%A3), %A5
MEM[0].W = OCT "040720" | 5 << 9 | 3
MEM[2].W = TEST.BREAK

-- LEA (0x10, %A3), %A5
MEM[4].W = OCT "040750" | 5 << 9 | 3
MEM[6].W = 0x10
MEM[8].W = TEST.BREAK

-- LEA (0x10, %A3, %D2.W), %A5
MEM[10].W = OCT "040760" | 5 << 9 | 3
MEM[12].W = 2 << 12 | 0 << 11 | 0x10
MEM[14].W = TEST.BREAK

for sc = 0, 3 do
    -- LEA (0x10, %A3, %D2*sc), %A5
    MEM[16 + 6 * sc].W = OCT "040760" | 5 << 9 | 3
    MEM[18 + 6 * sc].W = 2 << 12 | 1 << 11 | sc << 9 | 0x10
    MEM[20 + 6 * sc].W = TEST.BREAK
end
-- LEA(%A3, %D2), %A5
MEM[40].W = OCT "040760" | 5 << 9 | 3
MEM[42].W = 2 << 12 | 1 << 11 | 1 << 8 | 1 << 4
MEM[44].W = TEST.BREAK

-- LEA(%10.W, %A3, %D2), %A5
MEM[46].W = OCT "040760" | 5 << 9 | 3
MEM[48].W = 2 << 12 | 1 << 11 | 1 << 8 | 2 << 4
MEM[50].W = 0x10
MEM[52].W = TEST.BREAK

-- LEA(%10.L, %A3, %D2), %A5
MEM[54].W = OCT "040760" | 5 << 9 | 3
MEM[56].W = 2 << 12 | 1 << 11 | 1 << 8 | 3 << 4
MEM[58].L = 0x10
MEM[62].W = TEST.BREAK

-- LEA(%10.W, %D2), %A5
MEM[64].W = OCT "040760" | 5 << 9 | 3
MEM[66].W = 2 << 12 | 1 << 11 | 1 << 8 | 1 << 7 | 2 << 4
MEM[68].W = 0x10
MEM[70].W = TEST.BREAK

-- LEA(%10.W, %A3), %A5
MEM[72].W = OCT "040760" | 5 << 9 | 3
MEM[74].W = 2 << 12 | 1 << 11 | 1 << 8 | 1 << 6 | 2 << 4
MEM[76].W = 0x10
MEM[78].W = TEST.BREAK

-- LEA ([0x10.W, %A3, %D2]), %A5
MEM[80].W = OCT "040760" | 5 << 9 | 3
MEM[82].W = 2 << 12 | 1 << 11 | 1 << 8 | 2 << 4 | 1
MEM[84].W = 0x10
MEM[86].W = TEST.BREAK

-- LEA ([0x10.W, %A3, %D2], 0x30.W), %A5
MEM[88].W = OCT "040760" | 5 << 9 | 3
MEM[90].W = 2 << 12 | 1 << 11 | 1 << 8 | 2 << 4 | 2
MEM[92].W = 0x10
MEM[94].W = 0x30
MEM[96].W = TEST.BREAK

-- LEA ([0x10.W, %A3, %D2], 0x30.L), %A5
MEM[98].W = OCT "040760" | 5 << 9 | 3
MEM[100].W = 2 << 12 | 1 << 11 | 1 << 8 | 2 << 4 | 3
MEM[102].W = 0x10
MEM[104].L = 0x30
MEM[108].W = TEST.BREAK

-- LEA ([0x10.W, %A3], %D2), %A5
MEM[110].W = OCT "040760" | 5 << 9 | 3
MEM[112].W = 2 << 12 | 1 << 11 | 1 << 8 | 2 << 4 | 1 << 2 | 1
MEM[114].W = 0x10
MEM[116].W = TEST.BREAK

-- LEA ([0x10.W, %A3], %D2, 0x30.W), %A5
MEM[118].W = OCT "040760" | 5 << 9 | 3
MEM[120].W = 2 << 12 | 1 << 11 | 1 << 8 | 2 << 4 | 1 << 2 | 2
MEM[122].W = 0x10
MEM[124].W = 0x30
MEM[126].W = TEST.BREAK

-- LEA ([0x10.W, %A3], %D2, 0x30.L), %A5
MEM[128].W = OCT "040760" | 5 << 9 | 3
MEM[130].W = 2 << 12 | 1 << 11 | 1 << 8 | 2 << 4 | 1 << 2 | 3
MEM[132].W = 0x10
MEM[134].L = 0x30
MEM[138].W = TEST.BREAK

-- LEA (0x100).W, %A5
MEM[140].W = OCT "040770" | 5 << 9
MEM[142].W = 0x100
MEM[144].W = TEST.BREAK

-- LEA (0x100).L, %A5
MEM[146].W = OCT "040771" | 5 << 9
MEM[148].L = 0x100
MEM[152].W = TEST.BREAK

-- LEA (0x10, %PC), %A5
MEM[154].W = OCT "040772" | 5 << 9
MEM[156].W = 0x10
MEM[158].W = TEST.BREAK

-- LEA (0x10, %PC, %D2.W), %A5
MEM[160].W = OCT "040773" | 5 << 9
MEM[162].W = 2 << 12 | 0 << 11 | 0x10
MEM[164].W = TEST.BREAK

for sc = 0, 3 do
    -- LEA (0x10, %PC, %D2*sc), %A5
    MEM[166 + 6 * sc].W = OCT "040773" | 5 << 9
    MEM[168 + 6 * sc].W = 2 << 12 | 1 << 11 | sc << 9 | 0x10
    MEM[170 + 6 * sc].W = TEST.BREAK
end

-- LEA (%PC, %D2), %A5
MEM[190].W = OCT "040773" | 5 << 9
MEM[192].W = 2 << 12 | 1 << 11 | 1 << 8 | 1 << 4
MEM[194].W = TEST.BREAK

-- LEA (%10.W, %PC, %D2), %A5
MEM[196].W = OCT "040773" | 5 << 9
MEM[198].W = 2 << 12 | 1 << 11 | 1 << 8 | 2 << 4
MEM[200].W = 0x10
MEM[202].W = TEST.BREAK

-- LEA (%10.L, %PC, %D2), %A5
MEM[204].W = OCT "040773" | 5 << 9
MEM[206].W = 2 << 12 | 1 << 11 | 1 << 8 | 3 << 4
MEM[208].L = 0x10
MEM[212].W = TEST.BREAK

-- LEA (%10.W, %NPC, %D2), %A5
MEM[214].W = OCT "040773" | 5 << 9
MEM[216].W = 2 << 12 | 1 << 11 | 1 << 8 | 1 << 7 | 2 << 4
MEM[218].W = 0x10
MEM[220].W = TEST.BREAK

-- LEA (0x10.W, %PC), %A5
MEM[222].W = OCT "040773" | 5 << 9
MEM[224].W = 2 << 12 | 1 << 11 | 1 << 8 | 1 << 6 | 2 << 4
MEM[226].W = 0x10
MEM[228].W = TEST.BREAK

-- LEA ([0x10.W, %PC, %D2]), %A5
MEM[230].W = OCT "040773" | 5 << 9
MEM[232].W = 2 << 12 | 1 << 11 | 1 << 8 | 2 << 4 | 1
MEM[234].W = 0x10
MEM[236].W = TEST.BREAK

-- LEA ([0x10.W, %PC, %D2], 0x30.W), %A5
MEM[238].W = OCT "040773" | 5 << 9
MEM[240].W = 2 << 12 | 1 << 11 | 1 << 8 | 2 << 4 | 2
MEM[242].W = 0x10
MEM[244].W = 0x30
MEM[246].W = TEST.BREAK

-- LEA ([0x10.W, %PC, %D2], 0x30.L), %A5
MEM[248].W = OCT "040773" | 5 << 9
MEM[250].W = 2 << 12 | 1 << 11 | 1 << 8 | 2 << 4 | 3
MEM[252].W = 0x10
MEM[254].L = 0x30
MEM[258].W = TEST.BREAK

-- LEA ([0x1000.W, %PC], %D2), %A5
MEM[260].W = OCT "040773" | 5 << 9
MEM[262].W = 2 << 12 | 1 << 11 | 1 << 8 | 2 << 4 | 1 << 2 | 1
MEM[264].W = 0x1000
MEM[266].W = TEST.BREAK

-- LEA ([0x1000.W, %PC], %D2, 0x30.W), %A5
MEM[268].W = OCT "040773" | 5 << 9
MEM[270].W = 2 << 12 | 1 << 11 | 1 << 8 | 2 << 4 | 1 << 2 | 2
MEM[272].W = 0x1000
MEM[274].W = 0x30
MEM[276].W = TEST.BREAK

-- LEA ([0x1000.W, %PC], %D2, 0x30.L), %A5
MEM[278].W = OCT "040773" | 5 << 9
MEM[280].W = 2 << 12 | 1 << 11 | 1 << 8 | 2 << 4 | 1 << 2 | 3
MEM[282].W = 0x1000
MEM[284].L = 0x30
MEM[288].W = TEST.BREAK

JIT_COMPILE(0, 278)

local run = {}
function run.pre() CPU.A[4] = 0x1000 end
function run.an()
    TEST.RUN(0)
    TEST.CHECK("CPU.A[6]", 0x1000)
end

function run.d_an()
    TEST.RUN(4)
    TEST.CHECK("CPU.A[6]", 0x1010)
end

run.d_an_ri = {
    w = function()
        CPU.D[3].H = -2
        TEST.RUN(10)
        TEST.CHECK("CPU.A[6]", 0x100E)
    end,
    sc = farray({0, 1, 2, 3}, function(sc)
        CPU.D[3].L = 3
        TEST.RUN(16 + 6 * sc)
        TEST.CHECK("CPU.A[6]", 0x1010 + (3 << sc))
    end)
}

run.an_rn = {
    pre = function() CPU.D[3].L = 3 end,
    base = function()
        TEST.RUN(40)
        TEST.CHECK("CPU.A[6]", 0x1003)
    end,
    bd_w = function()
        TEST.RUN(46)
        TEST.CHECK("CPU.A[6]", 0x1013)
    end,
    bd_l = function()
        TEST.RUN(54)
        TEST.CHECK("CPU.A[6]", 0x1013)
    end
}

function run.bd_ri()
    CPU.D[3].L = 3
    TEST.RUN(64)
    TEST.CHECK("CPU.A[6]", 0x13)
end

function run.bd_ai()
    TEST.RUN(72)
    TEST.CHECK("CPU.A[6]", 0x1010)
end

run.preindex = {
    indirect = function()
        CPU.D[3].L = 0x20
        MEM[0x1030].L = 0x3000
        TEST.RUN(80)
        TEST.CHECK("CPU.A[6]", 0x3000)
    end,
    od_w = function()
        CPU.D[3].L = 0x20
        MEM[0x1030].L = 0x3000
        TEST.RUN(88)
        TEST.CHECK("CPU.A[6]", 0x3030)
    end,
    od_l = function()
        CPU.D[3].L = 0x20
        MEM[0x1030].L = 0x3000
        TEST.RUN(98)
        TEST.CHECK("CPU.A[6]", 0x3030)
    end
}

run.postindex = {
    indirect = function()
        CPU.D[3].L = 0x20
        MEM[0x1010].L = 0x3000
        TEST.RUN(110)
        TEST.CHECK("CPU.A[6]", 0x3020)
    end,
    od_w = function()
        CPU.D[3].L = 0x20
        MEM[0x1010].L = 0x3000
        TEST.RUN(118)
        TEST.CHECK("CPU.A[6]", 0x3050)
    end,
    od_l = function()
        CPU.D[3].L = 0x20
        MEM[0x1010].L = 0x3000
        TEST.RUN(128)
        TEST.CHECK("CPU.A[6]", 0x3050)
    end
}

run.imm = {
    W = function()
        TEST.RUN(140)
        TEST.CHECK("CPU.A[6]", 0x100)
    end,
    W = function()
        TEST.RUN(146)
        TEST.CHECK("CPU.A[6]", 0x100)
    end
}

function run.d_pc()
    TEST.RUN(154)
    TEST.CHECK("CPU.A[6]", 156 + 0x10)
end

run.d_pc_ri = {
    w = function()
        CPU.D[3].H = -2
        TEST.RUN(160)
        TEST.CHECK("CPU.A[6]", 162 + 0x10 - 2)
    end,
    sc = farray({0, 1, 2, 3}, function(sc)
        CPU.D[3].L = 3
        TEST.RUN(166 + 6 * sc)
        TEST.CHECK("CPU.A[6]", 168 + 6 * sc + 0x10 + (3 << sc))
    end)
}

run.pre_rn = {
    pre = function() CPU.D[3].L = 3 end,
    base = function()
        TEST.RUN(190)
        TEST.CHECK("CPU.A[6]", 192 + 3)
    end,
    bd_w = function()
        TEST.RUN(196)
        TEST.CHECK("CPU.A[6]", 198 + 0x10 + 3)
    end,
    bd_l = function()
        TEST.RUN(204)
        TEST.CHECK("CPU.A[6]", 206 + 0x10 + 3)
    end
}

function run.zpc_bd_ri()
    CPU.D[3].L = 3
    TEST.RUN(214)
    TEST.CHECK("CPU.A[6]", 0x13)
end

function run.bd_pc()
    TEST.RUN(222)
    TEST.CHECK("CPU.A[6]", 224 + 0x10)
end

run.pc_preindex = {
    indirect = function()
        CPU.D[3].L = 0x1000
        MEM[232 + 0x1010].L = 0x3000
        TEST.RUN(230)
        TEST.CHECK("CPU.A[6]", 0x3000)
    end,
    od_w = function()
        CPU.D[3].L = 0x1000
        MEM[240 + 0x1010].L = 0x3000
        TEST.RUN(238)
        TEST.CHECK("CPU.A[6]", 0x3030)
    end,
    od_l = function()
        CPU.D[3].L = 0x1000
        MEM[250 + 0x1010].L = 0x3000
        TEST.RUN(248)
        TEST.CHECK("CPU.A[6]", 0x3030)
    end
}

run.pc_postindex = {
    indirect = function()
        CPU.D[3].L = 0x20
        MEM[262 + 0x1000].L = 0x3000
        TEST.RUN(260)
        TEST.CHECK("CPU.A[6]", 0x3020)
    end,
    od_w = function()
        CPU.D[3].L = 0x20
        MEM[270 + 0x1000].L = 0x3000
        TEST.RUN(268)
        TEST.CHECK("CPU.A[6]", 0x3050)
    end,
    od_l = function()
        CPU.D[3].L = 0x20
        MEM[280 + 0x1000].L = 0x3000
        TEST.RUN(278)
        TEST.CHECK("CPU.A[6]", 0x3050)
    end
}
return run
