require("Game/Script/lua_scripts/common")
require("Game/Script/lua_scripts/task")
require("Game/Script/lua_scripts/enemy")

function ease_out_quad(t) return t * (2 - t) end

function Action_Snipe_And_Leave(self, target_x)

    self:move_to(target_x, 200, 1.5, ease_out_quad)
    Wait(0.5)
    
    if self:is_alive() then

        self:shoot_ring("bullet_type_8", 200, 16, 2)
    end
    
    Wait(0.8)
    self:move_to(target_x, -200, 1.2)
end

function Action_Elite_Circle(self)
    self:move_to(387, 250, 3.0, ease_out_quad)

    Set3D("fog", 5, 11, 100, 0, 0)
    
    for i = 1, 5 do
        if not self:is_alive() then break end

        self:shoot_ring("bullet_type_17", 250, 10, i * 3.14 / 5)
        self:shoot_ring("bullet_type_1", 180, 20, i * 3.14 / 10)
        
        Wait(1.0)
    end

    Set3D("fog", 5, 100, 0, 0, 0)
    self:move_to(800, -100, 2.0)
end


function StageFlow()
    PlayBGM("bgm1")
    Wait(2.0)

    for i = 1, 3 do
        local e = Enemy:new("enemy_type_1", 100 + i*50, -50, 50, 1)
        StartTask(function() Action_Snipe_And_Leave(e, 100 + i*50) end)
        Wait(0.4)
    end
    
    Wait(3.0)

    for i = 1, 4 do
        local e = Enemy:new("enemy_type_3", 600 - i*40, -50, 80, 2)
        StartTask(function() Action_Snipe_And_Leave(e, 650 - i*60) end)
        Wait(0.3)
    end
    
    Wait(5.0)
    local boss = Enemy:new("enemy_type_7", 387, -100, 2000, 10)
    Action_Elite_Circle(boss)
    
    Wait(2.0)
    print("Stage 1 Part 1 Clear!")
    FinishStage()
end

function level_init()
    StartTask(StageFlow)
end