function Scene.onConfigureCamera()
    SceneCamera:setPosition(glm.vec3(2, 2, 2))
    SceneCamera:setYaw(229)
    SceneCamera:setPitch(-33)
    SceneCamera:setMoveSpeed(1)
    SceneCamera:setRotationSpeed(0.5)
end

function Scene.onCreateEntities()
    local entityId = Entity:create()

    Entity:setTagComponent(entityId, {
        tag = "Viking Room"
    })
    Entity:setMeshComponent(entityId, {
        modelPath = "models/viking_room/viking_room.obj",
        textureAtlasPath = "models/viking_room/viking_room.png",
    })
    Entity:setTransformComponent(entityId, {
        position = glm.vec3(0, 0, 0),
        yawOffset = 180.0,
        pitchOffset = -90.0,
    })
end
