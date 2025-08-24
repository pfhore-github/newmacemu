-- MOVEM.W %D0-%D3/%A0-%A3, (%A5)
MEM[0].W = OCT "044220" | 5
MEM[2].W = tonumber("0000111100001111", 2)
MEM[4].W = TEST.BREAK

-- MOVEM.W %D0-%D3/%A0-%A3, -(%A5)
MEM[6].W = OCT "044240" | 5
MEM[8].W = tonumber("1111000011110000", 2)
MEM[10].W = TEST.BREAK

-- MOVEM.W (%A5), %D0-%D3/%A0-%A3
MEM[12].W = OCT "046220" | 5
MEM[14].W = tonumber("0000111100001111", 2)
MEM[16].W = TEST.BREAK

-- MOVEM.W (%A5)+, %D0-%D3/%A0-%A3
MEM[18].W = OCT "046230" | 5
MEM[20].W = tonumber("0000111100001111", 2)
MEM[22].W = TEST.BREAK

-- MOVEM.L %D0-%D3/%A0-%A3, (%A6)
MEM[24].W = OCT "044320" | 6
MEM[26].W = tonumber("0000111100001111", 2)
MEM[28].W = TEST.BREAK

-- MOVEM.L %D0-%D3/%A0-%A3, -(%A6)
MEM[30].W = OCT "044340" | 6
MEM[32].W = tonumber("1111000011110000", 2)
MEM[34].W = TEST.BREAK

-- MOVEM.L (%A6), %D0-%D3/%A0-%A3
MEM[36].W = OCT "046320" | 6
MEM[38].W = tonumber("0000111100001111", 2)
MEM[40].W = TEST.BREAK

-- MOVEM.L (%A6)+, %D0-%D3/%A0-%A3
MEM[42].W = OCT "046330" | 6
MEM[44].W = tonumber("0000111100001111", 2)
MEM[46].W = TEST.BREAK

JIT_COMPILE(0, 48)

