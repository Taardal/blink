require("utils")

function ChurchRoofCamera.onUpdate(entity, timestep)
    local transformComponent = Entity:getTransformComponent(entity)

    local yaw = transformComponent.yaw
    local pitch = transformComponent.pitch
    local roll = transformComponent.roll

    yaw = yaw - 0.2
    if yaw > 360 then
        yaw = 0
    end

    local yawRadians = math.rad(yaw)
    local pitchRadians = math.rad(pitch)
    local rollRadians = math.rad(roll)

    local yawRotation = (glm.angleAxis(yawRadians, POSITIVE_Y_AXIS)):normalize()
    local pitchRotation = (glm.angleAxis(pitchRadians, POSITIVE_X_AXIS)):normalize()
    local rollRotation = (glm.angleAxis(rollRadians, POSITIVE_Z_AXIS)):normalize()

    local orientation = (yawRotation * pitchRotation * rollRotation):normalize()

    Entity:setTransformComponent(entity, {
        orientation = orientation,
        yaw = yaw,
        pitch = pitch,
        roll = roll,
    })
end
