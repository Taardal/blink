require("utils")

local moveSpeed = 200

function LinePatrol.onUpdate(entity, timestep)
    local transformComponent = Entity:getTransformComponent(entity)

    local position = transformComponent.position
    local forwardDirection = transformComponent.forwardDirection
    local yaw = transformComponent.yaw

    local velocity = moveSpeed * timestep
    local distance = forwardDirection * velocity
    position = position + distance

    if position.x > 0 and yaw == 270 then
        yaw = 90
    end
    if position.x < -1000 and yaw == 90 then
        yaw = 270
    end

    transformComponent.position = position
    transformComponent.forwardDirection = forwardDirection
    transformComponent.yaw = yaw

    Entity:setTransformComponent(entity, transformComponent)
end
