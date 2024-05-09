local Vector = require("vector")

local thrust = 0.1
local moveSpeed = 0
local maxMoveSpeed = 20
local lookSpeed = 0.75
local maxPitch = 89

function Player.onUpdate(entityId, timestep)
    local transformComponent = Entity:getTransformComponent(entityId)

    local translation = transformComponent.translation
    local rotation = transformComponent.rotation
    local scale = transformComponent.scale
    local position = transformComponent.position
    local forwardDirection = transformComponent.forwardDirection
    local rightDirection = transformComponent.rightDirection
    local upDirection = transformComponent.upDirection
    local worldUpDirection = transformComponent.worldUpDirection
    local yaw = transformComponent.yaw
    local pitch = transformComponent.pitch
    local roll = transformComponent.roll

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
    position = Vector.add(position, Vector.multiply(forwardDirection, velocity))

    if Keyboard:isPressed(Key.Numpad_6) or Keyboard:isPressed(Key.L) then
        yaw = yaw + lookSpeed
    end
    if Keyboard:isPressed(Key.Numpad_4) or Keyboard:isPressed(Key.H) then
        yaw = yaw - lookSpeed
    end
    if Keyboard:isPressed(Key.Numpad_8) or Keyboard:isPressed(Key.K) then
        pitch = pitch - lookSpeed
    end
    if Keyboard:isPressed(Key.Numpad_5) or Keyboard:isPressed(Key.J) then
        pitch = pitch + lookSpeed
    end
    --if Keyboard:isPressed(Key.Numpad_9) or Keyboard:isPressed(Key.O) then
    --    roll = roll + 1
    --end
    --if Keyboard:isPressed(Key.Numpad_7) or Keyboard:isPressed(Key.U) then
    --    roll = roll - 1
    --end
    if pitch > maxPitch then
        pitch = maxPitch
    end
    if pitch < -maxPitch then
        pitch = -maxPitch
    end

    forwardDirection.x = math.cos(math.rad(yaw)) * math.cos(math.rad(pitch));
    forwardDirection.y = math.sin(math.rad(pitch));
    forwardDirection.z = math.sin(math.rad(yaw)) * math.cos(math.rad(pitch));
    forwardDirection = glm.normalize(forwardDirection);

    rightDirection = glm.normalize(glm.cross(forwardDirection, worldUpDirection));
    upDirection = glm.normalize(glm.cross(rightDirection, forwardDirection));

    transformComponent.translation = translation
    transformComponent.rotation = rotation
    transformComponent.scale = scale
    transformComponent.position = position
    transformComponent.forwardDirection = forwardDirection
    transformComponent.rightDirection = rightDirection
    transformComponent.upDirection = upDirection
    transformComponent.worldUpDirection = worldUpDirection
    transformComponent.yaw = yaw
    transformComponent.pitch = pitch
    transformComponent.roll = roll

    Entity:setTransformComponent(entityId, transformComponent)
end
