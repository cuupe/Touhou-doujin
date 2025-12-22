-- require("resources/lua_scripts/common")
-- require("resources/lua_scripts/task")
-- require("resources/lua_scripts/enemy")

-- function ease_out_quad(t) return t * (2 - t) end

-- function Action_Snipe_And_Leave(self, target_x)
--     self:move_to(target_x, 200, 1.5, ease_out_quad)
--     Wait(0.5)

--     if self:is_alive() then
--         self:shoot_ring("bullet_type_8", 200, 16, 2)
--     end

--     Wait(0.8)
--     self:move_to(target_x, -200, 1.2)
-- end

-- function Action_Elite_Circle(self)
--     self:move_to(387, 250, 3.0, ease_out_quad)

--     Set3D("fog", 5, 11, 100, 0, 0)

--     for i = 1, 5 do
--         if not self:is_alive() then break end

--         self:shoot_ring("bullet_type_17", 250, 10, i * 3.14 / 5)
--         self:shoot_ring("bullet_type_1", 180, 20, i * 3.14 / 10)

--         Wait(1.0)
--     end

--     Set3D("fog", 5, 100, 0, 0, 0)
--     self:move_to(800, -100, 2.0)
-- end

-- function StageFlow()
--     PlayBGM("bgm1")
--     Wait(2.0)

--     for i = 1, 3 do
--         local e = Enemy:new("enemy_type_1", 100 + i * 50, -50, 50, 1)
--         StartTask(function() Action_Snipe_And_Leave(e, 100 + i * 50) end)
--         Wait(0.4)
--     end

--     Wait(3.0)

--     for i = 1, 4 do
--         local e = Enemy:new("enemy_type_3", 600 - i * 40, -50, 80, 2)
--         StartTask(function() Action_Snipe_And_Leave(e, 650 - i * 60) end)
--         Wait(0.3)
--     end

--     Wait(5.0)
--     local boss = Enemy:new("enemy_type_7", 387, -100, 2000, 10)
--     Action_Elite_Circle(boss)

--     Wait(2.0)
--     print("Stage 1 Part 1 Clear!")
--     FinishStage()
-- end

-- function level_init()
--     StartTask(StageFlow)
-- end
require("resources/lua_scripts/common")
require("resources/lua_scripts/task")
require("resources/lua_scripts/enemy")

-- ========================================================
-- 扩展：为 Enemy 类注入新的射击方法
-- ========================================================

-- 发射瞄准玩家的子弹（自机狙）
-- spread_angle: 扇形总角度 (如果 count > 1)
function Enemy:shoot_aim(bullet_id, speed, count, spread_angle)
    if not self:is_alive() then return end

    local x, y = self:get_pos()
    local aim_angle = GetAngleToPlayer(x, y) -- 调用API获取玩家角度

    if count <= 1 then
        CreateBullet(bullet_id, x, y, speed, aim_angle)
    else
        local start_angle = aim_angle - spread_angle / 2
        local step = spread_angle / (count - 1)
        for i = 0, count - 1 do
            CreateBullet(bullet_id, x, y, speed, start_angle + i * step)
        end
    end
end

-- ========================================================
-- 动作定义 (Actions)
-- ========================================================

function ease_out_quad(t) return t * (2 - t) end

-- [旧动作] 原始的狙击并撤离
function Action_Snipe_And_Leave(self, target_x)
    self:move_to(target_x, 200, 1.5, ease_out_quad)
    Wait(0.5)

    if self:is_alive() then
        self:shoot_ring("bullet_type_8", 200, 16, 2)
    end

    Wait(0.8)
    self:move_to(target_x, -200, 1.2)
end

-- [新动作] 俯冲并瞄准射击 (用于快速杂兵)
-- x_offset: 撤离时的横向偏移量
function Action_Dive_Aim(self, x_offset)
    local start_x, start_y = self:get_pos()

    -- 1. 快速俯冲
    self:move_to(start_x, 180, 0.8, ease_out_quad)
    Wait(0.2)

    -- 2. 连续发射3波自机狙
    for i = 1, 3 do
        if not self:is_alive() then return end
        -- 发射3发，扇形30度
        self:shoot_aim("bullet_type_1", 350, 3, 30)
        Wait(0.25)
    end

    -- 3. 向侧下方快速撤离
    self:move_to(start_x + x_offset, 600, 1.5)
