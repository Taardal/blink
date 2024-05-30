require("utils")

local moveSpeed = 200

function AiPatrolLine.onUpdate(entityId, timestep)
    local transformComponent = Entity:getTransformComponent(entityId)

    local position = transformComponent.position
    local forwardDirection = transformComponent.forwardDirection
    local yaw = transformComponent.yaw

    local velocity = moveSpeed * timestep
    position = glm.add(position, glm.multiply(forwardDirection, velocity))

    if position.x < -950 then
        yaw = 0
    end
    if position.x > -50 then
        yaw = 180
    end

    transformComponent.position = position
    transformComponent.forwardDirection = forwardDirection
    transformComponent.yaw = yaw

    Entity:setTransformComponent(entityId, transformComponent)
end
