local Vector = require("Vector")

local RADIUS = 0.1
local MOVE_SPEED = 200
local ORBIT_SPEED = 100

local a = 0.00833333
local b = 0.01666667

function AiPatrolCircle.onUpdate(entityId, timestep)
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

    print("entity " .. entityId .. " --> x " .. position.x .. ", y " .. position.y .. ", z " .. position.z)

    -- Move forward
    local linearVelocity = MOVE_SPEED * timestep
    position = Vector.add(position, Vector.multiply(forwardDirection, 1))

    -- Rotate
    local angularVelocity = linearVelocity / RADIUS
    --yaw = yaw + (angularVelocity * (timestep * ORBIT_SPEED))

    -- Ensure rotation stays within bounds (0 to 360 degrees)
    --yaw = yaw % 360

    local yawRadians = math.rad(yaw)
    local pitchRadians = math.rad(pitch)

    forwardDirection.x = math.cos(yawRadians) * math.cos(pitchRadians)
    forwardDirection.y = math.sin(pitchRadians)
    forwardDirection.z = math.sin(yawRadians) * math.cos(pitchRadians)
    forwardDirection = glm.normalize(forwardDirection)

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
