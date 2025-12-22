function Color(a, r, g, b)
    return {a, r, g, b}
end

sin = math.sin
cos = math.cos
rad = math.rad
mod = math.fmod

object = {}
function Class(base)
    local c = {}
    c.__index = c
    function c.new(...)
        local instance = setmetatable({}, c)
        if instance.init then instance:init(...) end
        return instance
    end
    return c
end

background = {}
function background.init(obj, flag) 
    obj.z = 0 
    obj.timer = 0
end
function background.WarpEffectCapture() end
function background.WarpEffectApply() end
function background.ClearToFogColor() end

task = {}
local tasks = {}

function task.New(obj, func)
    table.insert(tasks, {co = coroutine.create(func), obj = obj})
end

function task.Wait(frames)
    for i=1, frames do coroutine.yield() end
end

function task.Do(obj)
    for i=#tasks, 1, -1 do
        local t = tasks[i]
        if t.obj == obj and coroutine.status(t.co) ~= "dead" then
            local res, err = coroutine.resume(t.co)
            if not res then print("Lua Task Error: "..err) end
        elseif coroutine.status(t.co) == "dead" then
            table.remove(tasks, i)
        end
    end
end

function task.Clear(obj)
    for i=#tasks, 1, -1 do
        if tasks[i].obj == obj then
            table.remove(tasks, i)
        end
    end
end