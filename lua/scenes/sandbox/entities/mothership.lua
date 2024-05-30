local incrementY = true
local maxY = 280
local minY = 260

local thrust = 000.5
local moveSpeed = 000.5
local maxMoveSpeed = 0.05

function Mothership.onUpdate(entity, timestep)
    local transformComponent = Entity:getTransformComponent(entity)

    local position = transformComponent.position
    local forwardDirection = transformComponent.forwardDirection

    if (incrementY and position.y > maxY) then
        incrementY = false
    end
    if (not incrementY and position.y < minY) then
        incrementY = true
    end
    if incrementY then
        moveSpeed = math.min(moveSpeed + thrust, maxMoveSpeed)
    else
        moveSpeed = math.max(moveSpeed - thrust, -maxMoveSpeed)
    end
    position.y = position.y + moveSpeed

    transformComponent.position = position
    transformComponent.forwardDirection = forwardDirection

    Entity:setTransformComponent(entity, transformComponent)
end