end

-- [旧动作] 精英怪圆形弹幕
function Action_Elite_Circle(self)
    self:move_to(387, 250, 3.0, ease_out_quad)

    Set3D("fog", 5, 11, 100, 0, 0) -- 变红雾

    for i = 1, 5 do
        if not self:is_alive() then break end

        self:shoot_ring("bullet_type_17", 250, 10, i * 36) -- 36度间隔
        self:shoot_ring("bullet_type_1", 180, 20, i * 18)

        Wait(1.0)
    end

    Set3D("fog", 5, 100, 0, 0, 0) -- 恢复雾
    self:move_to(800, -100, 2.0)
end

-- [新动作] BOSS 花型弹幕
function Action_Flower_Barrage(self)
    -- 进场
    self:move_to(387, 200, 2.0, ease_out_quad)

    -- 开启紫色雾气警告
    Set3D("fog", 5, 12, 50, 0, 100)
    Wait(1.0)

    local base_angle = 0
    local timer = 0
    local duration = 10.0 -- 持续10秒

    -- 使用循环手动控制时间，实现旋转发射
    while timer < duration do
        if not self:is_alive() then break end

        -- 每一波的间隔
        Wait(0.1)
        timer = timer + 0.1

        base_angle = base_angle + 8 -- 持续旋转角度

        -- 每0.2秒发射一次
        if math.fmod(timer, 0.2) < 0.11 then
            -- 顺时针旋转的米弹
            self:shoot_ring("bullet_type_17", 220, 8, base_angle)
            -- 逆时针旋转的慢速弹
            self:shoot_ring("bullet_type_8", 150, 6, -base_angle * 1.5)
        end

        -- 偶尔发射一波自机狙干扰
        if math.fmod(timer, 2.0) < 0.11 then
            self:shoot_aim("bullet_type_1", 300, 5, 45)
        end
    end

    -- 恢复雾气
    Set3D("fog", 5, 100, 0, 0, 0)

    -- 撤离
    self:move_to(387, -200, 2.5)
end

-- ========================================================
-- 关卡流程 (Stage Flow)
-- ========================================================

function StageFlow()
    PlayBGM("bgm1")

    -- === 第一阶段：热身 ===
    Wait(2.0)
    print("Wave 1: Snipers")

    for i = 1, 3 do
        local e = Enemy:new("enemy_type_1", 100 + i * 50, -50, 50, 1)
        StartTask(function() Action_Snipe_And_Leave(e, 100 + i * 50) end)
        Wait(0.4)
    end

    Wait(2.0)

    for i = 1, 4 do
        local e = Enemy:new("enemy_type_3", 600 - i * 40, -50, 80, 2)
        StartTask(function() Action_Snipe_And_Leave(e, 650 - i * 60) end)
        Wait(0.3)
    end

    Wait(4.0)

    -- === 第二阶段：中BOSS ===
    print("Wave 2: Mid-Boss")
    local midboss = Enemy:new("enemy_type_7", 387, -100, 1500, 10)
    Action_Elite_Circle(midboss)

    Wait(3.0)

    -- === 第三阶段：左右夹击（新增） ===
    print("Wave 3: Rush")

    -- 左侧生成，向右撤离
    for i = 1, 5 do
        local e = Enemy:new("enemy_type_1", 150 + i * 40, -50, 30, 1)
        StartTask(function() Action_Dive_Aim(e, 200) end)
        Wait(0.3)
    end

    Wait(1.0)

    -- 右侧生成，向左撤离
    for i = 1, 5 do
        local e = Enemy:new("enemy_type_1", 620 - i * 40, -50, 30, 1)
        StartTask(function() Action_Dive_Aim(e, -200) end)
        Wait(0.3)
    end

    Wait(4.0)

    -- === 第四阶段：最终BOSS（新增） ===
    print("Wave 4: Final Boss")

    -- 血量 3000 的 Boss
    local boss = Enemy:new("enemy_type_7", 387, -100, 3000, 50)
    Action_Flower_Barrage(boss)

    Wait(4.0)

    print("Stage Clear!")
    FinishStage()
end

function level_init()
    StartTask(StageFlow)
end
