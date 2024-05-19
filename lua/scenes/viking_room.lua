function Scene.onCreate()

    SceneCamera:setFrustum({
        fieldOfView = math.rad(45.0),
        nearClip = 0.1,
        farClip = 10000.0,
    })
    SceneCamera:setMoveSpeed(1.0)
    SceneCamera:setRotationSpeed(1.0)
    SceneCamera:setPosition(glm.vec3(0, 0.5, 3))
    SceneCamera:setWorldUpDirection(glm.vec3(0, 1, 0))
    SceneCamera:setForwardDirection(glm.vec3(0, 0, -1))

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
