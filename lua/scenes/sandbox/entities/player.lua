require("utils")

local moveSpeed = 0
local thrust = 0.5
local maxMoveSpeed = 50

local rotationSpeed = 2

local maxYaw = 360
local maxPitch = 89
local maxRoll = 66

local loggingEnabled = false

function Player.onUpdate(entityId, timestep)
    if Keyboard:isPressed(Key.L) then
        loggingEnabled = true
    end
    if Keyboard:isPressed(Key.K) then
        loggingEnabled = false
    end
    if loggingEnabled then
        printt(entityId)
    end
    if Keyboard:isPressed(Key.Numpad_0) then
        Entity:setPosition(entityId, glm.vec3())
    end

    local transformComponent = Entity:getTransformComponent(entityId)

    local position = transformComponent.position
    local forwardDirection = transformComponent.forwardDirection
    --local yaw = transformComponent.yaw
    --local pitch = transformComponent.pitch
    --local roll = transformComponent.roll

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
    --transformComponent.yaw = yaw
    --transformComponent.pitch = pitch
    --transformComponent.roll = roll

    Entity:setTransformComponent(entityId, transformComponent)
end
