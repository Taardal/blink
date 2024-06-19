function Scene.onConfigureCamera()
    SceneCamera:setPosition(glm.vec3(1.29, 1.34, 2.11))
    SceneCamera:setYaw(30)
    SceneCamera:setPitch(-26)
    SceneCamera:setMoveSpeed(1)
    SceneCamera:setRotationSpeed(0.5)
    SceneCamera:setFarClip(10000)
    SceneCamera:setNearClip(0.1)
end

function Scene.onCreateEntities()
    local vikingRoom = Entity:create()
    Entity:setTagComponent(vikingRoom, {
        tag = "Viking Room"
    })
    Entity:setMeshComponent(vikingRoom, {
        modelPath = "models/viking_room/viking_room.obj",
        textureAtlasPath = "models/viking_room/viking_room.png",
    })
    Entity:setTransformComponent(vikingRoom, {
        yaw = 180,
    })
end
