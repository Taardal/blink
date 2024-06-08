function Roll.onUpdate(entity, timestep)
    local transformComponent = Entity:getTransformComponent(entity)
    transformComponent.roll = transformComponent.roll + 1
    if transformComponent.roll > 360 then
        transformComponent.roll = 0
    end
    Entity:setTransformComponent(entity, transformComponent)
end
