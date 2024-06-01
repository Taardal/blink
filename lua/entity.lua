function createEntityWithDefaultComponents()
    local entity = Entity:create()
    Entity:setTagComponent(entity, {
        tag = "Entity " .. entity,
    })
    Entity:setTransformComponent(entity, {
        translation = glm.mat4(1.0),
        rotation = glm.mat4(1.0),
        scale = glm.mat4(1.0),
        position = glm.vec3(0.0, 0.0, 0.0),
        size = glm.vec3(1.0, 1.0, 1.0),
        forwardDirection = WORLD_FORWARD_DIRECTION,
        rightDirection = WORLD_RIGHT_DIRECTION,
        upDirection = WORLD_UP_DIRECTION,
        worldUpDirection = WORLD_UP_DIRECTION,
        orientation = glm.quat(1.0, 0.0, 0.0, 0.0),
        yaw = 0.0,
        pitch = 0.0,
        roll = 0.0,
    })
    return entity
end
