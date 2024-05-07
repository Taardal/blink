local Vector = require("vector")

local moveSpeed = 1

function Player.onUpdate(entityId, timestep)
    local transformComponent = Entity:getTransformComponent(entityId)

    local position = transformComponent.position
    local forwardDirection = transformComponent.forwardDirection
    local rightDirection = transformComponent.rightDirection
    local pitch = transformComponent.pitch
    local yaw = transformComponent.yaw
    local roll = transformComponent.roll

    local velocity = moveSpeed * timestep

    if Keyboard:isPressed(Key.K) or Keyboard:isPressed(Key.Numpad_8) then
        --pitch = pitch - 1
        --pitch = math.max(pitch - 1, -89.0)
        position = Vector.add(position, Vector.multiply(forwardDirection, velocity))
    end
    if Keyboard:isPressed(Key.J) or Keyboard:isPressed(Key.Numpad_5) then
        --pitch = pitch + 1
        --pitch = math.min(pitch + 1, 89.0)
        position = Vector.subtract(position, Vector.multiply(forwardDirection, velocity))
    end
    if (Keyboard:isPressed(Key.L) or Keyboard:isPressed(Key.Numpad_6)) then
        --roll = roll + 1
        --roll = math.min(roll + 1, 30)
        position = Vector.add(position, Vector.multiply(rightDirection, velocity))
    end
    if (Keyboard:isPressed(Key.H) or Keyboard:isPressed(Key.Numpad_4)) then
        --roll = roll - 1
        --roll = math.max(roll - 1, -30)
        position = Vector.subtract(position, Vector.multiply(rightDirection, velocity))
    end
    if (Keyboard:isPressed(Key.Numpad_9)) then
        yaw = yaw + 1
    end
    if (Keyboard:isPressed(Key.Numpad_7)) then
        yaw = yaw - 1
    end

    transformComponent.position = position
    transformComponent.yaw = yaw
    transformComponent.pitch = pitch
    transformComponent.roll = roll

    --local speed = 0.1 -- Example speed
    --
    --local a = speed * timestep
    --print("a: " .. a)
    --local b = Vector.multiply(transformComponent.forwardDirection, a)
    --print("b: " .. b.x .. ", " .. b.y .. ", " .. b.z .. "]")
    --local c = Vector.add(transformComponent.position, b)
    --print("c: " .. c.x .. ", " .. c.y .. ", " .. c.z .. "]")

    --local a = Vector.add(transformComponent.position, transformComponent.forwardDirection)
    --print("a: " .. a.x .. ", " .. a.y .. ", " .. a.z .. "]")
    --local b = Vector.multiply(a, speed)
    --print("b: " .. b.x .. ", " .. b.y .. ", " .. b.z .. "]")
    --local c = Vector.multiply(b, timestep)
    --print("c: " .. c.x .. ", " .. c.y .. ", " .. c.z .. "]")

    --transformComponent.position = c

    Entity:setTransformComponent(entityId, transformComponent)

    --print("yaw [" .. transformComponent.yaw .. "]")
    --print("pitch [" .. transformComponent.pitch .. "]")
    --print("roll [" .. transformComponent.roll .. "]")
end
