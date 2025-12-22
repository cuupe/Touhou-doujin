Class = {}
function Class:new(o)
    o = o or {}
    setmetatable(o, self)
    self.__index = self
    return o
end

local function lerp(a, b, t) return a + (b - a) * t end
local function ease_out_quad(t) return t * (2 - t) end
local function ease_in_quad(t) return t * t end

Enemy = Class:new()

function Enemy:new(type_id, x, y, hp, drop)
    local o = {}
    setmetatable(o, self)
    self.__index = self

    o.id = CreateEnemy(type_id, x, y, hp, drop or 0)
    return o
end


function Enemy:is_alive()
    return Enemy_IsValid(self.id)
end

function Enemy:get_pos()
    if self:is_alive() then
        return Enemy_GetPosition(self.id)
    end
    return 0, 0
end


function Enemy:set_pos(x, y)
    if self:is_alive() then
        Enemy_SetPosition(self.id, x, y)
    end
end

function Enemy:move_to(target_x, target_y, duration, ease_func)
    local timer = 0
    local start_x, start_y = self:get_pos()

    while timer < duration do

        local dt = WaitFrame()
        if not self:is_alive() then return end

        timer = timer + dt
        if timer > duration then timer = duration end
        local t = timer / duration
        if ease_func then t = ease_func(t) end

        local curr_x = lerp(start_x, target_x, t)
        local curr_y = lerp(start_y, target_y, t)
        
        self:set_pos(curr_x, curr_y)
    end

    self:set_pos(target_x, target_y)
end

function Enemy:shoot_ring(bullet_id, speed, count, angle)
    if not self:is_alive() then return end
    local x, y = self:get_pos()
    local step = 360 / count
    for i = 0, count - 1 do
        CreateBullet(bullet_id, x, y, speed, i * step + angle)
    end
end

function Enemy:run_task(func)
    local bound_func = function() func(self) end
    StartTask(bound_func, self)
end