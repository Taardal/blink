require("utils")

local thrust = 0.5
local moveSpeed = 0
local maxMoveSpeed = 100

local yawSpeed = 1
local pitchSpeed = 0.5
local rollSpeed = 1

local maxRollWhenTurning = 45

local loggingEnabled = false

function Player.onUpdate(entity, timestep)

    if Keyboard:isPressed(Key.L) then
        loggingEnabled = true
    end
    if Keyboard:isPressed(Key.K) then
        loggingEnabled = false
    end
    if loggingEnabled then
        printt(entity)
    end
    if Keyboard:isPressed(Key.Numpad_0) then
        moveSpeed = 0
        Entity:setTransformComponent(entity, {
            position = glm.vec3(0, 0, 0),
            yaw = 0,
            pitch = 0,
            roll = 0,
        })
    end

    local transformComponent = Entity:getTransformComponent(entity)

    local position = transformComponent.position
    local forwardDirection = transformComponent.forwardDirection
    local yaw = transformComponent.yaw
    local pitch = transformComponent.pitch
    local roll = transformComponent.roll

    if Keyboard:isPressed(Key.Numpad_4) then
        yaw = yaw + yawSpeed
        if yaw > 360 then
            yaw = 0
        end
    end
    if Keyboard:isPressed(Key.Numpad_6) then
        yaw = yaw - yawSpeed
        if yaw < -360 then
            yaw = 0
        end
    end

    if Keyboard:isPressed(Key.Numpad_8) then
        pitch = pitch - pitchSpeed
        if pitch < -360 then
            pitch = 0
        end
    end
    if Keyboard:isPressed(Key.Numpad_5) then
        pitch = pitch + pitchSpeed
        if pitch > 360 then
            pitch = 0
        end
    end
    --if not Keyboard:isPressed(Key.Numpad_8) and not Keyboard:isPressed(Key.Numpad_5) then
    --    if pitch > 0 then
    --        pitch = pitch - pitchSpeed
    --    end
    --end

    if Keyboard:isPressed(Key.Numpad_4) then
        roll = roll + rollSpeed
        if roll > maxRollWhenTurning then
            roll = maxRollWhenTurning
        end
    end
    if Keyboard:isPressed(Key.Numpad_6) then
        roll = roll - rollSpeed
        if roll < -maxRollWhenTurning then
            roll = -maxRollWhenTurning
        end
    end
    if not Keyboard:isPressed(Key.Numpad_6) and not Keyboard:isPressed(Key.Numpad_4) then
        if roll < 0 then
            roll = roll + rollSpeed
        end
        if roll > 0 then
            roll = roll - rollSpeed
        end
    end

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
    local distance = forwardDirection * velocity
    position = position + distance

    transformComponent.position = position
    transformComponent.yaw = yaw
    transformComponent.pitch = pitch
    transformComponent.roll = roll

    Entity:setTransformComponent(entity, transformComponent)
end
