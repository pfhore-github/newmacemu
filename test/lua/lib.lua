local FIXTURE_TABLE = {}
function run_all(name, t)
    FIXTURE_TABLE[#FIXTURE_TABLE+1] = { pre = t.pre, post = t.post }
    if t.init then
        t.init()
        t.init = nil
    end
    local fin = t.fin
    t.fin = nil    
    t.pre = nil
    t.post = nil
    for name2, handler in pairs(t) do
        local fullName = tostring(name) .. "/" .. tostring(name2)
        if type(handler) == "table" then
            run_all( fullName, handler)
        elseif type(handler) == "function" then
            TEST.RESET()
            for i = 1, #FIXTURE_TABLE do
                if FIXTURE_TABLE[i].pre then
                    FIXTURE_TABLE[i].pre()
                end
            end
            pcall(TEST.EXEC, fullName, handler)
            for i = #FIXTURE_TABLE, 1, -1 do
                if FIXTURE_TABLE[i].post then
                    FIXTURE_TABLE[i].post()
                end
            end
        end
    end
    FIXTURE_TABLE[#FIXTURE_TABLE] = nil
    if fin then
        fin()
    end
end

function farray(s, f)
    local ret = {}
    for _, v in ipairs(s) do 
        ret[v] = function() f(v) end
    end
    return ret
end
function OCT(s)
    return tonumber(s, 8)
end

function cond(c, t, f)
    if c then 
        return t 
    else 
        return f
    end
end
function priv_test(n)
    CPU.SR.S = false
    CPU.SR.T = 0
    CPU.A[8] = 0x8000
    TEST.RUN(n, "privilege error")
end
function trace_test(n)
    CPU.SR.S = true
    CPU.SR.T = 1
    TEST.RUN(n, "trace")
end
TF = {false, true}
dofile("asm.lua")
