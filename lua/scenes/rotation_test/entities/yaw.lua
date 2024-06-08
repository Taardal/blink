function Yaw.onUpdate(entity, timestep)
    local transformComponent = Entity:getTransformComponent(entity)
    transformComponent.yaw = transformComponent.yaw + 1
    if transformComponent.yaw > 360 then
        transformComponent.yaw = 0
    end
    Entity:setTransformComponent(entity, transformComponent)
end