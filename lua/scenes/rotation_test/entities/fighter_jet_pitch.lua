function FighterJetPitch.onUpdate(entity, timestep)
    local transformComponent = Entity:getTransformComponent(entity)
    transformComponent.pitch = transformComponent.pitch + 1
    if transformComponent.pitch > 360 then
        transformComponent.pitch = 0
    end
    Entity:setTransformComponent(entity, transformComponent)
end
