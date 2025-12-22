local bg = {
    speed = 0.005,
    z = 0,
    timer = 0,
    eye_a = 1.0,
    eye_b = 3.6,
    eye_c = -7.1,
    alpha = 0,
    cover = { alpha = 0, r = 0, g = 0, b = 0 }
}

function Init()
    Set3D('eye', 0, 0, -5)
    Set3D('at', 0.0, 4, 0.0)
    Set3D('up', 0, 1, 0)

    Set3D('z', 1, 1000)
    Set3D('fovy', 0.6)
    Set3D('fog', 6.70, 10.8, 0, 0, 0)
end

function Update()
    bg.timer = bg.timer + 1
    bg.z = bg.z + bg.speed

    if bg.timer < 301 then
        bg.eye_a = 8.5 - 7.5 * (bg.timer / 265)
        bg.eye_b = 8.5 - 4.9 * (bg.timer / 300)
        Set3D('eye', bg.eye_a, bg.eye_b, -7.1)
        Set3D('up', 1 - 1 / 300 * bg.timer, 1, 0)
    end

    if bg.timer > 360 then
        local t = (bg.timer - 360) * 0.01745
        bg.alpha = 80 - 80 * math.cos(t)
        bg.cover.alpha = 30 - 30 * math.cos(t / 1.5)
        if bg.timer > 539 then
            bg.cover.r = 70 - 70 * math.cos(((bg.timer - 540) / 2.3) * 0.01745)
        end
        if bg.timer > 619 then
            bg.cover.b = 100 - 100 * math.cos(((bg.timer - 620) / 2.6) * 0.01745)
        end
    end
end

local function render_ground(z)
    local z1 = z + 1
    local z2 = z - 1
    local z3 = z - 3
    local z4 = z + 3

    local factor = 0

    local t_start = 300
    local t_end = 420
    local duration = t_end - t_start

    if bg.timer <= t_start then
        factor = 0.0
    elseif bg.timer > t_start and bg.timer < t_end then
        local t = (bg.timer - t_start) / duration
        factor = t * t * (3 - 2 * t)
    else
        factor = 1.0
    end
    SetImageState('stage04c', '', 255, 255, 255, 255)

    Render4VMix('stage04c', 'stage04b', factor, -2, 0, z1, 0, 0, z1, 0, 0, z2, -2, 0, z2)
    Render4VMix('stage04c', 'stage04b', factor, -2, 0, z4, 0, 0, z4, 0, 0, z1, -2, 0, z1)
    Render4VMix('stage04c', 'stage04b', factor, -2, 0, z2, 0, 0, z2, 0, 0, z3, -2, 0, z3)
    Render4VMix('stage04c', 'stage04b', factor, 0, 0, z1, 2, 0, z1, 2, 0, z2, 0, 0, z2)
    Render4VMix('stage04c', 'stage04b', factor, 0, 0, z4, 2, 0, z4, 2, 0, z1, 0, 0, z1)
    Render4VMix('stage04c', 'stage04b', factor, 0, 0, z2, 2, 0, z2, 2, 0, z3, 0, 0, z3)

    SetImageState('stage04a', '', 255, 255, 255, 255)
    Render4V('stage04a', -8, 0, z1, -2, 0, z1, -2, 0, z2, -8, 0, z2)
    Render4V('stage04a', -8, 0, z4, -2, 0, z4, -2, 0, z1, -8, 0, z1)
    Render4V('stage04a', -8, 0, z2, -2, 0, z2, -2, 0, z3, -8, 0, z3)
    Render4V('stage04a', 2, 0, z1, 8, 0, z1, 8, 0, z2, 2, 0, z2)
    Render4V('stage04a', 2, 0, z4, 8, 0, z4, 8, 0, z1, 2, 0, z1)
    Render4V('stage04a', 2, 0, z2, 8, 0, z2, 8, 0, z3, 2, 0, z3)
end

local function render_cover(z, alpha, r, g, b)
    if alpha <= 0 then return end
    SetImageState('stage04d', 'mul+add', r, g, b, alpha)
    local y = 0.02
    Render4V('stage04d', -8, y, z + 3, 8, y, z + 3, 8, y, z - 3, -8, y, z - 3)
end

function Render()
    for j = -2, 3 do
        local dz = 6 * j - math.fmod(bg.z, 6)
        render_ground(dz)
        render_cover(dz, bg.cover.alpha, bg.cover.r, 0, bg.cover.b)
    end
end
