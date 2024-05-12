local Vector = require("Vector")

local THRUST = 2
local MOVE_SPEED = 500

local moveSpeed = 0

function AiPatrolLine.onUpdate(entityId, timestep)
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

    local velocity = MOVE_SPEED * timestep
    position = Vector.add(position, Vector.multiply(forwardDirection, velocity))

    if position.x < -950 then
        yaw = 0
    end
    if position.x > -50 then
        yaw = 180
    end

    local yawRadians = math.rad(yaw)
    local pitchRadians = math.rad(pitch)

    --print("180 " .. math.rad(180) .. ", " .. math.cos(math.rad(180)))
    --print("-180 " .. math.rad(-180) .. ", " .. math.cos(math.rad(-180)))

    forwardDirection.x = math.cos(math.rad(yaw)) * math.cos(math.rad(pitch));
    forwardDirection.y = math.sin(math.rad(pitch));
    forwardDirection.z = math.sin(math.rad(yaw)) * math.cos(math.rad(pitch));
    forwardDirection = glm.normalize(forwardDirection)

    --print("a " .. forwardDirection.x)
    --forwardDirection.x = 1
    --print("b " .. forwardDirection.x)

    --if moveForwards and position.x < -100 then
    --        position.x = 0
    --        forwardDirection.x = 1
    --        yaw = 180
    --    end
    --    if not moveForwards and position.x > 100 then
    --        forwardDirection.x = - 1
    --        yaw = -180
    --    end
    --
    --    print(forwardDirection.x .. ", " .. tostring(moveForwards) .. ", " .. position.x)

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