local run = {}
run.W = {
    store = {
        base = {
            ok = function()
                CPU.D[1].W = 0x0001
                CPU.D[2].W = 0x0002
                CPU.D[3].W = 0x0003
                CPU.D[4].W = 0x0004
                CPU.A[1] = 0x1001
                CPU.A[2] = 0x1002
                CPU.A[3] = 0x1003
                CPU.A[4] = 0x1004
                CPU.A[6] = 0x1200
                TEST.RUN(0)
                TEST.CHECK("CPU.A[6]", 0x1200)
                TEST.CHECK("MEM[0x1200].W", 0x0001)
                TEST.CHECK("MEM[0x1202].W", 0x0002)
                TEST.CHECK("MEM[0x1204].W", 0x0003)
                TEST.CHECK("MEM[0x1206].W", 0x0004)
                TEST.CHECK("MEM[0x1208].W", 0x1001)
                TEST.CHECK("MEM[0x120A].W", 0x1002)
                TEST.CHECK("MEM[0x120C].W", 0x1003)
                TEST.CHECK("MEM[0x120E].W", 0x1004)
            end,
            fail = function()
                CPU.A[6] = 0x1FFE
                TEST.RUN(0, "access fault")
                TEST.CHECK("CPU.SSW.CM", true)
                TEST.CHECK("CPU.ex_addr", 0x2000)
                TEST.CHECK("CPU.EA", 0x1FFE)
            end
        },
        decr = {
            ok = function()
                CPU.D[1].W = 0x0001
                CPU.D[2].W = 0x0002
                CPU.D[3].W = 0x0003
                CPU.D[4].W = 0x0004
                CPU.A[1] = 0x1001
                CPU.A[2] = 0x1002
                CPU.A[3] = 0x1003
                CPU.A[4] = 0x1004
                CPU.A[6] = 0x1210
                TEST.RUN(6)
                TEST.CHECK("CPU.A[6]", 0x1200)
                TEST.CHECK("MEM[0x1200].W", 0x0001)
                TEST.CHECK("MEM[0x1202].W", 0x0002)
                TEST.CHECK("MEM[0x1204].W", 0x0003)
                TEST.CHECK("MEM[0x1206].W", 0x0004)
                TEST.CHECK("MEM[0x1208].W", 0x1001)
                TEST.CHECK("MEM[0x120A].W", 0x1002)
                TEST.CHECK("MEM[0x120C].W", 0x1003)
                TEST.CHECK("MEM[0x120E].W", 0x1004)
            end,
            fail = function()
                CPU.A[6] = 0x1004
                TEST.RUN(6, "access fault")
                TEST.CHECK("CPU.SSW.CM", true)
                TEST.CHECK("CPU.ex_addr", 0xFFE)
                TEST.CHECK("CPU.EA", 0x1002)
            end
        }
    },
    load = {
        base = {
            ok = function()
                MEM[0x1200].W = 0x0001
                MEM[0x1202].W = 0x0002
                MEM[0x1204].W = 0x0003
                MEM[0x1206].W = 0x0004
                MEM[0x1208].W = 0x1000
                MEM[0x120A].W = 0x1001
                MEM[0x120C].W = 0x1002
                MEM[0x120E].W = 0x1003
                CPU.A[6] = 0x1200
                TEST.RUN(12)
                TEST.CHECK("CPU.A[6]", 0x1200)
                TEST.CHECK("CPU.D[1].W", 0x0001)
                TEST.CHECK("CPU.D[2].W", 0x0002)
                TEST.CHECK("CPU.D[3].W", 0x0003)
                TEST.CHECK("CPU.D[4].W", 0x0004)
                TEST.CHECK("CPU.A[1]", 0x1000)
                TEST.CHECK("CPU.A[2]", 0x1001)
                TEST.CHECK("CPU.A[3]", 0x1002)
                TEST.CHECK("CPU.A[4]", 0x1003)
            end,
            fail = function()
                CPU.A[6] = 0x2FFC
                TEST.RUN(12, "access fault")
                TEST.CHECK("CPU.SSW.CM", true)
                TEST.CHECK("CPU.ex_addr", 0x3000)
                TEST.CHECK("CPU.EA", 0x2FFC)
            end
        },
        incr = {
            ok = function()
                MEM[0x1200].W = 0x0001
                MEM[0x1202].W = 0x0002
                MEM[0x1204].W = 0x0003
                MEM[0x1206].W = 0x0004
                MEM[0x1208].W = 0x1000
                MEM[0x120A].W = 0x1001
                MEM[0x120C].W = 0x1002
                MEM[0x120E].W = 0x1003
                CPU.A[6] = 0x1200
                TEST.RUN(18)
                TEST.CHECK("CPU.A[6]", 0x1210)
                TEST.CHECK("CPU.D[1].W", 0x0001)
                TEST.CHECK("CPU.D[2].W", 0x0002)
                TEST.CHECK("CPU.D[3].W", 0x0003)
                TEST.CHECK("CPU.D[4].W", 0x0004)
                TEST.CHECK("CPU.A[1]", 0x1000)
                TEST.CHECK("CPU.A[2]", 0x1001)
                TEST.CHECK("CPU.A[3]", 0x1002)
                TEST.CHECK("CPU.A[4]", 0x1003)
            end,
            fail = function()
                CPU.A[6] = 0x2FFC
                TEST.RUN(18, "access fault")
                TEST.CHECK("CPU.SSW.CM", true)
                TEST.CHECK("CPU.ex_addr", 0x3000)
                TEST.CHECK("CPU.EA", 0x2FFC)
            end
        }
    }
}
run.L = {
    store = {
        base = {
            ok = function()
                CPU.D[1].L = 0x00000001
                CPU.D[2].L = 0x00000002
                CPU.D[3].L = 0x00000003
                CPU.D[4].L = 0x00000004
                CPU.A[1] = 0x10000001
                CPU.A[2] = 0x10000002
                CPU.A[3] = 0x10000003
                CPU.A[4] = 0x10000004
                CPU.A[7] = 0x1200
                TEST.RUN(24)
                TEST.CHECK("CPU.A[7]", 0x1200)
                TEST.CHECK("MEM[0x1200].L", 0x00000001)
                TEST.CHECK("MEM[0x1204].L", 0x00000002)
                TEST.CHECK("MEM[0x1208].L", 0x00000003)
                TEST.CHECK("MEM[0x120C].L", 0x00000004)
                TEST.CHECK("MEM[0x1210].L", 0x10000001)
                TEST.CHECK("MEM[0x1214].L", 0x10000002)
                TEST.CHECK("MEM[0x1218].L", 0x10000003)
                TEST.CHECK("MEM[0x121C].L", 0x10000004)
            end,
            fail = function()
                CPU.A[7] = 0x1FFC
                TEST.RUN(24, "access fault")
                TEST.CHECK("CPU.SSW.CM", true)
                TEST.CHECK("CPU.ex_addr", 0x2000)
                TEST.CHECK("CPU.EA", 0x1FFC)
            end
        },
        decr = {
            ok = function()
                CPU.D[1].L = 0x00000001
                CPU.D[2].L = 0x00000002
                CPU.D[3].L = 0x00000003
                CPU.D[4].L = 0x00000004
                CPU.A[1] = 0x10000001
                CPU.A[2] = 0x10000002
                CPU.A[3] = 0x10000003
                CPU.A[4] = 0x10000004
                CPU.A[7] = 0x1220
                TEST.RUN(30)
                TEST.CHECK("CPU.A[7]", 0x1200)
                TEST.CHECK("MEM[0x1200].L", 0x00000001)
                TEST.CHECK("MEM[0x1204].L", 0x00000002)
                TEST.CHECK("MEM[0x1208].L", 0x00000003)
                TEST.CHECK("MEM[0x120C].L", 0x00000004)
                TEST.CHECK("MEM[0x1210].L", 0x10000001)
                TEST.CHECK("MEM[0x1214].L", 0x10000002)
                TEST.CHECK("MEM[0x1218].L", 0x10000003)
                TEST.CHECK("MEM[0x121C].L", 0x10000004)
            end,
            fail = function()
                CPU.A[7] = 0x1004
                TEST.RUN(30, "access fault")
                TEST.CHECK("CPU.SSW.CM", true)
                TEST.CHECK("CPU.ex_addr", 0xFFC)
                TEST.CHECK("CPU.EA", 0x1000)
            end
        }
    },
    load = {
        base = {
            ok = function()
                MEM[0x1200].L = 0x00000001
                MEM[0x1204].L = 0x00000002
                MEM[0x1208].L = 0x00000003
                MEM[0x120C].L = 0x00000004
                MEM[0x1210].L = 0x10000000
                MEM[0x1214].L = 0x10000001
                MEM[0x1218].L = 0x10000002
                MEM[0x121C].L = 0x10000003
                CPU.A[7] = 0x1200
                TEST.RUN(36)
                TEST.CHECK("CPU.A[7]", 0x1200)
                TEST.CHECK("CPU.D[1].L", 0x00000001)
                TEST.CHECK("CPU.D[2].L", 0x00000002)
                TEST.CHECK("CPU.D[3].L", 0x00000003)
                TEST.CHECK("CPU.D[4].L", 0x00000004)
                TEST.CHECK("CPU.A[1]", 0x10000000)
                TEST.CHECK("CPU.A[2]", 0x10000001)
                TEST.CHECK("CPU.A[3]", 0x10000002)
                TEST.CHECK("CPU.A[4]", 0x10000003)
            end,
            fail = function()
                CPU.A[7] = 0x2FFC
                TEST.RUN(36, "access fault")
                TEST.CHECK("CPU.SSW.CM", true)
                TEST.CHECK("CPU.ex_addr", 0x3000)
                TEST.CHECK("CPU.EA", 0x2FFC)
            end
        },
        incr = {
            ok = function()
                MEM[0x1200].L = 0x00000001
                MEM[0x1204].L = 0x00000002
                MEM[0x1208].L = 0x00000003
                MEM[0x120C].L = 0x00000004
                MEM[0x1210].L = 0x10000000
                MEM[0x1214].L = 0x10000001
                MEM[0x1218].L = 0x10000002
                MEM[0x121C].L = 0x10000003
                CPU.A[7] = 0x1200
                TEST.RUN(42)
                TEST.CHECK("CPU.A[7]", 0x1220)
                TEST.CHECK("CPU.D[1].L", 0x00000001)
                TEST.CHECK("CPU.D[2].L", 0x00000002)
                TEST.CHECK("CPU.D[3].L", 0x00000003)
                TEST.CHECK("CPU.D[4].L", 0x00000004)
                TEST.CHECK("CPU.A[1]", 0x10000000)
                TEST.CHECK("CPU.A[2]", 0x10000001)
                TEST.CHECK("CPU.A[3]", 0x10000002)
                TEST.CHECK("CPU.A[4]", 0x10000003)
            end,
            fail = function()
                CPU.A[7] = 0x2FFC
                TEST.RUN(42, "access fault")
                TEST.CHECK("CPU.SSW.CM", true)
                TEST.CHECK("CPU.ex_addr", 0x3000)
                TEST.CHECK("CPU.EA", 0x2FFC)
            end
        }
    }
}
return run
