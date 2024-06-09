function PitchAndRoll.onUpdate(entity, timestep)
    local transformComponent = Entity:getTransformComponent(entity)

    transformComponent.pitch = transformComponent.pitch + 1
    if transformComponent.pitch > 360 then
        transformComponent.pitch = 0
    end
    transformComponent.roll = transformComponent.roll + 1
    if transformComponent.roll > 360 then
        transformComponent.roll = 0
    end

    Entity:setTransformComponent(entity, transformComponent)
end
