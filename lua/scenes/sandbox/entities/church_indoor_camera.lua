require("utils")

local left = true
local rotationSpeed = 0.05

function ChurchIndoorCamera.onUpdate(entity, timestep)
    local transformComponent = Entity:getTransformComponent(entity)

    local yaw = transformComponent.yaw
    local pitch = transformComponent.pitch
    local roll = transformComponent.roll

    if left and yaw < 80 then
        left = false
    end
    if not left and yaw > 140 then
        left = true
    end
    if left then
        yaw = yaw - rotationSpeed
    else
        yaw = yaw + rotationSpeed
    end
    if yaw > 360 then
        yaw = 0
    end

    local yawRadians = math.rad(yaw)
    local pitchRadians = math.rad(pitch)
    local rollRadians = math.rad(roll)

    local yawRotation = glm.normalizeQuat(glm.angleAxis(yawRadians, POSITIVE_Y_AXIS))
    local pitchRotation = glm.normalizeQuat(glm.angleAxis(pitchRadians, POSITIVE_X_AXIS))
    local rollRotation = glm.normalizeQuat(glm.angleAxis(rollRadians, POSITIVE_Z_AXIS))

    local orientation = glm.normalizeQuat(yawRotation * pitchRotation * rollRotation)

    Entity:setTransformComponent(entity, {
        orientation = orientation,
        yaw = yaw,
        pitch = pitch,
        roll = roll,
    })
end
