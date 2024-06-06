require("utils")

local thrust = 0.5
local moveSpeed = 0
local maxMoveSpeed = 100

local yawSpeed = 0.75
local pitchSpeed = 0.5
local rollSpeed = yawSpeed * 1.5

local maxRollWhenTurning = 45

local function reset(entity)
    moveSpeed = 0
    Entity:setTransformComponent(entity, {
        position = glm.vec3(0, 0, 0),
        orientation = glm.quat(1, 0, 0, 0),
        forwardDirection = WORLD_FORWARD_DIRECTION,
        upDirection = WORLD_UP_DIRECTION,
        rightDirection = WORLD_RIGHT_DIRECTION,
        yaw = 0,
        pitch = 0,
        roll = 0,
    })
end

function Player.onUpdate(entity, timestep)
    if Keyboard:isPressed(Key.M) then
        reset(entity)
    end

    local transformComponent = Entity:getTransformComponent(entity)

    local position = transformComponent.position
    local forwardDirection = transformComponent.forwardDirection
    local yaw = transformComponent.yaw
    local pitch = transformComponent.pitch
    local roll = transformComponent.roll

    local up = Keyboard:isPressed(Key.Numpad_8) or Keyboard:isPressed(Key.I)
    local down = Keyboard:isPressed(Key.Numpad_5) or Keyboard:isPressed(Key.K)

    local left = Keyboard:isPressed(Key.Numpad_4) or Keyboard:isPressed(Key.J)
    local right = Keyboard:isPressed(Key.Numpad_6) or Keyboard:isPressed(Key.L)

    local rollLeft = Keyboard:isPressed(Key.Numpad_7) or Keyboard:isPressed(Key.U)
    local rollRight = Keyboard:isPressed(Key.Numpad_9) or Keyboard:isPressed(Key.O)

    local forwards = Keyboard:isPressed(Key.X)
    local backwards = Keyboard:isPressed(Key.Z)

    if left then
        yaw = yaw + yawSpeed
        if yaw > 360 then
            yaw = 0
        end
    end
    if right then
        yaw = yaw - yawSpeed
        if yaw < -360 then
            yaw = 0
        end
    end

    if up then
        pitch = pitch - pitchSpeed
        if pitch < -360 then
            pitch = 0
        end
    end
    if down then
        pitch = pitch + pitchSpeed
        if pitch > 360 then
            pitch = 0
        end
    end

    if left then
        roll = roll + rollSpeed
        if roll > maxRollWhenTurning then
            roll = maxRollWhenTurning
        end
    end
    if right then
        roll = roll - rollSpeed
        if roll < -maxRollWhenTurning then
            roll = -maxRollWhenTurning
        end
    end
    if not left and not right and not rollLeft and not rollRight then
        if roll < 0 then
            roll = roll + rollSpeed
        end
        if roll > 0 then
            roll = roll - rollSpeed
        end
    end

    if rollLeft then
        roll = roll + rollSpeed
    end
    if rollRight then
        roll = roll - rollSpeed
    end

    if forwards then
        moveSpeed = math.min(moveSpeed + thrust, maxMoveSpeed)
    end
    if backwards then
        moveSpeed = math.max(moveSpeed - thrust, -maxMoveSpeed)
    end
    if not forwards and not backwards then
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

    Entity:setTransformComponent(entity, {
        position = position,
        yaw = yaw,
        pitch = pitch,
        roll = roll,
    })
end