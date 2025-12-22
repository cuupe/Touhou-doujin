TaskManager = {}
TaskManager.list = {}

function StartTask(func, entity)
    local co = coroutine.create(func)
    table.insert(TaskManager.list, { 
        co = co, 
        wait_timer = 0, 
        entity = entity 
    })
end

function Wait(seconds)
    local dt = coroutine.yield(seconds)
end

function WaitFrame()
    local dt = coroutine.yield(0) 
    return dt
end

function level_update(dt)
    for i = #TaskManager.list, 1, -1 do
        local task = TaskManager.list[i]

        local is_dead = false
        if task.entity and task.entity.is_alive then
            if not task.entity:is_alive() then
                is_dead = true
            end
        end

        if is_dead then
            table.remove(TaskManager.list, i)
        elseif task.wait_timer > 0 then
            task.wait_timer = task.wait_timer - dt
        else
            if coroutine.status(task.co) ~= "dead" then
                local success, result = coroutine.resume(task.co, dt)
                
                if success then
                    if type(result) == "number" then
                        task.wait_timer = result
                    else
                        task.wait_timer = 0
                    end
                else
                    print("Lua Error: " .. tostring(result))
                end
            else
                table.remove(TaskManager.list, i)
            end
        end
    end
end