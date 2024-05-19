require("utils")

local moveSpeed = 0
local thrust = 0.5
local maxMoveSpeed = 50

local rotationSpeed = 1

local maxYaw = 360
local maxPitch = 33
local maxRoll = 33

--local drag = 0
--local thrust = 0
--local lift = 0
--local gravity = 0

local loggingEnabled = false

function Player.onUpdate(entityId, timestep)
    local transformComponent = Entity:getTransformComponent(entityId)

    local position = transformComponent.position
    local forwardDirection = transformComponent.forwardDirection
    local yaw = transformComponent.yaw
    local pitch = transformComponent.pitch
    local roll = transformComponent.roll

    if Keyboard:isPressed(Key.Numpad_0) then
        position = glm.vec3()
    end
    if Keyboard:isPressed(Key.L) then
        loggingEnabled = true
    end
    if Keyboard:isPressed(Key.K) then
        loggingEnabled = false
    end
    if loggingEnabled then
        printt(entityId)
    end

    if Keyboard:isPressed(Key.Numpad_6) then
        yaw = yaw + rotationSpeed
    end
    if Keyboard:isPressed(Key.Numpad_4) then
        yaw = yaw - rotationSpeed
    end
    if yaw > 360 then
        yaw = 0
    end
    if yaw < -360 then
        yaw = 0
    end

    if Keyboard:isPressed(Key.Numpad_8) then
        pitch = pitch - rotationSpeed
    end
    if Keyboard:isPressed(Key.Numpad_5) then
        pitch = pitch + rotationSpeed
    end
    if pitch > maxPitch then
        pitch = maxPitch
    end
    if pitch < -maxPitch then
        pitch = -maxPitch
    end
    if not Keyboard:isPressed(Key.Numpad_8) and not Keyboard:isPressed(Key.Numpad_5) then
        if pitch < 0 then
            pitch = pitch + rotationSpeed
        end
        if pitch > 0 then
            pitch = pitch - rotationSpeed
        end
    end

    --if Keyboard:isPressed(Key.Numpad_6) then
    --    roll = roll + rotationSpeed
    --end
    --if Keyboard:isPressed(Key.Numpad_4) then
    --    roll = roll - rotationSpeed
    --end
    --if roll > maxRoll then
    --    roll = maxRoll
    --end
    --if roll < -maxRoll then
    --    roll = -maxRoll
    --end
    --if not Keyboard:isPressed(Key.Numpad_6) and not Keyboard:isPressed(Key.Numpad_4) then
    --    if roll < 0 then
    --        roll = roll + rotationSpeed
    --    end
    --    if roll > 0 then
    --        roll = roll - rotationSpeed
    --    end
    --end

    if Keyboard:isPressed(Key.X) then
        moveSpeed = math.min(moveSpeed + thrust, maxMoveSpeed)
    end
    if Keyboard:isPressed(Key.Z) then
        moveSpeed = math.max(moveSpeed - thrust, -maxMoveSpeed)
    end
    if not Keyboard:isPressed(Key.X) and not Keyboard:isPressed(Key.Z) then
        if moveSpeed > 0 then
            moveSpeed = math.max(moveSpeed - thrust, 0)
        end
        if moveSpeed < 0 then
            moveSpeed = math.min(moveSpeed + thrust, 0)
        end
    end

    local velocity = moveSpeed * timestep
    local distance = glm.multiply(forwardDirection, velocity)
    position = glm.add(position, distance)

    transformComponent.position = position
    transformComponent.yaw = yaw
    transformComponent.pitch = pitch
    transformComponent.roll = roll

    Entity:setTransformComponent(entityId, transformComponent)
end
