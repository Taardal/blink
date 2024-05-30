local moveSpeed = 75
local rotationSpeed = 1

function RollPatrol.onUpdate(entity, timestep)
    local transformComponent = Entity:getTransformComponent(entity)

    local position = transformComponent.position
    local forwardDirection = transformComponent.forwardDirection
    local yaw = transformComponent.yaw
    local roll = transformComponent.roll

    local velocity = moveSpeed * timestep
    local distance = forwardDirection * velocity
    position = position + distance

    if position.x > -100 and yaw == 255 then
        yaw = 75
    end
    if position.x < -600 and yaw == 75 then
        yaw = 255
    end

    roll = roll + rotationSpeed % 360

    transformComponent.position = position
    transformComponent.forwardDirection = forwardDirection
    transformComponent.yaw = yaw
    transformComponent.roll = roll

    Entity:setTransformComponent(entity, transformComponent)
end
